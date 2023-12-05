import os
import json
import torch
import torch.nn as nn
from torchvision.io import read_image
from torchvision.transforms import Resize
from torch.utils.data import Dataset, DataLoader
import matplotlib.pyplot as plt

class_names = ('big_one', 'big_three', 'big_four', 'big_five', 'big_base',
               'small_two', 'small_three', 'small_four', 'small_five',
               'small_base', 'small_santry', 'small_outpost',
               'no_pattern')

class ArmorPatternDataset(Dataset):
    def __init__(self, dir, transform=None, target_transform=None):
        self.dir = dir
        self.img_labels = []
        for root, _, files in os.walk(dir):
            for file in files:
                if file.endswith('json'):
                    img_name = file.replace('json', 'png')
                    class_name = json.load(open(os.path.join(root, file)))['labels'][0]['name']
                    class_id = class_names.index(class_name)
                    self.img_labels.append((img_name, class_id))

        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.img_labels)

    def __getitem__(self, idx):
        img_path = os.path.join(self.dir, self.img_labels[idx][0])
        image = read_image(img_path)
        image.to(torch.float32)
        image = image / 255.0
        label = self.img_labels[idx][1]
        if self.transform:
            image = self.transform(image)
        if self.target_transform:
            label = self.target_transform(label)
        return image, label


class LeNet(nn.Module):
    def __init__(self, num_classes):
        super(LeNet, self).__init__()  # 继承nn.Module中的变量和函数
        self.features = nn.Sequential(  # 序列容器，神经网络相邻的两个层必须前后匹配
            # 第一层网络
            # 初始 50x50x1
            nn.Conv2d(1, 6, kernel_size=5),  # 卷积层 46x46x6
            nn.BatchNorm2d(6),  # 卷积后归一化处理，防止部分数直过大
            nn.ReLU(),  # f(x)=max(0,x)
            nn.MaxPool2d(kernel_size=2, stride=2),  # 最大池化层  23x23x6

            # 第二层网络
            nn.Conv2d(6, 16, kernel_size=5),  # 19x19x16
            nn.BatchNorm2d(16),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2),  #     9x9x16
        )                                           #       |
        self.classifier = nn.Sequential(            #       |
            # 线性全连接层                            #       |
            nn.Linear(16 * 9 * 9, 120), # 1x120     #       |
            nn.ReLU(),                              #       |
            nn.Dropout(0.25),                       #       |
            nn.Linear(120, 84),         # 1x84      #       |
            nn.ReLU(),                              #       |
            nn.Dropout(0.25),                       #       |
            nn.Linear(84, num_classes), # 1x13      #       |
        )                                           #       |
                                                    #       |
    def forward(self, x):                           #       |
        x = self.features(x)  # 输出16*9*9的特征图            |
        x = x.reshape(x.size(0), -1)  # 展平为(1,1296) <—-—-—-
        logits = self.classifier(x)  # 输出10个结果
        return logits


if __name__ == '__main__':
    # 硬件
    device = torch.device('cpu')

    # 超参数
    batch_size = 16
    learning_rate = 0.005
    num_epochs = 14

    # 训练集
    train_dataset = ArmorPatternDataset('assets/armor_pattern_dataset/train', Resize(50))
    test_dataset = ArmorPatternDataset('assets/armor_pattern_dataset/test', Resize(50))
    train_loader = DataLoader(dataset=train_dataset, batch_size=batch_size, shuffle=True)
    test_loader = DataLoader(dataset=test_dataset, batch_size=batch_size, shuffle=True)

    model = LeNet(len(class_names)).to(device)
    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate, weight_decay=0.005, momentum=0.9)  # 构建一个优化器

    for epoch in range(num_epochs):
        # train
        model.train()
        for images, labels in train_loader:
            images = images.to(device)  # 将数据放入CPU
            labels = labels.to(device)

            outputs = model(images)  # 输出10分类结果
            loss = criterion(outputs, labels)  # 前向传播计算误差

            optimizer.zero_grad()  # 梯度置零
            loss.backward()  # 反向传播更新权重
            optimizer.step()  # 更新参数

        # test
        model.eval()
        correct = 0
        total = 0
        with torch.no_grad():  # ？？
            for images, labels in test_loader:
                images = images.to(device)
                labels = labels.to(device)

                outputs = model(images)
                _, predicted = torch.max(outputs.data, 1)

                total += labels.size(0)
                correct += (predicted == labels).sum().item()

        print(f'Epoch [{epoch+1}/{num_epochs}], Loss: {loss.item():.4f}, Accuracy: {(100 * correct / total):.4f}')

    # 导出
    export_path = 'assets/model.onnx'
    x = torch.randn(1, 1, 50, 50)
    x = x.to(device)
    torch.onnx.export(model,
                      x,
                      export_path,
                      input_names=['input'],
                      output_names=['output'],
                      dynamic_axes={'input': {0: 'batch_size'},
                                    'output': {0: 'batch_size'}})
    print(f'Model is exported at {export_path}')

    # show result
    model.eval()
    figure = plt.figure(figsize=(8, 8))
    cols, rows = 4, 4
    for i in range(1, cols * rows + 1):
        sample_idx = torch.randint(len(test_dataset), size=(1,)).item()
        img, label = test_dataset[sample_idx]
        img = img.to(device)

        with torch.no_grad():
            outputs = model(img.unsqueeze(0))
            probs = torch.nn.functional.softmax(outputs, dim=1)
            max_probability, predicted = torch.max(probs, dim=1)
            confidence = max_probability.item()
            class_id = predicted.item()

        figure.add_subplot(rows, cols, i)
        plt.title(f'{class_names[class_id]} {confidence:.2f} {"T" if label==class_id else "F"}')
        plt.axis("off")
        img = img.to('cpu')
        plt.imshow(img.squeeze(), cmap="gray")
    plt.show()

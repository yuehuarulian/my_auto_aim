k1,k2,b=0,0,0
iteration=10
lr=0.5
labels=[[0,0],[0,1]]
def quan(x,y): # he
    return k1*x+k2*y+b

def ji(quan): # jihuo
    if quan>0:
        return quan
    else:
        return 0

for i in range(iteration):
    for x in range(2):
        for y in range(2):
            q=quan(x,y)
            dji=1 if q>=0 else 0 # ji daoshu
            d_k1=(ji(q)-labels[x][y])*dji*x*lr
            d_k2=(ji(q)-labels[x][y])*dji*y*lr
            d_b=(ji(q)-labels[x][y])*dji*lr

            k1-=d_k1
            k2-=d_k2
            b-=d_b

            print(f'{i=} {x=} {k1=} {k2=} {b=}')
    print()
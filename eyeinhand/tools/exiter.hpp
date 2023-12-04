#ifndef TOOLS__EXITER_HPP
#define TOOLS__EXITER_HPP

#include <signal.h>

#include <stdexcept>

namespace tools
{
bool exit_ = false;
bool exiter_inited_ = false;

class Exiter
{
public:
  Exiter()
  {
    if (exiter_inited_) throw std::runtime_error("Multiple Exiter instances!");
    signal(SIGINT, &callback);
    exiter_inited_ = true;
  }

  bool exit() const { return exit_; }

private:
  static void callback(int) { exit_ = true; }
};

}  // namespace tools

#endif  // TOOLS__EXITER_HPP
#include "output.h"
#include "common/exception.h"

namespace AV {

Output::Output(const std::string& url)
{
  int result = avio_open2(
          &ctx_,
          url.c_str(),
          AVIO_FLAG_WRITE,
          nullptr/*interrupt_cb*/,
          nullptr/*opts*/);
  if (result < 0) {
    throw Common::Exception("Failed to open IO context");
  }
  ctx_->direct = 1;
}
Output::Output(Output&& other) :
  ctx_(other.ctx_)
{
  other.ctx_ = nullptr;
}
Output::~Output()
{
  avio_closep(&ctx_);
}
Output& Output::operator=(Output&& other)
{
  if (&other == this) {
    return *this;
  }
  avio_closep(&ctx_);
  std::swap(ctx_, other.ctx_);
  return *this;
}

} // namespace AV


#pragma once

#include <stdexcept>
#include <string>

namespace network {
class QueryPerformer {
 public:
  struct errors {
    struct General : public std::exception {};
    struct CouldNotConnectToServer : public General {};
  };
  virtual ~QueryPerformer() = default;
  virtual std::string PerformGetRequest(const std::string& url,
                                        const std::string& parameters) = 0;
  virtual std::string PerformPostRequest(const std::string& url,
                                         const std::string& data,
                                         const std::string& data_format) = 0;
};

namespace data_format {
constexpr inline char kUrlEncoded[] = "application/x-www-form-urlencoded";
}
}  // namespace network

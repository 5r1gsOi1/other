
#pragma once

#include <string>
#include <vector>
#include <stdexcept>

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
                                         const std::string& data) = 0;
};
}  // namespace network

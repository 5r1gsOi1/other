
#pragma once

#include "query.h"

#include <memory>

namespace network {
class QueryViaCurl : public QueryPerformer {
 public:
  QueryViaCurl();
  virtual ~QueryViaCurl() override;

  virtual std::string PerformGetRequest(
      const std::string& url, const std::string& parameters_string) override;
  virtual std::string PerformPostRequest(
      const std::string& url, const std::string& data,
      const std::string& data_format) override;

 protected:
  class QueryViaCurlImpl;
  std::unique_ptr<QueryViaCurlImpl> pimpl_;
};
}  // namespace network

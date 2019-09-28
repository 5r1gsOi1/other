
#pragma once

#include "network/query.h"
#include "wiki/query.h"

#include <memory>
#include <string>

namespace wiki {

class QueryViaNetwork : public wiki::QueryPerformer {
 public:
  struct errors {
    using Technical = QueryPerformer::errors::Technical;
    using Connection = QueryPerformer::errors::Connection;
    using Response = QueryPerformer::errors::Response;
    using Login = QueryPerformer::errors::Login;

    struct ResponseIsNotValid : public Response {
      const char* what() const noexcept override {
        return "ResponseIsNotValid";
      }
    };
    struct ResponseTitleMismatsh : public Response {
      const char* what() const noexcept override {
        return "ResponseTitleMismatsh";
      }
    };
    struct UnexpectedContent : public Response {
      const char* what() const noexcept override { return "UnexpectedContent"; }
    };
    struct ContentSizeMismatsh : public Response {
      const char* what() const noexcept override {
        return "ContentSizeMismatsh";
      }
    };
    struct LoginFailed : public Login {};
    struct LoginTokenFailed : public Login {};
  };

  using RequestParameter = std::pair<std::string, std::string>;
  using RequestParameters = std::map<std::string, std::string>;

  struct ServerParameters {
    const std::string api_address;
    const std::string pages_address;
  };

  QueryViaNetwork(const QueryViaNetwork::ServerParameters& parameters,
                  std::unique_ptr<network::QueryPerformer>&& network);
  virtual ~QueryViaNetwork();

  virtual Page GetPageLastRevision(const std::string& page_name);
  virtual Page GetPageRevision(const std::string& page_name,
                               const std::string& timestamp);
  virtual Page GetPageRevisions(
      const std::string& page_title,
      const std::string& start_timestamp = std::string{},
      const std::string& end_timestamp = std::string{},
      const bool content_should_be_included = true);
  virtual Pages GetPagesInCategory(const std::string& category_name);

 protected:
  virtual void Login(const Credentials& credentials);
  virtual void Logout();
  bool IsLoggedIn() const { return logged_in_; }

  std::string PerformPostRequest(
      const std::string& url,
      const wiki::QueryViaNetwork::RequestParameters& parameters);

  std::string PerformGetRequest(
      const std::string& url,
      const wiki::QueryViaNetwork::RequestParameters& parameters);

  virtual std::string GetLoginToken();

  ServerParameters parameters_;

  std::string lguserid_{};
  std::string lgusername_{};

  std::unique_ptr<network::QueryPerformer> network_;

  bool logged_in_{false};
};

}  // namespace wiki

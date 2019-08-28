
#include "query_via_curl.h"

#include <curl/curl.h>
#include <iostream>

namespace network {
class QueryViaCurl::QueryViaCurlImpl {
 public:
  QueryViaCurlImpl();
  ~QueryViaCurlImpl();
  std::string PerformGetRequest(const std::string& url,
                                const std::string& parameters);
  std::string PerformPostRequest(const std::string& url,
                                 const std::string& data);

 private:
  CURL* curl_{};
  std::string cookies_{};
};

QueryViaCurl::QueryViaCurl() : pimpl_{std::make_unique<QueryViaCurlImpl>()} {}

QueryViaCurl::~QueryViaCurl() = default;

std::string QueryViaCurl::PerformGetRequest(const std::string& url,
                                            const std::string& parameters) {
  return pimpl_->PerformGetRequest(url, parameters);
}

std::string QueryViaCurl::PerformPostRequest(const std::string& url,
                                             const std::string& data) {
  return pimpl_->PerformPostRequest(url, data);
}

size_t CurlWrite_CallbackFunc_StdString(void* contents, size_t size,
                                        size_t nmemb, std::string* s) {
  size_t newLength = size * nmemb;
  size_t oldLength = s->size();
  try {
    s->resize(oldLength + newLength);
  } catch (std::bad_alloc& e) {
    // handle memory problem
    return 0;
  }

  std::copy((char*)contents, (char*)contents + newLength,
            s->begin() + oldLength);
  return size * nmemb;
}

QueryViaCurl::QueryViaCurlImpl::QueryViaCurlImpl() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl_ = curl_easy_init();
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION,
                   CurlWrite_CallbackFunc_StdString);
}

QueryViaCurl::QueryViaCurlImpl::~QueryViaCurlImpl() {
  curl_easy_cleanup(curl_);
}

std::string QueryViaCurl::QueryViaCurlImpl::PerformGetRequest(
    const std::string& url, const std::string& parameters) {
  std::string response;
  std::string url_with_parameters = url + "?" + parameters;
  if (curl_) {
    curl_easy_setopt(curl_, CURLOPT_URL, url_with_parameters.c_str());

    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https

    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    char header[] = "Content-Type: application/octet-stream";
    curl_slist headers{header, nullptr};
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, &headers);

    curl_easy_setopt(curl_, CURLOPT_POST, 0L);
    curl_easy_setopt(curl_, CURLOPT_COOKIEFILE, "");

    curl_easy_setopt(curl_, CURLOPT_COOKIE, cookies_.c_str());
    // curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
    // curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L); //remove this to disable

    CURLcode result = curl_easy_perform(curl_);
    if (result != CURLE_OK) {
      // std::cout << "ERROR: " << curl_easy_strerror(result) << std::endl;
    }
  }
  return response;
}

std::string QueryViaCurl::QueryViaCurlImpl::PerformPostRequest(
    const std::string& url, const std::string& data) {
  std::string response;
  if (curl_) {
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION,
                     CurlWrite_CallbackFunc_StdString);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    char header[] = "Content-Type: application/octet-stream";
    curl_slist headers{header, nullptr};
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, &headers);

    curl_easy_setopt(curl_, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, data.size());
    curl_easy_setopt(curl_, CURLOPT_COOKIEFILE, "");

    curl_easy_setopt(curl_, CURLOPT_COOKIE, cookies_.c_str());
    // curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
    // curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L); //remove this to disable

    CURLcode result = curl_easy_perform(curl_);
    if (result != CURLE_OK) {
      std::cout << "ERROR: " << curl_easy_strerror(result) << std::endl;
      throw errors::CouldNotConnectToServer();
    }
    // std::cout << "string = " << result << std::endl;
    // print_cookies(curl_);
  }
  return response;
}

}  // namespace network

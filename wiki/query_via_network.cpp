
#include "query_via_network.h"

#include "third/jzon/jzon.h"

#include <iomanip>
#include <iostream>

// https://en.wikipedia.org/w/api.php ?action=query &titles=Personnel_(film)
// &prop=revisions &rvprop=content|timestamp|user|userid|size &formatversion=2
// &format=json

// api = https://en.wikipedia.org/w/api.php
// pages = https://en.wikipedia.org/wiki/

namespace wiki {
std::string EncodeRequestParametersAsJson(
    const QueryViaNetwork::RequestParameters& parameters) {
  jzon::Node node = jzon::object();
  for (auto& i : parameters) {
    node.add(UrlEncode(i.first), UrlEncode(i.second));
  }
  jzon::Writer writer{};
  std::string result{};
  writer.writeString(node, result);
  return result;
}

std::string EncodeRequestParametersAsString(
    const QueryViaNetwork::RequestParameters& parameters) {
  std::string linearized_parameters;
  for (auto& i : parameters) {
    linearized_parameters +=
        UrlEncode(i.first) + "=" + UrlEncode(i.second) + "&";
  }
  linearized_parameters.erase(linearized_parameters.size() - 1);
  return linearized_parameters;
}

std::string EncodeParametersForPost(
    const QueryViaNetwork::RequestParameters& parameters) {
  return EncodeRequestParametersAsString(parameters);
}

std::string EncodeParametersForGet(
    const QueryViaNetwork::RequestParameters& parameters) {
  return EncodeRequestParametersAsString(parameters);
}

QueryViaNetwork::QueryViaNetwork(
    const QueryViaNetwork::ServerParameters& parameters,
    std::unique_ptr<network::QueryPerformer>&& network)
    : parameters_{parameters}, network_{std::move(network)} {}

QueryViaNetwork::~QueryViaNetwork() = default;

QueryViaNetwork::RequestParameters CreateGetPagesInCategoryParameters(
    const std::string& category_name) {
  QueryViaNetwork::RequestParameters parameters{{{"action", "query"},
                                                 {"list", "categorymembers"},
                                                 {"cmtitle", category_name},
                                                 {"cmlimit", "500"},
                                                 {"format", "json"},
                                                 {"formatversion", "2"}}};
  return parameters;
}

QueryViaNetwork::RequestParameters CreateGetRevisionsParameters(
    const std::string& page_title, const std::string& start_timestamp,
    const std::string& end_timestamp, const bool content_should_be_included) {
  QueryViaNetwork::RequestParameters parameters{
      {{"action", "query"},
       {"titles", page_title},
       {"prop", "revisions"},
       {"rvslots", "main"},
       {"rvdir", "newer"},
       {"rvprop", std::string{content_should_be_included ? "content|" : ""} +
                      "timestamp|user|userid|size"},
       {"rvlimit", "max"},
       {"formatversion", "2"},
       {"format", "json"}}};
  if (not start_timestamp.empty()) {
    parameters.insert(
        QueryViaNetwork::RequestParameter{"rvstart", start_timestamp});
  }
  if (not end_timestamp.empty()) {
    parameters.insert(
        QueryViaNetwork::RequestParameter{"rvend", end_timestamp});
  }
  return parameters;
}

std::string QueryViaNetwork::PerformPostRequest(
    const std::string& url,
    const QueryViaNetwork::RequestParameters& parameters) {
  return network_->PerformPostRequest(url, EncodeParametersForPost(parameters),
                                      network::data_format::kUrlEncoded);
}

std::string QueryViaNetwork::PerformGetRequest(
    const std::string& url,
    const QueryViaNetwork::RequestParameters& parameters) {
  return network_->PerformGetRequest(url, EncodeParametersForGet(parameters));
}

bool RequestIsComplete(const jzon::Node& node) {
  return node.get("batchcomplete").toBool();
}

Page DecodeJsonResult(const std::string& page_title, const std::string& json) {
  jzon::Parser parser{};
  jzon::Node node{parser.parseString(json)};
  if (not node.isValid() or not RequestIsComplete(node)) {
    std::cerr << "throwing errors::RequestNotValid()" << std::endl
              << json << std::endl;
    throw QueryViaNetwork::errors::ResponseIsNotValid();
  }
  auto json_page = node.get("query").get("pages").get(0);
  if (not json_page.isValid() or not json_page.get("title").isValid() or
      json_page.get("title").toString() != page_title) {
    throw QueryViaNetwork::errors::ResponseTitleMismatsh();
  }
  auto last_revision = json_page.get("revisions").get(0);
  if (not last_revision.isValid() or not last_revision.get("user").isValid() or
      not last_revision.get("user").isValid() or
      not last_revision.get("userid").isValid() or
      not last_revision.get("timestamp").isValid() or
      not last_revision.get("size").isValid() or
      not last_revision.get("contentformat").isValid() or
      not last_revision.get("contentmodel").isValid() or
      not last_revision.get("content").isValid() or
      last_revision.get("contentformat").toString() != "text/x-wiki" or
      last_revision.get("contentmodel").toString() != "wikitext") {
    throw QueryViaNetwork::errors::UnexpectedContent();
  }
  auto unescaped_content =
      UnescapeString(last_revision.get("content").toString());
  auto content_size = last_revision.get("size").toInt();
  if (unescaped_content.size() != static_cast<unsigned>(content_size)) {
    // TODO: try to decode this:
    // https://ru.wikipedia.org/w/index.php?oldid=94475349
    std::cerr << "content size mismatch: unescaped_content.size() = "
              << unescaped_content.size()
              << ", static_cast<unsigned>(content_size) = "
              << static_cast<unsigned>(content_size) << std::endl;
    auto content{last_revision.get("content").toString()};
    std::cout << unescaped_content << std::endl;
    throw QueryViaNetwork::errors::ContentSizeMismatsh();
  }
  Page page;
  page.title = page_title;
  // pageid
  page.revisions.insert(std::pair<std::string, Page::Revision>{
      last_revision.get("timestamp").toString(),
      {{last_revision.get("timestamp").toString(),
        last_revision.get("user").toString(),
        last_revision.get("userid").toInt(), content_size},
       unescaped_content}});
  return page;
}

Page QueryViaNetwork::GetPageLastRevision(const std::string& page_title) {
  RequestParameters parameters{
      {"action", "query"},    {"titles", page_title},
      {"prop", "revisions"},  {"rvprop", "content|timestamp|user|userid|size"},
      {"formatversion", "2"}, {"format", "json"}};
  auto server_response =
      PerformPostRequest(parameters_.api_address, parameters);
  return DecodeJsonResult(page_title, server_response);
}

Page QueryViaNetwork::GetPageRevision(const std::string& page_name,
                                      const std::string& timestamp) {
  // TODO
}

void AddRevisionsFromQueryResponseToPage(const jzon::Node& response,
                                         wiki::Page& page) {
  const auto& page0 = response.get("query").get("pages").get(0);
  if (page.title.empty()) {
    page.title = page0.get("title").toString();
    page.id = page0.get("pageid").toInt();
  }
  const jzon::Node& revisions = page0.get("revisions");
  if (revisions.isArray()) {
    for (size_t i = 0; i < revisions.getCount(); ++i) {
      if (not(revisions.get(i).get("user").isString() and
              revisions.get(i).get("userid").isNumber() and
              revisions.get(i).get("timestamp").isString() and
              revisions.get(i).get("size").isNumber() and
              revisions.get(i).get("slots").isObject() and
              revisions.get(i).get("slots").get("main").isObject())) {
        /*
        std::cout << revisions.get(i).get("slots").isObject() << std::endl
                  << revisions.get(i).get("slots").get("main").isValid()
                  << std::endl;

                jzon::Writer writer;
                std::string string;
                writer.writeString(revisions.get(i).get("slots").get("main"),
        string); std::cout << std::endl << std::endl << string << std::endl <<
           std::endl;

                std::cout << "i = " << i << std::endl;
                std::cout << revisions.get(i).has("timestamp") << std::endl;

                std::cout << "====\n";

                std::cout << revisions.get(i).get("user").isString() <<
           std::endl;
                std::cout << revisions.get(i).get("userid").isNumber() <<
           std::endl;
                std::cout << revisions.get(i).get("timestamp").isString() <<
           std::endl;
                std::cout << revisions.get(i).get("size").isNumber() <<
           std::endl;
                std::cout << revisions.get(i).get("content").isString() <<
           std::endl;

                std::cerr << "AddRevisionsFromQueryResponseToPage" <<
           std::endl;///*/
        std::cerr << "throwing wiki::errors::RequestNotValid() #1" << std::endl;
        throw QueryViaNetwork::errors::ResponseIsNotValid{};
      }
      auto revision = wiki::Page::Revision{
          wiki::Page::Revision::Header{
              revisions.get(i).get("timestamp").toString(),
              revisions.get(i).get("user").toString(),
              revisions.get(i).get("userid").toInt(),
              revisions.get(i).get("size").toInt()},
          revisions.get(i).get("slots").get("main").get("content").toString()};
      page.revisions.insert(std::pair<std::string, wiki::Page::Revision>(
          revisions.get(i).get("timestamp").toString(), revision));
    }
  } else {
    std::cout << "\tNo data from server\n" << std::flush;
    // throw wiki::errors::RequestNotValid();
  }
  std::cout << "\trevisions = " << page.revisions.size() << ", ";
  if (not page.revisions.empty()) {
    std::cout << "\t" << page.revisions.begin()->first << " --- "
              << (--page.revisions.end())->first;
  }
  std::cout << std::endl;
}

Page QueryViaNetwork::GetPageRevisions(const std::string& page_title,
                                       const std::string& start_timestamp,
                                       const std::string& end_timestamp,
                                       const bool content_should_be_included) {
  Page page;
  auto parameters = CreateGetRevisionsParameters(page_title, start_timestamp,
                                                 end_timestamp,
                                                 content_should_be_included),
       parameters_copy = parameters;
  std::cout << EncodeParametersForPost(parameters) << std::endl;
DoWhileContinueMarkerIsFoundInResponse:
  auto response_string =
      PerformPostRequest(parameters_.api_address, parameters_copy);
  // std::cout << response_string << std::endl;
  jzon::Node response_json{jzon::Parser{}.parseString(response_string)};
  if (not response_json.isValid()) {
    std::cout << __func__ << std::endl << response_string << std::endl;
    throw errors::ResponseIsNotValid();
  }
  try {
    AddRevisionsFromQueryResponseToPage(response_json, page);
  } catch (const errors::ResponseIsNotValid&) {
    throw;
  }
  jzon::Node continue_marker = response_json.get("continue");
  if (continue_marker.isValid()) {
    parameters_copy = parameters;
    for (auto& i : continue_marker) {
      parameters_copy.insert(RequestParameter(i.first, i.second.toString()));
    }
    goto DoWhileContinueMarkerIsFoundInResponse;
  }
  return page;
}

void AddPagesFromQueryResponseToPagesList(const jzon::Node& response,
                                          wiki::Pages& pages) {
  const auto& response_pages = response.get("query").get("categorymembers");
  if (response_pages.isArray()) {
    for (size_t i = 0; i < response_pages.getCount(); ++i) {
      if (not(response_pages.get(i).get("ns").isNumber() or
              response_pages.get(i).get("title").isString() or
              response_pages.get(i).get("pageid").isNumber())) {
        std::cerr << "throwing wiki::errors::RequestNotValid() #2" << std::endl;
        throw QueryViaNetwork::errors::ResponseIsNotValid{};
      }
      auto page = wiki::Page{response_pages.get(i).get("title").toString(),
                             response_pages.get(i).get("pageid").toInt(),
                             {}};
      pages.push_back(page);
    }
  } else {
    std::cout << "\tNo data from server\n" << std::flush;
    // throw wiki::errors::RequestNotValid();
  }
  std::cout << std::endl;
}

Pages QueryViaNetwork::GetPagesInCategory(const std::string& category_name) {
  Pages pages;
  auto parameters = CreateGetPagesInCategoryParameters(category_name),
       parameters_copy = parameters;
DoWhileContinueMarkerIsFoundInResponse:
  auto response_string =
      PerformPostRequest(parameters_.api_address, parameters_copy);
  jzon::Node response_json{jzon::Parser{}.parseString(response_string)};
  if (not response_json.isValid()) {
    // std::cout << __func__ << std::endl << response_string << std::endl;
    throw errors::ResponseIsNotValid();
  }
  try {
    AddPagesFromQueryResponseToPagesList(response_json, pages);
  } catch (const errors::ResponseIsNotValid&) {
    throw;
  }
  jzon::Node continue_marker = response_json.get("continue");
  if (continue_marker.isValid()) {
    parameters_copy = parameters;
    for (auto& i : continue_marker) {
      parameters_copy.insert(RequestParameter(i.first, i.second.toString()));
    }
    goto DoWhileContinueMarkerIsFoundInResponse;
  }
  return pages;
}

std::string ExtractLoginTokenFromResponseString(const std::string& string) {
  jzon::Node node = jzon::Parser{}.parseString(string);
  if (node.get("query").get("tokens").get("logintoken").isString()) {
    return node.get("query").get("tokens").get("logintoken").toString();
  } else {
    return std::string{};
  }
}

std::string QueryViaNetwork::GetLoginToken() {
  RequestParameters parameters{{"action", "query"},
                               {"meta", "tokens"},
                               {"type", "login"},
                               {"format", "json"}};
  // std::cout << request << std::endl;
  auto response_string =
      PerformPostRequest(parameters_.api_address, parameters);
  // std::cout << parameters_.api_address << std::endl;
  auto login_token = ExtractLoginTokenFromResponseString(response_string);
  if (login_token.empty()) {
    throw errors::LoginTokenFailed{};
  }
  return login_token;
}

void QueryViaNetwork::Login(const Credentials& c) {
  auto login_token = GetLoginToken();
  // std::cout << login_token << std::endl;
  RequestParameters parameters{{"format", "json"},
                               {"action", "login"},
                               {"lgname", c.username},
                               {"lgpassword", c.password},
                               {"lgtoken", login_token}};
  auto response_string =
      PerformPostRequest(parameters_.api_address, parameters);

  jzon::Node response = jzon::Parser{}.parseString(response_string);

  if (response.get("login").get("result").toString() != "Success") {
    throw QueryViaNetwork::errors::LoginFailed{};
  }
  lguserid_ = response.get("login").get("lguserid").toString();
  lgusername_ = response.get("login").get("lgusername").toString();
  logged_in_ = true;
}

void QueryViaNetwork::Logout() {
  PerformPostRequest(
      parameters_.api_address,
      RequestParameters{{"action", "logout"}, {"format", "json"}});
  logged_in_ = false;
}

}  // namespace wiki

#if 0







  //*/

 static void print_cookies(CURL* curl) {
   CURLcode res;
   struct curl_slist* cookies;
   struct curl_slist* nc;
   int i;

   printf("Cookies, curl knows:\n");
   res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
   if (res != CURLE_OK) {
     fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
             curl_easy_strerror(res));
     exit(1);
   }
   nc = cookies;
   i = 1;
   while (nc) {
     printf("[%d]: %s\n", i, nc->data);
     nc = nc->next;
     i++;
   }
   if (i == 1) {
     printf("(none)\n");
   }
   curl_slist_free_all(cookies);
 }

#endif

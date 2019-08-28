
#include "afi.h"

#include "network/query_via_curl.h"
#include "statistics/statistics.h"
#include "tasks/charts/dibot/dibot_chart.h"
#include "wiki/cache_fs.h"
#include "wiki/parse.h"
#include "wiki/query_via_network.h"

#include <chrono>
#include <regex>
#include <set>

//#include "tasks/charts/detailed/afi_detailed_chart.h"

/*

class WikiServerCacher2 {
 public:
  struct errors {
    struct General : public ::errors::General {};
    struct DownloadFromServerFailed : public General {};
    struct UnexpectedDataFromServer : public General {};
    struct DataIsAbsentOnServer : public General {};
  };

  WikiServerCacher2(std::unique_ptr<CacheType>&& cache,
                    std::unique_ptr<wiki::Server>&& server)
      : cache_(std::move(cache)), server_(std::move(server)) {
    if (not server_ or not cache_) {
      throw ::errors::InvalidArguments{};
    }
  }

  std::string get(const std::string& page_name) {
    // ask cache for the latest revision timestamp
    // get timestamp for current time, compare with latest revision
    // if difference is greater than some constant then load from server
    // otherwise load from cache
    std::string page_text{};
    try {
      page_text = cache_->get(wiki::Timestamp{});
    } catch (const CacheType::errors::NoSuchIdentifier&) {
      try {
        page_text = GetLastRevisionFromServer(page_name);
      } catch (const errors::DownloadFromServerFailed&) {
        throw;
      } catch (const errors::DataIsAbsentOnServer&) {
        cache_->MarkIdentifierAsAbsent();
      }
    } catch (const CacheType::errors::IdentifierIsMarkedAsAbsent&) {
    }

    return page_text;
  }

  wiki::Page get(const std::string& page_name, const Date& date) {
    if (index_) {
    }
  }

  wiki::Page get(const std::string& page_name,
                 const wiki::Timestamp& timestamp) {
    if (server_) {
      return server_->GetPageRevisions(page_name,
                                       static_cast<std::string>(timestamp));
    } else {
      return wiki::Page{};
    }
  }

 protected:
  std::string GetLastRevisionFromServer(const std::string& page_name) {
    try {
      auto page{server_->GetPageLastRevision(page_name)};
      if (0 == page.revisions.size()) {
        throw errors::DataIsAbsentOnServer{};
      } else {
        return page.revisions.begin()->second.content;
      }
    } catch (const wiki::Server::errors::General&) {
      throw errors::DownloadFromServerFailed{};
    }
  }

  std::unique_ptr<CacheType> cache_;
  std::unique_ptr<wiki::Server> server_;
};

// WikiServerCacher;

struct UserMessage {
  std::string name;
  std::string text;
  wiki::Timestamp timestamp;
};

struct Nomination {
  std::string name;
  bool is_closed;
  UserMessage start_user, close_user;
};

struct NominationsDayStatus {};

struct NominationsDay {
  Date date;
  bool is_closed;
  std::vector<Nomination> nominations;
};

/*
std::ostream& operator<<(const NominationsDay& nomination_day,
                         std::ostream& stream) {
  return stream;
}//*/
/*
using NominationsDays = std::vector<NominationsDay>;

template <auto ParseFunction>
Date GetScanStartDateFromServer(WikiServerCacherExtended& server,
                                const std::string& wiki_page_name,
                                const Date& start_date) {
  auto page_text{server.get(wiki_page_name, start_date)};
  std::vector<Date> day_dates{ParseFunction(page_text)};
  return day_dates.front();
}

// static const char kServerPageName[] = "Википедия:К удалению";

template <auto MainPageParseFunction, auto DayPageParseFunction>
NominationsDays GetNominationsDaysFromServer(
    const GeneralParameters& parameters, const std::string& project_page,
    const Date& start_date, const Date& end_date) {
  WikiServerCacherExtended server(parameters);
  NominationsDays result;
  Date scan_start_date{GetScanStartDateFromServer<MainPageParseFunction>(
      server, project_page, --Date{start_date})};
  for (auto date{scan_start_date}; date <= end_date; ++date) {
    // std::cout << "Downloading date " << date << std::endl;
    std::string page_name{project_page + "/" + date.ToStringInRussian()};
    auto page_text{server.get(page_name)};
    NominationsDay nominations_day{DayPageParseFunction(page_text)};
    result.push_back(nominations_day);
  }
  return result;
}

void CheckForDuplicates(const Pages& sorted_pages) {
  auto it = sorted_pages.begin();
  while (it != sorted_pages.end()) {
    it = std::adjacent_find(it, sorted_pages.end(),
                            [](const Page& a, const Page& b) -> bool {
                              return a.second == b.second;
                            });
    if (it != sorted_pages.end()) {
      std::cout << it->first << "  ---  " << (it + 1)->first << "  ---  "
                << "[[" << it->second << "]]\n";
      ++it;
    }
  }
}

template <typename ParseFunctionType>
Pages GetListOfNominatedArticles(const GeneralParameters& parameters,
                                 const std::string& project_page,
                                 const Date& date,
                                 ParseFunctionType& parse_function) {
  WikiServerCacherExtended server_cacher(parameters);
  auto page_text = server_cacher.get(project_page, date);
  return parse_function(date, page_text);
}

Pages CreatePagesFromWikiPages(const wiki::Pages& wiki_pages) {
  Pages pages;
  pages.reserve(wiki_pages.size());
  for (auto p : wiki_pages) {
    pages.push_back(std::make_pair(Date(), p.title));
  }
  return pages;
}
/*
void CheckAfiForDuplicates(const GeneralParameters& parameters,
                           const Date& date) {
  const std::string kProjectPageName = "Википедия:К улучшению";
  WikiServerCacherExtended server_cacher(parameters);
  auto page_text = server_cacher.get(kProjectPageName, date);
  auto pages = ParseImproveMainPage(date, page_text);
  auto ComparePagesOnlyByTitle = [](const Page& a, const Page& b) -> bool {
    return a.second < b.second;
  };
  std::sort(pages.begin(), pages.end(), ComparePagesOnlyByTitle);
  CheckForDuplicates(pages);

  const std::string kCategoryName =
      "Категория:Википедия:Статьи для срочного улучшения";
  auto category_wiki_pages =
      server_cacher.InternalServer().GetPagesInCategory(kCategoryName);
  std::sort(category_wiki_pages.begin(), category_wiki_pages.end(),
            [](const wiki::Page& a, const wiki::Page& b) -> bool {
              return a.title < b.title;
            });
  auto category_pages = CreatePagesFromWikiPages(category_wiki_pages);
  Pages diff1;
  std::set_difference(
      category_pages.begin(), category_pages.end(), pages.begin(),
pages.end(), std::inserter(diff1, diff1.begin()), ComparePagesOnlyByTitle);

  auto ComparePagesOnlyByDate = [](const Page& a, const Page& b) -> bool {
    return a.first < b.first;
  };
  std::sort(diff1.begin(), diff1.end(), ComparePagesOnlyByDate);

  for (const auto& p : diff1) {
    std::cout << p.first << "  ---  [[" << p.second << "]]\n";
  }

  std::cout << "\n\nDIFF 2" << std::endl;
  Pages diff2;
  std::set_difference(pages.begin(), pages.end(), category_pages.begin(),
                      category_pages.end(), std::inserter(diff2,
diff2.begin()), ComparePagesOnlyByTitle); std::sort(diff2.begin(),
diff2.end(), ComparePagesOnlyByDate);

  for (const auto& p : diff2) {
    std::cout << "[[ВП:К улучшению/" << p.first.ToStringInRussian() << "#"
              << p.second << "|" << p.first << "]]  ---  [[" << p.second
              << "]]\n";
  }
}
//*/

auto CreatePageGetter(const GeneralParameters& parameters) {
  wiki::QueryViaNetwork::ServerParameters server_parameters{
      parameters.server.api_address, parameters.server.pages_address};
  auto network_query{std::make_unique<network::QueryViaCurl>()};
  auto query{std::make_unique<wiki::QueryViaNetwork>(server_parameters,
                                                     std::move(network_query))};
  auto cache{std::make_unique<wiki::PageCacheFS>(
      parameters.paths.server_cache_folder)};
  // TODO: make cache more intelligent, for different functions
  auto policy{std::make_unique<wiki::CachePolicyAlwaysCache>()};

  auto page_getter{std::make_unique<wiki::CachedPageGetter>(
      std::move(query), std::move(cache), std::move(policy))};

  return page_getter;
}

std::vector<Date> CreateListOfDays(const Date& start_date,
                                   const Date& end_date) {
  std::vector<Date> result{};
  result.reserve(end_date - start_date + 1);
  for (Date date{start_date}; date != end_date; ++date) {
    result.push_back(date);
  }
  result.push_back(end_date);
  return result;
}

std::vector<Date> CreateDaysFromNominations(
    const stats::Nominations& nominations) {
  std::vector<Date> result;
  result.reserve(nominations.size() + 1);
  for (const auto& n : nominations) {
    result.push_back(n.date);
  }
  result.erase(std::unique(result.begin(), result.end()), result.end());
  return result;
}

static const std::string kAfiPageName{"Википедия:К удалению"};

static const char kAFDParseRegex[] =
    R"(\{\{\s*(У|у){1}даление\s*статей\s*\|\s*([0-9]{4}-[0-9]{1,2}-[0-9]{1,2})\s*\|\s*(.*)\}\}\n)";

// TODO: get rid of regex, they are too slow
stats::Nominations ParseMainPage(const wiki::Page::Revision& revision,
                                 const std::string& regex) {
  stats::Nominations result{};
  std::regex day_regex{
      regex, std::regex_constants::ECMAScript | std::regex_constants::icase};
  auto days_begin = std::sregex_iterator(revision.content.begin(),
                                         revision.content.end(), day_regex);
  auto days_end = std::sregex_iterator();
  Date date{};
  for (auto& it = days_begin; it != days_end; ++it) {
    std::string date_string{(*it)[2].str()};
    date.FromString(date_string);
    // std::cout << (*it)[2].str() << std::endl;
    auto page_names{ParseDayString((*it)[3].str())};
    for (auto& i : page_names) {
      result.push_back(stats::Nomination{date, i});
    }
  }
  return result;
}

std::vector<Date> GetNotClosedDays(const Date& start_date, const Date& end_date,
                                   wiki::PageGetter& page_getter) {
  auto date{start_date};
  auto start_date_revision{
      page_getter.GetLastRevisionForDay(kAfiPageName, --date)};
  if (start_date_revision.has_value()) {
    auto nominations{
        ParseMainPage(start_date_revision.value(), kAFDParseRegex)};
    auto days{CreateDaysFromNominations(nominations)};
    auto days_till_end{CreateListOfDays(start_date, end_date)};
    days.insert(days.begin(), days_till_end.rbegin(), days_till_end.rend());
    return days;
  }
  return std::vector<Date>{};
}

bool StringIsFirstLevelHeader(const std::string& string) {
  auto string_size{string.size()};
  return string[0] == '=' and string[1] == '=' and string[2] != '=' and
         string[string_size - 1] == '=' and string[string_size - 2] == '=';
}

std::size_t GetHeaderLevelOfString(const std::string& string) {
  auto string_size{string.size()};
  std::size_t left{}, right{};
  while (left < string_size and string[left++] == '=') {
  };
  while (string_size + 1 - right > 0 and
         string[string_size - 1 - right++] == '=') {
  };
  return std::min(left - 1, right - 1);
}

std::string ExtractSectionTitle(const std::string& section_name,
                                const std::size_t level) {
  return RemoveRepeatingSpaces(
      section_name.substr(level, section_name.size() - 2 * level));
}

struct PageSection {
  std::string title{};
  std::size_t level{};  // number of sequential '=' chars
  std::string text{};
  std::vector<std::unique_ptr<PageSection>> subsections{};
  PageSection* parent{};
};

PageSection SplitPageToSections(const std::string& page_name,
                                const std::string& page_content) {
  std::stringstream stream{page_content};
  PageSection root{page_name, 0, std::string{}, {}, nullptr};
  PageSection* current_section{&root};
  std::string line{};
  while (std::getline(stream, line)) {
    std::size_t header_level{GetHeaderLevelOfString(line)};
    if (header_level == 0) {
      if (not line.empty()) {
        current_section->text += line + "\n";
      }
    } else {
      PageSection* parent{};
      if (header_level > current_section->level) {
        parent = current_section;
      } else if (header_level <= current_section->level) {
        parent = current_section->parent;
        while (parent and parent->level >= header_level) {
          parent = parent->parent;
        }
      }
      PageSection* new_section{
          new PageSection{ExtractSectionTitle(line, header_level),
                          header_level,
                          std::string{},
                          {},
                          parent}};
      parent->subsections.push_back(std::unique_ptr<PageSection>{new_section});
      current_section = new_section;
    }
  }
  return root;
}

void ParseSectionText(const PageSection& section) {
  std::stringstream stream{section.text};
  std::string line{};
  while (std::getline(stream, line)) {
  }
}

bool TitleBelongsToForAllSection(const std::string& title) {
  const auto first_maker{title.find("о всем")};
  bool contains_link{wiki::parse::StringContainsLink(title)};
  return not contains_link and first_maker != std::string::npos and
         first_maker < 3;
}

bool SectionContainsForAllSubsection(const PageSection& section) {
  if (section.subsections.size() > 0) {
    for (const auto& ss : section.subsections) {
      if (ss and TitleBelongsToForAllSection(ss->title)) {
        return true;
      }
    }
    if (section.subsections.back()) {
      return SectionContainsForAllSubsection(*section.subsections.back());
    }
  }
  return false;
}

enum class SectionType : int {
  kUnknown = 0,
  kSimple = 2,
  kCompound = 4,
};

SectionType DetermineSectionType(const PageSection& section) {
  using namespace wiki::parse;
  std::string title{RemoveStrikeOutTags(section.title)};
  bool title_is_full_link{StringIsFullLink(RemoveStrikeOutTags(title))};
  auto subsections_count{section.subsections.size()};
  bool first_subsection_is_full_link{false};
  if (subsections_count > 0 and section.subsections.front() and
      StringIsFullLink(
          RemoveStrikeOutTags(section.subsections.front()->title))) {
    first_subsection_is_full_link = true;
  }
  bool contains_forall_subsection{SectionContainsForAllSubsection(section)};
  if (title_is_full_link and
      (subsections_count == 0 or (not first_subsection_is_full_link and
                                  not contains_forall_subsection))) {
    return SectionType::kSimple;
  } else if (not title_is_full_link and subsections_count != 0 and
             first_subsection_is_full_link) {
    return SectionType::kCompound;
  } else {
    return SectionType::kUnknown;
  }
}

bool LinkPrefixBelongsToUser(const std::string& prefix) {
  return prefix == "u" or prefix == "U" or prefix == "у" or prefix == "У" or
         prefix == "user" or prefix == "User" or prefix == "Участник" or
         prefix == "участник";
}

bool LinkPrefixBelongsToIP(const std::string& prefix) {
  return prefix == "Special" or prefix == "special";
}

std::optional<std::string> GetLastUserNameFromString(const std::string& text) {
  for (auto end{text.rfind("]]")}, start{text.rfind("[[", end)};
       end != std::string::npos and start != std::string::npos;
       end = text.rfind("]]", end - 1), start = text.rfind("[[", end)) {
    auto full_link{text.substr(start + 2, end - start - 2)};
    if (not full_link.empty()) {
      auto middle{full_link.find("|")};
      if (middle != std::string::npos) {
        auto colon{full_link.find(":")};
        if (colon != std::string::npos and colon < middle) {
          auto link_prefix{full_link.substr(0, colon)};
          if (LinkPrefixBelongsToUser(link_prefix)) {
            auto username{full_link.substr(colon + 1, middle - colon - 1)};
            return RemoveLeadingSpaces(username);
          } else if (LinkPrefixBelongsToIP(link_prefix)) {
            auto slash{full_link.find("/")};
            if (slash != std::string::npos) {
              auto username{full_link.substr(slash + 1, middle - slash - 1)};
              return RemoveLeadingSpaces(username);
            }
          }
        }
      }
    }
  }
  return std::nullopt;
}

std::optional<wiki::parse::UserMessage> GetUserWithLeastTimestamp(
    const std::string& text) {
  std::size_t ts_pos{}, prev_ts_pos{};
  wiki::Timestamp min_ts{};
  auto text_size{text.size()};
  for (std::size_t i{}; i < text_size; ++i) {
    if (std::isdigit(text[i])) {
      auto timestamp{wiki::Timestamp::FromString(text.c_str() + i)};
      if (not timestamp.IsNull() and (min_ts.IsNull() or timestamp < min_ts)) {
        min_ts = timestamp;
        prev_ts_pos = ts_pos;
        ts_pos = i;
      }
    }
  }
  wiki::parse::UserMessage result{};
  if (not min_ts.IsNull()) {
    result.timestamp = static_cast<std::string>(min_ts);
    auto name{GetLastUserNameFromString(
        text.substr(prev_ts_pos, ts_pos - prev_ts_pos))};
    if (name.has_value()) {
      result.name = name.value();
      return result;
    }
  }
  return std::nullopt;
}

std::optional<wiki::parse::UserMessage> GetUserFromAutoSummary(
    const std::string& text) {
  const std::string start_text{"Страница была удалена "},
      timestamp_end_marker{" участником "},
      after_timestamp_text{". Была указана следующая причина: «"},
      message_end_marker{"». Данное сообщение было автоматически"};

  auto timestamp_start{start_text.size()},
      timestamp_end{text.find(timestamp_end_marker, timestamp_start)};
  if (timestamp_end != std::string::npos) {
    auto timestamp{
        text.substr(timestamp_start, timestamp_end - timestamp_start)};
    wiki::Timestamp normal_timestamp{wiki::Timestamp::FromString(timestamp)};
    bool ts_is_valid{not normal_timestamp.IsNull()};
    auto user_name_end{text.find(after_timestamp_text,
                                 timestamp_end + timestamp_end_marker.size())};
    if (ts_is_valid and user_name_end != std::string::npos) {
      auto username_start{timestamp_end + timestamp_end_marker.size()};
      auto username{
          text.substr(username_start, user_name_end - username_start)};
      auto message_end{text.find(message_end_marker,
                                 user_name_end + after_timestamp_text.size())};
      if (message_end != std::string::npos) {
        auto message_start{user_name_end + after_timestamp_text.size()};
        auto message{text.substr(message_start, message_end - message_start)};
        auto pure_username{GetLastUserNameFromString(username)};
        if (pure_username.has_value()) {
          return wiki::parse::UserMessage{
              pure_username.value(), message,
              static_cast<std::string>(normal_timestamp)};
        }
      }
    }
  }
  return std::nullopt;
}

PageSection* FindSubsectionByName(const PageSection& section,
                                  const std::string& name) {
  for (auto& ss : section.subsections) {
    if (ss and ss->title == name) {
      return ss.get();
    }
  }
  return nullptr;
}

PageSection* GetSummarySubsection(const PageSection& section) {
  return FindSubsectionByName(section, "Итог");
}

PageSection* GetAutoSummarySubsection(const PageSection& section) {
  return FindSubsectionByName(section, "Автоитог");
}

wiki::parse::Nomination CreateNominationFromSimpleSection(
    const PageSection& section) {
  wiki::parse::Nomination result{};
  bool title_is_stricken_out{
      wiki::parse::SectionTitleIsStrickenOut(section.title)};
  result.name = wiki::parse::RemoveLinkBrackets(
      wiki::parse::RemoveStrikeOutTags(section.title));
  result.is_closed = title_is_stricken_out;
  auto start_user{GetUserWithLeastTimestamp(section.text)};
  if (start_user.has_value()) {
    result.start_user = start_user.value();
  }

  auto* autosummary{GetAutoSummarySubsection(section)};
  if (autosummary) {
    auto end_user{GetUserFromAutoSummary(autosummary->text)};
    if (end_user.has_value()) {
      result.close_user = end_user.value();
    }
  } else {
    auto* summary{GetSummarySubsection(section)};
    if (summary) {
      auto end_user{GetUserWithLeastTimestamp(summary->text)};
      if (end_user.has_value()) {
        result.close_user = end_user.value();
      }
    }
  }

  return result;
}

std::vector<wiki::parse::Nomination> CreateNominationsFromSimpleSection(
    const PageSection& section) {
  return {CreateNominationFromSimpleSection(section)};
}

std::vector<wiki::parse::Nomination> CreateNominationsFromCompoundSection(
    const PageSection& section) {
  std::vector<wiki::parse::Nomination> result{};
  for (const auto& ss : section.subsections) {
    if (ss and wiki::parse::StringIsFullLink(
                   wiki::parse::RemoveStrikeOutTags(ss->title))) {
      auto n{CreateNominationFromSimpleSection(*ss)};
      result.push_back(n);
    }
  }

  // add for all section

  const auto* summary{GetSummarySubsection(section)};
  if (summary) {
    auto end_user{GetUserWithLeastTimestamp(summary->text)};
    if (end_user.has_value()) {
      for (auto& n : result) {
        n.is_closed = true;
        if (n.close_user.name.empty()) {
          n.close_user = end_user.value();
        }
      }
    }
  }

  return result;
}

std::vector<wiki::parse::Nomination> CreateNominationsFromSection(
    const PageSection& section) {
  switch (DetermineSectionType(section)) {
    case SectionType::kSimple:
      return CreateNominationsFromSimpleSection(section);
    case SectionType::kCompound:
      return CreateNominationsFromCompoundSection(section);
    default:
      return {};
  }
}

wiki::parse::NominationsDay GetNominationsDayFromAfiDayPage(
    const Date& date, const std::string& page_name,
    const wiki::Page::Revision& revision) {
  wiki::parse::NominationsDay day{};
  auto sections{SplitPageToSections(page_name, revision.content)};
  for (const auto& s : sections.subsections) {
    if (s) {
      const auto nominations{CreateNominationsFromSection(*s.get())};
      day.nominations.insert(day.nominations.end(), nominations.begin(),
                             nominations.end());
    }
  }
  return day;
}

Date GetDateFromTimestamp(const wiki::Timestamp& ts) {
  return Date{ts.year, ts.month, ts.day};
}

stats::DetailedRanks CreateRanksForDay(const Date& date,
                                       const std::string& project_name,
                                       const std::string& page_name,
                                       const wiki::Page::Revision& revision) {
  stats::DetailedRanks ranks{};
  auto day{GetNominationsDayFromAfiDayPage(date, page_name, revision)};
  for (const auto& n : day.nominations) {
    if (not n.start_user.name.empty()) {
      Date open_date{GetDateFromTimestamp(n.start_user.timestamp)};
      ranks.ranks.AddUserStat(project_name, n.start_user.name,
                              {{{open_date, n.name}}, {}});
    } else {
      ranks.unparsed.AddUserStat(project_name, std::string{},
                                 {{{date, n.name}}, {}});
    }
    if (n.is_closed and not n.close_user.name.empty()) {
      Date close_date{GetDateFromTimestamp(n.close_user.timestamp)};
      ranks.ranks.AddUserStat(project_name, n.close_user.name,
                              {{}, {{close_date, n.name}}});
    } else {
      ranks.unparsed.AddUserStat(project_name, std::string{},
                                 {{}, {{date, n.name}}});
    }
  }
  return ranks;
}

void SaveRanksToFile(const stats::DetailedRanks& ranks,
                     const std::string& file_name,
                     const std::string& project_name) {
  std::ofstream stream(file_name);
  if (not stream) {
    return;
  }

  stream << "{| class=\"wikitable sortable\"\n"
            "! # !! Пользователь !! Opened !! Closed\n"
            "|-\n";

  int i{};
  for (const auto& r : ranks.ranks.storage_) {
    //|1 || {{u|Джекалоп}} ||2086 || 1983 || 15 ||  || 2 ||  ||  || 86
    //|-

    stream << "|" << i << "||"
           << "{{u|" << r.first << "}} || "
           << r.second.value.at(project_name).opened.size() << " || "
           << r.second.value.at(project_name).closed.size() << "\n|-\n";
    ++i;
  }
}

void SaveDetailedRanksToFile(const stats::DetailedRanks& ranks,
                             const std::string& file_name,
                             const std::string& project_name) {
  std::ofstream stream(file_name);
  if (not stream) {
    return;
  }

  int i{};
  for (const auto& r : ranks.ranks.storage_) {
    stream << i << ": {{u|" << r.first << "}}\n";
    for (const auto& on : r.second.value.at(project_name).opened) {
      stream << "* [[Википедия:К удалению/" << on.date.ToStringInRussian()
             << "#" << on.name << "]] (opened)\n";
    }

    for (const auto& on : r.second.value.at(project_name).closed) {
      stream << "* [[Википедия:К удалению/" << on.date.ToStringInRussian()
             << "#" << on.name << "]] (closed)\n";
    }
    ++i;
  }
}

void SaveUnparsedRanksToFile(const stats::DetailedRanks& ranks,
                             const std::string& file_name,
                             const std::string& project_name) {
  std::ofstream stream(file_name);
  if (not stream) {
    return;
  }

  int i{};
  for (const auto& r : ranks.unparsed.storage_) {
    stream << i << ": ";
    for (const auto& on : r.second.value.at(project_name).opened) {
      stream << "* [[Википедия:К удалению/" << on.date.ToStringInRussian()
             << "#" << on.name << "]] (opened)\n";
    }
    for (const auto& on : r.second.value.at(project_name).closed) {
      stream << "* [[Википедия:К удалению/" << on.date.ToStringInRussian()
             << "#" << on.name << "]] (closed)\n";
    }
    ++i;
  }
}

stats::DetailedRanks CreateRanksForAfd(const GeneralParameters& parameters,
                                       const Date& start_date,
                                       const Date& end_date) {
  // 1. get list of dates for downloading
  //    a. get afd page revision for start_date
  //    b. parse it and get list of dates
  //    c. add to that list all dates started at start_date till the end_date
  //    d. the result represents all days that are not closed till start_date
  //       and therefore people can make summaries there since start_date
  // 2. get list of nominations for every day
  //    a. download afd day
  //    b. parse day to sections
  //    c. for every section get opener and closer names and dates,
  //       probably search for timestamps and get first link to userpage
  //       just before that, note rename messages and try to filter them
  // 3. merge list of nominations
  //    a. for every day, that should be easy, repeats are ignored at this
  //    stage
  // 4. crete a wiki-table
  //    a. just like
  // 5. upload to server
  //    a. that's a new one, should be tested thoroughly, and i think we need
  //    our own wiki server or maybe we can use test.wikipedia.org

  stats::DetailedRanks ranks{};
  auto page_getter{CreatePageGetter(parameters)};
  wiki::parse::NominationsDay all_nominations{};
  std::vector<Date> dates{
      GetNotClosedDays(start_date, end_date, *page_getter.get())};
  for (const auto& date : dates) {
    std::string page_name{kAfiPageName + "/" + date.ToStringInRussian()};
    std::cerr << page_name << std::endl;
    auto revision{page_getter->GetLastRevision(page_name)};
    if (revision.has_value()) {
      ranks += CreateRanksForDay(date, "AFD", page_name, revision.value());
      std::cerr << ranks.ranks.storage_.size() << ", "
                << ranks.unparsed.storage_.size() << std::endl;
    } else {
      std::cerr << "\t\tNO VALUE" << std::endl;
    }
  }

  std::cerr << "saving to file ... " << std::endl;
  SaveRanksToFile(ranks, parameters.paths.out_charts_folder + "/ranks.txt",
                  "AFD");
  SaveDetailedRanksToFile(
      ranks, parameters.paths.out_charts_folder + "/detailed.txt", "AFD");
  SaveUnparsedRanksToFile(
      ranks, parameters.paths.out_charts_folder + "/unparsed.txt", "AFD");

  return ranks;
}

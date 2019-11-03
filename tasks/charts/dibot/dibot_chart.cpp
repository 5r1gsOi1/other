
#include "dibot_chart.h"

#include "chart/date_chart/chart.h"
#include "chart/date_chart/curves.h"
#include "network/query_via_curl.h"
#include "tasks/charts/dibot/dibot_chart.h"
#include "wiki/cache_fs.h"
#include "wiki/query_via_network.h"

using DibotMonthStats = std::map<Date, chart::DateData>;

std::vector<int> ParseSingleStatsLine(const std::string& string) {
  std::vector<int> result;
  std::string delimiter{"||"};
  std::size_t size{delimiter.size()};
  std::string::size_type first{1 + string.find("|")}, second{};
  std::string number_string{};
  while (second = string.find(delimiter, first), second != std::string::npos) {
    number_string = string.substr(first, second - first);
    result.push_back(number_string.empty() ? 0 : std::stoi(number_string));
    first = second + size;
  }
  number_string = string.substr(first);
  result.push_back(number_string.empty() ? 0 : std::stoi(number_string));
  return result;
}

Date GetDateFromString(const std::string& string) {
  std::string::size_type first_dot{string.find(".")};
  std::string::size_type second_dot{string.find(".", first_dot + 1)};
  std::string day{string.substr(1, first_dot - 1)};
  std::string month{string.substr(first_dot + 1, 2)};
  std::string year{string.substr(second_dot + 1, 4)};
  return Date{std::stoi(year), std::stoi(month), std::stoi(day)};
}

DibotMonthStats DibotMonthPageParseFunction(const std::string& page_text) {
  enum ParserState {
    kSearchingForHeaderDelimiter,
    kSearchingForLineDelimiter,
    kSearchingForDate,
    kReadingValues
  };

  DibotMonthStats result{};
  chart::DateData day_stats{};
  Date date{};
  std::istringstream input_stream(page_text);
  std::string line{};
  int value_index{};

  day_stats.values.reserve(17);
  auto state{kSearchingForHeaderDelimiter};
  while (std::getline(input_stream, line)) {
    switch (state) {
      case kSearchingForHeaderDelimiter:
        if (line.find("|- align=\"right\"") != std::string::npos) {
          state = kSearchingForLineDelimiter;
        }
        continue;
      case kSearchingForLineDelimiter:
        if (line.find("|- align=\"right\"") != std::string::npos) {
          state = kSearchingForDate;
        }
        continue;
      case kSearchingForDate:
        date = GetDateFromString(line);
        state = kReadingValues;
        continue;
      case kReadingValues:
        auto single_values{ParseSingleStatsLine(line)};
        day_stats.values.insert(day_stats.values.end(), single_values.begin(),
                                single_values.end());
        if (value_index < 5) {
          ++value_index;
        } else {
          value_index = 0;
          result[date] = day_stats;
          day_stats.values.clear();
          state = kSearchingForLineDelimiter;
        }
        continue;
    }
  }
  return result;
}

std::string GetYearMonthStringFromDate(const Date& date) {
  std::stringstream ss{};
  ss << std::setw(4) << date.year << std::setw(2) << std::setfill('0')
     << date.month;
  return ss.str();
}

class DibotDataProvider::DibotDataProviderImpl {
 public:
  DibotDataProviderImpl(std::unique_ptr<wiki::PageGetter>&& page_getter)
      : getter_{std::move(page_getter)} {}
  ~DibotDataProviderImpl() = default;

  std::optional<chart::DateData> GetDateData(const Date& date) {
    auto year_month{GetYearMonthStringFromDate(date)};
    if (current_year_month_ != year_month) {
      auto page_name{kDibotStatisticsPageName + year_month};
      auto revision{getter_->GetLastRevision(page_name)};
      if (revision.has_value()) {
        month_stats = DibotMonthPageParseFunction(revision.value().content);
      }
      current_year_month_ = year_month;
    }
    if (month_stats.count(date) >= 1) {
      return month_stats[date];
    }

    return std::nullopt;
  }

  std::vector<std::string> GetLabels() {
    return {"КУЛ",      "КУЛ ≥365", "КУЛ ≥183", "КУЛ ≥90", "КУЛ ≥30",
            "КУЛ <30",  "КУЛ дней", "КУ",       "КУ дней", "КПМ",
            "КПМ дней", "КОБ",      "КОБ дней", "КРАЗД",   "КРАЗД дней",
            "ВУС",      "ВУС дней"};
  }

 protected:
  static inline const std::string kDibotStatisticsPageName{
      "Участник:Dibot/TalkStat/"};
  DibotMonthStats month_stats;
  std::string current_year_month_;
  std::unique_ptr<wiki::PageGetter> getter_;
};

DibotDataProvider::DibotDataProvider(
    std::unique_ptr<wiki::PageGetter>&& page_getter)
    : impl_{std::make_unique<DibotDataProviderImpl>(std::move(page_getter))} {}

DibotDataProvider::~DibotDataProvider() = default;

std::optional<chart::DateData> DibotDataProvider::GetDateData(
    const Date& date) {
  return impl_->GetDateData(date);
}

std::vector<std::string> DibotDataProvider::GetLabels() {
  return impl_->GetLabels();
}

class DibotCurvesDataPolicy::DibotCurvesDataPolicyImpl {
 public:
  DibotCurvesDataPolicyImpl(const std::string& absent_dates_file_name,
                            const std::string& excluded_dates_file_name);
  ~DibotCurvesDataPolicyImpl() = default;
  bool CacheDataIsPreferable(const Date& date);
  bool DateMustBeIgnored(const Date& date);
  bool MarkDateAsAbsent(const Date& date);

 protected:
  Date today_;
  FileIndexOneValue<Date> absent_dates_;
  FileIndexOneValue<Date> manually_excluded_dates_;
};

DibotCurvesDataPolicy::DibotCurvesDataPolicyImpl::DibotCurvesDataPolicyImpl(
    const std::string& absent_dates_file_name,
    const std::string& excluded_dates_file_name)
    : today_{Date::Today()},
      absent_dates_(absent_dates_file_name),
      manually_excluded_dates_(excluded_dates_file_name) {}

bool DibotCurvesDataPolicy::DibotCurvesDataPolicyImpl::CacheDataIsPreferable(
    const Date& date) {
  return today_ - date >= 1;
}

bool DibotCurvesDataPolicy::DibotCurvesDataPolicyImpl::DateMustBeIgnored(
    const Date& date) {
  return date >= today_ or absent_dates_.contains(date) or
         manually_excluded_dates_.contains(date);
}

bool DibotCurvesDataPolicy::DibotCurvesDataPolicyImpl::MarkDateAsAbsent(
    const Date& date) {
  return date >= today_ ? false : absent_dates_.add(date);
}

DibotCurvesDataPolicy::DibotCurvesDataPolicy(
    const std::string& absent_dates_file_name,
    const std::string& excluded_dates_file_name)
    : impl_{std::make_unique<DibotCurvesDataPolicyImpl>(
          absent_dates_file_name, excluded_dates_file_name)} {}

DibotCurvesDataPolicy::~DibotCurvesDataPolicy() = default;

bool DibotCurvesDataPolicy::CacheDataIsPreferable(const Date& date) {
  return impl_->CacheDataIsPreferable(date);
}

bool DibotCurvesDataPolicy::DateMustBeIgnored(const Date& date) {
  return impl_->DateMustBeIgnored(date);
}

bool DibotCurvesDataPolicy::MarkDateAsAbsent(const Date& date) {
  return impl_->MarkDateAsAbsent(date);
}

class OutputManager {
 public:
  // std::string GetCache
 protected:
};

auto GetStandardPageGetter(const GeneralParameters& parameters) {
  wiki::QueryViaNetwork::ServerParameters server_parameters{
      parameters.server.api_address, parameters.server.pages_address};
  auto network_query{std::make_unique<network::QueryViaCurl>()};
  auto query{std::make_unique<wiki::QueryViaNetwork>(server_parameters,
                                                     std::move(network_query))};
  auto cache{std::make_unique<wiki::PageCacheFS>(
      parameters.paths.server_cache_folder)};
  auto policy{std::make_unique<wiki::CachePolicyNeverCache>()};

  auto page_getter{std::make_unique<wiki::CachedPageGetter>(
      std::move(query), std::move(cache), std::move(policy))};

  return page_getter;
}

std::unique_ptr<chart::DateCurvesCreator> CreateDibotDateCurvesCreator(
    const GeneralParameters& parameters) {
  auto page_getter{GetStandardPageGetter(parameters)};
  auto data_provider{
      std::make_unique<DibotDataProvider>(std::move(page_getter))};

  std::string dibot_subfolder{parameters.paths.parser_cache_folder + "/dibot"};
  CreateDirectoryIfNotExists(dibot_subfolder);
  auto curves_data_policy{std::make_unique<DibotCurvesDataPolicy>(
      dibot_subfolder + "/_absent.txt", dibot_subfolder + "/_excluded.txt")};

  auto curves_cache{std::make_unique<chart::CurvesDataCacheFS>(dibot_subfolder +
                                                               "/_cache.txt")};

  auto dibot_curves_creator{std::make_unique<chart::DateCurvesCreator>(
      std::move(data_provider), std::move(curves_cache),
      std::move(curves_data_policy))};

  return dibot_curves_creator;
}

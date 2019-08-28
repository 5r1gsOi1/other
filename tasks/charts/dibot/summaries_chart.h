
#ifndef SUMMARIES_CHART_H_
#define SUMMARIES_CHART_H_

#include "statistics/statistics.h"
//#include "tasks/charts/detailed/detailed_chart.h"

//using SummariesDayStats = DetailedDayStats;

void RemoveDuplicates(stats::Nominations& pages);
/*
// abridged
template <auto MainPageParserFunction>
class SummariesMainPageCacher {
 public:
  SummariesMainPageCacher(const std::string& wiki_page_name,
                          const std::string& project_subfolder_name,
                          const GeneralParameters& parameters,
                          const bool ignore_cache = false)
      : cache_file_name_(parameters.cache_folder + "/" +
                         project_subfolder_name +
                         "/parser/summaries/_!_summaries.txt"),
        server_cacher_(
            wiki_page_name,
            parameters.cache_folder + "/" + project_subfolder_name + "/server/",
            parameters.server, parameters.credentials),
        ignore_cache_(ignore_cache) {}

  virtual Pages get(const Date& identifier) {
    if (identifier > cache_date) {
      UpdatePages(identifier);
    }
    return pages_;
  }

 protected:
  using PageNominations = std::vector<std::pair<Date, Date>>;
  using PageName = std::string;
  std::map<PageName, PageNominations> all_nominations;
  Date cache_date;
  std::string cache_file_name_;
  WikiServerCacher server_cacher_;
  bool ignore_cache_;

  void AddDayPages(const Date& identifier, Pages& day_pages) {
    for (auto& page : day_pages) {
      auto& page_date = page.first;
      auto& page_name = page.second;
      auto result = std::find(
          all_nominations.begin(), all_nominations.end(),
          [page_name](const std::pair<PageName, PageNominations>& element) {
            return page_name == element.first;
          });

      if (result == day_pages.end()) {
        all_nominations.insert(
            page_name, PageNominations{std::make_pair(page_date, Date())});
      } else {
        auto& last_date_pair = *--(result->second.end());
        auto& last_date_start = last_date_pair.first;
        auto& last_date_end = last_date_pair.second;
        if (last_date_start == page_date) {
          if (last_date_end != Date()) {
            last_date_end = Date();
          }
        } else {
          result->second.push_back(std::make_pair(page_date, Date()));
        }
      }
    }

    for (auto& nomination : all_nominations) {
      auto result = std::find_if(
          day_pages.begin(),
          day_pages.end(), [name = nomination.first](const Page& element) {
            return element.second == name;
          });
      if (result == all_nominations.end()) {
        auto& last_date = *(--nomination.second.end());
        if (last_date.second != Date()) {
          last_date.second = identifier;
        }
      } else {
      }
    }
  }

  void UpdatePages(const Date& identifier) {
    if (identifier <= cache_date) {
      return;
    }
    auto page_text = server_cacher_.get(identifier);
    auto day_pages = MainPageParseFunction(page_text);
    AddDayPages(identifier, day_pages);
  }
};

template <auto MainPageParserFunction>
class SummariesMainPageParser {
 public:
  using PageNominations = std::vector<std::pair<Date, Date>>;
  using PageName = std::string;
  std::map<PageName, PageNominations> all_nominations;

  SummariesMainPageParser() = default;
  ~SummariesMainPageParser() = default;

  virtual SummariesDayStats operator()(const Date& identifier,
                                       const std::string& page_text,
                                       const std::string& previous_page_text) {
    SummariesDayStats stats;
    auto pages = MainPageParserFunction(identifier, page_text);
    auto previous_pages =
        MainPageParserFunction(identifier, previous_page_text);
    RemoveDuplicates(pages);
    stats.values = GetStatsForCurrentDay(identifier, pages, previous_pages);
    return stats;
  }

 protected:
  std::vector<int> GetStatsForCurrentDay(const Date& current_date,
                                         const Pages& pages,
                                         const Pages& previous_pages) {
    std::vector<int> stats{0, 0, 0};
    bool return_empty_stats = false;

    for (const auto& page : pages) {
      auto found_nomination = std::find_if(
          previous_pages.begin(), previous_pages.end(),
          [page](const Page& p) { return page.second == p.second; });
      if (found_nomination != nominations_.end()) {
        Date last_start_date = found_nomination->GetLastStartDate();
        if (last_start_date != page.first) {
          found_nomination->AddNewStartDate(page.first);
          ++stats.at(2);
        }
      } else {
        nominations_.push_back(PageNominationDates(page.second, page.first));
        ++stats.at(0);
      }
    }
    for (auto& page : previous_pages) {
      auto page = std::find_if(pages.begin(), pages.end(),
                               [nomination](const Page& page) {
                                 return page.second == nomination.name;
                               });
      if (page == pages.end() and not nomination.LastDateIsEnd()) {
        nomination.AddNewEndDate(current_date);
        ++stats.at(1);
      }
    }
    if (return_empty_stats) {
      stats = std::vector<int>{0, 0, 0};
    }
    return stats;
  }
};  //*/
/*
template <auto MainPageParserFunction>
class SummariesParserCacher : public SingleFileCacher<Date, SummariesDayStats> {
 public:
  SummariesParserCacher(const std::string& wiki_page_name,
                        const std::string& project_subfolder_name,
                        const GeneralParameters& parameters,
                        const bool ignore_cache = false)
      : SingleFileCacher(parameters.cache_folder + "/" +
                         project_subfolder_name +
                         "/parser/summaries/_!_summaries.txt"),
        server_cacher_(
            wiki_page_name,
            parameters.cache_folder + "/" + project_subfolder_name + "/server/",
            parameters.server, parameters.credentials),
        ignore_cache_(ignore_cache) {}
  virtual ~SummariesParserCacher() override = default;

 protected:
  WikiServerCacher server_cacher_;
  bool ignore_cache_;
  SummariesMainPageParser<MainPageParserFunction> main_page_parser_;

  SingleFileCacher;

  virtual SummariesDayStats GetData(const Date& identifier) override {
    auto previous_identifier = identifier;
    --previous_identifier;

    Date::Today();

    std::string previous_data = server_cacher_.get(identifier, ignore_cache_);
    std::string data = server_cacher_.get(identifier, ignore_cache_);
    return main_page_parser_(identifier, data, previous_data);
  }
};

/*
class SummariesCurvesCreator : public CurvesCreator {
 public:
  SummariesCurvesCreator(const GeneralParameters& parameters,
                         const std::string& server_page_name,
                         const std::string& project_subfolder_name,
                         MainPageParser& main_page_parser)
      : CurvesCreator(server_page_name,
                      parameters.cache_folder + "/" + project_subfolder_name +
                          "/parser/summaries/_!_excluded_dates.txt"),
        data_provider_(parameters, project_subfolder_name, main_page_parser) {}

  virtual ~SummariesCurvesCreator() override = default;

 protected:
  SummariesParserCacher data_provider_;

  virtual std::vector<std::string> CreateLabelsForDayStats() const override {
    return {"Открытие", "Закрытие", "Перенос"};
  }

  virtual std::vector<double> CreateStatsForRevision(
      const StatisticsRevisionIdentifier& identifier) override {
    try {
      auto values = data_provider_.get(identifier).values;
      std::vector<double> stats(values.begin(), values.end());
      return stats;
    } catch (const errors::NoDataForThisIdentifier&) {
      throw NoDataForThisDay();
    }
  }
};

template <auto ParserFunction>
SummariesDayStats CreateSummariesStatsFromParsingMainPage(
    const StatisticsRevisionIdentifier& identifier,
    const std::string& page_text) {
  return SummariesMainPageParser<ParserFunction>()(identifier, page_text);
}
//*/
#endif

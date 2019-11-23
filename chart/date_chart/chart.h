
#pragma once

#include "chart/curves.h"
#include "chart/svg_chart.h"
#include "common/simple_date.h"
#include "statistics/statistics.h"

#include <regex>

PointArea CalculateExtremumValues(const chart::Curves<double>& curves);

std::vector<double> CreateMonthGridPoints(const Date& start, const Date& end);
std::vector<double> CreateYearGridPoints(const Date& start, const Date& end);
std::vector<double> CreateValueGridPoints(const int max_y, const int step);

std::vector<std::pair<double, std::string>> CreateYearGridNumbers(
    const Date& start, const Date& end);
std::vector<std::pair<double, std::string>> CreateValueGridNumbers(
    const int max_y, const int step);

#if 0

struct DetailedDayStats;

std::ostream& operator<<(std::ostream& stream, const DetailedDayStats& stats);
std::istream& operator>>(std::istream& stream, DetailedDayStats& stats);

struct DetailedDayStats {
  DetailedDayStats() = default;
  DetailedDayStats(const std::string& string) {
    auto fields = SplitString(string, '|');
    if (fields.size() == 0) {
      throw errors::NotParsedFromString();
    }
    this->values.resize(fields.size());
    for (auto i = 0ul; i < fields.size(); ++i) {
      this->values[i] = std::stoi(fields[i]);
    }
  }
  ~DetailedDayStats() = default;

  std::vector<int> values;
};

template <auto ParserFunction>
DetailedDayStats CreateDetailedStatsFromParsingMainPage(
    const Date& identifier, const std::string& page_text) {
  DetailedDayStats stats;
  auto pages = ParserFunction(identifier, page_text);
  stats.values.resize(7);
  for (const auto& page : pages) {
    auto interval = identifier - page.first;
    ++stats.values[0];
    if (interval >= 8) {
      ++stats.values[1];
      if (interval >= 30) {
        ++stats.values[2];
        if (interval >= 90) {
          ++stats.values[3];
          if (interval >= 183) {
            ++stats.values[4];
            if (interval >= 365) {
              ++stats.values[5];
              if (interval >= 730) {
                ++stats.values[6];
              }
            }
          }
        }
      }
    }
  }
  return stats;
}

struct WikiPageDateTimestampElement {
  std::string name;
  Date date;
  wiki::Timestamp timestamp;

  WikiPageDateTimestampElement() = default;
  WikiPageDateTimestampElement(
      const std::string& name_, const Date& date_,
      const wiki::Timestamp& timestamp_ = wiki::Timestamp())
      : name(name_), date(date_), timestamp(timestamp_) {}

  WikiPageDateTimestampElement(const std::string& string) {
    auto first_delimiter = string.find_first_of('|');
    auto second_delimiter = string.find_first_of('|', first_delimiter + 1);
    this->name = string.substr(0, first_delimiter);
    this->date = Date(
        string.substr(first_delimiter + 1, second_delimiter - first_delimiter));
    this->timestamp = wiki::Timestamp(string.substr(second_delimiter + 1));
  }

  bool operator==(const WikiPageDateTimestampElement& other) const {
    return name == other.name and date == other.date and
           timestamp == other.timestamp;
  }
  bool operator<(const WikiPageDateTimestampElement& other) const {
    return name < other.name and date < other.date and
           timestamp < other.timestamp;
  }
  operator std::string() const {
    return this->name + "|" + static_cast<std::string>(date) + "|" +
           static_cast<std::string>(timestamp);
  }
};  //*/

class WikiServerCacherExtended : public WikiServerCacher {
 public:
  WikiServerCacherExtended() = delete;
  WikiServerCacherExtended(
      const GeneralParameters& parameters,
      std::unique_ptr<wiki::QueryPerformer>&& query_performer)
      : WikiServerCacher{parameters.cache_folder, std::move(query_performer)},
        timestamp_from_date_(parameters.cache_folder +
                             "/detailed/date_timestamp_table.txt") {}

  virtual std::string get(const std::string& wiki_page_name, const Date& date,
                          const bool ignore_cache = false) {
    return "";
  }

 protected:
  CacheIndex<WikiPageDateTimestampElement> timestamp_from_date_;

  wiki::Page::Revision GetBestRevisionFromServer(
      const std::string& wiki_page_name, const Date& date) {
    std::string start_timestamp = date.ToString() + "T23:59:59Z";
    std::string end_timestamp = date.ToString() + "T00:00:00Z";

    return wiki::Page::Revision{};
  }

  WikiPageIdentifier GetWikiPageIdentifierFromDate(
      const std::string& wiki_page_name, const Date& date) {
    return WikiPageIdentifier{wiki_page_name, wiki::Timestamp{}};
  }
};

template <auto MainPageParseFunction>
class DetailedParserCacher : public SingleFileCacher<Date, DetailedDayStats> {
 public:
  DetailedParserCacher(const std::string& wiki_page_name,
                       const std::string& unique_name,
                       const GeneralParameters& parameters,
                       const bool ignore_cache = false)
      : SingleFileCacher(parameters.cache_folder + "/detailed/" + unique_name +
                         "_parser.txt"),
        // server_cacher_(parameters),
        wiki_page_name_(wiki_page_name),
        ignore_cache_(ignore_cache) {}
  ~DetailedParserCacher() = default;

 protected:
  // WikiServerCacherExtended server_cacher_;
  std::string wiki_page_name_;
  bool ignore_cache_;

  virtual DetailedDayStats GetData(const Date& identifier) override {
    // std::string server_data =
    //    server_cacher_.get(wiki_page_name_, identifier, ignore_cache_);
    // return MainPageParseFunction(identifier, server_data);
    return DetailedDayStats{};
  }
};

template <auto MainPageParseFunction>
class DetailedCurvesCreator : public CurvesCreator {
 public:
  DetailedCurvesCreator(const std::string& wiki_page_name,
                        const std::string& unique_name,
                        const GeneralParameters& parameters)
      : CurvesCreator(parameters.cache_folder + "/detailed/" + unique_name +
                      "_excluded_dates.txt"),
        data_provider_(wiki_page_name, unique_name, parameters) {}

  virtual ~DetailedCurvesCreator() override = default;

 protected:
  DetailedParserCacher<MainPageParseFunction> data_provider_;

  virtual std::vector<std::string> CreateLabelsForDayStats() const override {
    return {"Всего", "≥8", "≥30", "≥90", "≥183", "≥365", "≥730"};
  }

  virtual std::vector<double> CreateStatsForDate(
      const Date& identifier) override {
    try {
      auto values = data_provider_.get(identifier).values;
      std::vector<double> stats(values.begin(), values.end());
      return stats;
    } catch (const errors::NoDataForThisIdentifier&) {
      throw NoDataForThisDay();
    }
  }
};

template <auto Regex>
Pages ParseMainPage(const Date& identifier, const std::string& page_text) {
  // std::ignore = identifier;
  Pages result;
  std::regex day_regex(
      Regex, std::regex_constants::ECMAScript | std::regex_constants::icase);
  auto days_begin =
      std::sregex_iterator(page_text.begin(), page_text.end(), day_regex);
  auto days_end = std::sregex_iterator();
  Date date;
  for (auto& it = days_begin; it != days_end; ++it) {
    std::string date_string = (*it)[2].str();
    date.FromString(date_string);
    // auto pages = ParseDayString((*it)[3].str());
    // for (auto& i : pages) {
    //  result.push_back(std::pair<Date, std::string>(date, i));
    //}
  }
  std::cout << "parsing ... " << identifier << std::endl;
  if (identifier == Date(2011, 9, 19)) {
    /*
    std::ofstream stream("/media/sf_D_DRIVE/wiki/dump1.txt");
    if (stream) {
      for (auto& page : result) {
        stream << page.first << " | " << page.second << std::endl;
      }
    } else {
      std::cout << "error\n";
    }
    std::cout << "DUMPED\n";
    */
  }
  return result;
}

#endif

class DetailedSvgChart : public SvgChart {
 public:
  DetailedSvgChart(const Point<double> image_size, const Date& start_date,
                   const Date& end_date, const std::string& title,
                   const int vertical_numbers_step = 1000,
                   const int vertical_grid_step = 1000,
                   const int vertical_rounding_step = 500);

  void SetVertialSteps(const int grid_step, const int numbers_step,
                       const int rounding_step);

  void AddCurve(const chart::Curve<double>& curve, const svg::Attributes& attributes,
                const bool add_to_legend = true);

  void AddCurve(const chart::Curve<double>& curve, const svg::Attributes& attributes,
                const std::string& override_name);

  void AddDatesRects(const std::string& legend_name,
                     const std::vector<std::pair<Date, Date>>& dates,
                     const svg::Attributes& fill);

  void SaveToFile(const std::string& file_name);

 protected:
  PointArea extremums_;
  Date start_date_, end_date_;
  int vertical_numbers_step_, vertical_grid_step_, vertical_rounding_step_;
  std::vector<chart::Curve<double>> curves_;
  std::vector<svg::Attributes> attributes_;
  std::vector<bool> add_to_legend_;

  std::vector<std::pair<Date, Date>> dates_rects_;
  std::string dates_rects_name_;
  svg::Attributes dates_rects_fill_;

 protected:
  void CalculateCurvesSpecificData();
  void AddClipPath();
  void AddGrid();
  void AddNumbers();
  void AddCurves();
  void AddCurvesToLegend();
  void AddFrame();
  void AddLegend();
  void AddBackground();
  void AddDatesRects();
  void AddDatesRectsToLegend();
  void AddInnerSvg();
};

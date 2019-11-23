
#pragma once

#include "chart/curves.h"

namespace chart {
struct DateData {
  DateData() = default;
  DateData(const std::string& string) {
    auto fields = SplitString(string, '|');
    if (fields.size() != 0) {
      this->values.resize(fields.size());
      for (auto i = 0ul; i < fields.size(); ++i) {
        this->values[i] = std::stoi(fields[i]);
      }
    }
  }
  operator std::string() const {
    if (not values.empty()) {
      std::stringstream ss{};
      ss << *values.begin();
      for (auto it = ++values.begin(); it != values.end(); ++it) {
        ss << " | " << *it;
      }
      return ss.str();
    }
    return std::string{};
  }
  ~DateData() = default;

  std::vector<double> values;
};

struct DateDataProvider {
  virtual ~DateDataProvider() = default;
  virtual std::optional<DateData> GetDateData(const Date& date) = 0;
  virtual std::vector<std::string> GetLabels() = 0;
};

struct CurvesDataCache {
  virtual ~CurvesDataCache() = default;
  virtual std::optional<DateData> get(const Date& date) = 0;
  virtual void put(const Date& date, const DateData& data) = 0;
};

class CurvesDataCacheFS : public CurvesDataCache {
 public:
  CurvesDataCacheFS(const std::string& file_name) : index_{file_name} {}
  virtual ~CurvesDataCacheFS() override = default;
  virtual std::optional<DateData> get(const Date& date) override {
    return index_.find(date);
  }
  virtual void put(const Date& date, const DateData& data) override {
    index_.add(date, data);
  }

 protected:
  FileIndexTwoValues<Date, DateData> index_;
};

struct CurvesDataPolicy {
  virtual ~CurvesDataPolicy() = default;
  virtual bool CacheDataIsPreferable(const Date& date) = 0;
  virtual bool DateMustBeIgnored(const Date& date) = 0;
  virtual bool MarkDateAsAbsent(const Date& date) = 0;
};

struct DefaultCurvesDataPolicy : public CurvesDataPolicy {
  virtual ~DefaultCurvesDataPolicy() override = default;
  virtual bool CacheDataIsPreferable(const Date&) override { return true; }
  virtual bool DateMustBeIgnored(const Date&) override { return false; }
  virtual bool MarkDateAsAbsent(const Date&) override { return false; }
};

class DateCurvesCreator {
 public:
  struct errors {};
  DateCurvesCreator() = delete;
  DateCurvesCreator(std::unique_ptr<DateDataProvider>&& data_provider,
                    std::unique_ptr<CurvesDataCache>&& data_cache,
                    std::unique_ptr<CurvesDataPolicy>&& cache_policy)
      : data_{std::move(data_provider)},
        cache_{std::move(data_cache)},
        policy_{std::move(cache_policy)} {}
  virtual ~DateCurvesCreator() = default;

  virtual Curves<double> create(const Date& start_date, const Date& end_date) {
    Curves<double> curves{};
    auto labels = data_->GetLabels();
    std::size_t number_of_curves{labels.size()};
    curves.reserve(number_of_curves);
    for (const auto& label : labels) {
      curves.push_back(Curve<double>{label});
      (--curves.end())->points.reserve(end_date - start_date + 1);
    }
    long x{};
    for (Date date{start_date}; date <= end_date; ++date, ++x) {
      std::cout << "date = " << date << " ... " << std::flush;
      if (policy_->DateMustBeIgnored(date)) {
        std::cout << "skipped" << std::endl;
        continue;
      }
      if (policy_->CacheDataIsPreferable(date)) {
        auto data{cache_->get(date)};
        if (data.has_value()) {
          std::cout << "from cache" << std::endl;
          AddToCurves(static_cast<double>(x), data.value(), number_of_curves,
                      curves);
          continue;
        }
      }
      std::cout << "from page getter ... " << std::flush;
      auto data{data_->GetDateData(date)};
      if (data.has_value()) {
        std::cout << "got" << std::endl;
        AddToCurves(static_cast<double>(x), data.value(), number_of_curves,
                    curves);
        cache_->put(date, data.value());
      } else {
        std::cout << "absent" << std::endl;
        policy_->MarkDateAsAbsent(date);
      }
    }
    for (auto& curve : curves) {
      curve.points.shrink_to_fit();
    }
    return curves;
  }

 protected:
  void AddToCurves(const double x, const DateData& data,
                   const std::size_t number_of_curves, Curves<double>& curves) {
    Point<double> p{};
    p.x = x;
    for (std::size_t i{}; i < number_of_curves; ++i) {
      p.y = static_cast<double>(data.values[i]);
      curves[i].points.push_back(p);
    }
  }

  std::unique_ptr<DateDataProvider> data_;
  std::unique_ptr<CurvesDataCache> cache_;
  std::unique_ptr<CurvesDataPolicy> policy_;
};
}  // namespace chart

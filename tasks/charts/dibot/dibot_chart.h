
#pragma once

#include "chart/date_chart/curves.h"
#include "wiki/page_getter.h"

class DibotDataProvider : public chart::DateDataProvider {
 public:
  DibotDataProvider(std::unique_ptr<wiki::PageGetter>&& page_getter);
  virtual ~DibotDataProvider() override;
  virtual std::optional<chart::DateData> GetDateData(const Date& date) override;
  virtual std::vector<std::string> GetLabels() override;

 protected:
  class DibotDataProviderImpl;
  std::unique_ptr<DibotDataProviderImpl> impl_;
};

class DibotCurvesDataPolicy : public chart::CurvesDataPolicy {
 public:
  DibotCurvesDataPolicy(const std::string& absent_dates_file_name,
                        const std::string& excluded_dates_file_name);
  virtual ~DibotCurvesDataPolicy() override;
  virtual bool CacheDataIsPreferable(const Date& date) override;
  virtual bool DateMustBeIgnored(const Date& date) override;
  virtual bool MarkDateAsAbsent(const Date& date) override;

 protected:
  class DibotCurvesDataPolicyImpl;
  std::unique_ptr<DibotCurvesDataPolicyImpl> impl_;
};

std::unique_ptr<chart::DateCurvesCreator> CreateDibotDateCurvesCreator(
    const GeneralParameters& parameters);

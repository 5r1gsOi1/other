
#pragma once

#include "cache.h"
#include "common/simple_date.h"
#include "query.h"

#include <memory>

namespace wiki {

struct PageGetter {
  virtual ~PageGetter() = default;
  virtual std::optional<wiki::Page::Revision> GetRevision(
      const std::string& page_name, const std::string& timestamp) = 0;
  virtual std::optional<wiki::Page::Revision> GetLastRevisionForRange(
      const std::string& page_name, const std::string& start_timestamp,
      const std::string& end_timestamp) = 0;
  virtual std::optional<wiki::Page::Revision> GetLastRevisionForDay(
      const std::string& page_name, const Date& date) = 0;
  virtual std::optional<wiki::Page::Revision> GetLastRevision(
      const std::string& page_name) = 0;
};

class CachedPageGetter : public PageGetter {
 public:
  CachedPageGetter() = delete;
  CachedPageGetter(std::unique_ptr<QueryPerformer>&& query_performer,
                   std::unique_ptr<PageCache>&& page_cache,
                   std::unique_ptr<CachePolicy>&& cache_policy)
      : query_{std::move(query_performer)},
        cache_{std::move(page_cache)},
        policy_{std::move(cache_policy)} {}
  CachedPageGetter(CachedPageGetter&) = delete;
  CachedPageGetter(CachedPageGetter&&) = default;

  virtual ~CachedPageGetter() override = default;

  virtual std::optional<wiki::Page::Revision> GetRevision(
      const std::string& page_name, const std::string& timestamp) override;

  std::optional<wiki::Page::Revision> GetLastRevisionForRange(
      const std::string& page_name, const std::string& start_timestamp,
      const std::string& end_timestamp) override;

  virtual std::optional<wiki::Page::Revision> GetLastRevisionForDay(
      const std::string& page_name, const Date& date) override;

  virtual std::optional<wiki::Page::Revision> GetLastRevision(
      const std::string& page_name) override;

 protected:
  std::unique_ptr<QueryPerformer> query_;
  std::unique_ptr<PageCache> cache_;
  std::unique_ptr<CachePolicy> policy_;
};

struct DirectPageGetter : public PageGetter {
  DirectPageGetter() = delete;
  DirectPageGetter(std::unique_ptr<QueryPerformer>&& query_performer)
      : query_{std::move(query_performer)} {}
  DirectPageGetter(DirectPageGetter&) = delete;
  DirectPageGetter(DirectPageGetter&&) = default;

  virtual ~DirectPageGetter() override = default;

  virtual std::optional<wiki::Page::Revision> GetRevision(
      const std::string& page_name, const std::string& timestamp) override;

  std::optional<wiki::Page::Revision> GetLastRevisionForRange(
      const std::string& page_name, const std::string& start_timestamp,
      const std::string& end_timestamp) override;

  virtual std::optional<wiki::Page::Revision> GetLastRevisionForDay(
      const std::string& page_name, const Date& date) override;

  virtual std::optional<wiki::Page::Revision> GetLastRevision(
      const std::string& page_name) override;

 protected:
  std::unique_ptr<QueryPerformer> query_;
};

}  // namespace wiki

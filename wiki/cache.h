
#pragma once

#include <optional>
#include <set>
#include <string>

#include "common/simple_date.h"
#include "wiki.h"

namespace wiki {
struct PageCache {
  virtual ~PageCache() = default;
  virtual std::optional<wiki::Page::Revision> GetLastRevision(
      const std::string& page_name) = 0;
  virtual std::optional<wiki::Page::Revision> GetRevision(
      const std::string& page_name, const std::string& timestamp) = 0;
  virtual std::optional<std::set<std::string>> GetAvailableTimestamps(
      const std::string& page_name) = 0;
  virtual bool AddRevision(const std::string& page_name,
                           const Page::Revision& revision,
                           const bool override_existing = false) = 0;
  // virtual void RemoveAllRevisionsButLast(const std::string& page_name) = 0;
};

struct CachePolicy {
  virtual ~CachePolicy() = default;

  virtual bool CacheDataIsPreferable(const std::string& page_name,
                                     const std::string& timestamp) = 0;
  virtual bool CacheDataIsPreferable(const std::string& page_name,
                                     const Date& date) = 0;
  virtual bool CacheDataIsPreferable(const std::string& page_name) = 0;
  virtual void PageIsAbsentOnServer(const std::string& page_name) = 0;
  virtual void PageRevisionIsAbsentOnServer(const std::string& page_name,
                                            const std::string& timestamp) = 0;
};

class CachePolicyAlwaysCache : public CachePolicy {
 public:
  CachePolicyAlwaysCache() = default;
  virtual ~CachePolicyAlwaysCache() override = default;

  virtual bool CacheDataIsPreferable(const std::string&) override {
    return true;
  }

  virtual bool CacheDataIsPreferable(const std::string&,
                                     const std::string&) override {
    return true;
  }
  virtual bool CacheDataIsPreferable(const std::string&, const Date&) override {
    return true;
  }

  virtual void PageIsAbsentOnServer(const std::string&) override {}

  virtual void PageRevisionIsAbsentOnServer(const std::string&,
                                            const std::string&) override {}
};

class CachePolicyNeverCache : public CachePolicy {
 public:
  CachePolicyNeverCache() = default;
  virtual ~CachePolicyNeverCache() override = default;

  virtual bool CacheDataIsPreferable(const std::string&) override {
    return false;
  }

  virtual bool CacheDataIsPreferable(const std::string&,
                                     const std::string&) override {
    return false;
  }
  virtual bool CacheDataIsPreferable(const std::string&, const Date&) override {
    return false;
  }

  virtual void PageIsAbsentOnServer(const std::string&) override {}

  virtual void PageRevisionIsAbsentOnServer(const std::string&,
                                            const std::string&) override {}
};

}  // namespace wiki

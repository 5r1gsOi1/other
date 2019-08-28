
#pragma once

#include "cache.h"
#include "common/file_index.h"
#include "wiki.h"

#include <algorithm>
#include <set>

namespace wiki {

class PageCacheFS : public PageCache {
 public:
  PageCacheFS() = delete;
  PageCacheFS(const std::string& directory)
      : directory_{directory}, index_{directory + "/" + kIndexFileName} {
    CreateDirectoryIfNotExists(directory_);
  }
  virtual ~PageCacheFS() override = default;

  virtual std::optional<wiki::Page::Revision> GetLastRevision(
      const std::string& page_name) override;

  virtual std::optional<wiki::Page::Revision> GetRevision(
      const std::string& page_name, const std::string& timestamp) override;

  virtual std::optional<std::set<std::string>> GetAvailableTimestamps(
      const std::string& page_name) override;

  virtual bool AddRevision(const std::string& page_name,
                           const Page::Revision& revision,
                           const bool override_existing = false) override;

 private:
  std::string directory_;
  inline static const std::string kIndexFileName{"_index.txt"};
  inline static const std::string kPageInfoFileName{"_info.txt"};
  FileIndexTwoValues<std::string, std::string> index_;
};
}  // namespace wiki

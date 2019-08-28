
#pragma once

#include "wiki.h"

namespace wiki {
struct QueryPerformer {
  struct errors {
    struct General : public wiki::errors::General {};

    struct Technical : public General {};
    struct Connection : public Technical {};
    struct Response : public Technical {};
    struct Login : public Technical {};

    struct Semantic : public General {};
    struct Page : public Semantic {};
    struct PageAbsent : public Page {};
    struct RevisionAbsent : public Page {};
    struct Category : public Semantic {};
    struct CategoryAbsent : public Category {};
  };

  virtual ~QueryPerformer() = default;

  virtual Page GetPageRevision(const std::string& page_name,
                               const std::string& timestamp) = 0;
  virtual Page GetPageRevisions(
      const std::string& page_name,
      const std::string& start_timestamp = std::string{},
      const std::string& end_timestamp = std::string{},
      const bool content_should_be_included = true) = 0;
  virtual Page GetPageLastRevision(const std::string& page_name) = 0;

  virtual Pages GetPagesInCategory(const std::string& category_name) = 0;
};
}  // namespace wiki

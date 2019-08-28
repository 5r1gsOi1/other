
#include "page_getter.h"
#include <iostream>

std::optional<wiki::Page::Revision> wiki::CachedPageGetter::GetRevision(
    const std::string &page_name, const std::string &timestamp) {
  if (policy_->CacheDataIsPreferable(page_name, timestamp)) {
    return cache_->GetRevision(page_name, timestamp);
  } else {
    try {
      auto page{query_->GetPageRevision(page_name, timestamp)};
      const auto &revision{page.revisions.begin()->second};
      cache_->AddRevision(page_name, revision, true);
      return revision;
    } catch (wiki::QueryPerformer::errors::Semantic &) {
      policy_->PageRevisionIsAbsentOnServer(page_name, timestamp);
      return std::nullopt;
    } catch (wiki::QueryPerformer::errors::Technical &) {
      return std::nullopt;
    }
  }
}

std::optional<std::string> GetLastTimestampForDate(
    const std::set<std::string> &timestamps, const Date &date) {
  const std::string start_timestamp{date.ToString() + "T00:00:00"};
  const std::string end_timestamp{date.ToString() + "T23:59:59"};
  std::string last_timestamp{};
  for (const auto &ts : timestamps) {
    if (ts >= start_timestamp and ts <= end_timestamp and ts > last_timestamp) {
      last_timestamp = ts;
    }
  }
  return last_timestamp.empty() ? std::nullopt
                                : std::make_optional(last_timestamp);
}

std::optional<wiki::Page::Revision>
wiki::CachedPageGetter::GetLastRevisionForRange(
    const std::string &page_name, const std::string &start_timestamp,
    const std::string &end_timestamp) {
  try {
    auto page{
        query_->GetPageRevisions(page_name, start_timestamp, end_timestamp)};

    auto ts{std::find_if(page.revisions.begin(), page.revisions.end(),
                         [&start_timestamp, &end_timestamp](const auto &rv) {
                           return rv.first >= start_timestamp and
                                  rv.first <= end_timestamp;
                         })};
    if (ts != page.revisions.end()) {
      return ts->second;
    }
  } catch (wiki::QueryPerformer::errors::General &) {
    // TODO: what to do here?
  }
  return std::nullopt;
}

std::optional<wiki::Page::Revision>
wiki::CachedPageGetter::GetLastRevisionForDay(const std::string &page_name,
                                              const Date &date) {
  const std::string start_ts{date.ToString() + "T00:00:00Z"};
  const std::string end_ts{date.ToString() + "T23:59:59Z"};

  if (policy_->CacheDataIsPreferable(page_name, date)) {
    auto timestamps{cache_->GetAvailableTimestamps(page_name)};
    if (timestamps.has_value()) {
      auto last_ts{std::find_if(timestamps.value().rbegin(),
                                timestamps.value().rend(),
                                [start_ts, end_ts](const auto &ts) {
                                  return ts >= start_ts and ts <= end_ts;
                                })};
      if (last_ts != timestamps.value().rend()) {
        return cache_->GetRevision(page_name, *last_ts);
      }
    }
  }
  auto revision{GetLastRevisionForRange(page_name, start_ts, end_ts)};
  if (revision.has_value()) {
    cache_->AddRevision(page_name, revision.value(), true);
  }
  return revision;
}

std::optional<wiki::Page::Revision> wiki::CachedPageGetter::GetLastRevision(
    const std::string &page_name) {
  if (policy_->CacheDataIsPreferable(page_name)) {
    auto revision{cache_->GetLastRevision(page_name)};
    if (revision.has_value()) {
      return revision;
    }
  }
  try {
    auto page{query_->GetPageLastRevision(page_name)};
    const auto &revision{page.revisions.begin()->second};
    cache_->AddRevision(page_name, revision, true);
    return revision;
  } catch (wiki::QueryPerformer::errors::Semantic &) {
    policy_->PageIsAbsentOnServer(page_name);
    return std::nullopt;
  } catch (wiki::QueryPerformer::errors::Technical &e) {
    std::cout << "\nTechnical error: " << e.what() << std::endl;
    return std::nullopt;
  }
}


#pragma once

#include <string>
#include <vector>
#include "chart/chart.h"
#include "common/simple_date.h"
#include "statistics/cache.h"
#include "wiki/query.h"
#include "wiki/timestamp.h"
#include "wiki/wiki.h"

namespace stats {

template <class UserStat>
class Ranks {
 public:
  using UserName = std::string;
  using ProjectName = std::string;
  struct UserRank {
    std::map<ProjectName, UserStat> value{};
  };

  void AddUserStat(const std::string& project_name,
                   const std::string& user_name, const UserStat& stat) {
    storage_[user_name].value[project_name] += stat;
  }

  Ranks& operator+=(const Ranks& other) {
    for (const auto& r : other.storage_) {
      for (const auto& v : r.second.value) {
        this->storage_[r.first].value[v.first] += v.second;
      }
    }
    return *this;
  }

  // protected:
  std::map<UserName, UserRank> storage_{};
};

struct Nomination {
  Date date;
  std::string name;
};
using Nominations = std::vector<Nomination>;

bool operator<(const Nomination& left, const Nomination& right);

struct UserStatGeneral {
  int opened, closed;

  UserStatGeneral& operator=(const UserStatGeneral& other) {
    this->opened = other.opened;
    this->closed = other.closed;
    return *this;
  }

  UserStatGeneral& operator+=(const UserStatGeneral& other) {
    this->opened += other.opened;
    this->closed += other.closed;
    return *this;
  }
};
template <class UserStat>
struct RanksWithUnparsed {
  Ranks<UserStat> ranks, unparsed;
  RanksWithUnparsed& operator +=(const RanksWithUnparsed& other) {
    this->ranks += other.ranks;
    this->unparsed += other.unparsed;
    return *this;
  }
};

using GeneralRanks = RanksWithUnparsed<UserStatGeneral>;

struct UserStatDetailed {
  Nominations opened, closed;

  UserStatDetailed& operator=(const UserStatDetailed& other) {
    this->opened = other.opened;
    this->closed = other.closed;
    return *this;
  }

  UserStatDetailed& operator+=(const UserStatDetailed& other) {
    this->opened.insert(this->opened.end(), other.opened.begin(),
                        other.opened.end());
    this->closed.insert(this->closed.end(), other.closed.begin(),
                        other.closed.end());
    return *this;
  }
};

using DetailedRanks = RanksWithUnparsed<UserStatDetailed>;

struct StatisticsRevisionIdentifier {
  std::string name;
  Date date;
  bool operator==(const StatisticsRevisionIdentifier& other) const {
    return this->date == other.date and this->name == other.name;
  }
  operator std::string() const { return name + date.ToString(); }
};

std::ostream& operator<<(std::ostream& stream,
                         const StatisticsRevisionIdentifier& id);

std::istream& operator>>(std::istream& stream,
                         StatisticsRevisionIdentifier& id);

struct WikiPageIdentifier {
  std::string name;
  wiki::Timestamp timestamp;

  WikiPageIdentifier() = default;
  WikiPageIdentifier(const std::string& name_,
                     const wiki::Timestamp& timestamp_)
      : name(name_), timestamp(timestamp_) {}
  WikiPageIdentifier(const std::string& string) {
    auto delimiter_position = string.find_last_of('|');
    if (delimiter_position == std::string::npos) {
      throw errors::NotParsedFromString();
    }
    this->name = string.substr(0, delimiter_position);
    this->timestamp = wiki::Timestamp(string.substr(delimiter_position + 1));
  }

  bool operator==(const WikiPageIdentifier& other) const {
    return this->name == other.name and this->timestamp == other.timestamp;
  }

  bool operator<(const WikiPageIdentifier& other) const {
    return this->name < other.name and this->timestamp < other.timestamp;
  }

  operator std::string() const {
    return name + "|" + static_cast<std::string>(timestamp);
  }
};
}  // namespace stats

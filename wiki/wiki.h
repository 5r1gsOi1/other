
#pragma once

#include "common/basic.h"

#include <map>
#include <string>
#include <vector>

namespace wiki {

struct Page {
  using Timestamp = std::string;
  struct Revision {
    struct Header {
      Timestamp timestamp;
      std::string user;
      int user_id;
      int size;
    } header;
    std::string content;
  };

  std::string title;
  int id;
  std::map<Timestamp, Revision> revisions;
};

inline std::ostream& operator<<(std::ostream& stream,
                         const Page::Revision::Header& header) {
  stream << header.timestamp << std::endl
         << header.user << std::endl
         << header.user_id << std::endl
         << header.size << std::endl;
  return stream;
}

using Pages = std::vector<Page>;

namespace errors {
struct General : public ::errors::General {};
}  // namespace errors
}  // namespace wiki

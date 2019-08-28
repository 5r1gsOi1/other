
#pragma once

#include <string>

namespace parse {
struct ParsedData {
  virtual ~ParsedData() = default;
};

struct PageParser {
  virtual ~PageParser() = default;
  virtual ParsedData parse(const std::string& page_title,
                           const std::string& page_text) = 0;
};

}  // namespace parse

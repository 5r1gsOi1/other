
#include "statistics.h"
#include "common/basic.h"
#include "statistics/cache.h"
#include "wiki/parse.h"

#include <regex>

bool PageIsTechnical(const std::string page_string) {
  return page_string.find("Википедия:") != std::string::npos;
}

Date ExtractTemplateDateFromPage(const std::string& page_text) {
  // std::cout << page_name << "\n";
  // std::string page_text = GetRawRuWikiPage(page_name);
  Date date;
  std::regex afi_template_regex(
      R"(\{\{\s*(К|к)\s*улучшению\s*\|\s*([^\{\}]*)\s*\}\})");
  std::smatch match;
  if (std::regex_search(page_text, match, afi_template_regex)) {
    std::cout << "match : " << match[1].str() << std::endl;
    date.FromString(match[1].str());
  }
  std::cout << date << std::endl << std::endl;
  return date;
}

const std::string delimiter = " <|> ";

std::ostream& stats::operator<<(std::ostream& stream,
                                const StatisticsRevisionIdentifier& id) {
  stream << id.name << delimiter << id.date;
  return stream;
}

std::istream& stats::operator>>(std::istream& stream,
                                StatisticsRevisionIdentifier& id) {
  std::string line;
  std::getline(stream, line);
  auto delimiter_position = line.find(delimiter);
  if (delimiter_position != std::string::npos) {
    id.name = line.substr(0, delimiter_position);
    id.date.FromString(line.substr(delimiter_position + delimiter.size()));
  } else {
    throw errors::NotParsedFromString();
  }
  return stream;
}

bool stats::operator<(const Nomination& left, const Nomination& right) {
  return left.date < right.date or
         (left.date == right.date and left.name < right.name);
}


#ifndef WIKI_PARSE_
#define WIKI_PARSE_

#include "common/simple_date.h"
#include "query.h"

#include <memory>
#include <string>

std::string RemoveTag(const std::string& tag_name, const std::string& string);

std::string RemoveTagBodies(const std::string& tag_name, std::string& string);

std::string RemoveLink(const std::string& string);

bool ContainsTagStart(const std::string& tag_name, const std::string& string);
bool ContainsTagEnd(const std::string& tag_name, const std::string& string);

std::vector<std::string> GetLinksFromString(const std::string& string);
bool ClearPageName(const std::string& page_name, std::string& cleared_string);
std::vector<std::string> ParseDayString(const std::string& day_string);

namespace wiki::parse {
struct UserMessage {
  std::string name{}, text{}, timestamp{};
  bool is_ip{false};
};

struct Nomination {
  Date date;
  std::string name;
  bool is_closed;
  UserMessage start_user, close_user;
};

struct NominationsDayStatus {};  // TODO

struct NominationsDay {
  Date date;
  bool is_closed;
  std::vector<Nomination> nominations;
};

std::vector<std::string> ParseDayString(const std::string& day_string);

bool SectionTitleIsStrickenOut(const std::string& page_string);
std::string RemoveStrikeOutTags(const std::string& string);

bool StringIsFullLink(const std::string& string);
bool StringContainsLink(const std::string& string);

std::string RemoveLinkBrackets(const std::string& string);

inline bool NominationDateComparison(const Nomination& left,
                                     const Nomination& right) {
  return left.date < right.date;
}

using Nominations = std::vector<Nomination>;

}  // namespace wiki::parse

/*
namespace parse {


class ParserCacher : curves::DataProvider {
 public:
  ParserCacher(std::unique_ptr<wiki::QueryPerformer>&& query,
               std::unique_ptr<parse::PageParser>&& page_parser)
      : query_{std::move(query)}, page_parser_{std::move(page_parser)} {}
  virtual ~ParserCacher() = default;

 protected:
  std::unique_ptr<wiki::QueryPerformer> query_;
  std::unique_ptr<parse::PageParser> page_parser_;
};

}  // namespace parse

][

*/

#endif

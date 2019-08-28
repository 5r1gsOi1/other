
#include "parse.h"
#include "common/basic.h"

#include <regex>
#include <vector>

std::string RemoveTag(const std::string& tag_name, const std::string& string) {
  std::string open_tag = "<" + tag_name + ">",
              close_tag = "</" + tag_name + ">";
  auto open_position = string.find(open_tag),
       close_position = string.find(close_tag);
  if (open_position != std::string::npos and
      close_position != std::string::npos) {
    return string.substr(open_position + open_tag.size(),
                         close_position - open_position - open_tag.size());
  }
  return string;
}

std::string& RemoveTag(const std::string& tag_element, std::string& string) {
  std::string::size_type open_position;
  while ((open_position = string.find(tag_element)) != std::string::npos) {
    string.erase(open_position, tag_element.size());
  }
  return string;
}

std::string RemoveTagBodies(const std::string& tag_name, std::string& string) {
  RemoveTag("<" + tag_name + ">", string);
  RemoveTag("</" + tag_name + ">", string);
  return string;
}

std::string RemoveLink(const std::string& string) {
  std::string open_link = "[[", open_link_with_colon = "[[:", close_link = "]]";
  auto open_with_colon_position = string.find(open_link_with_colon);
  decltype(open_with_colon_position) open_position = std::string::npos;
  if (open_with_colon_position == std::string::npos) {
    open_position = string.find(open_link);
  }
  auto close_position = string.find(close_link);
  if (open_with_colon_position != std::string::npos and
      close_position != std::string::npos) {
    return string.substr(open_with_colon_position + open_link_with_colon.size(),
                         close_position - open_with_colon_position -
                             open_link_with_colon.size());
  } else if (open_position != std::string::npos and
             close_position != std::string::npos) {
    return string.substr(open_position + open_link.size(),
                         close_position - open_position - open_link.size());
  }
  return string;
}

//
// TODO: ContainsTag, GetLinksFromString
//

bool ContainsTagStart(const std::string& tag_name, const std::string& string) {
  if (tag_name.empty()) {
    return false;
  }
  return std::string::npos != string.find("<" + tag_name + ">");
}

bool ContainsTagEnd(const std::string& tag_name, const std::string& string) {
  if (tag_name.empty()) {
    return false;
  }
  return std::string::npos != string.find("</" + tag_name + ">");
}

template <class StringIt, class PatternIt>
StringIt StringStartsWithPattern(StringIt string_start, StringIt string_end,
                                 PatternIt pattern_start,
                                 PatternIt pattern_end) {
  while (string_start != string_end) {
    if (*string_start == ' ') {
      ++string_start;
      continue;
    }
    if (*string_start == *pattern_start) {
      ++pattern_start;
      if (pattern_start == pattern_end) {
        return ++string_start;
      }
    } else {
      return string_end;
    }
    ++string_start;
  }
  return string_end;
}

bool wiki::parse::SectionTitleIsStrickenOut(const std::string& name) {
  std::string open_pattern{"<s>"}, close_pattern{"</s>"};
  return name.end() != StringStartsWithPattern(name.begin(), name.end(),
                                               open_pattern.begin(),
                                               open_pattern.end()) and
         name.rend() != StringStartsWithPattern(name.rbegin(), name.rend(),
                                                close_pattern.rbegin(),
                                                close_pattern.rend());
}

bool wiki::parse::StringIsFullLink(const std::string& string) {
  std::string open_pattern{"[["}, close_pattern{"]]"};
  return string.end() != StringStartsWithPattern(string.begin(), string.end(),
                                                 open_pattern.begin(),
                                                 open_pattern.end()) and
         string.rend() != StringStartsWithPattern(
                              string.rbegin(), string.rend(),
                              close_pattern.rbegin(), close_pattern.rend());
}

bool wiki::parse::StringContainsLink(const std::string& string) {
  std::string open_pattern{"[["}, close_pattern{"]]"};
  auto start_pos{string.find(open_pattern)};
  if (start_pos != std::string::npos) {
    auto end_pos{string.find(close_pattern, start_pos)};
    return end_pos != std::string::npos;
  }
  return false;
}

std::string wiki::parse::RemoveStrikeOutTags(const std::string& string) {
  std::string open_pattern{"<s>"}, close_pattern{"</s>"};
  auto open_end{StringStartsWithPattern(
      string.begin(), string.end(), open_pattern.begin(), open_pattern.end())};
  auto close_end{StringStartsWithPattern(string.rbegin(), string.rend(),
                                         close_pattern.rbegin(),
                                         close_pattern.rend())};
  if (open_end == string.end() or close_end == string.rend()) {
    return string;
  }
  std::string result{};
  auto begin{open_end}, end{close_end.base()};
  std::copy(begin, end, std::back_inserter(result));
  return result;
}

std::string wiki::parse::RemoveLinkBrackets(const std::string& string) {
  std::string open_pattern{"[["}, close_pattern{"]]"};
  auto open_end{StringStartsWithPattern(
      string.begin(), string.end(), open_pattern.begin(), open_pattern.end())};
  auto close_end{StringStartsWithPattern(string.rbegin(), string.rend(),
                                         close_pattern.rbegin(),
                                         close_pattern.rend())};
  if (open_end == string.end() or close_end == string.rend()) {
    return string;
  }
  std::string result{};
  auto begin{open_end}, end{close_end.base()};
  if (*begin == ':') {
    ++begin;
  }
  std::copy(begin, end, std::back_inserter(result));
  return result;
}

std::vector<std::string> GetLinksFromString(const std::string& string) {
  std::vector<std::string> result;
  std::size_t search_start = 0, cut_start = 0, cut_end = 0;
DoWhileThereAreLinks:
  std::size_t link_start_size = 2;
  auto link_begin_position = string.find("[[", search_start);
  if (link_begin_position != std::string::npos) {
    if (link_begin_position + 2 < string.size() and
        string[link_begin_position + 2] == ':') {
      link_start_size = 3;
    }
    auto link_end_position =
        string.find("]]", link_begin_position + link_start_size);
    auto delimiter_position =
        string.find('|', link_begin_position + link_start_size);
    if (std::string::npos != delimiter_position and
        delimiter_position < link_end_position) {
      cut_end = delimiter_position;
    } else {
      cut_end = link_end_position;
    }
    cut_start = link_begin_position + link_start_size;
    result.push_back(string.substr(cut_start, cut_end - cut_start));
    search_start = link_end_position + 2;
    goto DoWhileThereAreLinks;
  }
  return result;
}

bool ClearPageName(const std::string& page_name, std::string& cleared_string) {
  bool was_cleared = false;
  std::string working_string = page_name;
DoUntilAllTagsRemoved:
  working_string = RemoveRepeatingSpaces(working_string);
  // std::cout << working_string << std::endl;
  if (working_string[0] == ':') {
    if (was_cleared) {
      working_string.erase(0, 1);
      goto DoUntilAllTagsRemoved;
    }
  } else if (working_string[0] == '[') {
    std::string link_begin = "[[", link_end = "]]";
    auto link_end_position = working_string.find(link_end);
    if (link_end_position != std::string::npos and
        link_end_position == working_string.size() - 2) {
      auto delimiter_position = working_string.find("|");
      if (delimiter_position == std::string::npos or
          RemoveRepeatingSpaces(
              working_string.substr(delimiter_position + 1,
                                    link_end_position - delimiter_position - 1))
              .empty()) {
        working_string = working_string.substr(
            link_begin.size(), link_end_position - link_begin.size());
        was_cleared = true;
        goto DoUntilAllTagsRemoved;
      } else {
        working_string = working_string.substr(
            link_begin.size(), delimiter_position - link_begin.size());
        was_cleared = true;
        goto DoUntilAllTagsRemoved;
      }
    }
  } else if (working_string[0] == '<') {
    auto tag_close_position = working_string.find(">", 1);
    if (tag_close_position != std::string::npos) {
      std::string tag_name = working_string.substr(1, tag_close_position - 1);
      std::size_t text_start = tag_name.size() + 2;

      auto end_tag_position = working_string.find("</" + tag_name + ">");
      if (end_tag_position != std::string::npos) {
        if (RemoveRepeatingSpaces(
                working_string.substr(end_tag_position + tag_name.size() + 3))
                .empty()) {
          working_string =
              working_string.substr(text_start, end_tag_position - text_start);
          goto DoUntilAllTagsRemoved;
        }
      }
    }
  } else if (working_string[0] == '{') {
    if (0 == working_string.find("{{")) {
      auto templates_end_position = working_string.find("}}");
      if (templates_end_position == working_string.size() - 2) {
        auto delimiter_position = working_string.find("|");
        if (delimiter_position != std::string::npos) {
          auto template_name = RemoveRepeatingSpaces(
              working_string.substr(2, delimiter_position - 2));
          if (template_name == "Страница дополнена" or
              template_name == "страница дополнена") {
            working_string = working_string.substr(
                delimiter_position + 1,
                templates_end_position - delimiter_position - 1);
            goto DoUntilAllTagsRemoved;
          }
        }
      }
    }
  }
  if (was_cleared and not working_string.empty()) {
    cleared_string = working_string;
  }
  return was_cleared;
}

struct PageName {
  std::string string;
  bool is_title_of_group;
  bool is_in_subgroup;
};

bool PageHasLink(const std::string page_string) {
  return page_string.find("[[") != std::string::npos and
         page_string.find("]]") != std::string::npos;
}

auto SplitDayString(const std::string& day_string) {
  std::vector<PageName> result;
  std::string delimiter = "•";
  std::string working_string = day_string;
  std::string::size_type start = 0, end = working_string.find(delimiter, start);
  std::string page_name;
  bool is_in_subgroup = false;

  bool contains_small_begin = false, contains_small_end = false;

  while (end != std::string::npos) {
    page_name = working_string.substr(start, end - start);

    if (not is_in_subgroup) {
      contains_small_begin = ContainsTagStart("small", page_name);
      is_in_subgroup = contains_small_begin;
      if (contains_small_begin and not result.empty()) {
        (--result.end())->is_title_of_group = true;
      }
    }
    if (is_in_subgroup) {
      contains_small_end = ContainsTagEnd("small", page_name);
    }
    if (not wiki::parse::SectionTitleIsStrickenOut(page_name) and
        PageHasLink(page_name)) {
      RemoveTagBodies("small", page_name);
      result.push_back(PageName{page_name, false, is_in_subgroup});
    }
    if (is_in_subgroup and contains_small_end) {
      is_in_subgroup = false;
    }
    start = end + delimiter.size();
    end = working_string.find(delimiter, start);
  }
  page_name = working_string.substr(start);
  if (not wiki::parse::SectionTitleIsStrickenOut(page_name) and
      PageHasLink(page_name)) {
    RemoveTagBodies("small", page_name);
    result.push_back(PageName{page_name, false, is_in_subgroup});
  }
  return result;
}

void TryToAddLinksFromStringToResult(const PageName& name,
                                     std::vector<std::string>& result) {
  std::string cleared;
  bool is_simple_link = ClearPageName(name.string, cleared);
  // std::cout << "is_simple_link = " << is_simple_link << std::endl;

  if (is_simple_link) {
    result.push_back(cleared);
  } else if (not name.is_title_of_group) {
    auto links = GetLinksFromString(name.string);
    result.insert(result.end(), links.begin(), links.end());
  }
}

std::vector<std::string> ParseDayString(const std::string& day_string) {
  std::vector<std::string> result;
  auto names = SplitDayString(day_string);
  for (auto& i : names) {
    TryToAddLinksFromStringToResult(i, result);
  }
  return result;
}

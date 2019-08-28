
#include "statistics/statistics.h"
#include <vector>
#include "common/tests.h"
#include "wiki/parse.h"

bool Test_GetLinksFromString_SingleCall(
    const std::string& string,
    const std::vector<std::string>& expected_result) {
  auto result = GetLinksFromString(string);
  return expected_result == result;
}

bool Test_GetLinksFromString_Empty() {
  return Test_GetLinksFromString_SingleCall("text text", {});
}

bool Test_GetLinksFromString_One() {
  return Test_GetLinksFromString_SingleCall("[[link link]]", {"link link"});
}

bool Test_GetLinksFromString_OneWithColon() {
  return Test_GetLinksFromString_SingleCall("[[:link link]]", {"link link"});
}

bool Test_GetLinksFromString_Many() {
  return Test_GetLinksFromString_SingleCall(
      "[[link1]] , [[:link2]], [[link link link]]",
      {"link1", "link2", "link link link"});
}

bool Test_GetLinksFromString() {
  TEST_START;
  CALL_AND_PRINT(Test_GetLinksFromString_Empty);
  CALL_AND_PRINT(Test_GetLinksFromString_One);
  CALL_AND_PRINT(Test_GetLinksFromString_OneWithColon);
  CALL_AND_PRINT(Test_GetLinksFromString_Many);
  TEST_END;
}

bool Test_SingleCall_ClearPageName(
    const std::string& string, const bool expected_result,
    const std::string& expected_cleared_string = std::string()) {
  std::string cleared_string;
  bool result = ClearPageName(string, cleared_string);
  return result == expected_result and
         cleared_string == expected_cleared_string;
}

bool Test_ClearPageName_WithSimpleName() {
  return Test_SingleCall_ClearPageName("  Name  ", false);
}

bool Test_ClearPageName_WithMixedNameWithLinkLast() {
  return Test_SingleCall_ClearPageName(" Name   [[:Link]] ", false);
}

bool Test_ClearPageName_WithMixedNameWithLinkFirst() {
  return Test_SingleCall_ClearPageName(" [[:Link]]  Name ", false);
}

bool Test_ClearPageName_WithSimpleLink() {
  return Test_SingleCall_ClearPageName(" [[Link]]   ", true, "Link");
}

bool Test_ClearPageName_WithLinkWithDelimiter() {
  return Test_SingleCall_ClearPageName("  [[  Link  |  Name   ]]   ", true,
                                       "Link");
}

bool Test_ClearPageName_WithLinkWithColon() {
  return Test_SingleCall_ClearPageName(" [[:Link]]   ", true, "Link");
}

bool Test_ClearPageName_WithLinkWithSpaces() {
  return Test_SingleCall_ClearPageName(" [[  :  Link    with   spaces   ]]   ",
                                       true, "Link with spaces");
}

bool Test_ClearPageName_WithTemplate() {
  return Test_SingleCall_ClearPageName(
      "   {{   Страница дополнена   |  [[  :  Link    with   spaces   ]]   }} ",
      true, "Link with spaces");
}

bool Test_ClearPageName_WithSTag() {
  return Test_SingleCall_ClearPageName(
      "   <s> [[  :  Link    with   spaces   ]]  </s> ", true,
      "Link with spaces");
}

bool Test_ClearPageName_WithSmallTag() {
  return Test_SingleCall_ClearPageName(
      "   <small> [[  :  Link    with   spaces   ]]  </small> ", true,
      "Link with spaces");
}

bool Test_ClearPageName_WithMixedContent() {
  return Test_SingleCall_ClearPageName(
      "   <small> <s>   {{   Страница дополнена   |  [[  :  Link    with   "
      "spaces   ]] }}  </s> </small> ",
      true, "Link with spaces");
}

bool Test_ClearPageName() {
  TEST_START;
  CALL_AND_PRINT(Test_ClearPageName_WithSimpleName);

  CALL_AND_PRINT(Test_ClearPageName_WithLinkWithDelimiter);

  CALL_AND_PRINT(Test_ClearPageName_WithMixedNameWithLinkLast);
  CALL_AND_PRINT(Test_ClearPageName_WithMixedNameWithLinkFirst);

  CALL_AND_PRINT(Test_ClearPageName_WithSimpleLink);
  CALL_AND_PRINT(Test_ClearPageName_WithLinkWithColon);
  CALL_AND_PRINT(Test_ClearPageName_WithLinkWithSpaces);

  CALL_AND_PRINT(Test_ClearPageName_WithTemplate);
  CALL_AND_PRINT(Test_ClearPageName_WithSTag);
  CALL_AND_PRINT(Test_ClearPageName_WithSmallTag);
  TEST_END;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_ClearPageName);
  CALL_AND_PRINT(Test_GetLinksFromString);
  TEST_END;
}

MAIN_TEST(Test_All)

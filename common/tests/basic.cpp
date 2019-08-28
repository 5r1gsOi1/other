
#include "common/basic.h"
#include "common/tests.h"

bool Test_RemoveRepeatingSpaces_SingleCall(const std::string& string,
                                           const std::string& expected_result) {
  return expected_result == RemoveRepeatingSpaces(string);
}

bool Test_RemoveRepeatingSpaces_WithNoSpaces() {
  return Test_RemoveRepeatingSpaces_SingleCall("string", "string");
}

bool Test_RemoveRepeatingSpaces_WithLeadingSpaces() {
  return Test_RemoveRepeatingSpaces_SingleCall("   string", "string");
}

bool Test_RemoveRepeatingSpaces_WithTrailingSpaces() {
  return Test_RemoveRepeatingSpaces_SingleCall("string   ", "string");
}

bool Test_RemoveRepeatingSpaces_WithLeadingAndTrailingSpaces() {
  return Test_RemoveRepeatingSpaces_SingleCall("     string   ", "string");
}

bool Test_RemoveRepeatingSpaces_WithMixedSpaces() {
  return Test_RemoveRepeatingSpaces_SingleCall("  ss ss   ss    ss  string   ",
                                               "ss ss ss ss string");
}

bool Test_RemoveRepeatingSpaces() {
  TEST_START;
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces_WithNoSpaces);
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces_WithLeadingSpaces);
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces_WithTrailingSpaces);
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces_WithLeadingAndTrailingSpaces);
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces_WithMixedSpaces);
  TEST_END;
}

bool Test_ReplaceSpacesWithHtmlCode_NoSpaces() {
  std::string string = "123456789";
  return string == ReplaceSpacesWithHtmlCode(string);
}

bool Test_ReplaceSpacesWithHtmlCode_StartAndEndSpaces() {
  std::string string1 = "  123456789   ";
  std::string string2 = "%20%20123456789%20%20%20";
  return string2 == ReplaceSpacesWithHtmlCode(string1);
}

bool Test_ReplaceSpacesWithHtmlCode_OnlySpaces() {
  std::string string1 = "      ";
  std::string string2 = "%20%20%20%20%20%20";
  return string2 == ReplaceSpacesWithHtmlCode(string1);
}

bool Test_ReplaceSpacesWithHtmlCode_NormalSpaces() {
  std::string string1 = "I love apples.";
  std::string string2 = "I%20love%20apples.";
  return string2 == ReplaceSpacesWithHtmlCode(string1);
}

bool Test_ReplaceSpacesWithHtmlCode_EmptyString() {
  std::string string1 = "";
  std::string string2 = "";
  return string2 == ReplaceSpacesWithHtmlCode(string1);
}

bool Test_ReplaceSpacesWithHtmlCode() {
  TEST_START;
  CALL_AND_PRINT(Test_ReplaceSpacesWithHtmlCode_NoSpaces);
  CALL_AND_PRINT(Test_ReplaceSpacesWithHtmlCode_StartAndEndSpaces);
  CALL_AND_PRINT(Test_ReplaceSpacesWithHtmlCode_NormalSpaces);
  CALL_AND_PRINT(Test_ReplaceSpacesWithHtmlCode_EmptyString);
  TEST_END;
}

bool Test_SplitString_EmptyString() {
  std::string string = "";
  auto result = SplitString(string, 'x');
  return std::vector<std::string>{""} == SplitString(string, 'x');
}

bool Test_SplitString_SingleString() {
  std::string string = "123456";
  return std::vector<std::string>{"123456"} == SplitString(string, 'x');
}

bool Test_SplitString_OneDelimiter() {
  std::string string = "123x456";
  return std::vector<std::string>{"123", "456"} == SplitString(string, 'x');
}

bool Test_SplitString_ManyDelimiters() {
  std::string string = "123x456x789x000";
  return std::vector<std::string>{"123", "456", "789", "000"} ==
         SplitString(string, 'x');
}

bool Test_SplitString_ManyDelimitersWithSpaces() {
  std::string string = "123x  456   x789  x   000";
  return std::vector<std::string>{"123", "  456   ", "789  ", "   000"} ==
         SplitString(string, 'x');
}

bool Test_SplitString_DelimiterAtBeginning() {
  std::string string = "x123x456x789";
  return std::vector<std::string>{"", "123", "456", "789"} ==
         SplitString(string, 'x');
}

bool Test_SplitString_DelimiterAtEnd() {
  std::string string = "123x456x789x";
  return std::vector<std::string>{"123", "456", "789", ""} ==
         SplitString(string, 'x');
}

bool Test_SplitString_DelimitersAtBeginningAndAtEnd() {
  std::string string = "x123x456x789x";
  return std::vector<std::string>{"", "123", "456", "789", ""} ==
         SplitString(string, 'x');
}

bool Test_SplitString_ManyDelimitersWithEmptyString() {
  std::string string = "xxx";
  auto result = SplitString(string, 'x');
  return std::vector<std::string>{"", "", "", ""} == SplitString(string, 'x');
}

bool Test_SplitString_ManyDelimitersWithSpacesString() {
  std::string string = " x  x   x    ";
  auto result = SplitString(string, 'x');
  return std::vector<std::string>{" ", "  ", "   ", "    "} ==
         SplitString(string, 'x');
}

bool Test_SplitString_Fail1() {
  std::string string = "12345";
  return std::vector<std::string>{"7890"} != SplitString(string, 'x');
}

bool Test_SplitString_Fail2() {
  std::string string = "x123x456x";
  auto result = SplitString(string, 'x');
  return std::vector<std::string>{"123", "456"} != SplitString(string, 'x');
}

bool Test_SplitString() {
  TEST_START;
  CALL_AND_PRINT(Test_SplitString_EmptyString);
  CALL_AND_PRINT(Test_SplitString_SingleString);
  CALL_AND_PRINT(Test_SplitString_OneDelimiter);
  CALL_AND_PRINT(Test_SplitString_ManyDelimiters);
  CALL_AND_PRINT(Test_SplitString_ManyDelimitersWithSpaces);
  CALL_AND_PRINT(Test_SplitString_DelimiterAtBeginning);
  CALL_AND_PRINT(Test_SplitString_DelimiterAtEnd);
  CALL_AND_PRINT(Test_SplitString_DelimitersAtBeginningAndAtEnd);
  CALL_AND_PRINT(Test_SplitString_ManyDelimitersWithEmptyString);
  CALL_AND_PRINT(Test_SplitString_ManyDelimitersWithSpacesString);
  CALL_AND_PRINT(Test_SplitString_Fail1);
  CALL_AND_PRINT(Test_SplitString_Fail2);
  TEST_END;
}

bool Test_FixedPointNumber_DefaultCtor() {
  FixedPointNumber<> n;
  return n.integer() == 0 and n.fraction() == 0;
}

bool Test_FixedPointNumber_IntCtor() {
  FixedPointNumber<> n(123, 456);
  return n.integer() == 123 and n.fraction() == 456;
}

bool Test_FixedPointNumber_DoubleCtor() {
  FixedPointNumber<> n(123.456);
  return n.integer() == 123 and n.fraction() == 456;
}

bool Test_FixedPointNumber_OperatorStringWithFraction() {
  FixedPointNumber<> n(123.456);
  return n.operator std::string() == "123.456";
}

bool Test_FixedPointNumber_OperatorStringWithoutFraction() {
  FixedPointNumber<> n(123.000);
  return n.operator std::string() == "123";
}

bool Test_FixedPointNumber() {
  TEST_START;
  CALL_AND_PRINT(Test_FixedPointNumber_DefaultCtor);
  CALL_AND_PRINT(Test_FixedPointNumber_IntCtor);
  CALL_AND_PRINT(Test_FixedPointNumber_DoubleCtor);
  CALL_AND_PRINT(Test_FixedPointNumber_OperatorStringWithFraction);
  CALL_AND_PRINT(Test_FixedPointNumber_OperatorStringWithoutFraction);
  TEST_END;
}

bool Test_CutToPrecision_Precise() {
  unsigned int cut_number = CutToPrecision(12345, 5);
  return cut_number == 12345;
}

bool Test_CutToPrecision_Cut() {
  unsigned int cut_number = CutToPrecision(1233333, 5);
  return cut_number == 12333;
}

bool Test_CutToPrecision_WithoutCut() {
  unsigned int cut_number = CutToPrecision(123, 55);
  return cut_number == 123;
}

bool Test_CutToPrecision_MaxWithoutCut() {
  unsigned int cut_number =
      CutToPrecision(std::numeric_limits<unsigned int>::max(), 55);
  return cut_number == std::numeric_limits<unsigned int>::max();
}

bool Test_CutToPrecision_Rounding() {
  unsigned int cut_number = CutToPrecision(12345678, 5);
  return cut_number == 12346;
}

bool Test_CutToPrecision_RoundingWithIncrement() {
  unsigned int cut_number = CutToPrecision(99999, 3);
  return cut_number == 100;
}

bool Test_CutToPrecision() {
  TEST_START;
  CALL_AND_PRINT(Test_CutToPrecision_Precise);
  CALL_AND_PRINT(Test_CutToPrecision_Cut);
  CALL_AND_PRINT(Test_CutToPrecision_WithoutCut);
  CALL_AND_PRINT(Test_CutToPrecision_MaxWithoutCut);
  CALL_AND_PRINT(Test_CutToPrecision_Rounding);
  CALL_AND_PRINT(Test_CutToPrecision_RoundingWithIncrement);
  TEST_END;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces);
  CALL_AND_PRINT(Test_ReplaceSpacesWithHtmlCode);
  CALL_AND_PRINT(Test_SplitString);
  CALL_AND_PRINT(Test_FixedPointNumber);
  CALL_AND_PRINT(Test_CutToPrecision);
  TEST_END;
}

MAIN_TEST(Test_All)

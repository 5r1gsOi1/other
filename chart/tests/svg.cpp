
#include "chart/svg.h"
#include <dirent.h>
#include <iostream>
#include <vector>
#include "common/tests.h"

bool Test_Color_OperatorString() {
  svg::attributes::Color c{0x12, 0x34, 0x56};
  std::cout << c.operator std::string() << std::endl;
  return c.operator std::string() == "#123456";
}

bool Test_Color_DefaultZero() {
  svg::attributes::Color c;
  return c.r == 0 and c.g == 0 and c.b == 0;
}

bool Test_Color() {
  TEST_START;
  CALL_AND_PRINT(Test_Color_OperatorString);
  CALL_AND_PRINT(Test_Color_DefaultZero);
  TEST_END;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_Color);
  TEST_END;
}

MAIN_TEST(Test_All)

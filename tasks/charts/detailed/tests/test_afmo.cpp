
#include <dirent.h>
#include <iostream>
#include <vector>
#include "common/tests.h"
#include "tasks/charts/detailed/afmo_detailed_chart.h"


bool Test_ParseMainPage() {
  bool result{false};



  return result;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_ParseMainPage);
  TEST_END;
}

MAIN_TEST(Test_All)

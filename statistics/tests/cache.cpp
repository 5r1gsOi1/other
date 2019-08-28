
#include "statistics/cache.h"
#include <vector>
#include "common/simple_date.h"
#include "common/tests.h"
/*
bool Test_CacheIndex_Simple() {
  const std::string file_name = "fake file name";
  { volatile std::ofstream vvv(file_name); }
  CacheIndex<WikiPageDateTimestamp> index(file_name);
  index.Add(WikiPageDateTimestamp{"name", Date{2012, 10, 1}});
  index.Add(WikiPageDateTimestamp{"name2", Date{2012, 10, 1}});
  index.Add(WikiPageDateTimestamp{"name3", Date{2012, 10, 1}});
  index.Add(WikiPageDateTimestamp{"name", Date{2012, 10, 1}});
  index.Add(WikiPageDateTimestamp{"name5", Date{2012, 10, 1}});
  index.Add(WikiPageDateTimestamp{"name5", Date{2012, 10, 1}});
  index.Add(WikiPageDateTimestamp{"name", Date{2012, 10, 1}});
  auto found = index.Get([](const WikiPageDateTimestamp& element) {
    return element.name == "name";
  });
  for (auto& i : found) {
    std::cout << i.name << " == " << i.date << std::endl;
  }
  return true;
}

bool Test_CacheIndex() {
  TEST_START;
  CALL_AND_PRINT(Test_CacheIndex_Simple);
  TEST_END;
}
*/
bool Test_All() {
  TEST_START;
  //CALL_AND_PRINT(Test_CacheIndex);
  TEST_END;
}

MAIN_TEST(Test_All)

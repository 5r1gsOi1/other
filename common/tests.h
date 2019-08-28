
#ifndef TESTS_BASIC_
#define TESTS_BASIC_

#include <iostream>
#include <string>

inline std::string::size_type _global_test_indent = 0;

#define TEST_START                                        \
  const std::ios::fmtflags _old_flags(std::cout.flags()); \
  std::cout << std::boolalpha;                            \
  _global_test_indent += 2;                               \
  bool _succeeded = true;

#define TEST_END               \
  std::cout.flags(_old_flags); \
  _global_test_indent -= 2;    \
  return _succeeded;

#define PRINT_RESULT(result) \
  (result ? "\033[1;32mtrue\033[0m" : "\033[1;41mfalse\033[0m")

#define CALL_AND_PRINT(f)                                             \
  {                                                                   \
    bool _result = f();                                               \
    _succeeded &= _result;                                            \
    std::cout << std::string(_global_test_indent, ' ') << #f << " : " \
              << PRINT_RESULT(_result) << std::endl;                  \
  }

#define MAIN_TEST(f)                                                          \
  int TestAll() {                                                            \
    bool was_successful = f();                                        \
    std::cout << "Test_All : " << PRINT_RESULT(was_successful) << std::endl; \
    return was_successful ? 0 : -1;                                          \
  }                                                                          \
  int main() { return TestAll(); }

#endif

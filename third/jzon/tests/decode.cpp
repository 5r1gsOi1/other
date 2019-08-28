
#include <dirent.h>
#include <iostream>
#include <vector>
#include "common/tests.h"
#include "third/jzon/jzon.h"

//

const std::vector<std::string> passes = {
    R"([
        "JSON Test Pattern pass1",
        {"object with 1 member":["array with 1 element"]},
        {},
        [],
        -42,
        true,
        false,
        null,
        {
            "integer": 1234567890,
            "real": -9876.543210,
            "e": 0.123456789e-12,
            "E": 1.234567890E+34,
            "":  23456789012E66,
            "zero": 0,
            "one": 1,
            "space": " ",
            "quote": "\"",
            "backslash": "\\",
            "controls": "\b\f\n\r\t",
            "slash": "/ & \/",
            "alpha": "abcdefghijklmnopqrstuvwyz",
            "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
            "digit": "0123456789",
            "0123456789": "digit",
            "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
            "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
            "true": true,
            "false": false,
            "null": null,
            "array":[  ],
            "object":{  },
            "address": "50 St. James Street",
            "url": "http://www.JSON.org/",
            "comment": "// /* <!-- --",
            "# -- --> */": " ",
            " s p a c e d " :[1,2 , 3

    ,

    4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
            "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
            "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
            "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
    : "A key can be any string"
        },
        0.5 ,98.6
    ,
    99.44
    ,

    1066,
    1e1,
    0.1e1,
    1e-1,
    1e00,2e+00,2e-00
    ,"rosebud"]
    )",
    R"([[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]])",
    R"({
                          "JSON Test Pattern pass3": {
                              "The outermost value": "must be an object or array.",
                              "In this test": "It is an object."
                          }
                      })"};

const std::vector<std::string> fails = {
    R"("A JSON payload should be an object or array, not a string.")",
    R"(["Unclosed array")",
    R"({unquoted_key: "keys must be quoted"})",
    R"(["extra comma",])",
    R"(["double extra comma",,])",
    R"([, "<-- missing value"])",
    R"(["Comma after the close"],)",
    R"(["Extra close"]])",
    R"({"Extra comma": true,})",
    R"({"Extra value after close": true} "misplaced quoted value")",
    R"({"Illegal expression": 1 + 2})",
    R"({"Illegal invocation": alert()})",
    R"({"Numbers cannot be hex": 0x14})",
    R"(["Illegal backslash escape: \x15"])",
    R"([\naked])",
    R"(["Illegal backslash escape: \017"])",
    R"({"Missing colon" null})",
    R"({"Double colon":: null})",
    R"({"Comma instead of colon", null})",
    R"(["Colon instead of comma": false])",
    R"(["Bad value", truth])",
    R"(['single quote'])",
    R"([0e])",
    R"([0e+])",
    R"([0e+-1])",
    R"({"Comma instead if closing brace": true,)",
    R"(["mismatch"})"};

bool Test_BackSlash() {
  jzon::Parser parser;
  jzon::Node node = parser.parseString(R"({"test":"\\\\\\\"\\\\\""})");
  return node.isValid() and node.get("test").toString() == R"(\\\"\\")";
}

bool Test_SingleStringFunction(const std::string& string) {
  jzon::Parser parser;
  jzon::Node node = parser.parseString(string);
  if (!node.isValid()) {
    std::cerr << parser.getError() << std::endl;
    return false;
  }
  return true;
}

class Test_SingleString {
 public:
  Test_SingleString(const std::string& string) : string_(string) {}
  bool operator()() { return Test_SingleStringFunction(string_); }

 private:
  std::string string_;
};

bool Test_PassFiles() {
  TEST_START;
  for (auto& i : passes) {
    CALL_AND_PRINT(Test_SingleString(i));
  }
  TEST_END;
}

bool Test_FailFiles() {
  TEST_START;
  CALL_AND_PRINT(Test_SingleString(fails[5]));
  for (auto& i : fails) {
    //CALL_AND_PRINT(not Test_SingleString(i));
  }
  TEST_END;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_BackSlash);
  CALL_AND_PRINT(Test_PassFiles);
  CALL_AND_PRINT(Test_FailFiles);
  TEST_END;
}

MAIN_TEST(Test_All)

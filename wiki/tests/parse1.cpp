
#include <fstream>
#include <iostream>
#include "common/tests.h"
#include "wiki/parse.h"
#include "wiki/timestamp.h"

bool Test_RemoveTagBodies_SingleCall(const std::string& tag_name,
                                     const std::string& string,
                                     const std::string& expected_result) {
  std::string local_copy = string;
  RemoveTagBodies(tag_name, local_copy);
  return expected_result == local_copy;
}

bool Test_RemoveRepeatingSpaces_Simple() {
  return Test_RemoveTagBodies_SingleCall(
      "small", "123 <small> 123 </small> 123", "123  123  123");
}

bool Test_RemoveTagBodies() {
  TEST_START;
  CALL_AND_PRINT(Test_RemoveRepeatingSpaces_Simple);
  TEST_END;
}

bool Test_ContainsTagStart_SingleCall(const std::string& string,
                                      const std::string& tag,
                                      const bool expected_result) {
  auto result = ContainsTagStart(string, tag);
  return expected_result == result;
}

bool Test_ContainsTagStart_Empty() {
  return Test_ContainsTagStart_SingleCall("", "", false);
}

bool Test_ContainsTagStart_AtBegin() {
  return Test_ContainsTagStart_SingleCall("tag", "<tag>  texte t trte", true);
}

bool Test_ContainsTagStart_AtEnd() {
  return Test_ContainsTagStart_SingleCall("tag", "texte t trte  <tag>", true);
}

bool Test_ContainsTagStart_InCenter() {
  return Test_ContainsTagStart_SingleCall("tag", "text <tag> text", true);
}

bool Test_ContainsTagStart_EndTag() {
  return Test_ContainsTagStart_SingleCall("tag", "text </tag> text", false);
}

bool Test_PageIsStrokeAway() {
  return wiki::parse::SectionTitleIsStrickenOut("  < s  >  <  / s >  ") and
         not wiki::parse::SectionTitleIsStrickenOut("  < s > /s >  ") and
         wiki::parse::SectionTitleIsStrickenOut("<s>12345</s>") and
         wiki::parse::SectionTitleIsStrickenOut("<s></s>");
}

bool Test_RemoveStrikeOutTags() {
  return wiki::parse::RemoveStrikeOutTags("  < s  > 123 <  / s >  ") ==
             " 123 " and
         wiki::parse::RemoveStrikeOutTags("  < s > 456 /s >  ") ==
             "  < s > 456 /s >  " and
         wiki::parse::RemoveStrikeOutTags("<s></s>") == "" and
         wiki::parse::RemoveStrikeOutTags("123") == "123" and
         wiki::parse::RemoveStrikeOutTags("") == "";
}

bool Test_RemoveLinkBrackets() {
  return wiki::parse::RemoveLinkBrackets("  [[test]]  ") == "test" and
         wiki::parse::RemoveLinkBrackets("  [[:123]]   ") == "123" and
         wiki::parse::RemoveLinkBrackets("[[]]") == "" and
         wiki::parse::RemoveLinkBrackets("123") == "123" and
         wiki::parse::RemoveLinkBrackets("") == "";
}

std::optional<std::string> GetLastUserNameFromString(const std::string& text) {
  for (auto end{text.rfind("]]")}, start{text.rfind("[[", end)};
       end != std::string::npos and start != std::string::npos;
       end = text.rfind("]]", end - 1), start = text.rfind("[[", end)) {
    auto full_link{text.substr(start + 2, end - start - 2)};
    if (not full_link.empty()) {
      auto middle{full_link.find("|")};
      if (middle != std::string::npos) {
        auto colon{full_link.find(":")};
        if (colon != std::string::npos and colon < middle) {
          auto link_prefix{full_link.substr(0, colon)};
          if (link_prefix == "u" or link_prefix == "U" or link_prefix == "у" or
              link_prefix == "У" or link_prefix == "user" or
              link_prefix == "User" or link_prefix == "Участник" or
              link_prefix == "участник") {
            auto username{full_link.substr(colon + 1, middle - colon - 1)};
            return std::optional<std::string>{username};
          }
        }
      }
    }
  }
  return std::nullopt;
}

std::optional<wiki::parse::UserMessage> GetUserFromAutoSummary(
    const std::string& text) {
  const std::string start_text{"Страница была удалена "},
      timestamp_end_marker{" участником "},
      after_timestamp_text{". Была указана следующая причина: «"},
      message_end_marker{"». Данное сообщение было автоматически"};

  auto timestamp_start{start_text.size()},
      timestamp_end{text.find(timestamp_end_marker, timestamp_start)};
  if (timestamp_end != std::string::npos) {
    auto timestamp{
        text.substr(timestamp_start, timestamp_end - timestamp_start)};
    wiki::Timestamp normal_timestamp{wiki::Timestamp::FromString(timestamp)};
    bool ts_is_valid{not normal_timestamp.IsNull()};
    auto user_name_end{text.find(after_timestamp_text,
                                 timestamp_end + timestamp_end_marker.size())};
    if (ts_is_valid and user_name_end != std::string::npos) {
      auto username_start{timestamp_end + timestamp_end_marker.size()};
      auto username{
          text.substr(username_start, user_name_end - username_start)};
      auto message_end{text.find(message_end_marker,
                                 user_name_end + after_timestamp_text.size())};
      if (message_end != std::string::npos) {
        auto message_start{user_name_end + after_timestamp_text.size()};
        auto message{text.substr(message_start, message_end - message_start)};
        auto pure_username{GetLastUserNameFromString(username)};
        if (pure_username.has_value()) {
          return wiki::parse::UserMessage{
              pure_username.value(), message,
              static_cast<std::string>(normal_timestamp)};
        }
      }
    }
  }
  return std::nullopt;
}

bool Test_ContainsTagStart() {
  TEST_START;
  CALL_AND_PRINT(Test_ContainsTagStart_Empty);
  CALL_AND_PRINT(Test_ContainsTagStart_AtBegin);
  CALL_AND_PRINT(Test_ContainsTagStart_AtEnd);
  CALL_AND_PRINT(Test_ContainsTagStart_InCenter);
  CALL_AND_PRINT(Test_ContainsTagStart_EndTag);
  CALL_AND_PRINT(Test_PageIsStrokeAway);
  CALL_AND_PRINT(Test_RemoveStrikeOutTags);
  CALL_AND_PRINT(Test_RemoveLinkBrackets);

  auto text{
      "Из [[Википедия:Инкубатор|Инкубатора]]. Просрочено на "
      "[[Проект:ИНК-МР|мини-рецензировании]]. Нужно оценить допустимость "
      "переноса статьи в основное пространство. "
      "<small>([https://ru.wikipedia.org/?oldid=82336522 подробнее]). "
      "[[Участник:Dibot/mrKU|Данное сообщение]] и номинация выполнены в "
      "[[Участник:Dibot/MRitog|автоматическом режиме]] ботом.</small> "
      "[[У:Dibоt12|Dibоt]] ([[ОУ:Dibоt|обс.]]) 00:45, 31 декабря 2018 (UTC)"};

  auto tt{GetLastUserNameFromString(text)};
  if (tt.has_value()) {
    std::cout << tt.value() << std::endl;
  } else {
    std::cout << "no value\n";
  }

  /*
  wiki::Timestamp ts;
  ts.FromString("23:25, 31 декабря 2018 (UTC)");
  std::cout << static_cast<std::string>(ts) << std::endl;

  std::cout << int(u'Р') << std::endl;

  std::ofstream f("/media/sf_old_d/test.txt");
  std::string s{"Редактирование"};
  for (auto c : s) {
    f << c << " ";
  }//*/

  TEST_END;
}

bool Test_All() {
  TEST_START;
  CALL_AND_PRINT(Test_RemoveTagBodies);
  CALL_AND_PRINT(Test_ContainsTagStart);
  TEST_END;
}

MAIN_TEST(Test_All)

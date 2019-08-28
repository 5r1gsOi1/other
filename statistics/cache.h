
#ifndef STATISTICS_CACHE_H_
#define STATISTICS_CACHE_H_

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "common/basic.h"
#include "common/simple_date.h"


#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "=" VALUE(var)

//#pragma message(VAR_NAME_VALUE(__cplusplus))



/*
// template

template <class Identifier, class Data>
class SingleFileCacher : public AbstractCacher<Identifier, Data> {
 public:
  SingleFileCacher(const std::string& file_name) : file_name_(file_name) {
    CreateDirectoryIfNotExists(
        file_name.substr(0, file_name.find_last_of('/')));
    number_of_changes_ = 0;
    LoadFromFile();
  }
  virtual ~SingleFileCacher() { SaveToFile(); }

 protected:
  std::string file_name_;
  std::map<Identifier, std::pair<bool, Data>> elements_;
  std::size_t number_of_changes_;
  static constexpr int number_of_changes_before_save = 50;

 protected:
  void IncrementAndSave() {
    ++number_of_changes_;
    if (number_of_changes_ >= number_of_changes_before_save) {
      number_of_changes_ = 0;
      SaveToFile();
    }
  }

  void SaveToFile() {
    if (not file_name_.empty()) {
      std::ofstream stream(file_name_, std::ios_base::out);
      if (stream.is_open()) {
        for (const auto& e : elements_) {
          stream << e.first << " | ";
          if (e.second.first) {
            stream << e.second.second;
          } else {
            stream << "absent";
          }
          stream << std::endl;
        }
        number_of_changes_ = 0;
      }
    }
  }

  void AddEmptyElement(const Identifier& identifier) {
    elements_[identifier] = std::make_pair(false, Data());
  }

  void AddElement(const Identifier& identifier,
                  const std::string& data_string) {
    elements_[identifier] = std::make_pair(true, Data(data_string));
  }

  void ReadLineToElements(const std::string& line) {
    auto delimiter_position = line.find_first_of("|");
    if (delimiter_position == std::string::npos) {
      throw errors::NotParsedFromString();
    }
    Identifier identifier(line.substr(0, delimiter_position));
    auto data_string = line.substr(delimiter_position + 1);
    if (RemoveRepeatingSpaces(data_string) == "absent") {
      AddEmptyElement(identifier);
    } else {
      AddElement(identifier, data_string);
    }
  }

  void LoadFromFile() {
    if (not file_name_.empty()) {
      std::ifstream stream(file_name_, std::ios_base::in);
      if (stream.is_open()) {
        elements_.clear();
        std::string line;
        while (std::getline(stream, line)) {
          if (line.empty()) {
            continue;
          }
          ReadLineToElements(line);
        }
        number_of_changes_ = 0;
      }
    }
  }

 protected:
  virtual bool IdentifierMarkedAsAbsent(const Identifier& identifier) override {
    auto found = elements_.find(identifier);
    if (found != elements_.end() and false == found->second.first) {
      return true;
    }
    return false;
  }

  virtual void MarkIdentifierAsAbsent(const Identifier& identifier) override {
    elements_[identifier] = std::make_pair(false, Data());
    IncrementAndSave();
  }

  virtual void PutDataToCache(const Identifier& identifier,
                              const Data& data) override {
    elements_[identifier] = std::make_pair(true, data);
    IncrementAndSave();
  }

  virtual Data GetDataFromCache(const Identifier& identifier) override {
    try {
      if (false == elements_.at(identifier).first) {
        throw errors::NoCacheForThisIdentifier();
      }
      return elements_.at(identifier).second;
    } catch (const std::out_of_range&) {
      throw errors::NoCacheForThisIdentifier();
    }
  }

  virtual Data GetData(const Identifier& identifier) override = 0;
};

//*/
/*
template <class Identifier, class ParsedData>
class CacheDaddy {
 public:
  struct DataWithIdentifier {
    Identifier identifier;
    ParsedData data;
  };

  CacheDaddy() = delete;
  CacheDaddy(const Server<Identifier>* server,
             const Parser<Identifier, ParsedData>* parser,
             const std::string& folder_path)
      : server_(server),
        parser_(parser),
        server_cache_(folder_path),
        absent_pages_index_(folder_path + "/_!_absent_pages.txt"),
        parser_cache_(folder_path + "/_!_values_from_parser.txt") {}
  virtual ~CacheDaddy() = default;

  bool get(const Identifier& identifier, ParsedData& data,
           const bool force_download = false) {
    if (force_download) {
      std::string raw_data;
      return GetDataFromServerAndCacheIt(identifier, raw_data) and
             ParseDataAndCachesResult(identifier, raw_data, data);
    } else {
      if (GetDataFromParserCache(identifier, data)) {
        return true;
      } else if (not IsAbsentOnServer(identifier)) {
        std::string raw_data;
        if (GetDataFromServerCache(identifier, raw_data) or
            GetDataFromServerAndCacheIt(identifier, raw_data)) {
          return ParseDataAndCachesResult(identifier, raw_data, data);
        }
      }
    }
    return false;
  }

 protected:
  Server<Identifier>* server_;
  Parser<Identifier, ParsedData>* parser_;
  FolderCache<std::string> server_cache_;
  CacheIndex<Identifier> absent_pages_index_;
  CacheIndex<DataWithIdentifier> parser_cache_;

  bool GetDataFromParserCache(const Identifier& identifier, ParsedData& data) {
    auto data_from_cache = parser_cache_.Get([identifier](
        const DataWithIdentifier& id) { return id.identifier == identifier; });
    if (data_from_cache.size() > 1) {
      throw MoreThanOneDataForOneIdentifier();
    }
    bool there_is_data = not data_from_cache.empty();
    if (there_is_data) {
      data = data_from_cache.at(0).data;
    }
    return there_is_data;
  }

  bool GetDataFromServerCache(const Identifier& identifier, std::string& data) {
    return server_cache_.get(identifier, data);
  }

  bool IsAbsentOnServer(const Identifier& identifier) {
    auto absent_pages = absent_pages_index_.Get(
        [identifier](const Identifier& id) { return id == identifier; });
    return not absent_pages.empty();
  }

  bool GetDataFromServerAndCacheIt(const Identifier& identifier,
                                   std::string& data) {
    return GetDataFromServer(identifier, data) and
           server_cache_.put(identifier, data);
  }

  bool ParseDataAndCachesResult(const Identifier& identifier,
                                const std::string& raw_data,
                                ParsedData& parsed_data) {
    return ParseDataFromServer(identifier, raw_data, parsed_data) and
           parser_cache_.Add(parsed_data);
  }

  bool ParseDataFromServer(const Identifier& identifier,
                           const std::string& raw_data,
                           ParsedData& parsed_data) {
    if (parser_) {
      return parser_->parse(identifier, raw_data, parsed_data);
    } else {
      return false;
    }
  }

  bool GetDataFromServer(const Identifier& identifier, std::string& data) {
    if (server_) {
      return server_->get(identifier, data);
    } else {
      return false;
    }
  }
};//*/

#endif

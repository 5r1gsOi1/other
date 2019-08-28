
#pragma once

#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <vector>

template <class T>
struct FileIndexOneValue {
  FileIndexOneValue(const std::string& file_name) : file_name_(file_name) {
    load();
  }
  ~FileIndexOneValue() { save(); }

  virtual bool contains(const T& element) {
    return 1 <= storage_.count(element);
  }
  virtual bool empty() const { return storage_.empty(); }
  virtual std::size_t size() const { return storage_.size(); }

  virtual bool add(const T& new_element) {
    auto result{storage_.insert(new_element).second};
    IncrementChangesAndSave();
    return result;
  }

  template <class PredicateT>
  std::vector<T> find(PredicateT predicate) {
    std::vector<T> result;
    for (auto& e : this->storage_) {
      if (predicate(e)) {
        result.push_back(e);
      }
    }
    return result;
  }

  virtual void save() {
    if (not file_name_.empty()) {
      std::ofstream stream(file_name_, std::ios_base::out);
      if (stream.is_open()) {
        for (const auto& e : this->storage_) {
          stream << std::string(e) << std::endl;
        }
        this->number_of_changes_ = 0;
      }
    }
  }

  virtual void load() {
    if (not file_name_.empty()) {
      std::ifstream stream(file_name_, std::ios_base::in);
      if (stream.is_open()) {
        this->storage_.clear();
        std::string line;
        while (std::getline(stream, line)) {
          if (line.empty()) {
            continue;
          }
          T element(line);
          this->storage_.insert(element);
        }
        this->number_of_changes_ = 0;
      }
    }
  }

 protected:
  void IncrementChangesAndSave() {
    ++number_of_changes_;
    if (number_of_changes_ >= number_of_changes_before_save) {
      number_of_changes_ = 0;
      save();
    }
  }

 protected:
  std::string file_name_;
  std::set<T> storage_;
  int number_of_changes_;
  static constexpr int number_of_changes_before_save = 50;
};

template <class Key, class Value>
struct FileIndexTwoValues {
  FileIndexTwoValues(const std::string& file_name) : file_name_(file_name) {
    load();
  }
  ~FileIndexTwoValues() { save(); }

  bool add(const Key& key, const Value& value) {
    auto result{storage_.insert(std::make_pair(key, value))};
    IncrementChangesAndSave();
    return result.second;
  }

  bool contains(const Key& key) { return 1 <= storage_.count(key); }
  bool empty() const { return storage_.empty(); }
  std::size_t size() const { return storage_.size(); }

  std::optional<Value> find(const Key& key) {
    auto it{storage_.find(key)};
    return it != storage_.end() ? std::optional<Value>(it->second)
                                : std::nullopt;
  }

  virtual void save() {
    if (not file_name_.empty()) {
      std::ofstream stream(file_name_, std::ios_base::out |
                                           std::ios_base::trunc |
                                           std::ios_base::binary);
      if (stream.is_open()) {
        for (const auto& e : this->storage_) {
          stream << std::string{e.first} << std::endl
                 << std::string{e.second} << std::endl;
        }
        this->number_of_changes_ = 0;
      }
    }
  }

  virtual void load() {
    if (not file_name_.empty()) {
      std::ifstream stream(file_name_, std::ios_base::in);
      if (stream.is_open()) {
        this->storage_.clear();
        std::string line;
        bool is_key{true};
        std::pair<Key, Value> element;
        while (std::getline(stream, line)) {
          if (line.empty()) {
            continue;
          }
          if (is_key) {
            element.first = Key{line};
          } else {
            element.second = Value{line};
            storage_.insert(std::move(element));
          }
          is_key = not is_key;
        }
        this->number_of_changes_ = 0;
      }
    }
  }

 protected:
  void IncrementChangesAndSave() {
    ++number_of_changes_;
    if (number_of_changes_ >= kNumberOfChangesBeforeSave) {
      number_of_changes_ = 0;
      save();
    }
  }

  std::map<Key, Value> storage_;
  std::string file_name_;
  int number_of_changes_;
  static constexpr int kNumberOfChangesBeforeSave{50};
};

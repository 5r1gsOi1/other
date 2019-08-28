
#ifndef COMMON_ANY_H_
#define COMMON_ANY_H_

#include <exception>
#include <string>

template <typename = void, typename...>
struct ToStringArgumentsMatch : std::false_type {};

template <typename... Args>
struct ToStringArgumentsMatch<
    std::void_t<decltype(std::to_string(std::declval<Args>()...))>, Args...>
    : std::true_type {};

template <typename... Args>
static inline constexpr bool CanBeArgumentOfToString =
    ToStringArgumentsMatch<void, Args...>::value;

class Any {
 public:
  Any() : content(nullptr) {}
  ~Any() { delete content; }
  Any(const Any& other) {
    this->content = other.content ? other.content->clone() : nullptr;
  }
  Any(Any&& other) {
    this->content = other.content;
    other.content = nullptr;
  }
  template <typename T>
  Any(const T& x) {
    content = new Holder(x);
  }

  const std::type_info& type_info() const {
    return content ? content->type_info() : typeid(nullptr);
  }

  template <typename T>
  T value() const {
    if (this->type_info() != typeid(T)) {
      throw std::bad_cast();
    }
    return dynamic_cast<Holder<T>*>(content)->held;
  }

  operator std::string() const {
    return content ? content->operator std::string() : std::string();
  }

  bool operator==(const Any& other) {
    return this->type_info() == other.type_info() and
           (this->content == other.content or *this->content == *other.content);
  }

  Any operator=(const Any& other) {
    if (this->content != other.content) {
      delete this->content;
      this->content = other.content ? other.content->clone() : nullptr;
    }
    return *this;
  }

 private:
  class PlaceHolder {
   public:
    virtual ~PlaceHolder() {}
    virtual const std::type_info& type_info() const = 0;
    virtual PlaceHolder* clone() const = 0;
    virtual operator std::string() const = 0;
    virtual bool operator==(const PlaceHolder& other) const {
      return this->type_info() == other.type_info();
    }
  };

  template <typename ValueType>
  class Holder : public PlaceHolder {
    friend class Any;

   public:
    Holder(const ValueType& value) : held(value) {}
    virtual const std::type_info& type_info() const override {
      return typeid(ValueType);
    }
    virtual PlaceHolder* clone() const override { return new Holder(held); }
    virtual operator std::string() const override {
      if constexpr (CanBeArgumentOfToString<ValueType>) {
        return std::to_string(held);
      } else {
        return static_cast<std::string>(held);
      }
    }
    virtual bool operator==(const Holder& other) {
      return this->held == other.held;
    }

   private:
    const ValueType held;
  };

  PlaceHolder* content;
};

#endif

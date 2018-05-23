#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstddef>
#include "clist.h"


//! g++ 8.1.0
//! -O3 -march=native

template <std::size_t SIZE> class Binary {

#define AND &&
#define BIT_AND &
#define BIT_OR |
#define NOT !
#define OR ||
#define XOR ^

 public:
  Binary() { static_assert(!((SIZE != 0) && ((SIZE & (SIZE - 1))))); }

  //! explicit constructor to avoid implicit convert from char
  explicit Binary(int32_t num) {
    for (std::size_t idx = SIZE; idx-- > 0;)
      num & (1 << (SIZE - 1 - idx)) ? bytearray[idx] = 1 : bytearray[idx] = 0;
  }

  static int32_t
  B2D(const std::array<bool, SIZE>& that) noexcept {
    int32_t decimal;
    for (std::size_t idx = 0; idx < SIZE; idx++)
      decimal = decimal << 1 BIT_OR that[idx];
    return decimal;
  }

  inline std::array<bool, SIZE>
  getField() const noexcept { return this -> bytearray; }

  friend std::ostream& operator<<(std::ostream& os, const Binary& that) {
    os << "[";
    for (const auto& bit: that.bytearray) os << bit;
    os << "] " << Binary::B2D(that.bytearray);
    return os;
  }

  Binary operator=(const Binary& that) noexcept {
    this->bytearray = that.bytearray;
    return *this;
  }
  Binary operator+(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::add(this -> bytearray, that.bytearray);
    return temp;
  }
  Binary operator-(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::sub(this -> bytearray, that.bytearray);
    return temp;
  }
  Binary operator*(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::mul(this -> bytearray, that.bytearray);
    return temp;
  }
  Binary operator/(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::div(this -> bytearray, that.bytearray);
    return temp;
  }

  // comparators
  // [ ... ] implement relational [ <, > ] operators

  friend bool
  operator==(const Binary& lhs, const Binary& rhs) {
    for (std::size_t idx = 0; idx < SIZE; ++idx) {
      if ((!(lhs.bytearray[idx] XOR rhs.bytearray[idx])) == 0)
        return false;
    }
    return true;
  }

  friend bool operator!=(const Binary& lhs, const Binary& rhs) {
    return !(lhs == rhs);
  }

//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

 private:

  std::array<bool, SIZE> bytearray;

  static bool
  ifExists(const std::array<bool, SIZE>& that,
                       const bool& param) noexcept {
    return std::find(std::begin(that), std::end(that), param) != std::end(that);
  }

  static void
  lsh(std::array<bool, SIZE>& that) noexcept
  {
    for (std::size_t idx = 0; idx < SIZE - 1; idx++)
      that[idx] = that[idx + 1];
    that[SIZE - 1] = 0;
  }

  static void
  rsh(std::array<bool, SIZE>& that) noexcept
  {
    for (std::size_t idx = SIZE - 1; idx > 0; idx--)
      that[idx] = that[idx - 1];
    that[0] = 0;
  }

  static std::array<bool, SIZE>
  add(const std::array<bool, SIZE>& lhs,
      const std::array<bool, SIZE>& rhs)
  {
    std::array<bool, SIZE> res = { 0 };
    bool rem = 0;
    for (std::size_t idx = SIZE; idx-- > 0;) {
      res[idx] = (lhs[idx] XOR rhs[idx]) XOR rem;
      rem = (lhs[idx] BIT_AND rhs[idx]) BIT_OR (rem BIT_AND (lhs[idx] XOR rhs[idx]));
    }
    return res;
  }

  static std::array<bool, SIZE>
  sub(const std::array<bool, SIZE>& lhs,
      const std::array<bool, SIZE>& rhs)
  {
    std::array<bool, SIZE> res = { 0 };
    bool rem = 0;
    for (std::size_t idx = SIZE; idx-- > 0;) {
      res[idx] = rem XOR (lhs[idx] XOR rhs[idx]);
      rem = (not lhs[idx] BIT_AND rhs[idx]) or (not lhs[idx] BIT_AND rem) or (rhs[idx] BIT_AND rem);
    }
    return res;
  }

  static std::array<bool, SIZE>
  mul(std::array<bool, SIZE> lhs,
      std::array<bool, SIZE> rhs)
  {
    std::array<bool, SIZE> res = { 0 };
    for (; ifExists(lhs, 1);)
    {
      if (rhs[SIZE - 1] BIT_AND 1)
        res = Binary::add(res, lhs);
      rsh(rhs); lsh(lhs);
    }
    return res;
  }

  [[deprecated("... doesn't work for non-integer results")]]
  static std::array<bool, SIZE>
  div(std::array<bool, SIZE> lhs,
      std::array<bool, SIZE> rhs)
  {
    std::array<bool, SIZE> res = { 0 };
    for (; ifExists(lhs, 1); ) {
       lhs = sub(lhs, rhs);
       res = add(res, Binary(1).getField());
    }
    return res;
  }
};

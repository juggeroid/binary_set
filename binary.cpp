#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

template <std::size_t SIZE> class Binary {

  using bytearray_t = std::array<bool, SIZE>;

 public:

   /*
    *  Binary:  a basic templated class made solely for educational purposes
    *  TODO:    division (with remainder?), rot operations,
    *           overflow exceptions, bits conveying
    *
    *           tested under Linux using g++ 8.1.0;
    *           -O3 -march=native --std=c++14
    */
  
  static_assert(SIZE > 0,                     "zero and negative values are not allowed");
  static_assert(SIZE <= 32,                   "size must not exceed 32");
  static_assert(SIZE && !(SIZE & (SIZE - 1)), "size must be a power of 2");

  Binary() {}

  //! explicit constructor to avoid implicit convert from char
  explicit Binary(int32_t num) {
    for (std::size_t idx = SIZE; idx-- > 0;) {
      bytearray[idx] = num & 0x01;
      num >>= 1;
    }
  }

  static int32_t B2D(const bytearray_t& that) noexcept {
    int32_t decimal = 0;
    for (std::size_t idx = 0; idx < SIZE; ++idx)
      decimal = decimal << 1 | that[idx];
    return decimal;
  }

  bytearray_t getField() const noexcept { return this->bytearray; }

  friend std::ostream& operator<<(std::ostream& os, const Binary& that) {
    os << "[";
    for (const auto& bit : that.bytearray) os << bit;
    os << "] -> " << Binary::B2D(that.bytearray);
    return os;
  }

  Binary operator=(const Binary& that) noexcept
  { bytearray = that.bytearray; }

  Binary operator+(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::add(this->bytearray, that.bytearray);
    return temp;
  }

  Binary operator-(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::sub(this->bytearray, that.bytearray);
    return temp;
  }

  Binary operator*(const Binary& that) noexcept {
    Binary temp = *this;
    temp.bytearray = Binary::mul(this->bytearray, that.bytearray);
    return temp;
  }
  
  bool operator==(const Binary& arg) const noexcept
  { return (bytearray == arg.bytearray); }

  bool operator!=(const Binary& arg) const noexcept
  { return !(bytearray == arg.bytearray); }

  //==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

 private:

  bytearray_t bytearray;
  
  static bytearray_t add(const bytearray_t& lhs, const bytearray_t& rhs) {
    bytearray_t res = {0};
    bool rem = 0;
    for (std::size_t idx = SIZE; idx-- > 0;) {
      res[idx] = (lhs[idx] ^ rhs[idx])^ rem;
      rem = (lhs[idx] & rhs[idx]) | (rem & (lhs[idx] ^ rhs[idx]));
    }
    return res;
  }

  static bytearray_t sub(const bytearray_t& lhs, const bytearray_t& rhs) {
    bytearray_t res = {0};
    bool rem = 0;
    for (std::size_t idx = SIZE; idx-- > 0;) {
      res[idx] = rem ^ (lhs[idx] ^ rhs[idx]);
      rem = (! lhs[idx] & rhs[idx]) || (! lhs[idx] & rem) || (rhs[idx] & rem);
    }
    return res;
  }

  static bytearray_t mul(bytearray_t lhs, bytearray_t rhs) {
    bytearray_t res = {0};
    while (std::find(std::begin(lhs), std::end(lhs), 1) != std::end(lhs)) {
      if (rhs[SIZE - 1] & 1)
        res = Binary::add(res, lhs);
      //! @note right and left shifting
      std::rotate(std::rbegin(rhs), std::rbegin(rhs) + 1, std::rend(rhs));
      std::rotate(std::begin(lhs), std::begin(lhs) + 1, std::end(lhs));
      rhs[0] = 0; lhs[SIZE - 1] = 0;
    }
    return res;
  }

};

int main() { std::cout << (Binary<16>(8) << 2) << "\n"; }

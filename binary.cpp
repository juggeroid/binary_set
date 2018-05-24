#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

/*  g++ 8.1.0: compile parameters -O3 -march=native
 *  has not yet been tested under MSVC, although there are no specific features used
 */

template <std::size_t SIZE> class Binary {

  using bytearray_t = std::array<bool, SIZE>;

 public:

  static_assert(SIZE > 0, "zero and negative values are not allowed");
  static_assert(SIZE <= 32, "size must not exceed 32");
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

  inline bytearray_t getField() const noexcept { return this->bytearray; }

  friend std::ostream& operator<<(std::ostream& os, const Binary& that) {
    os << "[";
    for (const auto& bit : that.bytearray) os << bit;
    os << "] -> " << Binary::B2D(that.bytearray);
    return os;
  }

  Binary operator=(const Binary& that) noexcept {
    this->bytearray = that.bytearray;
    return *this;
  }

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

  // bit-shifting overloading
  Binary operator<<(unsigned shift) {
    for (std::size_t i = 0; i < shift; ++i) lsh(bytearray);
    return *this;
  }

  Binary operator>>(unsigned shift) {
    for (std::size_t i = 0; i < shift; ++i) rsh(bytearray);
    return *this;
  }

  friend bool operator==(const Binary& lhs, const Binary& rhs) {
    for (std::size_t idx = 0; idx < SIZE; ++idx) {
      if (lhs.bytearray[idx] ^ rhs.bytearray[idx])
        return false;
    }
    return true;
  }

  friend bool operator!=(const Binary& lhs, const Binary& rhs) {
    return !(lhs == rhs);
  }

  //==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

 private:

  bytearray_t bytearray;

  static bool ifExists(const bytearray_t& that, const bool& param) noexcept {
    return std::find(std::begin(that), std::end(that), param) != std::end(that);
  }

  static void lsh(bytearray_t& that) noexcept {
    for (std::size_t idx = 0; idx < SIZE - 1; idx++) that[idx] = that[idx + 1];
    that[SIZE - 1] = 0;
  }

  static void rsh(bytearray_t& that) noexcept {
    for (std::size_t idx = SIZE - 1; idx > 0; idx--) that[idx] = that[idx - 1];
    that[0] = 0;
  }

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
    for (; ifExists(lhs, 1); ) {
      if (rhs[SIZE - 1] & 1) res = Binary::add(res, lhs);
      rsh(rhs); lsh(lhs);
    }
    return res;
  }

};

int main() { std::cout << (Binary<16>(8) << 2) << "\n"; }

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

template <std::size_t SIZE = 32> class Binary {

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

  // explicit constructor to avoid implicit convert from char
  explicit Binary(int64_t num) {
    for (std::size_t idx = SIZE; idx-- > 0;) {
      bytearray[idx] = num & 0x01;
      num >>= 1;
    }
  }

  Binary(Binary const& arg)
  { bytearray = arg.bytearray; }

  //! @note uint32_t ???
  int32_t to_int() noexcept {
    int32_t decimal = 0;
    for (std::size_t idx = 0; idx < SIZE; ++idx)
      decimal = decimal << 1 | bytearray[idx];
    return decimal;
  }

  Binary& operator+=(Binary const& arg) {
    add_to(this -> bytearray, arg.bytearray);
    return *this;
  }

  Binary& operator-=(Binary const& arg) {
    sub_from(this -> bytearray, arg.bytearray);
    return *this;
  }

  Binary operator*(const Binary& that) noexcept {
    Binary temp = *this;
    Binary::mul(this->bytearray, that.bytearray);
    return temp;
  }

  //! @note Allow to access individual bits by its index.

  auto& operator[](std::size_t idx)
  { return bytearray[idx]; }

  auto& operator[](std::size_t idx) const
  { return bytearray[idx]; }

  bytearray_t& getByteArray() const noexcept
  { return bytearray; }


  //! @note If there's a necessity in precise bitwise comparison, define xnor function instead?
  //! First byte should be checked to prevent incorrect result for negative values.

  bool operator>(Binary const& arg) const {
    for (std::size_t idx = 1; idx < SIZE; ++idx) {
      if (bytearray[idx] > arg.bytearray[idx]) return 1;
      if (bytearray[idx] < arg.bytearray[idx]) return 0;
    }
  }

  bool operator<(Binary const& arg) const
  { return (arg > *this); }

  bool operator>=(Binary const& arg) const
  { return !(*this < arg); }

  bool operator<=(Binary const& arg) const
  { return !(*this > arg); }

  bool operator==(Binary const& arg) const noexcept
  { return (bytearray == arg.bytearray); }

  bool operator!=(Binary const& arg) const noexcept
  { return bytearray != arg.bytearray; }

  //! @note std::ostream operator overloading (<<)
  friend std::ostream& operator<<(std::ostream& os, const Binary<SIZE>& arg) noexcept {
    os << "[";
    for (const auto& bit: arg.bytearray)
      os << bit;
    os << "]";
  return os;
  }
  
  //==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

 private:

  bytearray_t bytearray;
  
  static bool add_to(bytearray_t& lhs, bytearray_t const& rhs) noexcept
  {
    bool carry = 0;
    for (std::size_t idx = SIZE; idx-- > 0;) {
      auto temp = ((lhs[idx] ^ rhs[idx]) ^ carry);
      carry = (lhs[idx] & rhs[idx]) | (carry & (lhs[idx] ^ rhs[idx]));
      lhs[idx] = temp;
    }
    return carry;
  }

  static std::tuple<bytearray_t, bool> add(bytearray_t lhs, bytearray_t const& rhs) noexcept
  {
      auto carry = add_to(lhs, rhs);
      return std::tuple<bytearray_t, bool> {lhs, carry};
  }

  static bool sub_from(bytearray_t& lhs, bytearray_t const& rhs) {
    bool carry = 0;
    for (std::size_t idx = SIZE; idx-- > 0;) {
      auto temp = carry ^ (lhs[idx] ^ rhs[idx]);
      carry = (! lhs[idx] & rhs[idx]) || (! lhs[idx] & carry) || (rhs[idx] & carry);
      lhs[idx] = temp;
    }
    return carry;
  }

  static std::tuple<bytearray_t, bool> sub(bytearray_t lhs, bytearray_t const& rhs) noexcept
  {
      auto carry = sub_from(lhs, rhs);
      return std::tuple<bytearray_t, bool> {lhs, carry};
  }

  static bytearray_t mul(bytearray_t lhs, bytearray_t rhs) {
     bytearray_t res = {0};
    while (std::find(std::begin(lhs), std::end(lhs), 1) != std::end(lhs)) {
       if (rhs[SIZE - 1] & 1) {
         Binary::add_to(res, lhs);
        }
      //! @note Shift to the right/left.
      //!       std::copy with index shifted might be faster, although no benchmarks were conducted.
      std::rotate(std::rbegin(rhs), std::rbegin(rhs) + 1, std::rend(rhs));
      std::rotate(std::begin(lhs), std::begin(lhs) + 1, std::end(lhs));
      rhs[0] = 0; lhs[SIZE - 1] = 0;
    }
    return res;
  }
  
};

template <std::size_t SIZE>
Binary<SIZE> operator+(Binary<SIZE> a, Binary<SIZE> const& b) noexcept
{ a += b; return a; }

template <std::size_t SIZE>
Binary<SIZE> operator-(Binary<SIZE> a, Binary<SIZE> const& b) noexcept
{ a -= b; return a; }

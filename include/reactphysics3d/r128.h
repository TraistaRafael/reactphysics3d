/*
r128.h: 128-bit (64.64) signed fixed-point arithmetic. Version 1.5.0

COMPILATION
-----------
Drop this header file somewhere in your project and include it wherever it is
needed. There is no separate .c file for this library. To get the code, in ONE
file in your project, put:

#define R128_IMPLEMENTATION

before you include this file. You may also provide a definition for R128_ASSERT
to force the library to use a custom assert macro.

COMPILER/LIBRARY SUPPORT
------------------------
This library requires a C89 compiler with support for 64-bit integers. If your
compiler does not support the long long data type, the R128_U64, etc. macros
must be set appropriately. On x86 and x64 targets, Intel intrinsics are used
for speed. If your compiler does not support these intrinsics, you can add
#define R128_STDC_ONLY
in your implementation file before including r128.h.

The only C runtime library functionality used by this library is <assert.h>.
This can be avoided by defining an R128_ASSERT macro in your implementation
file. Since this library uses 64-bit arithmetic, this may implicitly add a
runtime library dependency on 32-bit platforms.

C++ SUPPORT
-----------
Operator overloads are supplied for C++ files that include this file. Since all
C++ functions are declared inline (or static inline), the R128_IMPLEMENTATION
file can be either C++ or C.

LICENSE
-------
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef H_R128_H
#define H_R128_H

#include <stddef.h>

// 64-bit integer support
// If your compiler does not have stdint.h, add appropriate defines for these macros.
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#  define R128_S32 __int32
#  define R128_U32 unsigned __int32
#  define R128_S64 __int64
#  define R128_U64 unsigned __int64
#  define R128_LIT_S64(x) x##i64
#  define R128_LIT_U64(x) x##ui64
#else
#  include <stdint.h>
#  define R128_S32 int32_t
#  define R128_U32 uint32_t
#  define R128_S64 long long
#  define R128_U64 unsigned long long
#  define R128_LIT_S64(x) x##ll
#  define R128_LIT_U64(x) x##ull
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct R128 {
		R128_U64 lo;
		R128_U64 hi;

#ifdef __cplusplus
		R128();
		//R128(R128 &v);
		R128(double);
		R128(int);
		R128(unsigned int);
		R128(R128_S64);
		R128(R128_U64 low, R128_U64 high);

		operator double() const;
		operator float() const;
		operator R128_S64() const;
		operator int() const;
		operator bool() const;

		bool operator!() const;
		R128 operator~() const;
		R128 operator-() const;
		R128 &operator|=(const R128 &rhs);
		R128 &operator&=(const R128 &rhs);
		R128 &operator^=(const R128 &rhs);
		R128 &operator+=(const R128 &rhs);
		R128 &operator-=(const R128 &rhs);
		R128 &operator*=(const R128 &rhs);
		R128 &operator/=(const R128 &rhs);
		R128 &operator%=(const R128 &rhs);
		R128 &operator<<=(int amount);
		R128 &operator>>=(int amount);

		R128 &operator+=(double rhs);
		R128 &operator-=(double rhs);
		R128 &operator*=(double rhs);
		R128 &operator/=(double rhs);

		R128 &operator+=(float rhs);
		R128 &operator-=(float rhs);
		R128 &operator*=(float rhs);
		R128 &operator/=(float rhs);

		R128 &operator+=(int rhs);
		R128 &operator-=(int rhs);
		R128 &operator*=(int rhs);
		R128 &operator/=(int rhs);

#endif   //__cplusplus
	} R128;

	// Type conversion
	extern void r128FromInt(R128 *dst, R128_S64 v);
	extern void r128FromFloat(R128 *dst, double v);
	extern R128_S64 r128ToInt(const R128 *v);
	extern double r128ToFloat(const R128 *v);

	// Copy
	extern void r128Copy(R128 *dst, const R128 *src);

	// Sign manipulation
	extern void r128Neg(R128 *dst, const R128 *v);   // -v
	extern void r128Abs(R128* dst, const R128* v);   // abs(v)
	extern void r128Nabs(R128* dst, const R128* v);  // -abs(v)

	// Bitwise operations
	extern void r128Not(R128 *dst, const R128 *src);               // ~a
	extern void r128Or(R128 *dst, const R128 *a, const R128 *b);   // a | b
	extern void r128And(R128 *dst, const R128 *a, const R128 *b);  // a & b
	extern void r128Xor(R128 *dst, const R128 *a, const R128 *b);  // a ^ b
	extern void r128Shl(R128 *dst, const R128 *src, int amount);   // shift left by amount mod 128
	extern void r128Shr(R128 *dst, const R128 *src, int amount);   // shift right logical by amount mod 128
	extern void r128Sar(R128 *dst, const R128 *src, int amount);   // shift right arithmetic by amount mod 128

	// Arithmetic
	extern void r128Add(R128 *dst, const R128 *a, const R128 *b);  // a + b
	extern void r128Sub(R128 *dst, const R128 *a, const R128 *b);  // a - b
	extern void r128Mul(R128 *dst, const R128 *a, const R128 *b);  // a * b
	extern void r128Div(R128 *dst, const R128 *a, const R128 *b);  // a / b
	extern void r128Mod(R128 *dst, const R128 *a, const R128 *b);  // a - toInt(a / b) * b

	extern void r128Sqrt(R128 *dst, const R128 *v);  // sqrt(v)
	extern void r128Rsqrt(R128 *dst, const R128 *v); // 1 / sqrt(v)

	// Comparison
	extern int  r128Cmp(const R128 *a, const R128 *b);  // sign of a-b
	extern void r128Min(R128 *dst, const R128 *a, const R128 *b);
	extern void r128Max(R128 *dst, const R128 *a, const R128 *b);
	extern void r128Floor(R128 *dst, const R128 *v);
	extern void r128Ceil(R128 *dst, const R128 *v);
	extern int  r128IsNeg(const R128 *v); // quick check for < 0

	// String conversion
	//
	typedef enum R128ToStringSign {
		R128ToStringSign_Default,  // no sign character for positive values
		R128ToStringSign_Space,    // leading space for positive values
		R128ToStringSign_Plus,     // leading '+' for positive values
	} R128ToStringSign;

	// Formatting options for use with r128ToStringOpt. The "defaults" correspond
	// to a format string of "%f".
	//
	typedef struct R128ToStringFormat {
		// sign character for positive values. Default is R128ToStringSign_Default.
		R128ToStringSign sign;

		// minimum number of characters to write. Default is 0.
		int width;

		// place to the right of the decimal at which rounding is performed. If negative,
		// a maximum of 20 decimal places will be written, with no trailing zeroes.
		// (20 places is sufficient to ensure that r128FromString will convert back to the
		// original value.) Default is -1. NOTE: This is not the same default that the C
		// standard library uses for %f.
		int precision;

		// If non-zero, pads the output string with leading zeroes if the final result is
		// fewer than width characters. Otherwise, leading spaces are used. Default is 0.
		int zeroPad;

		// Always print a decimal point, even if the value is an integer. Default is 0.
		int decimal;

		// Left-align output if width specifier requires padding.
		// Default is 0 (right align).
		int leftAlign;
	} R128ToStringFormat;

	// r128ToStringOpt: convert R128 to a decimal string, with formatting.
	//
	// dst and dstSize: specify the buffer to write into. At most dstSize bytes will be written
	// (including null terminator). No additional rounding is performed if dstSize is not large
	// enough to hold the entire string.
	//
	// opt: an R128ToStringFormat struct (q.v.) with formatting options.
	//
	// Uses the R128_decimal global as the decimal point character.
	// Always writes a null terminator, even if the destination buffer is not large enough.
	//
	// Number of bytes that will be written (i.e. how big does dst need to be?):
	// If width is specified: width + 1 bytes.
	// If precision is specified: at most precision + 22 bytes.
	// If neither is specified: at most 42 bytes.
	//
	// Returns the number of bytes that would have been written if dst was sufficiently large,
	// not including the final null terminator.
	//
	extern int r128ToStringOpt(char *dst, size_t dstSize, const R128 *v, const R128ToStringFormat *opt);

	// r128ToStringf: convert R128 to a decimal string, with formatting.
	//
	// dst and dstSize: specify the buffer to write into. At most dstSize bytes will be written
	// (including null terminator).
	//
	// format: a printf-style format specifier, as one would use with floating point types.
	//    e.g. "%+5.2f". (The leading % and trailing f are optional.)
	//    NOTE: This is NOT a full replacement for sprintf. Any characters in the format string
	//       that do not correspond to a format placeholder are ignored.
	//
	// Uses the R128_decimal global as the decimal point character.
	// Always writes a null terminator, even if the destination buffer is not large enough.
	//
	// Number of bytes that will be written (i.e. how big does dst need to be?):
	// If the precision field is specified: at most max(width, precision + 21) + 1 bytes
	// Otherwise: at most max(width, 41) + 1 bytes.
	//
	// Returns the number of bytes that would have been written if dst was sufficiently large,
	// not including the final null terminator.
	//
	extern int r128ToStringf(char *dst, size_t dstSize, const char *format, const R128 *v);

	// r128ToString: convert R128 to a decimal string, with default formatting.
	// Equivalent to r128ToStringf(dst, dstSize, "%f", v).
	//
	// Uses the R128_decimal global as the decimal point character.
	// Always writes a null terminator, even if the destination buffer is not large enough.
	//
	// Will write at most 42 bytes (including NUL) to dst.
	//
	// Returns the number of bytes that would have been written if dst was sufficiently large,
	// not including the final null terminator.
	//
	extern int r128ToString(char *dst, size_t dstSize, const R128 *v);

	// r128FromString: Convert string to R128.
	//
	// The string can be formatted either as a decimal number with optional sign
	// or as hexadecimal with a prefix of 0x or 0X.
	//
	// endptr, if not NULL, is set to the character following the last character
	//   used in the conversion.
	//
	extern void r128FromString(R128 *dst, const char *s, char **endptr);

	// Constants
	extern const R128 R128_min;      // minimum (most negative) value
	extern const R128 R128_max;      // maximum (most positive) value
	extern const R128 R128_smallest; // smallest positive value
	extern const R128 R128_zero;     // zero
	extern const R128 R128_one;      // 1.0

	extern char R128_decimal;        // decimal point character used by r128From/ToString. defaults to '.'

#ifdef __cplusplus
}

#include <limits>
#include <string>

namespace std {

	static R128 sqrt(const R128 &v)
	{ 
		R128 ret;
		r128Sqrt(&ret, &v);
		return ret;
	};

	static R128 sin(const R128 &v) 
	{ 
		return R128(std::sin((double)(v))); 
	};

	static R128 asin(const R128 &v)
	{
		return R128(std::asin((double)(v)));
	};

	static R128 cos(const R128 &v) 
	{ 
		return R128(std::cos((double)(v))); 
	};

	static R128 acos(const R128 &v) 
	{ 
		return R128(std::acos((double)(v)));
	};

	static R128 atan2(const R128 &a, const R128 &b)
	{
		return R128(std::atan2((double)(a), (double) b));
	};

	static R128 abs(const R128 &v)
	{
		return R128(std::abs((double)(v)));
	};

	static std::string to_string(const R128 &v)
	{
		return std::to_string((double) v);
	};

	static R128 fmod(const R128 &a, const R128 &b)
	{
		return R128(std::fmod((double) a, (double) b));
	};

	template<>
	struct numeric_limits<R128>
	{
		static const bool is_specialized = true;

		/*static R128 min() { return R128_min; }
		static R128 max() { return R128_max; }
*/
		static R128 (std::numeric_limits<R128>::min)() { return R128_min; };
		static R128 (std::numeric_limits<R128>::max)() { return R128_max; };

		static const int digits = 127;
		static const int digits10 = 38;
		static const bool is_signed = true;
		static const bool is_integer = false;
		static const bool is_exact = false;
		static const int radix = 2;
		static R128 epsilon() throw() { return R128_smallest; }
		static R128 round_error() throw() { return R128_one; }

		static const int min_exponent = 0;
		static const int min_exponent10 = 0;
		static const int max_exponent = 0;
		static const int max_exponent10 = 0;

		static const bool has_infinity = false;
		static const bool has_quiet_NaN = false;
		static const bool has_signaling_NaN = false;
		static const float_denorm_style has_denorm = denorm_absent;
		static const bool has_denorm_loss = false;

		static R128 infinity() throw() { return R128_zero; }
		static R128 quiet_NaN() throw() { return R128_zero; }
		static R128 signaling_NaN() throw() { return R128_zero; }
		static R128 denorm_min() throw() { return R128_zero; }

		static const bool is_iec559 = false;
		static const bool is_bounded = true;
		static const bool is_modulo = true;

		static const bool traps = numeric_limits<R128_U64>::traps;
		static const bool tinyness_before = false;
		static const float_round_style round_style = round_toward_zero;
	};
}  //namespace std

inline R128::R128() {}

inline R128::R128(unsigned int v)
{
	r128FromInt(this, v);
};

inline R128::R128(double v)
{
	r128FromFloat(this, v);
}

inline R128::R128(int v)
{
	r128FromInt(this, v);
}

inline R128::R128(R128_S64 v)
{
	r128FromInt(this, v);
}

inline R128::R128(R128_U64 low, R128_U64 high)
{
	lo = low;
	hi = high;
}

inline R128::operator double() const
{
	return r128ToFloat(this);
}

inline R128::operator float() const
{
	return (float) r128ToFloat(this);
}

inline R128::operator R128_S64() const
{
	return r128ToInt(this);
}

inline R128::operator int() const
{
	return (int)r128ToInt(this);
}

inline R128::operator bool() const
{
	return lo || hi;
}

inline bool R128::operator!() const
{
	return !lo && !hi;
}

inline R128 R128::operator~() const
{
	R128 r;
	r128Not(&r, this);
	return r;
}

inline R128 R128::operator-() const
{
	R128 r;
	r128Neg(&r, this);
	return r;
}

inline R128 &R128::operator|=(const R128 &rhs)
{
	r128Or(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator&=(const R128 &rhs)
{
	r128And(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator^=(const R128 &rhs)
{
	r128Xor(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator+=(const R128 &rhs)
{
	r128Add(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator-=(const R128 &rhs)
{
	r128Sub(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator*=(const R128 &rhs)
{
	r128Mul(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator/=(const R128 &rhs)
{
	r128Div(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator%=(const R128 &rhs)
{
	r128Mod(this, this, &rhs);
	return *this;
}

inline R128 &R128::operator<<=(int amount)
{
	r128Shl(this, this, amount);
	return *this;
}

inline R128 &R128::operator>>=(int amount)
{
	r128Sar(this, this, amount);
	return *this;
}

static inline R128 operator|(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r |= rhs;
}

static inline R128 operator&(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r &= rhs;
}

static inline R128 operator^(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r ^= rhs;
}

static inline R128 operator+(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r += rhs;
}

static inline R128 operator-(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r -= rhs;
}

static inline R128 operator*(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r *= rhs;
}

static inline R128 operator/(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r /= rhs;
}

static inline R128 operator%(const R128 &lhs, const R128 &rhs)
{
	R128 r(lhs);
	return r %= rhs;
}

static inline R128 operator<<(const R128 &lhs, int amount)
{
	R128 r(lhs);
	return r <<= amount;
}

static inline R128 operator>>(const R128 &lhs, int amount)
{
	R128 r(lhs);
	return r >>= amount;
}

static inline bool operator<(const R128 &lhs, const R128 &rhs)
{
	return r128Cmp(&lhs, &rhs) < 0;
}

static inline bool operator>(const R128 &lhs, const R128 &rhs)
{
	return r128Cmp(&lhs, &rhs) > 0;
}

static inline bool operator<=(const R128 &lhs, const R128 &rhs)
{
	return r128Cmp(&lhs, &rhs) <= 0;
}

static inline bool operator>=(const R128 &lhs, const R128 &rhs)
{
	return r128Cmp(&lhs, &rhs) >= 0;
}

static inline bool operator==(const R128 &lhs, const R128 &rhs)
{
	return lhs.lo == rhs.lo && lhs.hi == rhs.hi;
}

static inline bool operator!=(const R128 &lhs, const R128 &rhs)
{
	return lhs.lo != rhs.lo || lhs.hi != rhs.hi;
}

// ---------------------------------------------------------------------
// Other operators
static inline std::ostream & operator<< (std::ostream &out, R128 const &t)
{
	out << (double)t;
	return out;
}

// ---------------------------------------------------------------------
// double R128 
static inline R128 operator+(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) + rhs;
}

static inline R128 operator-(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) - rhs;
}

static inline R128 operator*(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) * rhs;
}

static inline R128 operator/(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) / rhs;
}

static inline bool operator<(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) < rhs;
}

static inline bool operator>(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) > rhs;
}

static inline bool operator<=(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) <= rhs;
}

static inline bool operator>=(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) >= rhs;
}

static inline bool operator==(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) == rhs;
}

static inline bool operator!=(double lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) != rhs;
}

// -----------------------------------------------------------------------------------
// R128 double 

static inline R128 operator+(const R128 &rhs, double rhs_raw)
{
	return rhs + R128(rhs_raw);
}

static inline R128 operator-(const R128 &rhs, double rhs_raw)
{
	return rhs - R128(rhs_raw);
}

static inline R128 operator*(const R128 &rhs, double rhs_raw)
{
	return rhs * R128(rhs_raw);
}

static inline R128 operator/(const R128 &rhs, double rhs_raw)
{
	return rhs / R128(rhs_raw);
}

static inline bool operator<(const R128 &rhs, double rhs_raw)
{
	return rhs < R128(rhs_raw);
}

static inline bool operator>(const R128 &rhs, double rhs_raw)
{
	return rhs > R128(rhs_raw);
}

static inline bool operator<=(const R128 &rhs, double rhs_raw)
{
	return rhs <= R128(rhs_raw);
}

static inline bool operator>=(const R128 &rhs, double rhs_raw)
{
	return rhs >= R128(rhs_raw);
}

static inline bool operator==(const R128 &rhs, double rhs_raw)
{
	return rhs == R128(rhs_raw);
}

static inline bool operator!=(const R128 &rhs, double rhs_raw)
{
	return rhs != R128(rhs_raw);
}

// ---------------------------------------------------------------------
// float R128 

static inline R128 operator+(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) + rhs;
}

static inline R128 operator-(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) - rhs;
}

static inline R128 operator*(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) * rhs;
}

static inline R128 operator/(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) / rhs;
}

static inline bool operator<(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) < rhs;
}

static inline bool operator>(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) > rhs;
}

static inline bool operator<=(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) <= rhs;
}

static inline bool operator>=(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) >= rhs;
}

static inline bool operator==(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) == rhs;
}

static inline bool operator!=(float lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) != rhs;
}

// -----------------------------------------------------------------------------------
// R128 float 

static inline R128 operator+(const R128 &rhs, float rhs_raw)
{
	return rhs + R128(rhs_raw);
}

static inline R128 operator-(const R128 &rhs, float rhs_raw)
{
	return rhs - R128(rhs_raw);
}

static inline R128 operator*(const R128 &rhs, float rhs_raw)
{
	return rhs * R128(rhs_raw);
}

static inline R128 operator/(const R128 &rhs, float rhs_raw)
{
	return rhs / R128(rhs_raw);
}

static inline bool operator<(const R128 &rhs, float rhs_raw)
{
	return rhs < R128(rhs_raw);
}

static inline bool operator>(const R128 &rhs, float rhs_raw)
{
	return rhs > R128(rhs_raw);
}

static inline bool operator<=(const R128 &rhs, float rhs_raw)
{
	return rhs <= R128(rhs_raw);
}

static inline bool operator>=(const R128 &rhs, float rhs_raw)
{
	return rhs >= R128(rhs_raw);
}

static inline bool operator==(const R128 &rhs, float rhs_raw)
{
	return rhs == R128(rhs_raw);
}

static inline bool operator!=(const R128 &rhs, float rhs_raw)
{
	return rhs != R128(rhs_raw);
}
// ---------------------------------------------------------------------
// int R128 

static inline R128 operator+(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) + rhs;
}

static inline R128 operator-(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) - rhs;
}

static inline R128 operator*(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) * rhs;
}

static inline R128 operator/(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) / rhs;
}

static inline bool operator<(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) < rhs;
}

static inline bool operator>(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) > rhs;
}

static inline bool operator<=(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) <= rhs;
}

static inline bool operator>=(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) >= rhs;
}

static inline bool operator==(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) == rhs;
}

static inline bool operator!=(int lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) != rhs;
}

// -----------------------------------------------------------------------------------
// R128 int 

static inline R128 operator+(const R128 &rhs, int rhs_raw)
{
	return rhs + R128(rhs_raw);
}

static inline R128 operator-(const R128 &rhs, int rhs_raw)
{
	return rhs - R128(rhs_raw);
}

static inline R128 operator*(const R128 &rhs, int rhs_raw)
{
	return rhs * R128(rhs_raw);
}

static inline R128 operator/(const R128 &rhs, int rhs_raw)
{
	return rhs / R128(rhs_raw);
}

static inline bool operator<(const R128 &rhs, int rhs_raw)
{
	return rhs < R128(rhs_raw);
}

static inline bool operator>(const R128 &rhs, int rhs_raw)
{
	return rhs > R128(rhs_raw);
}

static inline bool operator<=(const R128 &rhs, int rhs_raw)
{
	return rhs <= R128(rhs_raw);
}

static inline bool operator>=(const R128 &rhs, int rhs_raw)
{
	return rhs >= R128(rhs_raw);
}

static inline bool operator==(const R128 &rhs, int rhs_raw)
{
	return rhs == R128(rhs_raw);
}

static inline bool operator!=(const R128 &rhs, int rhs_raw)
{
	return rhs != R128(rhs_raw);
}


//----------------------------------------------------------------------------------
// std::uint32_t R128 

static inline R128 operator+(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) + rhs;
}

static inline R128 operator-(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) - rhs;
}

static inline R128 operator*(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) * rhs;
}

static inline R128 operator/(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) / rhs;
}

static inline bool operator<(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) < rhs;
}

//static inline bool operator>(std::uint32_t lhs_raw, const R128 &rhs)
//{
//	return R128(lhs_raw) > rhs;
//}

static inline bool operator<=(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) <= rhs;
}

static inline bool operator>=(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) >= rhs;
}

static inline bool operator==(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) == rhs;
}

static inline bool operator!=(std::uint32_t lhs_raw, const R128 &rhs)
{
	return R128(lhs_raw) != rhs;
}

// -----------------------------------------------------------------------------------
// R128 std::uint32_t 

static inline R128 operator+(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs + R128(rhs_raw);
}

static inline R128 operator-(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs - R128(rhs_raw);
}

static inline R128 operator*(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs * R128(rhs_raw);
}

static inline R128 operator/(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs / R128(rhs_raw);
}

//static inline bool operator<(const R128 &rhs, std::uint32_t rhs_raw)
//{
//	return rhs < R128(rhs_raw);
//}
//
//static inline bool operator>(const R128 &rhs, std::uint32_t rhs_raw)
//{
//	return rhs > R128(rhs_raw);
//}
//
//static inline bool operator<=(const R128 &rhs, std::uint32_t rhs_raw)
//{
//	return rhs <= R128(rhs_raw);
//}

static inline bool operator>=(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs >= R128(rhs_raw);
}

static inline bool operator==(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs == R128(rhs_raw);
}

static inline bool operator!=(const R128 &rhs, std::uint32_t rhs_raw)
{
	return rhs != R128(rhs_raw);
}

// -----------------------------------------------------------------------------------

inline R128 &R128::operator+=(double rhs)
{
	return *this += R128(rhs);
}

inline R128 &R128::operator-=(double rhs)
{
	return *this -= R128(rhs);
}

inline R128 &R128::operator*=(double rhs)
{
	return *this *= R128(rhs);
}

inline R128 &R128::operator/=(double rhs)
{
	return *this /= R128(rhs);
}
//----------------------------------------------------------------------------------
inline R128 &R128::operator+=(float rhs)
{
	return *this += R128(rhs);
}

inline R128 &R128::operator-=(float rhs)
{
	return *this -= R128(rhs);
}

inline R128 &R128::operator*=(float rhs)
{
	return *this *= R128(rhs);
}

inline R128 &R128::operator/=(float rhs)
{
	return *this /= R128(rhs);
}
//----------------------------------------------------------------------------------
inline R128 &R128::operator+=(int rhs)
{
	return *this += R128(rhs);
}

inline R128 &R128::operator-=(int rhs)
{
	return *this -= R128(rhs);
}

inline R128 &R128::operator*=(int rhs)
{
	return *this *= R128(rhs);
}

inline R128 &R128::operator/=(int rhs)
{
	return *this /= R128(rhs);
}
//----------------------------------------------------------------------------------

#endif   //__cplusplus
#endif   //H_R128_H

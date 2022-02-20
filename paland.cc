///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2017-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief printf unit tests
//
///////////////////////////////////////////////////////////////////////////////

// Rewritten for nanoprintf by Charles Nicholson (charles.nicholson@gmail.com)
// A derivative work of Paland's original, so released under the MIT License.

#include <string.h>
#include <math.h>
#include <limits>
#include <string>
#include <iostream>
#include <sstream>

// The configuration flags are injected by CMakeLists.txt in the npf project.
#define NANOPRINTF_IMPLEMENTATION
#include "../../nanoprintf.h"

#if NANOPRINTF_HAVE_GCC_WARNING_PRAGMAS
  #if NANOPRINTF_CLANG
    #pragma GCC diagnostic ignored "-Wc++98-compat-pedantic"
    #ifndef __APPLE__
      #pragma GCC diagnostic ignored "-Wreserved-identifier"
    #endif
  #endif
#endif

#ifdef _MSC_VER
  #pragma warning(disable:4514) // unreferenced inline function removed
#endif

#include "../doctest.h"

#if NANOPRINTF_HAVE_GCC_WARNING_PRAGMAS
  #if NANOPRINTF_CLANG
    #pragma GCC diagnostic ignored "-Wformat-pedantic"
  #endif
  #pragma GCC diagnostic ignored "-Wold-style-cast"
  #pragma GCC diagnostic ignored "-Wpadded"
  #pragma GCC diagnostic ignored "-Wformat"
  #pragma GCC diagnostic ignored "-Wformat-nonliteral"
  #pragma GCC diagnostic ignored "-Wformat-security"
#endif

namespace {
void require_conform(char const *expected, char const *fmt, ...) {
  char buf[256];

  std::string npf_result; {
    va_list args;
    va_start(args, fmt);
    npf_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf)-1] = '\0';
    npf_result = buf;
  }

  std::string sys_result; {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf)-1] = '\0';
    sys_result = buf;
  }

  if (expected) {
    if (npf_result != std::string{expected}) { MESSAGE(sys_result); }
    REQUIRE(npf_result == std::string{expected});
  } else {
    REQUIRE(npf_result == sys_result);
  }
}
}

TEST_CASE("space flag") {
  require_conform(" 42",             "% d", 42);
  require_conform("-42",             "% d", -42);
  require_conform(" 1024",           "% d", 1024);
  require_conform("-1024",           "% d", -1024);
  require_conform(" 1024",           "% i", 1024);
  require_conform("-1024",           "% i", -1024);

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("   42",           "% 5d", 42);
  require_conform("  -42",           "% 5d", -42);
  require_conform("             42", "% 15d", 42);
  require_conform("            -42", "% 15d", -42);
  require_conform("            -42", "% 15d", -42);
#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
  require_conform("        -42.986", "% 15.3f", -42.987);
  require_conform("         42.986", "% 15.3f", 42.987);
#endif
#endif
}

TEST_CASE("space flag - non-standard format") {
  require_conform("Hello testing", "% s", "Hello testing");
  require_conform("1024",          "% u", 1024);
  require_conform("4294966272",    "% u", 4294966272U);
  require_conform("777",           "% o", 511);
  require_conform("37777777001",   "% o", 4294966785U);
  require_conform("1234abcd",      "% x", 305441741);
  require_conform("edcb5433",      "% x", 3989525555U);
  require_conform("1234ABCD",      "% X", 305441741);
  require_conform("EDCB5433",      "% X", 3989525555U);
  require_conform("x",             "% c", 'x');
}

TEST_CASE("+ flag") {
  require_conform("+42",             "%+d", 42);
  require_conform("-42",             "%+d", -42);
  require_conform("+1024",           "%+d", 1024);
  require_conform("-1024",           "%+d", -1024);
  require_conform("+1024",           "%+i", 1024);
  require_conform("-1024",           "%+i", -1024);

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("  +42",           "%+5d", 42);
  require_conform("  -42",           "%+5d", -42);
  require_conform("            +42", "%+15d", 42);
  require_conform("            -42", "%+15d", -42);
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
  require_conform("+",               "%+.0d", 0);
#endif
}

TEST_CASE("+ flag - non-standard format") {
  require_conform("Hello testing", "%+s", "Hello testing");
  require_conform("1024",          "%+u", 1024);
  require_conform("4294966272",    "%+u", 4294966272U);
  require_conform("777",           "%+o", 511);
  require_conform("37777777001",   "%+o", 4294966785U);
  require_conform("1234abcd",      "%+x", 305441741);
  require_conform("edcb5433",      "%+x", 3989525555U);
  require_conform("1234ABCD",      "%+X", 305441741);
  require_conform("EDCB5433",      "%+X", 3989525555U);
  require_conform("x",             "%+c", 'x');
}

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("0 flag") {
  require_conform("42",              "%0d", 42);
  require_conform("42",              "%0ld", 42L);
  require_conform("-42",             "%0d", -42);
  require_conform("00042",           "%05d", 42);
  require_conform("-0042",           "%05d", -42);
  require_conform("000000000000042", "%015d", 42);
  require_conform("-00000000000042", "%015d", -42);
#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
  require_conform("000000000042.12", "%015.2f", 42.1234);
  require_conform("00000000042.987", "%015.3f", 42.9876);
  require_conform("-00000042.98759", "%015.5f", -42.9876);
#endif
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("- flag") {
  require_conform("42",              "%-d", 42);
  require_conform("-42",             "%-d", -42);
  require_conform("42   ",           "%-5d", 42);
  require_conform("-42  ",           "%-5d", -42);
  require_conform("42             ", "%-15d", 42);
  require_conform("-42            ", "%-15d", -42);
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("- flag and non-standard 0 modifier for integers") {
  require_conform("42",              "%-0d", 42);
  require_conform("-42",             "%-0d", -42);
  require_conform("42   ",           "%-05d", 42);
  require_conform("-42  ",           "%-05d", -42);
  require_conform("42             ", "%-015d", 42);
  require_conform("-42            ", "%-015d", -42);
  require_conform("42",              "%0-d", 42);
  require_conform("-42",             "%0-d", -42);
  require_conform("42   ",           "%0-5d", 42);
  require_conform("-42  ",           "%0-5d", -42);
  require_conform("42             ", "%0-15d", 42);
  require_conform("-42            ", "%0-15d", -42);
  // require_conform("-4.200e+01     ", "%0-15.3e", -42.);
}
#endif

TEST_CASE("# flag") {
  require_conform("0",          "%#o",          0);
  require_conform("01",         "%#o",          1);

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("0",          "%#0o",         0);
  require_conform("   0",       "%#4o",         0);
  require_conform("01",         "%#0o",         1);
  require_conform("  01",       "%#4o",         1);
  require_conform("0x1001",     "%#04x",   0x1001);
  require_conform("01001",      "%#04o",    01001);
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
  require_conform("0",          "%#.0o",        0);
  require_conform("0",          "%#.1o",        0);
  require_conform("0000",       "%#.4o",        0);
  require_conform("01",         "%#.0o",        1);
  require_conform("01",         "%#.1o",        1);
  require_conform("0001",       "%#.4o",        1);
  require_conform("",           "%#.0x",        0);
  require_conform("0x0000614e", "%#.8x",   0x614e);
#endif
}

TEST_CASE("# flag - non-standard format") {
#if NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS == 1
  require_conform("0b110", "%#b", 6);
#endif
}

#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
TEST_CASE("# flag with long-long") {
  require_conform("0",          "%#llo",   (long long)     0);
  require_conform("01",         "%#llo",   (long long)     1);

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("0",          "%#0llo",  (long long)     0);
  require_conform("   0",       "%#4llo",  (long long)     0);
  require_conform("01",         "%#0llo",  (long long)     1);
  require_conform("  01",       "%#4llo",  (long long)     1);
  require_conform("0x1001",     "%#04llx", (long long)0x1001);
  require_conform("01001",      "%#04llo", (long long) 01001);
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
  require_conform("0",          "%#.0llo", (long long)     0);
  require_conform("0",          "%#.1llo", (long long)     0);
  require_conform("0000",       "%#.4llo", (long long)     0);
  require_conform("01",         "%#.0llo", (long long)     1);
  require_conform("01",         "%#.1llo", (long long)     1);
  require_conform("0001",       "%#.4llo", (long long)     1);
  require_conform("",           "%#.0llx", (long long)     0);
  require_conform("0x0000614e", "%#.8llx", (long long)0x614e);
#endif
}
#endif

TEST_CASE("# flag with long-long - non-standard format") {
#if (NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1) && \
    (NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS == 1)
  require_conform("0b110", "%#llb", (long long) 6);
#endif
}

TEST_CASE("specifier") {
  require_conform("Hello testing", "Hello testing");
  require_conform("Hello testing", "%s", "Hello testing");
//  require_conform( "(null)", "%s", (const char*)nullptr);
  require_conform("1024",        "%d", 1024);
  require_conform("-1024",       "%d", -1024);
  require_conform("1024",        "%i", 1024);
  require_conform("-1024",       "%i", -1024);
  require_conform("1024",        "%u", 1024);
  require_conform("4294966272",  "%u", 4294966272U);
  require_conform("777",         "%o", 511);
  require_conform("37777777001", "%o", 4294966785U);
  require_conform("1234abcd",    "%x", 305441741);
  require_conform("edcb5433",    "%x", 3989525555U);
  require_conform("1234ABCD",    "%X", 305441741);
  require_conform("EDCB5433",    "%X", 3989525555U);
  require_conform("%",           "%%");
}

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("width") {
  require_conform("Hello testing", "%1s", "Hello testing");
  require_conform("1024",          "%1d", 1024);
  require_conform("-1024",         "%1d", -1024);
  require_conform("1024",          "%1i", 1024);
  require_conform("-1024",         "%1i", -1024);
  require_conform("1024",          "%1u", 1024);
  require_conform("4294966272",    "%1u", 4294966272U);
  require_conform("777",           "%1o", 511);
  require_conform("37777777001",   "%1o", 4294966785U);
  require_conform("1234abcd",      "%1x", 305441741);
  require_conform("edcb5433",      "%1x", 3989525555U);
  require_conform("1234ABCD",      "%1X", 305441741);
  require_conform("EDCB5433",      "%1X", 3989525555U);
  require_conform("x",             "%1c", 'x');
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("width 20") {
  require_conform("               Hello", "%20s",   "Hello");
  require_conform("                1024", "%20d",   1024);
  require_conform("               -1024", "%20d",   -1024);
  require_conform("                1024", "%20i",   1024);
  require_conform("               -1024", "%20i",   -1024);
  require_conform("                   0", "%20i",   0);
  require_conform("                1024", "%20u",   1024);
  require_conform("          4294966272", "%20u",   4294966272U);
  require_conform("                 777", "%20o",   511);
  require_conform("         37777777001", "%20o",   4294966785U);
  require_conform("            1234abcd", "%20x",   305441741);
  require_conform("            edcb5433", "%20x",   3989525555U);
  require_conform("            1234ABCD", "%20X",   305441741);
  require_conform("            EDCB5433", "%20X",   3989525555U);
  require_conform("                   0", "%20X",   0);
  require_conform("                   0", "%20X",   0U);
  require_conform("                   x", "%20c",   'x');
#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
  require_conform("                   0", "%20llX", 0ULL);
#endif
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("width *20") {
  require_conform("               Hello", "%*s", 20, "Hello");
  require_conform("                1024", "%*d", 20, 1024);
  require_conform("               -1024", "%*d", 20, -1024);
  require_conform("                1024", "%*i", 20, 1024);
  require_conform("               -1024", "%*i", 20, -1024);
  require_conform("                1024", "%*u", 20, 1024);
  require_conform("          4294966272", "%*u", 20, 4294966272U);
  require_conform("                 777", "%*o", 20, 511);
  require_conform("         37777777001", "%*o", 20, 4294966785U);
  require_conform("            1234abcd", "%*x", 20, 305441741);
  require_conform("            edcb5433", "%*x", 20, 3989525555U);
  require_conform("            1234ABCD", "%*X", 20, 305441741);
  require_conform("            EDCB5433", "%*X", 20, 3989525555U);
  require_conform("                   x", "%*c", 20,'x');
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("width -20") {
  require_conform("Hello               ", "%-20s", "Hello");
  require_conform("1024                ", "%-20d", 1024);
  require_conform("-1024               ", "%-20d", -1024);
  require_conform("1024                ", "%-20i", 1024);
  require_conform("-1024               ", "%-20i", -1024);
  require_conform("1024                ", "%-20u", 1024);
  require_conform("4294966272          ", "%-20u", 4294966272U);
  require_conform("777                 ", "%-20o", 511);
  require_conform("37777777001         ", "%-20o", 4294966785U);
  require_conform("1234abcd            ", "%-20x", 305441741);
  require_conform("edcb5433            ", "%-20x", 3989525555U);
  require_conform("1234ABCD            ", "%-20X", 305441741);
  require_conform("EDCB5433            ", "%-20X", 3989525555U);
  require_conform("x                   ", "%-20c", 'x');
  require_conform("|    9| |9 | |    9|", "|%5d| |%-2d| |%5d|", 9, 9, 9);
  require_conform("|   10| |10| |   10|", "|%5d| |%-2d| |%5d|", 10, 10, 10);
  require_conform("|    9| |9           | |    9|", "|%5d| |%-12d| |%5d|", 9, 9, 9);
  require_conform("|   10| |10          | |   10|", "|%5d| |%-12d| |%5d|", 10, 10, 10);

#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
  require_conform("1024.1234           ", "%-20.4f", 1024.1234);
#endif
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("width 0-20") {
  require_conform("Hello               ", "%0-20s", "Hello");
  require_conform("1024                ", "%0-20d", 1024);
  require_conform("-1024               ", "%0-20d", -1024);
  require_conform("1024                ", "%0-20i", 1024);
  require_conform("-1024               ", "%0-20i", -1024);
  require_conform("1024                ", "%0-20u", 1024);
  require_conform("4294966272          ", "%0-20u", 4294966272U);
  require_conform("777                 ", "%0-20o", 511);
  require_conform("37777777001         ", "%0-20o", 4294966785U);
  require_conform("1234abcd            ", "%0-20x", 305441741);
  require_conform("edcb5433            ", "%0-20x", 3989525555U);
  require_conform("1234ABCD            ", "%0-20X", 305441741);
  require_conform("EDCB5433            ", "%0-20X", 3989525555U);
  require_conform("x                   ", "%0-20c", 'x');
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("padding 20") {
  require_conform("00000000000000001024", "%020d", 1024);
  require_conform("-0000000000000001024", "%020d", -1024);
  require_conform("00000000000000001024", "%020i", 1024);
  require_conform("-0000000000000001024", "%020i", -1024);
  require_conform("00000000000000001024", "%020u", 1024);
  require_conform("00000000004294966272", "%020u", 4294966272U);
  require_conform("00000000000000000777", "%020o", 511);
  require_conform("00000000037777777001", "%020o", 4294966785U);
  require_conform("0000000000001234abcd", "%020x", 305441741);
  require_conform("000000000000edcb5433", "%020x", 3989525555U);
  require_conform("0000000000001234ABCD", "%020X", 305441741);
  require_conform("000000000000EDCB5433", "%020X", 3989525555U);
}
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
TEST_CASE("padding .20") {
  require_conform("00000000000000001024",  "%.20d", 1024);
  require_conform("-00000000000000001024", "%.20d", -1024);
  require_conform("00000000000000001024",  "%.20i", 1024);
  require_conform("-00000000000000001024", "%.20i", -1024);
  require_conform("00000000000000001024",  "%.20u", 1024);
  require_conform("00000000004294966272",  "%.20u", 4294966272U);
  require_conform("00000000000000000777",  "%.20o", 511);
  require_conform("00000000037777777001",  "%.20o", 4294966785U);
  require_conform("0000000000001234abcd",  "%.20x", 305441741);
  require_conform("000000000000edcb5433",  "%.20x", 3989525555U);
  require_conform("0000000000001234ABCD",  "%.20X", 305441741);
  require_conform("000000000000EDCB5433",  "%.20X", 3989525555U);
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("padding #020 - non-standard format") {
  require_conform("00000000000000001024", "%#020d", 1024);
  require_conform("-0000000000000001024", "%#020d", -1024);
  require_conform("00000000000000001024", "%#020i", 1024);
  require_conform("-0000000000000001024", "%#020i", -1024);
  require_conform("00000000000000001024", "%#020u", 1024);
  require_conform("00000000004294966272", "%#020u", 4294966272U);
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("padding #020") {
  require_conform("00000000000000000777", "%#020o", 511);
  require_conform("00000000037777777001", "%#020o", 4294966785U);
  require_conform("0x00000000001234abcd", "%#020x", 305441741);
  require_conform("0x0000000000edcb5433", "%#020x", 3989525555U);
  require_conform("0X00000000001234ABCD", "%#020X", 305441741);
  require_conform("0X0000000000EDCB5433", "%#020X", 3989525555U);
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("padding #20 - non-standard format") {
  require_conform("                1024", "%#20d", 1024);
  require_conform("               -1024", "%#20d", -1024);
  require_conform("                1024", "%#20i", 1024);
  require_conform("               -1024", "%#20i", -1024);
  require_conform("                1024", "%#20u", 1024);
  require_conform("          4294966272", "%#20u", 4294966272U);
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("padding #20") {
  require_conform("                0777", "%#20o", 511);
  require_conform("        037777777001", "%#20o", 4294966785U);
  require_conform("          0x1234abcd", "%#20x", 305441741);
  require_conform("          0xedcb5433", "%#20x", 3989525555U);
  require_conform("          0X1234ABCD", "%#20X", 305441741);
  require_conform("          0XEDCB5433", "%#20X", 3989525555U);
}
#endif

#if (NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1) && \
    (NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1)
TEST_CASE("padding 20.5") {
  require_conform("               01024", "%20.5d", 1024);
  require_conform("              -01024", "%20.5d", -1024);
  require_conform("               01024", "%20.5i", 1024);
  require_conform("              -01024", "%20.5i", -1024);
  require_conform("               01024", "%20.5u", 1024);
  require_conform("          4294966272", "%20.5u", 4294966272U);
  require_conform("               00777", "%20.5o", 511);
  require_conform("         37777777001", "%20.5o", 4294966785U);
  require_conform("            1234abcd", "%20.5x", 305441741);
  require_conform("          00edcb5433", "%20.10x", 3989525555U);
  require_conform("            1234ABCD", "%20.5X", 305441741);
  require_conform("          00EDCB5433", "%20.10X", 3989525555U);
}
#endif

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
TEST_CASE("padding neg numbers") {
  // space padding
  require_conform("-5",   "% 1d", -5);
  require_conform("-5",   "% 2d", -5);
  require_conform(" -5",  "% 3d", -5);
  require_conform("  -5", "% 4d", -5);

  // zero padding
  require_conform("-5",   "%01d", -5);
  require_conform("-5",   "%02d", -5);
  require_conform("-05",  "%03d", -5);
  require_conform("-005", "%04d", -5);
}
#endif

#if (NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1) && \
    (NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1)
TEST_CASE("float padding neg numbers") {
  // space padding
  require_conform("-5.0",       "% 3.1f", -5.);
  require_conform("-5.0",       "% 4.1f", -5.);
  require_conform(" -5.0",      "% 5.1f", -5.);

  // zero padding
  require_conform("-5.0",       "%03.1f", -5.);
  require_conform("-5.0",       "%04.1f", -5.);
  require_conform("-05.0",      "%05.1f", -5.);

  // zero padding no decimal point
  require_conform("-5",         "%01.0f", -5.);
  require_conform("-5",         "%02.0f", -5.);
  require_conform("-05",        "%03.0f", -5.);

  // require_conform("-005.0e+00", "%010.1e", -5.);
  // require_conform("-05E+00",    "%07.0E", -5.);
  // require_conform("-05",        "%03.0g", -5.);
  // require_conform("    -5",     "% 6.1g", -5.);
  // require_conform("-5.0e+00",   "% 6.1e", -5.);
  // require_conform("  -5.0e+00", "% 10.1e", -5.);
}
#endif

#if (NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1) && \
    (NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1)
TEST_CASE("length") {
  require_conform("",                     "%.0s", "Hello testing");
  require_conform("                    ", "%20.0s", "Hello testing");
  require_conform("",                     "%.s", "Hello testing");
  require_conform("                    ", "%20.s", "Hello testing");
  require_conform("                1024", "%20.0d", 1024);
  require_conform("               -1024", "%20.0d", -1024);
  require_conform("                    ", "%20.d", 0);
  require_conform("                1024", "%20.0i", 1024);
  require_conform("               -1024", "%20.i", -1024);
  require_conform("                    ", "%20.i", 0);
  require_conform("                1024", "%20.u", 1024);
  require_conform("          4294966272", "%20.0u", 4294966272U);
  require_conform("                    ", "%20.u", 0U);
  require_conform("                 777", "%20.o", 511);
  require_conform("         37777777001", "%20.0o", 4294966785U);
  require_conform("                    ", "%20.o", 0U);
  require_conform("            1234abcd", "%20.x", 305441741);
  require_conform("                                          1234abcd", "%50.x", 305441741);
  require_conform("                                          1234abcd     12345", "%50.x%10.u", 305441741, 12345);
  require_conform("            edcb5433", "%20.0x", 3989525555U);
  require_conform("                    ", "%20.x", 0U);
  require_conform("            1234ABCD", "%20.X", 305441741);
  require_conform("            EDCB5433", "%20.0X", 3989525555U);
  require_conform("                    ", "%20.X", 0U);
}
#endif

#if (NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1) && \
    (NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1)
TEST_CASE("length - non-standard format") {
  require_conform("  ", "%02.0u", 0U);
  require_conform("  ", "%02.0d", 0);
}
#endif

#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
TEST_CASE("float") {
  require_conform("3.1415",    "%.4f", 3.1415354);
  require_conform("30343.140", "%.3f", 30343.1415354);
  require_conform("34",               "%.0f", 34.1415354);
  require_conform("1",                "%.0f", 1.3);
  require_conform("1",                "%.0f", 1.55);
  require_conform("1.6",              "%.1f", 1.64);
  require_conform("42.89",            "%.2f", 42.8952);
  require_conform("42.895198820",     "%.9f", 42.8952);
  require_conform("42.8952217100",    "%.10f", 42.895223);
  require_conform("42.895221710000",  "%.12f", 42.89522312345678);
  require_conform("42.895225520000",  "%.12f", 42.89522387654321);
  require_conform("42.500000",        "%f", 42.5);
  require_conform("42.5",             "%.1f", 42.5);
  require_conform("42167.000000",     "%f", 42167.0);
  require_conform("-12345.987304680", "%.9f", -12345.987654321);
  require_conform("3.9",              "%.1f", 3.999);
  require_conform("3",                "%.0f", 3.5);
  require_conform("4",                "%.0f", 4.5);
  require_conform("3",                "%.0f", 3.49);
  require_conform("3.4",              "%.1f", 3.49);

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("     nan",  "%8f", (double)NAN);
  require_conform("     inf",  "%8f", (double)INFINITY);
  require_conform("-inf    ",  "%-8f", (double)-INFINITY);
  require_conform("42477.371093750000000", "%020.15f", 42477.37109375);
  require_conform(" 42.89",           "%6.2f", 42.8952);
  require_conform("+42.89",           "%+6.2f", 42.8952);
  require_conform("+42.9",            "%+5.1f", 42.9252);
  require_conform("a0.5  ",           "a%-5.1f", 0.5);
  require_conform("a0.5  end",        "a%-5.1fend", 0.5);
#endif

  // switch from decimal to exponential representation
  //
//  require_conform("    +inf",  "%+8e", (double)INFINITY);
//  CAPTURE_AND_PRINT(test::sprintf_, buffer, "%.0f", (double) ((int64_t)1 * 1000 ) );
//  if (PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL < 3) {
//    CHECK(!strcmp(buffer, "1e+3"));
//  }
//  else {
//    CHECK(!strcmp(buffer, "1000"));
//  }
//
//  CAPTURE_AND_PRINT(test::sprintf_, buffer, "%.0f", (double) ((int64_t)1 * 1000 * 1000 ) );
//  if (PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL < 6) {
//    CHECK(!strcmp(buffer, "1e+6"));
//  }
//  else {
//    CHECK(!strcmp(buffer, "1000000"));
//  }
//
//  CAPTURE_AND_PRINT(test::sprintf_, buffer, "%.0f", (double) ((int64_t)1 * 1000 * 1000 * 1000 ) );
//  if (PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL < 9) {
//    CHECK(!strcmp(buffer, "1e+9"));
//  }
//  else {
//    CHECK(!strcmp(buffer, "1000000000"));
//  }
//
//  CAPTURE_AND_PRINT(test::sprintf_, buffer, "%.0f", (double) ((int64_t)1 * 1000 * 1000 * 1000 * 1000) );
//  if (PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL < 12) {
//#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
//    CHECK(!strcmp(buffer, "1e+12"));
//#else
//    CHECK(!strcmp(buffer, ""));
//#endif
//  }
//  else {
//    CHECK(!strcmp(buffer, "1000000000000"));
//  }
//
//  CAPTURE_AND_PRINT(test::sprintf_, buffer, "%.0f", (double) ((int64_t)1 * 1000 * 1000 * 1000 * 1000 * 1000) );
//  if (PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL < 15) {
//#if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
//    CHECK(!strcmp(buffer, "1e+15"));
//#else
//    CHECK(!strcmp(buffer, ""));
//#endif
//  }
//  else {
//    CHECK(!strcmp(buffer, "1000000000000000"));
//  }
//
//  PRINTING_CHECK("0.5",              "%.4g", 0.5);
//  PRINTING_CHECK("1",                "%.4g", 1.0);
//  PRINTING_CHECK("12345.7",          "%G", 12345.678);
//  PRINTING_CHECK("12345.68",         "%.7G", 12345.678);
//  PRINTING_CHECK("1.2346E+08",       "%.5G", 123456789.);
//  PRINTING_CHECK("12345",            "%.6G", 12345.);
//  PRINTING_CHECK("  +1.235e+08",     "%+12.4g", 123456789.);
//  PRINTING_CHECK("0.0012",           "%.2G", 0.001234);
//  PRINTING_CHECK(" +0.001234",       "%+10.4G", 0.001234);
//  PRINTING_CHECK("+001.234e-05",     "%+012.4g", 0.00001234);
//  PRINTING_CHECK("-1.23e-308",       "%.3g", -1.2345e-308);
//  PRINTING_CHECK("+1.230E+308",      "%+.3E", 1.23e+308);
//  PRINTING_CHECK("1.000e+01",        "%.3e", 9.9996);
//  PRINTING_CHECK("0",                "%g", 0.);
//  PRINTING_CHECK("-0",               "%g", -0.);
//  PRINTING_CHECK("+0",               "%+g", 0.);
//  PRINTING_CHECK("-0",               "%+g", -0.);
//  PRINTING_CHECK("-4e+04",           "%.1g", -40661.5);
//  PRINTING_CHECK("-4.e+04",          "%#.1g", -40661.5);
//  PRINTING_CHECK("100.",             "%#.3g", 99.998580932617187500);
//  // Rounding-focused checks
//  PRINTING_CHECK("4.895512e+04",     "%e", 48955.125);
//  PRINTING_CHECK("9.2524e+04",       "%.4e", 92523.5);
//  PRINTING_CHECK("-8.380923438e+04", "%.9e", -83809.234375);
  // out of range for float: should switch to exp notation if supported, else empty
// #if PRINTF_SUPPORT_DECIMAL_SPECIFIERS
//   CAPTURE_AND_PRINT(test::sprintf_, buffer, "%.1f", 1E20);
// #if PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
//   CHECK(!strcmp(buffer, "1.0e+20"));
// #else
//   CHECK(!strcmp(buffer, ""));
// #endif
// #endif
}
#endif

TEST_CASE("types") {
  require_conform("0",                    "%i", 0);
  require_conform("1234",                 "%i", 1234);
  require_conform("32767",                "%i", 32767);
  require_conform("-32767",               "%i", -32767);
  require_conform("30",                   "%li", 30L);
  require_conform("-2147483647",          "%li", -2147483647L);
  require_conform("2147483647",           "%li", 2147483647L);
  require_conform("100000",               "%lu", 100000L);
  require_conform("4294967295",           "%lu", 0xFFFFFFFFL);
  require_conform("165140",               "%o", 60000);
  require_conform("57060516",             "%lo", 12345678L);
  require_conform("12345678",             "%lx", 0x12345678L);
  require_conform("abcdefab",             "%lx", 0xabcdefabL);
  require_conform("ABCDEFAB",             "%lX", 0xabcdefabL);
  require_conform("v",                    "%c", 'v');
  require_conform("wv",                   "%cv", 'w');
  require_conform("A Test",               "%s", "A Test");
  require_conform("255",                  "%hhu", (unsigned char) 0xFFU);
  require_conform("4660",                 "%hu", (unsigned short) 0x1234u);
  require_conform("Test100 65535",        "%s%hhi %hu", "Test", (char) 100, (unsigned short) 0xFFFF);

#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
  require_conform("2147483647",           "%zu", (size_t)2147483647UL);
  require_conform("2147483647",           "%zd", (size_t)2147483647UL);
  require_conform("-2147483647",          "%zi", (ssize_t)-2147483647L);

  {
    char buffer[11];
    require_conform("a",                    "%tx", &buffer[10] - &buffer[0]);
  }
#endif

#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
  require_conform("30",                   "%lli", 30LL);
  require_conform("-9223372036854775807", "%lli", -9223372036854775807LL);
  require_conform("9223372036854775807",  "%lli", 9223372036854775807LL);
  require_conform("281474976710656",      "%llu", 281474976710656LLU);
  require_conform("18446744073709551615", "%llu", 18446744073709551615LLU);
  require_conform("-2147483647",          "%ji", (intmax_t)-2147483647L);
  require_conform("1234567891234567",     "%llx", 0x1234567891234567LLU);
#endif
}

#if NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS == 1
TEST_CASE("types - non-standard format") {
  require_conform("1110101001100000", "%b", 60000);
  require_conform("101111000110000101001110", "%lb", 12345678L);
}
#endif

TEST_CASE("pointer") { // mpaland pads to register width (non-standard), npf doesn't.
  require_conform("0x1234", "%p", (void *)0x1234u);
  require_conform("0x12345678", "%p", (void *)0x12345678u);
  require_conform(
    "0x12345678-0x7edcba98", "%p-%p", (void *)0x12345678u, (void *)0x7edcba98u);
  require_conform("0xffffffff", "%p", (void *)(uintptr_t)0xffffffffu);
}

TEST_CASE("unknown flag (non-standard format)") {
  require_conform("%kmarco", "%kmarco"); // mpaland printf removes leading %
}

TEST_CASE("string length") {
#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
  require_conform("This", "%.4s", "This is a test");
  require_conform("test", "%.4s", "test");
  require_conform("123", "%.7s", "123");
  require_conform("", "%.7s", "");
  require_conform("1234ab", "%.4s%.2s", "123456", "abcdef");
  require_conform("123", "%.*s", 3, "123456");
#endif
  // npf is not tolerant of null string pointers.
  // require_conform("(null)", "%.*s", 3, (const char*) NULL);
}

TEST_CASE("string length (non-standard format)") {
  // mpaland consumes malformed format string, npf does not.
  require_conform("%.4.2s", "%.4.2s", "123456");
}

TEST_CASE("misc") {
  require_conform("53000atest-20 bit",    "%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit");

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("hi x",                 "%*sx", -3, "hi");
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
  require_conform("1",                    "%.*d", -1, 1);
  require_conform(" ",                    "% .0d", 0);
  require_conform("foo",                  "%.3s", "foobar");
#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
  require_conform("     00004",           "%10.5d", 4);
  require_conform("00123               ", "%-20.5i", 123);
#endif
#endif

#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
  require_conform("-67224.546875000000000000", "%.18f", -67224.546875);
  require_conform("0.33",                 "%.*f", 2, 0.33333333);
#endif

#if 0
  require_conform("0.33",              "%.*g", 2, 0.33333333);
  require_conform("3.33e-01",          "%.*e", 2, 0.33333333);
  require_conform("0.000000e+00",      "%e", 0.0);
  require_conform("-0.000000e+00",     "%e", -0.0);
#endif
}

TEST_CASE("extremal signed integer values") {
  require_conform(nullptr, "%hhd", std::numeric_limits<char>::max());
  require_conform(nullptr, "%hd", std::numeric_limits<short int>::max());
  require_conform(nullptr, "%d", std::numeric_limits<int>::min());
  require_conform(nullptr, "%d", std::numeric_limits<int>::max());
  require_conform(nullptr, "%ld", std::numeric_limits<long int>::min());
  require_conform(nullptr, "%ld", std::numeric_limits<long int>::max());
#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
  require_conform(nullptr, "%lld", std::numeric_limits<long long int>::min());
  require_conform(nullptr, "%lld", std::numeric_limits<long long int>::max());
#endif
}

TEST_CASE("extremal unsigned integer values") {
  require_conform(nullptr, "%hhu", std::numeric_limits<char unsigned>::max());
  require_conform(nullptr, "%hu", std::numeric_limits<short unsigned>::max());
  require_conform(nullptr, "%u", std::numeric_limits<unsigned>::max());
  require_conform(nullptr, "%lu", std::numeric_limits<long unsigned>::max());
#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
  require_conform(nullptr, "%llu", std::numeric_limits<long long unsigned>::max());
#endif
}

#if NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS == 1
TEST_CASE("writeback specifier") {
  char buffer[100];

  struct {
    char char_;
    short short_;
    int int_;
    long long_;
    long long long_long_;
  } num_written;

  num_written.int_ = 1234;
  npf_snprintf(buffer, sizeof(buffer), "%n", &num_written.int_);
  REQUIRE(num_written.int_ == 0);
  REQUIRE(std::string{buffer} == "");

  num_written.int_ = 1234;
  npf_snprintf(buffer, sizeof(buffer), "foo%nbar", &num_written.int_);
  REQUIRE(num_written.int_ == 3);
  REQUIRE(std::string{buffer} == "foobar");
}
#endif

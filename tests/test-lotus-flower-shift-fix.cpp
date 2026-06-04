// Lotus Flower fork — sticky shift fix tests.
// Compile & run:
//   g++ -std=c++17 -I../src -o test-shift-fix test-lotus-flower-shift-fix.cpp ../src/lotus-flower-shift-fix.cpp && ./test-shift-fix

#include "lotus-flower-shift-fix.h"
#include <cassert>
#include <cstdio>
#include <string>

#define TEST(name, input, expected)                                  \
    do {                                                             \
        std::string got = lotus_shift_fix::fixShift(input);          \
        if (got == expected) {                                       \
            std::printf("  PASS  %s\n", name);                       \
        } else {                                                     \
            std::printf("  FAIL  %s: \"%s\" -> \"%s\", want \"%s\"\n", \
                        name, input, got.c_str(), expected);         \
            ok = false;                                              \
        }                                                            \
    } while (0)

int main() {
    bool ok = true;

    std::printf("=== fixShift — should fix (valid Vietnamse syllable) ===\n");
    // Basic cases: two uppercase initials + rest lowercase → valid syllable
    TEST("THái",         "THái",    "Thái");
    TEST("GIa",          "GIa",     "Gia");
    TEST("CHa",          "CHa",     "Cha");
    TEST("QUả",          "QUả",     "Quả");
    TEST("THem",         "THem",    "Them"); // "them" = valid VNmese (th+em)
    TEST("NGon",         "NGon",    "Ngon"); // "ngon" = valid
    TEST("TRường",       "TRường",  "Trường");
    TEST("UYên",         "UYên",    "Uyên");
    TEST("NOt",          "NOt",     "Not");  // "not" = valid (n+ot)
    TEST("TRa",          "TRa",     "Tra");  // "tra" = valid (tr+a)

    std::printf("\n=== fixShift — should NOT fix (length < 3) ===\n");
    TEST("short EM",     "EM",      "EM");
    TEST("short à",      "à",       "à");
    TEST("short AB",     "AB",      "AB");
    TEST("short Đà",     "Đà",      "Đà");

    std::printf("\n=== fixShift — should NOT fix (first char not upper) ===\n");
    TEST("lower aBC",    "aBC",     "aBC");
    TEST("lower àBC",    "àBC",     "àBC");

    std::printf("\n=== fixShift — should NOT fix (second char not upper) ===\n");
    TEST("mixed AaB",    "AaB",     "AaB");
    TEST("mixed AàB",    "AàB",     "AàB");
    TEST("mixed ĐàN",    "ĐàN",     "ĐàN"); // 'à' is lowercase → not upper

    std::printf("\n=== fixShift — should NOT fix (rest contains non-lowercase) ===\n");
    TEST("all upper THIS",     "THIS",     "THIS");
    TEST("with dash NÓN-VN",   "NÓN-VN",  "NÓN-VN"); // 'V' uppercase
    TEST("digit A1a",          "A1a",      "A1a");    // '1' not lower
    TEST("more upper ABcD",    "ABcD",     "ABcD");   // 'D' uppercase
    TEST("3upper NGHĩ",       "NGHĩ",     "NGHĩ");   // 'H' uppercase
    TEST("3upper NGOài",      "NGOài",    "NGOài");  // 'O' uppercase
    TEST("3upper THIện",      "THIện",    "THIện");  // 'I' uppercase

    std::printf("\n=== fixShift — should NOT fix (not a valid Vietnamese syllable) ===\n");
    TEST("This invalid",        "THis",      "THis");      // "this" → "is" not a rhyme
    TEST("USa invalid",         "USa",       "USa");       // "usa" → not a rhyme
    TEST("AUa invalid",         "AUa",       "AUa");       // "aua" → not a rhyme

    std::printf("\n=== fixShift — should NOT fix (non-Vietnamese chars only) ===\n");
    TEST("pure EN noon",       "NOON",      "NOON");
    TEST("pure EN USa",        "USa",       "USa");
    TEST("pure EN This",       "THis",      "THis");

    std::printf("\n=== fixShift — Vietnamese chars with tone on second position ===\n");
    TEST("THòe",               "THòe",      "Thòe");
    TEST("ĐÀn",                "ĐÀn",       "Đàn");

    std::printf("\n=== fixShift — trailing non-Vietnamese chars (word breaks) ===\n");
    TEST("trailing space",       "THái ",        "Thái ");
    TEST("trailing period",      "GIa.",         "Gia.");
    TEST("trailing comma space", "QUả, ",        "Quả, ");
    TEST("trailing newline",     "THem\n",       "Them\n");
    TEST("trailing digit",       "THái1",        "Thái1");
    TEST("no trailing (clean)",  "THái",         "Thái");
    TEST("trailing chars invalid syllable", "AUa, ",  "AUa, ");

    std::printf("\n=== fixShift — edge: empty / single char ===\n");
    TEST("empty",              "",          "");
    TEST("single X",           "X",         "X");

    std::printf("\n");
    if (ok) {
        std::printf("All tests passed.\n");
        return 0;
    } else {
        std::printf("Some tests FAILED.\n");
        return 1;
    }
}

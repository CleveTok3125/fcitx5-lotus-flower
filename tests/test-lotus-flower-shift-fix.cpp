// Lotus Flower fork — sticky shift fix tests.
// Compile & run:
//   g++ -std=c++17 -I../src -o test-shift-fix test-lotus-flower-shift-fix.cpp ../src/lotus-flower-shift-fix.cpp && ./test-shift-fix

#include "lotus-flower-shift-fix.h"
#include <cassert>
#include <cstdio>
#include <string>

#define TEST(name, input, expected)                                                                                                                                                \
    do {                                                                                                                                                                           \
        std::string got = lotus_shift_fix::fixShift(input);                                                                                                                        \
        if (got == expected) {                                                                                                                                                     \
            std::printf("  PASS  %s\n", name);                                                                                                                                     \
        } else {                                                                                                                                                                   \
            std::printf("  FAIL  %s: \"%s\" -> \"%s\", want \"%s\"\n", name, input, got.c_str(), expected);                                                                        \
            ok = false;                                                                                                                                                            \
        }                                                                                                                                                                          \
    } while (0)

#define TEST2(name, input, maxSh, expected)                                                                                                                                        \
    do {                                                                                                                                                                           \
        std::string got = lotus_shift_fix::fixShift(input, maxSh);                                                                                                                 \
        if (got == expected) {                                                                                                                                                     \
            std::printf("  PASS  %s\n", name);                                                                                                                                     \
        } else {                                                                                                                                                                   \
            std::printf("  FAIL  %s: \"%s\" -> \"%s\", want \"%s\"\n", name, input, got.c_str(), expected);                                                                        \
            ok = false;                                                                                                                                                            \
        }                                                                                                                                                                          \
    } while (0)

int main() {
    bool ok = true;

    std::printf("=== fixShift — should fix (valid Vietnamse syllable) ===\n");
    // Basic cases: two uppercase initials + rest lowercase → valid syllable
    TEST("THái", "THái", "Thái");
    TEST("GIa", "GIa", "Gia");
    TEST("CHa", "CHa", "Cha");
    TEST("QUả", "QUả", "Quả");
    TEST("THem", "THem", "Them"); // "them" = valid VNmese (th+em)
    TEST("NGon", "NGon", "Ngon"); // "ngon" = valid
    TEST("TRường", "TRường", "Trường");
    TEST("UYên", "UYên", "Uyên");
    TEST("NOt", "NOt", "Not"); // "not" = valid (n+ot)
    TEST("TRa", "TRa", "Tra"); // "tra" = valid (tr+a)

    std::printf("\n=== fixShift — should NOT fix (length < 3) ===\n");
    TEST("short EM", "EM", "EM");
    TEST("short à", "à", "à");
    TEST("short AB", "AB", "AB");
    TEST("short Đà", "Đà", "Đà");

    std::printf("\n=== fixShift — should NOT fix (first char not upper) ===\n");
    TEST("lower aBC", "aBC", "aBC");
    TEST("lower àBC", "àBC", "àBC");

    std::printf("\n=== fixShift — should NOT fix (second char not upper) ===\n");
    TEST("mixed AaB", "AaB", "AaB");
    TEST("mixed AàB", "AàB", "AàB");
    TEST("mixed ĐàN", "ĐàN", "ĐàN"); // 'à' is lowercase → not upper

    std::printf("\n=== fixShift — should NOT fix (rest contains non-lowercase) ===\n");
    TEST("all upper THIS", "THIS", "THIS");
    TEST("with dash NÓN-VN", "NÓN-VN", "NÓN-VN"); // 'V' uppercase
    TEST("digit A1a", "A1a", "A1a");              // '1' not lower
    TEST("more upper ABcD", "ABcD", "ABcD");      // 'D' uppercase
    TEST("3upper NGHĩ", "NGHĩ", "NGHĩ");          // 'H' uppercase
    TEST("3upper NGOài", "NGOài", "NGOài");       // 'O' uppercase
    TEST("3upper THIện", "THIện", "THIện");       // 'I' uppercase

    std::printf("\n=== fixShift — should NOT fix (not a valid Vietnamese syllable) ===\n");
    TEST("This invalid", "THis", "THis"); // "this" → "is" not a rhyme
    TEST("USa invalid", "USa", "USa");    // "usa" → not a rhyme
    TEST("AUa invalid", "AUa", "AUa");    // "aua" → not a rhyme

    std::printf("\n=== fixShift — should NOT fix (non-Vietnamese chars only) ===\n");
    TEST("pure EN noon", "NOON", "NOON");
    TEST("pure EN USa", "USa", "USa");
    TEST("pure EN This", "THis", "THis");

    std::printf("\n=== fixShift — Vietnamese chars with tone on second position ===\n");
    TEST("THòe", "THòe", "Thòe");
    TEST("ĐÀn", "ĐÀn", "Đàn");

    std::printf("\n=== fixShift — trailing non-Vietnamese chars (word breaks) ===\n");
    TEST("trailing space", "THái ", "Thái ");
    TEST("trailing period", "GIa.", "Gia.");
    TEST("trailing comma space", "QUả, ", "Quả, ");
    TEST("trailing newline", "THem\n", "Them\n");
    TEST("trailing digit", "THái1", "Thái1");
    TEST("no trailing (clean)", "THái", "Thái");
    TEST("trailing chars invalid syllable", "AUa, ", "AUa, ");

    std::printf("\n=== fixShift — edge: empty / single char ===\n");
    TEST("empty", "", "");
    TEST("single X", "X", "X");

    std::printf("\n=== fixShift with maxShifted=2 — should fix ===\n");
    TEST2("THÁi max2", "THÁi", 2, "Thái");
    TEST2("NGHĩ max2", "NGHĩ", 2, "Nghĩ");
    TEST2("NGOài max2", "NGOài", 2, "Ngoài");
    TEST2("THIện max2", "THIện", 2, "Thiện");

    std::printf("\n=== fixShift with maxShifted=2 — should NOT fix ===\n");
    TEST2("THÁI max2", "THÁI", 2, "THÁI");

    std::printf("\n=== fixShift with maxShifted=3 — should fix ===\n");
    TEST2("THÁI max3", "THÁI", 3, "Thái");

    std::printf("\n=== fixShift with maxShifted=3 — should NOT fix ===\n");
    TEST2("THIS max3", "THIS", 3, "THIS"); // all upper, no trailing lower
    TEST2("USa max3", "USa", 3, "USa");    // invalid syllable

    std::printf("\n=== fixShift with maxShifted=0 (unlimited) — should fix ===\n");
    TEST2("THÁi max0", "THÁi", 0, "Thái");
    TEST2("NGHĩ max0", "NGHĩ", 0, "Nghĩ");
    TEST2("THÁI max0", "THÁI", 0, "Thái");

    std::printf("\n=== fixShift with maxShifted=0 (unlimited) — should NOT fix ===\n");
    TEST2("THIS max0", "THIS", 0, "THIS");      // all upper, no trailing lower
    TEST2("invalid USa max0", "USa", 0, "USa"); // invalid syllable

    std::printf("\n=== fixShift with maxShifted=1 — backward compat ===\n");
    TEST2("THái max1", "THái", 1, "Thái");
    TEST2("THÁi max1", "THÁi", 1, "THÁi");
    TEST2("NGHĩ max1", "NGHĩ", 1, "NGHĩ");
    TEST2("THÁI max1", "THÁI", 1, "THÁI");

    std::printf("\n=== fixShift with negative maxShifted ===\n");
    TEST2("THÁi max-1", "THÁi", -1, "Thái");
    TEST2("THÁI max-1 no tail", "THÁI", -1, "THÁI");
    TEST2("THái max-1", "THái", -1, "Thái");
    TEST2("THem max-1", "THem", -1, "Them");
    TEST2("NGHĩ max-1", "NGHĩ", -1, "Nghĩ");
    TEST2("THái max-2", "THái", -2, "Thái");
    TEST2("THÁi max-2 upper", "THÁi", -2, "THÁi");
    TEST2("THem max-2", "THem", -2, "Them");
    TEST2("NGOài max-2", "NGOài", -2, "Ngoài");
    TEST2("NGHĩ max-2", "NGHĩ", -2, "NGHĩ");
    TEST2("THÁI max-3 all up", "THÁI", -3, "THÁI");
    TEST2("UYên max-1", "UYên", -1, "Uyên");
    TEST2("TRường max-1", "TRường", -1, "Trường");
    TEST2("THem max-7 short", "THem", -7, "THem");
    TEST2("THÁi max-7 short", "THÁi", -7, "THÁi");

    std::printf("\n");
    if (ok) {
        std::printf("All tests passed.\n");
        return 0;
    } else {
        std::printf("Some tests FAILED.\n");
        return 1;
    }
}

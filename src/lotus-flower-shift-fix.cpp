#include "lotus-flower-shift-fix.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lotus_shift_fix {

    std::vector<uint32_t> utf8ToCodepoints(const std::string& s) {
        std::vector<uint32_t> cps;
        for (size_t i = 0; i < s.size();) {
            uint32_t      cp;
            unsigned char c = s[i];
            if ((c & 0x80) == 0) {
                cp = c;
                i++;
            } else if ((c & 0xE0) == 0xC0) {
                if (i + 1 >= s.size())
                    break;
                cp = ((c & 0x1F) << 6) | (s[i + 1] & 0x3F);
                i += 2;
            } else if ((c & 0xF0) == 0xE0) {
                if (i + 2 >= s.size())
                    break;
                cp = ((c & 0x0F) << 12) | ((s[i + 1] & 0x3F) << 6) | (s[i + 2] & 0x3F);
                i += 3;
            } else if ((c & 0xF8) == 0xF0) {
                if (i + 3 >= s.size())
                    break;
                cp = ((c & 0x07) << 18) | ((s[i + 1] & 0x3F) << 12) | ((s[i + 2] & 0x3F) << 6) | (s[i + 3] & 0x3F);
                i += 4;
            } else {
                i++;
                continue;
            }
            cps.push_back(cp);
        }
        return cps;
    }

    std::string codepointsToUtf8(const std::vector<uint32_t>& cps) {
        std::string out;
        for (uint32_t cp : cps) {
            if (cp < 0x80) {
                out += (char)cp;
            } else if (cp < 0x800) {
                out += (char)(0xC0 | (cp >> 6));
                out += (char)(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                out += (char)(0xE0 | (cp >> 12));
                out += (char)(0x80 | ((cp >> 6) & 0x3F));
                out += (char)(0x80 | (cp & 0x3F));
            } else if (cp < 0x110000) {
                out += (char)(0xF0 | (cp >> 18));
                out += (char)(0x80 | ((cp >> 12) & 0x3F));
                out += (char)(0x80 | ((cp >> 6) & 0x3F));
                out += (char)(0x80 | (cp & 0x3F));
            }
        }
        return out;
    }

    static std::unordered_map<uint32_t, uint32_t> upperToLower;
    static std::unordered_map<uint32_t, uint32_t> lowerToBase;
    static std::unordered_set<uint32_t>           upperSet;
    static std::unordered_set<uint32_t>           lowerSet;

    static void                                   addPair(uint32_t upper, uint32_t lower, uint32_t base) {
        upperToLower[upper] = lower;
        lowerToBase[lower]  = base;
        upperSet.insert(upper);
        lowerSet.insert(lower);
    }

    // Initialize all mappings
    static struct InitTables {
        InitTables() {
            // ASCII A-Z
            for (uint32_t u = 'A'; u <= 'Z'; ++u) {
                uint32_t l = u + 0x20;
                addPair(u, l, l);
            }
            // Đ - đ
            addPair(0x0110, 0x0111, 0x0111);
            // A with diacritics
            addPair(0x00C0, 0x00E0, 'a');
            addPair(0x00C1, 0x00E1, 'a');
            addPair(0x1EA2, 0x1EA3, 'a');
            addPair(0x00C3, 0x00E3, 'a');
            addPair(0x1EA0, 0x1EA1, 'a');
            // Ă
            addPair(0x0102, 0x0103, 0x0103);
            addPair(0x1EB0, 0x1EB1, 0x0103);
            addPair(0x1EAE, 0x1EAF, 0x0103);
            addPair(0x1EB2, 0x1EB3, 0x0103);
            addPair(0x1EB4, 0x1EB5, 0x0103);
            addPair(0x1EB6, 0x1EB7, 0x0103);
            // Â
            addPair(0x00C2, 0x00E2, 0x00E2);
            addPair(0x1EA6, 0x1EA7, 0x00E2);
            addPair(0x1EA4, 0x1EA5, 0x00E2);
            addPair(0x1EA8, 0x1EA9, 0x00E2);
            addPair(0x1EAA, 0x1EAB, 0x00E2);
            addPair(0x1EAC, 0x1EAD, 0x00E2);
            // E
            addPair(0x00C8, 0x00E8, 'e');
            addPair(0x00C9, 0x00E9, 'e');
            addPair(0x1EBA, 0x1EBB, 'e');
            addPair(0x1EBC, 0x1EBD, 'e');
            addPair(0x1EB8, 0x1EB9, 'e');
            // Ê
            addPair(0x00CA, 0x00EA, 0x00EA);
            addPair(0x1EC0, 0x1EC1, 0x00EA);
            addPair(0x1EBE, 0x1EBF, 0x00EA);
            addPair(0x1EC2, 0x1EC3, 0x00EA);
            addPair(0x1EC4, 0x1EC5, 0x00EA);
            addPair(0x1EC6, 0x1EC7, 0x00EA);
            // I
            addPair(0x00CC, 0x00EC, 'i');
            addPair(0x00CD, 0x00ED, 'i');
            addPair(0x1EC8, 0x1EC9, 'i');
            addPair(0x0128, 0x0129, 'i');
            addPair(0x1ECA, 0x1ECB, 'i');
            // O
            addPair(0x00D2, 0x00F2, 'o');
            addPair(0x00D3, 0x00F3, 'o');
            addPair(0x1ECE, 0x1ECF, 'o');
            addPair(0x00D5, 0x00F5, 'o');
            addPair(0x1ECC, 0x1ECD, 'o');
            // Ô
            addPair(0x00D4, 0x00F4, 0x00F4);
            addPair(0x1ED2, 0x1ED3, 0x00F4);
            addPair(0x1ED0, 0x1ED1, 0x00F4);
            addPair(0x1ED4, 0x1ED5, 0x00F4);
            addPair(0x1ED6, 0x1ED7, 0x00F4);
            addPair(0x1ED8, 0x1ED9, 0x00F4);
            // Ơ
            addPair(0x01A0, 0x01A1, 0x01A1);
            addPair(0x1EDC, 0x1EDD, 0x01A1);
            addPair(0x1EDA, 0x1EDB, 0x01A1);
            addPair(0x1EDE, 0x1EDF, 0x01A1);
            addPair(0x1EE0, 0x1EE1, 0x01A1);
            addPair(0x1EE2, 0x1EE3, 0x01A1);
            // U
            addPair(0x00D9, 0x00F9, 'u');
            addPair(0x00DA, 0x00FA, 'u');
            addPair(0x1EE6, 0x1EE7, 'u');
            addPair(0x0168, 0x0169, 'u');
            addPair(0x1EE4, 0x1EE5, 'u');
            // Ư
            addPair(0x01AF, 0x01B0, 0x01B0);
            addPair(0x1EEA, 0x1EEB, 0x01B0);
            addPair(0x1EE8, 0x1EE9, 0x01B0);
            addPair(0x1EEC, 0x1EED, 0x01B0);
            addPair(0x1EEE, 0x1EEF, 0x01B0);
            addPair(0x1EF0, 0x1EF1, 0x01B0);
            // Y
            addPair(0x00DD, 0x00FD, 'y');
            addPair(0x1EF2, 0x1EF3, 'y');
            addPair(0x1EF6, 0x1EF7, 'y');
            addPair(0x1EF8, 0x1EF9, 'y');
            addPair(0x1EF4, 0x1EF5, 'y');
        }
    } init;

    static bool isVietnameseUpper(uint32_t cp) {
        return upperSet.count(cp) > 0;
    }
    static bool isVietnameseLower(uint32_t cp) {
        return lowerSet.count(cp) > 0;
    }

    static uint32_t toLowerCp(uint32_t cp) {
        auto it = upperToLower.find(cp);
        return (it != upperToLower.end()) ? it->second : cp;
    }

    static uint32_t toBaseCp(uint32_t cp) {
        uint32_t lower = toLowerCp(cp);
        auto     it    = lowerToBase.find(lower);
        return (it != lowerToBase.end()) ? it->second : lower;
    }

    static std::string toBase(const std::string& utf8) {
        auto cps = utf8ToCodepoints(utf8);
        for (auto& cp : cps)
            cp = toBaseCp(cp);
        return codepointsToUtf8(cps);
    }

    // ========== Phonetic structure databases ==========
    static const std::vector<std::string> INITIALS = {
        "ngh", "ng", "nh", "gh", "gi", "kh", "ph", "th", "tr", "ch", "qu", "b", "c", "d", "đ", "g", "h", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "x",
    };

    static const std::unordered_set<std::string> RHYMES_NO_GLIDE = {
        "a",    "ai",  "ao",  "am",  "an",  "ang",  "anh", "ac",  "ach",  "ap",  "at",  "au",  "ay",  "ă",   "ăm",   "ăn",   "ăng", "ăp",  "ăt",  "ăc",   "â",    "âm",  "ân",
        "âng",  "âp",  "ât",  "âc",  "âu",  "ây",   "e",   "em",  "en",   "eng", "ec",  "ep",  "et",  "eo",  "ê",    "êm",   "ên",  "êng", "êc",  "êp",   "êt",   "êu",  "ênh",
        "i",    "im",  "in",  "inh", "ip",  "it",   "iu",  "o",   "om",   "on",  "ong", "oc",  "op",  "ot",  "oi",   "ô",    "ôm",  "ôn",  "ông", "ôc",   "ôp",   "ôt",  "ôi",
        "ơ",    "ơm",  "ơn",  "ơng", "ơc",  "ơp",   "ơt",  "ơi",  "u",    "um",  "un",  "ung", "uc",  "up",  "ut",   "ui",   "ư",   "ưm",  "ưn",  "ưng",  "ưc",   "ưp",  "ưt",
        "ưi",   "ia",  "iê",  "iêm", "iên", "iêng", "iêp", "iêt", "iêc",  "iêu", "ya",  "yê",  "yêm", "yên", "yêng", "yêp",  "yêt", "yêc", "yêu", "ua",   "uô",   "uôm", "uôn",
        "uông", "uôc", "uôt", "uôi", "ưa",  "ươ",   "ươm", "ươn", "ương", "ươp", "ươt", "ươc", "ươi", "uy",  "uynh", "uych", "uyt", "uyn", "uya", "uyên", "uyêt",
    };

    static const std::unordered_set<std::string> GLIDE_O = {
        "oa", "oai", "oan", "oang", "oac", "oach", "oat", "oap", "oăm", "oăn", "oăng", "oăc", "oăt", "oăp", "oe", "oeo", "oen", "oeng", "oet", "oec",
    };

    static const std::unordered_set<std::string> GLIDE_U = {
        "ua",   "uai",  "uan", "uang", "uac", "uach", "uat", "uap", "uây",  "uân",  "uâng", "uât", "uâc",  "uê",
        "uênh", "uêch", "uên", "uêt",  "uơ",  "uơi",  "uy",  "uya", "uynh", "uych", "uyt",  "uyn", "uyên", "uyêt",
    };

    static const std::unordered_set<std::string> AFTER_QU = {
        "a", "ai", "an", "ang", "ac", "ach", "at", "ap", "ây", "ân", "âng", "ât", "âc", "ê", "ênh", "êch", "ên", "êt", "ơ", "ơi", "y", "ya", "ynh", "ych", "yt", "yn", "yên", "yêt",
    };

    static bool isValidRhyme(const std::string& rhyme, const std::string& init) {
        if (init == "qu")
            return AFTER_QU.count(rhyme) > 0;
        return RHYMES_NO_GLIDE.count(rhyme) || GLIDE_O.count(rhyme) || GLIDE_U.count(rhyme);
    }

    static std::pair<std::string, std::string> extractInitialAndRhyme(const std::string& base) {
        for (const auto& init : INITIALS) {
            if (base.compare(0, init.size(), init) == 0) {
                return {init, base.substr(init.size())};
            }
        }
        return {"", base};
    }

    static bool isValidSyllable(const std::string& s) {
        auto cps = utf8ToCodepoints(s);
        for (auto& cp : cps)
            cp = toLowerCp(cp);
        std::string lower  = codepointsToUtf8(cps);
        std::string base   = toBase(lower);
        auto [init, rhyme] = extractInitialAndRhyme(base);
        return isValidRhyme(rhyme, init);
    }

    static bool isVietnameseChar(uint32_t cp) {
        return isVietnameseUpper(cp) || isVietnameseLower(cp);
    }

    std::string fixShift(const std::string& word, int maxShifted) {
        auto cps = utf8ToCodepoints(word);
        if (cps.size() < 3)
            return word;

        size_t vnEnd = cps.size();
        while (vnEnd > 0 && !isVietnameseChar(cps[vnEnd - 1]))
            vnEnd--;

        if (vnEnd < 3)
            return word;

        if (!isVietnameseUpper(cps[0]))
            return word;

        if (maxShifted < 0) {
            unsigned int n = static_cast<unsigned int>(-maxShifted);
            if (vnEnd < static_cast<size_t>(n) + 2)
                return word;

            for (size_t i = vnEnd - n; i < vnEnd; ++i)
                if (!isVietnameseLower(cps[i]))
                    return word;

            size_t stickyEnd = vnEnd - n;
            if (stickyEnd <= 1)
                return word;

            bool hasSticky = false;
            for (size_t i = 1; i < stickyEnd; ++i)
                if (isVietnameseUpper(cps[i]))
                    hasSticky = true;

            if (!hasSticky)
                return word;

            std::vector<uint32_t> candCps(cps.begin(), cps.begin() + vnEnd);
            for (size_t i = 1; i < stickyEnd; ++i)
                if (isVietnameseUpper(cps[i]))
                    candCps[i] = toLowerCp(cps[i]);

            std::string candidate = codepointsToUtf8(candCps);
            if (isValidSyllable(candidate)) {
                if (vnEnd == cps.size())
                    return candidate;
                return candidate + codepointsToUtf8(std::vector<uint32_t>(cps.begin() + vnEnd, cps.end()));
            }
            return word;
        }

        size_t shifted = 0;
        for (size_t i = 1; i < vnEnd; ++i) {
            if (!isVietnameseUpper(cps[i]))
                break;
            shifted++;
        }

        if (shifted < 1)
            return word;

        if (maxShifted > 0 && shifted > static_cast<size_t>(maxShifted))
            return word;

        for (size_t i = 1 + shifted; i < vnEnd; ++i)
            if (!isVietnameseLower(cps[i]))
                return word;

        std::vector<uint32_t> candCps(cps.begin(), cps.begin() + vnEnd);
        for (size_t i = 1; i <= shifted; ++i)
            candCps[i] = toLowerCp(cps[i]);

        std::string candidate = codepointsToUtf8(candCps);

        if (isValidSyllable(candidate)) {
            if (vnEnd == cps.size())
                return candidate;
            return candidate + codepointsToUtf8(std::vector<uint32_t>(cps.begin() + vnEnd, cps.end()));
        }
        return word;
    }

} // namespace lotus_shift_fix

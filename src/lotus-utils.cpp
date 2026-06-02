/*
 * SPDX-FileCopyrightText: 2025 Võ Ngô Hoàng Thành <thanhpy2009@gmail.com>
 * SPDX-FileCopyrightText: 2026 Nguyễn Hoàng Kỳ  <nhktmdzhg@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "lotus-utils.h"
#include "lotus-config.h"

#include <cstddef>
#include <fcitx-utils/utf8.h>
#include <pwd.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>

// Global variables
std::atomic<fcitx::LotusMode> realMode{fcitx::LotusMode::Smooth};
std::atomic<bool>             needEngineReset{false};
std::atomic<bool>             g_mouse_clicked{false};
std::atomic<bool>             is_deleting_{false};
std::atomic<bool>             stop_flag_monitor{false};
std::atomic<int>              uinput_client_fd_{-1};
std::atomic<unsigned int>     realtextLen{0};
std::atomic<int>              mouse_socket_fd{-1};

FCITX_DEFINE_LOG_CATEGORY(lotus, "lotus", fcitx::LogLevel::NoLog);

std::string buildSocketPath(const char* base_path_suffix) {
    struct passwd  pwd{};
    struct passwd* result   = nullptr;
    long           buf_size = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (buf_size == -1) {
        buf_size = 16384;
    }
    std::vector<char> buf(buf_size);
    std::string       username;
    int               res = getpwuid_r(getuid(), &pwd, buf.data(), buf_size, &result);
    if (res == 0 && result != nullptr) {
        username = result->pw_name;
    } else {
        username = "unknown";
    }
    std::string path;
    path.reserve(32);
    path += "lotussocket-";
    path += username;
    path += '-';
    path += base_path_suffix;
    const size_t max_socket_path_length = UNIX_PATH_MAX - 1;
    path.resize(std::min(path.length(), max_socket_path_length));
    return path;
}

int64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

bool isBackspace(uint32_t sym) {
    return sym == 65288 || sym == 8 || sym == FcitxKey_BackSpace;
}

int compareAndSplitStrings(const std::string& A, const std::string& B, std::string& deletedPart, std::string& addedPart) {
    size_t i = 0;
    size_t j = 0;
    while (i < A.size() && j < B.size()) {
        unsigned int lenA = fcitx_utf8_char_len(&A[i]);
        unsigned int lenB = fcitx_utf8_char_len(&B[j]);
        if (lenA == 0 || lenB == 0) {
            break;
        }
        if (i + lenA > A.size() || j + lenB > B.size()) {
            break;
        }
        if (lenA == lenB && std::strncmp(&A[i], &B[j], lenA) == 0) {
            i += lenA;
            j += lenB;
        } else {
            break;
        }
    }

    deletedPart.assign(A, i);
    addedPart.assign(B, j);
    return (deletedPart.empty() && addedPart.empty()) ? 1 : 2;
}

bool isStartsWith(const std::string& str, const std::string& prefix) {
#if __cplusplus >= 202002L
    return str.starts_with(prefix);
#else
    return str.substr(0, prefix.size()) == prefix;
#endif
}

std::string getFrontendName(fcitx::InputContext* ic) {
    if (ic == nullptr) {
        return "unknown";
    }
    return ic->frontend();
}

std::string fixStickyShift(const std::string& word) {
    size_t len = fcitx::utf8::length(word);
    if (len < 3)
        return word;

    uint32_t ch1;
    auto     it = fcitx::utf8::getNextChar(word.begin(), word.end(), &ch1);
    if (ch1 < 'A' || ch1 > 'Z')
        return word;

    uint32_t ch2;
    it = fcitx::utf8::getNextChar(it, word.end(), &ch2);
    if (ch2 < 'A' || ch2 > 'Z')
        return word;

    bool hasNonUpper = false;
    auto checkIt     = it;
    while (checkIt != word.end()) {
        uint32_t ch;
        checkIt = fcitx::utf8::getNextChar(checkIt, word.end(), &ch);
        if (ch < 'A' || ch > 'Z') {
            hasNonUpper = true;
            break;
        }
    }
    if (!hasNonUpper)
        return word;

    std::string result;
    result.reserve(word.size());
    result += word[0];
    for (size_t i = 1; i < word.size(); ++i) {
        auto c = static_cast<unsigned char>(word[i]);
        if (c >= 'A' && c <= 'Z')
            result += static_cast<char>(c + 32);
        else
            result += word[i];
    }
    return result;
}
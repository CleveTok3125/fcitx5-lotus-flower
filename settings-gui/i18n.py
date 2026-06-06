# SPDX-FileCopyrightText: 2026 Nguyen Hoang Ky <nhktmdzhg@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Internationalization setup for the application.
"""

import gettext
import locale
import os


def setup_i18n():
    """Initialize gettext with system locale."""
    try:
        locale.setlocale(locale.LC_ALL, "")
        domain = "fcitx5-lotus"
        localedir = "/usr/share/locale"

        if os.path.exists(localedir):
            gettext.bindtextdomain(domain, localedir)
            gettext.textdomain(domain)
    except Exception as e:
        print(f"Failed to initialize i18n: {e}")


_ = gettext.gettext


def N_(message: str) -> str:
    """Marker for translatable strings (extracted by xgettext, not translated).
    Use N_() at module level to mark strings for .pot extraction without
    actually translating them at import time. Call _() on the result later
    after i18n setup.
    """
    return message

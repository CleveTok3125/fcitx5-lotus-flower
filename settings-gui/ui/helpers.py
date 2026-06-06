# SPDX-FileCopyrightText: 2026 Nguyen Hoang Ky <nhktmdzhg@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Helper utilities and shared mappings for the Lotus settings GUI.
"""

from i18n import _
from ui.components import HelpIcon


# Tooltip text for specific settings keys
HELPERS = {
    "FreeMarking": _(
        "You can type tone marks at the end of the word or anywhere inside."
    ),
    "FixUinputWithAck": _(
        "Fix typing issues in Uinput mode for Chromium-based browsers like Chrome or Edge."
    ),
    "CapitalizeMacro": _(
        "Automatically match expansion case to trigger key case.\n\n"
        "Example if 'kg' is 'khô gà':\n"
        "- kg -> khô gà\n"
        "- Kg -> Khô gà\n"
        "- KG -> KHÔ GÀ"
    ),
    "AutoNonVnRestore": _(
        "Automatically revert the typed sequence if the resulting word is not in the dictionary.\n"
        "This helps prevent accidental Vietnamese transformations on English words or mixed text."
    ),
    "EnableMacroInOffMode": _(
        "Allow macros to work when the input mode is OFF.\n"
        "When disabled, macros are only available in active typing modes."
    ),
    "FixStickyShift": _(
        "Auto fix sticky Shift capitalization for words (3+ letters).\n\n"
        "When you hold Shift a bit too long while typing the first letter, "
        "the second letter may also be capitalized by accident.\n"
        "For example: 'GHi' → 'Ghi', 'THái' → 'Thái'."
    ),
    "FixStickyShiftMaxChars": _(
        "Maximum consecutive uppercase letters to fix (after the first letter).\n\n"
        "Controls how many uppercase letters after the first one are lowered.\n"
        "0 = unlimited (any number of uppercase letters will be fixed)\n"
        "1 = only fix the second letter (default, e.g. 'THái' → 'Thái')\n"
        "2 = fix up to 2 letters (e.g. 'THÁi' → 'Thái')\n"
        "3 = fix up to 3 letters (e.g. 'THÁI' → 'Thái')\n\n"
        "Negative values count from the end:\n"
        "-1 = last 1 char is lowercase (e.g. 'THÁi' → 'Thái')\n"
        "-2 = last 2 chars are lowercase (e.g. 'THái' → 'Thái')\n\n"
        "Range: –7 to 7."
    ),
}


def add_help_icon(layout, key, clear_existing=False):
    """
    Utility to add a HelpIcon to a layout based on a setting key.
    Optionally clears existing HelpIcons from the layout first.
    """
    if clear_existing:
        # Avoid duplicate icons by removing existing ones in the layout cleanly
        for i in reversed(range(layout.count())):
            item = layout.itemAt(i)
            if item and item.widget() and isinstance(item.widget(), HelpIcon):
                widget = layout.takeAt(i).widget()
                if widget:
                    widget.deleteLater()

    # Only add if we have a mapped helper text
    helper_text = HELPERS.get(key)
    if helper_text:
        icon = HelpIcon(helper_text)
        layout.addWidget(icon)
        return icon
    return None

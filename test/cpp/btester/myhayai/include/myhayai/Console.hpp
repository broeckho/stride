#pragma once
/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */

/**
 * @file
 * Header file for Console.
 */

#include <cstdio>
#include <iostream>
#include <unistd.h>

namespace myhayai {

/// Static helper class for outputting to a terminal/console.
class Console
{
public:
        /// Console text colors.Default console color (used for resets).
        /// @warning Avoid using black unless it is absolutely necesssary.
        /// @warning Avoid using white unless it is absolutely necessary.
        enum TextColor
        {
                TextDefault,
                TextBlack,
                TextBlue,
                TextGreen,
                TextCyan,
                TextRed,
                TextPurple,
                TextYellow,
                TextWhite
        };

        /// Get the singleton instance of @ref Console.
        /// @returns a reference to the singleton instance of the benchmarker console.
        static Console& Instance()
        {
                static Console singleton;
                return singleton;
        }

        /// Test if formatting is enabled.
        static bool IsFormattingEnabled() { return Instance()._formattingEnabled; }

        /// Set whether formatting is enabled.
        static void SetFormattingEnabled(bool enabled) { Instance()._formattingEnabled = enabled; }

private:
        Console() : _formattingEnabled(true) {}
        bool _formattingEnabled;
};

#if !defined(HAYAI_NO_COLOR)
inline std::ostream& operator<<(std::ostream& stream, const Console::TextColor& color)
{
        static const bool outputNoColor = isatty(fileno(stdout)) != 1;

        if ((!Console::IsFormattingEnabled()) || (outputNoColor) ||
            ((stream.rdbuf() != std::cout.rdbuf()) && (stream.rdbuf() != std::cerr.rdbuf())))
                return stream;

        const char* value = "";
        switch (color) {
        case Console::TextDefault: value = "\033[m"; break;
        case Console::TextBlack: value = "\033[0;30m"; break;
        case Console::TextBlue: value = "\033[0;34m"; break;
        case Console::TextGreen: value = "\033[0;32m"; break;
        case Console::TextCyan: value = "\033[0;36m"; break;
        case Console::TextRed: value = "\033[0;31m"; break;
        case Console::TextPurple: value = "\033[0;35m"; break;
        case Console::TextYellow: value = "\033[0;33m"; break;
        case Console::TextWhite: value = "\033[0;37m"; break;
        }
        return stream << value;
}
#else // No color
inline std::ostream& operator<<(std::ostream& stream, const Console::TextColor&) { return stream; }
#endif

} // namespace myhayai

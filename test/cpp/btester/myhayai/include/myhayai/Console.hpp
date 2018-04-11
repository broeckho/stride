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

class Console
{
public:
        /// Console text colors. EnableColor and DisableColor change the
        /// stream state to enable/disable color. The enable makes a
        /// fair attempt to discover whther the stream is a teminal.
        /// Default console color (used for resets).
        /// @warning Avoid using black unless it is absolutely necesssary.
        /// @warning Avoid using white unless it is absolutely necessary.
        enum TextColor
        {
                EnableColor,
                DisableColor,
                Default,
                Black,
                Blue,
                Green,
                Cyan,
                Red,
                Purple,
                Yellow,
                White
        };
};

inline std::ostream& operator<<(std::ostream& stream, const Console::TextColor& color)
{
        static bool c = false;

        const char *value = "";
        switch (color) {
                case Console::EnableColor: {
                        if ((isatty(fileno(stdout)) == 1) &&
                            ((stream.rdbuf() != std::cout.rdbuf()) || (stream.rdbuf() != std::cerr.rdbuf()))) {
                                c = true;
                        }
                        return stream;
                }
                case Console::DisableColor: {
                        c = false;
                        return stream;
                }
                case Console::Default: value = c ? "\033[m" : ""; break;
                case Console::Black: value = c ? "\033[0;30m" : ""; break;
                case Console::Blue: value = c ? "\033[0;34m" : ""; break;
                case Console::Green:value = c ? "\033[0;32m" : ""; break;
                case Console::Cyan: value = c ? "\033[0;36m" : ""; break;
                case Console::Red: value = c ? "\033[0;31m" : ""; break;
                case Console::Purple: value = c ? "\033[0;35m" : ""; break;
                case Console::Yellow: value = c ? "\033[0;33m" : ""; break;
                case Console::White: value = c ? "\033[0;37m" : ""; break;
        }

        return stream << value;
}

} // namespace myhayai

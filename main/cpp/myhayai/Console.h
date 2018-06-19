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
 */
/**
 * @file
 * Header file for Console.
 */

#include <cstdio>
#include <iostream>
#include <unistd.h>

namespace myhayai {
namespace console {

/// Console text colors. EnableColor and DisableColor change the
/// stream state to enable/disable color. The enable makes a
/// fair attempt to discover whther the stream is a teminal.
/// Default console color (used for resets).
/// @warning Avoid using black unless it is absolutely necesssary.
/// @warning Avoid using white unless it is absolutely necessary.
enum class Color
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

inline std::ostream& operator<<(std::ostream& stream, const Color& color)
{
        static bool c = false;

        const char* value = "";
        switch (color) {
        case Color::EnableColor: {
                if ((isatty(fileno(stdout)) == 1) &&
                    ((stream.rdbuf() != std::cout.rdbuf()) || (stream.rdbuf() != std::cerr.rdbuf()))) {
                        c = true;
                }
                return stream;
        }
        case Color::DisableColor: {
                c = false;
                return stream;
        }
        case Color::Default: value = (c ? "\033[m" : ""); break;
        case Color::Black: value = (c ? "\033[0;30m" : ""); break;
        case Color::Blue: value = (c ? "\033[0;34m" : ""); break;
        case Color::Green: value = (c ? "\033[0;32m" : ""); break;
        case Color::Cyan: value = (c ? "\033[0;36m" : ""); break;
        case Color::Red: value = (c ? "\033[0;31m" : ""); break;
        case Color::Purple: value = (c ? "\033[0;35m" : ""); break;
        case Color::Yellow: value = (c ? "\033[0;33m" : ""); break;
        case Color::White: value = (c ? "\033[0;37m" : ""); break;
        }

        return stream << value;
}

} // namespace console
} // namespace myhayai

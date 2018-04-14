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
 *  Copyright 2017, 2018 Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Info on configuration..
 */

#include <string>

namespace stride {
namespace util {

/**
 * Config information.
 */
class ConfigInfo
{
public:
        ///
        static constexpr bool HaveOpenMP()
        {
#ifdef _OPENMP
                return true;
#else
                return false;
#endif
        }

        ///
        static std::string GitRevision();

        ///
        static std::string GetHostname();

        ///
        static unsigned int NumberAvailableThreads();

        ///
        static constexpr unsigned int ProcessorCount() { return PROCCOUNT; }
};

} // namespace util
} // namespace stride

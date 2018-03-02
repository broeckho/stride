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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Info on configuration through compile time constants i.o. macros.
 */

#include <omp.h>
#include <string>

namespace stride {
namespace util {

/**
 * Info on revision id and commit date.
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
        static unsigned int NumberAvailableThreads()
        {
                unsigned int i = 1U;
#pragma omp parallel
                {
                        i = static_cast<unsigned int>(omp_get_num_threads());
                }
                return i;
        }
};

} // namespace util
} // namespace stride

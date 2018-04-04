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

#include "util/Git.h"

#include <omp.h>
#include <string>
#include <unistd.h>

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

        static std::string GitRevision()
        {
                return stride_git_hash;
        }

        static std::string GetHostname()
        {
                char hostname[40];
                gethostname(hostname, 40);
                return hostname;
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

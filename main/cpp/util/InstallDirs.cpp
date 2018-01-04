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
 * Interface for install directory queries.
 */

#include "InstallDirs.h"

#include "util/StringUtils.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <string>

#if defined(WIN32)
#include <stdlib.h>
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <limits.h>
#include <mach-o/dyld.h>
#endif

namespace stride {
namespace util {

using namespace std;
using namespace boost::filesystem;

InstallDirs::InstallDirs() { Initialize(); }

void InstallDirs::Initialize()
{
        //------- Retrieving path of executable
        {
#if defined(WIN32)
                char exePath[MAX_PATH];
                HMODULE hModule = GetModuleHandle(NULL);
                if (GetModuleFileName(NULL, exePath, sizeof(exePath)) != 0)
                        ;
                {
                        m_exec_path = canonical(system_complete(exePath));
                }
#elif defined(__linux__)
                char exePath[PATH_MAX];
                size_t size = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
                if (size > 0 && size != sizeof(exePath)) {
                        m_exec_path = canonical(system_complete(exePath));
                }
#elif defined(__APPLE__)
                char exePath[PATH_MAX];
                uint32_t size = sizeof(exePath);
                if (_NSGetExecutablePath(exePath, &size) == 0) {
                        m_exec_path = canonical(system_complete(exePath));
                }
#endif
        }

        //------- Retrieving root and bin directory (the subdirectory of the install root)
        {
                path exec_dir = m_exec_path.parent_path();
                if (!m_exec_path.empty()) {
#if (__APPLE__)
                        if (exec_dir.filename().string() == "MacOS") {
                                // app
                                //      -Contents               <-Root Path
                                //              -MacOS
                                //                   -executables
                                m_bin_dir = exec_dir;
                                m_root_dir = exec_dir.parent_path();
                        } else
#endif
                            if (ToLower(exec_dir.filename().string()) == "debug" ||
                                ToLower(exec_dir.filename().string()) == "release") {
                                // x/exec                <-Root Path
                                //      -bin
                                //              -release/debug
                                //                      -executables
                                m_bin_dir = exec_dir.parent_path();
                                m_root_dir = exec_dir.parent_path().parent_path();
                        } else
#if (WIN32)
                            if (exec_dir.filename().string() != "bin") {
                                // Executables in root folder
                                m_bin_dir = exec_dir;
                                m_root_dir = exec_dir;
                        } else
#endif
                        {
                                // x/exec                <-Root Path
                                //      -bin
                                //              -executables
                                m_bin_dir = exec_dir;
                                m_root_dir = exec_dir.parent_path();
                        }
                }
        }

        //------- Data Dir
        {
                m_data_dir = m_root_dir / "data";
                m_data_dir = is_directory(m_data_dir) ? m_data_dir : path();
        }

        //------- Current Dir
        {
                m_current_dir = system_complete(current_path());
        }
}

} // end of namespace
} // end of namespace

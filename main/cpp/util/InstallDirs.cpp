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
#include <climits>
#include <mach-o/dyld.h>
#endif

namespace stride {
namespace util {

using namespace std;
using namespace boost::filesystem;

void InstallDirs::Print(ostream& os)
{
        os << "Local      directory:   " << InstallDirs::GetExecPath().string() << endl;
        os << "Current    directory:   " << InstallDirs::GetCurrentDir().string() << endl;
        os << "Install    directory:   " << InstallDirs::GetRootDir().string() << endl;
        os << "Config     directory:   " << InstallDirs::GetConfigDir().string() << endl;
        os << "Data       directory:   " << InstallDirs::GetDataDir().string() << endl;
        os << "Checkpoint directory:   " << InstallDirs::GetCheckpointsDir().string() << endl;
}

void InstallDirs::Check()
{
        if (InstallDirs::GetCurrentDir().compare(InstallDirs::GetRootDir()) != 0) {
                throw runtime_error(string(__func__) + "> Current directory is not install root! Aborting.");
        }
        if (InstallDirs::GetDataDir().empty()) {
                throw runtime_error(string(__func__) + "> Data directory not present! Aborting.");
        }
}

InstallDirs::Dirs& InstallDirs::Get()
{
        static Dirs dirs = Initialize();
        return dirs;
}

InstallDirs::Dirs InstallDirs::Initialize()
{
        Dirs dirs;
        //------- Retrieving path of executable
        {
#if defined(WIN32)
                char    exePath[MAX_PATH];
                HMODULE hModule = GetModuleHandle(NULL);
                if (GetModuleFileName(NULL, exePath, sizeof(exePath)) != 0)
                        ;
                {
                        dirs.m_exec_path = canonical(system_complete(exePath));
                }
#elif defined(__linux__)
                char   exePath[PATH_MAX];
                size_t size = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
                if (size > 0 && size < sizeof(exePath)) {
                        exePath[size]    = '\0';
                        dirs.m_exec_path = canonical(system_complete(exePath));
                }
#elif defined(__APPLE__)
                char     exePath[PATH_MAX];
                uint32_t size = sizeof(exePath);
                if (_NSGetExecutablePath(exePath, &size) == 0) {
                        dirs.m_exec_path = canonical(system_complete(exePath));
                }
#endif
        }

        //------- Retrieving root and bin directory (the subdirectory of the install root)
        {
                path exec_dir = dirs.m_exec_path.parent_path();
                if (!dirs.m_exec_path.empty()) {
#if (__APPLE__)
                        if (exec_dir.filename().string() == "MacOS") {
                                // app
                                //      -Contents               <-Root Path
                                //              -MacOS
                                //                   -executables
                                dirs.m_bin_dir  = exec_dir;
                                dirs.m_root_dir = exec_dir.parent_path();
                        } else
#endif
                            if (ToLower(exec_dir.filename().string()) == "debug" ||
                                ToLower(exec_dir.filename().string()) == "release") {
                                // x/exec                <-Root Path
                                //      -bin
                                //              -release/debug
                                //                      -executables
                                dirs.m_bin_dir  = exec_dir.parent_path();
                                dirs.m_root_dir = exec_dir.parent_path().parent_path();
                        } else
#if (WIN32)
                            if (exec_dir.filename().string() != "bin") {
                                // Executables in root folder
                                dirs.m_bin_dir  = exec_dir;
                                dirs.m_root_dir = exec_dir;
                        } else
#endif
                        {
                                // x/exec                <-Root Path
                                //      -bin
                                //              -executables
                                dirs.m_bin_dir  = exec_dir;
                                dirs.m_root_dir = exec_dir.parent_path();
                        }
                }
        }
        //------- Data Dir
        {
                dirs.m_data_dir = dirs.m_root_dir / "data";
                dirs.m_data_dir = is_directory(dirs.m_data_dir) ? dirs.m_data_dir : path();
        }
        //------- Current Dir
        {
                dirs.m_current_dir = system_complete(current_path());
        }
        //------- Checkpoints Dir
        {
                dirs.m_checkpoints_dir = dirs.m_root_dir / "checkpoints";
                dirs.m_checkpoints_dir = is_directory(dirs.m_checkpoints_dir) ? dirs.m_checkpoints_dir : path();
        }
        //------- Tests Dir
        {
                dirs.m_tests_dir = dirs.m_root_dir / "tests";
                dirs.m_tests_dir = is_directory(dirs.m_tests_dir) ? dirs.m_tests_dir : path();
        }
        //------- Config Dir
        {
                dirs.m_config_dir = dirs.m_root_dir / "config";
                dirs.m_config_dir = is_directory(dirs.m_config_dir) ? dirs.m_config_dir : path();
        }
        return dirs;
}

} // namespace util
} // namespace stride

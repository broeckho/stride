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
 * Interface for install directory queries.
 */

#include <boost/filesystem.hpp>
#include <iostream>

namespace stride {
namespace util {

/**
 * Install directories.
 */
class InstallDirs
{
public:
        /// Utility method: get path to bin directory.
        static boost::filesystem::path GetBinDir() { return Get().m_bin_dir; }

        /// Utility method: get path to the current directory.
        static boost::filesystem::path GetCurrentDir() { return Get().m_current_dir; }

        /// Utility method: get path to the directory for data files.
        static boost::filesystem::path GetDataDir() { return Get().m_data_dir; }

        /// Utility method: get name of executable.
        static boost::filesystem::path GetExecPath() { return Get().m_exec_path; }

        /// Utility method: get application installation root directory.
        static boost::filesystem::path GetRootDir() { return Get().m_root_dir; }

        /// Utility method: get path to checkpoints directory
        static boost::filesystem::path GetCheckpointsDir() { return Get().m_checkpoints_dir; }

        /// Utility method: get path to test data directory
        static boost::filesystem::path GetTestsDir() { return Get().m_tests_dir; }

        static boost::filesystem::path GetConfigDir() { return Get().m_config_dir; }

        /// Utility method: print all configured directories
        static void Print(std::ostream& os);

        /// Verifies that all required directories exists. Throws runtime_error if not valid.
        static void Check();

private:
        /// Using this to avoid global variables & their initialization.
        struct Dirs
        {
                boost::filesystem::path m_bin_dir;
                boost::filesystem::path m_current_dir;
                boost::filesystem::path m_data_dir;
                boost::filesystem::path m_exec_path;
                boost::filesystem::path m_root_dir;
                boost::filesystem::path m_checkpoints_dir;
                boost::filesystem::path m_tests_dir;
                boost::filesystem::path m_config_dir;
        };

private:
        /// Initialize all paths.
        static Dirs Initialize();

        /// Return paths.
        static Dirs& Get();
};

} // namespace util
} // namespace stride

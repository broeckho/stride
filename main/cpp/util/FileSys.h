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
#include <functional>
#include <string>

namespace stride {
namespace util {

/**
 * Utilities for interaction with filesystem.
 */
class FileSys
{
public:
        /// String represents a directory path (relative or absolute) iff it contains
        /// at least one / (may be a trailing one).
        static bool IsDirectoryString(const std::string& s);

        /// Interpret prefix (directory or filename prefix) and return appropriate path.
        static boost::filesystem::path BuildPath(const std::string& output_prefix, const std::string& filename);

public:
        /// Get path to the current directory.
        static boost::filesystem::path GetCurrentDir() { return Get().m_current_dir; }

        /// Get path of the executable.
        static boost::filesystem::path GetExecPath() { return Get().m_exec_path; }

public:
        /// Verify that current dir is root dir and all install dirs are present.
        /// \param logger       logger verification finds failures; defaults to no-op.
        /// \return             staus true iff everything verifies ok.
        static bool CheckInstallEnv(
            std::function<void(const std::string&)> logger = std::function<void(const std::string&)>());

        /// Return bin dir (only relevant when use_install_dirs mode is active)
        static boost::filesystem::path GetBinDir() { return Get().m_bin_dir; }

        /// Return config dir (only relevant when use_install_dirs mode is active)
        static boost::filesystem::path GetConfigDir() { return Get().m_config_dir; }

        /// /// Return data dir (only relevant when use_install_dirs mode is active)
        static boost::filesystem::path GetDataDir() { return Get().m_data_dir; }

        /// Return install root dir (only relevant when use_install_dirs mode is active)
        static boost::filesystem::path GetRootDir() { return Get().m_root_dir; }

        /// Return tests dir (only relevant when use_install_dirs mode is active)
        static boost::filesystem::path GetTestsDir() { return Get().m_tests_dir; }

private:
        /// Using this to avoid global variables & their initialization.
        struct Dirs
        {
                Dirs() = default;

                boost::filesystem::path m_current_dir;
                boost::filesystem::path m_exec_path;

                // only relevant when use_install_dirs mode is active
                boost::filesystem::path m_bin_dir;
                boost::filesystem::path m_config_dir;
                boost::filesystem::path m_data_dir;
                boost::filesystem::path m_root_dir;
                boost::filesystem::path m_tests_dir;
        };

private:
        /// Initialize all paths.
        static Dirs Initialize();

        /// Return paths.
        static Dirs& Get();
};

} // namespace util
} // namespace stride

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

#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <functional>
#include <string>

#ifdef BOOST_FOUND
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace filesys = boost::filesystem;
#else
#include <filesystem>
namespace filesys = std::filesystem;
#endif

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

        /// Create a directory relative to the current directory with the given path, returns if it was succesful.
        static bool CreateDirectory(std::string s);

        /// Interpret prefix (directory or filename prefix) and return appropriate path.
        static filesys::path BuildPath(const std::string& output_prefix, const std::string& filename);

        /// Read ptree from file at path.
        static boost::property_tree::ptree ReadPtreeFile(const filesys::path& f_p);

        /// Read ptree from file specifified by name string.
        static boost::property_tree::ptree ReadPtreeFile(const std::string& f_n);

        /// Write ptree to file at path.
        static void WritePtreeFile(const filesys::path& f_p, const boost::property_tree::ptree& pt);

        /// Write ptree to file specifified by name string.
        static void WritePtreeFile(const std::string& f_n, const boost::property_tree::ptree& pt);

public:
        /// Get path to the current directory.
        static filesys::path GetCurrentDir() { return Get().m_current_dir; }

        /// Get path of the executable.
        static filesys::path GetExecPath() { return Get().m_exec_path; }

public:
        /// Verify that current dir is root dir and all install dirs are present.
        /// \param logger       logger verification finds failures; defaults to no-op.
        /// \return             staus true iff everything verifies ok.
        static bool CheckInstallEnv(
            std::function<void(const std::string&)> logger = std::function<void(const std::string&)>());

        /// Return bin dir (only relevant when use_install_dirs mode is active)
        static filesys::path GetBinDir() { return Get().m_bin_dir; }

        /// Return config dir (only relevant when use_install_dirs mode is active)
        static filesys::path GetConfigDir() { return Get().m_config_dir; }

        /// /// Return data dir (only relevant when use_install_dirs mode is active)
        static filesys::path GetDataDir() { return Get().m_data_dir; }

        /// Return install root dir (only relevant when use_install_dirs mode is active)
        static filesys::path GetRootDir() { return Get().m_root_dir; }

        /// Return tests dir (only relevant when use_install_dirs mode is active)
        static filesys::path GetTestsDir() { return Get().m_tests_dir; }

private:
        /// Using this to avoid global variables & their initialization.
        struct Dirs
        {
                Dirs()
                    : m_current_dir(), m_exec_path(), m_bin_dir(), m_config_dir(), m_data_dir(), m_root_dir(),
                      m_tests_dir(){};

                filesys::path m_current_dir;
                filesys::path m_exec_path;

                // only relevant when use_install_dirs mode is active
                filesys::path m_bin_dir;
                filesys::path m_config_dir;
                filesys::path m_data_dir;
                filesys::path m_root_dir;
                filesys::path m_tests_dir;
        };

private:
        /// Initialize all paths.
        static Dirs Initialize();

        /// Return paths.
        static Dirs& Get();
};

} // namespace util
} // namespace stride

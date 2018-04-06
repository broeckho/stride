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
 * Header file for FileOutputter.
 */
#include "myhayai/Outputter.hpp"

#include <fstream>

namespace myhayai {

/// File outputter.
class FileOutputter
{
public:
        /// File outputter.
        /// @param path Output path. Expected to be available during the life time of the outputter.
        explicit FileOutputter(const char* path) : _path(path), _outputter(nullptr) {}

        virtual ~FileOutputter();

        /// Set up.
        /// Opens the output file for writing and initializes the outputter.
        virtual void SetUp();

        /// Outputter.
        virtual Outputter& GetOutputter();

protected:
        /// Create outputter from output stream.
        /// @param stream Output stream for the outputter.
        /// @returns the outputter for the given format.
        virtual Outputter* CreateOutputter(std::ostream& stream) = 0;

private:
        const char*   _path;
        std::ofstream _stream;
        Outputter*    _outputter;
};

} // namespace myhayai

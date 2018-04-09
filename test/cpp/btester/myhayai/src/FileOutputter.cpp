
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
 * Implementation file for FileOutputter.
 */

#include "myhayai/FileOutputter.hpp"

#include <fstream>

namespace myhayai {

FileOutputter::~FileOutputter()
{
        if (m_outputter) {
                delete m_outputter;
        }
        m_stream.close();
}

/// Set up.
/// Opens the output file for writing and initializes the outputter.
void FileOutputter::SetUp()
{
        m_stream.open(m_path, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (m_stream.bad()) {
                std::stringstream error;
                error << "failed to open " << m_path << " for writing: " << strerror(errno);
                throw std::runtime_error(error.str());
        }
        m_outputter = CreateOutputter(m_stream);
}

/// Outputter.
Outputter& FileOutputter::GetOutputter()
{
        if (!m_outputter) {
                throw std::runtime_error("outputter has not been set up");
        }
        return *m_outputter;
}

} // namespace myhayai

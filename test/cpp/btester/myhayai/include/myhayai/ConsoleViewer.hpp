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
 * Header file for ConsoleOutputter.
 */

#include "Benchmarker.hpp"
#include "Payload.hpp"
#include "TestDescriptors.hpp"

#include <ostream>

namespace myhayai {

/// Console outputter. Prints the result to standard output.
class ConsoleViewer
{
public:
        /// Initialize.
        /// @param stream Output stream. Must exist for the entire duration of
        /// the outputter's use.
        explicit ConsoleViewer(std::ostream& stream = std::cout)
            : m_descriptors(Benchmarker::Instance().GetTestDescriptors()), m_stream(stream)
        {
        }

        void Update(const event::Payload& payload);

private:
        TestDescriptors m_descriptors;
        std::ostream&   m_stream;
};

} // namespace myhayai

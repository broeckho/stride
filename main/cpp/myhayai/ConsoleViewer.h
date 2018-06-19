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
 */
/**
 * @file
 * Header file for ConsoleViewer.
 */

#include "BenchmarkRunner.h"
#include "Console.h"
#include "Payload.h"
#include "TestDescriptors.h"

#include <chrono>
#include <ostream>

namespace myhayai {

/**
 * Console viewer outputs (on the console) info on the benchmarks as they run.
 *
 * \tparam T   Time units used in reporting.
 */
template <typename T>
class ConsoleViewer
{
public:
        /// Initialize.
        /// @param stream   Output stream. Must exist for the duration of the outputter's use.
        explicit ConsoleViewer(std::ostream& stream = std::cout, bool noColor = false)
            : m_descriptors(BenchmarkRunner::Instance().GetTestDescriptors()), m_stream(stream)
        {
                if (noColor) {
                        stream << console::Color::DisableColor;
                } else {
                        stream << console::Color::EnableColor;
                }
        }

        /// Observer method, registered with subject.
        void Update(const event::Payload& payload);

private:
        TestDescriptors m_descriptors;
        std::ostream&   m_stream;
};

extern template class ConsoleViewer<std::chrono::microseconds>;
extern template class ConsoleViewer<std::chrono::milliseconds>;
extern template class ConsoleViewer<std::chrono::seconds>;

} // namespace myhayai

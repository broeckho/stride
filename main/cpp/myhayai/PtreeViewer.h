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
 * Header file for PtreeViewer.
 */

#include "BenchmarkRunner.h"
#include "Payload.h"
#include "TestDescriptors.h"

#include <boost/property_tree/ptree.hpp>
#include <chrono>

namespace myhayai {

/**
 *  Constructs a (Boost) propert tree with info about the tests run in
 *  the benchmark.
 *
 *  \tparam T   Time units used in reporting.
 */
template <typename T>
class PtreeViewer
{
public:
        /// Initialize.
        /// @param stream Output stream. Must exist for the entire duration of
        /// the outputter's use.
        explicit PtreeViewer() : m_descriptors(BenchmarkRunner::Instance().GetTestDescriptors()), m_ptree() {}

        /// Get reference to the ptree.
        const boost::property_tree::ptree& CGet() { return m_ptree; }

        /// Observer method, registered with the subject.
        void Update(const event::Payload& payload);

private:
        TestDescriptors             m_descriptors;
        boost::property_tree::ptree m_ptree;
};

extern template class PtreeViewer<std::chrono::microseconds>;
extern template class PtreeViewer<std::chrono::milliseconds>;
extern template class PtreeViewer<std::chrono::seconds>;

} // namespace myhayai

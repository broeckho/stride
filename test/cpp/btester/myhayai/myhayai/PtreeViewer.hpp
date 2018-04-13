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

#include "BenchmarkRunner.hpp"
#include "Payload.hpp"
#include "TestDescriptors.hpp"

#include <boost/property_tree/ptree.hpp>

namespace myhayai {

/// Constructs ptree with benchmark info.
class PtreeViewer
{
public:
        /// Initialize.
        /// @param stream Output stream. Must exist for the entire duration of
        /// the outputter's use.
        explicit PtreeViewer() : m_descriptors(BenchmarkRunner::Instance().GetTestDescriptors()), m_ptree() {}

        ///
        const boost::property_tree::ptree& CGet() { return m_ptree; }

        ///
        void Update(const event::Payload& payload);

private:
        TestDescriptors             m_descriptors;
        boost::property_tree::ptree m_ptree;
};

} // namespace myhayai

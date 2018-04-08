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
 * Header file for TestParametersDescriptor.
 */

#include "TestParameterDescriptor.hpp"

#include <boost/property_tree/ptree.hpp>
#include <ostream>

namespace myhayai {

/// Test parameters descriptor.
struct TestParametersDescriptor
{
        TestParametersDescriptor() : m_num_threads(0U) {}
        unsigned int m_num_threads;
};

inline boost::property_tree::ptree& operator<<(boost::property_tree::ptree& pt, const TestParametersDescriptor& t_d)
{
        pt.put("num_threads", t_d.m_num_threads);
        return pt;
}

inline std::ostream& operator<<(std::ostream& os, const TestParametersDescriptor& t_d)
{
        os << "num_threads" << t_d.m_num_threads << std::endl;
        return os;
}

} // namespace myhayai

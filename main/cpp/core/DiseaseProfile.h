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
 * Disease profile.
 */

#include <boost/property_tree/ptree.hpp>

namespace stride {

class DiseaseProfile
{
public:
        /// Initialize.
        DiseaseProfile() : m_transmission_rate(0.0), m_is_operational(true) {}

        /// Return transmission rate.
        double GetTransmissionRate() const { return m_transmission_rate; }

        /// Check if the disease configuration was valid.
        bool IsOperational() const { return m_is_operational; }

        /// Initialize.
        bool Initialize(const boost::property_tree::ptree& pt_config, const boost::property_tree::ptree& pt_disease);

private:
        double m_transmission_rate;
        bool   m_is_operational;
};

} // namespace stride

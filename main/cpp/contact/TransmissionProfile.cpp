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
 * Omplemenatation for the TransmissionProfile class.
 */

#include "TransmissionProfile.h"

#include <cmath>

namespace stride {

using namespace std;
using namespace boost::property_tree;


void TransmissionProfile::Initialize(const ptree& configPt, const ptree& diseasePt)
{
	boost::optional<double> r0 = configPt.get_optional<double>("run.r0");
	if (r0) {
	        // Use a quadratic model, fitted to simulation data:
	        // Expected(R0) = (0 + b1*transm_probability + b2*transm_probability^2).
	        const auto b0 = diseasePt.get<double>("disease.transmission.b0");
	        const auto b1 = diseasePt.get<double>("disease.transmission.b1");
	        const auto b2 = diseasePt.get<double>("disease.transmission.b2");

	        // Find root
	        const auto a = b2;
	        const auto b = b1;
	        const auto c = b0 - *r0;

	        // To obtain a real values (instead of complex)
	        if (*r0 < (-(b * b) / (4 * a))) {
	                const double determ = (b * b) - 4 * a * c;
	                double transmission_rate = (-b + sqrt(determ)) / (2 * a);
	                m_transmission_probability = 1.0 - std::exp(-transmission_rate);
	        } else {
	                throw runtime_error("TransmissionProfile::Initialize> Illegal input values.");
	        }

	} else {
		// Use transmission probability parameter
		m_transmission_probability = configPt.get<double>("run.transmission_probability");
	}
}

} // namespace stride

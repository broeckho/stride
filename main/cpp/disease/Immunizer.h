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
 * Header for the Immunizer class.
 */

#include "pool/ContactPool.h"

#include <vector>

namespace stride {

namespace util {
class RNManager;
}

class Immunizer
{
public:
        ///
        explicit Immunizer(util::RNManager& rnManager);

        /// Random immunization.
        void Random(const std::vector<ContactPool>& pools, std::vector<double>& immunityDistribution,
                    double immunityLinkProbability);

        /// Cocoon immunization.
        void Cocoon(const std::vector<ContactPool>& pools, std::vector<double>& immunityDistribution,
                    double immunityLinkProbability);

private:
        util::RNManager& m_rn_manager; ///< Random number manager.
};

} // namespace stride

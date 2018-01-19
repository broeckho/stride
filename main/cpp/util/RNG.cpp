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
 *  Copyright 2017, De Pauw J, Draulans S, Leys T, Truyts T, Van Leeuwen L
 */

#include "RNGProperty.h"

namespace stride {
namespace config {

RNGMap rngMap;

std::string RNGProperty::validate(const std::string& rng_value) const
{
        if (rngMap.find(rng_value) != rngMap.end()) {
                return rng_value;
        } else {
                throw std::runtime_error("Unknown RNG: " + rng_value);
        }
}

std::shared_ptr<RNGInterface> RNGProperty::getRNG() { return rngMap[this->value](); }

} // namespace config
} // namespace stride

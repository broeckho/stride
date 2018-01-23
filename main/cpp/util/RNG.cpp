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

#include "RNG.h"

namespace stride {
namespace config {

using stride::util::RNGInterface;

RNGMap rngMap;

std::string RNGvalidate(const std::string& id)
{
        if (rngMap.find(id) != rngMap.end()) {
                return id;
        } else {
                throw std::runtime_error("Unknown RNG: " + id);
        }
}

std::shared_ptr<RNGInterface> RNGget(const std::string& id) { return rngMap[id](); }

} // namespace config
} // namespace stride

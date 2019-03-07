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
 * Interface of RnMan.
 */

#pragma once

#include "Rn.h"

namespace stride {
namespace util {

using RnPcg64 = Rn<pcg64>;
// using RnLcg64 = Rn<trng::lcg64>;

/*
 * A using statement here, would bar having RnMan in forward class declarations.
 */
class RnMan : public RnPcg64
{
public:
        using RnPcg64::RnPcg64; // make constructors accessible.
};

} // namespace util
} // namespace stride

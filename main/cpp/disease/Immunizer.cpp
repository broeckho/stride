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
 * Implementation for the Immunizer class.
 */

#include "Immunizer.h"

#include "pop/Person.h"
#include "util/RnMan.h"

#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>
#include <numeric>
#include <vector>

namespace stride {

using namespace std;
using namespace util;

Immunizer::Immunizer(stride::util::RnMan& rnManager) : m_rn_manager(rnManager) {}

} // namespace stride

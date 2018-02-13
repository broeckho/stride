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
 * Parser for family data file.
 */

#include <fstream>
#include <string>
#include <vector>

namespace stride {
namespace popgen {

using uint         = unsigned int;
using FamilyConfig = std::vector<uint>;

class FamilyParser
{
public:
        FamilyParser() = default;

        ~FamilyParser() = default;

        std::vector<FamilyConfig> parseFamilies(std::string filename) const;

private:
        FamilyConfig parseFamily(std::string config) const;
};

} // namespace popgen
} // namespace stride
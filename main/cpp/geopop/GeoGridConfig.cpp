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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "GeoGridConfig.h"

#include "geopop/io/HouseholdReader.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <iomanip>

namespace geopop {

using stride::util::intToDottedString;
using namespace std;
using namespace boost::property_tree;

GeoGridConfig::GeoGridConfig() : input{}, refHH{}, popInfo{}, pools{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt) : GeoGridConfig()
{
        input.pop_size                     = configPt.get<unsigned int>("run.geopop_gen.population_size");
        input.participation_college        = configPt.get<double>("run.geopop_gen.participation_college");
        input.fraction_workplace_commuters = configPt.get<double>("run.geopop_gen.fraction_workplace_commuters");
        input.fraction_college_commuters   = configPt.get<double>("run.geopop_gen.fraction_college_commuters");
        input.particpation_workplace       = configPt.get<double>("run.geopop_gen.particpation_workplace");
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int width = 53;
        out << left << "Input:" << endl;
        out << left << setw(width) << "Fraction college commuters:" << config.input.fraction_college_commuters << endl;
        out << left << setw(width) << "Fraction workplace commuters:" << config.input.fraction_workplace_commuters
            << endl;

        out << left << setw(width) << "Participation fraction of college:" << config.input.participation_college
            << endl;
        out << left << setw(width) << "Participation fraaction of workplace:" << config.input.particpation_workplace
            << endl;

        out << left << setw(width) << "Target population size" << intToDottedString(config.input.pop_size) << endl;
        out << endl;

        out << left << "Calculated:" << endl;
        out << left << setw(width)
            << "Number of K12School students:" << intToDottedString(config.popInfo.popcount_k12school) << endl;
        out << left << setw(width)
            << "Number of college students:" << intToDottedString(config.popInfo.popcount_college) << endl;
        out << left << setw(width)
            << "Number of people with workplace:" << intToDottedString(config.popInfo.popcount_workplace) << endl;
        out << endl;
        return out;
}

} // namespace geopop

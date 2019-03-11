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
        const int w = 53;
        out.setf(std::ios_base::left, std::ios_base::adjustfield);
        out << "Input:" << endl;
        out << setw(w) << "Fraction college commuters:" << config.input.fraction_college_commuters << "\n";
        out << setw(w) << "Fraction workplace commuters:" << config.input.fraction_workplace_commuters << "\n";
        out << setw(w) << "Participation fraction of college:" << config.input.participation_college << "\n";
        out << setw(w) << "Participation fraaction of workplace:" << config.input.particpation_workplace << "\n";
        out << setw(w) << "Target population size" << intToDottedString(config.input.pop_size) << "\n"
            << "\n";
        out << "Calculated:"
            << "\n";
        out << setw(w) << "K12School student count:" << intToDottedString(config.popInfo.popcount_k12school) << "\n";
        out << setw(w) << "College student count:" << intToDottedString(config.popInfo.popcount_college) << "\n";
        out << setw(w) << "Workplace person count:" << intToDottedString(config.popInfo.popcount_workplace) << "\n";
        out << endl;
        return out;
}

} // namespace geopop

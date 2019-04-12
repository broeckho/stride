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

#include "contact/AgeBrackets.h"
#include "contact/ContactType.h"
#include "geopop/io/HouseholdReader.h"
#include "geopop/io/ReaderFactory.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <iomanip>

namespace geopop {

using namespace std;
using namespace boost::property_tree;
using namespace stride::AgeBrackets;
using namespace stride::ContactType;
using stride::util::intToDottedString;

GeoGridConfig::GeoGridConfig() : param{}, refHH{}, info{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt) : GeoGridConfig()
{
        const auto pt = configPt.get_child("run.geopop_gen");
        param.pop_size                     = pt.get<unsigned int>("population_size");
        param.participation_college        = pt.get<double>("participation_college");
        param.fraction_workplace_commuters = pt.get<double>("fraction_workplace_commuters");
        param.fraction_college_commuters   = pt.get<double>("fraction_college_commuters");
        param.particpation_workplace       = pt.get<double>("particpation_workplace");

        people[Id::K12School]              = pt.get<unsigned int>("people_per_K12School", 500U);
        people[Id::College]                = pt.get<unsigned int>("people_per_College", 3000U);
        people[Id::Workplace]              = pt.get<unsigned int>("people_per_Workplace", 20U);
        people[Id::PrimaryCommunity]       = pt.get<unsigned int>("people_per_PrimaryCommunity", 2000U);
        people[Id::SecondaryCommunity]     = pt.get<unsigned int>("people_per_SecondaryCommunity", 2000U);

        pools[Id::K12School]              = pt.get<unsigned int>("pools_per_K12School", 25U);
        pools[Id::College]                = pt.get<unsigned int>("pools_per_College", 20U);
}

void GeoGridConfig::SetData(const string& householdsFileName)
{
        auto householdsReader = ReaderFactory::CreateHouseholdReader(householdsFileName);
        householdsReader->SetReferenceHouseholds(refHH.person_count, refHH.ages);
        const auto popSize = param.pop_size;

        //----------------------------------------------------------------
        // Determine age makeup of reference houshold population.
        //----------------------------------------------------------------
        const auto ref_p_count   = refHH.person_count;
        const auto averageHhSize = static_cast<double>(ref_p_count) / static_cast<double>(refHH.ages.size());

        auto ref_k12school = 0U;
        auto ref_college   = 0U;
        auto ref_workplace = 0U;
        for (const auto& hhAgeProfile : refHH.ages) {
                for (const auto& age : hhAgeProfile) {
                        if (K12School::HasAge(age)) {
                                ref_k12school++;
                        }
                        if (College::HasAge(age)) {
                                ref_college++;
                        }
                        if (Workplace::HasAge(age)) {
                                ref_workplace++;
                        }
                }
        }
        //----------------------------------------------------------------
        // Scale up to the generated population size.
        //----------------------------------------------------------------
        const auto fraction_k12school_age = static_cast<double>(ref_k12school) / static_cast<double>(ref_p_count);
        const auto fraction_college_age   = static_cast<double>(ref_college) / static_cast<double>(ref_p_count);
        const auto fraction_workplace_age = static_cast<double>(ref_workplace) / static_cast<double>(ref_p_count);

        const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
        const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
        const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

        info.popcount_k12school = age_count_k12school;

        info.popcount_college = static_cast<unsigned int>(floor(param.participation_college * age_count_college));

        info.popcount_workplace = static_cast<unsigned int>(
            floor(param.particpation_workplace * (age_count_workplace - info.popcount_college)));

        info.count_households = static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int w = 53;
        out.setf(std::ios_base::left, std::ios_base::adjustfield);
        out << "Input:" << endl;
        out << setw(w) << "Fraction college commuters:" << config.param.fraction_college_commuters << "\n";
        out << setw(w) << "Fraction workplace commuters:" << config.param.fraction_workplace_commuters << "\n";
        out << setw(w) << "Participation fraction of college:" << config.param.participation_college << "\n";
        out << setw(w) << "Participation fraaction of workplace:" << config.param.particpation_workplace << "\n";
        out << setw(w) << "Target population size" << intToDottedString(config.param.pop_size) << "\n"
            << "\n";
        out << "Calculated:"
            << "\n";
        out << setw(w) << "K12School student count:" << intToDottedString(config.info.popcount_k12school) << "\n";
        out << setw(w) << "College student count:" << intToDottedString(config.info.popcount_college) << "\n";
        out << setw(w) << "Workplace person count:" << intToDottedString(config.info.popcount_workplace) << "\n";
        out << endl;
        return out;
}

} // namespace geopop

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
 * Core Population class
 */

#include "Population.h"

#include "behaviour/belief_policies/Imitation.h"
#include "behaviour/belief_policies/NoBelief.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree.hpp>
#include <cassert>

using namespace boost::property_tree;
using namespace std;

namespace stride {


unsigned int Population::GetInfectedCount() const
{
        unsigned int total{0U};
        for (const auto& p : *this) {
                const auto& h = p.GetHealth();
                total += h.IsInfected() || h.IsRecovered();
        }
        return total;
}

unsigned int Population::GetAdoptedCount() const
{
        unsigned int total{0U};
        for (const auto& p : *this) {
                auto belief  = p.GetBelief();
                bool adopted = belief->HasAdopted();
                if (adopted) {
                        total++;
                }
        }
        return total;
}

template <typename BeliefPolicy>
void Population::NewPerson(unsigned int id, double age, unsigned int household_id, unsigned int school_id,
                           unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
                           unsigned int start_infectiousness, unsigned int start_symptomatic, unsigned int time_infectious,
                           unsigned int time_symptomatic, const ptree& pt_belief, double risk_averseness
                           )
{
        static util::SegmentedVector<BeliefPolicy> beliefs_container;
        const BeliefPolicy                         b(pt_belief);

        assert(this->size() == beliefs_container.size() && "Person and Beliefs container sizes not equal!");

        BeliefPolicy* bp = beliefs_container.emplace_back(b);
        this->emplace_back(Person(id, age, household_id, school_id, work_id, primary_community_id,
                                  secondary_community_id, start_infectiousness, start_symptomatic,
                                  time_infectious, time_symptomatic, risk_averseness, bp));

        assert(this->size() == beliefs_container.size() && "Person and Beliefs container sizes not equal!");
}


void Population::CreatePerson(unsigned int id, double age, unsigned int household_id, unsigned int school_id,
                  unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
                  unsigned int start_infectiousness, unsigned int start_symptomatic,
                  unsigned int time_infectious, unsigned int time_symptomatic, const ptree& pt_belief, double risk_averseness
                  )
{
        string belief_policy = pt_belief.get<string>("name");
        if (belief_policy == "NoBelief") {
                NewPerson<NoBelief>(id, age, household_id, school_id, work_id, primary_community_id,
                                    secondary_community_id, start_infectiousness, start_symptomatic,
                                    time_infectious, time_symptomatic, pt_belief, risk_averseness);
        } else if (belief_policy == "Imitation") {
                NewPerson<Imitation>(id, age, household_id, school_id, work_id, primary_community_id,
                                     secondary_community_id, start_infectiousness, start_symptomatic,
                                     time_infectious, time_symptomatic, pt_belief, risk_averseness);
        } else {
                throw runtime_error(string(__func__) + "No valid belief policy!");
        }
}

} // namespace stride

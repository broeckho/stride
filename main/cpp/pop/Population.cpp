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
#include "disease/Health.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <utility>

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
                if (p.GetBelief()->HasAdopted()) {
                        total++;
                }
        }
        return total;
}

template <typename BeliefPolicy>
void Population::NewPerson(unsigned int id, double age, unsigned int household_id, unsigned int school_id,
                           unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
                           Health health, const ptree& belief_pt, double risk_averseness)
{
        if (!beliefs_container) {
                beliefs_container.emplace<util::SegmentedVector<BeliefPolicy>>();
        }
        auto container = beliefs_container.cast<util::SegmentedVector<BeliefPolicy>>();

        assert(this->size() == container->size() && "Person and Beliefs container sizes not equal!");

        BeliefPolicy* bp = container->emplace_back(belief_pt);
        this->emplace_back(Person(id, age, household_id, school_id, work_id, primary_community_id,
                                  secondary_community_id, health, risk_averseness, bp));

        assert(this->size() == container->size() && "Person and Beliefs container sizes not equal!");
}

void Population::CreatePerson(unsigned int id, double age, unsigned int household_id, unsigned int school_id,
                              unsigned int work_id, unsigned int primary_community_id,
                              unsigned int secondary_community_id, Health health, const ptree& belief_pt,
                              double risk_averseness)
{
        string belief_policy = belief_pt.get<string>("name");

        if (belief_policy == "NoBelief") {
                NewPerson<NoBelief>(id, age, household_id, school_id, work_id, primary_community_id,
                                    secondary_community_id, health, belief_pt, risk_averseness);
        } else if (belief_policy == "Imitation") {
                NewPerson<Imitation>(id, age, household_id, school_id, work_id, primary_community_id,
                                     secondary_community_id, health, belief_pt, risk_averseness);
        } else {
                throw runtime_error(string(__func__) + "No valid belief policy!");
        }
}

} // namespace stride

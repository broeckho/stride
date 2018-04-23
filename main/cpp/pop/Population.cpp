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

unsigned int Population::GetInfectedCount() const
{
        unsigned int total{0U};
        for (const auto& p : *this) {
                const auto& h = p.GetHealth();
                total += h.IsInfected() || h.IsRecovered();
        }
        return total;
}

void Population::CreatePerson(unsigned int id, double age, unsigned int householdId, unsigned int schoolId,
                              unsigned int workId, unsigned int primaryCommunityId, unsigned int secondaryCommunityId,
                              Health health, const ptree& beliefPt, double riskAverseness)
{
        string belief_policy = beliefPt.get<string>("name");

        if (belief_policy == "NoBelief") {
                NewPerson<NoBelief>(id, age, householdId, schoolId, workId, primaryCommunityId, secondaryCommunityId,
                                    health, beliefPt, riskAverseness);
        } else if (belief_policy == "Imitation") {
                NewPerson<Imitation>(id, age, householdId, schoolId, workId, primaryCommunityId, secondaryCommunityId,
                                     health, beliefPt, riskAverseness);
        } else {
                throw runtime_error(string(__func__) + "No valid belief policy!");
        }
}

template <typename BeliefPolicy>
void Population::NewPerson(unsigned int id, double age, unsigned int householdId, unsigned int schoolId,
                           unsigned int workId, unsigned int primaryCommunityId, unsigned int secondaryCommunityId,
                           Health health, const ptree& beliefPt, double riskAverseness)
{
        if (!beliefs_container) {
                beliefs_container.emplace<util::SegmentedVector<BeliefPolicy>>();
        }
        auto container = beliefs_container.cast<util::SegmentedVector<BeliefPolicy>>();

        assert(this->size() == container->size() && "Person and Beliefs container sizes not equal!");

        BeliefPolicy* bp = container->emplace_back(beliefPt);
        this->emplace_back(Person(id, age, householdId, schoolId, workId, primaryCommunityId, secondaryCommunityId,
                                  health, riskAverseness, bp));

        assert(this->size() == container->size() && "Person and Beliefs container sizes not equal!");
}

} // namespace stride

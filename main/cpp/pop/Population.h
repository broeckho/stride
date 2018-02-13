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
 * Header file for the core Population class
 */
#include "behaviour/belief_policies/Imitation.h"
#include "behaviour/belief_policies/NoBelief.h"
#include "pop/Person.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <vector>

#include <iostream>

namespace stride {

/**
 * Container for persons in population.
 */
class Population : public std::vector<Person>
{
public:
        /// Get the cumulative number of cases.
        unsigned int GetInfectedCount() const
        {
                unsigned int total{0U};
                for (const auto& p : *this) {
                        const auto& h = p.GetHealth();
                        total += h.IsInfected() || h.IsRecovered();
                }
                return total;
        }

        double GetFractionInfected() const { return GetInfectedCount() / this->size(); }

        unsigned int GetAdoptedCount() const
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

        void CreatePerson(unsigned int id, double age, unsigned int household_id, unsigned int school_id,
                          unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
                          unsigned int start_infectiousness, unsigned int start_symptomatic,
                          unsigned int time_infectious, unsigned int time_symptomatic, double risk_averseness = 0,
                          boost::property_tree::ptree pt_belief = boost::property_tree::ptree())
        {
                std::string belief_policy = pt_belief.get<std::string>("name"); // TODO default?
                if (belief_policy == "NoBelief") {
                        NewPerson<NoBelief>(id, age, household_id, school_id, work_id, primary_community_id,
                                            secondary_community_id, start_infectiousness, start_symptomatic,
                                            time_infectious, time_symptomatic, risk_averseness, pt_belief);
                } else if (belief_policy == "Imitation") {
                        NewPerson<Imitation>(id, age, household_id, school_id, work_id, primary_community_id,
                                             secondary_community_id, start_infectiousness, start_symptomatic,
                                             time_infectious, time_symptomatic, risk_averseness, pt_belief);
                } else {
                        throw std::runtime_error(std::string(__func__) + "No valid belief policy!");
                }
        }

private:
        template <typename BeliefPolicy>
        void NewPerson(unsigned int id, double age, unsigned int household_id, unsigned int school_id,
                       unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
                       unsigned int start_infectiousness, unsigned int start_symptomatic, unsigned int time_infectious,
                       unsigned int time_symptomatic, double risk_averseness = 0,
                       boost::property_tree::ptree pt_belief = boost::property_tree::ptree())
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
};

} // namespace stride

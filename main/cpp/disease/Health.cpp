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
 *  Copyright 2017, 2018 Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation for the Health class.
 */

#include "Health.h"

#include "util/Assert.h"

namespace stride {

Health::Health(unsigned short int start_infectiousness, unsigned short int start_symptomatic,
               unsigned short int time_infectious, unsigned short int time_symptomatic)
    : m_disease_counter(0U), m_status(HealthStatus::Susceptible), m_start_infectiousness(start_infectiousness),
      m_start_symptomatic(start_symptomatic), m_end_infectiousness(start_infectiousness + time_infectious),
      m_end_symptomatic(start_symptomatic + time_symptomatic)
{
}

void Health::StartInfection()
{
        AssertThrow(m_status == HealthStatus::Susceptible, "Inconsistent Health change", nullptr);
        m_status = HealthStatus::Exposed;
        ResetDiseaseCounter();
}

void Health::StopInfection()
{
        AssertThrow(IsInfected(), "Person not infected", nullptr);
        m_status = HealthStatus::Recovered;
}

void Health::Update()
{
        if (IsInfected()) {
                IncrementDiseaseCounter();
                if (GetDiseaseCounter() == m_start_infectiousness) {
                        if (m_status == HealthStatus::Symptomatic) {
                                m_status = HealthStatus::InfectiousAndSymptomatic;
                        } else {
                                m_status = HealthStatus::Infectious;
                        }
                }
                if (GetDiseaseCounter() == m_start_symptomatic) {
                        if (m_status == HealthStatus::Infectious) {
                                m_status = HealthStatus::InfectiousAndSymptomatic;
                        } else {
                                m_status = HealthStatus::Symptomatic;
                        }
                }
                if (GetDiseaseCounter() == m_end_symptomatic) {
                        if (m_status == HealthStatus::InfectiousAndSymptomatic) {
                                m_status = HealthStatus::Infectious;
                        } else {
                                StopInfection();
                        }
                }
                if (GetDiseaseCounter() == m_end_infectiousness) {
                        if (m_status == HealthStatus::InfectiousAndSymptomatic) {
                                m_status = HealthStatus::Symptomatic;
                        } else {
                                StopInfection();
                        }
                }
        }
}

} // namespace stride

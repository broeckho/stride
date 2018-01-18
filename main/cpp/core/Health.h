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

namespace stride {

/*
 *
 */
enum class HealthStatus
{
        Susceptible = 0U,
        Exposed = 1U,
        Infectious = 2U,
        Symptomatic = 3U,
        InfectiousAndSymptomatic = 4U,
        Recovered = 5U,
        Immune = 6U,
        Null
};

/*
 *
 */
class Health
{
public:
        ///
        Health(unsigned int start_infectiousness, unsigned int start_symptomatic, unsigned int time_infectious,
               unsigned int time_symptomatic);

        ///
        HealthStatus GetHealthStatus() const { return m_status; }

        ///
        unsigned int GetEndInfectiousness() const { return m_end_infectiousness; }

        ///
        unsigned int GetEndSymptomatic() const { return m_end_symptomatic; }

        ///
        unsigned int GetStartInfectiousness() const { return m_start_infectiousness; }

        ///
        unsigned int GetStartSymptomatic() const { return m_start_symptomatic; }

        ///
        bool IsImmune() const { return m_status == HealthStatus::Immune; }

        ///
        bool IsInfected() const
        {
                return m_status == HealthStatus::Exposed || m_status == HealthStatus::Infectious ||
                       m_status == HealthStatus::InfectiousAndSymptomatic || m_status == HealthStatus::Symptomatic;
        }

        ///
        bool IsInfectious() const
        {
                return m_status == HealthStatus::Infectious || m_status == HealthStatus::InfectiousAndSymptomatic;
        }

        ///
        bool IsRecovered() const { return m_status == HealthStatus::Recovered; }

        /// Is this person susceptible?
        bool IsSusceptible() const { return m_status == HealthStatus::Susceptible; }

        /// Is this person symptomatic?
        bool IsSymptomatic() const
        {
                return m_status == HealthStatus::Symptomatic || m_status == HealthStatus::InfectiousAndSymptomatic;
        }

        /// Set health state to immune.
        void SetImmune();

        /// Set health state to susceptible
        void SetSusceptible();

        /// Start the infection.
        void StartInfection();

        /// Stop the infection.
        void StopInfection();

        /// Update progress of the disease.
        void Update();

private:
        /// Get the disease counter.
        unsigned int GetDiseaseCounter() const { return m_disease_counter; }

        /// Increment disease counter.
        void IncrementDiseaseCounter() { m_disease_counter++; }

        /// Reset the disease counter.
        void ResetDiseaseCounter() { m_disease_counter = 0U; }

private:
        unsigned int m_disease_counter; ///< The disease counter.
        HealthStatus m_status;          ///< The current status of the person w.r.t. the disease.

        unsigned int m_start_infectiousness; ///< Days after infection to become infectious.
        unsigned int m_start_symptomatic;    ///< Days after infection to become symptomatic.
        unsigned int m_end_infectiousness;   ///< Days after infection to end infectious state.
        unsigned int m_end_symptomatic;      ///< Days after infection to end symptomatic state.
};

} // namespace stride

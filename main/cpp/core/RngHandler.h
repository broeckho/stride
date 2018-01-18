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
 * Header for the ContactHandler class.
 */

#include "math.h"
#include "util/Random.h"

namespace stride {

/**
 * Processes the contacts between persons and determines whether transmission
 * occurs.
 */
class RngHandler
{
public:
        /// Constructor sets the transmission rate and random number generator.
        RngHandler(unsigned int seed, unsigned int stream_count, unsigned int id) : m_rng(seed)
        {
                m_rng.Split(stream_count, id);
        }

        /// Convert rate into probability
        double RateToProbability(double rate) { return 1 - exp(-rate); }

        /// Check if two individuals have transmission.
        bool HasContactAndTransmission(double contact_rate, double transmission_rate)
        {
                return m_rng.NextDouble() < RateToProbability(transmission_rate * contact_rate);
        }

        /// Check if two individuals have contact.
        bool HasContact(double contact_rate) { return m_rng.NextDouble() < RateToProbability(contact_rate); }

        ///
        bool HasTransmission(double transmission_rate)
        {
                return m_rng.NextDouble() < RateToProbability(transmission_rate);
        }

private:
        util::Random m_rng; ///< Random number engine.
};

} // namespace stride

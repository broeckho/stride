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
 * Implementation of Infector algorithms.
 */

#include "Infector.h"

#include "calendar/Calendar.h"
#include "pool/ContactPool.h"
#include "pop/Person.h"

#include <memory>

namespace stride {

/// Primary LOG_POLICY policy, implements LogMode::None.
/// \tparam LL
template <ContactLogMode::Id LL>
class LOG_POLICY
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                            unsigned short int)
        {
        }

        static void Trans(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                          unsigned short int)
        {
        }
};

/// Specialized LOG_POLICY policy LogMode::Transmissions.
template <>
class LOG_POLICY<ContactLogMode::Id::Transmissions>
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                            unsigned short int)
        {
        }

        static void Trans(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                          ContactPoolType::Id type, unsigned short int sim_day)
        {
                logger->info("[TRAN] {} {} {} {}", p1->GetId(), p2->GetId(), ToString(type), sim_day);
        }
};

/// Specialized LOG_POLICY policy LogMode::All.
template <>
class LOG_POLICY<ContactLogMode::Id::All>
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                            ContactPoolType::Id type, unsigned short int sim_day)
        {
                if (p1->IsSurveyParticipant()) {
                        logger->info("[CONT] {} {} {} {} {} {} {} {} {}", p1->GetId(), p1->GetAge(), p2->GetAge(),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::Household),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::School),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::Work),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::PrimaryCommunity),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::SecondaryCommunity),
                                     sim_day);
                }
        }

        static void Trans(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                          ContactPoolType::Id type, unsigned short int sim_day)
        {
                logger->info("[TRAN] {} {} {} {}", p1->GetId(), p2->GetId(), ToString(type), sim_day);
        }
};

/// Specialized LOG_POLICY policy LogMode::Susceptibles.
template <>
class LOG_POLICY<ContactLogMode::Id::Susceptibles>
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                            ContactPoolType::Id, unsigned short int)
        {
                if (p1->IsSurveyParticipant() && p1->GetHealth().IsSusceptible() && p2->GetHealth().IsSusceptible()) {
                        logger->info("[CONT] {} {}", p1->GetId(), p2->GetId());
                }
        }

        static void Trans(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                          unsigned short int)
        {
        }
};

} // namespace stride

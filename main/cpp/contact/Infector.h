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
 * Header for the Infector class.
 */

#include "ContactLogMode.h"
#include "TransmissionProfile.h"
#include "behaviour/information_policies/LocalDiscussion.h"
#include "behaviour/information_policies/NoLocalInformation.h"
#include "calendar/Calendar.h"
#include "contact/AgeContactProfile.h"
#include "contact/ContactHandler.h"

#include <memory>
#include <spdlog/spdlog.h>

namespace stride {

class ContactPool;

/// Indicates whether optimized implementation may be used.
/// \tparam LL          LogLevel
/// \tparam LIP         LocalInformationPolicy
template <ContactLogMode::Id LL, typename LIP>
struct UseOptimizedInfector
{
        static constexpr bool value = false;
};

/// Indicates whether optimized implementation may be used.
template <>
struct UseOptimizedInfector<ContactLogMode::Id::None, NoLocalInformation>
{
        static constexpr bool value = true;
};

/// Indicates whether optimized implementation may be used.
template <>
struct UseOptimizedInfector<ContactLogMode::Id::Transmissions, NoLocalInformation>
{
        static constexpr bool value = true;
};

/// Actual contacts and transmission in contactpool (primary template).
/// \tparam LL          LogLevel
/// \tparam TIC         TrackIndexCase
/// \tparam LIP         LocalInformationPolicy
template <ContactLogMode::Id LL, bool TIC, typename LIP, bool TO = UseOptimizedInfector<LL, LIP>::value>
class Infector
{
public:
        ///
        static void Exec(ContactPool& pool, const AgeContactProfile& profile, const TransmissionProfile& trans_profile,
                         ContactHandler& c_handler, unsigned short int sim_day,
                         std::shared_ptr<spdlog::logger> c_logger);
};

/// Time-optimized version (Only for NoLocalInformation policy in combination with None || Transmission logging).
/// \tparam LL          LogLevel
/// \tparam TIC         TrackIndexCase
template <ContactLogMode::Id LL, bool TIC>
class Infector<LL, TIC, NoLocalInformation, true>
{
public:
        ///
        static void Exec(ContactPool& pool, const AgeContactProfile& profile, const TransmissionProfile& trans_profile,
                         ContactHandler& c_handler, unsigned short int sim_day,
                         std::shared_ptr<spdlog::logger> c_logger);
};

/// Explicit instantiations in cpp file.
extern template class Infector<ContactLogMode::Id::None, false, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::None, false, LocalDiscussion>;
extern template class Infector<ContactLogMode::Id::None, true, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::None, true, LocalDiscussion>;

extern template class Infector<ContactLogMode::Id::Transmissions, false, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::Transmissions, false, LocalDiscussion>;
extern template class Infector<ContactLogMode::Id::Transmissions, true, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::Transmissions, true, LocalDiscussion>;

extern template class Infector<ContactLogMode::Id::All, false, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::All, false, LocalDiscussion>;
extern template class Infector<ContactLogMode::Id::All, true, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::All, true, LocalDiscussion>;

extern template class Infector<ContactLogMode::Id::Susceptibles, false, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::Susceptibles, false, LocalDiscussion>;
extern template class Infector<ContactLogMode::Id::Susceptibles, true, NoLocalInformation>;
extern template class Infector<ContactLogMode::Id::Susceptibles, true, LocalDiscussion>;

} // namespace stride

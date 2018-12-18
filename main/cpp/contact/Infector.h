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

#pragma once

#include "ContactLogMode.h"
#include "TransmissionProfile.h"
#include "calendar/Calendar.h"
#include "contact/AgeContactProfile.h"
#include "contact/ContactHandler.h"

#include <memory>
#include <spdlog/spdlog.h>

namespace {

using namespace stride;

/// Indicates whether optimized implementation may be used.
/// \tparam LL          LogLevel
template <ContactLogMode::Id LL>
struct UseOptimizedInfector
{
        static constexpr bool value = false;
};

/// Indicates whether optimized implementation may be used.
template <>
struct UseOptimizedInfector<ContactLogMode::Id::None>
{
        static constexpr bool value = true;
};

/// Indicates whether optimized implementation may be used.
template <>
struct UseOptimizedInfector<ContactLogMode::Id::Transmissions>
{
        static constexpr bool value = true;
};

} // namespace

namespace stride {

class ContactPool;

/// Actual contacts and transmission in contactpool (primary template).
/// \tparam LL          LogLevel
/// \tparam TIC         TrackIndexCase
/// \tparam LIP         LocalInformationPolicy
template <ContactLogMode::Id LL, bool TIC, bool TO = UseOptimizedInfector<LL>::value>
class Infector
{
public:
        ///
        static void Exec(ContactPool& pool, const AgeContactProfile& profile, const TransmissionProfile& transProfile,
                         ContactHandler& cHandler, unsigned short int simDay, std::shared_ptr<spdlog::logger> cLogger);
};

/// Time-optimized version (For NoLocalInformation && (None || Transmission logging)).
/// \tparam LL          LogLevel
/// \tparam TIC         TrackIndexCase
template <ContactLogMode::Id LL, bool TIC>
class Infector<LL, TIC, true>
{
public:
        ///
        static void Exec(ContactPool& pool, const AgeContactProfile& profile, const TransmissionProfile& transProfile,
                         ContactHandler& cHandler, unsigned short int simDay, std::shared_ptr<spdlog::logger> cLogger);
};

/// Explicit instantiations in cpp file.
extern template class Infector<ContactLogMode::Id::None, false>;
extern template class Infector<ContactLogMode::Id::None, true>;
extern template class Infector<ContactLogMode::Id::Transmissions, false>;
extern template class Infector<ContactLogMode::Id::Transmissions, true>;
extern template class Infector<ContactLogMode::Id::All, false>;
extern template class Infector<ContactLogMode::Id::All, true>;
extern template class Infector<ContactLogMode::Id::Susceptibles, false>;
extern template class Infector<ContactLogMode::Id::Susceptibles, true>;

} // namespace

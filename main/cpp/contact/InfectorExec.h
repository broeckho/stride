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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the InfectorExec class.
 */

#include <spdlog/spdlog.h>

namespace stride {

class ContactPool;
class AgeContactProfile;
class TransmissionProfile;
class ContactHandler;

/// For use in the InfectorMap and Sim.
typedef void(InfectorExec)(ContactPool& pool, const AgeContactProfile& profile,
                           const TransmissionProfile& trans_profile, ContactHandler& c_handler,
                           unsigned short int sim_day, std::shared_ptr<spdlog::logger> c_logger);

} // namespace stride

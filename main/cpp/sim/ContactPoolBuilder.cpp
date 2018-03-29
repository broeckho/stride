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
 * Implementation for the ContactPoolBuilder class.
 */

#include "ContactPoolBuilder.h"

#include "calendar/Calendar.h"
#include "core/ContactPoolSys.h"
#include "immunity/Vaccinator.h"
#include "pop/PopulationBuilder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <trng/uniform_int_dist.hpp>
#include <cassert>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

namespace stride {

using namespace boost::property_tree;
using namespace boost::filesystem;
using namespace std;
using namespace util;

ContactPoolBuilder::ContactPoolBuilder(std::shared_ptr<spdlog::logger> logger) : m_stride_logger(std::move(logger))
{
        assert(m_stride_logger && "ContactPoolBuilder::ContactPoolBuilder> Nullptr for logger not acceptable!");
}

void ContactPoolBuilder::Build(const ptree& pt_contact, std::shared_ptr<Simulator> sim)
{
        m_stride_logger->trace("Starting ContactPoolBuilder::Build.");
        using Id = ContactPoolType::Id;

        // --------------------------------------------------------------
        // Contact profiles & initialize contactpools.
        // --------------------------------------------------------------
        sim->m_contact_profiles[ToSizeT(Id::Household)]          = ContactProfile(Id::Household, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::School)]             = ContactProfile(Id::School, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::Work)]               = ContactProfile(Id::Work, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::PrimaryCommunity)]   = ContactProfile(Id::PrimaryCommunity, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::SecondaryCommunity)] = ContactProfile(Id::SecondaryCommunity, pt_contact);

        // --------------------------------------------------------------
        // Contact profiles & initialize contactpools.
        // --------------------------------------------------------------
        InitializeContactPools(sim->m_pool_sys, sim);

        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        m_stride_logger->trace("Finished ContactPoolBuilder::Build.");
}

void ContactPoolBuilder::InitializeContactPools(ContactPoolSys& pool_sys, std::shared_ptr<Simulator> sim)
{
        // Initilize ContactPoolSys and the number of contactpools.
        unsigned int max_id_households{0U};
        unsigned int max_id_school_pools{0U};
        unsigned int max_id_work_pools{0U};
        unsigned int max_id_primary_community{0U};
        unsigned int max_id_secondary_community{0U};

        Population& population{*sim->m_population};
        using Id = ContactPoolType::Id;

        for (const auto& p : population) {
                max_id_households        = max(max_id_households, p.GetContactPoolId(Id::Household));
                max_id_school_pools      = max(max_id_school_pools, p.GetContactPoolId(Id::School));
                max_id_work_pools        = max(max_id_work_pools, p.GetContactPoolId(Id::Work));
                max_id_primary_community = max(max_id_primary_community, p.GetContactPoolId(Id::PrimaryCommunity));
                max_id_secondary_community =
                    max(max_id_secondary_community, p.GetContactPoolId(Id::SecondaryCommunity));
        }

        // Keep separate id counter to provide a unique id for every contactpool.
        unsigned int c_id = 1;

        for (size_t i = 0; i <= max_id_households; i++) {
                pool_sys[ToSizeT(Id::Household)].emplace_back(
                    ContactPool(c_id, Id::Household, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_school_pools; i++) {
                pool_sys[ToSizeT(Id::School)].emplace_back(ContactPool(c_id, Id::School, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_work_pools; i++) {
                pool_sys[ToSizeT(Id::Work)].emplace_back(ContactPool(c_id, Id::Work, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_primary_community; i++) {
                pool_sys[ToSizeT(Id::PrimaryCommunity)].emplace_back(
                    ContactPool(c_id, Id::PrimaryCommunity, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_secondary_community; i++) {
                pool_sys[ToSizeT(Id::SecondaryCommunity)].emplace_back(
                    ContactPool(c_id, Id::SecondaryCommunity, sim->m_contact_profiles));
                c_id++;
        }

        // Having contactpool id '0' means "not present in any contactpool of that type".
        for (auto& p : population) {
                const auto hh_id = p.GetContactPoolId(Id::Household);
                if (hh_id > 0) {
                        pool_sys[ToSizeT(Id::Household)][hh_id].AddMember(&p);
                }
                const auto sc_id = p.GetContactPoolId(Id::School);
                if (sc_id > 0) {
                        pool_sys[ToSizeT(Id::School)][sc_id].AddMember(&p);
                }
                const auto wo_id = p.GetContactPoolId(Id::Work);
                if (wo_id > 0) {
                        pool_sys[ToSizeT(Id::Work)][wo_id].AddMember(&p);
                }
                const auto primCom_id = p.GetContactPoolId(Id::PrimaryCommunity);
                if (primCom_id > 0) {
                        pool_sys[ToSizeT(Id::PrimaryCommunity)][primCom_id].AddMember(&p);
                }
                const auto secCom_id = p.GetContactPoolId(Id::SecondaryCommunity);
                if (secCom_id > 0) {
                        pool_sys[ToSizeT(Id::SecondaryCommunity)][secCom_id].AddMember(&p);
                }
        }
}

} // namespace stride

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

ContactPoolBuilder::ContactPoolBuilder(const boost::property_tree::ptree& config_pt,
                                       std::shared_ptr<spdlog::logger>    logger)
    : m_config_pt(config_pt), m_stride_logger(std::move(logger))
{
        assert(m_stride_logger && "ContactPoolBuilder::ContactPoolBuilder> Nullptr for logger not acceptable!");
        assert(m_config_pt.empty() && "ContactPoolBuilder::ContactPoolBuilder> Empty ptree not acceptable!");
}

void ContactPoolBuilder::Build(std::shared_ptr<Simulator> sim)
{
        m_stride_logger->trace("Starting ContactPoolBuilder::Build.");
        const auto pt_contact = ReadContactPtree();

        if (!pt_contact.empty()) {
                Build(pt_contact, sim);
        }

        m_stride_logger->trace("Finished ContactPoolBuilder::Build.");
}

ptree ContactPoolBuilder::ReadContactPtree()
{
        const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");

        ptree      pt;
        const auto fn = m_config_pt.get("run.age_contact_matrix_file", "contact_matrix.xml");
        const auto fp = (use_install_dirs) ? FileSys::GetDataDir() /= fn : fn;
        if (!exists(fp) || !is_regular_file(fp)) {
                m_stride_logger->critical("Age-Contact matrix file {} not present! Quitting.", fp.string());
        } else {
                m_stride_logger->debug("Age-Contact matrix file:  {}", fp.string());
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        m_stride_logger->critical("Error reading {}\nException: {}", canonical(fp).string(), e.what());
                        pt.clear();
                }
        }

        return pt;
}

void ContactPoolBuilder::Build(const ptree& pt_contact, std::shared_ptr<Simulator> sim)
{
        // --------------------------------------------------------------
        // Contact profiles & initialize contactpools.
        // --------------------------------------------------------------
        using Id                                                 = ContactPoolType::Id;
        sim->m_contact_profiles[ToSizeT(Id::Household)]          = ContactProfile(Id::Household, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::School)]             = ContactProfile(Id::School, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::Work)]               = ContactProfile(Id::Work, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::PrimaryCommunity)]   = ContactProfile(Id::PrimaryCommunity, pt_contact);
        sim->m_contact_profiles[ToSizeT(Id::SecondaryCommunity)] = ContactProfile(Id::SecondaryCommunity, pt_contact);

        // --------------------------------------------------------------
        // Contact profiles & initialize contactpools.
        // --------------------------------------------------------------
        InitializeContactPools(sim);
}

void ContactPoolBuilder::InitializeContactPools(std::shared_ptr<Simulator> sim)
{
        // Determine the number of contactpools.
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
                sim->m_households.emplace_back(ContactPool(c_id, Id::Household, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_school_pools; i++) {
                sim->m_school_pools.emplace_back(ContactPool(c_id, Id::School, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_work_pools; i++) {
                sim->m_work_pools.emplace_back(ContactPool(c_id, Id::Work, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_primary_community; i++) {
                sim->m_primary_community.emplace_back(ContactPool(c_id, Id::PrimaryCommunity, sim->m_contact_profiles));
                c_id++;
        }
        for (size_t i = 0; i <= max_id_secondary_community; i++) {
                sim->m_secondary_community.emplace_back(
                    ContactPool(c_id, Id::SecondaryCommunity, sim->m_contact_profiles));
                c_id++;
        }

        // Having contactpool id '0' means "not present in any contactpool of that type".
        for (auto& p : population) {
                const auto hh_id = p.GetContactPoolId(Id::Household);
                if (hh_id > 0) {
                        sim->m_households[hh_id].AddMember(&p);
                }
                const auto sc_id = p.GetContactPoolId(Id::School);
                if (sc_id > 0) {
                        sim->m_school_pools[sc_id].AddMember(&p);
                }
                const auto wo_id = p.GetContactPoolId(Id::Work);
                if (wo_id > 0) {
                        sim->m_work_pools[wo_id].AddMember(&p);
                }
                const auto primCom_id = p.GetContactPoolId(Id::PrimaryCommunity);
                if (primCom_id > 0) {
                        sim->m_primary_community[primCom_id].AddMember(&p);
                }
                const auto secCom_id = p.GetContactPoolId(Id::SecondaryCommunity);
                if (secCom_id > 0) {
                        sim->m_secondary_community[secCom_id].AddMember(&p);
                }
        }
}

} // namespace stride

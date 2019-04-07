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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

/**
 * For use in  cpp-python interface.
 */

#include "create_sim.h"

#include "pop/Population.h"
#include "util/RnMan.h"
#include "util/RunConfigManager.h"

#include <iostream>

using namespace std;

std::shared_ptr<stride::Sim> CreateSim(const std::string& configString)
{
        const auto config_pt = stride::util::RunConfigManager::FromString(configString);

        auto rnMan = std::make_shared<stride::util::RnMan>();

        rnMan->Initialize(stride::util::RnMan::Info{config_pt.get<std::string>("pop.rng_seed", "1,2,3,4"), "",
                                                    config_pt.get<unsigned int>("run.num_threads")});

        auto population = stride::Population::Create(config_pt, *rnMan.get());

        auto sim = stride::Sim::Create(config_pt, population, rnMan);

        return sim;
}

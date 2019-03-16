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

using namespace std;
using namespace stride;
using namespace stride::util;

shared_ptr<Sim> CreateSim(const string& configString)
{
        const auto config = RunConfigManager::FromString(configString);

        RnInfo info{config.get<string>("pop.rng_seed", "1,2,3,4"), "", config.get<unsigned int>("run.num_threads")};
        RnMan  rnMan(info);

        auto population = Population::Create(config, rnMan);

        auto sim = Sim::Create(config, population, rnMan);

        return sim;
}

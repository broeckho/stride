#include "create_sim.h"
#include "pop/Population.h"
#include <util/RnMan.h>
#include <util/RunConfigManager.h>

std::shared_ptr<stride::Sim> CreateSim(const std::string& configString)
{
        auto config_pt = stride::util::RunConfigManager::FromString(configString);

        auto rnMan = std::make_shared<stride::util::RnMan>();
        rnMan->Initialize(stride::util::RnMan::Info{config_pt.get<std::string>("pop.rng_seed", "1,2,3,4"), "",
                                                    config_pt.get<unsigned int>("run.num_threads")});

        auto population = stride::Population::Create(config_pt, *rnMan.get());

        return stride::Sim::Create(config_pt, population, rnMan);
}

#############################################################################
# Script for running baseline simulations (no beliefs & no behaviours)      #
#############################################################################

import itertools

import pystride
from pystride.stride.stride import Id_Household, Id_School, Id_Work, Id_PrimaryCommunity, Id_SecondaryCommunity
from pystride.Simulation import Simulation

def countImmune(sim, timestep):
    if timestep == 0:
        print("Counting pocket size of non immunized persons")
        households = {}
        schools = {}
        workplaces = {}
        communities1 = {}
        communities2 = {}

        pop = sim.GetSimulator().GetPopulation()
        for pIndex in range(pop.size()):
            person = pop[pIndex]
            household_id = person.GetClusterId(Id_Household)
            school_id = person.GetClusterId(Id_Household)
            work_id = person.GetClusterId(Id_Work)
            community1_id = person.GetClusterId(Id_PrimaryCommunity)
            community2_id = person.GetClusterId(Id_SecondaryCommunity)
            if pop[pIndex].GetHealth().IsSusceptible():
                if household_id in households:
                    households[household_id] += 1
                else:
                    households[household_id] = 1

                if school_id in schools:
                    schools[school_id] += 1
                else:
                    schools[school_id] = 1

                if work_id in workplaces:
                    workplaces[work_id] += 1
                else:
                    workplaces[work_id] = 1

                if community1_id in communities1:
                    communities1[community1_id] += 1
                else:
                    communities1[community1_id] = 1

                if community2_id in communities2:
                    communities2[community2_id] += 1
                else:
                    communities2[community2_id] = 1

sim = Simulation()
# Load configuration common to all simulations for this study
sim.loadRunConfig("config/common.xml")

# Set parameters that are the same for all baseline simulations
sim.runConfig.setParameter("output_prefix", "Baseline")
sim.runConfig.setParameter("seeding_rate", 0.002)
sim.runConfig.setParameter("local_information_policy", "NoLocalInformation")
sim.runConfig.setParameter("global_information_policy", "NoGlobalInformation")
sim.runConfig.setParameter("belief_policy/name", "NoBelief")
sim.runConfig.setParameter("behaviour_policy", "NoBehaviour")

# TODO have common natural immunity rate?
# From what age to what age and what percentage?

seeds = list(range(10))
vaccs = [0.6, 0.7, 0.8, 0.9]

# Create forks with varying seed & vaccination coverage
for seed, vac_cov in list(itertools.product(seeds, vaccs)):
    # Create fork
    fork = sim.fork("vac_cov_" + str(vac_cov) + "_" + str(seed))
    fork.runConfig.setParameter("rng_seed", seed)
    fork.runConfig.setParameter("immunity_rate", vac_cov)
    fork.runConfig.setParameter("vaccine_rate", vac_cov)
    # Register callback function
    fork.registerCallback(countImmune)

# Run all forks
sim.runForks()

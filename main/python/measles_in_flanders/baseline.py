########################################################################
# Script for running baseline simulations (no beliefs & no behaviours) #
########################################################################

import itertools

import pystride
from pystride.Simulation import Simulation

sim = Simulation()
sim.loadRunConfig("config/default.xml")

# Set parameters that are the same for all baseline simulations
sim.runConfig.setParameter("output_prefix", "Baseline")
sim.runConfig.setParameter("local_information_policy", "NoLocalInformation")
sim.runConfig.setParameter("global_information_policy", "NoGlobalInformation")
sim.runConfig.setParameter("belief_policy", "NoBelief")
sim.runConfig.setParameter("behaviour_policy", "NoBehaviour")

#TODO find out from Lander how immunity/vaccine rates should be combined

seeds = list(range(10))
vaccs = [0, 0.5, 1] #TODO check which vaccination coverages are interesting

# Create forks with varying seed & vaccination coverage
for seed, vac_cov in list(itertools.product(seeds, vaccs)):
    # Create fork
    fork = sim.fork("vac_cov_" + str(vac_cov) + "_" + str(seed))
    fork.runConfig.setParameter("rng_seed", seed)
    fork.runConfig.setParameter("immunity_rate", vac_cov)
    fork.runConfig.setParameter("vaccine_rate", vac_cov)

# Run all forks
sim.runForks()

#############################################################################
# Script for running baseline simulations (no beliefs & no behaviours)      #
#############################################################################

import itertools

import pystride
from pystride.Simulation import Simulation

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
vaccs = [0.6, 0.7, 0.8, 0.9, 1.0]

# Create forks with varying seed & vaccination coverage
for seed, vac_cov in list(itertools.product(seeds, vaccs)):
    # Create fork
    fork = sim.fork("vac_cov_" + str(vac_cov) + "_" + str(seed))
    fork.runConfig.setParameter("rng_seed", seed)
    fork.runConfig.setParameter("immunity_rate", vac_cov)
    fork.runConfig.setParameter("vaccine_rate", vac_cov)

# Run all forks
sim.runForks()

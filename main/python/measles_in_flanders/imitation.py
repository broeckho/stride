#############################################################################
# Script for simulations with no-vaccination clustering                     #
# using Imitation belief policy.                                            #
#############################################################################

import itertools

import pystride
from pystride.Simulation import Simulation

# TODO callback to vaccinate + seed after x days

sim = Simulation()
sim.loadRunConfig("config/common.xml")

# Set parameters that are the same for all simulations
sim.runConfig.setParameter("output_prefix", "Imitation")
sim.runConfig.setParameter("seeding_rate", 0)

# TODO natural immunity?
sim.runConfig.setParameter("immunity_rate", 0)
sim.runConfig.setParameter("vaccine_rate", 0)

sim.runConfig.setParameter("local_information_policy", "LocalDiscussion")
sim.runConfig.setParameter("global_information_policy", "NoLocalInformation")
sim.runConfig.setParameter("belief_policy/name", "Imitation")
sim.runConfig.setParameter("belief_policy/accept_threshold", 0.95)
sim.runConfig.setParameter("belief_policy/no_accept_threshold", -0.95)
sim.runConfig.setParameter("behaviour_policy", "Vaccination")

seeds = list(range(10))
# TODO belief and behaviour related parameters

# Create forks with varying seeds and behaviour parameters
for seed in list(itertools.product(seeds)):
    # Create fork
    fork = sim.fork(str(seed))
    fork.runConfig.setParameter("rng_seed", seed)

# Run all forks
sim.runForks()

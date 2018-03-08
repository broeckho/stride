#############################################################################
# Script for simulations with no-vaccination clustering                     #
# using Imitation belief policy.                                            #
#############################################################################

import itertools

import pystride
from pystride.Simulation import Simulation

# TODO callback function for measuring clustering
# TODO callback function for vaccination

def runImitiation():
    """
    """
    sim = Simulation()

    # Set parameters
    sim.runConfig.setParameter("output_prefix", "Imitation")
    sim.runConfig.setParameter("num_days", 60)

    sim.runConfig.setParameter("disease_config_file", "data/disease_measles.xml")
    sim.runConfig.setParameter("r0", 11)
    sim.runConfig.setParameter("seeding_rate", 0)
    sim.runConfig.setParameter("seeding_age_min", 1)
    sim.runConfig.setParameter("seeding_age_max", 99)

    sim.runConfig.setParameter("population_file", "data/pop_flanders600.csv")
    sim.runConfig.setParameter("generate_person_file", 0)
    sim.runConfig.setParameter("num_participants_survey", 0)    # do not log contacts while beliefs are still being exchanged
    sim.runConfig.setParameter("age_contact_matrix_file", "data/contact_matrix_flanders_subpop.xml")
    sim.runConfig.setParameter("log_level", "SusceptibleContacts")
    sim.runConfig.setParameter("start_date", "2017-01-01")
    sim.runConfig.setParameter("holidays_file", "data/holidays_none.json")

    # immunity_profile
    # vaccine_profile
    # vaccine_link_probability
    # immunity_rate
    # vaccine_rate

    sim.RunConfig.setParameter("local_information_policy", "LocalDiscussion")

'''
    sim.runConfig.setParameter("belief_policy/name", "NoBelief")

    sim.runConfig.setParameter("rng_seed", 1)

    # Register callbacks
    sim.RegisterCallback(measureSusceptiblesClustering)
    sim.RegisterCallback(seedInfection)

    # TODO run forks with varying parameters (itertools.product(...))
    # Run simulation
    sim.run()

'''

'''
sim.runConfig.setParameter("local_information_policy", "LocalDiscussion")
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
'''

#############################################################################
# Script for running baseline simulations (no beliefs & no behaviours)      #
#############################################################################

import itertools

import pystride
from pystride.Simulation import Simulation

def measureSusceptiblesClustering(sim, timestep):
    """
    """
    if timestep == 9:
        # Count contacts between susceptibles after 10 timesteps
        pass

def seedInfection(sim, timestep):
    """
    """
    if timestep == 9:
        pass

def runBaseline():
    """
    """
    sim = Simulation()
    # Set parameters
    sim.runConfig.setParameter("output_prefix", "Baseline")
    sim.runConfig.setParameter("num_days", 60)

    sim.runConfig.setParameter("disease_config_file", "data/disease_measles.xml")
    sim.runConfig.setParameter("r0", 11)
    sim.runConfig.setParameter("seeding_rate", 0)
    sim.runConfig.setParameter("seeding_age_min", 1)
    sim.runConfig.setParameter("seeding_age_max", 99)

    sim.runConfig.setParameter("population_file", "data/pop_flanders600.csv")
    sim.runConfig.setParameter("output_persons", "false")
    sim.runConfig.setParameter("num_participants_survey", 10000)
    sim.runConfig.setParameter("age_contact_matrix_file", "data/contact_matrix_flanders_subpop.xml")
    sim.runConfig.setParameter("log_level", "SusceptibleContacts")
    sim.runConfig.setParameter("start_date", "2017-01-01")
    sim.runConfig.setParameter("holidays_file", "data/holidays_none.json")
    
    # immunity_profile
    # vaccine_profile
    # vaccine_link_probability
    # immunity_rate
    # vaccine_rate

    sim.runConfig.setParameter("local_information_policy", "NoLocalInformation")
    sim.runConfig.setParameter("global_information_policy", "NoGlobalInformation")
    sim.runConfig.setParameter("belief_policy/name", "NoBelief")
    sim.runConfig.setParameter("behaviour_policy", "NoBehaviour")

    sim.runConfig.setParameter("rng_seed", 1)

    # Register callbacks
    sim.RegisterCallback(measureSusceptiblesClustering)
    sim.RegisterCallback(seedInfection)

    # TODO run forks with varying parameters (itertools.product(...))
    # Run simulation
    sim.run()

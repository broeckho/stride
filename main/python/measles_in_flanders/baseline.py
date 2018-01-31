#############################################################################
# Script for running baseline simulations (no beliefs & no behaviours)      #
#############################################################################

import itertools

import matplotlib.pyplot as plt
import numpy as np

import pystride
from pystride.stride.stride import Id_Household, Id_School, Id_Work, Id_PrimaryCommunity, Id_SecondaryCommunity
from pystride.Simulation import Simulation


def countImmune(sim, timestep):
    if timestep == 0:
        print("Determining pocket sizes of non-immunized individuals")
        households = {}
        schools = {}
        workplaces = {}
        communities1 = {}
        communities2 = {}

        pop = sim.GetSimulator().GetPopulation()
        for pIndex in range(pop.size()):
            person = pop[pIndex]
            if person.GetHealth().IsSusceptible():
                addPerson(households, person.GetClusterId(Id_Household), True)
                addPerson(schools, person.GetClusterId(Id_School), True)
                addPerson(workplaces, person.GetClusterId(Id_Work), True)
                addPerson(communities1, person.GetClusterId(Id_PrimaryCommunity), True)
                addPerson(communities2, person.GetClusterId(Id_SecondaryCommunity), True)
            else:
                addPerson(households, person.GetClusterId(Id_Household), False)
                addPerson(schools, person.GetClusterId(Id_School), False)
                addPerson(workplaces, person.GetClusterId(Id_Work), False)
                addPerson(communities1, person.GetClusterId(Id_PrimaryCommunity), False)
                addPerson(communities2, person.GetClusterId(Id_SecondaryCommunity), False)

        # Distribution of susceptibles by cluster type
        '''bins = [0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        household_results = toHist(toFractions(households), bins)
        school_results = toHist(toFractions(schools), bins)
        work_results = toHist(toFractions(workplaces), bins)
        comm1_results = toHist(toFractions(communities1), bins)
        comm2_results = toHist(toFractions(communities2), bins)

        width = 0.015

        fig, ax = plt.subplots()
        ax.bar(bins, household_results, width=width, color='red')
        ax.bar([x + width for x in bins], school_results, width, color='tan')
        ax.bar([x + (2* width) for x in bins], work_results, width, color='lime')
        ax.bar([x + (3* width) for x in bins], comm1_results, width, color='orange')
        ax.bar([x + (4* width) for x in bins], comm2_results, width, color='black')
        plt.show()'''

        # Overall distribution of susceptibles
        bins = [0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        all_fractions = toFractions(households) + toFractions(schools) + toFractions(workplaces) + toFractions(communities1) + toFractions(communities2)
        all_results = toHist(all_fractions, bins)
        plt.ylim([0,1])
        plt.legend(['Baseline'])
        plt.bar(bins, all_results, width=0.08)
        plt.savefig('baseline')

def toHist(fractions, bins):
    ys = [0] * len(bins)
    for frac in fractions:
        for i_bin in range(len(bins)):
            if frac <= bins[i_bin]:
                ys[i_bin] += 1
                break
    for i_y in range(len(ys)):
        ys[i_y] = ys[i_y] / len(fractions)
    return ys


def toFractions(dictionary):
    fractions = []
    for key in dictionary:
        cluster_size = dictionary[key]['susceptible'] + dictionary[key]['other']
        frac_susceptible = dictionary[key]['susceptible'] / cluster_size
        fractions.append(frac_susceptible)
    return fractions


def addPerson(dictionary, cluster_id, susceptible):
    if cluster_id != 0:
        if not (cluster_id in dictionary):
            dictionary[cluster_id] = {
                'susceptible' : 0,
                'other' : 0
            }

        if susceptible:
            dictionary[cluster_id]['susceptible'] += 1
        else:
            dictionary[cluster_id]['other'] += 1


sim = Simulation()
# Load configuration common to all simulations for this study
sim.loadRunConfig("config/common.xml")

# Set parameters that are the same for all baseline simulations
sim.runConfig.setParameter("output_prefix", "Baseline")
sim.runConfig.setParameter("seeding_rate", 0)
sim.runConfig.setParameter("local_information_policy", "NoLocalInformation")
sim.runConfig.setParameter("global_information_policy", "NoGlobalInformation")
sim.runConfig.setParameter("belief_policy/name", "NoBelief")
sim.runConfig.setParameter("behaviour_policy", "NoBehaviour")

# TODO have common natural immunity rate?
# From what age to what age and what percentage?

sim.runConfig.setParameter("num_days", 1)

seeds = [1]
vaccs = [0.97]

# Create forks with varying seed & vaccination coverage
for seed, vac_cov in list(itertools.product(seeds, vaccs)):
    # Create fork
    fork = sim.fork("vac_cov_" + str(vac_cov) + "_" + str(seed))
    fork.runConfig.setParameter("rng_seed", seed)
    fork.runConfig.setParameter("immunity_rate", vac_cov)
    fork.runConfig.setParameter("vaccine_rate", vac_cov)
    # Register callback function
    # fork.registerCallback(countImmune)

# Run all forks
sim.runForks()

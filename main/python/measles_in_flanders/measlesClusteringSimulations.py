import multiprocessing
import os

from measlesClusteringCallbacks import registerAssociativityCoefficient, registerSampleAssociativityCoefficient, registerSusceptibles
from measlesClusteringUtil import getCommonParameters, generateRngSeeds, writeRngSeeds

from pystride.Event import Event, EventType
from pystride.PyController import PyController

def runSimulation(outputPrefix, seed, extraParams={}, callbacks=[]):
    control = PyController(data_dir="data")
    extraParams.update(getCommonParameters())
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("output_prefix", outputPrefix + "_" + str(seed))
    for callbackFunction, callbackEvent in callbacks:
        control.registerCallback(callbackFunction, callbackEvent)
    control.control()

def runR0Simulations(numRuns, transmissionProbabilities, poolSize):
    for prob in transmissionProbabilities:
        extraParams = {
            "immunity_profile": "None",
            "num_days": 30,
            "population_file": "pop_flanders600.csv",
            "num_index_cases": 1,
            "transmission_probability": prob,
            "track_index_case": "true",
            "vaccine_profile": "None",
        }
        outputPrefix = "r0_TP_" + str(prob)
        seeds = generateRngSeeds(numRuns)
        writeRngSeeds(outputPrefix, seeds)
        with multiprocessing.Pool(processes=poolSize) as pool:
            pool.starmap(runSimulation, [(outputPrefix, s, extraParams) for s in seeds])

def runFullScenarioSimulations(numRuns, numDays, immunityFileChildren, immunityFileAdults,
    transmissionProbabilities, clusteringLevels, clusteringPoolType, poolSize):
    """
        Run simulations and keep track of cases, susceptibles
        and contact-pool constitutions.
    """
    callbacks = [
        (registerAssociativityCoefficient, EventType.AtStart),
        (registerSampleAssociativityCoefficient, EventType.AtStart),
        (registerSusceptibles, EventType.AtStart),
    ]
    for prob in transmissionProbabilities:
        for level in clusteringLevels:
            extraParams = {
                "immunity_distribution_file": immunityFileAdults,
                "immunity_link_probability": 0,
                "immunity_profile": "AgeDependent",
                "num_days": numDays,
                "population_file": "pop_flanders600.csv",
                "num_index_cases": 1,
                "track_index_case": "false",
                "transmission_probability": prob,
                "vaccine_distribution_file": immunityFileChildren,
                "vaccine_link_probability": level,
                "vaccine_clustering_pooltype": clusteringPoolType,
                "vaccine_profile": "AgeDependent",
            }
            outputPrefix = "AGEDEPENDENT_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = generateRngSeeds(numRuns)
            writeRngSeeds(outputPrefix, seeds)
            with multiprocessing.Pool(processes=poolSize) as pool:
                pool.starmap(runSimulation, [(outputPrefix, s, extraParams, callbacks) for s in seeds])

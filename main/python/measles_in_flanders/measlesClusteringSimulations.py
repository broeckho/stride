import multiprocessing
import numpy
import os

from pystride.Event import Event, EventType
from pystride.PyController import PyController
from measlesClusteringCallbacks import registerMemberships, registerSusceptibles, trackCases
from measlesClusteringUtil import generateRngSeeds, getCommonParameters, writeRngSeeds

IMMUNITY_FILE_CHILDREN = os.path.join("data", "2020_measles_child_immunity.xml")
IMMUNITY_FILE_ADULTS = os.path.join("data", "2020_measles_adult_immunity.xml")

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
    """
        Run simulations to establish relationship between P(transmission)
        and R0.
    """
    for prob in transmissionProbabilities:
        extraParams = {
            "immunity_profile": "None",
            "num_days": 25,
            "transmission_probability": prob,
            "track_index_case": "true",
            "vaccine_profile": "None",
        }
        outputPrefix = "r0_TP_" + str(prob)
        seeds = generateRngSeeds(numRuns)
        writeRngSeeds(outputPrefix, seeds)
        with multiprocessing.Pool(processes=poolSize) as pool:
            pool.starmap(runSimulation, [(outputPrefix, s, extraParams) for s in seeds])

def runEffectiveRSimulations(numRuns, transmissionProbabilities, clusteringLevels, poolSize):
    """
        Run simulations to establish relationship between P(transmission),
        clustering level, and secondary cases caused by index case in partially
        susceptible populaiton.
    """
    for prob in transmissionProbabilities:
        for level in clusteringLevels:
            extraParams = {
                "immunity_distribution_file": IMMUNITY_FILE_ADULTS,
                "immunity_link_probability": 0,
                "immunity_profile": "AgeDependent",
                "num_days": 25,
                "track_index_case": "true",
                "transmission_probability": prob,
                "vaccine_distribution_file": IMMUNITY_FILE_CHILDREN,
                "vaccine_link_probability": level,
                "vaccine_profile": "AgeDependent",
            }
            outputPrefix = "ER_AGEDEPENDENT_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = generateRngSeeds(numRuns)
            writeRngSeeds(outputPrefix, seeds)
            with multiprocessing.Pool(processes=poolSize) as pool:
                pool.starmap(runSimulation, [(outputPrefix, s, extraParams) for s in seeds])

def runFullScenarioSimulations(numRuns, transmissionProbabilities, clusteringLevels, poolSize):
    """
        Run simulations and keep track of new cases, susceptibles and household
        constitutions.
    """
    for prob in transmissionProbabilities:
        for level in clusteringLevels:
            extraParams = {
                "immunity_distribution_file": IMMUNITY_FILE_ADULTS,
                "immunity_link_probability": 0,
                "immunity_profile": "AgeDependent",
                "num_days": 730,
                "track_index_case": "false",
                "transmission_probability": prob,
                "vaccine_distribution_file": IMMUNITY_FILE_CHILDREN,
                "vaccine_link_probability": level,
                "vaccine_profile": "AgeDependent",
            }
            callbacks = [
                (registerSusceptibles, EventType.AtStart),
                (registerMemberships, EventType.AtStart),
                (trackCases, EventType.Stepped)
            ]
            outputPrefix = "AGEDEPENDENT_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = generateRngSeeds(numRuns)
            writeRngSeeds(outputPrefix, seeds)
            with multiprocessing.Pool(processes=poolSize) as pool:
                pool.starmap(runSimulation, [(outputPrefix, s, extraParams, callbacks) for s in seeds])

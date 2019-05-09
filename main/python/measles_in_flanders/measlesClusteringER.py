import argparse
import csv
import math
import multiprocessing
import numpy
import os
import time

from pystride.Event import Event, EventType
from pystride.PyController import PyController

from measlesClusteringParameters import getCommonParameters
from measlesClusteringUtil import generateRngSeeds, writeRngSeeds
from measlesClusteringUtil import calculateUniformImmunityLevel, createUniformImmunityDistributionFiles

from postprocessing import EffectiveR

def runSimulation(scenarioName, transmissionProbability, clusteringLevel, seed, extraParams):
    control = PyController(data_dir="data")
    # Get common parameters
    extraParams.update(getCommonParameters())
    # Set parameters
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("transmission_probability", transmissionProbability)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("output_prefix", "ER_" + scenarioName + "_CLUSTERING_"
                                        + str(clusteringLevel) + "_TP_"
                                        + str(transmissionProbability) + "_" + str(seed))
    control.control()

def runSimulations(numRuns, scenario, transmissionProbability, clusteringLevel, numDays, immunityFileChildren, immunityFileAdults, poolSize):
    print("Scenario = {}, P(t) = {}, clustering level = {}".format(scenario, transmissionProbability, clusteringLevel))
    # Check whether to use age-dependent immunity levels or uniform levels
    if scenario == "UNIFORM":
        immunityFileChildren = os.path.join("data", "measles_uniform_child_immunity.xml")
        immunityFileAdults = os.path.join("data", "measles_uniform_adult_immunity.xml")
    extraParams = {
        "immunity_distribution_file": immunityFileAdults,
        "num_days": numDays,
        "track_index_case": "true",
        "vaccine_distribution_file": immunityFileChildren,
        "vaccine_link_probability": clusteringLevel,
    }
    seeds = generateRngSeeds(numRuns)
    writeRngSeeds("ER_" + scenario + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(transmissionProbability), seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(scenario, transmissionProbability, clusteringLevel, s, extraParams) for s in seeds])

def main(numRuns, transmissionProbabilities, clusteringLevels, numDays, poolSize):
    start = time.perf_counter()
    immunityFileChildren = os.path.join("data", "2020_measles_child_immunity.xml")
    immunityFileAdults = os.path.join("data", "2020_measles_adult_immunity.xml")
    scenarioNames = ["AGEDEPENDENT", "UNIFORM"]

    # Calculate uniform immunity rate
    uniformImmunityLevel = calculateUniformImmunityLevel(os.path.join("data", getCommonParameters()["population_file"]),
                                                            immunityFileChildren,
                                                            immunityFileAdults)
    # Create uniform immunity files
    createUniformImmunityDistributionFiles(uniformImmunityLevel)

    # Run simulations
    for scenario in scenarioNames:
        for prob in transmissionProbabilities:
            for level in clusteringLevels:
                runSimulations(numRuns, scenario, prob, level, numDays,
                                immunityFileChildren, immunityFileAdults, poolSize)

    # Create plots
    for scenario in scenarioNames:
        EffectiveR.createEffectiveR3DPlot(".", "ER_" + scenario, transmissionProbabilities,
                                            clusteringLevels, poolSize, erCalculation="index")
        EffectiveR.createEffectiveRHeatmap(".", "ER_" + scenario, transmissionProbabilities,
                                            clusteringLevels, poolSize, erCalculation="index")
        for level in clusteringLevels:
            EffectiveR.createEffectiveRPlot(".", "ER_" + scenario, transmissionProbabilities, level, poolSize, erCalculation="index")

    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = totalTime - (hours * 3600) - (minutes * 60)
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of runs per scenario")
    parser.add_argument("--transmissionProbs", type=float, nargs="+", default=numpy.arange(0.30, 0.75, 0.05), help="Transmission probabilities to test")
    parser.add_argument("--clusteringLevels", type=float, nargs="+", default=[0, 0.25, 0.5, 0.75, 1], help="Clustering levels to test")
    parser.add_argument("--numDays", type=int, default=730, help="Number of time-steps to simulate")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.numRuns, args.transmissionProbs, args.clusteringLevels, args.numDays, args.poolSize)

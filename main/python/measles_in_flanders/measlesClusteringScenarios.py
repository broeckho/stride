import argparse
import csv
import multiprocessing
import os
import time

from pystride.Event import Event, EventType
from pystride.PyController import PyController

from measlesClusteringParameters import getCommonParameters
from measlesClusteringUtil import generateRngSeeds, writeRngSeeds
from measlesClusteringUtil import calculateUniformImmunityLevel, createUniformImmunityDistributionFiles

def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()
    with open(os.path.join(outputPrefix, "susceptibles.csv"), "w") as csvfile:
        fieldnames = ["person_id", "age", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for i in range(pop.size()):
            person = pop[i]
            personID = person.GetId()
            age = person.GetAge()
            isSusceptible = person.GetHealth().IsSusceptible()
            # Convert boolean to int for easier reading from file in different langs
            if isSusceptible:
                isSusceptible = 1
            else:
                isSusceptible = 0
            writer.writerow({"person_id": personID, "age": age, "susceptible": isSusceptible})

def addToDict(dictionary, poolID, personID):
    if poolID != 0:
        if poolID in dictionary:
            dictionary[poolID].append(personID)
        else:
            dictionary[poolID] = [personID]

def writeMembershipToFile(outputPrefix, filename, dictionary):
    with open(os.path.join(outputPrefix, filename), "w") as csvfile:
        fieldnames = ["pool_id", "person_ids"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for poolID, personIDs in dictionary.items():
            writer.writerow({"pool_id": poolID, "person_ids": personIDs})

def registerMemberships(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()
    households = {}
    schools = {}
    colleges = {}
    workplaces = {}
    primaryCommunities = {}
    secondaryCommunities = {}
    for i in range(pop.size()):
        person = pop[i]
        personID = person.GetId()
        householdID = person.GetPoolId(0)
        addToDict(households, householdID, personID)
    writeMembershipToFile(outputPrefix, "households.csv", households)

def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        at each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

def runSimulation(scenarioName, R0, clusteringLevel, seed, extraParams):
    control = PyController(data_dir="data")
    # Get common parameters
    extraParams.update(getCommonParameters())
    # Set parameters
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("r0", R0)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("output_prefix", scenarioName + "_CLUSTERING_"
                                        + str(clusteringLevel) + "_R0_"
                                        + str(R0) + "_" + str(seed))
    # Register callbacks
    control.registerCallback(registerSusceptibles, EventType.AtStart)
    control.registerCallback(registerMemberships, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()

def runSimulations(trackIndexCase, numRuns, scenario, R0, clusteringLevel, numDays,
    immunityFileChildren, immunityFileAdults, poolSize):
    print("Scenario = {}, R0 = {}, clustering level = {}".format(scenario, R0, clusteringLevel))
    # Convert bool to string for inclusion in xml configuration file
    trackIndexCaseStr = "false"
    if trackIndexCase:
        trackIndexCaseStr = "true"
    # Check whether to use age-dependent immunity levels or uniform levels
    if scenario == "UNIFORM":
        immunityFileChildren = os.path.join("data", "measles_uniform_child_immunity.xml")
        immunityFileAdults = os.path.join("data", "measles_uniform_adult_immunity.xml")
    extraParams = {
        "immunity_distribution_file": immunityFileAdults,
        "num_days": numDays,
        "track_index_case": trackIndexCaseStr,
        "vaccine_distribution_file": immunityFileChildren,
        "vaccine_link_probability": clusteringLevel,
    }
    seeds = generateRngSeeds(numRuns)
    writeRngSeeds(scenario + "_CLUSTERING_" + str(clusteringLevel) + "_R0_" + str(R0), seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(scenario, R0, clusteringLevel, s, extraParams) for s in seeds])

def main(trackIndexCase, numRuns, R0s, clusteringLevels, numDays, poolSize):
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
        for R0 in R0s:
            for level in clusteringLevels:
                runSimulations(trackIndexCase, numRuns, scenario, R0, level, numDays,
                                immunityFileChildren, immunityFileAdults, poolSize)

    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = totalTime - (hours * 3600) - (minutes * 60)
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--trackIndexCase", action="store_true",
                        help="Only simulate secondary cases? Useful for effective R calcuation")
    parser.add_argument("--numRuns", type=int, default=10, help="Number of runs per scenario")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values of R0 to test")
    parser.add_argument("--clusteringLevels", type=int, nargs="+", default=[0, 0.5, 1], help="Clustering levels to test")
    parser.add_argument("--numDays", type=int, default=730, help="Number of time-steps to simulate")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.trackIndexCase, args.numRuns, args.R0s, args.clusteringLevels, args.numDays, args.poolSize)

import argparse
import csv
import multiprocessing
import os
import time

from pystride.Event import Event, EventType
from pystride.PyController import PyController

from measlesClusteringParameters import getCommonParameters, getScenarioParameters
from measlesClusteringUtil import generateRngSeeds, writeRngSeeds
from measlesClusteringUtil import calculateUniformImmunityLevel, createUniformImmunityDistributionFiles

def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()
    with open(os.path.join(outputPrefix, "susceptibles.csv"), "w") as csvfile:
        fieldnames = ["age", "hh_id", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for i in range(pop.size()):
            person = pop[i]
            age = person.GetAge()
            hhID = person.GetHouseholdId()
            isSusceptible = person.GetHealth().IsSusceptible()
            # Convert boolean to int for easier reading from file in different langs
            if isSusceptible:
                isSusceptible = 1
            else:
                isSusceptible = 0
            writer.writerow({"age": age, "hh_id": hhID, "susceptible": isSusceptible})

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

def registerAgesInfected(simulator, event):
    outputPrefix = simulator.GetConfigValue('run.output_prefix')
    pop = simulator.GetPopulation()
    with open(os.path.join(outputPrefix, 'infected.csv'), 'w') as csvfile:
        fieldnames = ["age", "infected"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for i in range(pop.size()):
            person = pop[i]
            age = person.GetAge()
            beenInfected = person.GetHealth().IsInfected() or person.GetHealth().IsRecovered()
            if beenInfected:
                beenInfected = 1
            else:
                beenInfected = 0
            writer.writerow({"age": age, "infected": beenInfected})

def runSimulation(R0, scenarioName, seed, immunityFileChildren, immunityFileAdults, extraParams):
    control = PyController(data_dir="data")
    # Get common + scenario-dependent parameters
    extraParams.update(getCommonParameters())
    extraParams.update(getScenarioParameters(scenarioName, immunityFileChildren, immunityFileAdults))
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("r0", R0)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("output_prefix", scenarioName + "_R0_" + str(R0) + "_" + str(seed))
    # Register callbacks
    control.registerCallback(registerSusceptibles, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.registerCallback(registerAgesInfected, EventType.AtFinished)
    control.control()

def runSimulations(trackIndexCase, numRuns, R0, scenarioName, numDays,
    immunityFileChildren, immunityFileAdults, poolSize):
    trackIndexCaseStr = "false"
    if trackIndexCase:
        trackIndexCaseStr = "true"
    extraParams = {
        "track_index_case": trackIndexCaseStr,
        "num_days": numDays,
    }

    seeds = generateRngSeeds(numRuns)
    writeRngSeeds(scenarioName, R0, seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(R0, scenarioName, s,
                                        immunityFileChildren, immunityFileAdults,
                                        extraParams) for s in seeds])

def main(trackIndexCase, numRuns, R0s, numDays, immunityFileChildren, immunityFileAdults, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM_NOCLUSTERING","AGEDEPENDENT_NOCLUSTERING","AGEDEPENDENT_CLUSTERING"]

    # Calculate uniform immunity rate
    uniformImmunityLevel = calculateUniformImmunityLevel(os.path.join("data", getCommonParameters()["population_file"]),
                                                            immunityFileChildren,
                                                            immunityFileAdults)
    # Create uniform immunity files
    createUniformImmunityDistributionFiles(uniformImmunityLevel)

    # Run simulations
    for R0 in R0s:
        for scenario in scenarioNames:
            runSimulations(trackIndexCase, numRuns, R0, scenario, numDays,
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
    parser.add_argument("--numDays", type=int, default=365, help="Number of time-steps to simulate")
    parser.add_argument("--immunityFileChildren", type=str, default=os.path.join("data", "2020_measles_child_immunity.xml"))
    parser.add_argument("--immunityFileAdults", type=str, default=os.path.join("data", "2020_measles_adult_immunity.xml"))
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.trackIndexCase, args.numRuns, args.R0s, args.numDays,
            args.immunityFileChildren, args.immunityFileAdults,
            args.poolSize)

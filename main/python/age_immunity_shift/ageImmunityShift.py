import argparse
import csv
import multiprocessing
import os
import time
import xml.etree.ElementTree as ET

from pystride.Event import Event, EventType
from pystride.PyController import PyController

# Callback function to register person's age and immunity status
# at the beginning of the simulation
def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue('run.output_prefix')
    pop = simulator.GetPopulation()
    totalPopulation = 0
    totalSusceptible = 0
    with open(os.path.join(outputPrefix, 'susceptibles.csv'), 'w') as csvfile:
        fieldnames = ["age", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        # Write header
        writer.writeheader()
        # Write susceptibility info per person
        for i in range(pop.size()):
            totalPopulation += 1
            person = pop[i]
            age = person.GetAge()
            isSusceptible = person.GetHealth().IsSusceptible()
            if isSusceptible:
                isSusceptible = 1
                totalSusceptible += 1
            else:
                isSusceptible = 0
            writer.writerow({"age": age, "susceptible": isSusceptible})

# Callback function to track the cumulative cases
# at each timestep
def trackCases(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def writeSeeds(scenarioName, seeds):
    with open(scenarioName + "Seeds.csv", "a") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

def getRngSeeds(scenarioName):
    seeds = []
    seedsFile = scenarioName + "Seeds.csv"
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def getImmunityRate(scenarioName, seed):
    totalPersons = 0
    susceptiblePersons = 0
    susceptiblesFile = os.path.join(scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalPersons += 1
            if int(row["susceptible"]):
                susceptiblePersons += 1
    immunityRate = 1 - (susceptiblePersons / totalPersons)
    return immunityRate

def getAvgImmunityRate(scenarioNames, poolSize):
    allImmunityRates = []
    print("Calculating average immunity rate of previous simulations...")
    for scenario in scenarioNames:
        seeds = getRngSeeds(scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            scenarioImmunityRates = pool.starmap(getImmunityRate, [(scenario, s) for s in seeds])
        allImmunityRates += scenarioImmunityRates
    return sum(allImmunityRates) / len(allImmunityRates)

def runSimulation(scenarioName, R0, seed, extraParams={}):
    print("Running " + scenarioName + " with R0 " + str(R0))
    configFile = os.path.join("config", "ageImmunityShift.xml")
    control = PyController(data_dir="data")
    control.loadRunConfig(configFile)
    control.runConfig.setParameter("output_prefix", scenarioName + "_" + str(seed))
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("r0", R0)
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.registerCallback(registerSusceptibles, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()
    return

def runSimulations(scenarioName, numRuns, R0, poolSize, extraParams={}):
    seeds = generateRngSeeds(numRuns)
    writeSeeds(scenarioName, seeds)
    args = [(scenarioName, R0, seeds[i], extraParams) for i in range(numRuns)]
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, args)

def main(numRuns, R0, poolSize):
    start = time.perf_counter()
    # Run simulations with age-immunity profile TODAY
    runSimulations("AgeImmunity2013", numRuns, R0, poolSize,
                    {"immunity_profile": "AgeDependent",
                    "immunity_distribution_file": "data/measles_age_immunity_2013.xml"})
    # Calculate immunity rate
    immunityRate = getAvgImmunityRate(["AgeImmunity2013"], poolSize)
    print(immunityRate)
    # Run simulations without age-immunity profile TODAY
    runSimulations("Uniform2013", numRuns, R0, poolSize,
                    {"immunity_profile": "Random", "immunity_rate": immunityRate})

    # Run simulations with age-immunity profile IN THE FUTURE
    runSimulations("AgeImmunity2033", numRuns, R0, poolSize,
                    {"immunity_profile": "AgeDependent",
                    "immunity_distribution_file": "data/measles_age_immunity_2033.xml"})
    # Calculate immunity rate
    immunityRate = getAvgImmunityRate(["AgeImmunity2033"], poolSize)
    print(immunityRate)
    # Run simulations without age-immunity profile IN THE FUTURE
    runSimulations("Uniform2033", numRuns, R0, poolSize,
                    {"immunity_profile": "Random", "immunity_rate": immunityRate})

    # Run simulations with age-immunity profile IN THE FUTURE - accounting for waning
    runSimulations("AgeImmunity2033Waning", numRuns, R0, poolSize,
                    {"immunity_profile": "AgeDependent",
                    "immunity_distribution_file": "data/measles_age_immunity_2033_waning.xml"})
    # Calculate immunity rate
    immunityRate = getAvgImmunityRate(["AgeImmunity2033Waning"], poolSize)
    print(immunityRate)
    # Run simulations without age-immunity profile IN THE FUTURE - accounting for waning
    runSimulations("Uniform2033Waning", numRuns, R0, poolSize,
                    {"immunity_profile": "Random", "immunity_rate": immunityRate})
    end = time.perf_counter()
    print("Total time: {}".format(end - start))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of simulation runs per scenario")
    parser.add_argument("--R0", type=int, default=12, help="Basic reproduction number for simulated disease")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.numRuns, args.R0, args.poolSize)

'''import argparse
import csv
import multiprocessing
import os
import time
import xml.etree.ElementTree as ET

from pystride.Event import Event, EventType
from pystride.PyController import PyController

from postprocessing.AgeImmunity import getAvgOverallImmunityRate

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

def writeSeeds(scenarioName, R0, seeds):
    with open(scenarioName + "_R0_" + str(R0) + "_seeds.csv", "w") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

def createRandomImmunityDistributionFiles(immunityRate):
    randomChildImmunity = ET.Element('immunity')
    randomAdultImmunity = ET.Element('immunity')
    for age in range(18):
        elemChild = ET.SubElement(randomChildImmunity, 'age' + str(age))
        elemChild.text = str(immunityRate)
        elemAdult = ET.SubElement(randomAdultImmunity, 'age' + str(age))
        elemAdult.text = str(0)
    for age in range(18, 100):
        elemChild = ET.SubElement(randomChildImmunity, 'age' + str(age))
        elemChild.text = str(0)
        elemAdult = ET.SubElement(randomAdultImmunity, 'age' + str(age))
        elemAdult.text = str(immunityRate)

    ET.ElementTree(randomChildImmunity).write(os.path.join('data', 'measles_random_child_immunity.xml'))
    ET.ElementTree(randomAdultImmunity).write(os.path.join('data', 'measles_random_adult_immunity.xml'))

def runSimulation(scenarioName, R0, seed, extraParams):
    configFile = os.path.join("config", "measles" + scenarioName + ".xml")
    fullScenarioName = scenarioName + "_R0_" + str(R0) + "_" + str(seed)
    control = PyController(data_dir="data")
    control.loadRunConfig(configFile)
    control.runConfig.setParameter("output_prefix", fullScenarioName)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("r0", R0)
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    # Register callbacks
    control.registerCallback(registerSusceptibles, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.registerCallback(registerAgesInfected, EventType.AtFinished)
    control.control()

def runSimulations(numRuns, scenarioName, R0, extraParams, poolSize):
    seeds = generateRngSeeds(numRuns)
    writeSeeds(scenarioName, R0, seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(scenarioName, R0, s, extraParams) for s in seeds])

def main(numRuns, R0s, immunityFileChildren, immunityFileAdults, poolSize):
    start = time.perf_counter()
    adScenarios = ["AD_NCLU", "AD_CLUS"]
    for R0 in R0s:
        print("Running simulations for R0 {}".format(R0))
        # Run scenarios with age-dependent immunity levels
        avgImmunityRates = []
        for scenario in adScenarios:
            runSimulations(numRuns, scenario, R0,
                            {"vaccine_distribution_file": immunityFileChildren,
                            "immunity_distribution_file": immunityFileAdults},
                            poolSize)
            avgImmunityRates.append(getAvgOverallImmunityRate(".", scenario + "_R0_" + str(R0), poolSize))
        avgImmunity = sum(avgImmunityRates) / len(avgImmunityRates)
        print(avgImmunity)
        createRandomImmunityDistributionFiles(avgImmunity)
        # Run scenarios with uniform immunity levels
        runSimulations(numRuns, "UN_NCLU", R0, {"immunity_rate": avgImmunity}, poolSize)
        runSimulations(numRuns, "UN_CLUS", R0,
                        {"vaccine_distribution_file": "data/measles_random_child_immunity.xml",
                        "immunity_distribution_file": "data/measles_random_adult_immunity.xml"},
                        poolSize)
    end = time.perf_counter()
    totalTimeSeconds = end - start
    totalTimeMinutes = totalTimeSeconds / 60
    totalTimeHours = totalTimeMinutes / 60
    print("Total time: {} seconds or {} minutes or {} hours".format(totalTimeSeconds,
                                                                totalTimeMinutes,
                                                                totalTimeHours))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of simulation runs per scenario")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 13], help="Different vallues for R0 to test")
    parser.add_argument("--immunityFileChildren", type=str, default="data/measles_child_immunity_90.xml")
    parser.add_argument("--immunityFileAdults", type=str, default="data/measles_adult_immunity_90.xml")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.numRuns, args.R0s, args.immunityFileChildren, args.immunityFileAdults, args.poolSize)
'''

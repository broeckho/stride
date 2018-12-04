import argparse
import csv
import multiprocessing
import os
import time
import xml.etree.ElementTree as ET

from pystride.Event import Event, EventType
from pystride.PyController import PyController

# Callback function to register person's age, household id
# adn immunity status at the beginning of the simulation
def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue('run.output_prefix')
    pop = simulator.GetPopulation()
    totalPopulation = 0
    totalSusceptible = 0
    with open(os.path.join(outputPrefix, 'susceptibles.csv'), 'w') as csvfile:
        fieldnames = ["age", "susceptible", "household"]
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
            hhId = person.GetHouseholdId()
            writer.writerow({"age": age, "susceptible": isSusceptible, "household": hhId})

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

def getAvgImmunityRate(scenarioNames):
    allImmunityRates = []
    print("Calculating average immunity rate of previous simulations...")
    for scenario in scenarioNames:
        seeds = getRngSeeds(scenario)
        with multiprocessing.Pool(processes=8) as pool:
            scenarioImmunityRates = pool.starmap(getImmunityRate, [(scenario, s) for s in seeds])
        allImmunityRates += scenarioImmunityRates
    return sum(allImmunityRates) / len(allImmunityRates)

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

def runSimulation(scenarioName, R0, startDate, seed, extraParams={}):
    print("Running " + scenarioName + " with R0 " + str(R0) + " and start " + startDate)
    configFile = os.path.join("config", "measles" + scenarioName + ".xml")
    control = PyController(data_dir="data")
    control.loadRunConfig(configFile)
    control.runConfig.setParameter("output_prefix", scenarioName + "_" + str(seed))
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("r0", R0)
    control.runConfig.setParameter("start_date", startDate)
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.registerCallback(registerSusceptibles, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()
    return

def runSimulations(scenarioName, numRuns, R0, startDates, extraParams={}):
    totalRuns = numRuns * len(startDates)
    seeds = generateRngSeeds(totalRuns)
    writeSeeds(scenarioName, seeds)
    jobs = []
    for i in range(totalRuns):
        startDate = startDates[i % len(startDates)]
        print(startDate)
        p = multiprocessing.Process(target=runSimulation, args=(scenarioName, R0, startDate, seeds[i], extraParams))
        jobs.append(p)
        p.start()
    for j in jobs:
        j.join()

def main(numRuns, immunityFileChildren, immunityFileAdults, R0, startDates):
    start = time.perf_counter()
    """
    Run scenarios with age-dependent immunity rates.
    From these runs, a uniform immunity rate can then be calculated.
    """
    # Age-dependent immunity rates + no household-based clustering
    runSimulations("Scenario2", numRuns, R0, startDates,
                    {"immunity_distribution_file": immunityFileAdults,
                    "vaccine_distribution_file": immunityFileChildren})
    # Age-dependent immunity rates + household-based clustering
    runSimulations("Scenario4", numRuns, R0, startDates,
                    {"immunity_distribution_file": immunityFileAdults,
                    "vaccine_distribution_file": immunityFileChildren})
    """
    Calculate uniform immunity rate from previous runs, and
    create uniform age-immunity files.
    """
    immunityRate = getAvgImmunityRate(["Scenario2", "Scenario4"])
    print(immunityRate)
    createRandomImmunityDistributionFiles(immunityRate)
    """
    Run scenarios with uniform immunity rates.
    """
    # Uniform immunity rates + no household-based clustering
    runSimulations("Scenario1", numRuns, R0, startDates, {"immunity_rate": immunityRate})
    # Uniform immunity rates + household-based clustering
    runSimulations("Scenario3", numRuns, R0, startDates,
                    {"immunity_distribution_file": "data/measles_random_adult_immunity.xml",
                    "vaccine_distribution_file": "data/measles_random_child_immunity.xml"})
    end = time.perf_counter()
    print("Total time: {}".format(end - start))
    # TODO convert to more readable time format minutes - seconds - milliseconds - ...

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=10, help="Number of simulation runs per scenario")
    parser.add_argument("--immunityFileChildren", type=str, default="data/measles_child_immunity.xml")
    parser.add_argument("--immunityFileAdults", type=str, default="data/measles_adult_immunity.xml")
    parser.add_argument("--R0", type=int, default=12, help="Value for r0")
    parser.add_argument("--startDates", type=str, nargs="+", default=["2017-01-01"], help="Values for start_date")
    args = parser.parse_args()
    main(args.numRuns, args.immunityFileChildren, args.immunityFileAdults, args.R0, args.startDates)

import argparse
import csv
import os
import xml.etree.ElementTree as ET

from pystride.Event import Event, SteppedEvent, EventType
from pystride.PyController import PyController

# Callback function to register person's age, household id
# and immunity status at the beginning of the simulation
def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue('run.output_prefix')
    pop = simulator.GetPopulation()
    with open(os.path.join(outputPrefix, 'susceptibles.csv'), 'w') as csvfile:
        fieldnames = ["age", "susceptible", "household"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        # write header
        writer.writeheader()
        for i in range(pop.size()):
            person = pop[i]
            age = person.GetAge()
            isSusceptible = person.GetHealth().IsSusceptible()
            isSusceptibleInt = 0
            if isSusceptible:
                isSusceptibleInt = 1
            hhId = person.GetHouseholdId()
            writer.writerow({"age": age, "susceptible": isSusceptibleInt,
                             "household": hhId})

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

def getSeeds(scenarioName):
    with open(scenarioName + "Seeds.csv") as csvfile:
        reader = csv.reader(csvfile)
        seeds = [int(x) for x in next(reader)]
    return seeds

def getAvgImmunityRate(scenarioNames):
    immunityRates = []
    for scenario in scenarioNames:
        # Get seeds
        seeds = getSeeds(scenario)
        for s in seeds:
            totalPersons = 0
            totalSusceptibles = 0
            dirName = scenario + str(s)
            susceptiblesFile = os.path.join(dirName, 'susceptibles.csv')
            with open(susceptiblesFile) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    isSusceptible = int(row['susceptible'])
                    totalPersons += 1
                    if isSusceptible:
                        totalSusceptibles += 1
                pctImmune = 1 - (float(totalSusceptibles) / totalPersons)
                immunityRates.append(pctImmune)
    return sum(immunityRates) / len(immunityRates)

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

def runSimulations(scenarioName, numRuns, extraParams={}):
    # Generate seeds
    seeds = generateRngSeeds(numRuns)
    with open(scenarioName + 'Seeds.csv', 'w') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)
    configFile = os.path.join("config", "measles" + scenarioName + "Immunity.xml")
    for s in seeds:
        control = PyController(data_dir='data')
        control.loadRunConfig(configFile)
        control.runConfig.setParameter('output_prefix', scenarioName + str(s))
        control.runConfig.setParameter('rng_seed', s)
        for paramName, paramValue in extraParams.items():
            control.runConfig.setParameter(paramName, paramValue)
        control.registerCallback(registerSusceptibles, EventType.AtStart)
        control.registerCallback(trackCases, EventType.Stepped)
        control.control()

def main(numRuns):
    # Run AgeClustering scenario
    runSimulations('AgeClustering', numRuns)
    # Run AgeAndHouseholdClustering scenario
    runSimulations('AgeAndHouseholdClustering', numRuns)
    # Calculate immunity rates from previous runs and generate distribution files
    immunityRate = getAvgImmunityRate(['AgeClustering', 'AgeAndHouseholdClustering'])
    createRandomImmunityDistributionFiles(immunityRate)
    # Run Random scenario
    runSimulations('Random', numRuns, {'immunity_rate': immunityRate})
    # Run HouseholdClustering scenario
    runSimulations('HouseholdClustering',
                    numRuns,
                    {'immunity_distribution_file': 'data/measles_random_adult_immunity.xml',
                    'vaccine_distribution_file': 'data/measles_random_child_immunity.xml'})

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--numRuns', type=int, default=1, help='Number of simulation runs per scenario')
    args = parser.parse_args()
    main(args.numRuns)

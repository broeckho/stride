import argparse
import csv
import multiprocessing
import os
import time
import xml.etree.ElementTree as ET

from pystride.PyController import PyController

MAX_AGE = 99

CONFIG_FILE = os.path.join("config", "measlesClustering.xml")
POP_FILE = os.path.join("data", "pop_flanders600.csv")
IMMUNITY_FILE_CHILDREN = os.path.join("data", "measles_child_immunity_90.xml")
IMMUNITY_FILE_ADULTS = os.path.join("data", "measles_adult_immunity_90.xml")
SCENARIO_PARAMS = {
    "UN_NCLU": {
        "immunity_profile": "None",
        "vaccine_link_probability": 0,
        "vaccine_profile": "Random",
        "vaccine_rate": 0,
    },
    "UN_CLU": {
        "immunity_link_probability": 0,
        "immunity_profile": "AgeDependent",
        "immunity_distribution_file": os.path.join("data", "measles_uniform_adult_immunity.xml"),
        "vaccine_link_probability": 1,
        "vaccine_profile": "AgeDependent",
        "vaccine_distribution_file": os.path.join("data", "measles_uniform_child_immunity.xml")
    },
    "AD_NCLU": {
        "immunity_link_probability": 0,
        "immunity_profile": "AgeDependent",
        "immunity_distribution_file": IMMUNITY_FILE_ADULTS,
        "vaccine_link_probability": 0,
        "vaccine_profile": "AgeDependent",
        "vaccine_distribution_file": IMMUNITY_FILE_CHILDREN
    },
    "AD_CLU": {
        "immunity_link_probability": 0,
        "immunity_profile": "AgeDependent",
        "immunity_distribution_file": IMMUNITY_FILE_ADULTS,
        "vaccine_link_probability": 1,
        "vaccine_profile": "AgeDependent",
        "vaccine_distribution_file": IMMUNITY_FILE_CHILDREN
    }
}

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def writeRngSeeds(scenarioName, R0, seeds):
    with open(scenarioName + "_R0_" + str(R0) + "_seeds.csv", "w") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

def getImmunityLevels(xmlTree):
    age = 0
    levels = [0] * (MAX_AGE + 1)
    for child in xmlTree:
        if child.tag not in ["data_source", "data_manipulation"]:
            levels[age] = float(child.text)
            age += 1
    return levels

def calculateUniformImmunityLevel(populationFile, immunityFileChildren, immunityFileAdults):
    totalsByAge = [0] * (MAX_AGE + 1)
    with open(populationFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(row["age"])
            totalsByAge[age] += 1
    childLevels = getImmunityLevels(ET.parse(immunityFileChildren).getroot())
    adultLevels = getImmunityLevels(ET.parse(immunityFileAdults).getroot())
    immunityByAge = [x + y for x,y in zip(childLevels, adultLevels)]
    return sum([immunityByAge[i] * totalsByAge[i] for i in range(MAX_AGE + 1)]) / sum(totalsByAge)

def createUniformImmunityDistributionFiles(immunityLevel):
    uniformChildImmunity = ET.Element('immunity')
    uniformAdultImmunity = ET.Element('immunity')
    for age in range(18):
        elemChild = ET.SubElement(uniformChildImmunity, 'age' + str(age))
        elemChild.text = str(immunityLevel)
        elemAdult = ET.SubElement(uniformAdultImmunity, 'age' + str(age))
        elemAdult.text = str(0)
    for age in range(18, 100):
        elemChild = ET.SubElement(uniformChildImmunity, 'age' + str(age))
        elemChild.text = str(0)
        elemAdult = ET.SubElement(uniformAdultImmunity, 'age' + str(age))
        elemAdult.text = str(immunityLevel)

    ET.ElementTree(uniformChildImmunity).write(os.path.join('data', 'measles_uniform_child_immunity.xml'))
    ET.ElementTree(uniformAdultImmunity).write(os.path.join('data', 'measles_uniform_adult_immunity.xml'))

def runSimulation(R0, scenarioName, seed, numDays):
    control = PyController(data_dir="data")
    control.loadRunConfig(CONFIG_FILE)
    control.runConfig.setParameter("output_prefix",
                                scenarioName + "_R0_" + str(R0) + "_" + str(seed))
    control.runConfig.setParameter("r0", R0)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("num_days", numDays)
    control.runConfig.setParameter("track_index_case", "true")
    scenarioParams = SCENARIO_PARAMS[scenarioName]
    for paramName, paramValue in scenarioParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.control()

def runSimulations(numRuns, R0, scenarioName, numDays, poolSize):
    seeds = generateRngSeeds(numRuns)
    writeRngSeeds(scenarioName, R0, seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(R0, scenarioName, s, numDays) for s in seeds])

def main(numRuns, R0s, numDays, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UN_NCLU", "UN_CLU", "AD_NCLU", "AD_CLU"]
    global UNIFORM_IMMUNITY_LEVEL
    uniform_immunity_level = calculateUniformImmunityLevel(POP_FILE,
                                                        IMMUNITY_FILE_CHILDREN,
                                                        IMMUNITY_FILE_ADULTS)
    global SCENARIO_PARAMS
    SCENARIO_PARAMS["UN_NCLU"]["vaccine_rate"] = uniform_immunity_level
    createUniformImmunityDistributionFiles(uniform_immunity_level)
    for R0 in R0s:
        for scenario in scenarioNames:
            runSimulations(numRuns, R0, scenario, numDays, poolSize)
    end = time.perf_counter()
    totalTimeSeconds = end - start
    totalTimeMinutes = totalTimeSeconds / 60
    totalTimeHours = totalTimeMinutes / 60
    print("Total time: {} seconds or {} minutes or {} hours".format(totalTimeSeconds,
                                                                totalTimeMinutes,
                                                                totalTimeHours))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=10, help="Number of runs per scenario")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values of R0 to test")
    parser.add_argument("--numDays", type=int, default=50, help="Number of simulation days")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.numRuns, args.R0s, args.numDays, args.poolSize)

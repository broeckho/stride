import argparse
import csv
import multiprocessing
import os
import time
import xml.etree.ElementTree as ET

from pystride.PyController import PyController

MAX_AGE = 99

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

def getCommonParameters():
    parameters = {
        "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
        "contact_log_level": "Transmissions",
        "contact_output_file": "true",
        "disease_config_file": "disease_measles.xml",
        "holidays_file": "holidays_none.json",
        "immunity_link_probability": 0,
        "immunity_profile": "AgeDependent",
        "num_participants_survey": 0,
        "num_threads": 1,
        "output_cases": "false",
        "output_persons": "false",
        "output_summary": "false",
        "population_file": "pop_flanders600.csv",
        "population_type": "default",
        "rng_type": "mrg2",
        "seeding_age_min": 1,
        "seeding_age_max": 99,
        "seeding_rate": 0.000001667,
        "start_date": "2019-01-01",
        "stride_log_level": "info",
        "use_install_dirs": "true",
        "vaccine_profile": "AgeDependent",
    }
    return parameters

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
    for age in range(18, MAX_AGE + 1):
        elemChild = ET.SubElement(uniformChildImmunity, 'age' + str(age))
        elemChild.text = str(0)
        elemAdult = ET.SubElement(uniformAdultImmunity, 'age' + str(age))
        elemAdult.text = str(immunityLevel)

    ET.ElementTree(uniformChildImmunity).write(os.path.join('data', 'measles_uniform_child_immunity.xml'))
    ET.ElementTree(uniformAdultImmunity).write(os.path.join('data', 'measles_uniform_adult_immunity.xml'))

def runSimulation(R0, scenarioName, seed, extraParams):
    control = PyController(data_dir="data")
    extraParams.update(getCommonParameters())
    for paramName, paramValue in extraParams.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("output_prefix",
                                    scenarioName + "_R0_" + str(R0) + "_" + str(seed))
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("r0", R0)
    control.control()

def runSimulations(numRuns, R0, scenarioName, extraParams, poolSize):
    seeds = generateRngSeeds(numRuns)
    writeRngSeeds(scenarioName, R0, seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(R0, scenarioName, s, extraParams) for s in seeds])

def main(numRuns, R0s, immunityFileChildren, immunityFileAdults, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM_NOCLUSTERING", "AGEDEPENDENT_NOCLUSTERING",
                        "UNIFORM_CLUSTERING", "AGEDEPENDENT_CLUSTERING"]
    numDays = 100
    scenarioParameters = {
        "UNIFORM_NOCLUSTERING": {
            "immunity_distribution_file": os.path.join("data", "measles_uniform_adult_immunity.xml"),
            "num_days": numDays,
            "track_index_case": "true",
            "vaccine_distribution_file": os.path.join("data", "measles_uniform_child_immunity.xml"),
            "vaccine_link_probability": 0,
        },
        "AGEDEPENDENT_NOCLUSTERING": {
            "immunity_distribution_file": immunityFileAdults,
            "num_days": numDays,
            "track_index_case": "true",
            "vaccine_distribution_file": immunityFileChildren,
            "vaccine_link_probability": 0,
        },
        "UNIFORM_CLUSTERING": {
            "immunity_distribution_file": os.path.join("data", "measles_uniform_adult_immunity.xml"),
            "num_days": numDays,
            "track_index_case": "true",
            "vaccine_distribution_file": os.path.join("data", "measles_uniform_child_immunity.xml"),
            "vaccine_link_probability": 1,
        },
        "AGEDEPENDENT_CLUSTERING": {
            "immunity_distribution_file": immunityFileAdults,
            "num_days": numDays,
            "track_index_case": "true",
            "vaccine_distribution_file": immunityFileChildren,
            "vaccine_link_probability": 1,
        },
    }
    # Calculate uniform immunity rate
    uniformImmunityLevel = calculateUniformImmunityLevel(os.path.join("data",getCommonParameters()["population_file"]),
                                                            immunityFileChildren,
                                                            immunityFileAdults)
    # Create uniform immunity files
    createUniformImmunityDistributionFiles(uniformImmunityLevel)
    for R0 in R0s:
        for scenario in scenarioNames:
            runSimulations(numRuns, R0, scenario, scenarioParameters[scenario], poolSize)
    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=10, help="Number of runs per scenario")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values of R0 to test")
    parser.add_argument("--immunityFileChildren", type=str, default=os.path.join("data", "2020_measles_child_immunity.xml"))
    parser.add_argument("--immunityFileAdults", type=str, default=os.path.join("data", "2020_measles_adult_immunity.xml"))
    parser.add_argument("--poolSize", type=str, default=8, help="Number of workers in multiprocessing pool")

    args = parser.parse_args()
    main(args.numRuns, args.R0s, args.immunityFileChildren, args.immunityFileAdults, args.poolSize)

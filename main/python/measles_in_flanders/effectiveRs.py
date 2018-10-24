import argparse
import csv
import matplotlib.pyplot as plt
import os

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "Seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep == (numDays - 1):
                return int(row["cases"])

def main(outputDir, numDays):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform\nimmunity rates\n+ no clustering",
                            "Age-dependent\nimmunity rates\n+ no clustering",
                            "Uniform\nimmunity rates\n+ clustering",
                            "Age-dependent\nimmunity rates +\nclustering"]
    allRs = []
    for scenario in scenarioNames:
        scenarioRs = []
        seeds = getRngSeeds(outputDir, scenario)
        for s in seeds:
            infectors = {}
            transmissionsFile = os.path.join(outputDir, scenario + "_" + str(s) + "_contact_log.txt")
            '''
            [TRAN INFECTOR_ID INFECTED_ID INFECTOR_AGE INFECTED_AGE CLUSTER_TYPE SIMULATION_DY]
            '''
            with open(transmissionsFile) as f:
                for line in f:
                    transmissionInfo = line.split(' ')
                    infectorId = int(transmissionInfo[2])
                    simDay = int(transmissionInfo[6])
                    if simDay < numDays and transmissionInfo[0] == "[TRAN]":
                        if infectorId in infectors:
                            infectors[infectorId] += 1
                        else:
                            infectors[infectorId] = 1
            avgR = sum(list(infectors.values())) / (getFinalOutbreakSize(outputDir, scenario, s, numDays - 7))
            scenarioRs.append(avgR)
        allRs.append(scenarioRs)
    plt.boxplot(allRs, labels=scenarioDisplayNames)
    plt.ylabel("Effective R")
    plt.savefig(os.path.join(outputDir, "EffectiveRs.png"))
    plt.clf()

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays)

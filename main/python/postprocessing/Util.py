import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

MAX_AGE = 99


def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "_seeds.csv")
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

def getFractionSusceptibles(outputDir, scenarioName, transmissionProbability, clusteringLevel, seed):
    totalPopulation = 0
    totalSusceptibles = 0
    susceptiblesFile = os.path.join(outputDir,
                                    scenarioName + "_CLUSTERING_" + str(clusteringLevel)
                                        + "_TP_" + str(transmissionProbability) + "_" + str(seed),
                                    "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalPopulation += 1
            if (int(row["susceptible"])):
                 totalSusceptibles += 1
    return totalSusceptibles / totalPopulation

def getAllFractionsSusceptibles(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels, poolSize):
    allFractions = []
    for scenario in scenarioNames:
        for level in clusteringLevels:
            for prob in transmissionProbabilities:
                seeds = getRngSeeds(outputDir, scenario + "_CLUSTERING_" + str(level) + "_TP_" + str(prob))
                with multiprocessing.Pool(processes=poolSize) as pool:
                    fractions = pool.starmap(getFractionSusceptibles, [(outputDir, scenario, prob, level, s) for s in seeds])
                    allFractions += fractions
    return allFractions

def saveFig(outputDir, figName, extension="eps"):
    plt.savefig(os.path.join(outputDir, figName + "." + extension), format=extension, dpi=1000)
    plt.clf()

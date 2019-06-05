import itertools
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getRngSeeds, MAX_AGE, saveFig

def getInfectedByAge(outputDir, scenarioName, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    infectedByAge = [0] * (MAX_AGE + 1)
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            ageOfInfected = int(line[3])
            infectedByAge[ageOfInfected] += 1
    return infectedByAge

def createInfectedByAgeScatterplot(outputDir, scenarioName, transmissionProbability, clusteringLevels, poolSize):
    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']
    markers = ['o', 'v', 's', '*', '+']
    i = 0
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, fullScenarioName, s) for s in seeds])
            plt.plot(list(range(MAX_AGE + 1)) * len(infectedByAge), list(itertools.chain.from_iterable(infectedByAge)),
                    colors[i] + markers[i], markersize=4)
        i += 1
    plt.xlabel("Age (in years)")
    plt.ylabel("Number of infected")
    plt.legend(["Clustering = {}".format(c) for c in clusteringLevels])
    saveFig(outputDir, scenarioName + "_InfectedByAgeScatter_TP_" + str(transmissionProbability))

def createInfectedByAgePlot(outputDir, scenarioName, transmissionProbability, clusteringLevels, poolSize):
    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']
    i = 0
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, fullScenarioName, s) for s in seeds])
            meansByAge = []
            for age in range(MAX_AGE + 1):
                infectedInAge = [run[age] for run in infectedByAge]
                meansByAge.append(sum(infectedInAge) / len(infectedInAge))
            plt.plot(range(MAX_AGE + 1), meansByAge, color=colors[i])
        i += 1
    plt.xlabel("Age (in years)")
    plt.ylabel("Number of infected (mean)")
    plt.legend(["Clustering = {}".format(c) for c in clusteringLevels])
    saveFig(outputDir, scenarioName + "_InfectedByAge_TP_" + str(transmissionProbability))

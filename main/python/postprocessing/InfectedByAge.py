import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os

from .Util import getRngSeeds

def getMeanAgeOfInfection(outputDir, scenarioName, seed, numDays):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")

    agesOfInfected = []
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                agesOfInfected.append(int(line[3]))
    if len(agesOfInfected) > 0:
        return sum(agesOfInfected) / len(agesOfInfected)

def meanAgeOfInfectionHeatmap(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, numDays, poolSize):
    allAges = []
    for prob in transmissionProbabilities:
        ages = []
        for level in clusteringLevels:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                meanAgesOfInfection = pool.starmap(getMeanAgeOfInfection,
                                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                meanAgesOfInfection = [x for x in meanAgesOfInfection if x is not None]
                if len(meanAgesOfInfection) > 0:
                    ages.append(sum(meanAgesOfInfection) / len(meanAgesOfInfection))
                else:
                    ages.append(numpy.nan)
        allAges.append(ages)
    plt.imshow(allAges, vmin=0, vmax=100)
    plt.colorbar()
    plt.xlabel("Clustering level")
    plt.xticks(range(len(clusteringLevels)), clusteringLevels)
    plt.ylabel("Transmission probability")
    plt.yticks(range(len(transmissionProbabilities))[::2], transmissionProbabilities[::2])
    saveFig(outputDir, "MeanAgeOfInfection_" + scenarioName)

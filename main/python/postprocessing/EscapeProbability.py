import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from mpl_toolkits.mplot3d import Axes3D

from .Util import COLORS
from .Util import getFinalOutbreakSize, getRngSeeds, saveFig

def getTotalNumSusceptibles(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles_by_age.csv")
    totalSusceptibles = 0
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalSusceptibles += int(row["susceptible"])
    return totalSusceptibles

def getEscapeProbability(outputDir, scenarioName, seed, numDays):
    numSusceptibles = getTotalNumSusceptibles(outputDir, scenarioName, seed)
    numInfected = getFinalOutbreakSize(outputDir, scenarioName, seed, numDays)
    return (1 - (numInfected / numSusceptibles))

def createEscapeProbabilityBoxplot(outputDir, scenarioName, transmissionProbability,
    clusteringLevels, numDays, poolSize):
    allEscapeProbabilities = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            allEscapeProbabilities.append(pool.starmap(getEscapeProbability,
                                            [(outputDir, fullScenarioName, s, numDays) for s in seeds]))
    plt.boxplot(allEscapeProbabilities, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Escape probability after {} days".format(numDays))
    #plt.ylim(0.9, 1)
    saveFig(outputDir, "EscapeProbability_" + scenarioName + "_TP_" + str(transmissionProbability))

def createEscapeProbabilityHeatmapPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, poolSize):
    allEscapeProbabilities = []
    for prob in transmissionProbabilities:
        means = []
        for level in clusteringLevels:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                escapeProbabilities = pool.starmap(getEscapeProbability,
                                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                means.append(sum(escapeProbabilities) / len(escapeProbabilities))
        allEscapeProbabilities.append(means)
    plt.imshow(allEscapeProbabilities) # TODO set vmin=0 and vmax=1?
    plt.colorbar()
    plt.xlabel("Clustering level")
    plt.xticks(range(len(clusteringLevels)), clusteringLevels)
    plt.ylabel("Transmission probability")
    plt.yticks(range(len(transmissionProbabilities))[::2], transmissionProbabilities[::2])
    saveFig(outputDir, "EscapeProbabilityHeatmap_" + scenarioName)

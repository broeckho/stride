import matplotlib.pyplot as plt
import multiprocessing

from .Util import getFinalOutbreakSize, getRngSeeds, saveFig

from mpl_toolkits.mplot3d import Axes3D

def createExtinctionThresholdHistogram(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, numDays, poolSize):
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    for prob in transmissionProbabilities:
        finalSizes = []
        for level in clusteringLevels:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes.append(pool.starmap(getFinalOutbreakSize,
                                    [(outputDir, fullScenarioName, s, numDays) for s in seeds]))
        n, bins, patches = plt.hist(finalSizes, bins=20, histtype="barstacked", color=colors[:len(finalSizes)])
        hatches = ['-', '+', 'x', '\\', 'o', '.']
        for patch_set, hatch in zip(patches, hatches):
            plt.setp(patch_set, hatch=hatch)
        plt.xlabel("Total number of cases after {} days".format(numDays))
        plt.ylabel("Frequency")
        plt.legend(["Clustering level = {}".format(c) for c in clusteringLevels])
        saveFig(outputDir, scenarioName + "_FinalSizesHist_TP_" + str(prob))

def createOutbreakSizes3DPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, numDays, extinctionThreshold, poolSize):
    ax = plt.axes(projection="3d")
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    for prob_i in range(len(transmissionProbabilities))[::2]:
        means = []
        for level_i in range(len(clusteringLevels)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                finalSizes = [s for s in finalSizes if s >= extinctionThreshold]
                if len(finalSizes) > 0:
                    means.append(sum(finalSizes) / len(finalSizes))
                else:
                    means.append(0)
        ax.bar(range(len(clusteringLevels)), means, zs=prob_i, zdir="y", alpha=0.5, color=colors[prob_i % len(colors)])
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Final outbreak size after {} days".format(numDays))
    ax.set_zlim(-0.1, 50000)
    saveFig(outputDir, scenarioName + "_OutbreakSizes3D", "png")

def createOutbreakSizesPlot(outputDir, scenarioName, transmissionProbability,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    allFinalSizes = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, fullScenarioName, s, numDays) for s in seeds])
            finalSizes = [s for s in finalSizes if s >= extinctionThreshold]
            allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Final outbreak size after {} days".format(numDays))
    plt.ylim(extinctionThreshold, 50000)
    saveFig(outputDir, scenarioName + "_OutbreakSizes_TP_" + str(transmissionProbability))

"""import csv
import math
import os
import statistics

def getSusceptiblesAtStart(outputDir, scenarioName, seed):
    totalSusceptible = 0
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            if int(row["susceptible"]):
                totalSusceptible += 1
    return totalSusceptible

def getEscapeProbability(outputDir, scenarioName, seed, numDays):
    totalInfected = getFinalOutbreakSize(outputDir, scenarioName, seed, numDays)
    totalSusceptible = getSusceptiblesAtStart(outputDir, scenarioName, seed)
    if totalSusceptible > 0:
        return (totalSusceptible - totalInfected) / totalSusceptible
    else:
        return 1 # If no-one is susceptible, escape probability is 1

def createEscapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames,
    numDays, extinctionThreshold, poolSize, figName):
    allEscapeProbabilities = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            escapeProbabilities = pool.starmap(getEscapeProbability,
                                        [(outputDir, scenario, s, numDays, extinctionThreshold) for s in seeds])
            allEscapeProbabilities.append([x for x in escapeProbabilities if x is not None])
    plt.boxplot(allEscapeProbabilities, labels=scenarioDisplayNames)
    plt.ylabel("Escape probability")
    plt.ylim(0, 1.05)
    saveFig(outputDir, figName)

def createEscapeProbabilityOverviewPlots(outputDir, R0s, scenarioNames, scenarioDisplayNames,
    numDays, poolSize, figName, stat="mean"):
    ax = plt.axes(projection="3d")

    z = 0
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for R0 in R0s:
        allEscapeProbabilities = []
        for scenario in scenarioNames:
            scenarioName = scenario + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                escapeProbabilities = pool.starmap(getEscapeProbability,
                                            [(outputDir, scenarioName, s, numDays) for s in seeds])
                if stat == "mean":
                    allEscapeProbabilities.append(sum(escapeProbabilities) / len(escapeProbabilities))
                elif stat == "median":
                    allEscapeProbabilities.append(statistics.median(escapeProbabilities))
        plt.bar(range(len(scenarioNames)), allEscapeProbabilities, zs=R0, zdir="y", color=colors[z], alpha=0.4)
        z += 1
    ax.set_xlabel("Scenario")
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(R0s)
    ax.set_zlabel("Escape probability ({})".format(stat))
    ax.set_zlim(0, 1)
    saveFig(outputDir, figName, "png")
"""

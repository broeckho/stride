import itertools
import matplotlib.pyplot as plt
import multiprocessing
import os

from mpl_toolkits.mplot3d import Axes3D

from .Util import getRngSeeds, saveFig

def getEffectiveR(outputDir, scenarioName, transmissionProbability, clusteringLevel, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_CLUSTERING_"
                                                    + str(clusteringLevel) + "_TP_"
                                                    + str(transmissionProbability)
                                                    + "_" + str(seed) + "_contact_log.txt")
    indexCaseIDs = []
    infectedBy = {}
    infectedByIndexCases = []
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(' ')
            if line[0] == "[PRIM]":
                indexCaseIDs.append(int(line[1]))
            elif line[0] == "[TRAN]":
                infectorID = int(line[2])
                if infectorID in infectedBy:
                    infectedBy[infectorID] += 1
                else:
                    infectedBy[infectorID] = 1
    if len(indexCaseIDs) > 1:
        print("More than one index case!")
    for indexCase in indexCaseIDs:
        if indexCase in infectedBy:
            infectedByIndexCases.append(infectedBy[indexCase])
        else:
            infectedByIndexCases.append(0)
    return sum(infectedByIndexCases) / len(infectedByIndexCases)

def createEffectiveROverviewPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels, poolSize):
    ax = plt.axes(projection="3d")
    colors = ["blue", "orange", "pink", "green", "red", "purple", "brown", "magenta", "cyan", "firebrick"]

    z = 0

    for prob in transmissionProbabilities:
        effectiveRs = []
        for scenario in scenarioNames:
            for level in clusteringLevels:
                seeds = getRngSeeds(outputDir, scenario + "_CLUSTERING_" + str(level) + "_TP_" + str(prob))
                with multiprocessing.Pool(processes=poolSize) as pool:
                    effectiveRs.append(pool.starmap(getEffectiveR, [(outputDir, scenario, prob, level, s) for s in seeds]))
                    # TODO add median?
        ax.bar(range(len(effectiveRs)), [sum(x) / len(x) for x in effectiveRs], zs=z, zdir="y", color=colors[z], alpha=0.4)
        z += 1
    #ax.set_xlabel("Scenario")
    ax.set_xticks(range(len(scenarioNames) * len(clusteringLevels)))
    ax.set_xticklabels([x.capitalize() + ",\nClustering = " + str(y) for x, y in itertools.product(scenarioNames, clusteringLevels)],
                        rotation=45)
    ax.set_ylabel("P(transmission)")
    ax.set_yticks(range(len(transmissionProbabilities)))
    ax.set_yticklabels(transmissionProbabilities)
    ax.set_zlabel("Effective R (mean)")
    saveFig(outputDir, "EffectiveRs", "png")

def createEffectiveRPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevel, poolSize):
    allEffectiveRs = []
    for prob in transmissionProbabilities:
        effectiveRs = []
        seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob))
        with multiprocessing.Pool(processes=poolSize) as pool:
            effectiveRs += pool.starmap(getEffectiveR,
                                    [(outputDir, scenarioName, prob, clusteringLevel, s) for s in seeds])
        allEffectiveRs.append(effectiveRs)
    plt.boxplot(allEffectiveRs, labels=transmissionProbabilities)
    plt.xlabel("P(transmission)")
    plt.ylabel("Effective R")
    saveFig(outputDir, "EffectiveRs_" + scenarioName + "_C_" + str(clusteringLevel))


'''
from numpy import median, percentile
from random import sample

def createEffectiveRTracePlot(outputDir, sampleSizesRange, scenarioName, poolSize):
    allEffectiveRs = []
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        allEffectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenarioName, s) for s in seeds])
    medians = []
    q1s = []
    q3s = []
    sampleSizes = range(sampleSizesRange[0], sampleSizesRange[1], sampleSizesRange[2])
    for size in sampleSizes:
        effectiveRs = sample(allEffectiveRs, size)
        medians.append(median(effectiveRs))
        q1s.append(percentile(effectiveRs, 25))
        q3s.append(percentile(effectiveRs, 75))
    plt.plot(sampleSizes, medians)
    plt.plot(sampleSizes, q1s)
    plt.plot(sampleSizes, q3s)
    plt.xlabel("Iterations")
    plt.ylabel("Effective R")
    plt.legend(["Median", "Q1", "Q3"])
    saveFig(outputDir, scenarioName + "_EffectiveRTrace")
'''

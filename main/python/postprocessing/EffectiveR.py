'''import math
import numpy

from .Util import getRngSeeds, saveFig

def lnFunc(x, a, b):
    return a + (b * math.log(1 + x))

def createEffectiveRScatterPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevel, poolSize):
    allTransmissionProbabilities = []
    allEffectiveRs = []
    for prob in transmissionProbabilities:
        seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob))
        with multiprocessing.Pool(processes=poolSize) as pool:
            allEffectiveRs += pool.starmap(getEffectiveR, [(outputDir, scenarioName, prob, clusteringLevel, s) for s in seeds])
            allTransmissionProbabilities += [prob] * len(seeds)
    plt.plot(allTransmissionProbabilities, allEffectiveRs, "bo")
    plt.xlabel("Transmission probability")
    plt.ylabel("Secondary cases")
    plt.ylim(0, 10)
    saveFig(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_ScatterSecCases")

def createEffectiveRPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevel, poolSize, r0CoeffA, r0CoeffB, fracSusceptibles):
    allEffectiveRs = []
    expectedRs = [(r0CoeffA + (r0CoeffB * math.log(1 + x))) * fracSusceptibles for x in numpy.arange(0, 1.05, 0.05)]
    for prob in transmissionProbabilities:
        seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob))
        with multiprocessing.Pool(processes=poolSize) as pool:
            allEffectiveRs.append(pool.starmap(getEffectiveR, [(outputDir, scenarioName, prob, clusteringLevel, s) for s in seeds]))
    plt.boxplot(allEffectiveRs, labels=transmissionProbabilities)
    plt.plot(range(len(expectedRs)), expectedRs)
    plt.xlabel("P(transmission)")
    plt.ylabel("Effective R")
    saveFig(outputDir, "EffectiveRs_" + scenarioName + "_C_" + str(clusteringLevel))

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

import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os
import random

from mpl_toolkits.mplot3d import Axes3D

from .Util import getRngSeeds, saveFig

def getRandomEffectiveR(outputDir, scenarioName, transmissionProbability, clusteringLevel, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_CLUSTERING_"
                            + str(clusteringLevel) + "_TP_"
                            + str(transmissionProbability) + "_"
                            + str(seed) + "_contact_log.txt")
    infectors = {}
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            infectorID = int(line[2])
            if infectorID != -1:
                if infectorID in infectors:
                    infectors[infectorID] += 1
                else:
                    infectors[infectorID] = 1
    if len(infectors) > 0:
        randomInfector = random.choice(list(infectors.keys()))
        return infectors[randomInfector]
    else:
        return 0

def createEffectiveROverviewPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize, stat="mean"):
    ax = plt.axes(projection="3d")
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    z = 0
    for prob in transmissionProbabilities:
        effectiveRs = []
        for level in clusteringLevels:
            seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob))
            with multiprocessing.Pool(processes=poolSize) as pool:
                effectiveRs.append(pool.starmap(getRandomEffectiveR, [(outputDir, scenarioName, prob, level, s) for s in seeds]))
        if stat == "mean":
            ax.bar(range(len(effectiveRs)), [sum(x) / len(x) for x in effectiveRs], zs=z, zdir="y", color=colors[z % len(colors)], alpha=0.5)
        elif stat == "median":
            ax.bar(range(len(effectiveRs)), [numpy.median(x) for x in effectiveRs], zs=z, zdir="y", color=colors[z % len(colors)], alpha=0.5)
        z += 1
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("P(transmission)")
    ax.set_yticks(range(len(transmissionProbabilities)))
    ax.set_yticklabels(transmissionProbabilities)
    ax.set_zlabel("Effective R ({})".format(stat))
    #ax.set_zlim()
    saveFig(outputDir, scenarioName + "_EffectiveRs_" + stat, "png")

'''
def createEffectiveROverviewPlot(outputDir, scenario, transmissionProbabilities,
    clusteringLevels, poolSize, r0CoeffA, r0CoeffB, fracSusceptibles, stat="mean"):
    handles = []
    for prob in transmissionProbabilities:
        r0 = lnFunc(prob, r0CoeffA, r0CoeffB)
        expectedR = r0 * fracSusceptibles
        line, = ax.plot(range(len(effectiveRs)), [expectedR] * len(effectiveRs), zs=z, zdir="y", color=colors[z % len(colors)], label="R0 * fraction susceptible = {:.2f}".format(expectedR))
        handles.append(line)
    ax.legend(handles=handles, bbox_to_anchor=(0.43,1.15))
'''

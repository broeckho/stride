import math
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

def getIndexCaseEffectiveR(outputDir, scenarioName, transmissionProbability, clusteringLevel, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_CLUSTERING_"
                            + str(clusteringLevel) + "_TP_"
                            + str(transmissionProbability) + "_"
                            + str(seed) + "_contact_log.txt")
    indexCases = []
    infectors = {}
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            infectorID = int(line[2])
            if infectorID == -1:
                indexCases.append(int(line[1]))
            else:
                if infectorID in infectors:
                    infectors[infectorID] += 1
                else:
                    infectors[infectorID] = 1
    if len(indexCases) > 1:
        print("Multiple index cases!")
    else:
        if indexCases[0] in infectors:
            return infectors[indexCases[0]]
        else:
            return 0

def lnFunc(x, a, b):
    return a + (b * math.log(x + 1))

def createEffectiveRPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevel, poolSize, r0CoeffA, r0CoeffB, erCalculation="random"):
    allEffectiveRs = []
    for prob in transmissionProbabilities:
        seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob))
        with multiprocessing.Pool(processes=poolSize) as pool:
            if erCalculation == "random":
                allEffectiveRs.append(pool.starmap(getRandomEffectiveR, [(outputDir, scenarioName, prob, clusteringLevel, s) for s in seeds]))
            elif erCalculation == "index":
                allEffectiveRs.append(pool.starmap(getIndexCaseEffectiveR, [(outputDir, scenarioName, prob, clusteringLevel, s) for s in seeds]))
    plt.boxplot(allEffectiveRs, labels=transmissionProbabilities)
    plt.xlabel("Transmission probability")
    plt.xticks(range(len(transmissionProbabilities)),
                [""] + ['{}\n'.format(x) + r'$R_0 \approx$' + ' {:.2f}'.format(lnFunc(x, r0CoeffA, r0CoeffB)) for x in transmissionProbabilities])
    plt.ylabel("Secondary cases from {} case".format(erCalculation))
    #plt.tight_layout()
    saveFig(outputDir, "EffectiveRs_" + scenarioName + "_C_" + str(clusteringLevel))

def createEffectiveRHeatmap(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize, r0CoeffA, r0CoeffB, erCalculation="random"):
    allEffectiveRs =[]
    for prob in transmissionProbabilities:
        effectiveRsProb = []
        for level in clusteringLevels:
            seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob))
            with multiprocessing.Pool(processes=poolSize) as pool:
                effectiveRs = []
                if erCalculation == "random":
                    effectiveRs = pool.starmap(getRandomEffectiveR, [(outputDir, scenarioName, prob, level, s) for s in seeds])
                elif erCalculation == "index":
                    effectiveRs = pool.starmap(getIndexCaseEffectiveR, [(outputDir, scenarioName, prob, level, s) for s in seeds])
                effectiveRsProb.append(sum(effectiveRs) / len(effectiveRs))
        allEffectiveRs.append(effectiveRsProb)
    plt.imshow(allEffectiveRs, cmap="afmhot")
    plt.xlabel("Clustering level")
    locs, labels = plt.xticks()
    plt.xticks(locs, [""] + clusteringLevels + [""])
    plt.ylabel("Transmission probability")
    locs, labels = plt.yticks()
    plt.yticks(locs, [""] + transmissionProbabilities + [""])
    saveFig(outputDir, scenarioName + "_EffectiveRs_heatmap")

def createEffectiveR3DPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize, r0CoeffA, r0CoeffB, erCalculation="random"):
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
                if erCalculation == "random":
                    effectiveRs.append(pool.starmap(getRandomEffectiveR, [(outputDir, scenarioName, prob, level, s) for s in seeds]))
                elif erCalculation == "index":
                    effectiveRs.append(pool.starmap(getIndexCaseEffectiveR, [(outputDir, scenarioName, prob, level, s) for s in seeds]))
        ax.bar(range(len(effectiveRs)), [sum(x) / len(x) for x in effectiveRs], zs=z, zdir="y", color=colors[z % len(colors)], alpha=0.5)
        z += 1
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities)))
    ax.set_yticklabels(['{}\n'.format(x) + r'$R_0 \approx$' + ' {:.2f}'.format(lnFunc(x, r0CoeffA, r0CoeffB)) for x in transmissionProbabilities])
    ax.set_zlabel("Effective R (mean)")
    saveFig(outputDir, scenarioName + "_EffectiveRs_" + erCalculation, "png")

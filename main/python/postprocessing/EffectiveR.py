import matplotlib.pyplot as plt
import multiprocessing
import os
import statistics

from mpl_toolkits.mplot3d import Axes3D

from .Util import getRngSeeds, saveFig

def getEffectiveR(outputDir, scenarioName, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    totalInfected = 0
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(' ')
            if line[0] == "[TRAN]":
                totalInfected += 1
    return totalInfected

def createEffectiveRPlot(outputDir, scenarioNames, scenarioDisplayNames, poolSize, xLabel, figName):
    allEffectiveRs = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenario, s) for s in seeds])
            allEffectiveRs.append(effectiveRs)
    plt.boxplot(allEffectiveRs, labels=scenarioDisplayNames)
    plt.xlabel(xLabel)
    plt.ylabel("Effective R")
    saveFig(outputDir, figName)

def createEffectiveROverviewPlot(outputDir, scenarioNames, scenarioDisplayNames, R0s, poolSize, figName, stat="mean"):
    ax = plt.axes(projection="3d")
    z = 0
    for R0 in R0s:
        results = []
        for scenario in scenarioNames:
            scenarioName = str(scenario) + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenarioName, s) for s in seeds])
                if stat == "mean":
                    results.append(sum(effectiveRs) / len(effectiveRs))
                elif stat == "median":
                    results.append(statistics.median(effectiveRs))
                else:
                    print("No valid statistic supplied!")
        ax.bar(range(len(results)), results, zs=z, zdir="y", alpha=0.8)
        z += 1
    ax.set_xlabel("Calendar year")
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(range(len(R0s)))
    ax.set_yticklabels(R0s)
    ax.set_zlabel("Effective R")
    saveFig(outputDir, figName)

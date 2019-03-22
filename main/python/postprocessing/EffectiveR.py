import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from numpy import median, percentile
from random import sample

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

def getExpectedR(R0, fractionSusceptibles):
    return R0 * fractionSusceptibles

def createEffectiveRPlot(outputDir, R0, scenarioNames, scenarioDisplayNames,
    fractionSusceptibles, poolSize, xLabel, figName):
    expectedR = getExpectedR(R0, fractionSusceptibles)
    allEffectiveRs = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenario, s) for s in seeds])
            allEffectiveRs.append(effectiveRs)
    plt.axhline(expectedR)
    plt.boxplot(allEffectiveRs, labels=scenarioDisplayNames)
    plt.legend(["Expected R = {0:.3f}".format(expectedR)])
    plt.xlabel(xLabel)
    plt.ylabel("Effective R")
    saveFig(outputDir, figName)

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

def createEffectiveROverviewPlot(outputDir, R0s, scenarioNames, scenarioDisplayNames,
    fractionSusceptibles, poolSize, xLabel, figName, stat="mean"):
    ax = plt.axes(projection="3d")
    colors = ["blue", "orange", "pink", "green", "red", "purple", "brown", "magenta", "cyan", "firebrick"]
    expectedRs = []
    z = 0
    for R0 in R0s:
        expectedR = getExpectedR(R0, fractionSusceptibles)
        expectedRs.append(expectedR)
        results = []
        for scenario in scenarioNames:
            scenarioName = str(scenario) + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenarioName, s) for s in seeds])
                if stat == "mean":
                    results.append(sum(effectiveRs) / len(effectiveRs))
                elif stat == "median":
                    results.append(median(effectiveRs))
                else:
                    print("No valid statistic supplied!")
        ax.plot(range(len(results)), [expectedR] * len(results), zs=R0, zdir="y", color=colors[z])
        ax.bar(range(len(results)), results, zs=R0, zdir="y", color=colors[z], alpha=0.4)
        z+= 1
    ax.set_xlabel(xLabel)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(R0s)
    ax.set_xlim(R0s[0] - 1, R0s[-1] + 1)
    ax.set_zlabel("Effective R ({})".format(stat))
    ax.set_zlim(0, max(results))
    plt.legend(["Expected R = {0:.3f}".format(r) for r in expectedRs])
    saveFig(outputDir, figName, "png")

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

def getExpectedR(outputDir, R0, scenarioName, seed):
    totalSusceptibles = 0
    totalPersons = 0
    susceptiblesfile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalPersons += 1
            isSusceptible = int(row["susceptible"])
            if isSusceptible:
                totalSusceptibles += 1
    fracSusceptibles = totalSusceptibles / totalPersons
    return R0 * fracSusceptibles

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

def createEffectiveROverviewPlot(outputDir, scenarioNames, scenarioDisplayNames, R0s, poolSize, xLabel, figName, stat="mean"):
    ax = plt.axes(projection="3d")
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
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
        ax.bar(range(len(results)), results, zs=z, zdir="y", color=colors[z], alpha=0.4)
        z += 1
    ax.set_xlabel(xLabel)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(range(len(R0s)))
    ax.set_yticklabels(R0s)
    ax.set_zlabel("Effective R ({})".format(stat))
    saveFig(outputDir, figName, "png")

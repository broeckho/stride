import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getRngSeeds

def getEffectiveR(outputDir, scenarioName, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    totalInfected = 0
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(' ')
            if line[0] == "[TRAN]":
                totalInfected += 1
    return totalInfected


def createEffectiveRPlot(outputDir, scenarioNames, scenarioDisplayNames, poolSize, figName):
    allEffectiveRs = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenario, s) for s in seeds])
            allEffectiveRs.append(effectiveRs)
    plt.boxplot(allEffectiveRs, labels=scenarioDisplayNames)
    saveFig(outputDir, figName)


def createEffectiveROverviewPlot(outputDir, scenarioNames, scenarioDisplayNames, R0s):
    pass

import multiprocessing
import os

from .Util import getRngSeeds

"""
import csv
import matplotlib.pyplot as plt

def main(outputDir, numDays):
            infectors = {}
            '''
            [TRAN INFECTOR_ID INFECTED_ID INFECTOR_AGE INFECTED_AGE CLUSTER_TYPE SIMULATION_DY]
            '''
            with open(transmissionsFile) as f:
                for line in f:
                    infectorId = int(transmissionInfo[2])
                    simDay = int(transmissionInfo[6])
                    if simDay < numDays and transmissionInfo[0] == "[TRAN]":
                        if infectorId in infectors:
                            infectors[infectorId] += 1
                        else:
                            infectors[infectorId] = 1
            avgR = sum(list(infectors.values())) / (getFinalOutbreakSize(outputDir, scenario, s, numDays - 7))
            scenarioRs.append(avgR)
        allRs.append(scenarioRs)
    plt.boxplot(allRs, labels=scenarioDisplayNames)
    plt.ylabel("Effective R")
    plt.savefig(os.path.join(outputDir, "EffectiveRs.png"))
    plt.clf()
"""

def getEffectiveR(outputDir, scenarioName, seed, numDays):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    with open(transmissionsFile) as f:
        for line in f:
            transmissionInfo = line.split(" ")

def createEffectiveRBoxplot(outputDir, scenarioNames, scenarioDisplayNames, numDays, poolSize, figName):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenario, s, numDays) for s in seeds])

#TODO 2D effective R
#TODO 3D effective R

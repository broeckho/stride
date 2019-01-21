import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getFinalOutbreakSize, getRngSeeds, saveFig

def createFinalSizeHistogram(outputDir, scenarioName, numDays, poolSize, figName):
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        finalSizes = pool.starmap(getFinalOutbreakSize,
                                [(outputDir, scenarioName, s, numDays) for s in seeds])
        plt.hist(finalSizes)
        plt.xlabel("Final size after {} days".format(numDays))
        plt.ylabel("Frequency")
        plt.title(scenarioName)
        saveFig(outputDir, figName)

def createFinalSizesHistogram(outputDir, scenarioNames, scenarioDisplayNames, numDays, poolSize, figName):
    allFinalSizes = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize,
                                    [(outputDir, scenario, s, numDays) for s in seeds])
            allFinalSizes.append(finalSizes)
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    n, bins, patches = plt.hist(allFinalSizes,bins=25,histtype="barstacked", color=colors[:len(allFinalSizes)])
    hatches = ['-', '+', 'x', '\\', 'o', '.']
    for patch_set, hatch in zip(patches, hatches):
        plt.setp(patch_set, hatch=hatch)
    plt.xlabel("Final size after {} days".format(numDays))
    plt.ylabel("Frequency")
    plt.legend(scenarioDisplayNames)
    saveFig(outputDir, figName)

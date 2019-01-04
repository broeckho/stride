import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getFinalOutbreakSize, getRngSeeds

def createFinalSizeHistogram(outputDir, scenarioName, numDays, poolSize, figName):
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        finalSizes = pool.starmap(getFinalOutbreakSize,
                                [(outputDir, scenarioName, s, numDays) for s in seeds])
        plt.hist(finalSizes)
        plt.xlabel("Final size after {} days".format(numDays))
        plt.ylabel("Frequency")
        plt.title(scenarioName)
        plt.savefig(os.path.join(outputDir, figName))
        plt.clf()

def createFinalSizesHistogram(outputDir, scenarioNames, scenarioDisplayNames, numDays, poolSize, figName):
    allFinalSizes = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize,
                                    [(outputDir, scenario, s, numDays) for s in seeds])
            allFinalSizes.append(finalSizes)
    plt.hist(allFinalSizes,bins=25,histtype="barstacked")
    plt.xlabel("Final size after {} days".format(numDays))
    plt.ylabel("Frequency")
    plt.legend(scenarioDisplayNames)
    plt.savefig(os.path.join(outputDir, figName))
    plt.clf()

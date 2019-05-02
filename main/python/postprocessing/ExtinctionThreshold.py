import csv
import itertools
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getFinalOutbreakSize, getRngSeeds, saveFig

def createFinalSizesHistogram(outputDir, scenarioNames, transmissionProbability, clusteringLevels, numDays, poolSize):
    finalSizes = []
    for scenario in scenarioNames:
        for level in clusteringLevels:
            scenarioNameFull = scenario + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
            seeds = getRngSeeds(outputDir, scenarioNameFull)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes.append(pool.starmap(getFinalOutbreakSize,
                            [(outputDir, scenarioNameFull, s, numDays) for s in seeds]))
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    n, bins, patches = plt.hist(finalSizes,bins=25,histtype="barstacked", color=colors[:len(finalSizes)])
    #hatches = ['-', '+', 'x', '\\', 'o', '.']
    #for patch_set, hatch in zip(patches, hatches):
    #    plt.setp(patch_set, hatch=hatch)
    plt.xlabel("Final size after {} days".format(numDays))
    plt.ylabel("Frequency")
    plt.legend([str(x).capitalize() + " clustering = " + str(y) for x, y in itertools.product(scenarioNames, clusteringLevels)])
    saveFig(outputDir, "FinalSizes_TP_" + str(transmissionProbability))

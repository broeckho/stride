import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getFinalOutbreakSize, getRngSeeds

"""
'''
def chunks(l, n):
#Yield successive n-sized chunks from l.
    for i in range(0, len(l), n):
        yield l[i:i + n]
'''

"""

def createOutbreakOccurrencePlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold, poolSize, figName):
    pctOutbreaks = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, scenario, s, numDays) for s in seeds])
            outbreaks = [1 if x >= extinctionThreshold else 0 for x in finalSizes]
            pctOutbreaks.append((sum(outbreaks) / len(outbreaks)) * 100)
    plt.bar(scenarioDisplayNames, pctOutbreaks)
    plt.ylim(0, 100)
    plt.ylabel("Percentage outbreaks")
    plt.savefig(os.path.join(outputDir, figName))
    plt.clf()

def createFinalSizesBoxplot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold, poolSize, figName):
    allFinalSizes = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, scenario, s, numDays) for s in seeds])
            finalSizes = [x for x in finalSizes if x >= extinctionThreshold]
            allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=scenarioDisplayNames)
    plt.ylabel("Finals outbreak size after {} days".format(numDays))
    plt.savefig(os.path.join(outputDir, figName))
    plt.clf()

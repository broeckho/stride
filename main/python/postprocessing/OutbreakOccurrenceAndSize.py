import csv
import math
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getFinalOutbreakSize, getRngSeeds

def calculateSE(fractionSuccesses, sampleSize, confidenceLevel=95):
    zValues = {90: 1.645, 95: 1.96, 99: 2.576}
    # There should be at least 5 successes and 5 failures for this formula
    # to be appropriate
    numSuccesses = fractionSuccesses * sampleSize
    numFailures = sampleSize - numSuccesses
    if numSuccesses >= 5 and numFailures >= 5:
        z = zValues[confidenceLevel]
        se = z * math.sqrt((fractionSuccesses * (1 - fractionSuccesses)) / sampleSize)
        return se
    else:
        print("Confidence interval could not be calculated: sample too small")

def createOutbreakOccurrencePlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold, poolSize, figName):
    fractionOutbreaks = []
    numRuns = 0
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        numRuns = len(seeds)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, scenario, s, numDays) for s in seeds])
            outbreaks = [1 if x>= extinctionThreshold else 0 for x in finalSizes]
            fractionOutbreaks.append(sum(outbreaks) / len(outbreaks))
    if numRuns > 0:
        SEs = []
        # Calculate confidence intervals
        for frac in fractionOutbreaks:
            se = calculateSE(frac, numRuns)
            if se is not None:
                SEs.append(se)
            else:
                SEs.append(0)
        plt.errorbar(scenarioDisplayNames, fractionOutbreaks, SEs, fmt="o", ecolor="red", capsize=4)
        plt.ylabel("Fraction outbreaks")
        plt.ylim(0, 1)
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

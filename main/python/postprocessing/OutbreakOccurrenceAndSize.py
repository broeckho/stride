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
        plt.errorbar([int(x) for x in scenarioDisplayNames], fractionOutbreaks, SEs, fmt="o", ecolor="red", capsize=4)
        plt.ylabel("Fraction outbreaks")
        plt.ylim(0, 1)
        plt.xlim(2012, 2041)
        plt.xticks([2013, 2020, 2025, 2030, 2035, 2040])
        plt.savefig(os.path.join(outputDir, figName))
        plt.clf()

def createOutbreakOccurrenceOverviewPlot(outputDir, R0s, years, numDays, extinctionThreshold, poolSize):
    fmts = ['o', 'v', '+', 'D', '.', '*', 'x']
    for R0_i in range(len(R0s)):
        R0 = R0s[R0_i]
        fractionOutbreaks = []
        numRuns = 0
        for year in years:
            scenarioName = str(year) + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            numRuns = len(seeds)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, scenarioName, s, numDays) for s in seeds])
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
            plt.errorbar(years, fractionOutbreaks, SEs, fmt=fmts[R0_i], markersize=7, capsize=5)
            #TODO ecolor?
    plt.xlim(2012, 2041)
    plt.xticks([2013, 2020, 2025, 2030, 2035, 2040])
    plt.ylabel("Fraction outbreaks")
    plt.ylim(0, 1)
    plt.legend([r'$R_0 = $' + str(x) for x in R0s])
    plt.savefig(os.path.join(outputDir, "AllOutbreakOccurrences.png"))
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
    plt.ylabel("Final outbreak size after {} days".format(numDays))
    plt.ylim(0, 10000)
    plt.savefig(os.path.join(outputDir, figName))
    plt.clf()

def createFinalSizesOverviewPlot(outputDir, R0s, years, numDays, extinctionThreshold, poolSize):
    subplotCoors = [(1, 2, 1), (1, 2, 2)]
    for R0_i in range(len(R0s)):
        R0 = R0s[R0_i]
        allFinalSizes = []
        for year in years:
            scenarioName = str(year) + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, scenarioName, s, numDays) for s in seeds])
                finalSizes = [x for x in finalSizes if x >= extinctionThreshold]
                allFinalSizes.append(finalSizes)
        plt.subplot(subplotCoors[R0_i][0], subplotCoors[R0_i][1], subplotCoors[R0_i][2])
        plt.boxplot(allFinalSizes, labels=years)
        plt.ylim(0, 100000)
        plt.title(r'$R_0 = $' + str(R0))
        if R0_i == 0:
            plt.ylabel("Final outbreak sizes after {} days".format(numDays))
        else:
            plt.tick_params(
                axis='y',          # changes apply to the x-axis
                which='both',      # both major and minor ticks are affected
                left=False,      # ticks along the bottom edge are off
                right=False,         # ticks along the top edge are off
                labelleft=False) # labels along the bottom edge are off
    plt.tight_layout()
    plt.savefig(os.path.join(outputDir, "AllOutbreakSizes.png"))
    plt.clf()

import csv
import math
import matplotlib.pyplot as plt
import multiprocessing
import os
import statistics

from mpl_toolkits import mplot3d

from .Util import getFinalOutbreakSize, getRngSeeds, saveFig

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
        plt.errorbar(range(len(fractionOutbreaks)), fractionOutbreaks, SEs, fmt="o", ecolor="red", capsize=4)

        plt.ylabel("Fraction outbreaks")
        plt.ylim(0, 1)
        plt.xticks(range(len(fractionOutbreaks)), scenarioDisplayNames)
        saveFig(outputDir, figName)

# TODO make this a 3D plot too?
def createOutbreakOccurrenceOverviewPlot(outputDir, R0s, scenarioNames, scenarioDisplayNames, xLabel, numDays, extinctionThreshold, poolSize):
    fmts = ['o', 'v', '+', 'D', '.', '*', 'x']
    for R0_i in range(len(R0s)):
        R0 = R0s[R0_i]
        fractionOutbreaks = []
        numRuns = 0
        for scn in scenarioNames:
            scenarioName = str(scn) + "_R0_" + str(R0)
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
            plt.errorbar(range(len(scenarioNames)), fractionOutbreaks, SEs, fmt=fmts[R0_i], markersize=7, capsize=5)
            #TODO ecolor?
    xticks = range(len(scenarioNames))
    plt.xticks(xticks, scenarioDisplayNames)
    plt.xlim(xticks[0] - 1, xticks[-1] + 1)
    plt.xlabel(xLabel)
    plt.ylabel("Fraction outbreaks")
    plt.ylim(0, 1)
    plt.legend([r'$R_0 = $' + str(x) for x in R0s], loc=4, numpoints=1)
    saveFig(outputDir, "AllOutbreakOccurrences")

def createFinalSizesSideBySidePlot(outputDir, R0s, years, numDays, extinctionThreshold, poolSize):
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
        plt.xlabel("Calendar year")
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
    saveFig(outputDir, "AllOutbreakSizesSBS")

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
    #plt.ylim(0, 10000)
    saveFig(outputDir, figName)

def createFinalSizesOverviewPlots(outputDir, R0s, scenarioNames, scenarioDisplayNames,
    numDays, extinctionThreshold, poolSize, figName, stat="mean"):
    ax = plt.axes(projection="3d")
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    z = 0
    for R0 in R0s:
        allFinalSizes = []
        for scenario in scenarioNames:
            scenarioName = scenario + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                                            [(outputDir, scenarioName, s, numDays) for s in seeds])
                finalSizes = [f for f in finalSizes if f >= extinctionThreshold]
                if stat == "mean":
                    allFinalSizes.append(sum(finalSizes) / len(finalSizes))
                elif stat == "median":
                    allFinalSizes.append(statistics.median(finalSizes))
        ax.bar(range(len(scenarioNames)), allFinalSizes, zs=R0, zdir="y", color=colors[z], alpha=0.4)
        z += 1
    ax.set_xlabel("Scenario")
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(R0s)
    ax.set_zlabel("Final outbreak size ({})".format(stat))
    saveFig(outputDir, figName, "png")

def getSusceptiblesAtStart(outputDir, scenarioName, seed):
    totalSusceptible = 0
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            if int(row["susceptible"]):
                totalSusceptible += 1
    return totalSusceptible

def getEscapeProbability(outputDir, scenarioName, seed, numDays):
    totalInfected = getFinalOutbreakSize(outputDir, scenarioName, seed, numDays)
    totalSusceptible = getSusceptiblesAtStart(outputDir, scenarioName, seed)
    if totalSusceptible > 0:
        return (totalSusceptible - totalInfected) / totalSusceptible
    else:
        return 1 # If no-one is susceptible, escape probability is 1

def createEscapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames,
    numDays, extinctionThreshold, poolSize, figName):
    allEscapeProbabilities = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            escapeProbabilities = pool.starmap(getEscapeProbability,
                                        [(outputDir, scenario, s, numDays, extinctionThreshold) for s in seeds])
            allEscapeProbabilities.append([x for x in escapeProbabilities if x is not None])
    plt.boxplot(allEscapeProbabilities, labels=scenarioDisplayNames)
    plt.ylabel("Escape probability")
    plt.ylim(0, 1.05)
    saveFig(outputDir, figName)

def createEscapeProbabilityOverviewPlots(outputDir, R0s, scenarioNames, scenarioDisplayNames,
    numDays, poolSize, figName, stat="mean"):
    ax = plt.axes(projection="3d")

    z = 0
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for R0 in R0s:
        allEscapeProbabilities = []
        for scenario in scenarioNames:
            scenarioName = scenario + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                escapeProbabilities = pool.starmap(getEscapeProbability,
                                            [(outputDir, scenarioName, s, numDays) for s in seeds])
                if stat == "mean":
                    allEscapeProbabilities.append(sum(escapeProbabilities) / len(escapeProbabilities))
                elif stat == "median":
                    allEscapeProbabilities.append(statistics.median(escapeProbabilities))
        plt.bar(range(len(scenarioNames)), allEscapeProbabilities, zs=R0, zdir="y", color=colors[z], alpha=0.4)
        z += 1
    ax.set_xlabel("Scenario")
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(R0s)
    ax.set_zlabel("Escape probability ({})".format(stat))
    ax.set_zlim(0, 1)
    saveFig(outputDir, figName, "png")

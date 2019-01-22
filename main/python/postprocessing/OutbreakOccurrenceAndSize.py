import csv
import math
import matplotlib.pyplot as plt
import multiprocessing
import os

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
    plt.legend([r'$R_0 = $' + str(x) for x in R0s], loc=4)
    saveFig(outputDir, "AllOutbreakOccurrences")

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
    saveFig(outputDir, "AllOutbreakSizes")

'''
import argparse
import csv
import matplotlib.pyplot as plt
import os

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "Seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep == numDays - 1:
                return int(row["cases"])

def getSusceptiblesAtStart(outputDir, scenario, seed):
    totalSusceptibles = 0
    susceptiblesFile = os.path.join(outputDir, scenario + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            isSusceptible = int(row["susceptible"])
            if isSusceptible == 1:
                totalSusceptibles += 1
    return totalSusceptibles

def getEscapeProbability(outputDir, scenarioName, seed, finalOutbreakSize):
    totalSusceptibles = getSusceptiblesAtStart(outputDir, scenarioName, seed)
    escapeProbability = (totalSusceptibles - finalOutbreakSize) / totalSusceptibles
    return escapeProbability

def escapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    allEscapeProbabilities = []
    for scenario in scenarioNames:
        print(scenario)
        seeds = getRngSeeds(outputDir, scenario)
        scenarioEscapeProbabilities = []
        for s in seeds:
            print("        " + scenario + str(s))
            finalOutbreakSize = getFinalOutbreakSize(outputDir, scenario, s, numDays)
            if finalOutbreakSize > extinctionThreshold:
                scenarioEscapeProbabilities.append(getEscapeProbability(outputDir, scenario, s, finalOutbreakSize))
        allEscapeProbabilities.append(scenarioEscapeProbabilities)
    plt.boxplot(allEscapeProbabilities, labels=scenarioDisplayNames)
    plt.ylabel("Escape probability")
    plt.ylim(0, 1.05)
    plt.savefig(os.path.join(outputDir, "EscapeProbabilityPlot.png"))
    plt.clf()

def main(outputDir, numDays, extinctionThreshold):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform\nimmunity rates\n+ no clustering",
                            "Age-dependent\nimmunity rates\n+ no clustering",
                            "Uniform\nimmunity rates\n+ clustering",
                            "Age-dependent\nimmunity rates +\nclustering"]
    escapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Extinction threshold.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays, args.extinctionThreshold)

'''

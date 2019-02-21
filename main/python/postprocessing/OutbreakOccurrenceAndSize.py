import csv
import math
import matplotlib.pyplot as plt
import multiprocessing
import os

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
def createOutbreakOccurrenceOverviewPlot(outputDir, R0s, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold, poolSize):
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
    plt.xticks(range(len(scenarioNames)), scenarioDisplayNames)
    plt.xlabel("Calendar year")
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
    #plt.savefig(os.path.join(outputDir, "AllOutbreakSizes.png"))
    #plt.clf()
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

def createFinalSizesOverviewPlots(outputDir, R0s, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold, poolSize):
    xs_surf = [] # x-axis = scenario
    xs_scat = []
    ys_surf = [] # y-axis = R0
    ys_scat = []
    zs_surf = [] # z-axis = mean final size when no extinction occurs
    zs_scat = []
    for R0 in R0s:
        for s_i in range(len(scenarioNames)):
            scenarioName = str(scenarioNames[s_i]) + "_R0_" + str(R0)
            xs_surf.append(s_i)
            ys_surf.append(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                                            [(outputDir, scenarioName, s, numDays) for s in seeds])
                finalSizes = [f for f in finalSizes if f >= extinctionThreshold]
                for f in finalSizes:
                    xs_scat.append(s_i)
                    ys_scat.append(R0)
                    zs_scat.append(f)
                if len(finalSizes) > 0:
                    zs_surf.append(sum(finalSizes) / len(finalSizes))
                else:
                    zs_surf.append(0)
    # Create 3D surface plot
    ax = plt.axes(projection="3d")
    ax.plot_trisurf(xs_surf, ys_surf, zs_surf)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_yticks(R0s)
    saveFig(outputDir, "AllOutbreakSizesSurface")
    # Create 3D scatter plot
    ax = plt.axes(projection="3d")
    ax.scatter(xs_scat, ys_scat, zs_scat)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_yticks(R0s)
    saveFig(outputDir, "AllOutbreakSizesScatter")

def getSusceptiblesAtStart(outputDir, scenarioName, seed):
    totalSusceptible = 0
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            if int(row["susceptible"]):
                totalSusceptible += 1
    return totalSusceptible

def getEscapeProbability(outputDir, scenarioName, seed, numDays, extinctionThreshold):
    totalInfected = getFinalOutbreakSize(outputDir, scenarioName, seed, numDays)
    if totalInfected >= extinctionThreshold:
        totalSusceptible = getSusceptiblesAtStart(outputDir, scenarioName, seed)
        if totalSusceptible > 0:
            return (totalSusceptible - totalInfected) / totalSusceptible
        else:
            return 1 # If no-one is susceptible, escape probability is 1
    else:
        return None

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
    numDays, extinctionThreshold, poolSize):
    xs_surf = [] # x-axis = scenario
    xs_scat = []
    ys_surf = [] # y-axis = R0
    ys_scat = []
    zs_surf = [] # z-axis = escape probability
    zs_scat = []
    for R0 in R0s:
        for s_i in range(len(scenarioNames)):
            scenarioName = scenarioNames[s_i] + "_R0_" + str(R0)
            xs_surf.append(s_i)
            ys_surf.append(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                escapeProbabilities = pool.starmap(getEscapeProbability,
                                                    [(outputDir, scenarioName, s,
                                                    numDays, extinctionThreshold)
                                                    for s in seeds])
                escapeProbabilities = [e for e in escapeProbabilities if e is not None]
                for e in escapeProbabilities:
                    xs_scat.append(s_i)
                    ys_scat.append(R0)
                    zs_scat.append(e)
                if len(escapeProbabilities) > 0:
                    zs_surf.append(sum(escapeProbabilities) / len(escapeProbabilities))
                else:
                    xs_surf.pop()
                    ys_surf.pop()
    # Create 3D surface plot
    ax = plt.axes(projection="3d")
    ax.plot_trisurf(xs_surf, ys_surf, zs_surf)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_yticks(R0s)
    ax.set_zlim(0, 1)
    saveFig(outputDir, "AllEscapeProbabilitiesSurface")
    # Create 3D scatter plot
    ax = plt.axes(projection="3d")
    ax.scatter(xs_scat, ys_scat, zs_scat)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_yticks(R0s)
    ax.set_zlim(0, 1)
    saveFig(outputDir, "AllEscapeProbabilitiesScatter")

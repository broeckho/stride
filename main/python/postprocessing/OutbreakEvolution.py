import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getRngSeeds, saveFig

def getNewCasesPerDay(outputDir, scenarioName, seed, numDays):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    transmissions = {}
    for i in range(numDays):
        transmissions[i] = 0
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                simDay = int(line[6])
                if simDay < numDays:
                    transmissions[simDay] += 1
    return transmissions


def getCumulativeCasesPerDay(outputDir, scenarioName, seed, numDays):
    newCasesPerDay = getNewCasesPerDay(outputDir, scenarioName, seed, numDays)
    cumulativeCasesPerDay = []
    previousDay = 0
    for day in newCasesPerDay:
        cumulative = previousDay + newCasesPerDay[day]
        previousDay = cumulative
        cumulativeCasesPerDay.append(cumulative)
    return cumulativeCasesPerDay

def createNewCasesPerDayPlot(outputDir, scenarioName, transmissionProbability, clusteringLevel, numDays, poolSize):
    fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(transmissionProbability)
    seeds = getRngSeeds(outputDir, fullScenarioName)

    allNewCases = []
    with multiprocessing.Pool(processes=poolSize) as pool:
        newCasesPerDay = pool.starmap(getNewCasesPerDay,
                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
        # TODO extinction threshold?
        for day in range(numDays)[::5]:
            allNewCases.append([run[day] for run in newCasesPerDay])
    plt.boxplot(allNewCases, labels=range(numDays)[::5])
    plt.xlabel("Simulation day")
    plt.ylabel("New cases")
    plt.ylim(0, max(max(max(x) for x in allNewCases), 25))
    saveFig(outputDir, "NewCasesPerDay_" + scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(transmissionProbability))

def createCumulativeCasesPerDayPlot(outputDir, scenarioName, transmissionProbability, clusteringLevel, numDays, poolSize):
    fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(transmissionProbability)
    seeds = getRngSeeds(outputDir, fullScenarioName)

    allCumulativeCases = []
    with multiprocessing.Pool(processes=poolSize) as pool:
        cumulativeCasesPerDay = pool.starmap(getCumulativeCasesPerDay,
                                        [(outputDir, fullScenarioName, s, numDays) for s in seeds])
        # TODO extinction threshold?
        for day in range(numDays)[::5]:
            allCumulativeCases.append([run[day] for run in cumulativeCasesPerDay])
    plt.boxplot(allCumulativeCases, labels=range(numDays)[::5])
    plt.xlabel("Simulation day")
    plt.ylabel("Cumulative cases")
    plt.ylim(0, max(max(max(x) for x in allCumulativeCases), 250))
    saveFig(outputDir, "CumulativeCasesPerDay_" + scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(transmissionProbability))

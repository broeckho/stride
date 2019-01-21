import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getRngSeeds, saveFig

def getCumulativeCasesPerDay(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    cumulativeCases = []
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep < numDays:
                cumulativeCases.append(int(row["cases"]))
        return cumulativeCases

def getNewCasesPerDay(outputDir, scenarioName, seed, numDays, extinctionThreshold):
    cumulativeCases = getCumulativeCasesPerDay(outputDir, scenarioName, seed, numDays)
    if cumulativeCases[-1] >= extinctionThreshold:
        newCasesPerDay = []
        lastDay = 1
        for today in cumulativeCases:
            newCasesPerDay.append(today - lastDay)
            lastDay = today
        return newCasesPerDay
    else:
        return None

def createCumulativeCasesPerDayPlot(outputDir, scenarioName, numDays, extinctionThreshold, poolSize, figName):
    dayScale = int(numDays / 20)
    days = range(numDays)[::dayScale]
    allCumulativeCases = []
    for i in range(numDays):
        allCumulativeCases.append([])
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        cumulativeCasesPerDay = pool.starmap(getCumulativeCasesPerDay,
                                    [(outputDir, scenarioName, s, numDays) for s in seeds])
        for run in cumulativeCasesPerDay:
            if run[-1] >= extinctionThreshold:
                for d in range(numDays):
                    allCumulativeCases[d].append(run[d])
    plt.boxplot(allCumulativeCases[::dayScale], labels=days)
    plt.xlabel("Day")
    plt.xticks(rotation=90)
    plt.ylabel("Cumulative cases")
    saveFig(outputDir, figName)

def createNewCasesPerDayPlot(outputDir, scenarioName, numDays, extinctionThreshold, poolSize, figName):
    dayScale = int(numDays / 20)
    days = range(numDays)[::dayScale]
    allNewCases = []
    for i in range(numDays):
        allNewCases.append([])
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        newCasesPerDay = pool.starmap(getNewCasesPerDay,
                                [(outputDir, scenarioName, s, numDays, extinctionThreshold) for s in seeds])
        for run in newCasesPerDay:
            if run is not None:
                for d in range(numDays):
                    allNewCases[d].append(run[d])
    plt.boxplot(allNewCases[::dayScale], labels=days)
    plt.xticks(rotation=90)
    plt.xlabel("Day")
    plt.ylabel("New cases")
    saveFig(outputDir, figName)

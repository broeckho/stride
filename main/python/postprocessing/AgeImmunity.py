import csv
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os

from .Util import COLORS, MAX_AGE
from .Util import getRngSeeds, saveFig

def getSusceptibilityLevels(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles_by_age.csv")
    ages = {}
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(row["age"])
            numImmune = int(row["immune"])
            numSusceptible = int(row["susceptible"])
            totalOfAge = numImmune + numSusceptible
            fractionSusceptible = numSusceptible / totalOfAge
            ages[age] = fractionSusceptible
    return ages

def getTotalPercentageSusceptible(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles_by_age.csv")
    totalPersons = 0
    totalSusceptible = 0
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            numImmune = int(row["immune"])
            numSusceptible = int(row["susceptible"])
            totalOfAge = numImmune + numSusceptible
            totalPersons += totalOfAge
            totalSusceptible += numSusceptible
    totalPercentageSusceptible = (totalSusceptible / totalPersons) * 100
    return totalPercentageSusceptible

def getProjectedSusceptibilityLevels(dataDir, year):
    maxDataAge = 85
    numMunicipalities = 500
    yearDir = os.path.join(dataDir, "BootstrapSusceptibility" + str(year))

    ages = {}
    for age in range(maxDataAge + 1):
        ages[age] = []
    for i in range(1, numMunicipalities + 1):
        immunityFile = os.path.join(yearDir, "bootstrap.susceptibility" + str(year) + ".municipality" + str(i) + ".txt")
        with open(immunityFile) as f:
            for line in f:
                line = line.split(" ")
                for age in range(len(line)):
                    ages[age].append(float(line[age]))
    lower = []
    upper = []
    for age in ages:
        lower.append(numpy.percentile(ages[age], 2.5))
        upper.append(numpy.percentile(ages[age], 97.5))
    for age in range(maxDataAge + 1, MAX_AGE + 1):
        lower.append(lower[-1])
        upper.append(upper[-1])
    return (lower, upper)

def createAgeImmunityOverviewPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, poolSize, targetLevelsDir=None, targetLevelsYear=2020):
    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    # Get target levels if needed
    if targetLevelsDir is not None:
        upper, lower = getProjectedSusceptibilityLevels(targetLevelsDir, targetLevelsYear)
        plt.fill_between(range(MAX_AGE + 1), lower, upper, color="lightgrey")
    for level_i in range(len(clusteringLevels)):
        susceptibilityLevels = []
        for prob in transmissionProbabilities:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                susceptibilityLevels += pool.starmap(getSusceptibilityLevels,
                                            [(outputDir, fullScenarioName, s) for s in seeds])
        meansByAge = []
        for age in range(MAX_AGE + 1):
            levelsForAge = [run[age] for run in susceptibilityLevels]
            meansByAge.append(sum(levelsForAge) / len(levelsForAge))
        if dashes[level_i] is not None:
            plt.plot(range(MAX_AGE + 1), meansByAge, color=COLORS[level_i],
                        dashes=dashes[level_i], linestyle=linestyles[level_i])
        else:
            plt.plot(range(MAX_AGE + 1), meansByAge, color=COLORS[level_i],
                        linestyle=linestyles[level_i])
    plt.xlabel("Age (in years)")
    plt.xlim(0, 100)
    plt.ylabel("Fraction susceptible (mean)")
    plt.ylim(0, 1)
    plt.legend(["Clustering = {}".format(c) for c in clusteringLevels] + ["Projection"])
    saveFig(outputDir, "AgeImmunity_" + scenarioName)

def getMeanPercentageOfSusceptible(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    allPercentages = []
    for level in clusteringLevels:
        for prob in transmissionProbabilities:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                totalPercentageSusceptible = pool.starmap(getTotalPercentageSusceptible,
                                                        [(outputDir, fullScenarioName, s) for s in seeds])
                allPercentages += totalPercentageSusceptible
    meanPercentage = sum(allPercentages) / len(allPercentages)
    print(meanPercentage)

def createAgeImmunityBoxplot(outputDir, scenarioName, transmissionProbabilities, clusteringLevel, poolSize):
    susceptibilityLevels = []
    for prob in transmissionProbabilities:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            susceptibilityLevels += pool.starmap(getSusceptibilityLevels,
                                        [(outputDir, fullScenarioName, s) for s in seeds])
    levelsByAge = []
    for age in range(MAX_AGE + 1):
        levelsByAge.append([run[age] for run in susceptibilityLevels])
    plt.boxplot(levelsByAge)
    plt.xlabel("Age (in years)")
    plt.xlim(0, 100)
    plt.xticks(range(0, 101, 20), range(0, 101, 20))
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    saveFig(outputDir, "AgeImmunity_" + scenarioName + "_CLUSTERING_" + str(clusteringLevel))

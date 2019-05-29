import csv
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os

from .Util import MAX_AGE, getRngSeeds, saveFig

def getSusceptibilityLevels(outputDir, scenarioName, seed):
    totalsByAge = [0]  * (MAX_AGE + 1)
    susceptiblesByAge = [0] * (MAX_AGE + 1)
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row["age"]))
            totalsByAge[age] += 1
            if int(row["susceptible"]):
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

def getProjectedSusceptibilityLevels(dataDir):
    maxDataAge = 85
    numMunicipalities = 500
    year = 2020
    ages = {}
    for age in range(maxDataAge + 1):
        ages[age] = []
    yearDir = os.path.join(dataDir, "BootstrapSusceptibility" + str(year))
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
    clusteringLevels, poolSize, targetRatesDir=None):
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    # Get target rates if needed
    if targetRatesDir is not None:
        upper, lower = getProjectedSusceptibilityLevels(targetRatesDir)
        plt.fill_between(range(MAX_AGE + 1), lower, upper)
    i = 0
    for level in clusteringLevels:
        allAges = {}
        for age in range(MAX_AGE + 1):
            allAges[age] = []
        for prob in transmissionProbabilities:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                susceptibilityLevels = pool.starmap(getSusceptibilityLevels,
                                        [(outputDir, fullScenarioName, s) for s in seeds])
                for age in range(MAX_AGE + 1):
                    levelsForAge = [run[age] for run in susceptibilityLevels]
                    allAges[age] += levelsForAge
        linestyle = linestyles[i % len(linestyles)]
        dash = dashes[i % len(dashes)]
        if dash is not None:
            plt.plot(range(MAX_AGE + 1), [sum(allAges[age]) / len(allAges[age]) for age in range(MAX_AGE + 1)],
                    color=colors[i], linestyle=linestyle, dashes=dash)
        else:
            plt.plot(range(MAX_AGE + 1), [sum(allAges[age]) / len(allAges[age]) for age in range(MAX_AGE + 1)],
                    color=colors[i], linestyle=linestyle)
        i += 1
    plt.xlabel("Age (in years)")
    plt.xlim(0, 100)
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    plt.legend(clusteringLevels + ["Projection"])
    saveFig(outputDir, "AgeImmunity")

def createAgeImmunityPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevel, poolSize):
    allAges = {}
    for age in range(MAX_AGE + 1):
        allAges[age] = []
    for prob in transmissionProbabilities:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            susceptibilityLevels = pool.starmap(getSusceptibilityLevels,
                                                [(outputDir, fullScenarioName, s) for s in seeds])
            for age in range(MAX_AGE + 1):
                levelsForAge = [run[age] for run in susceptibilityLevels]
                allAges[age] += levelsForAge
    plt.boxplot([allAges[age] for age in range(MAX_AGE + 1)], labels=range(MAX_AGE + 1))
    plt.xlabel("Age (in years)")
    plt.xlim(0, 100)
    plt.xticks(range(0, 101, 20), range(0, 101, 20))
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    saveFig(outputDir, "AgeImmunity_" + scenarioName + "_CLUSTERING_" + str(clusteringLevel))

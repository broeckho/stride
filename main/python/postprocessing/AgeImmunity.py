import csv
import matplotlib.pyplot as plt
import multiprocessing
import os
import xml.etree.ElementTree as ET

from .Util import getRngSeeds, saveFig, MAX_AGE


def getTargetRates(outputDir, targetRatesFile):
    targetRatesTree = ET.parse(os.path.join(outputDir, 'data', targetRatesFile))
    targetRates = []
    for r in targetRatesTree.iter():
        if r.tag not in ["immunity", "data_source", "data_manipulation"]:
            targetRates.append(float(r.text))
    # 1 - immunityRate = susceptibilityRate
    return [1 - x for x in targetRates]

def getAgeSusceptibilityLevels(outputDir, scenarioName, transmissionProbability, clusteringLevel, seed):
    totalsByAge = [0] * (MAX_AGE + 1)
    susceptiblesByAge = [0] * (MAX_AGE + 1)
    susceptiblesFile = os.path.join(outputDir,
                                    scenarioName + "_CLUSTERING_"
                                    + str(clusteringLevel) + "_TP_" + str(transmissionProbability)
                                    + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row["age"]))
            totalsByAge[age] += 1
            if int(row["susceptible"]):
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

def createAgeImmunityOverviewPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels, poolSize, targetRatesFile=None):
    legend = []
    # TODO add target rates to plot
    if targetRatesFile is not None:
        targetRates = getTargetRates(outputDir, targetRatesFile)
        plt.plot(range(MAX_AGE + 1), targetRates, "bo")
        legend.append("Data")
    #linestyles = ['-', '--', '-.', ':', '--', '--']
    #dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan', 'magenta', 'blue', 'yellow']
    color_i = 0
    for scenario in scenarioNames:
        for level in clusteringLevels:
            legend.append(scenario + ", clustering " + str(level))
            allAges = {}
            for age in range(MAX_AGE + 1):
                allAges[age] = []
            for prob in transmissionProbabilities:
                seeds = getRngSeeds(outputDir, scenario + "_CLUSTERING_" + str(level) + "_TP_" + str(prob))
                with multiprocessing.Pool(processes=poolSize) as pool:
                    susceptibilityLevels = pool.starmap(getAgeSusceptibilityLevels,
                                                        [(outputDir, scenario, prob, level, s) for s in seeds])
                    for age in range(MAX_AGE + 1):
                        allLevelsForAge = [run[age] for run in susceptibilityLevels]
                        allAges[age] += allLevelsForAge
            plt.plot(range(MAX_AGE + 1), [sum(allAges[age]) / len(allAges[age]) for age in range(MAX_AGE + 1)],
                        color=colors[color_i])
            color_i += 1
    plt.xlabel("Age (years)")
    plt.xlim(0, MAX_AGE + 1)
    plt.ylabel("Fraction susceptible (mean)")
    plt.ylim(0, 1)
    plt.legend(legend)
    saveFig(outputDir, "AgeImmunity")

def createAgeImmunityPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevel, poolSize):
    allAges = {}
    for age in range(MAX_AGE + 1):
        allAges[age] = []
    for prob in transmissionProbabilities:
        seeds = getRngSeeds(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob))
        with multiprocessing.Pool(processes=poolSize) as pool:
            susceptibilityLevels = pool.starmap(getAgeSusceptibilityLevels,
                                                    [(outputDir, scenarioName, prob, clusteringLevel, s) for s in seeds])
            for age in range(MAX_AGE + 1):
                allLevelsForAge = [run[age] for run in susceptibilityLevels]
                allAges[age] += allLevelsForAge
    plt.boxplot(allAges.values(), labels=allAges.keys())
    plt.xlabel("Age (years)")
    plt.xticks(range(0, 101, 20), range(0, 101, 20))
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    saveFig(outputDir, "AgeImmunity_" + scenarioName + "_C_" + str(clusteringLevel))

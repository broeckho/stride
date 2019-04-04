import csv
import matplotlib.pyplot as plt
import multiprocessing
import os
import statistics
import xml.etree.ElementTree as ET

from .Util import getRngSeeds, saveFig, MAX_AGE

def getInfectedByAge(outputDir, scenarioName, seed):
    infectedFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "infected.csv")
    infectedByAge = [0] * (MAX_AGE + 1)
    with open(infectedFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            if (int(row["infected"])):
                age = int(float(row["age"]))
                infectedByAge[age] += 1
    return infectedByAge

def getTotalsByAge(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    totalsByAge = [0] * (MAX_AGE + 1)
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row["age"]))
            totalsByAge[age] += 1
    return totalsByAge

def createInfectedByAgePlot(outputDir, scenarioName, extinctionThreshold, poolSize, figName):
    seeds = getRngSeeds(outputDir, scenarioName)
    allInfectedByAge = []
    with multiprocessing.Pool(processes=poolSize) as pool:
        infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, scenarioName, s) for s in seeds])
        infectedByAge = [run for run in infectedByAge if sum(run) >= extinctionThreshold]
        for i in range(MAX_AGE + 1):
            allInfectedByAge.append([run[i] for run in infectedByAge])
    plt.boxplot(allInfectedByAge)
    plt.ylabel("Number of infected individuals")
    plt.xlabel("Age")
    plt.xticks(range(MAX_AGE + 2)[::5], range(MAX_AGE + 2)[::5], rotation=90)
    saveFig(outputDir, figName)

def createInfectedByAgeOverviewPlot(outputDir, scenarioNames, scenarioDisplayNames,
    extinctionThreshold, poolSize, figName, stat="mean"):

    allResults = []
    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, scenario, s) for s in seeds])
            infectedByAge = [run for run in infectedByAge if sum(run) >= extinctionThreshold]
            results = []
            for i in range(MAX_AGE + 1):
                age = [run[i] for run in infectedByAge]
                if stat == "mean":
                    if len(age) > 0:
                        results.append(sum(age) / len(age))
                    else:
                        results.append(0)
                elif stat == "median":
                    results.append(statistics.median(age))
                else:
                    print("No valid statistic supplied!")
            allResults.append(results)
    for r_i in range(len(allResults)):
        if dashes[r_i] is not None:
            plt.plot(allResults[r_i], linestyle=linestyles[r_i], dashes=dashes[r_i], color=colors[r_i])
        else:
            plt.plot(allResults[r_i], linestyle=linestyles[r_i], color=colors[r_i])
    plt.xlabel("Age")
    plt.ylabel("Number of infected ({})".format(stat))
    plt.legend(scenarioDisplayNames)
    saveFig(outputDir, figName)

def createInfectedFractionByAgeOverviewPlot(outputDir, scenarioNames, scenarioDisplayNames,
    extinctionThreshold, poolSize, figName, stat="mean"):
    """
        Create plot displaying the fraction of individuals
        per age category that were infected
        (num infected of age x / num persons of age).
    """
    allResults = []
    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, scenario, s) for s in seeds])
            totalsByAge = pool.starmap(getTotalsByAge, [(outputDir, scenario, s) for s in seeds])
            # Remove runs where total number of infected is below extinction threshold
            extIds = [i for i in range(len(infectedByAge)) if sum(infectedByAge[i]) < extinctionThreshold]
            infectedByAge = [infectedByAge[i] for i in range(len(infectedByAge)) if i not in extIds]
            totalsByAge = [totalsByAge[i] for i in range(len(infectedByAge)) if i not in extIds]
            results = []
            for a_i in range(MAX_AGE + 1):
                age = []
                for r_i in range(len(infectedByAge)):
                    age.append(infectedByAge[r_i][a_i] / totalsByAge[r_i][a_i])
                if stat == "mean":
                    if len(age) > 0:
                        results.append(sum(age) / len(age))
                    else:
                        results.append(0)
                elif stat == "median":
                    results.append(statistics.median(age))
                else:
                    print("No valid statistic supplied!")
            allResults.append(results)
    for r_i in range(len(allResults)):
        if dashes[r_i] is not None:
            plt.plot(allResults[r_i], linestyle=linestyles[r_i], dashes=dashes[r_i], color=colors[r_i])
        else:
            plt.plot(allResults[r_i], linestyle=linestyles[r_i], color=colors[r_i])
    plt.xlabel("Age")
    plt.ylabel("Fraction of age category infected ({})".format(stat))
    plt.ylim(0, 1)
    plt.legend(scenarioDisplayNames)
    saveFig(outputDir, figName)

def createFractionOfInfectedByAgeOverviewPlot(outputDir, scenarioNames, scenarioDisplayNames,
    extinctionThreshold, poolSize, figName, stat="mean"):
    """
        Create plot displaying the fraction of infected
        individuals of the total number of infected individuals
        there are per age category
        (num infected of age x / total num infected).
    """
    results = []
    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        scenarioResults = []
        with multiprocessing.Pool(processes=poolSize) as pool:
            infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, scenario, s) for s in seeds])
            infectedByAge = [run for run in infectedByAge if sum(run) >= extinctionThreshold]
            for r_i in range(len(infectedByAge)):
                if sum(infectedByAge[r_i]) > 0:
                    infectedByAge[r_i] = [a / sum(infectedByAge[r_i]) for a in infectedByAge[r_i]]
                else:
                    infectedByAge[r_i] = [0 for a in infectedByAge[r_i]]
            for a_i in range(MAX_AGE):
                age = []
                for run in infectedByAge:
                    age.append(run[a_i])
                if stat == "mean":
                    if len(age) > 0:
                        scenarioResults.append(sum(age) / len(age))
                    else:
                        scenarioResults.append(0)
                elif stat == "median":
                    scenarioResults.append(statistics.median(age))
                else:
                    print("No valid statistic given!")
            results.append(scenarioResults)
    for r_i in range(len(results)):
        if dashes[r_i] is not None:
            plt.plot(results[r_i], linestyle=linestyles[r_i], dashes=dashes[r_i], color=colors[r_i])
        else:
            plt.plot(results[r_i], linestyle=linestyles[r_i], color=colors[r_i])
    plt.xlabel("Age")
    plt.ylim(0, 1)
    plt.ylabel("Fraction of total infections ({})".format(stat))
    plt.legend(scenarioDisplayNames)
    saveFig(outputDir, figName)

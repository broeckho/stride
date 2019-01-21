import csv
import matplotlib.pyplot as plt
import multiprocessing
import os
import xml.etree.ElementTree as ET

from .Util import getRngSeeds, saveFig, MAX_AGE

"""
def createHouseholdConstitutionPlots(outputDir, scenarioNames):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        for s in seeds[:1]:
            households = {}
            susceptiblesFile = os.path.join(outputDir, scenario + str(s), 'susceptibles.csv')
            with open(susceptiblesFile) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    hhId = int(row['household'])
                    isSusceptible = int(row['susceptible'])
                    if hhId in households:
                        households[hhId].append(isSusceptible)
                    else:
                        households[hhId] = [isSusceptible]
            householdImmunities = []
            for hhId, members in households.items():
                pctImmune = 1 - (sum(members) / len(members))
                householdImmunities.append(pctImmune)
            plt.hist(householdImmunities)
            plt.title(scenario)
            plt.show()
"""

def getTargetRates(outputDir, targetRatesFile):
    targetRatesTree = ET.parse(os.path.join(outputDir, 'data', targetRatesFile))
    targetRates = []
    for r in targetRatesTree.iter():
        if r.tag not in ["immunity", "data_source", "data_manipulation"]:
            targetRates.append(float(r.text))
    # 1 - immunityRate = susceptibilityRate
    return [1 - x for x in targetRates]

def getAgeSusceptibilityRates(outputDir, scenarioName, seed):
    totalsByAge = [0] * (MAX_AGE + 1)
    susceptiblesByAge = [0] * (MAX_AGE + 1)
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row["age"]))
            totalsByAge[age] += 1
            if (int(row["susceptible"])):
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

def createAgeImmunityPlots(outputDir, scenarioNames, scenarioDisplayNames,
    poolSize, figName="AgeImmunityPlot.png", targetRatesFile=None):
    if targetRatesFile is not None:
        targetRates = getTargetRates(outputDir, targetRatesFile)
        plt.plot(targetRates, 'bo')
        scenarioDisplayNames = ["Data"] + scenarioDisplayNames

    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for scenario_i in range(len(scenarioNames)):
        scenario = scenarioNames[scenario_i]
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            ageSusceptibilityRates = pool.starmap(getAgeSusceptibilityRates, [(outputDir, scenario, s) for s in seeds])
            avgRates = []
            for i in range(MAX_AGE + 1):
                avgRates.append(sum([rates[i] for rates in ageSusceptibilityRates]) / len(ageSusceptibilityRates))
            if dashes[scenario_i] is not None:
                plt.plot(range(MAX_AGE + 1), avgRates, linestyle=linestyles[scenario_i], dashes=dashes[scenario_i], color=colors[scenario_i])
            else:
                plt.plot(range(MAX_AGE + 1), avgRates, linestyle=linestyles[scenario_i], color=colors[scenario_i])
    plt.xlabel("Age")
    plt.xlim(0, MAX_AGE + 1)
    plt.ylabel("Fraction susceptibles")
    plt.ylim(0, 1)
    plt.legend(scenarioDisplayNames)
    saveFig(outputDir, figName)

def getOverallImmunityRate(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    totalPersons = 0
    totalSusceptibles = 0
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalPersons += 1
            if (int(row["susceptible"])):
                totalSusceptibles += 1
    return 1 - (totalSusceptibles / totalPersons)

def getAvgOverallImmunityRate(outputDir, scenarioName, poolSize):
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        immunityRates = pool.starmap(getOverallImmunityRate,
                                    [(outputDir, scenarioName, s) for s in seeds])
        return (sum(immunityRates) / len(immunityRates))

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

def createInfectedByAgePlot(outputDir, scenarioName, poolSize, figName):
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, scenarioName, s) for s in seeds])
        allInfectedByAge = []
        for i in range(MAX_AGE + 1):
            age = []
            for run in infectedByAge:
                age.append(run[i])
            allInfectedByAge.append(age)
        plt.boxplot(allInfectedByAge)
        plt.ylabel("Number of infected individuals")
        plt.xlabel("Age")
        plt.xticks(range(MAX_AGE+2)[::5], range(MAX_AGE+2)[::5],rotation=90)
        saveFig(outputDir, figName)

def createInfectedByAgeOverviewPlot(outputDir, scenarioNames, scenarioDisplayNames, poolSize, figName, extinctionThreshold):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            infectedByAge = pool.starmap(getInfectedByAge, [(outputDir, scenario, s) for s in seeds])
            # Remove runs where extinction occurs
            infectedByAgeNoExt = []
            for run in infectedByAge:
                if sum(run) > extinctionThreshold:
                    infectedByAgeNoExt.append(run)
            allInfectedByAge = []
            for i in range(MAX_AGE + 1):
                age = []
                for run in infectedByAgeNoExt:
                    age.append(run[i])
                if sum(age) > 0:
                    allInfectedByAge.append(sum(age) / len(age))
                else:
                    allInfectedByAge.append(0)
        plt.plot(allInfectedByAge)
    plt.legend(scenarioDisplayNames)
    plt.xlabel("Age")
    plt.ylabel("Mean number of infected")
    saveFig(outputDir, figName)

def createInfectedPctByAgePlot(outputDir, scenarioName, poolSize):
    pass

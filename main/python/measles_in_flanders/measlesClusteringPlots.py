import argparse
import csv
import math
import matplotlib.pyplot as plt
import numpy as np
import os
import xml.etree.ElementTree as ET

from collections import OrderedDict

# Calculate mean, and 95% confidence interval of list of samples
def summary(l):
    mean = sum(l) / len(l)
    list.sort(l)
    minIndex = math.floor((len(l) / 100) * 5)
    maxIndex = math.floor((len(l) / 100) * 95)
    min95Conf = l[minIndex]
    max95Conf = l[maxIndex]
    return {'mean': mean, 'min_95_conf': min95Conf, 'max_95_conf': max95Conf}

def getSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + 'Seeds.csv')
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        seeds = [int(x) for x in next(reader)]
    return seeds

def getTargetSusceptibilityRates(outputDir):
    targetRatesChildTree = ET.parse(os.path.join(outputDir, 'data', 'measles_child_immunity.xml'))
    targetRatesChild = []
    for r in targetRatesChildTree.iter():
        if r.tag not in ['immunity', 'data_source', 'data_manipulation']:
            targetRatesChild.append(float(r.text))
    targetRatesAdultTree = ET.parse(os.path.join(outputDir, 'data', 'measles_adult_immunity.xml'))
    targetRatesAdult = []
    for r in targetRatesAdultTree.iter():
        if r.tag not in ['immunity', 'data_source', 'data_manipulation']:
            targetRatesAdult.append(float(r.text))
    # 1 - immunityRate = susceptibilityRate
    return [1 - (x + y) for x, y in zip(targetRatesChild, targetRatesAdult)]

def getActualSusceptibilityRates(outputDir, scenarioName, seed):
    maxAge = 99
    susceptiblesFile = os.path.join(outputDir, scenarioName + str(seed), 'susceptibles.csv')
    totalsByAge = {}
    susceptiblesByAge = {}
    for age in range(maxAge + 1):
        totalsByAge[age] = 0
        susceptiblesByAge[age] = 0
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row['age']))
            isSusceptible = int(float(row['susceptible']))
            totalsByAge[age] += 1
            if isSusceptible:
                susceptiblesByAge[age] += 1
    susceptibilityRates = []
    for age in range(maxAge + 1):
        susceptibilityRates.append(susceptiblesByAge[age] / totalsByAge[age])
    return susceptibilityRates

def ageImmunityPlots(outputDir, scenarioNames):
    maxAge = 99
    ages = range(maxAge + 1)
    # Plot target susceptibility rates
    targetSusceptibilityRates = getTargetSusceptibilityRates(outputDir)
    plt.plot(ages, targetSusceptibilityRates, 'bo')
    # Plot actual susceptibility rates per scenario
    for scenario in scenarioNames:
        seeds = getSeeds(outputDir, scenario)
        totalSuscpetibilityRates = []
        for i in range(maxAge + 1):
            totalSuscpetibilityRates.append([])
        for s in seeds:
            actualSusceptibilityRates = getActualSusceptibilityRates(outputDir, scenario, s)
            for age in range(maxAge + 1):
                totalSuscpetibilityRates[age].append(actualSusceptibilityRates[age])
        means = []
        for age in range(maxAge + 1):
            statSum = summary(totalSuscpetibilityRates[age])
            means.append(statSum['mean'])
        plt.plot(ages, means)
    plt.xlabel('Age')
    plt.ylabel('Fraction susceptible')
    plt.ylim(0,1)
    plt.legend(['Data'] + scenarioNames) #TODO better name?
    plt.savefig('AgeImmunityPlots.png')
    plt.clf()

def getFinalOutbreaksSizes(outputDir, scenarioName, finalDay):
    finalSizes = []
    seeds = getSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), 'cases.csv')
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if int(row['timestep']) == finalDay:
                    finalSizes.append(int(row['cases']))
                    break
    return finalSizes

def outbreakSizesPlots(outputDir, scenarioNames, finalDay=364):
    allFinalSizes = []
    for scenario in scenarioNames:
        finalSizes = getFinalOutbreaksSizes(outputDir, scenario, finalDay)
        allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=scenarioNames)
    plt.ylabel('Final outbreak size after {} days'.format(finalDay + 1))
    plt.savefig('OutbreakSizesPlots.png')
    plt.clf()

def getCasesPerDay(outputDir, scenarioName, seeds, finalDay):
    cumulativeCasesPerDay = OrderedDict()
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), 'cases.csv')
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                timestep = int(row['timestep'])
                cases = int(row['cases'])
                if timestep <= finalDay:
                    if timestep in cumulativeCasesPerDay:
                        cumulativeCasesPerDay[timestep].append(cases)
                    else:
                        cumulativeCasesPerDay[timestep] = [cases]
    newCasesPerDay = OrderedDict()
    prevDay = [1] * len(seeds)
    for day, cumulativeCases in cumulativeCasesPerDay.items():
        currDay = [x - y for x, y in zip(cumulativeCases, prevDay)]
        prevDay = cumulativeCases
        newCasesPerDay[day] = currDay
    return(cumulativeCasesPerDay, newCasesPerDay)

def outbreakEvolutionPlots(outputDir, scenarioNames, finalDay=364):
    dayScale = 20
    for scenario in scenarioNames:
        print("Generating evolution plots for {}".format(scenario))
        seeds = getSeeds(outputDir, scenario)
        (cumulativeCases, newCases) = getCasesPerDay(outputDir, scenario, seeds, finalDay)
        plt.boxplot(list(cumulativeCases.values())[::dayScale])
        plt.savefig(scenario + "CumulativeCases.png")
        plt.clf()
        plt.boxplot(list(newCases.values())[::dayScale])
        plt.savefig(scenario + "NewCases.png")
        plt.clf()

def main(outputDir):
    scenarioNames = ['Random', 'AgeClustering', 'HouseholdClustering', 'AgeAndHouseholdClustering']
    # Create age-immunity plots
    ageImmunityPlots(outputDir, scenarioNames)
    # TODO Household constitution plots -> how?
    # Plots for outbreak sizes
    outbreakSizesPlots(outputDir, scenarioNames)
    # Plots for new cases per day and cumulative cases per day
    outbreakEvolutionPlots(outputDir, scenarioNames)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="Directory containing simulation output files.")
    args = parser.parse_args()
    main(args.dir)

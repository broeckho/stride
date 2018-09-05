import argparse
import csv
import math
import matplotlib.pyplot as plt
import numpy as np
import os
import xml.etree.ElementTree as ET

"""
def getCasesPerDay(scenarioName):
    timesteps = []
    total_cases = []
    new_cases = []

    with open(os.path.join(scenarioName, "cases.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            cases = int(row["cases"])
            if timestep == 0:
                last = cases
            timesteps.append(timestep)
            total_cases.append(cases)
            new_cases.append(cases - last)
            last = cases
    return (timesteps, total_cases, new_cases)

def plotNewCasesPerDay(scenarios):
    lines = ["b-", "g-", "r-", "y-", "m-"]
    handles = [None] * len(scenarios)
    for i in range(len(scenarios)):
        lineStyle = lines[i]
        scenarioName = scenarios[i]
        (timesteps, totalCases, newCases) = getCasesPerDay(scenarioName)
        handles[i], = plt.plot(timesteps, newCases, lineStyle, label=scenarioName)
    plt.xlabel("Day")
    plt.ylabel("Number of new cases")
    plt.title("New cases per day")
    plt.legend(handles=handles)
    plt.show()

'''
def plotTotalCasesPerDay(scenarios):
    lines = ["b-", "g-", "r-", "y-", "m-"]
    handles = [None] * len(scenarios)
    for i in range(len(scenarios)):
        lineStyle = lines[i]
        scenarioName = scenarios[i]
        (timesteps, totalCases, newCases) = getCasesPerDay(os.path.join(scenarioName, "cases.csv"))
        handles[i], = plt.plot(timesteps, totalCases, lineStyle, label=scenarioName)
    plt.plot(timesteps, [SUSCEPTIBLES_AT_START] * len(timesteps), "k-")
    plt.xlabel("Day")
    plt.ylabel("Total number of cases")
    plt.title("Total number of cases per day\n(black horizontal line = # susceptibles at start)")
    plt.legend(handles=handles)
    plt.show()
"""
'''
def plotAgeImmunityProfiles(scenarioNames, seeds):
    # Plot target immunity profile
    targetImmunityChild = ET.parse(os.path.join('data', 'measles_child_immunity.xml')).getroot()
    targetImmunityAdult = ET.parse(os.path.join('data', 'measles_adult_immunity.xml')).getroot()
    targetProfile = []
    for age in range(18):
        tag = 'age' + str(age)
        targetProfile.append(1 - float(targetImmunityChild.find(tag).text))
    for age in range(19, 100):
        tag = 'age' + str(age)
        targetProfile.append(1 - float(targetImmunityAdult.find(tag).text))
    plt.plot(targetProfile, 'bo')
    linestyles = ['g', 'y', 'm', 'r']
    for scenario in scenarioNames:
        for seed in seeds:
            directory = scenario + '_' + str(seed)
            immunityFile = os.path.join(directory, 'susceptibles_by_age.json')
            with open(immunityFile) as jsonFile:
                immunityProfile = json.load(jsonFile)
            plt.plot(list(immunityProfile.values()))
'''

def summary(l):
    mean = sum(l) / len(l)
    list.sort(l)
    minIndex = math.floor((len(l) / 100) * 5)
    maxIndex = math.floor((len(l) / 100) * 95)
    min95Conf = l[minIndex]
    max95Conf = l[maxIndex]
    return {'mean': mean, 'min_95_conf': min95Conf, 'max_95_conf': max95Conf}

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

def ageImmunityPlots(outputDir, scenarioNames, seeds):
    # Plot target immunity rates by age
    ages = range(100)
    targetSusceptibilityRates = getTargetSusceptibilityRates(outputDir)
    '''plt.plot(ages, targetSusceptibilityRates, 'bo')
    plt.xlabel("Age")
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    plt.show()'''
    for scenario in scenarioNames:
        print(scenario)
        totalSuscpetibilityRates = []
        for i in range(100):
            totalSuscpetibilityRates.append([])
        for s in seeds:
            actualSusceptibilityRates = getActualSusceptibilityRates(outputDir, scenario, s)
            for age in range(100):
                totalSuscpetibilityRates[age].append(actualSusceptibilityRates[age])
        means = []
        minConfs = []
        maxConfs = []
        for age in range(100):
            s = summary(totalSuscpetibilityRates[age])
            means.append(s['mean'])
            minConfs.append(abs(s['mean'] - s['min_95_conf']))
            maxConfs.append(abs(s['mean'] - s['max_95_conf']))
        plt.errorbar(ages, means, yerr=[minConfs, maxConfs])
        plt.ylim(0,1)
        plt.show()

def getSeeds(outputDir):
    seedsFile = os.path.join(outputDir, 'seeds.csv')
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        seeds = [int(x) for x in next(reader)]
    return seeds

def main(outputDir):
    scenarioNames = ['Random', 'AgeClustering', 'HouseholdClustering', 'AgeAndHouseholdClustering']
    seeds = getSeeds(outputDir)
    # Age-dependent immunity plots
    ageImmunityPlots(outputDir, scenarioNames, seeds)
    # TODO household constitution
    # TODO outbreak occurence
    # TODO outbreak sizes
    # TODO outbreak durations
    # TODO new cases per day
    # TODO time + size of peak

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="Directory containing simulation output files.")
    args = parser.parse_args()
    main(args.dir)

import csv
import matplotlib.pyplot as plt
import os

MAX_AGE = 99

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "_seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def saveFig(outputDir, figName, extension="eps"):
    plt.savefig(os.path.join(outputDir, figName + "." + extension), format=extension, dpi=1000)
    plt.clf()

'''
import multiprocessing

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep == (numDays - 1):
                return int(row["cases"])


def getFractionSusceptibles(outputDir, scenarioName, seed):
    totalPopulation = 0
    totalSusceptibles = 0
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalPopulation += 1
            if int(row["susceptible"]):
                totalSusceptibles += 1
    if totalPopulation > 0:
        return totalSusceptibles / totalPopulation
    else:
        print("Empty population!")

def getOverallFractionSusceptibles(outputDir, R0s, scenarioNames, poolSize):
    allFractions = []
    for R0 in R0s:
        scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
        for scenario in scenarioNamesFull:
            seeds = getRngSeeds(outputDir, scenario)
            with multiprocessing.Pool(processes=poolSize) as pool:
                fractions = pool.starmap(getFractionSusceptibles, [(outputDir, scenario, s) for s in seeds])
                allFractions += fractions
    if all(x == allFractions[0] for x in allFractions):
        return allFractions[0]
    else:
        print("Differing immunity levels!")'''

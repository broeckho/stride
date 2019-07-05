import csv
import math
import matplotlib.pyplot as plt
import multiprocessing
import os

MAX_AGE = 99

COLORS = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
            'magenta', 'yellow', 'lime', 'violet', 'firebrick',
            'forestgreen', 'turquoise']

def lnFunc(xs, a, b):
    return [a + b * math.log(x + 1) for x in xs]

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "_seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep == (numDays - 1):
                return int(row["cases"])

def getSecondaryCases(outputDir, scenarioName, seed):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    indexCases = []
    infectors = {}
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            infectorID = int(line[2])
            if infectorID == -1:
                # Infection of index case at simulation setup
                indexCases.append(int(line[1]))
            else:
                if infectorID in infectors:
                    infectors[infectorID] += 1
                else:
                    infectors[infectorID] = 1
    secondaryCases = 0
    for i in indexCases:
        if i in infectors:
            secondaryCases += infectors[i]
    secondaryCases /= len(indexCases)
    return secondaryCases

def saveFig(outputDir, figName, extension="eps"):
    if not os.path.exists("fig"):
        os.mkdir("fig")
    plt.savefig(os.path.join(outputDir, "fig", figName + "." + extension), format=extension, dpi=1000)
    plt.clf()

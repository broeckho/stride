import csv
import matplotlib.pyplot as plt
import os

COLORS = ["red", "cyan", "magenta", "brown", "green", "orange", "pink"]
MAX_AGE = 99    # Maximum age in the simulations

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
    if not os.path.exists("fig"):
        os.mkdir("fig")
    plt.savefig(os.path.join(outputDir, "fig", figName + "." + extension), format=extension, dpi=1000)
    plt.clf()

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    totalTransmissions = 0
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                simDay = int(line[6])
                if simDay < numDays:
                    totalTransmissions += 1
    return totalTransmissions

def getSecondaryCasesFromIndex(outputDir, scenarioName, seed):
    """
        Get number of secondary cases caused by index case.
    """
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    indexCases = []
    infectors = {}
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            infectorID = int(line[2])
            if infectorID == -1:
                # Infection of index case at simulation setup
                indexCases.append(int(line[1])) # Add ID of infected to list of index cases
            else:
                if infectorID in infectors:
                    infectors[infectorID] += 1
                else:
                    infectors[infectorID] = 1
    secondaryCases = 0
    for pID in indexCases:
        if pID in infectors:
            secondaryCases += infectors[pID]
    secondaryCases /= len(indexCases) # Take mean in case of more than one index case
    return secondaryCases

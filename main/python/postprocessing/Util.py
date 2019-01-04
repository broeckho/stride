import csv
import os

MAX_AGE = 99

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep == (numDays - 1):
                return int(row["cases"])

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "_seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

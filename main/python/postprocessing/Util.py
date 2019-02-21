import csv
import os

import matplotlib.pyplot as plt

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

def saveFig(outputDir, figName):
    plt.savefig(os.path.join(outputDir, figName + ".eps"), format='eps', dpi=1000)
    plt.clf()

'''
bbox_inches : str or Bbox, optional
Bbox in inches. Only the given portion of the figure is saved. If 'tight', try to figure out the tight bbox of the figure. If None, use savefig.bbox

pad_inches : scalar, optional
Amount of padding around the figure when bbox_inches is 'tight'. If None, use savefig.pad_inches
'''

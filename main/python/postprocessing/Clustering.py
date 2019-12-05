import csv
import matplotlib.pyplot as plt
import multiprocessing
import os

from .Util import getRngSeeds, saveFig

def getAssortativityCoefficient(outputDir, scenarioName, seed, ageLim):
    assortativityCoeffFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "assortativity_coeff.csv")
    with open(assortativityCoeffFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            if int(row["age_lim"]) == ageLim:
                return float(row["assortativity_coeff"])

def createAssortativityCoefficientPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, poolSize, ageLim=99):

    allAssortativityCoefficients = []
    for level in clusteringLevels:
        assortativityCoefficients = []
        for prob in transmissionProbabilities:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                assortativityCoefficients += pool.starmap(getAssortativityCoefficient,
                                                [(outputDir, fullScenarioName, s, ageLim) for s in seeds])
        allAssortativityCoefficients.append(assortativityCoefficients)
    plt.boxplot(allAssortativityCoefficients, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Household assortativity coefficient")
    plt.ylim(0, 1)
    saveFig(outputDir, "AssortativityCoefficients_" + str(ageLim) + "_" + scenarioName)

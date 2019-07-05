import csv
import matplotlib.pyplot as plt
import multiprocessing
import networkx as nx
import os

from .Util import getRngSeeds, saveFig

def getAssortativityCoefficient(outputDir, scenarioName, seed):
    G = nx.Graph()
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    householdFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "households.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            # Add person node to graph
            G.add_node(int(row["person_id"]), susceptible=row["susceptible"])
    with open(householdFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            personIDs = (row["person_ids"])[1:-1].split(", ")
            personIDs = [int(x) for x in personIDs]
            for p1 in personIDs:
                for p2 in personIDs:
                    G.add_edge(p1, p2)
    assortativityCoeff = nx.attribute_assortativity_coefficient(G, "susceptible")
    return assortativityCoeff

def createAssortativityCoefficientPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    allAssortativityCoefficients = []
    for level in clusteringLevels:
        assortativityCoefficients = []
        for prob in transmissionProbabilities:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                assortativityCoefficients += pool.starmap(getAssortativityCoefficient,
                                                            [(outputDir, fullScenarioName, s) for s in seeds])
        allAssortativityCoefficients.append(assortativityCoefficients)
    plt.boxplot(allAssortativityCoefficients, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Assortativity coefficient")
    plt.ylim(0, 0.5)
    saveFig(outputDir, "AssortativityCoefficients_" + scenarioName)

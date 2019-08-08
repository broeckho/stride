import csv
import matplotlib.pyplot as plt
import multiprocessing
import networkx as nx
import os

from .Util import getRngSeeds, saveFig

def getAssortativityCoefficient(outputDir, scenarioName, seed, ageLim):
    G = nx.Graph()
    householdFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "households.csv")
    with open(householdFile) as csvfile:
        reader = csv.DictReader(csvfile)
        personID = 0
        for row in reader:
            personsImmunity = (row["susceptible"])[1:-1].split(", ")
            personsImmunity = [int(x) for x in personsImmunity]
            personAges = (row["ages"])[1:-1].split(", ")
            personAges = [int(x) for x in personAges]
            personIDs = []
            for p_i in range(len(personsImmunity)):
                # Add person node to graph
                if personAges[p_i] < ageLim:
                    G.add_node(personID, susceptible=personsImmunity[p_i])
                    personIDs.append(personID)
                    personID += 1
            for p1 in personIDs:
                for p2 in personIDs:
                    G.add_edge(p1, p2)
    assortativityCoeff = nx.attribute_assortativity_coefficient(G, "susceptible")
    return assortativityCoeff

def createAssortativityCoefficientPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, poolSize, ageLim=100):
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
    plt.ylabel("Assortativity coefficient")
    plt.ylim(0, 1)
    saveFig(outputDir, "AssortativityCoefficients_" + scenarioName)

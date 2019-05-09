import csv
import matplotlib.pyplot as plt
import multiprocessing
import networkx as nx
import os

from .Util import getRngSeeds, saveFig

def GetAssortativityCoefficient(outputDir, scenarioName, seed):
    G = nx.Graph()
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    householdFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "households.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            # Add person to graph
            G.add_node(int(row["person_id"]), susceptible=row["susceptible"])
    with open(householdFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            personIDs = (row["person_ids"])[1:-1].split(", ")
            personIDs = [int(x) for x in personIDs]
            for p1 in personIDs:
                for p2 in personIDs:
                    if p1 != p2:
                        G.add_edge(p1, p2)
    assortativityCoeff = nx.attribute_assortativity_coefficient(G, "susceptible")
    return assortativityCoeff

def createAssortativityCoefficientPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels, poolSize):
    allAssortativityCoefficients = []
    labels = []
    for scenario in scenarioNames:
        for level in clusteringLevels:
            assortativityCoefficients = []
            for prob in transmissionProbabilities:
                scenarioFull = scenario + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
                seeds = getRngSeeds(outputDir, scenarioFull)
                with multiprocessing.Pool(processes=poolSize) as pool:
                    assortativityCoefficients += pool.starmap(GetAssortativityCoefficient,
                                                    [(outputDir, scenarioFull, s) for s in seeds])
            allAssortativityCoefficients.append(assortativityCoefficients)
            labels.append(scenario.capitalize() + ",\nClustering = " + str(level))
    plt.boxplot(allAssortativityCoefficients, labels=labels)
    plt.xlabel("Scenario")
    plt.xticks(rotation=35)
    plt.ylabel("Assortativtity coefficient")
    plt.tight_layout()
    saveFig(outputDir, "AssortativityCoefficients")

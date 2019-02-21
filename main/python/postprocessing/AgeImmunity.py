import csv
import matplotlib.pyplot as plt
import multiprocessing
import os
import statistics
import xml.etree.ElementTree as ET

from .Util import getRngSeeds, saveFig, MAX_AGE

"""
def createHouseholdConstitutionPlots(outputDir, scenarioNames):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        for s in seeds[:1]:
            households = {}
            susceptiblesFile = os.path.join(outputDir, scenario + str(s), 'susceptibles.csv')
            with open(susceptiblesFile) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    hhId = int(row['household'])
                    isSusceptible = int(row['susceptible'])
                    if hhId in households:
                        households[hhId].append(isSusceptible)
                    else:
                        households[hhId] = [isSusceptible]
            householdImmunities = []
            for hhId, members in households.items():
                pctImmune = 1 - (sum(members) / len(members))
                householdImmunities.append(pctImmune)
            plt.hist(householdImmunities)
            plt.title(scenario)
            plt.show()
"""

def getTargetRates(outputDir, targetRatesFile):
    targetRatesTree = ET.parse(os.path.join(outputDir, 'data', targetRatesFile))
    targetRates = []
    for r in targetRatesTree.iter():
        if r.tag not in ["immunity", "data_source", "data_manipulation"]:
            targetRates.append(float(r.text))
    # 1 - immunityRate = susceptibilityRate
    return [1 - x for x in targetRates]

def getAgeSusceptibilityRates(outputDir, scenarioName, seed):
    totalsByAge = [0] * (MAX_AGE + 1)
    susceptiblesByAge = [0] * (MAX_AGE + 1)
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row["age"]))
            totalsByAge[age] += 1
            if (int(row["susceptible"])):
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

def createAgeImmunityPlot(outputDir, scenarioNames, scenarioDisplayNames,
    poolSize, figName="AgeImmunityPlot.png", targetRatesFile=None):
    if targetRatesFile is not None:
        targetRates = getTargetRates(outputDir, targetRatesFile)
        plt.plot(targetRates, 'bo')
        scenarioDisplayNames = ["Data"] + scenarioDisplayNames

    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for scenario_i in range(len(scenarioNames)):
        scenario = scenarioNames[scenario_i]
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            ageSusceptibilityRates = pool.starmap(getAgeSusceptibilityRates, [(outputDir, scenario, s) for s in seeds])
            avgRates = []
            for i in range(MAX_AGE + 1):
                avgRates.append(sum([rates[i] for rates in ageSusceptibilityRates]) / len(ageSusceptibilityRates))
            if dashes[scenario_i] is not None:
                plt.plot(range(MAX_AGE + 1), avgRates, linestyle=linestyles[scenario_i], dashes=dashes[scenario_i], color=colors[scenario_i])
            else:
                plt.plot(range(MAX_AGE + 1), avgRates, linestyle=linestyles[scenario_i], color=colors[scenario_i])
    plt.xlabel("Age")
    plt.xlim(0, MAX_AGE + 1)
    plt.ylabel("Fraction susceptibles")
    plt.ylim(0, 1)
    plt.legend(scenarioDisplayNames)
    saveFig(outputDir, figName)

def getOverallImmunityRate(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    totalPersons = 0
    totalSusceptibles = 0
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            totalPersons += 1
            if (int(row["susceptible"])):
                totalSusceptibles += 1
    return 1 - (totalSusceptibles / totalPersons)

def getAvgOverallImmunityRate(outputDir, scenarioName, poolSize):
    seeds = getRngSeeds(outputDir, scenarioName)
    with multiprocessing.Pool(processes=poolSize) as pool:
        immunityRates = pool.starmap(getOverallImmunityRate,
                                    [(outputDir, scenarioName, s) for s in seeds])
        return (sum(immunityRates) / len(immunityRates))

def createAgeDistributionPlots(outputDir, popFile):
    allAges = []
    with open(os.path.join(outputDir, popFile)) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(row["age"])
            allAges.append(age)
    plt.hist(allAges, bins=100)
    plt.xlabel("Age")
    plt.ylabel("Number of persons")
    saveFig(outputDir, "AgeDistribution")
'''
import networkx as nx

def getSusceptibleContactsGraph(logfile):
    """
        Turn logfile of contacts between susceptibles
        into a graph.
    """
    G = nx.Graph()
    with open(logfile) as f:
        for line in f:
            linedata = line.split(' ')
            if linedata[0] == '[CONT]':
                p1_id = int(linedata[1])
                p2_id = int(linedata[2])
                G.add_nodes_from([p1_id, p2_id])
                G.add_edge(p1_id, p2_id)
    return G


def countImmune(sim, timestep):
    if timestep == 0:
        print("Determining pocket sizes of non-immunized individuals")
        households = {}
        schools = {}
        workplaces = {}
        communities1 = {}
        communities2 = {}

        pop = sim.GetSimulator().GetPopulation()
        for pIndex in range(pop.size()):
            person = pop[pIndex]
            if person.GetHealth().IsSusceptible():
                addPerson(households, person.GetClusterId(Id_Household), True)
                addPerson(schools, person.GetClusterId(Id_School), True)
                addPerson(workplaces, person.GetClusterId(Id_Work), True)
                addPerson(communities1, person.GetClusterId(Id_PrimaryCommunity), True)
                addPerson(communities2, person.GetClusterId(Id_SecondaryCommunity), True)
            else:
                addPerson(households, person.GetClusterId(Id_Household), False)
                addPerson(schools, person.GetClusterId(Id_School), False)
                addPerson(workplaces, person.GetClusterId(Id_Work), False)
                addPerson(communities1, person.GetClusterId(Id_PrimaryCommunity), False)
                addPerson(communities2, person.GetClusterId(Id_SecondaryCommunity), False)

        # Distribution of susceptibles by cluster type
        bins = [0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        household_results = toHist(toFractions(households), bins)
        school_results = toHist(toFractions(schools), bins)
        work_results = toHist(toFractions(workplaces), bins)
        comm1_results = toHist(toFractions(communities1), bins)
        comm2_results = toHist(toFractions(communities2), bins)

        width = 0.015

        fig, ax = plt.subplots()
        ax.bar(bins, household_results, width=width, color='red')
        ax.bar([x + width for x in bins], school_results, width, color='tan')
        ax.bar([x + (2* width) for x in bins], work_results, width, color='lime')
        ax.bar([x + (3* width) for x in bins], comm1_results, width, color='orange')
        ax.bar([x + (4* width) for x in bins], comm2_results, width, color='black')
        plt.show()
        # Overall distribution of susceptibles
        bins = [0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        all_fractions = toFractions(households) + toFractions(schools) + toFractions(workplaces) + toFractions(communities1) + toFractions(communities2)
        all_results = toHist(all_fractions, bins)
        plt.ylim([0,1])
        plt.legend(['Baseline'])
        plt.bar(bins, all_results, width=0.08)
        plt.savefig('baseline')

def toHist(fractions, bins):
    ys = [0] * len(bins)
    for frac in fractions:
        for i_bin in range(len(bins)):
            if frac <= bins[i_bin]:
                ys[i_bin] += 1
                break
    for i_y in range(len(ys)):
        ys[i_y] = ys[i_y] / len(fractions)
    return ys


def toFractions(dictionary):
    fractions = []
    for key in dictionary:
        cluster_size = dictionary[key]['susceptible'] + dictionary[key]['other']
        frac_susceptible = dictionary[key]['susceptible'] / cluster_size
        fractions.append(frac_susceptible)
    return fractions


def addPerson(dictionary, cluster_id, susceptible):
    if cluster_id != 0:
        if not (cluster_id in dictionary):
            dictionary[cluster_id] = {
                'susceptible' : 0,
                'other' : 0
            }

        if susceptible:
            dictionary[cluster_id]['susceptible'] += 1
        else:
            dictionary[cluster_id]['other'] += 1
'''

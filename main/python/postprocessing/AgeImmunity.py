import collections
import csv
import matplotlib.pyplot as plt
import multiprocessing
import os
import statistics
import xml.etree.ElementTree as ET

from .Util import getRngSeeds, saveFig, MAX_AGE

def getHouseholdConstitutions(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        households = {}
        for row in reader:
            age = int(float(row["age"]))
            if age < 18:
                hhID = int(row["hh_id"])
                isSusceptible = int(row["susceptible"])
                if hhID in households:
                    households[hhID].append(isSusceptible)
                else:
                    households[hhID] = [isSusceptible]
        return list(households.values())

def groupBySizeAndConstitution(households, bins):
    allSizes = {}
    for hh in households:
        if len(hh) not in allSizes:
            allSizes[len(hh)] = dict(bins)
        pctSusceptible = (sum(hh) / len(hh)) * 100
        for b in bins:
            if pctSusceptible >= b[0] and pctSusceptible <= b[1]:
                allSizes[len(hh)][b] += 1
                break
    for s in allSizes:
        numHHOfSize = sum(allSizes[s].values())
        for b in allSizes[s]:
            allSizes[s][b] /= numHHOfSize
    return allSizes

def createHouseholdConstitutionPlot(outputDir, scenarioName, poolSize, figName, stat="mean"):
    ax = plt.axes(projection="3d")
    bins = {
        (0, 0): 0,
        (1, 10): 0,
        (11, 20): 0,
        (21, 30): 0,
        (31, 40): 0,
        (41, 50): 0,
        (51, 60): 0,
        (61, 70): 0,
        (71, 80): 0,
        (81, 90): 0,
        (91, 100): 0,
    }
    seeds = getRngSeeds(outputDir, scenarioName)
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown', 'magenta', 'yellow', "cyan"]
    with multiprocessing.Pool(processes=poolSize) as pool:
        households = pool.starmap(getHouseholdConstitutions, [(outputDir, scenarioName, s) for s in seeds])
        allSizes = []
        allGrouped = []
        for run in households:
            grouped = groupBySizeAndConstitution(run, bins)
            allSizes += [hhSize for hhSize in grouped.keys() if hhSize not in allSizes]
            allGrouped.append(grouped)
        allSizes.sort()
        for hhSize in allSizes:
            sortedBins = list(bins.keys())
            sortedBins.sort(key=lambda x: x[0])
            ys = []
            for b in sortedBins:
                allFrequencies = [run[hhSize][b] for run in allGrouped]
                if stat == "mean":
                    ys.append(sum(allFrequencies) / len(allFrequencies))
                elif stat == "median":
                    ys.append(statistics.median(allFrequencies))
                else:
                    print("No valid statistic supplied!")
            ax.bar(range(len(sortedBins)), ys, zs=hhSize, zdir="y", color=colors[hhSize - 1], alpha=0.4)
    ax.set_xlabel("Pct susceptibles")
    ax.set_xticks(range(len(sortedBins)))
    ax.set_xticklabels([b[1] for b in sortedBins])
    ax.set_ylabel("Number of children in hh")
    ax.set_zlabel("Frequency")
    ax.set_zlim(0, 1)
    saveFig(outputDir, figName, "png")

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

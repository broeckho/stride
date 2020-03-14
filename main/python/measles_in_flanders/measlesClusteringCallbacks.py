import csv
import networkx as nx
import os
import random

from measlesClusteringUtil import MAX_AGE

def registerSampleAssociativityCoefficient(simulator, event):
    ageLims = [99]

    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    clusteringPoolType = simulator.GetConfigValue("run.vaccine_clustering_pooltype")

    pop = simulator.GetPopulation()

    poolTypes = {
        "Household": 0,
        "K12School": 1,
        "College": 2,
        "Workplace": 3,
        "PrimaryCommunity": 4,
        "SecondaryCommunity": 5,
    }

    poolTypeID = poolTypes[clusteringPoolType]

    pools = {}
    for i in range(pop.size()):
        person = pop[i]
        age = person.GetAge()
        poolID = person.GetPoolId(poolTypeID)
        isSusceptible = person.GetHealth().IsSusceptible()
        if poolID != 0:
            if poolID in pools:
                pools[poolID].append((age, isSusceptible))
            else:
                pools[poolID] = [(age, isSusceptible)]

    # select sample of 1000 pools
    samplePools = random.sample(pools, k=1000)

    with open(os.path.join(outputPrefix, "assortativity_coeff.csv"), "w") as csvfile:
        fieldnames = ["age_lim", "assortativity_coeff"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        for lim in ageLims:
            G = nx.Graph()
            personID = 0
            for poolID in pools:
                personIDs = []
                for personAge, personImmunity in pools[poolID]:
                    # Add person node to graph (if within age limit)
                    if personAge <= lim:
                        G.add_node(personID, susceptible=personImmunity)
                        personIDs.append(personID)
                        personID += 1
                # Add edges between persons belonging to the same pool
                for p1 in personIDs:
                    for p2 in personIDs:
                        G.add_edge(p1, p2)
            assCoeff = nx.attribute_assortativity_coefficient(G, "susceptible")
            writer.writerow({"age_lim": lim, "assortativity_coeff": assCoeff})

def registerAssociativityCoefficient(simulator, event):
    ageLims = [99]

    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    clusteringPoolType = simulator.GetConfigValue("run.vaccine_clustering_pooltype")

    pop = simulator.GetPopulation()

    poolTypes = {
        "Household": 0,
        "K12School": 1,
        "College": 2,
        "Workplace": 3,
        "PrimaryCommunity": 4,
        "SecondaryCommunity": 5,
    }

    poolTypeID = poolTypes[clusteringPoolType]

    pools = {}
    for i in range(pop.size()):
        person = pop[i]
        age = person.GetAge()
        poolID = person.GetPoolId(poolTypeID)
        isSusceptible = person.GetHealth().IsSusceptible()
        if poolID != 0:
            if poolID in pools:
                pools[poolID].append((age, isSusceptible))
            else:
                pools[poolID] = [(age, isSusceptible)]

    with open(os.path.join(outputPrefix, "assortativity_coeff.csv"), "w") as csvfile:
        fieldnames = ["age_lim", "assortativity_coeff"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        for lim in ageLims:
            G = nx.Graph()
            personID = 0
            for poolID in pools:
                personIDs = []
                for personAge, personImmunity in pools[poolID]:
                    # Add person node to graph (if within age limit)
                    if personAge <= lim:
                        G.add_node(personID, susceptible=personImmunity)
                        personIDs.append(personID)
                        personID += 1
                # Add edges between persons belonging to the same pool
                for p1 in personIDs:
                    for p2 in personIDs:
                        G.add_edge(p1, p2)
            assCoeff = nx.attribute_assortativity_coefficient(G, "susceptible")
            writer.writerow({"age_lim": lim, "assortativity_coeff": assCoeff})

def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()

    immunityByAge = []
    for age in range(MAX_AGE + 1):
        immunityByAge.append({"age": age, "susceptible": 0, "immune": 0})

    for i in range(pop.size()):
        person = pop[i]
        age = int(person.GetAge())
        isSusceptible = person.GetHealth().IsSusceptible()
        if isSusceptible:
            immunityByAge[age]["susceptible"] += 1
        else:
            immunityByAge[age]["immune"] += 1

    with open(os.path.join(outputPrefix, "susceptibles_by_age.csv"), "w") as csvfile:
        fieldnames = ["age", "susceptible", "immune"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for entry in immunityByAge:
            writer.writerow(entry)

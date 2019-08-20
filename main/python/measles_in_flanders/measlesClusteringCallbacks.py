import csv
import networkx as nx
import os

from measlesClusteringUtil import MAX_AGE

def registerAssociativityCoefficient(simulator, event):
    # TODO Take ages of persons into account?
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()

    households = {}
    for i in range(pop.size()):
        person = pop[i]
        householdID = person.GetPoolId(0)
        isSusceptible = person.GetHealth().IsSusceptible()
        if householdID in households:
            households[householdID].append(isSusceptible)
        else:
            households[householdID] = [isSusceptible]

    G = nx.Graph()
    personID = 0
    for hh in households:
        personIDs = []
        for personImmunity in households[hh]:
            # Add person node to graph
            G.add_node(personID, susceptible=personImmunity)
            personIDs.append(personID)
            personID += 1
        for p1 in personIDs:
            for p2 in personIDs:
                G.add_edge(p1, p2)
    assortativityCoeff = nx.attribute_assortativity_coefficient(G, "susceptible")
    with open(os.path.join(outputPrefix, "assortativity_coeff.csv"), "w") as csvfile:
        fieldnames = ["age_lim", "assortativity_coeff"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerow({"age_lim": 99, "assortativity_coeff": assortativityCoeff})

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

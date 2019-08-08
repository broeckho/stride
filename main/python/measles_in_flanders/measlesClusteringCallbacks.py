import csv
import os

from measlesClusteringUtil import MAX_AGE

def registerMemberships(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()
    households = {}
    for i in range(pop.size()):
        person = pop[i]
        householdID = person.GetPoolId(0)
        age = int(person.GetAge())
        isSusceptible = person.GetHealth().IsSusceptible()
        if isSusceptible:
            isSusceptible = 1
        else:
            isSusceptible = 0
        if householdID in households:
            households[householdID]["ages"].append(age)
            households[householdID]["susceptible"].append(isSusceptible)
        else:
            households[householdID] = {"ages": [age], "susceptible": [isSusceptible]}
    with open(os.path.join(outputPrefix, "households.csv"), "w") as csvfile:
        fieldnames = ["household_id", "ages", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for hhID in households:
            writer.writerow({"household_id": hhID,
                                "ages": households[hhID]["ages"],
                                "susceptible": households[hhID]["susceptible"]})

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

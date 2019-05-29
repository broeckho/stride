import csv
import os

from measlesClusteringUtil import MAX_AGE

def registerMemberships(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()
    households = {}
    for i in range(pop.size()):
        person = pop[i]
        personID = int(person.GetId())
        householdID = int(person.GetPoolId(0))
        if householdID != 0:
            if householdID in households:
                households[householdID].append(personID)
            else:
                households[householdID] = [personID]
    print(len(households))
    with open(os.path.join(outputPrefix, "households.csv"), "w") as csvfile:
        fieldnames = ["pool_id", "person_ids"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for poolID, personIDs in households.items():
            writer.writerow({"pool_id": poolID, "person_ids": personIDs})

def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    pop = simulator.GetPopulation()
    with open(os.path.join(outputPrefix, "susceptibles.csv"), "w") as csvfile:
        fieldnames = ["person_id", "age", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for i in range(pop.size()):
            person = pop[i]
            personID = person.GetId()
            age = person.GetAge()
            isSusceptible = person.GetHealth().IsSusceptible()
            # Convert boolean to int for easier reading from file in different langs
            if isSusceptible:
                isSusceptible = 1
            else:
                isSusceptible = 0
            writer.writerow({"person_id": personID, "age": age, "susceptible": isSusceptible})


def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        at each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

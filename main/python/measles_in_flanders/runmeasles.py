import csv
import os

from pystride.Event import Event, SteppedEvent, EventType
from pystride.PyController import PyController


SUSCEPTIBLES_AT_START = []

def checkHouseholds(simulator, event):
    pop = simulator.GetPopulation()
    households = {}
    for i in range(pop.size()):
        hhId = pop[i].GetHouseholdId()
        age = pop[i].GetAge()
        susceptible = pop[i].GetHealth().IsSusceptible()
        if hhId in households:
            households[hhId].append((age, susceptible))
        else:
            households[hhId] = [(age, susceptible)]
    numHouseholds = len(households)
    numScatteredHouseholds = 0
    for hhId, members in households.items():
        numSusceptible = 0
        for p in members:
            if p[1]:
                numSusceptible += 1
        pctSusceptible = numSusceptible / len(members)
        if pctSusceptible > 0 and pctSusceptible < 1:
            # Not all members have same vaccination status
            children = [p for p in members if p[0] < 18 and p[0] > 0]
            if len(children) > 0:
                numSusceptible = 0
                for c in children:
                    if c[1]:
                        numSusceptible += 1
                pctSusceptible = numSusceptible / len(children)
                if pctSusceptible > 0 and pctSusceptible < 1:
                    numScatteredHouseholds += 1
    print(float(numScatteredHouseholds) / numHouseholds)

def getNumSusceptible(simulator, event):
    numSusceptible = 0
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        if pop[i].GetHealth().IsSusceptible():
            numSusceptible += 1
    global SUSCEPTIBLES_AT_START
    SUSCEPTIBLES_AT_START.append(numSusceptible)

def trackCases(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

def getAgeImmunityProfile(simulator, event):
    totalPopulation = {}
    susceptiblePopulation = {}
    for age in range(100):
        totalPopulation[age] = 0
        susceptiblePopulation[age] = 0
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        p = pop[i]
        totalPopulation[p.GetAge()] += 1
        if p.GetHealth().IsSusceptible():
            susceptiblePopulation[p.GetAge()] += 1
    susceptibleProportion = {}
    for age in range(100):
        susceptibleProportion[age] = susceptiblePopulation[age] / totalPopulation[age]

    # Write results to file
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    with open(os.path.join(outputPrefix, "age_immunity_profile.csv"), "w") as csvfile:
        fieldnames = ["age", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for age, proportion in susceptibleProportion.items():
            writer.writerow({"age": age, "susceptible": proportion})

COMMON_PARAMS = {
    "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
    "behaviour_policy": "NoBehaviour",
    "belief_policy.name": "NoBelief",
    "contact_log_level": "Transmissions",
    "contact_output_file": "true",
    "disease_config_file": "disease_measles.xml",
    "global_information_policy": "NoGlobalInformation",
    "holidays_file": "holidays_none.json",
    "local_information_policy": "NoLocalInformation",
    "num_days": 365,
    "num_participants_survey": 10,
    "num_threads": 1,
    "output_adopted": "false",
    "output_cases": "false",
    "output_persons": "false",
    "output_summary": "false",
    "population_file": "pop_flanders600.csv",
    "rng_seed": 0,
    "rng_type": "mrg2",
    "r0": 16,
    "seeding_age_min": 1,
    "seeding_age_max": 99,
    "seeding_rate": 0.000001667,
    "start_date": "2017-01-01",
    "stride_log_level": "info",
    "track_index_case": "false",
    "use_install_dirs": "true",
}

RANDOM_PARAMS = {
    "output_prefix": "Random",
    "immunity_profile": "Random",
    "immunity_rate": 0.951513,
    # Vaccine profile is empty since random immunity was seeded
    # through immunity profile
    "vaccine_profile": "None",
}

AGEDEPENDENT_PARAMS = {
    "output_prefix": "AgeDependent",
    "immunity_link_probability": 0,
    "immunity_profile": "AgeDependent",
    "immunity_distribution": "../data/measles_adult_immunity.xml",
    "vaccine_link_probability": 0,
    "vaccine_profile": "AgeDependent",
    "vaccine_distribution": "../data/measles_child_immunity.xml",
}

CLUSTERING_PARAMS = {
    "output_prefix": "Clustering",
    "immunity_link_probability": 0,
    "immunity_profile": "AgeDependent",
    "immunity_distribution": "../data/measles_adult_immunity.xml",
    "vaccine_link_probability": 1,
    "vaccine_profile": "AgeDependent",
    "vaccine_distribution": "../data/measles_child_immunity.xml",
}

def setParams(controller, paramsDict):
    for param, value in paramsDict.items():
        controller.runConfig.setParameter(param, value)

def runScenario(scenarioName, scenarioParams):
    # Configure simulation
    control = PyController()
    setParams(control, COMMON_PARAMS)
    setParams(control, scenarioParams)
    # Register callbacks
    control.registerCallback(getAgeImmunityProfile, EventType.AtStart)
    control.registerCallback(getNumSusceptible, EventType.AtStart)
    control.registerCallback(checkHouseholds, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    # Run simulation
    control.control()

def main():
    runScenario("Random", RANDOM_PARAMS)
    runScenario("AgeDependent", AGEDEPENDENT_PARAMS)
    runScenario("Clustering", CLUSTERING_PARAMS)

if __name__=="__main__":
    main()

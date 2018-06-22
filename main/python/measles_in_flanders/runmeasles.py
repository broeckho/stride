import csv
import os

import matplotlib.pyplot as plt

from pystride.Event import Event, SteppedEvent, EventType
from pystride.PyController import PyController

SUSCEPTIBLES_AT_START = 0

def getNumSusceptible(simulator, event):
    numSusceptible = 0
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        if pop[i].GetHealth().IsSusceptible():
            numSusceptible += 1
    global SUSCEPTIBLES_AT_START
    SUSCEPTIBLES_AT_START = numSusceptible
    print(numSusceptible)

def trackCases(simulator, event):
    outputPrefix = simulator.GetRunConfigParam("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

def getCasesPerDay(filename):
    timesteps = []
    total_cases = []
    new_cases = []

    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            cases = int(row["cases"])
            if timestep == 0:
                last = cases
            timesteps.append(timestep)
            total_cases.append(cases)
            new_cases.append(cases - last)
            last = cases
    return (timesteps, total_cases, new_cases)

def setCommonParams(controller):
    common_params = {
        "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
        "behaviour_policy": "NoBehaviour", "belief_policy.name": "NoBelief",
        "contact_log_level": "Transmissions", "contact_output_file": "true",
        "disease_config_file": "disease_measles.xml",
        "global_information_policy": "NoGlobalInformation",
        "holidays_file": "holidays_none.json",
        "local_information_policy": "NoLocalInformation",
        "num_days": 365,
        "num_participants_survey": 10, "num_threads": 1,
        "output_adopted": "false", "output_cases": "false",
        "output_persons": "false", "output_summary": "false",
        "population_file": "pop_flanders600.csv",
        "rng_seed": 0, "rng_type": "mrg2",
        "r0": 16, "seeding_age_min": 1, "seeding_age_max": 99,
        "seeding_rate": 0.000001667,
        "start_date": "2017-01-01", "stride_log_level": "info",
        "track_index_case": "false", "use_install_dirs": "true",
    }
    for param, value in common_params.items():
        controller.runConfig.setParameter(param, value)

def runRandom():
    control = PyController()
    setCommonParams(control)
    # Output prefix
    control.runConfig.setParameter("output_prefix", "Random")
    # Immunity profile
    control.runConfig.setParameter("immunity_profile", "Random")
    control.runConfig.setParameter("immunity_rate", 0.951513)
    # Vaccination profile is empty since random immunity was seeded
    # through immunity profile
    control.runConfig.setParameter("vaccine_profile", "None")
    control.registerCallback(getNumSusceptible, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()

def runAgeDependent():
    control = PyController()
    setCommonParams(control)
    # Output prefix
    control.runConfig.setParameter("output_prefix", "AgeDependent")
    # Immunity profile
    control.runConfig.setParameter("immunity_link_probability", 0)
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    control.runConfig.setParameter("immunity_distribution", "../data/measles_natural_immunity.xml")
    # Vaccination profile
    control.runConfig.setParameter("vaccine_link_probability", 0)
    control.runConfig.setParameter("vaccine_profile", "AgeDependent")
    control.runConfig.setParameter("vaccine_distribution", "../data/measles_vaccination.xml")
    control.registerCallback(getNumSusceptible, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()

def runClustering():
    control = PyController()
    setCommonParams(control)
    # Output prefix
    control.runConfig.setParameter("output_prefix", "Clustering")
    # Immunity profile
    control.runConfig.setParameter("immunity_link_probability", 0)
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    control.runConfig.setParameter("immunity_distribution", "../data/measles_natural_immunity.xml")
    # Vaccination profile
    control.runConfig.setParameter("vaccine_link_probability", 1)
    control.runConfig.setParameter("vaccine_profile", "AgeDependent")
    control.runConfig.setParameter("vaccine_distribution", "../data/measles_vaccination.xml")
    control.registerCallback(getNumSusceptible, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()

def plotNewCasesPerDay(scenarios):
    lines = ["b-", "g-", "r-", "y-", "m-"]
    handles = [None] * len(scenarios)
    for i in range(len(scenarios)):
        lineStyle = lines[i]
        scenarioName = scenarios[i]
        (timesteps, totalCases, newCases) = getCasesPerDay(os.path.join(scenarioName, "cases.csv"))
        handles[i], = plt.plot(timesteps, newCases, lineStyle, label=scenarioName)
    plt.xlabel("Day")
    plt.ylabel("Number of new cases")
    plt.title("New cases per day")
    plt.legend(handles=handles)
    plt.show()

def plotTotalCasesPerDay(scenarios):
    lines = ["b-", "g-", "r-", "y-", "m-"]
    handles = [None] * len(scenarios)
    for i in range(len(scenarios)):
        lineStyle = lines[i]
        scenarioName = scenarios[i]
        (timesteps, totalCases, newCases) = getCasesPerDay(os.path.join(scenarioName, "cases.csv"))
        handles[i], = plt.plot(timesteps, totalCases, lineStyle, label=scenarioName)
    plt.plot(timesteps, [SUSCEPTIBLES_AT_START] * len(timesteps), "k-")
    plt.xlabel("Day")
    plt.ylabel("Total number of cases")
    plt.title("Total number of cases per day\n(black horizontal line = # susceptibles at start)")
    plt.legend(handles=handles)
    plt.show()

'''
def plotOutbreakSize(scenarios, seeds):
    outbreakSizesAll = []
    for scenarioName in scenarios:
        outbreakSizes = []
        for s in seeds:
            outputDir = scenarioName + "_seed" + str(s)
            (timesteps, totalCases, newCases) = getCasesPerDay(os.path.join(outputDir, "cases.csv"))
            outbreakSizes.append(totalCases[-1])
        outbreakSizesAll.append(outbreakSizes)
    plt.boxplot(outbreakSizesAll, labels=scenarios)
    plt.show()
'''

def ageDependentImmunityProfile():
    ages = range(100)
    immunityProbs = [1] * 100
    immunityProbs[0:10] = [0.30, 0.95, 0.94, 0.93, 0.91, 0.89, 0.87, 0.87, 0.89, 0.91]
    immunityProbs[10:20] = [0.93, 0.94, 0.95, 0.95, 0.94, 0.92, 0.88, 0.83, 0.81, 0.83]
    immunityProbs[20:30] = [0.84, 0.86, 0.87, 0.88, 0.93, 0.96, 0.97, 0.98, 0.99, 0.99]

    susceptibleProbs = [1 - x for x in immunityProbs]
    plt.plot(ages, susceptibleProbs, 'bo')
    plt.xlabel("Age")
    plt.ylabel("Percent susceptible")
    plt.ylim([0,1])
    plt.show()

def main():
    runRandom()
    runAgeDependent()
    runClustering()
    #plotNewCasesPerDay(["Random", "AgeDependent", "Clustering"])
    #plotTotalCasesPerDay(["Random", "AgeDependent", "Clustering"])
    #plotOutbreakSize(["Random", "AgeDependent", "Clustering"], seeds)
    #ageDependentImmunityProfile()

if __name__=="__main__":
    main()

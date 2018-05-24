
#############################################################################
#            Running simulations for 'measles in Flanders' study            #
#############################################################################
import csv
import os

import matplotlib.pyplot as plt

from pystride.Event import Event, SteppedEvent, EventType
from pystride.PyController import PyController

############################################################
#                    Callback functions                    #
############################################################

NUM_SUSCEPTIBLE = {"Random": 0, "AgeDependent": 0, "Clustering": 0}

def getNumImmune(simulator, event):
    num_immune = 0
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        if pop[i].GetHealth().IsImmune():
            num_immune += 1
    print(num_immune)

def getNumSusceptible(simulator):
    num_susceptible = 0
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        if pop[i].GetHealth().IsSusceptible():
            num_susceptible += 1
    return num_susceptible

def getNumSusceptibleRandom(simulator, event):
    global NUM_SUSCEPTIBLE
    NUM_SUSCEPTIBLE["Random"] = getNumSusceptible(simulator)

def getNumSusceptibleAgeDependent(simulator, event):
    global NUM_SUSCEPTIBLE
    NUM_SUSCEPTIBLE["AgeDependent"] = getNumSusceptible(simulator)

def getNumSusceptibleClustering(simulator, event):
    global NUM_SUSCEPTIBLE
    NUM_SUSCEPTIBLE["Clustering"] = getNumSusceptible(simulator)

def trackCases(timestep, num_cases, filename):
    with open(filename, "a", newline="") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": num_cases})

def trackRandomCases(simulator, event):
    num_cases = simulator.GetPopulation().GetInfectedCount()
    filename = os.path.join("Random", "cases.csv")
    trackCases(event.timestep, num_cases, filename)

def trackAgeDependentCases(simulator, event):
    num_cases = simulator.GetPopulation().GetInfectedCount()
    filename = os.path.join("AgeDependent", "cases.csv")
    trackCases(event.timestep, num_cases, filename)

def trackAgeDependent2Cases(simulator, event):
    num_cases = simulator.GetPopulation().GetInfectedCount()
    filename = os.path.join("AgeDependent2", "cases.csv")
    trackCases(event.timestep, num_cases, filename)

def trackClusteringCases(simulator, event):
    num_cases = simulator.GetPopulation().GetInfectedCount()
    filename = os.path.join("Clustering", "cases.csv")
    trackCases(event.timestep, num_cases, filename)

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
        "rng_seed": 2, "rng_type": "mrg2",
        "r0": 16,
        "seeding_age_min": 1, "seeding_age_max": 99,
        "seeding_rate": 0.000001667,
        "start_date": "2017-01-01", "stride_log_level": "info",
        "track_index_case": "false", "use_install_dirs": "true",
    }
    for param, value in common_params.items():
        controller.runConfig.setParameter(param, value)

def runRandom():
    """
        A random 80% of the population is immune to measles,
        either due to natural immunity or due to vaccination.
    """
    control = PyController()
    # Set common parameters
    setCommonParams(control)
    # Set output prefix
    control.runConfig.setParameter("output_prefix", "Random")
    # Immunity profile
    #control.runConfig.setParameter("immunity_link_probability", 0)
    #control.runConfig.setParameter("immunity_profile", "AgeDependent")
    #control.runConfig.setParameter("immunity_distribution", "../data/measles_random_immunity.xml")
    control.runConfig.setParameter("immunity_profile", "Random")
    #0.9462999999999999
    #control.runConfig.setParameter("immunity_rate", 0.9463)

    # Immunity rate based on ACTUAL number of susceptibles in simulation when using age dependent profile
    #                                               0.921935
    control.runConfig.setParameter("immunity_rate", 0.92185)

    # Vaccination profile is empty since random immunity was seeded
    # through immunity profile
    control.runConfig.setParameter("vaccine_profile", "None")

    control.registerCallback(trackRandomCases, EventType.Stepped)
    control.registerCallback(getNumSusceptibleRandom, EventType.AtStart)
    control.control()

def runAgeProfiles():
    control = PyController()
    # Set common parameters
    setCommonParams(control)
    # Set output_prefix
    control.runConfig.setParameter("output_prefix", "AgeDependent")
    # Immunity profile
    control.runConfig.setParameter("immunity_link_probability", 0)
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    control.runConfig.setParameter("immunity_distribution", "../data/measles_natural_immunity.xml")
    # Vaccination profile
    control.runConfig.setParameter("vaccine_link_probability", 0)
    control.runConfig.setParameter("vaccine_profile", "AgeDependent")
    control.runConfig.setParameter("vaccine_distribution", "../data/measles_vaccination.xml")

    control.registerCallback(trackAgeDependentCases, EventType.Stepped)
    control.registerCallback(getNumSusceptibleAgeDependent, EventType.AtStart)
    control.control()

def runAgeProfiles2():
    control = PyController()
    # Set common parameters
    setCommonParams(control)
    # Set output_prefix
    control.runConfig.setParameter("output_prefix", "AgeDependent2")
    # Immunity profile
    control.runConfig.setParameter("immunity_link_probability", 0)
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    control.runConfig.setParameter("immunity_distribution", "../data/measles_age_dependent_immunity.xml")
    # Vaccination profile
    control.runConfig.setParameter("vaccine_profile", "None")

    control.registerCallback(trackAgeDependent2Cases, EventType.Stepped)
    #control.registerCallback(getNumSusceptible, EventType.AtStart)
    control.control()

def runClustering():
    control = PyController()
    # Set common parameters
    setCommonParams(control)
    # Set output prefix
    control.runConfig.setParameter("output_prefix", "Clustering")
    # Immunity profile
    control.runConfig.setParameter("immunity_link_probability", 0)
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    control.runConfig.setParameter("immunity_distribution", "../data/measles_natural_immunity.xml")
    # Vaccination profile
    control.runConfig.setParameter("vaccine_link_probability", 1)
    control.runConfig.setParameter("vaccine_profile", "AgeDependent")
    control.runConfig.setParameter("vaccine_distribution", "../data/measles_vaccination.xml")
    control.registerCallback(trackClusteringCases, EventType.Stepped)
    control.registerCallback(getNumSusceptibleClustering, EventType.AtStart)
    control.control()


#
# Postprocessing functions
#

def getNewCasesPerDay(filename):
    timesteps = []
    new_cases = []

    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            cases = int(row["cases"])
            if timestep == 0:
                last = cases
            timesteps.append(timestep)
            new_cases.append(cases - last)
            last = cases
    return (timesteps, new_cases)

def getTotalCases(filename):
    timesteps = []
    cases = []

    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timesteps.append(int(row["timestep"]))
            cases.append(int(row["cases"]))
    return (timesteps, cases)

def plotNewCasesPerDay():
    (timesteps, new_cases_random) = getNewCasesPerDay(os.path.join("Random", "cases.csv"))
    (timesteps, new_cases_agedependent) = getNewCasesPerDay(os.path.join("AgeDependent", "cases.csv"))
    (timesteps, new_cases_agedependent2) = getNewCasesPerDay(os.path.join("AgeDependent2", "cases.csv"))
    (timesteps, new_cases_clustered) = getNewCasesPerDay(os.path.join("Clustering", "cases.csv"))

    plt.plot(timesteps, new_cases_random)
    plt.plot(timesteps, new_cases_agedependent)
    plt.plot(timesteps, new_cases_agedependent2)
    plt.plot(timesteps, new_cases_clustered)
    plt.legend(["Random immunity", "Age-dependent immunity", "AGE2", "Clustered + Age-dependent immunity"])
    plt.xlabel("Day")
    plt.ylabel("New cases")
    plt.show()

def plotTotalCases():
    (timesteps, cases_random) = getTotalCases(os.path.join("Random", "cases.csv"))
    plt.plot(timesteps, [NUM_SUSCEPTIBLE["Random"]] * len(timesteps), "b-")
    plt_cases_random, = plt.plot(timesteps, cases_random, "b-", label="Random immunity")

    (timesteps, cases_agedependent) = getTotalCases(os.path.join("AgeDependent", "cases.csv"))
    plt.plot(timesteps, [NUM_SUSCEPTIBLE["AgeDependent"]] * len(timesteps), "r-")
    plt_cases_agedependent, = plt.plot(timesteps, cases_agedependent, "r-", label="Age-dependent immunity")

    (timesteps, cases_agedependent2) = getTotalCases(os.path.join("AgeDependent2", "cases.csv"))
    plt.plot(timesteps, [46839] * len(timesteps), "y-")
    plt_cases_agedependent2, = plt.plot(timesteps, cases_agedependent, "y-", label="AGE2")

    (timesteps, cases_clustering) = getTotalCases(os.path.join("Clustering", "cases.csv"))
    plt.plot(timesteps, [NUM_SUSCEPTIBLE["Clustering"]] * len(timesteps), "g-")
    plt_cases_clustering, = plt.plot(timesteps, cases_clustering, "g-", label="Clustered + Age-dependent immunity")

    plt.legend(handles=[plt_cases_random, plt_cases_agedependent, plt_cases_agedependent2, plt_cases_clustering])

    plt.show()

def main():
    print("Running simulations for Measles In Flanders study")
    runRandom()
    runAgeProfiles()
    runAgeProfiles2()
    runClustering()
    plotNewCasesPerDay()
    plotTotalCases()

if __name__=="__main__":
    main()

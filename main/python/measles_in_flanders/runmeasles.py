#############################################################################
#            Running simulations for 'measles in Flanders' study            #
#############################################################################
import csv
import os

import matplotlib.pyplot as plt

from pystride.Event import EventType
from pystride.PyController import PyController

def trackRandomCases(simulator, event):
    num_cases = simulator.GetPopulation().GetInfectedCount()
    filename = os.path.join("Random", "cases.csv")
    if not os.path.isfile(filename):
        f = open(filename, "w")
        f.close()
    with open(filename, "a", newline='') as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if event.timestep == 0:
            writer.writeheader()
        writer.writerow({'timestep': event.timestep, 'cases': num_cases})

def trackAgeDependentCases(simulator, event):
    num_cases = simulator.GetPopulation().GetInfectedCount()
    filename = os.path.join("AgeDependent", "cases.csv")
    if not os.path.isfile(filename):
        f = open(filename, "w")
        f.close()
    with open(filename, "a", newline='') as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if event.timestep == 0:
            writer.writeheader()
        writer.writerow({'timestep': event.timestep, 'cases': num_cases})


def runRandom():
    """
        A random 80% of the population is immune to measles,
        either due to natural immunity or due to vaccination.
    """
    control = PyController("../config/run_default.xml")
    # control.runConfig.setParameter("contact_log_level", "Susceptibles")

    # Immunity profile
    control.runConfig.setParameter("immunity_profile", "Random")
    control.runConfig.setParameter("immunity_rate", 0.8034)
    # Vaccination profile is empty since random immunity was seeded
    # through immunity profile
    control.runConfig.setParameter("vaccine_link_probability", 0)
    control.runConfig.setParameter("vaccine_profile", "None")
    control.runConfig.setParameter("vaccine_rate", 0.0)

    control.runConfig.setParameter("num_days", 100)
    # control.runConfig.setParameter("num_participants_survey", 10 000)
    control.runConfig.setParameter("output_prefix", "Random")

    control.registerCallback(trackRandomCases, EventType.Stepped)
    control.control()

def runAgeProfiles():
    control = PyController("../config/run_default.xml")
    #control.runConfig.setParameter("contact_log_level", "Susceptibles")
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    #control.runConfig.setParameter("immunity_rate", 0.7)
    control.runConfig.setParameter("immunity_distribution", "../data/measles_natural_immunity.xml")
    control.runConfig.setParameter("num_days", 100)
    # control.runConfig.setParameter("num_participants_survey", 10 000)
    control.runConfig.setParameter("output_prefix", "AgeDependent")
    control.runConfig.setParameter("vaccine_link_probability", 0)
    control.runConfig.setParameter("vaccine_profile", "Random")
    control.runConfig.setParameter("vaccine_distribution", "../data/measles_vaccination.xml")
    #control.runConfig.setParameter("vaccine_rate", 0.0)
    control.registerCallback(trackAgeDependentCases, EventType.Stepped)
    control.control()

def runClustering():
    control = PyController("../config/run_default.xml")
    # control.runConfig.setParameter("contact_log_level", "Susceptibles")
    # control.runConfig.setParameter("immunity_profile", "Random" + 35)
    # control.runConfig.setParameter("immunity_rate", ?)
    control.runConfig.setParameter("num_days", 100)
    # control.runConfig.setParameter("num_participants_survey", 10 000)
    control.runConfig.setParameter("output_prefix", "Clustering")
    # vaccine_link_probability
    # vaccine_profile
    # vaccine_rate
    control.control()

def postProcessing():
    timesteps = []
    cases_random = []
    cases_agedependent = []

    with open(os.path.join("Random", "cases.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timesteps.append(int(row['timestep']))
            cases_random.append(int(row["cases"]))

    with open(os.path.join("AgeDependent", "cases.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            cases_agedependent.append(int(row["cases"]))

    last = int(cases_random[0])
    new_cases_random = []
    for c in cases_random:
        new_cases_random.append(int(c) - last)
        last = int(c)

    last = int(cases_agedependent[0])
    new_cases_agedependent = []
    for c in cases_agedependent:
        new_cases_agedependent.append(int(c) - last)
        last = int(c)

    plt.plot(timesteps, new_cases_random)
    plt.plot(timesteps, new_cases_agedependent)
    plt.legend(["Random immunity", "Age-dependent immunity"])
    plt.show()

def main():
    print("Running simulations for Measles In Flanders study")
    runRandom()
    runAgeProfiles()
    #runClustering()
    postProcessing()

if __name__=="__main__":
    main()

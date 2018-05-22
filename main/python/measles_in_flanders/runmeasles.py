#############################################################################
#            Running simulations for 'measles in Flanders' study            #
#############################################################################
import csv
import os

import matplotlib.pyplot as plt

from pystride.Event import EventType
from pystride.PyController import PyController

############################################################
#                    Callback functions                    #
############################################################

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
        "rng_seed": 1, "rng_type": "mrg2",
        "r0": 17,
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
    control.runConfig.setParameter("immunity_link_probability", 0)
    control.runConfig.setParameter("immunity_profile", "AgeDependent")
    control.runConfig.setParameter("immunity_distribution", "../data/measles_random_immunity.xml")
    # Vaccination profile is empty since random immunity was seeded
    # through immunity profile
    control.runConfig.setParameter("vaccine_profile", "None")

    control.registerCallback(trackRandomCases, EventType.Stepped)
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

def postProcessing():
    (timesteps, new_cases_random) = getNewCasesPerDay(os.path.join("Random", "cases.csv"))
    (timesteps, new_cases_agedependent) = getNewCasesPerDay(os.path.join("AgeDependent", "cases.csv"))
    (timesteps, new_cases_clustered) = getNewCasesPerDay(os.path.join("Clustering", "cases.csv"))

    plt.plot(timesteps, new_cases_random)
    plt.plot(timesteps, new_cases_agedependent)
    plt.plot(timesteps, new_cases_clustered)
    plt.legend(["Random immunity", "Age-dependent immunity", "Clustered + Age-dependent immunity"])
    '''
    # TODO Clustering
    plt.xlabel("Day")
    plt.ylabel("New cases")'''
    plt.xlabel("Day")
    plt.ylabel("New cases")
    plt.show()

def immunityProfile(self):
    ages = range(101)
    susc = [0.02] * 101
    susc[0] = 0.75
    susc[1] = 0.10
    susc[2] = 0.11
    susc[3] = 0.13
    susc[4] = 0.15
    susc[5] = 0.17
    susc[6] = 0.18
    susc[7] = 0.18
    susc[8] = 0.17
    susc[9] = 0.15
    susc[10] = 0.13
    susc[11] = 0.10
    susc[12] = 0.09
    susc[13] = 0.09
    susc[14] = 0.10
    susc[15] = 0.13
    susc[16] = 0.18
    susc[17] = 0.25
    susc[18] = 0.23
    susc[19] = 0.22
    susc[20] = 0.20
    susc[21] = 0.19
    susc[22] = 0.18
    susc[23] = 0.17
    susc[24] = 0.15
    susc[25] = 0.10
    susc[26] = 0.09
    susc[27] = 0.08
    susc[28] = 0.07
    susc[29] = 0.07
    susc[30] = 0.06
    susc[31] = 0.06
    susc[32] = 0.05
    susc[33] = 0.05
    susc[34] = 0.04
    susc[35] = 0.04
    susc[36] = 0.04
    susc[37] = 0.03
    susc[38] = 0.03
    susc[39] = 0.03
    susc[40] = 0.03
    #plt.plot(ages, susc, 'o')
    #plt.show()
    immune = {}
    for i in range(len(susc)):
        immune[i] = round(1 - susc[i], 2)
    print(immune)
    print(float(sum(immune.values())) / float(len(immune.values())))

def main():
    print("Running simulations for Measles In Flanders study")
    runRandom()
    runAgeProfiles()
    runClustering()
    postProcessing()

if __name__=="__main__":
    main()

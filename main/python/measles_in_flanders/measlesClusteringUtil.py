import csv
import os

MAX_AGE = 99

def getCommonParameters():
    parameters = {
        "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
        "contact_log_level": "Transmissions",
        "contact_output_file": "true",
        "disease_config_file": "disease_measles_probability_instead_of_rate.xml",
        "holidays_file": "holidays_none.json",
        "num_participants_survey": 0,
        "num_threads": 1,
        "output_cases": "false",
        "output_persons": "false",
        "output_summary": "false",
        "population_type": "default",
        "rng_type": "mrg2",
        "seeding_age_min": 1,
        "seeding_age_max": 99,
        "seeding_rate": 0.000001667,
        "stride_log_level": "info",
        "use_install_dirs": "true",
    }
    return parameters

################################################################################
# Functions for generating rng seeds.                                          #
################################################################################

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def writeRngSeeds(scenarioName, seeds):
    with open(scenarioName + "_seeds.csv", "w") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

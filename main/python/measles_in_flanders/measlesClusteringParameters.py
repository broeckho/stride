import os

def getCommonParameters():
    parameters = {
        "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
        "contact_log_level": "Transmissions",
        "contact_output_file": "true",
        "disease_config_file": "disease_measles.xml",
        "holidays_file": "holidays_none.json",
        "immunity_link_probability": 0,
        "immunity_profile": "AgeDependent",
        "num_participants_survey": 0,
        "num_threads": 1,
        "output_cases": "false",
        "output_persons": "false",
        "output_summary": "false",
        "population_file": "pop_flanders600.csv",
        "population_type": "default",
        "rng_type": "mrg2",
        "seeding_age_min": 1,
        "seeding_age_max": 99,
        "seeding_rate": 0.000001667,
        "start_date": "2019-01-01",
        "stride_log_level": "info",
        "use_install_dirs": "true",
        "vaccine_profile": "AgeDependent",
    }
    return parameters

def getScenarioParameters(scenarioName, immunityFileChildren, immunityFileAdults):
    parameters = {
        "UNIFORM_NOCLUSTERING": {
            "immunity_distribution_file": os.path.join("data", "measles_uniform_adult_immunity.xml"),
            "vaccine_distribution_file": os.path.join("data", "measles_uniform_child_immunity.xml"),
            "vaccine_link_probability": 0,
        },
        "AGEDEPENDENT_NOCLUSTERING": {
            "immunity_distribution_file": immunityFileAdults,
            "vaccine_distribution_file": immunityFileChildren,
            "vaccine_link_probability": 0,
        },
        "UNIFORM_CLUSTERING": {
            "immunity_distribution_file": os.path.join("data", "measles_uniform_adult_immunity.xml"),
            "vaccine_distribution_file": os.path.join("data", "measles_uniform_child_immunity.xml"),
            "vaccine_link_probability": 1,
        },
        "AGEDEPENDENT_CLUSTERING": {
            "immunity_distribution_file": immunityFileAdults,
            "vaccine_distribution_file": immunityFileChildren,
            "vaccine_link_probability": 1,
        },
    }
    return parameters[scenarioName]

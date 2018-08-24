import argparse
import csv
import os

from pystride.Event import Event, SteppedEvent, EventType
from pystride.PyController import PyController

SUSCEPTIBLES_AT_START = {}

COMMON_PARAMS = {
    'age_contact_matrix_file': 'contact_matrix_flanders_subpop.xml',
    'behaviour_policy': 'NoBehaviour',
    'belief_policy.name': 'NoBelief',
    'contact_log_level': 'None',
    'contact_output_file': 'false',
    'disease_config_file': 'disease_measles.xml',
    'global_information_policy': 'NoGlobalInformation',
    'holidays_file': 'holidays_none.json',
    'local_information_policy': 'NoLocalInformation',
    'num_days': 365,
    'num_participants_survey': 0,
    'num_threads': 4,
    'output_adopted': 'false',
    'output_cases': 'false',
    'output_persons': 'false',
    'output_summary': 'false',
    'population_file': 'pop_flanders600.csv',
    'rng_type': 'mrg2',
    'r0': 12,
    'seeding_age_min': 1,
    'seeding_age_max': 99,
    'seeding_rate': 0.000001667,
    'start_date': '2017-01-01',
    'stride_log_level': 'info',
    'track_index_case': 'false',
    'use_install_dirs': 'true'
}

RANDOM_PARAMS = {
    'immunity_link_probability': 0,
    'immunity_profile': 'AgeDependent',
    'immunity_distribution': 'data/measles_random_immunity.xml',
    'vaccine_profile': 'None'
}

AGE_CLUSTERING_PARAMS = {
    'immunity_link_probability': 0,
    'immunity_profile': 'AgeDependent',
    'immunity_distribution': 'data/measles_adult_immunity.xml',
    'vaccine_link_probability': 0,
    'vaccine_profile': 'AgeDependent',
    'vaccine_distribution': 'data/measles_child_immunity.xml'
}

HOUSEHOLD_CLUSTERING_PARAMS = {
    'immunity_link_probability': 1,
    'immunity_profile': 'AgeDependent',
    'immunity_distribution': 'data/measles_random_immunity.xml',
    'vaccine_profile': 'None'
}

AGE_HOUSEHOLD_CLUSTERING_PARAMS = {
    'immunity_link_probability': 0,
    'immunity_profile': 'AgeDependent',
    'immunity_distribution': 'data/measles_adult_immunity.xml',
    'vaccine_link_probability': 1,
    'vaccine_profile': 'AgeDependent',
    'vaccine_distribution': 'data/measles_child_immunity.xml'
}

PARAMS = {
'Random': RANDOM_PARAMS,
'AgeClustering': AGE_CLUSTERING_PARAMS,
'HouseholdClustering': HOUSEHOLD_CLUSTERING_PARAMS,
'AgeAndHouseholdClustering': AGE_HOUSEHOLD_CLUSTERING_PARAMS
}

def setParams(control, params):
    for paramName, paramValue in params.items():
        control.runConfig.setParameter(paramName, paramValue)

def getHouseholdConsitution(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    households = {}
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        hhId = pop[i].GetHouseholdId()
        age = pop[i].GetAge()
        susceptible = pop[i].GetHealth().IsSusceptible()
        if hhId in households:
            households[hhId].append({'age': age, 'susceptible': susceptible})
        else:
            households[hhId] = [{'age': age, 'susceptible': susceptible}]

def getNumSusceptible(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    numSusceptible = 0
    pop = simulator.GetPopulation()
    for i in range(pop.size()):
        if pop[i].GetHealth().IsSusceptible():
            numSusceptible += 1
    global SUSCEPTIBLES_AT_START
    SUSCEPTIBLES_AT_START[outputPrefix] = numSusceptible

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

def runSimulation(scenarioName, seed):
    print("Running " + scenarioName + " with seed " + str(seed))
    # Standard configuration
    control = PyController(data_dir="data")
    setParams(control, COMMON_PARAMS)
    setParams(control, PARAMS[scenarioName])
    # Set output prefix and rng seed
    setParams(control,
        {'output_prefix': scenarioName + "_" + str(seed), 'rng_seed': seed})
    # Register callbacks
    control.registerCallback(getNumSusceptible, EventType.AtStart)
    control.registerCallback(getHouseholdConsitution, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    # Run simulation
    control.control()

def main(numRuns, scenarioNames):
    # Generate random number seeds for runs
    seeds = []
    for i in range(numRuns):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder="big"))
    # Run numRuns simulations for each scenario
    for scenario in scenarioNames:
        for i in range(numRuns):
            runSimulation(scenario, seeds[i])
    print(SUSCEPTIBLES_AT_START)

if __name__=="__main__":
    # TODO multiple R0 values
    # TODO vary disease counter in index case?
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=10, help='Number of simulation runs per scenario')
    parser.add_argument("--scenarioNames", type=str, nargs='+',
        default=['Random'])
    args = parser.parse_args()
    main(args.numRuns, args.scenarioNames)

import argparse
import csv
import multiprocessing
import os
import time

from pystride.Event import Event, EventType
from pystride.PyController import PyController

IMMUNITY_RATES = {
    2013: 0.9161450000000001,
    2020: 0.9081133333333328,
    2025: 0.8891299999999993,
    2030: 0.8666783333333348,
    2035: 0.8457200000000000,
    2040: 0.8290033333333328,
}

# Callback function to register person's age and immunity status
# at the beginning of the simulation
def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue('run.output_prefix')
    pop = simulator.GetPopulation()
    totalPopulation = 0
    totalSusceptible = 0
    with open(os.path.join(outputPrefix, 'susceptibles.csv'), 'w') as csvfile:
        fieldnames = ["age", "susceptible"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        # Write header
        writer.writeheader()
        # Write susceptibility info per person
        for i in range(pop.size()):
            totalPopulation += 1
            person = pop[i]
            age = person.GetAge()
            isSusceptible = person.GetHealth().IsSusceptible()
            if isSusceptible:
                isSusceptible = 1
                totalSusceptible += 1
            else:
                isSusceptible = 0
            writer.writerow({"age": age, "susceptible": isSusceptible})

# Callback function to track the cumulative cases
# at each timestep
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

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def writeSeeds(year, R0, seeds):
    fileName = str(year) + "_R0_" + str(R0) + "_seeds.csv"
    with open(fileName, "w") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

def runSimulation(year, R0, seed):
    configFile = os.path.join("config", "ageImmunityShift.xml")
    control = PyController(data_dir="data")
    control.loadRunConfig(configFile)
    control.runConfig.setParameter("output_prefix",
                                    str(year) + "_R0_" + str(R0) + "_" + str(seed))
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("r0", R0)
    control.runConfig.setParameter("immunity_profile", "Random")
    control.runConfig.setParameter("immunity_rate", IMMUNITY_RATES[year])
    control.registerCallback(registerSusceptibles, EventType.AtStart)
    control.registerCallback(trackCases, EventType.Stepped)
    control.control()
    return

def runSimulations(numRuns, year, R0, poolSize):
    seeds = generateRngSeeds(numRuns)
    writeSeeds(year, R0, seeds)
    with multiprocessing.Pool(processes=poolSize) as pool:
        pool.starmap(runSimulation, [(year, R0, s) for s in seeds])

def main(numRuns, years, R0s, poolSize):
    start = time.perf_counter()
    for year in years:
        for R0 in R0s:
            print("Running simulations for {} with R0 {}".format(year, R0))
            runSimulations(numRuns, year, R0, poolSize)
    end = time.perf_counter()
    totalTimeSeconds = end - start
    totalTimeMinutes = totalTimeSeconds / 60
    totalTimeHours = totalTimeMinutes / 60
    print("Total time: {} seconds or {} minutes or {} hours".format(totalTimeSeconds,
                                                                totalTimeMinutes,
                                                                totalTimeHours))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of simulation runs per scenario")
    parser.add_argument("--years", type=int, nargs="+", default=[2013, 2020], help="From which projected year the immunity data should be used")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 13], help="Basic reproduction numbers to simulate for")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.numRuns, args.years, args.R0s, args.poolSize)

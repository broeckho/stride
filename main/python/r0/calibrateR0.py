import argparse
import math
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os

from numpy.polynomial.polynomial import polyfit
from pystride.PyController import PyController

# [  1.48947087  27.38215366  -1.57265546]

def getCommonParameters():
    # TODO adaptive symptomatic behavior?
    parameters = {
        "contact_log_level": "Transmissions",
        "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
        "contact_output_file": "true",
        "disease_config_file": "disease_measles.xml",
        "immunity_profile": "None",
        "holidays_file": "holidays_none.json",
        "num_days": 25,
        "num_index_cases": 1,
        "num_participants_survey": 0,
        "num_threads": 1,
        "output_cases": "false",
        "output_persons": "false",
        "output_summary": "false",
        "population_file": "pop_flanders600.csv",
        "rng_type": "mrg2",
        "seeding_age_max": 99,
        "seeding_age_min": 1,
        "stride_log_level": "info",
        "track_index_case": "true",
        "use_install_dirs": "true",
        "vaccine_profile": "None",
    }
    return parameters

def probabilityToRate(p):
    if p < 1:
        return - math.log(1 - p)

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def runSimulation(startDate, transmissionRate, seed, runID):
    control = PyController(data_dir="data")
    for paramName, paramValue in getCommonParameters().items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("transmission_rate", transmissionRate)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("output_prefix", "TR_" + str(transmissionRate) + "_START_" + startDate + "_" + str(runID))
    control.runConfig.setParameter("start_date", startDate)
    control.control()

def runSimulations(numRuns, startDates, transmissionRates, poolSize):
    for transmissionRate in transmissionRates:
        for date in startDates:
            seeds = generateRngSeeds(numRuns)
            with multiprocessing.Pool(processes=poolSize) as pool:
                pool.starmap(runSimulation, [(date, transmissionRate, seeds[i], i) for i in range(len(seeds))])

def getSecondaryCases(transmissionRate, startDate, runID):
    secondaryCases = 0
    contactFile = "TR_" + str(transmissionRate) + "_START_" + startDate + "_" + str(runID) + "_contact_log.txt"
    with open(contactFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                secondaryCases += 1
    return secondaryCases

def analyseResults(numRuns, startDates, transmissionRates, poolSize):
    allTransmissionRates = []
    allSecondaryCases = []
    secondaryCasesByTR = []
    for rate in transmissionRates:
        colors = ["b", "g", "y", "k", "m", "c", "r"]
        days = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
        secondaryCasesTR = []
        for date_i in range(len(startDates)):
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCases, [(rate, startDates[date_i], i) for i in range(numRuns)])
                secondaryCasesTR += secondaryCases
                allSecondaryCases += secondaryCases
                allTransmissionRates += ([rate] * numRuns)
                plt.plot([rate] * numRuns, secondaryCases, colors[date_i] + "o")
        secondaryCasesByTR.append(secondaryCasesTR)
    newCoefficients = polyfit(allTransmissionRates, allSecondaryCases, 2)
    print(newCoefficients)
    plt.xlabel("Transmission rate")
    plt.ylabel("Secondary cases")
    plt.legend(days)
    plt.savefig("TransmissionRateVSSecCases")
    plt.clf()

    plt.boxplot(secondaryCasesByTR)
    plt.show()

def main(numRuns, poolSize):
    transmissionProbs = numpy.arange(0, 1, 0.05)
    transmissionRates = [probabilityToRate(x) for x in transmissionProbs]
    startDates = ["2020-01-01", "2020-01-02", "2020-01-03", "2020-01-04", "2020-01-05", "2020-01-06", "2020-01-07"]
    runSimulations(numRuns, startDates, transmissionRates, poolSize)
    analyseResults(numRuns, startDates, transmissionRates, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of iterations per scenario.")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool.")
    args = parser.parse_args()
    main(args.numRuns, args.poolSize)
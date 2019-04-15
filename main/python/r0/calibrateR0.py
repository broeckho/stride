import argparse
import math
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os
import statistics

import scipy.optimize

from pystride.PyController import PyController

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

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds


def runSimulation(startDate, transmissionProbability, seed, runID):
    control = PyController(data_dir="data")
    for paramName, paramValue in getCommonParameters().items():
        control.runConfig.setParameter(paramName, paramValue)
    control.runConfig.setParameter("transmission_probability", transmissionProbability)
    control.runConfig.setParameter("rng_seed", seed)
    control.runConfig.setParameter("output_prefix", "TP_" + str(transmissionProbability) + "_START_" + startDate + "_" + str(runID))
    control.runConfig.setParameter("start_date", startDate)
    control.control()

def runSimulations(numRuns, startDates, transmissionProbabilities, poolSize):
    for transmissionProb in transmissionProbabilities:
        for date in startDates:
            seeds = generateRngSeeds(numRuns)
            with multiprocessing.Pool(processes=poolSize) as pool:
                pool.starmap(runSimulation, [(date, transmissionProb, seeds[i], i) for i in range(len(seeds))])

def getSecondaryCases(transmissionProbability, startDate, runID):
    secondaryCases = 0
    contactFile = "TP_" + str(transmissionProbability) + "_START_" + startDate + "_" + str(runID) + "_contact_log.txt"
    with open(contactFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                secondaryCases += 1
    return secondaryCases

'''def lnFunc(xs, a, b):
    return [a + b * math.log(x + 1) for x in xs]'''

def analyseResults(numRuns, startDates, transmissionProbabilities, poolSize):
    secondaryCasesByProb = []
    secondaryCasesMeans = []
    secondaryCasesMedians = []

    allTransmissionProbs = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        colors = ["b", "g", "y", "k", "m", "c", "r"]
        days = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
        for date_i in range(len(startDates)):
            secondaryCases = []
            with multiprocessing.Pool(processes=poolSize) as pool:
                results = pool.starmap(getSecondaryCases, [(prob, startDates[date_i], i) for i in range(numRuns)])
                secondaryCases += results
                allSecondaryCases += results
                allTransmissionProbs += ([prob] * numRuns)
                plt.plot([prob] * numRuns, results, colors[date_i] + "o")
        secondaryCasesByProb.append(secondaryCases)
        secondaryCasesMeans.append(sum(secondaryCases) / len(secondaryCases))
        secondaryCasesMedians.append(statistics.median(secondaryCases))

    plt.xlabel("Transmission probability")
    plt.ylabel("Secondary cases")
    plt.legend(days)
    plt.savefig("SecondaryCasesScatter")
    plt.clf()

    bp = plt.boxplot(secondaryCasesByProb, labels=transmissionProbabilities)
    line1, = plt.plot(range(1, len(transmissionProbabilities) + 1), secondaryCasesMeans)
    line2, = plt.plot(range(1, len(transmissionProbabilities) + 1), secondaryCasesMedians)
    plt.xlabel("Transmission probability")
    plt.ylabel("Secondary cases")
    plt.legend([line1, line2],["Means", "Medians"])
    plt.savefig("SecondaryCasesBoxplots")
    plt.clf()

    '''
    fit = scipy.optimize.curve_fit(lnFunc, allTransmissionProbs, allSecondaryCases)
    ffunc = lnFunc(transmissionProbabilities, -0.43092477, 39.78475362)
    plt.plot(transmissionProbabilities, ffunc, color="red")

    errors = []
    for i in range(len(allTransmissionProbs)):
        model = -0.43092477 + (39.7847536 * math.log(allTransmissionProbs[i] + 1))
        e =  model - allSecondaryCases[i]
        errors.append(e)
    plt.plot(allTransmissionProbs, errors, "ro")'''

def main(numRuns, poolSize):
    transmissionProbabilities = numpy.arange(0, 1.05, 0.05)
    startDates = ["2020-01-01", "2020-01-02", "2020-01-03", "2020-01-04", "2020-01-05", "2020-01-06", "2020-01-07"]
    runSimulations(numRuns, startDates, transmissionProbabilities, poolSize)
    analyseResults(numRuns, startDates, transmissionProbabilities, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of iterations per scenario.")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool.")
    args = parser.parse_args()
    main(args.numRuns, args.poolSize)

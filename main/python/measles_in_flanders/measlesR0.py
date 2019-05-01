import argparse
import math
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import scipy.optimize
import time

from measlesClusteringUtil import generateRngSeeds

from pystride.PyController import PyController

def runSimulation(transmissionProbability, seed, runID):
    control = PyController(data_dir="data")
    parameters = {
        "age_contact_matrix_file": "contact_matrix_flanders_subpop.xml",
        "contact_log_level": "Transmissions",
        "contact_output_file": "true",
        "disease_config_file": "disease_measles_probability_instead_of_rate.xml",
        "holidays_file": "holidays_none.json",
        "immunity_profile": "None",
        "num_days": 30,
        "num_participants_survey": 0,
        "num_threads": 1,
        "output_cases": "false",
        "output_persons": "false",
        "output_prefix": "r0_TP_" + str(transmissionProbability) + "_" + str(runID),
        "output_summary": "false",
        "population_file": "pop_flanders600.csv",
        "population_type": "default",
        "rng_type": "mrg2",
        "rng_seed": seed,
        "seeding_age_min": 1,
        "seeding_age_max": 99,
        "seeding_rate": 0.000001667,
        "start_date": "2020-01-01",
        "stride_log_level": "info",
        "track_index_case": "true",
        "transmission_probability": transmissionProbability,
        "use_install_dirs": "true",
        "vaccine_profile": "None",
    }
    for paramName, paramValue in parameters.items():
        control.runConfig.setParameter(paramName, paramValue)
    control.control()

def runSimulations(numRuns, transmissionProbabilities, poolSize):
    for prob in transmissionProbabilities:
        seeds = generateRngSeeds(numRuns)
        with multiprocessing.Pool(processes=poolSize) as pool:
            pool.starmap(runSimulation, [(prob, seeds[i], i) for i in range(len(seeds))])

def getSecondaryCases(transmissionProbability, runID):
    transmissionsFile = "r0_TP_" + str(transmissionProbability)  + "_" + str(runID) + "_contact_log.txt"
    secondaryCases = 0
    with open(transmissionsFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                secondaryCases += 1
    return secondaryCases

def lnFunc(xs, a, b):
    return [a + b * math.log(x + 1) for x in xs]

def analyzeResults(numRuns, transmissionProbabilities, poolSize):
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCases, [(prob, i) for i in range(numRuns)])
            plt.plot([prob] * numRuns, secondaryCases, "bo")
            allSecondaryCases.append(secondaryCases)

    fit = scipy.optimize.curve_fit(lnFunc, [x for x in transmissionProbabilities for i in range(numRuns)], [y for x in allSecondaryCases for y in x])
    print(fit[0])

    plt.xlabel("P(transmission)")
    plt.ylabel("Secondary cases")
    fitFunc = lnFunc(transmissionProbabilities, fit[0][0], fit[0][1])
    line1, = plt.plot(transmissionProbabilities, fitFunc, color="red", label="{:.3f} + {:.3f} * ln(1+ x)".format(fit[0][0], fit[0][1]))
    plt.legend(handles=[line1])
    plt.savefig("SecondaryCases_scatter.png")
    plt.clf()

    plt.boxplot(allSecondaryCases, labels=transmissionProbabilities)
    plt.xlabel("P(transmission)")
    plt.ylabel("Secondary cases")
    plt.savefig("SecondaryCases_boxplot.png")
    plt.clf()

def main(numRuns, poolSize):
    start = time.perf_counter()
    transmissionProbabilities = numpy.arange(0, 1.05, 0.05)
    runSimulations(numRuns, transmissionProbabilities, poolSize)
    analyzeResults(numRuns, transmissionProbabilities, poolSize)
    end = time.perf_counter()
    print("Total time {} seconds".format(end - start))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=10, help="Number of runs per scenario")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.numRuns, args.poolSize)

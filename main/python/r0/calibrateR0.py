import argparse
import csv
import datetime
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import os
import statistics

from pystride.PyController import PyController

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

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

def dateToDayOfWeek(date):
    weekDays = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
    pyDate = datetime.datetime.strptime(date, "%Y-%m-%d")
    return weekDays[pyDate.weekday()]

def getHHSizes(popFile):
    hhSizes = {}
    with open(popFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            hhID = row["household_id"]
            if hhID in hhSizes:
                hhSizes[hhID] += 1
            else:
                hhSizes[hhID] = 1
    return hhSizes

def getSecondaryCases(startDate, transmissionProbability, runID, hhSizes):
    secondaryCases = 0
    indexCaseAge = -1
    indexCaseHHSize = -1
    dayOfWeek = dateToDayOfWeek(startDate)
    contactFile = "TP_" + str(transmissionProbability) + "_START_" + startDate + "_" + str(runID) + "_contact_log.txt"
    with open(contactFile) as f:
        for line in f:
            line = line.split(" ")
            if line[0] == "[TRAN]":
                secondaryCases += 1
            elif line[0] == "[PRIM]":
                indexCaseAge = line[3]
                indexCaseHHSize = hhSizes[line[7][:-1]]
    return {"secondary_cases": secondaryCases,
                "transmission_probability": transmissionProbability,
                "start_day": dayOfWeek,
                "index_case_age": indexCaseAge,
                "index_case_hh_size": indexCaseHHSize}

def plotCasesVSTransmissionProbability(secondaryCases):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    secondaryCasesByTP = {}
    for run in secondaryCases:
        transmissionProbability = run["transmission_probability"]
        cases = run["secondary_cases"]
        allTransmissionProbabilities.append(transmissionProbability)
        allSecondaryCases.append(cases)
        if transmissionProbability in secondaryCasesByTP:
            secondaryCasesByTP[transmissionProbability].append(cases)
        else:
            secondaryCasesByTP[transmissionProbability] = [cases]
    plt.plot(allTransmissionProbabilities, allSecondaryCases, "co")
    transmissionProbabilities = list(secondaryCasesByTP.keys())
    transmissionProbabilities.sort()
    means = []
    medians = []
    for prob in transmissionProbabilities:
        means.append(sum(secondaryCasesByTP[prob]) / len(secondaryCasesByTP[prob]))
        medians.append(statistics.median(secondaryCasesByTP[prob]))
    line1, = plt.plot(transmissionProbabilities, means, color="red", label="Mean")
    line2, = plt.plot(transmissionProbabilities, medians, color="blue", label="Median")
    plt.xlabel("Transmission probability")
    plt.ylabel("Secondary cases")
    plt.legend(handles=[line1, line2])
    plt.savefig("CasesVSTransmissionProbability")
    plt.clf()

def plotCasesVSDayOfWeek(secondaryCases):
    allSecondaryCases = {
        "Monday": [],
        "Tuesday": [],
        "Wednesday": [],
        "Thursday": [],
        "Friday": [],
        "Saturday": [],
        "Sunday": []
    }
    for run in secondaryCases:
        startDay = run["start_day"]
        cases = run["secondary_cases"]
        allSecondaryCases[startDay].append(cases)
    weekDays = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
    means = []
    medians = []
    for i in range(len(weekDays)):
        plt.plot([i] * len(allSecondaryCases[weekDays[i]]), allSecondaryCases[weekDays[i]], "co")
        means.append(sum(allSecondaryCases[weekDays[i]]) / len(allSecondaryCases[weekDays[i]]))
        medians.append(statistics.median(allSecondaryCases[weekDays[i]]))

    plt.xticks(range(len(weekDays)), weekDays)
    line1, = plt.plot(range(len(weekDays)), means, color="red", label="Mean")
    line2, = plt.plot(range(len(weekDays)), medians, color="blue", label="Median")

    plt.xlabel("Start day of simulation")
    plt.ylabel("Secondary cases")
    plt.legend(handles=[line1, line2])
    plt.savefig("CasesVSDayOfWeek")
    plt.clf()

def plotCasesVSAgeIndexCase(secondaryCases):
    allAges = {}
    for age in range(100):
        allAges[age] = []
    for run in secondaryCases:
        age = int(run["index_case_age"])
        cases = run["secondary_cases"]
        allAges[age].append(cases)
    agesWithData = []
    means = []
    medians = []
    for age in range(100):
        plt.plot([age] * len(allAges[age]), allAges[age], "co")
        if len(allAges[age]) > 0:
            agesWithData.append(age)
            means.append(sum(allAges[age]) / len(allAges[age]))
            medians.append(statistics.median(allAges[age]))
    line1, = plt.plot(agesWithData, means, color="red", label="Mean")
    line2, = plt.plot(agesWithData, medians, color="blue", label="Median")
    plt.xlabel("Age of index case")
    plt.ylabel("Secondary cases")
    plt.legend(handles=[line1, line2])
    plt.savefig("CasesVSAgeIndexCase")
    plt.clf()

def plotCasesVSHHSizeIndexCase(secondaryCases):
    allHHSizes = {}
    for run in secondaryCases:
        hhSize = run["index_case_hh_size"]
        cases = run["secondary_cases"]
        if hhSize in allHHSizes:
            allHHSizes[hhSize].append(cases)
        else:
            allHHSizes[hhSize] = [cases]
    hhSizes = list(allHHSizes.keys())
    hhSizes.sort()
    means = []
    medians = []
    for size in hhSizes:
        plt.plot([size] * len(allHHSizes[size]), allHHSizes[size], "co")
        means.append(sum(allHHSizes[size]) / len(allHHSizes[size]))
        medians.append(statistics.median(allHHSizes[size]))
    line1, = plt.plot(hhSizes, means, color="red", label="Mean")
    line2, = plt.plot(hhSizes, medians, color="blue", label="Median")
    plt.xlabel("Household size of index case")
    plt.ylabel("Secondary cases")
    plt.legend(handles=[line1, line2])
    plt.savefig("CasesVSHHSizeIndexCase")
    plt.clf()

def analyseResults(numRuns, startDates, transmissionProbabilities, poolSize):
    secondaryCases = []
    hhSizes = getHHSizes(os.path.join("data", "pop_flanders600.csv"))
    for prob in transmissionProbabilities:
        for date in startDates:
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases += pool.starmap(getSecondaryCases, [(date, prob, i, hhSizes) for i in range(numRuns)])
    plotCasesVSTransmissionProbability(secondaryCases)
    plotCasesVSDayOfWeek(secondaryCases)
    plotCasesVSAgeIndexCase(secondaryCases)
    plotCasesVSHHSizeIndexCase(secondaryCases)

def main(numRuns, poolSize):
    transmissionProbabilities = numpy.arange(0, 1.05, 0.05)
    startDates = ["2020-01-01", "2020-01-02", "2020-01-03", "2020-01-04", "2020-01-05", "2020-01-06", "2020-01-07"]
    # Run simulations
    runSimulations(numRuns, startDates, transmissionProbabilities, poolSize)
    # Analyse results
    analyseResults(numRuns, startDates, transmissionProbabilities, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=5, help="Number of stochastic runs per scenario")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.numRuns, args.poolSize)


"""
import math
import statistics

import scipy.optimize

'''def lnFunc(xs, a, b):
    return [a + b * math.log(x + 1) for x in xs]'''

def analyseResults(numRuns, startDates, transmissionProbabilities, poolSize):
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

'''
def createEffectiveROverviewPlot(outputDir, R0s, scenarioNames, scenarioDisplayNames,
    fractionSusceptibles, poolSize, xLabel, figName, stat="mean"):
    ax = plt.axes(projection="3d")
    colors = ["blue", "orange", "pink", "green", "red", "purple", "brown", "magenta", "cyan", "firebrick"]
    expectedRs = []
    z = 0
    for R0 in R0s:
        expectedR = getExpectedR(R0, fractionSusceptibles)
        expectedRs.append(expectedR)
        results = []
        for scenario in scenarioNames:
            scenarioName = str(scenario) + "_R0_" + str(R0)
            seeds = getRngSeeds(outputDir, scenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenarioName, s) for s in seeds])
                if stat == "mean":
                    results.append(sum(effectiveRs) / len(effectiveRs))
                elif stat == "median":
                    results.append(median(effectiveRs))
                else:
                    print("No valid statistic supplied!")
        ax.plot(range(len(results)), [expectedR] * len(results), zs=R0, zdir="y", color=colors[z])
        ax.bar(range(len(results)), results, zs=R0, zdir="y", color=colors[z], alpha=0.4)
        z+= 1
    ax.set_xlabel(xLabel)
    ax.set_xticks(range(len(scenarioNames)))
    ax.set_xticklabels(scenarioDisplayNames)
    ax.set_ylabel(r'$R_0$')
    ax.set_yticks(R0s)
    ax.set_xlim(R0s[0] - 1, R0s[-1] + 1)
    ax.set_zlabel("Effective R ({})".format(stat))
    ax.set_zlim(0, max(results))
    plt.legend(["Expected R = {0:.3f}".format(r) for r in expectedRs])
    saveFig(outputDir, figName, "png")
'''

"""

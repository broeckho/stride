import matplotlib.pyplot as plt
import multiprocessing
import numpy as np
import scipy.optimize
import statistics

from .Util import COLORS
from .Util import getRngSeeds, getSecondaryCasesFromIndex, saveFig

def lnFunc(xs, a, b):
    return [a + b * np.log(x + 1) for x in xs]

def getLnFit(outputDir, transmissionProbabilities, poolSize):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCasesFromIndex, [(outputDir, scenarioName, s) for s in seeds])
            allSecondaryCases += secondaryCases
            allTransmissionProbabilities += ([prob] * len(secondaryCases))
    popt, pcov = scipy.optimize.curve_fit(lnFunc, allTransmissionProbabilities, allSecondaryCases)
    # TODO heteroschedasticity?
    return popt

def createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir, transmissionProbabilities, poolSize):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCasesFromIndex, [(outputDir, scenarioName, s) for s in seeds])
            allSecondaryCases += secondaryCases
            allTransmissionProbabilities += ([prob] * len(secondaryCases))
    plt.plot(allTransmissionProbabilities, allSecondaryCases, "bo")
    plt.xlabel("Transmission probability")
    plt.xlim(0, 1)
    plt.ylabel("Secondary cases")
    plt.ylim(0, max(allSecondaryCases) + 2)
    saveFig(outputDir, "SecondaryCases_scatter")

def createTransmissionProbabilityVSSecondaryCasesBoxplots(outputDir, transmissionProbabilities, poolSize):
    secondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases.append(pool.starmap(getSecondaryCasesFromIndex, [(outputDir, scenarioName, s) for s in seeds]))
    plt.boxplot(secondaryCases, labels=transmissionProbabilities)
    plt.xlabel("Transmission probability")
    plt.xticks(rotation=45)
    plt.ylabel("Secondary cases")
    plt.tight_layout()
    saveFig(outputDir, "SecondaryCases_boxplots")

def createComparisonPlot(outputDir, transmissionProbabilities, coeffA, coeffB, poolSize):
    fit = lnFunc(transmissionProbabilities, coeffA, coeffB)
    means = []
    medians = []
    lower = []
    upper = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCasesFromIndex, [(outputDir, scenarioName, s) for s in seeds])
            mean = sum(secondaryCases) / len(secondaryCases)
            means.append(mean)
            median = statistics.median(secondaryCases)
            medians.append(median)
            ll = numpy.percentile(secondaryCases, 2.5)
            lower.append(ll)
            hh = numpy.percentile(secondaryCases, 97.5)
            upper.append(hh)

    plt.fill_between(transmissionProbabilities, lower, upper, color="lightgrey")
    plt.plot(transmissionProbabilities, means, color=COLORS[1],linestyle='-')
    plt.plot(transmissionProbabilities, medians, color=COLORS[2], linestyle='--')
    plt.plot(transmissionProbabilities, fit, color=COLORS[3], linestyle='-.')
    plt.xlabel("Transmission probability")
    plt.xlim(0, 1)
    plt.ylabel("Secondary cases")
    plt.legend(["Mean", "Median", "{0:.2f} + {0:.2f} * log(1 + x)".format(coeffA, coeffB)])
    saveFig(outputDir, "SecondaryCases_comparison")

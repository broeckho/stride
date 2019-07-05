import math
import matplotlib.pyplot as plt
import multiprocessing
import numpy
import scipy.optimize

from .Util import getSecondaryCases, getRngSeeds, saveFig, lnFunc

def getLnFit(outputDir, transmissionProbabilities, poolSize):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCases, [(outputDir, scenarioName, s) for s in seeds])
            allSecondaryCases += secondaryCases
            allTransmissionProbabilities += ([prob] * len(secondaryCases))
    popt, pcov = scipy.optimize.curve_fit(lnFunc, allTransmissionProbabilities, allSecondaryCases, method="lm")
    # TODO check quality of fit
    return popt

def createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir, scenarioName,
    transmissionProbabilities, r0CoeffA, r0CoeffB, poolSize):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCases, [(outputDir, scenarioName, s) for s in seeds])
            allSecondaryCases += secondaryCases
            allTransmissionProbabilities += ([prob] * len(secondaryCases))
    plt.plot(allTransmissionProbabilities, allSecondaryCases, "bo")
    fitLine, = plt.plot(transmissionProbabilities, lnFunc(transmissionProbabilities,
                            r0CoeffA, r0CoeffB), color="red",
                            label="{:.2f} + {:.2f} * ln(1 + x)".format(r0CoeffA, r0CoeffB))
    plt.xlabel("Transmission probability")
    plt.ylabel("Secondary cases")
    plt.ylim(0, max(allSecondaryCases))
    plt.legend(handles=[fitLine])
    saveFig(outputDir, "SecondaryCases_scatter")

def createTransmissionProbabilityVSSecondaryCasesBoxplots(outputDir, scenarioNames, transmissionProbabilities, poolSize):
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            allSecondaryCases.append(pool.starmap(getSecondaryCases, [(outputDir, scenarioName, s) for s in seeds]))
    plt.boxplot(allSecondaryCases, labels=transmissionProbabilities)
    plt.xlabel("Transmission probability")
    plt.ylabel("Secondary cases")
    saveFig(outputDir, "SecondaryCases_boxplots")

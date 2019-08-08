import matplotlib.pyplot as plt
import multiprocessing
import numpy
import scipy.optimize

from .Util import getRngSeeds, getSecondaryCasesFromIndex, saveFig

def lnFunc(xs, a, b):
    return [a + b * numpy.log(x + 1) for x in xs]

def getLnFit(outputDir, transmissionProbabilities, poolSize):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCasesFromIndex,
                                    [(outputDir, scenarioName, s) for s in seeds])
            allSecondaryCases += secondaryCases
            allTransmissionProbabilities += ([prob] * len(secondaryCases))
    popt, pcov = scipy.optimize.curve_fit(lnFunc, allTransmissionProbabilities, allSecondaryCases)
    # TODO check quality of fit
    # TODO heteroschedasticity?
    return popt

def createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir,
    transmissionProbabilities, coeffA, coeffB, poolSize):
    allTransmissionProbabilities = []
    allSecondaryCases = []
    for prob in transmissionProbabilities:
        scenarioName = "r0_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, scenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            secondaryCases = pool.starmap(getSecondaryCasesFromIndex,
                                    [(outputDir, scenarioName, s) for s in seeds])
            allSecondaryCases += secondaryCases
            allTransmissionProbabilities += ([prob] * len(secondaryCases))
    plt.plot(allTransmissionProbabilities, allSecondaryCases, "bo")
    fitLine, = plt.plot(transmissionProbabilities, lnFunc(transmissionProbabilities, coeffA, coeffB),
                        color="red", label="{:.2f} + {:.2f} * ln(1 + x)".format(coeffA, coeffB))
    plt.xlabel("Transmisison probability")
    plt.ylabel("Secondary cases")
    plt.ylim(0, max(allSecondaryCases) + 2)
    plt.legend(handles=[fitLine])
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

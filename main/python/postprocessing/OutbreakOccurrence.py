import math
import matplotlib.pyplot as plt
import multiprocessing
import numpy

from mpl_toolkits.mplot3d import Axes3D

from .Util import getFinalOutbreakSize, getRngSeeds, saveFig

def calculateSE(fractionSuccesses, sampleSize, confidenceLevel=95):
    zValues = {90: 1.645, 95: 1.96, 99: 2.576}
    # There should be at least 5 successes and 5 failures for this formula
    # to be appropriate
    numSuccesses = fractionSuccesses * sampleSize
    numFailures = sampleSize - numSuccesses
    if numSuccesses >= 5 and numFailures >= 5:
        z = zValues[confidenceLevel]
        se = z * math.sqrt((fractionSuccesses * (1 - fractionSuccesses)) / sampleSize)
        return se
    else:
        print("Confidence interval could not be calculated: sample too small")

def createOutbreakProbabilityPlot(outputDir, scenarioName, transmissionProbability,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    outbreakProbabilities = []
    SEs = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir,fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize,
                            [(outputDir, fullScenarioName, s, numDays) for s in seeds])
            outbreaks = [1 if x>= extinctionThreshold else 0 for x in finalSizes]
            outbreakProbability = sum(outbreaks) / len(outbreaks)
            se = calculateSE(outbreakProbability, len(finalSizes))
            outbreakProbabilities.append(outbreakProbability)
            if se is not None:
                SEs.append(se)
            else:
                SEs.append(0)
    plt.errorbar(clusteringLevels, outbreakProbabilities, yerr=SEs, fmt='o')
    plt.xlabel("Clustering level")
    plt.xlim(-0.1, 1.1)
    plt.xticks(clusteringLevels)
    plt.ylabel("Outbreak probability")
    plt.ylim(0, 1)
    saveFig(outputDir, scenarioName + "_TP_" + str(transmissionProbability) + "_OutbreakProbabilities")

def createOutbreakProbabilities3DPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    ax = plt.axes(projection="3d")
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']

    i = 0
    for level in clusteringLevels:
        outbreakProbabilities = []
        SEs = []
        for prob in transmissionProbabilities:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                    [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                outbreaks = [1 if x>= extinctionThreshold else 0 for x in finalSizes]
                outbreakProbability = sum(outbreaks) / len(outbreaks)
                se = calculateSE(outbreakProbability, len(finalSizes))
                outbreakProbabilities.append(outbreakProbability)
                SEs.append(se)
        ax.scatter([level] * len(transmissionProbabilities), transmissionProbabilities, outbreakProbabilities, color=colors[i], s=20)
        for j in range(len(outbreakProbabilities)):
            if SEs[j] is not None:
                ax.plot([level, level],
                    [transmissionProbabilities[j], transmissionProbabilities[j]],
                    [outbreakProbabilities[j]+SEs[j], outbreakProbabilities[j]-SEs[j]], marker="_", color=colors[i])
        i += 1

    ax.set_xlabel("Clustering level")
    ax.set_xticks(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(transmissionProbabilities[::5])
    ax.set_zlabel("Outbreak probability")
    ax.set_zlim(-0.1, 1.1)
    saveFig(outputDir, scenarioName + "_OutbreakProbabilities")

import math
import matplotlib.pyplot as plt
import multiprocessing

from mpl_toolkits.mplot3d import Axes3D

from .Util import COLORS
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
        print("Confidence interval could not be calculated: only {} successes and {} failures".format(numSuccesses, numFailures))

def createOutbreakProbability3DPlot(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    # TODO 95 percentile invervals?
    ax = plt.axes(projection="3d")
    for prob_i in range(len(transmissionProbabilities)):
        outbreakProbabilities = []
        for level_i in range(len(clusteringLevels)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                                            [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                outbreaks = [1 if x>= extinctionThreshold else 0 for x in finalSizes]
                outbreakProbability = sum(outbreaks) / len(outbreaks)
                outbreakProbabilities.append(outbreakProbability)
        ax.bar(range(len(clusteringLevels)), outbreakProbabilities, zs=prob_i, zdir="y", alpha=0.5, color=COLORS[prob_i % len(COLORS)])
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Outbreak probability")
    saveFig(outputDir, "OutbreakProbabilities3D_" + scenarioName, "png")

def createOutbreakProbabilityHeatmap(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    allOutbreakProbabilities = []
    for prob in transmissionProbabilities:
        outbreakProbabilities = []
        for level in clusteringLevels:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                outbreaks = [1 if x >= extinctionThreshold else 0 for x in finalSizes]
                outbreakProbability = sum(outbreaks) / len(outbreaks)
                outbreakProbabilities.append(outbreakProbability)
        allOutbreakProbabilities.append(outbreakProbabilities)
    plt.imshow(allOutbreakProbabilities, vmin=0, vmax=1, cmap="jet", interpolation="bilinear", origin="lower", extent=[0,100,0,1], aspect=100)
    plt.colorbar()
    plt.xlabel("Clustering level")
    plt.xticks(range(0,101,25), clusteringLevels)
    plt.ylabel("Transmission probability")
    plt.yticks([(x - min(transmissionProbabilities)) / (max(transmissionProbabilities) - min(transmissionProbabilities)) for x in transmissionProbabilities[::2]],
                        transmissionProbabilities[::2])
    saveFig(outputDir, "OutbreakProbabilitiesHeatmap_" + scenarioName)

def createOutbreakProbabilityPlot(outputDir, scenarioName, transmissionProbability,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    outbreakProbabilities = []
    SEs = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize,
                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
            outbreaks = [1 if x >= extinctionThreshold else 0 for x in finalSizes]
            outbreakProbability = sum(outbreaks) / len(outbreaks)
            se = calculateSE(outbreakProbability, len(outbreaks))
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
    saveFig(outputDir, "OutbreakProbabilities_" + scenarioName + "_TP_" + str(transmissionProbability))

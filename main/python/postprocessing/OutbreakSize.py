import matplotlib.pyplot as plt
import multiprocessing

from .Util import COLORS, getFinalOutbreakSize, getRngSeeds, saveFig

from mpl_toolkits.mplot3d import Axes3D

def createExtinctionThresholdHistogram(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, numDays, poolSize):
    for prob in transmissionProbabilities:
        finalSizes = []
        for level in clusteringLevels:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes.append(pool.starmap(getFinalOutbreakSize,
                                    [(outputDir, fullScenarioName, s, numDays) for s in seeds]))
        n, bins, patches = plt.hist(finalSizes, bins=20, histtype="barstacked", color=COLORS[:len(COLORS)])
        hatches = ['-', '+', 'x', '\\', 'o', '.']
        for patch_set, hatch in zip(patches, hatches):
            plt.setp(patch_set, hatch=hatch)
        plt.xlabel("Total number of cases after {} days".format(numDays))
        plt.ylabel("Frequency")
        plt.legend(["Clustering level = {}".format(c) for c in clusteringLevels])
        saveFig(outputDir, "FinalSizesHist_" + scenarioName + "_TP_" + str(prob))

def createOutbreakSizes3DPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, numDays, extinctionThreshold, poolSize):
    ax = plt.axes(projection="3d")
    for prob_i in range(len(transmissionProbabilities))[::2]:
        means = []
        for level_i in range(len(clusteringLevels)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                finalSizes = pool.starmap(getFinalOutbreakSize,
                                [(outputDir, fullScenarioName, s, numDays) for s in seeds])
                finalSizes = [s for s in finalSizes if s >= extinctionThreshold]
                if len(finalSizes) > 0:
                    means.append(sum(finalSizes) / len(finalSizes))
                else:
                    means.append(0)
        ax.bar(range(len(clusteringLevels)), means, zs=prob_i, zdir="y", alpha=0.5, color=COLORS[prob_i % len(COLORS)])
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Final outbreak size after {} days".format(numDays))
    ax.set_zlim(-0.1, 50000)
    saveFig(outputDir, "OutbreakSizes3D_" + scenarioName, "png")

def createOutbreakSizesPlot(outputDir, scenarioName, transmissionProbability,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    allFinalSizes = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            finalSizes = pool.starmap(getFinalOutbreakSize, [(outputDir, fullScenarioName, s, numDays) for s in seeds])
            finalSizes = [s for s in finalSizes if s >= extinctionThreshold]
            allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Final outbreak size after {} days".format(numDays))
    plt.ylim(extinctionThreshold, 50000)
    saveFig(outputDir, "OutbreakSizes_" + scenarioName + "_TP_" + str(transmissionProbability))

#TODO Escape probability?


import matplotlib.pyplot as plt
import multiprocessing

from mpl_toolkits.mplot3d import Axes3D

from .Util import COLORS
from .Util import getRngSeeds, getSecondaryCasesFromIndex, saveFig

def createEffectiveRPlot(outputDir, scenarioName, transmissionProbability, clusteringLevels, poolSize):
    allEffectiveRs = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            allEffectiveRs.append(pool.starmap(getSecondaryCasesFromIndex,
                                        [(outputDir, fullScenarioName, s) for s in seeds]))
    plt.boxplot(allEffectiveRs, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Secondary cases")
    # TODO same y limits for all plots?
    saveFig(outputDir, "EffectiveR_" + scenarioName + "_TP_" + str(transmissionProbability))

def createEffectiveR3DScatterPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    ax = plt.axes(projection="3d")
    for prob_i in range(len(transmissionProbabilities)):
        for level_i in range(len(clusteringLevels)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCasesFromIndex, [(outputDir, fullScenarioName, s) for s in seeds])
                ax.scatter([level_i] * len(seeds), [prob_i] * len(seeds), secondaryCases, color=COLORS[prob_i % len(COLORS)])
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Secondary cases")
    saveFig(outputDir, "EffectiveR3DScatter_" + scenarioName)

def createEffectiveR3DBarPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    ax = plt.axes(projection="3d")

    for prob_i in range(len(transmissionProbabilities))[::2]:
        means = []
        for level_i in range(len(clusteringLevels)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCasesFromIndex, [(outputDir, fullScenarioName, s) for s in seeds])
                means.append(sum(secondaryCases) / len(secondaryCases))
        ax.bar(range(len(clusteringLevels)), means, zs=prob_i, zdir="y", color=COLORS[prob_i % len(COLORS)], alpha=0.6)
    # TODO Display min/max or 95% percentile interval?
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Secondary cases")
    saveFig(outputDir, "EffectiveR3DBar_" + scenarioName, "png")

def createEffectiveRHeatmap(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    allSecondaryCases = []

    for prob in transmissionProbabilities:
        means = []
        for level in clusteringLevels:
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCasesFromIndex, [(outputDir, fullScenarioName, s) for s in seeds])
                means.append(sum(secondaryCases) / len(secondaryCases))
        allSecondaryCases.append(means)

    plt.imshow(allSecondaryCases)
    plt.colorbar()
    plt.xlabel("Clustering level")
    plt.xticks(range(len(clusteringLevels)), clusteringLevels)
    plt.ylabel("Transmission probability")
    plt.yticks(range(len(transmissionProbabilities))[::2], transmissionProbabilities[::2])
    saveFig(outputDir, "EffectiveRHeatmap_" + scenarioName)

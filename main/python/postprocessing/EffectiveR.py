import matplotlib.pyplot as plt
import multiprocessing

from mpl_toolkits.mplot3d import Axes3D

from .Util import COLORS
from .Util import getRngSeeds, getSecondaryCasesFromIndex, saveFig

def createEffectiveRBoxplot(outputDir, scenarioName, transmissionProbability, clusteringLevels, poolSize):
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
    plt.ylim(0, 11)
    saveFig(outputDir, "EffectiveR_" + scenarioName + "_TP_" + str(transmissionProbability))

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
        ax.bar(range(len(clusteringLevels)), means, zs=prob_i, zdir="y", color=COLORS[prob_i % len(COLORS)], alpha=0.5)
    # TODO Display min/max or 95% percentile interval?
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Effective R")
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

    plt.imshow(allSecondaryCases, vmin=0, vmax=3, cmap="jet", interpolation="bilinear", origin="lower", extent=[0,100,0,1], aspect=100)
    plt.colorbar()
    plt.xlabel("Clustering level")
    plt.xticks(range(0,101,25), clusteringLevels)
    plt.ylabel("Transmission probability")
    plt.yticks([(x - min(transmissionProbabilities)) / (max(transmissionProbabilities) - min(transmissionProbabilities)) for x in transmissionProbabilities[::2]],
                    transmissionProbabilities[::2])
    saveFig(outputDir, "EffectiveRHeatmap_" + scenarioName)

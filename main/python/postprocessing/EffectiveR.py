import matplotlib.pyplot as plt
import multiprocessing
import numpy

from mpl_toolkits.mplot3d import Axes3D

from .Util import getRngSeeds, getSecondaryCases, saveFig

def createEffectiveRPlot(outputDir, scenarioName, transmissionProbability, clusteringLevels, poolSize):
    allEffectiveRs = []
    for level in clusteringLevels:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(level) + "_TP_" + str(transmissionProbability)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            allEffectiveRs.append(pool.starmap(getSecondaryCases, [(outputDir, fullScenarioName, s) for s in seeds]))
    plt.boxplot(allEffectiveRs, labels=clusteringLevels)
    plt.xlabel("Clustering level")
    plt.ylabel("Secondary cases")
    #plt.ylim(-0.5, ??)
    saveFig(outputDir, scenarioName + "_TP_" + str(transmissionProbability))

def createEffectiveR3DScatterPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    ax = plt.axes(projection="3d")
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    for level_i in range(len(clusteringLevels)):
        for prob_i in range(len(transmissionProbabilities)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCases, [(outputDir, fullScenarioName, s) for s in seeds])
                ax.scatter([level_i] * len(seeds), [prob_i] * len(seeds), secondaryCases, color=colors[level_i])
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::5])
    ax.set_yticklabels(transmissionProbabilities[::5])
    ax.set_zlabel("Secondary cases")
    saveFig(outputDir, scenarioName + "_ER_3D_scatter")

def createEffectiveR3DBarPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    ax = plt.axes(projection="3d")
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    for prob_i in range(len(transmissionProbabilities))[::2]:
        means = []
        lower = []
        upper = []
        for level_i in range(len(clusteringLevels)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCases, [(outputDir, fullScenarioName, s) for s in seeds])
                means.append(sum(secondaryCases) / len(secondaryCases))
                lower.append(numpy.percentile(secondaryCases, 2.5))
                upper.append(numpy.percentile(secondaryCases, 97.5))
        ax.bar(range(len(clusteringLevels)), means, zs=prob_i, zdir="y", color=colors[prob_i % len(colors)], alpha=0.6)
        for j in range(len(means)):
            ax.plot([j, j],
                    [prob_i, prob_i],
                    [upper[j], lower[j]], marker="_", color=colors[prob_i])
    ax.set_xlabel("Clustering level")
    ax.set_xticks(range(len(clusteringLevels)))
    ax.set_xticklabels(clusteringLevels)
    ax.set_ylabel("Transmission probability")
    ax.set_yticks(range(len(transmissionProbabilities))[::2])
    ax.set_yticklabels(transmissionProbabilities[::2])
    ax.set_zlabel("Secondary cases")
    saveFig(outputDir, scenarioName + "_ER_3D_bar", "png")

import matplotlib.pyplot as plt
import multiprocessing

from mpl_toolkits.mplot3d import Axes3D

from .Util import getRngSeeds, getSecondaryCases, saveFig

def createEffectiveRPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevel, poolSize):
    allEffectiveRs = []
    for prob in transmissionProbabilities:
        fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevel) + "_TP_" + str(prob)
        seeds = getRngSeeds(outputDir, fullScenarioName)
        with multiprocessing.Pool(processes=poolSize) as pool:
            allEffectiveRs.append(pool.starmap(getSecondaryCases, [(outputDir, fullScenarioName, s) for s in seeds]))
    plt.boxplot(allEffectiveRs, labels=transmissionProbabilities)
    plt.xlabel("Transmission probability")
    plt.xticks(range(len(transmissionProbabilities))[::5])
    plt.ylabel("Secondary cases")
    plt.ylim(-0.5, 10)
    saveFig(outputDir, scenarioName + "_CLUSTERING_" + str(clusteringLevel))

def createEffectiveR3DPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize):
    ax = plt.axes(projection="3d")
    colors = ['orange', 'green', 'red', 'purple', 'brown', 'cyan',
                'magenta', 'blue', 'yellow', 'lime', 'violet', 'firebrick',
                'forestgreen', 'turquoise']
    for level_i in range(len(clusteringLevels)):
        means = []
        for prob_i in range(len(transmissionProbabilities)):
            fullScenarioName = scenarioName + "_CLUSTERING_" + str(clusteringLevels[level_i]) + "_TP_" + str(transmissionProbabilities[prob_i])
            seeds = getRngSeeds(outputDir, fullScenarioName)
            with multiprocessing.Pool(processes=poolSize) as pool:
                secondaryCases = pool.starmap(getSecondaryCases, [(outputDir, fullScenarioName, s) for s in seeds])
                ax.scatter([prob_i] * len(seeds), [level_i] * len(seeds), secondaryCases, color=colors[level_i])
                means.append(sum(secondaryCases) / len(secondaryCases))
        ax.plot(range(len(transmissionProbabilities)), [level_i] * len(transmissionProbabilities), means, color=colors[level_i])

    ax.set_xlabel("Transmission probability")
    ax.set_xticks(range(len(transmissionProbabilities))[::5])
    ax.set_xticklabels(transmissionProbabilities[::5])
    ax.set_ylabel("Clustering level")
    ax.set_yticks(range(len(clusteringLevels)))
    ax.set_yticklabels(clusteringLevels)
    ax.set_zlabel("Secondary cases")
    saveFig(outputDir, "ER_3D", "png")

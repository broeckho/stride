import argparse
import time

from postprocessing import AgeImmunity
from postprocessing import Clustering
from postprocessing import EffectiveR

def main(outputDir, transmissionProbabilities, clusteringLevels, numDays, extinctionThreshold, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM", "AGEDEPENDENT"]
    # Age immunity profile plots
    #AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels,
    #                                    poolSize, targetRatesFile="2020_measles_immunity.xml")
    # Household assortativity coefficients plot
    #Clustering.createAssortativityCoefficientPlot(outputDir, scenarioNames,
    #                                    transmissionProbabilities, clusteringLevels, poolSize)
    # Effective R
    #EffectiveR.createEffectiveROverviewPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels, poolSize)

    for scenario in scenarioNames:
        for level in clusteringLevels:
            #AgeImmunity.createAgeImmunityPlot(outputDir, scenario, transmissionProbabilities, level, poolSize)
            EffectiveR.createEffectiveRPlot(outputDir, scenario, transmissionProbabilities, level, poolSize)

    # TODO extinction threshold?
    # TODO outbreak occurrence?
    # TODO outbreak sizes?
    # TODO escape probabilities?
    # TODO infected by age?

    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("--transmissionProbs", type=float, nargs="+", default=[0.20, 0.30, 0.40, 0.50, 0.60, 0.70], help="Values of R0 to analyze")
    parser.add_argument("--clusteringLevels", type=float, nargs="+", default=[0, 0.5, 1], help="Clustering levels to analyze")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulated days")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.transmissionProbs, args.clusteringLevels, args.numDays, args.extinctionThreshold, args.poolSize)

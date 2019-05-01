import argparse
import math
import time

from postprocessing import AgeImmunity
from postprocessing import Clustering
from postprocessing import EffectiveR

def main(outputDir, transmissionProbabilities, clusteringLevels, numDays, extinctionThreshold, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM", "AGEDEPENDENT"]
    r0CoeffA = -0.26766068
    r0CoeffB = 38.45456595
    # Age immunity profile plots
    #AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels,
    #                                    poolSize, targetRatesFile="2020_measles_immunity.xml")

    # Household assortativity coefficients plot
    #Clustering.createAssortativityCoefficientPlot(outputDir, scenarioNames,
    #                                    transmissionProbabilities, clusteringLevels, poolSize)
    #for scenario in scenarioNames:
    #    for level in clusteringLevels:
    #        AgeImmunity.createAgeImmunityPlot(outputDir, scenario, transmissionProbabilities, level, poolSize)
    #        EffectiveR.createEffectiveRPlot(outputDir, scenario, transmissionProbabilities, level, poolSize)

    # TODO extinction threshold?
    # TODO outbreak occurrence?
    # TODO outbreak sizes?
    # TODO escape probabilities?
    # TODO infected by age?'''
    for scenario in scenarioNames:
        EffectiveR.createEffectiveROverviewPlot(outputDir, scenario, transmissionProbabilities, clusteringLevels, poolSize, r0CoeffA, r0CoeffB)

    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("--transmissionProbs", type=float, nargs="+", default=[0.30, 0.40, 0.50, 0.60, 0.70, 0.80], help="Values of R0 to analyze")
    parser.add_argument("--clusteringLevels", type=float, nargs="+", default=[0, 0.25, 0.5, 0.75, 1], help="Clustering levels to analyze")
    parser.add_argument("--numDays", type=int, default=730, help="Number of simulated days")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.transmissionProbs, args.clusteringLevels, args.numDays, args.extinctionThreshold, args.poolSize)

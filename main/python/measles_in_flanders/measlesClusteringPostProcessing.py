import argparse
import numpy
import time

from postprocessing import AgeImmunity
from postprocessing import Clustering
from postprocessing import EffectiveR
from postprocessing import ExtinctionThreshold
from postprocessing import Util

'''
import math

def main(outputDir, transmissionProbabilities, clusteringLevels, numDays, extinctionThreshold, poolSize)
    # TODO extinction threshold?
    # TODO outbreak occurrence?
    # TODO outbreak sizes?
    # TODO escape probabilities?
    # TODO infected by age?
    for scenario in scenarioNames:
        EffectiveR.createEffectiveROverviewPlot(outputDir, scenario, transmissionProbabilities, clusteringLevels, poolSize, r0CoeffA, r0CoeffB)
'''

def main(outputDir, transmissionProbabilities, clusteringLevels, numDays, extinctionThreshold, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM", "AGEDEPENDENT"]
    r0CoeffA = -0.26766068
    r0CoeffB = 38.45456595

    pctsSusceptibles = Util.getAllFractionsSusceptibles(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels, poolSize)
    if not [x == pctsSusceptibles[0] for x in pctsSusceptibles]:
        print("Differing immunity levels!")
    pctSusceptibles = sum(pctsSusceptibles) / len(pctsSusceptibles)
    # Age immunity profile plots
    AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioNames, transmissionProbabilities, clusteringLevels,
                                        poolSize, targetRatesFile="2020_measles_immunity.xml")
    # Household assortativity coefficients plot
    Clustering.createAssortativityCoefficientPlot(outputDir, scenarioNames,
                                        transmissionProbabilities, clusteringLevels, poolSize)

    for scenario in scenarioNames:
        EffectiveR.createEffectiveROverviewPlot(outputDir, scenario,
                            transmissionProbabilities, clusteringLevels,
                            poolSize, r0CoeffA, r0CoeffB, pctSusceptibles)
        EffectiveR.createEffectiveROverviewPlot(outputDir, scenario,
                            transmissionProbabilities, clusteringLevels,
                            poolSize, r0CoeffA, r0CoeffB, pctSusceptibles,
                            stat="median")
        for level in clusteringLevels:
            EffectiveR.createEffectiveRPlot(outputDir, scenario, transmissionProbabilities, level, poolSize, r0CoeffA, r0CoeffB, pctSusceptibles)
           AgeImmunity.createAgeImmunityPlot(outputDir, scenario, transmissionProbabilities, level, poolSize)

    for prob in transmissionProbabilities:
        ExtinctionThreshold.createFinalSizesHistogram(outputDir, scenarioNames, prob, clusteringLevels, numDays, poolSize)

    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("--transmissionProbs", type=float, nargs="+", default=numpy.arange(0.30, 0.75, 0.05), help="Transmission probabilities to test")
    parser.add_argument("--clusteringLevels", type=float, nargs="+", default=[0, 0.25, 0.5, 0.75, 1], help="Clustering levels to test")
    parser.add_argument("--numDays", type=int, default=730, help="Number of simulated days")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in multiprocessing pool")
    args = parser.parse_args()
    main(args.outputDir, args.transmissionProbs, args.clusteringLevels, args.numDays, args.extinctionThreshold, args.poolSize)

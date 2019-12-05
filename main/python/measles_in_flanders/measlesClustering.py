import argparse
import contextlib
import io
import numpy
import os
import sys
import time

from measlesClusteringSimulations import runR0Simulations, runFullScenarioSimulations
from measlesClusteringPostprocessing import agesOfInfectedPostprocessing, effectiveRPostprocessing, immunityProfilePostprocessing, outbreakOccurrencePostprocessing, outbreakSizePostprocessing, r0Postprocessing

@contextlib.contextmanager
def nostdout():
    save_stdout = sys.stdout
    sys.stdout = io.StringIO()
    yield
    sys.stdout = save_stdout

def runSimulations(numRunsR0, numRunsFull, numDaysFull, immunityFileChildren, immunityFileAdults,
    transmissionProbabilities0to1, transmissionProbabilitiesRestricted, clusteringLevels,
    clusteringPoolType, poolSize):
    start = time.perf_counter()
    if numRunsR0 > 0:
        with nostdout():
            runR0Simulations(numRunsR0, transmissionProbabilities0to1, poolSize)
    print("R0 simulations took {} seconds".format(time.perf_counter() - start))
    start = time.perf_counter()
    if numRunsFull > 0:
        with nostdout():
            runFullScenarioSimulations(numRunsFull, numDaysFull,
                                        immunityFileChildren, immunityFileAdults,
                                        transmissionProbabilitiesRestricted,
                                        clusteringLevels, clusteringPoolType,
                                        poolSize)
    print("Full simulations took {} seconds".format(time.perf_counter() - start))

def postprocessing(numRunsR0, numRunsFull, numDaysFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, extinctionThreshold, poolSize):
    outputDir = "."
    scenarioName = "AGEDEPENDENT"

    start = time.perf_counter()
    if numRunsR0 > 0:
        r0Postprocessing(outputDir, transmissionProbabilities0to1, poolSize)
    print("R0 postprocessing took {} seconds".format(time.perf_counter() - start))
    start = time.perf_counter()
    if numRunsFull > 0:
        agesOfInfectedPostprocessing(outputDir, scenarioName,
                                transmissionProbabilitiesRestricted,
                                clusteringLevels, numDaysFull, poolSize)
        effectiveRPostprocessing(outputDir, scenarioName,
                                transmissionProbabilitiesRestricted,
                                clusteringLevels, poolSize)
        immunityProfilePostprocessing(outputDir, scenarioName,
                                transmissionProbabilitiesRestricted,
                                clusteringLevels, poolSize)
        outbreakOccurrencePostprocessing(outputDir, scenarioName,
                                        transmissionProbabilitiesRestricted,
                                        clusteringLevels, numDaysFull,
                                        extinctionThreshold, poolSize)
        outbreakSizePostprocessing(outputDir, scenarioName,
                                        transmissionProbabilitiesRestricted,
                                        clusteringLevels, numDaysFull,
                                        extinctionThreshold, poolSize)

    print("Full postprocessing took {} seconds".format(time.perf_counter() - start))

def main(postprocessingOnly, numRunsR0, numRunsFull, numDaysFull, immunityFileChildren,
    immunityFileAdults, clusteringPoolType, extinctionThreshold, poolSize):
    transmissionProbabilities0to1 = numpy.arange(0, 1.05, 0.05)
    transmissionProbabilitiesRestricted = numpy.arange(0.2, 0.85, 0.05)
    clusteringLevels = [0, 0.25, 0.5, 0.75, 1]
    if not postprocessingOnly:
        runSimulations(numRunsR0, numRunsFull, numDaysFull, immunityFileChildren,
                            immunityFileAdults, transmissionProbabilities0to1,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            clusteringPoolType, poolSize)

    postprocessing(numRunsR0, numRunsFull, numDaysFull, transmissionProbabilities0to1,
                    transmissionProbabilitiesRestricted, clusteringLevels,
                    extinctionThreshold, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--postprocessingOnly", action="store_true", default=False)
    parser.add_argument("--numRunsR0", type=int, default=0)
    parser.add_argument("--numRunsFull", type=int, default=0)
    parser.add_argument("--numDaysFull", type=int, default=730)
    parser.add_argument("--immunityFileChildren", type=str, default=os.path.join("data", "2020_measles_child_immunity.xml"))
    parser.add_argument("--immunityFileAdults", type=str, default=os.path.join("data", "2020_measles_adult_immunity.xml"))
    parser.add_argument("--clusteringPoolType", type=str, default="Household")
    parser.add_argument("--extinctionThreshold", type=int, default=0)
    parser.add_argument("--poolSize", type=int, default=8)
    args = parser.parse_args()
    main(args.postprocessingOnly, args.numRunsR0, args.numRunsFull,
            args.numDaysFull, args.immunityFileChildren, args.immunityFileAdults,
            args.clusteringPoolType, args.extinctionThreshold, args.poolSize)

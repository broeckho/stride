import argparse
import contextlib
import io
import numpy
import sys
import time

from measlesClusteringSimulations import runR0Simulations, runFullScenarioSimulations

@contextlib.contextmanager
def nostdout():
    save_stdout = sys.stdout
    sys.stdout = io.StringIO()
    yield
    sys.stdout = save_stdout

def runSimulations(numRunsR0, numRunsFull, numDaysFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, poolSize):
    start = time.perf_counter()
    if numRunsR0 > 0:
        with nostdout():
            runR0Simulations(numRunsR0, transmissionProbabilities0to1, poolSize)
    print("R0 simulations took {} seconds".format(time.perf_counter() - start))
    start = time.perf_counter()
    if numRunsFull > 0:
        runFullScenarioSimulations(numRunsFull, numDaysFull,
                                        transmissionProbabilitiesRestricted,
                                        clusteringLevels, poolSize)
    print("Full simulations took {} seconds".format(time.perf_counter() - start))

def postprocessing(numRunsR0, numRunsFull, numDaysFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, extinctionThreshold, poolSize):
    outputDir = "."
    if numRunsR0 > 0:
        R
    if numRunsFull > 0:
        pass
'''
from postprocessing import AgeImmunity, Clustering, EffectiveR, EscapeProbability, InfectedByAge
from postprocessing import OutbreakEvolution, OutbreakOccurrence, OutbreakSize, R0

def postprocessing(numRunsR0, numRunsFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, extinctionThreshold, poolSize):
    scenarioName = "AGEDEPENDENT"
    numDaysR0 = 30
    numDaysFull = 60
    if numRunsR0 > 0:
        print("R0 postprocessing")
        fitCoeffs = R0.getLnFit(outputDir, transmissionProbabilities0to1, poolSize)

        R0.createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir,
                                transmissionProbabilities0to1, fitCoeffs[0],
                                fitCoeffs[1], poolSize)

        R0.createTransmissionProbabilityVSSecondaryCasesBoxplots(outputDir,
                                transmissionProbabilities0to1, poolSize)

    if numRunsFull > 0:
        print("Full scenario postprocessing")
        AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            poolSize, targetLevelsDir="../../Workspace/MeaslesInFlanders_Paper/ProjectedImmunity")
        Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels, poolSize)
        Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels, poolSize, ageLim=36)

        EffectiveR.createEffectiveR3DScatterPlot(outputDir, scenarioName, transmissionProbabilitiesRestricted,
                            clusteringLevels, poolSize)
        EffectiveR.createEffectiveR3DBarPlot(outputDir, scenarioName, transmissionProbabilitiesRestricted,
                            clusteringLevels, poolSize)
        EffectiveR.createEffectiveRHeatmap(outputDir, scenarioName, transmissionProbabilitiesRestricted,
                            clusteringLevels, poolSize)

        EscapeProbability.createEscapeProbabilityHeatmapPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, poolSize)

        InfectedByAge.meanAgeOfInfectionHeatmap(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, poolSize)

        OutbreakOccurrence.createOutbreakProbability3DPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, extinctionThreshold, poolSize)
        OutbreakOccurrence.createOutbreakProbabilityHeatmap(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, extinctionThreshold, poolSize)
        OutbreakSize.createOutbreakSizes3DPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, extinctionThreshold, poolSize)
        for level in clusteringLevels:
            AgeImmunity.createAgeImmunityBoxplot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, level, poolSize)

        for prob in transmissionProbabilitiesRestricted:
            EffectiveR.createEffectiveRPlot(outputDir, scenarioName, prob,
                            clusteringLevels, poolSize)
            OutbreakOccurrence.createOutbreakProbabilityPlot(outputDir, scenarioName, prob,
                            clusteringLevels, numDaysFull, extinctionThreshold, poolSize)
            OutbreakSize.createExtinctionThresholdHistogram(outputDir, scenarioName,
                            prob, clusteringLevels, numDaysFull, poolSize)
            OutbreakSize.createOutbreakSizesPlot(outputDir, scenarioName, prob,
                            clusteringLevels, numDaysFull, extinctionThreshold, poolSize)

            # TODO more scientific way to estimate extinction threshold?
            EscapeProbability.createEscapeProbabilityPlot(outputDir, scenarioName,
                            prob, clusteringLevels, numDaysFull, poolSize)
            for level in clusteringLevels:
                OutbreakEvolution.createNewCasesPerDayPlot(outputDir, scenarioName,
                                    prob, level, numDaysFull, poolSize)
                OutbreakEvolution.createCumulativeCasesPerDayPlot(outputDir, scenarioName,
                                    prob, level, numDaysFull, poolSize)
'''

def main(postprocessingOnly, numRunsR0, numRunsFull, numDaysFull, extinctionThreshold, poolSize):
    transmissionProbabilities0to1 = numpy.arange(0, 1.05, 0.05)
    transmissionProbabilitiesRestricted = numpy.arange(0.2, 0.85, 0.05)
    clusteringLevels = [0, 0.25, 0.5, 0.75, 1]
    if not postprocessingOnly:
        runSimulations(numRunsR0, numRunsFull, numDaysFull,
                        transmissionProbabilities0to1,
                        transmissionProbabilitiesRestricted,
                        clusteringLevels, poolSize)
    postprocessing(numRunsR0, numRunsFull, numDaysFull, transmissionProbabilities0to1,
                    transmissionProbabilitiesRestricted, clusteringLevels,
                    extinctionThreshold, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--postprocessingOnly", action="store_true", default=False)
    parser.add_argument("--numRunsR0", type=int, default=0)
    parser.add_argument("--numRunsFull", type=int, default=0)
    parser.add_argument("--numDaysFull", type=int, default=60)
    parser.add_argument("--extinctionThreshold", type=int, default=0)
    parser.add_argument("--poolSize", type=int, default=8)
    args = parser.parse_args()
    main(args.postprocessingOnly, args.numRunsR0, args.numRunsFull,
            args.numDaysFull, args.extinctionThreshold, args.poolSize)

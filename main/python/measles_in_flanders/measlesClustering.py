import argparse
import contextlib
import io
import numpy
import sys
import time

from measlesClusteringSimulations import runR0Simulations, runEffectiveRSimulations, runFullScenarioSimulations

from postprocessing import AgeImmunity, Clustering, EffectiveR, R0, OutbreakSize, OutbreakOccurrence, InfectedByAge

@contextlib.contextmanager
def nostdout():
    save_stdout = sys.stdout
    sys.stdout = io.StringIO()
    yield
    sys.stdout = save_stdout

def runSimulations(numRunsR0, numRunsER, numRunsFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, poolSize):

    if numRunsR0 > 0:
        # Run simulations to establish relation P(transmission) ~ R0
        # TODO different start dates?
        start = time.perf_counter()
        with nostdout():
            runR0Simulations(numRunsR0, transmissionProbabilities0to1, poolSize)
        print("R0 simulations took {} seconds".format(time.perf_counter() - start))
    if numRunsER > 0:
        # Run simulations to establish relation clustering level + P(transmission) ~ Effective R
        start = time.perf_counter()
        with nostdout():
            runEffectiveRSimulations(numRunsER, transmissionProbabilities0to1, clusteringLevels, poolSize)
        print("Effective R simulations took {} seconds".format(time.perf_counter() - start))

    if numRunsFull > 0:
        # Run simulations for 2 years, keeping track of cases, susceptibles, household constitutions
        start = time.perf_counter()
        with nostdout():
            runFullScenarioSimulations(numRunsFull, transmissionProbabilitiesRestricted, clusteringLevels, poolSize)
        print("Full simulations took {} seconds".format(time.perf_counter() - start))

def postprocessing(numRunsR0, numRunsER, numRunsFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, poolSize):

    outputDir = "."
    scenarioNames = ["AGEDEPENDENT"]
    numDaysIndex = 25
    numDaysFull = 730

    if numRunsR0 > 0:
        # Fit relation P(transmission) ~ R0 to logarithmic function
        start = time.perf_counter()
        fitCoeffs = R0.getLnFit(outputDir, transmissionProbabilities0to1, poolSize)
        print(fitCoeffs)
        R0.createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir,
                                    scenarioNames, transmissionProbabilities0to1,
                                    fitCoeffs[0], fitCoeffs[1], poolSize)
        R0.createTransmissionProbabilityVSSecondaryCasesBoxplots(outputDir,
                                    scenarioNames, transmissionProbabilities0to1,
                                    poolSize)

        print("R0 postprocessing took {} seconds".format(time.perf_counter() - start))

    if numRunsER > 0:
        # Effective R
        start = time.perf_counter()
        EffectiveR.createEffectiveR3DScatterPlot(outputDir, "ER_" + scenarioNames[0],
                                            transmissionProbabilities0to1,
                                            clusteringLevels, poolSize)
        EffectiveR.createEffectiveR3DBarPlot(outputDir, "ER_" + scenarioNames[0],
                                            transmissionProbabilitiesRestricted,
                                            clusteringLevels, poolSize)
        for prob in transmissionProbabilities0to1:
            EffectiveR.createEffectiveRPlot(outputDir, "ER_" + scenarioNames[0], prob, clusteringLevels, poolSize)
        print("ER postprocessing took {} seconds".format(time.perf_counter() - start))

    if numRunsFull > 0:
        # Age-immunity profiles
        start = time.perf_counter()
        AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioNames[0],
                                                transmissionProbabilitiesRestricted,
                                                clusteringLevels, poolSize,
                                                targetRatesDir="../../Workspace/MeaslesInFlanders_Paper/ProjectedImmunity")
        for level in clusteringLevels:
            AgeImmunity.createAgeImmunityPlot(outputDir, scenarioNames[0], transmissionProbabilitiesRestricted, level, poolSize)

        print("Age-immunity postprocessing took {} seconds".format(time.perf_counter() - start))

        # Calculate associativity within households
        start = time.perf_counter()
        Clustering.createAssortativityCoefficientPlot(outputDir, scenarioNames[0],
                                            transmissionProbabilitiesRestricted,
                                            clusteringLevels, poolSize)
        print("Associativity postprocessing took {} seconds".format(time.perf_counter() - start))

        # Outbreak probabilities + sizes
        start = time.perf_counter()
        OutbreakSize.createExtinctionThresholdHistogram(outputDir, scenarioNames[0], transmissionProbabilitiesRestricted,
                                            clusteringLevels, numDaysFull, poolSize)

        extinctionThreshold = 10000
        OutbreakOccurrence.createOutbreakProbabilities3DPlot(outputDir, scenarioNames[0],
                                            transmissionProbabilitiesRestricted,
                                            clusteringLevels, numDaysFull, extinctionThreshold, poolSize)
        OutbreakSize.createOutbreakSizes3DPlot(outputDir, scenarioNames[0],
                                            transmissionProbabilitiesRestricted,
                                            clusteringLevels, numDaysFull,
                                            extinctionThreshold, poolSize)

        for prob in transmissionProbabilitiesRestricted:
            InfectedByAge.createInfectedByAgeScatterplot(outputDir, scenarioNames[0], prob, clusteringLevels, poolSize)
            InfectedByAge.createInfectedByAgePlot(outputDir, scenarioNames[0], prob, clusteringLevels, poolSize)
            OutbreakSize.createOutbreakSizesPlot(outputDir, scenarioNames[0], prob, clusteringLevels, numDaysFull, extinctionThreshold, poolSize)
        print("Outbreak size + probability postprocessing took {} seconds".format(time.perf_counter() - start))
        # TODO Total number of susceptibles / overall level of immunity

def main(postprocessingOnly, numRunsR0, numRunsER, numRunsFull, poolSize):
    transmissionProbabilities0to1 = numpy.arange(0, 1.05, 0.05)
    transmissionProbabilitiesRestricted = numpy.arange(0.2, 0.85, 0.05)
    clusteringLevels = [0, 0.25, 0.5, 0.75, 1]
    if not postprocessingOnly:
        runSimulations(numRunsR0, numRunsER, numRunsFull,
                   transmissionProbabilities0to1, transmissionProbabilitiesRestricted,
                   clusteringLevels, poolSize)
    postprocessing(numRunsR0, numRunsER, numRunsFull,
                    transmissionProbabilities0to1, transmissionProbabilitiesRestricted,
                    clusteringLevels, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--postprocessingOnly", action="store_true", default=False)
    parser.add_argument("--numRunsR0", type=int, default=0)
    parser.add_argument("--numRunsER", type=int, default=0)
    parser.add_argument("--numRunsFull", type=int, default=0)
    parser.add_argument("--poolSize", type=int, default=8)
    args = parser.parse_args()
    main(args.postprocessingOnly, args.numRunsR0, args.numRunsER, args.numRunsFull, args.poolSize)

import argparse
import contextlib
import io
import multiprocessing
import numpy
import os
import sys
import time

from measlesClusteringCallbacks import registerAssociativityCoefficient, registerSusceptibles
from measlesClusteringSimulations import runSimulation
from measlesClusteringUtil import generateRngSeeds, writeRngSeeds, getCommonParameters

from pystride.Event import Event, EventType

from postprocessing import AgeImmunity, InfectedByAge, Clustering, EffectiveR
from postprocessing import EscapeProbability, OutbreakOccurrence, OutbreakSize

@contextlib.contextmanager
def nostdout():
    save_stdout = sys.stdout
    sys.stdout = io.StringIO()
    yield
    sys.stdout = save_stdout

# TODO calculate transmission probability - R0 relation for new populations
# TODO 2020
# TODO 2030
# TODO 2040

def runSimulations(numRuns, years, transmissionProbabilities, clusteringLevels, poolSize):
    start = time.perf_counter()

    callbacks = [
        (registerAssociativityCoefficient, EventType.AtStart),
        (registerSusceptibles, EventType.AtStart)
    ]

    extraParams = {
        "immunity_link_probability": 0,
        "immunity_profile": "AgeDependent",
        "num_days": 730,
        "num_index_cases": 1,
        "track_index_case": "false",
        "vaccine_profile": "AgeDependent",
        "vaccine_clustering_pooltype": "Household",
    }

    for year in years:
        extraParams["population_file"] = "pop_flanders_projections_" + str(year) + ".csv"
        extraParams["immunity_distribution_file"] = os.path.join("data", str(year) + "_measles_natural_immunity.xml")
        extraParams["vaccine_distribution_file"] = os.path.join("data", str(year) + "_measles_vaccine_immunity.xml")
        for prob in transmissionProbabilities:
            extraParams["transmission_probability"] = prob
            for level in clusteringLevels:
                with nostdout():
                    extraParams["vaccine_link_probability"] = level
                    outputPrefix = "AGEDEPENDENT_" + str(year) + "_CLUSTERING_" + str(level) + "_TP_" + str(prob)
                    seeds = generateRngSeeds(numRuns)
                    writeRngSeeds(outputPrefix, seeds)
                    with multiprocessing.Pool(processes=poolSize) as pool:
                        pool.starmap(runSimulation, [(outputPrefix, s, extraParams, callbacks) for s in seeds])
    print("Simulations took {} seconds".format(time.perf_counter() - start))

def postprocessing(numRuns, years, transmissionProbabilities, clusteringLevels, extinctionThreshold, poolSize):
    start = time.perf_counter()
    outputDir = "."
    scenarioName = "AGEDEPENDENT"
    numDays = 730
    for year in years:
        AgeImmunity.getMeanPercentageOfSusceptible(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels, poolSize)
        AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels, poolSize,
                                                targetLevelsDir="../../Workspace/ProjectedImmunity",
                                                targetLevelsYear=year)
        Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels,
                                                poolSize, ageLim=99)
        InfectedByAge.meanAgeOfInfectionHeatmap(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels,
                                                numDays, poolSize)

        EffectiveR.createEffectiveRHeatmap(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels,
                                                poolSize)
        EscapeProbability.createEscapeProbabilityHeatmapPlot(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels,
                                                numDays, poolSize)
        OutbreakOccurrence.createOutbreakProbabilityHeatmap(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels,
                                                numDays, extinctionThreshold, poolSize)
        OutbreakSize.createOutbreakSizes3DPlot(outputDir, scenarioName + "_" + str(year),
                                                transmissionProbabilities, clusteringLevels,
                                                numDays, extinctionThreshold, poolSize)
        for prob in transmissionProbabilities:
            OutbreakSize.createExtinctionThresholdHistogram(outputDir, scenarioName + "_" + str(year),
                                                prob, clusteringLevels, numDays, poolSize)
    print("postprocessing took {} seconds".format(time.perf_counter() - start))

def main(postprocessingOnly, numRuns, years, extinctionThreshold, poolSize):
    transmissionProbabilities = numpy.arange(0.40, 0.85, 0.05)
    clusteringLevels = [0, 0.25, 0.5, 0.75, 1]
    if not postprocessingOnly:
        runSimulations(numRuns, years, transmissionProbabilities, clusteringLevels, poolSize)
    postprocessing(numRuns, years, transmissionProbabilities, clusteringLevels, extinctionThreshold, poolSize)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--postprocessingOnly", action="store_true", default=False)
    parser.add_argument("--numRuns", type=int, default=5)
    parser.add_argument("--years", type=int, nargs="+", default=[2020, 2030, 2040])
    parser.add_argument("--extinctionThreshold", type=int, default=0)
    parser.add_argument("--poolSize", type=int, default=8)
    # TODO cutoff for natural immunity?
    args = parser.parse_args()
    main(args.postprocessingOnly, args.numRuns, args.years, args.extinctionThreshold, args.poolSize)

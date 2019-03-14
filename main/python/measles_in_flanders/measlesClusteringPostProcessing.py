import argparse
import time

from postprocessing import AgeImmunity
from postprocessing import EffectiveR
from postprocessing import ExtinctionThreshold
from postprocessing import OutbreakOccurrenceAndSize

from postprocessing import Util

def main(outputDir, plotER, R0s, numDays, extinctionThreshold, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM_NOCLUSTERING", "AGEDEPENDENT_NOCLUSTERING", "AGEDEPENDENT_CLUSTERING"]
    scenarioDisplayNames = ["A", "B", "C"]

    if plotER:
        fractionSusceptibles = Util.getOverallFractionSusceptibles(outputDir, R0s, scenarioNames, poolSize)
        # Overview of different R0 values vs effective Rs
        EffectiveR.createEffectiveROverviewPlot(outputDir, R0s, scenarioNames,
                                                    scenarioDisplayNames,
                                                    fractionSusceptibles,
                                                    poolSize, "Scenario",
                                                    "All_EffectiveR_Mean")
        EffectiveR.createEffectiveROverviewPlot(outputDir, R0s, scenarioNames,
                                                    scenarioDisplayNames,
                                                    fractionSusceptibles,
                                                    poolSize, "Scenario",
                                                    "All_EffectiveR_Median",
                                                    stat="median")
        for R0 in R0s:
            scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
            EffectiveR.createEffectiveRPlot(outputDir, R0,
                                            scenarioNamesFull,
                                            scenarioDisplayNames,
                                            fractionSusceptibles,
                                            poolSize,
                                            "Scenario", "R0_" + str(R0) + "_EffectiveR")
    else:
        # Overview plot for outbreak occurrence probabilities
        OutbreakOccurrenceAndSize.createOutbreakOccurrenceOverviewPlot(outputDir, R0s,
                                                    scenarioNames, scenarioDisplayNames,
                                                    "Scenario",
                                                    numDays,
                                                    extinctionThreshold, poolSize)
        # Escape probabilities
        OutbreakOccurrenceAndSize.createEscapeProbabilityOverviewPlots(outputDir, R0s,
                                                    scenarioNames, scenarioDisplayNames,
                                                    numDays, poolSize, "AllEscapeProbabilities_Mean")
        OutbreakOccurrenceAndSize.createEscapeProbabilityOverviewPlots(outputDir, R0s,
                                                    scenarioNames, scenarioDisplayNames,
                                                    numDays, poolSize, "AllEscapeProbabilities_Median",
                                                    "median")
        # Overview plot for outbreak sizes (when no extinction occurs)
        OutbreakOccurrenceAndSize.createFinalSizesOverviewPlots(outputDir, R0s,
                                                    scenarioNames, scenarioDisplayNames,
                                                    numDays, extinctionThreshold, poolSize,
                                                    "AllOutbreakSizes_Mean")
        OutbreakOccurrenceAndSize.createFinalSizesOverviewPlots(outputDir, R0s,
                                                    scenarioNames, scenarioDisplayNames,
                                                    numDays, extinctionThreshold, poolSize,
                                                    "AllOutbreakSizes_Median", stat="median")
        for R0 in R0s:
            scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
            # Plot age-dependent immunity levels
            AgeImmunity.createAgeImmunityPlot(outputDir, scenarioNamesFull,
                                                scenarioDisplayNames, poolSize,
                                                "R0_" + str(R0) + "_AgeImmunityPlot",
                                                targetRatesFile="2020_measles_immunity.xml")
            # Plot final size frequencies to determine extinction threshold
            ExtinctionThreshold.createFinalSizesHistogram(outputDir, scenarioNamesFull,
                                                scenarioDisplayNames,
                                                numDays, poolSize,
                                                "R0_" + str(R0) + "_ExtinctionThreshold")

            for scenario in scenarioNames:
                # Create household constitution plot
                # TODO add data from vaccination studie 2016?
                AgeImmunity.createHouseholdConstitutionPlot(outputDir,
                                                    scenario + "_R0_" + str(R0),
                                                    poolSize,
                                                    "R0_" + str(R0) + scenario + "_HHConstitutionPlot")
    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument('--plotER', action="store_true", help="Plot effective R?")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values of R0 to analyze")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulated days")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.plotER, args.R0s, args.numDays, args.extinctionThreshold, args.poolSize)

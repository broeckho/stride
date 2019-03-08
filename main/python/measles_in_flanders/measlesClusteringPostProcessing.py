import argparse
import time

from postprocessing import AgeImmunity
from postprocessing import EffectiveR

'''
from postprocessing import ExtinctionThreshold, OutbreakEvolution, OutbreakOccurrenceAndSize

def main(outputDir, numDays, R0s, extinctionThreshold, poolSize):
    # Overview plot for outbreak occurrence probabilities
    OutbreakOccurrenceAndSize.createOutbreakOccurrenceOverviewPlot(outputDir, R0s,
                                                    SCENARIO_NAMES, SCENARIO_DISPLAY_NAMES,
                                                    numDays,
                                                    extinctionThreshold, poolSize)
    # Overview plot for sizes of outbreaks?
    OutbreakOccurrenceAndSize.createFinalSizesOverviewPlots(outputDir, R0s,
                                                    SCENARIO_NAMES, SCENARIO_DISPLAY_NAMES,
                                                    numDays,
                                                    extinctionThreshold, poolSize)
    # Escape probabilities vs R0s
    OutbreakOccurrenceAndSize.createEscapeProbabilityOverviewPlots(outputDir, R0s,
                                                    SCENARIO_NAMES, SCENARIO_DISPLAY_NAMES,
                                                    numDays, extinctionThreshold,
                                                    poolSize)
    for R0 in R0s:
        ExtinctionThreshold.createFinalSizesHistogram(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES,
                                                    numDays, poolSize,
                                                    "R0_" + str(R0) + "_ExtinctionThreshold")
        OutbreakOccurrenceAndSize.createOutbreakOccurrencePlot(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES, numDays,
                                                    extinctionThreshold, poolSize,
                                                    "R0_" + str(R0) + "_OutbreakOccurrence")
        # Infected by age overview
        AgeImmunity.createInfectedByAgeOverviewPlot(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES, poolSize,
                                                    "R0_" + str(R0) + "_InfectedByAge",
                                                    extinctionThreshold)
        for scenario in scenarioNames:
            avgImmunityRate = AgeImmunity.getAvgOverallImmunityRate(outputDir, scenario, poolSize)
            print("Avg overall immunity rate for {} with R0 {} is {}".format(scenario, R0, avgImmunityRate))
            # Infected by age
            AgeImmunity.createInfectedByAgePlot(outputDir, scenario, poolSize, scenario + "_InfectedByAge")
'''

def main(outputDir, plotER, R0s, extinctionThreshold, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM_NOCLUSTERING", "AGEDEPENDENT_NOCLUSTERING", "AGEDEPENDENT_CLUSTERING"]
    scenarioDisplayNames = ["A", "B", "C"]

    if plotER:
        # Overview of different R0 values vs effective Rs
        # TODO add line to indicate expected R

        '''EffectiveR.createEffectiveROverviewPlot(outputDir, scenarioNames,
                                                    scenarioDisplayNames, R0s,
                                                    poolSize, "Scenario",
                                                    "All_EffectiveR_Mean")
        EffectiveR.createEffectiveROverviewPlot(outputDir, scenarioNames,
                                                    scenarioDisplayNames, R0s,
                                                    poolSize, "Scenario",
                                                    "All_EffectiveR_Median",
                                                    stat="median")'''
        for R0 in R0s:
            scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
            EffectiveR.createEffectiveRPlot(outputDir, R0, scenarioNamesFull,
                                            scenarioDisplayNames,
                                            poolSize,
                                            "Scenario", "R0_" + str(R0) + "_EffectiveR")
    else:
        for R0 in R0s:
            scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
            # Plot age-dependent immunity levels
            AgeImmunity.createAgeImmunityPlot(outputDir, scenarioNamesFull,
                                                scenarioDisplayNames, poolSize,
                                                "R0_" + str(R0) + "_AgeImmunityPlot",
                                                targetRatesFile="2020_measles_immunity.xml")
            for scenario in scenarioNames:
                # Create household constitution plot
                # TODO add data from vaccination studie 2016?
                AgeImmunity.createHouseholdConstitutionPlot(outputDir,
                                                    scenario + "_R0_" + str(R0),
                                                    poolSize,
                                                    "R0_" + str(R0) + scenario + "_HHConstitutionPlot")
    # TODO extinction Threshold
    # TODO outbreak occurrence
    # TODO escape probability
    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("plotER", type=bool, help="Plot Effective R?")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values of R0 to analyze")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.plotER, args.R0s, args.extinctionThreshold, args.poolSize)

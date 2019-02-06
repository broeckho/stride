import argparse
import time

from postprocessing import EffectiveR

SCENARIO_NAMES = ["UN_NCLU", "AD_NCLU", "UN_CLU", "AD_CLU"]
SCENARIO_DISPLAY_NAMES = ["Uniform\n+ no clustering",
                            "Age-dependent\n+ no clustering",
                            "Uniform\n+ clustering",
                            "Age-dependent\n+ clustering"]

'''

def main(outputDir, numDays, R0s, extinctionThreshold, poolSize):
    start = time.perf_counter()
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
    # TODO effective Rs vs R0s
    # Escape probabilities vs R0s
    OutbreakOccurrenceAndSize.createEscapeProbabilityOverviewPlots(outputDir, R0s,
                                                    SCENARIO_NAMES, SCENARIO_DISPLAY_NAMES,
                                                    numDays, extinctionThreshold,
                                                    poolSize)
    for R0 in R0s:
        scenarioNames = [s + "_R0_" + str(R0) for s in SCENARIO_NAMES]
        AgeImmunity.createAgeImmunityPlots(outputDir, scenarioNames, SCENARIO_DISPLAY_NAMES,
                                            poolSize, "R0_" + str(R0) + "_AgeImmunityPlot",
                                            targetRatesFile="2013_measles_immunity.xml")
        # TODO household constitutions
        ExtinctionThreshold.createFinalSizesHistogram(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES,
                                                    numDays, poolSize,
                                                    "R0_" + str(R0) + "_ExtinctionThreshold")
        OutbreakOccurrenceAndSize.createOutbreakOccurrencePlot(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES, numDays,
                                                    extinctionThreshold, poolSize,
                                                    "R0_" + str(R0) + "_OutbreakOccurrence")
        OutbreakOccurrenceAndSize.createFinalSizesBoxplot(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES, numDays,
                                                    extinctionThreshold, poolSize,
                                                    "R0_" + str(R0) + "_OutbreakSizes")
        OutbreakOccurrenceAndSize.createEscapeProbabilityPlot(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES, numDays,
                                                    extinctionThreshold, poolSize,
                                                    "R0_" + str(R0) + "_EscapeProbability")
        # Infected by age overview
        AgeImmunity.createInfectedByAgeOverviewPlot(outputDir, scenarioNames,
                                                    SCENARIO_DISPLAY_NAMES, poolSize,
                                                    "R0_" + str(R0) + "_InfectedByAge",
                                                    extinctionThreshold)
        for scenario in scenarioNames:
            avgImmunityRate = AgeImmunity.getAvgOverallImmunityRate(outputDir, scenario, poolSize)
            print("Avg overall immunity rate for {} with R0 {} is {}".format(scenario, R0, avgImmunityRate))
            # Cumulative cases per day
            OutbreakEvolution.createCumulativeCasesPerDayPlot(outputDir, scenario,
                                                numDays, extinctionThreshold,
                                                poolSize, scenario + "_CumulativeCases")
            # New cases per day
            OutbreakEvolution.createNewCasesPerDayPlot(outputDir, scenario,
                                                numDays, extinctionThreshold,
                                                poolSize, scenario + "_NewCases")
            # Infected by age
            AgeImmunity.createInfectedByAgePlot(outputDir, scenario, poolSize, scenario + "_InfectedByAge")
'''

'''
createEffectiveRPlot(outputDir, scenarioNames, scenarioDisplayNames, poolSize, figName)
'''

def main(outputDir, R0s, poolSize):
    start = time.perf_counter()
    EffectiveR.createEffectiveROverviewPlot(outputDir, SCENARIO_NAMES,
                                            SCENARIO_DISPLAY_NAMES, R0s,
                                            poolSize, "AllEffectiveR_Mean")
    EffectiveR.createEffectiveROverviewPlot(outputDir, SCENARIO_NAMES,
                                            SCENARIO_DISPLAY_NAMES, R0s,
                                            poolSize, "AllEffectiveR_Median",
                                            stat="median")
    for R0 in R0s:
        scenarioNames = [s + "_R0_" + str(R0) for s in SCENARIO_NAMES]
        EffectiveR.createEffectiveRPlot(outputDir, scenarioNames,
                                        SCENARIO_DISPLAY_NAMES, poolSize,
                                        "R0_" + str(R0) + "_EffectiveR")
    end = time.perf_counter()
    totalTimeSeconds = end - start
    totalTimeMinutes = totalTimeSeconds / 60
    totalTimeHours = totalTimeMinutes / 60
    print("Total time: {} seconds or {} minutes or {} hours.".format(totalTimeSeconds,
                                                                totalTimeMinutes,
                                                                totalTimeHours))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values for R0 to analyze")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.R0s, args.poolSize)

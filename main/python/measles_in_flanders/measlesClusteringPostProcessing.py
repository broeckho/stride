import argparse
import time

from postprocessing import AgeImmunity, ExtinctionThreshold, OutbreakEvolution, OutbreakOccurrenceAndSize

SCENARIO_NAMES = ["UN_NCLU", "AD_NCLU", "UN_CLUS", "AD_CLUS"]
SCENARIO_DISPLAY_NAMES = ["Uniform immunity\n+ no clustering",
                            "Age-dependent immunity\n+ no clustering",
                            "Uniform immunity\n+ household-based clustering",
                            "Age-dependent immunity\n+ household-based clustering"]

def main(outputDir, numDays, R0s, extinctionThreshold, poolSize):
    start = time.perf_counter()
    # TODO overview plot for outbreak occurrence probabilities
    # TODO overview plot for sizes of outbreaks?
    # TODO effective Rs vs R0s
    # TODO escape probabilities vs R0s
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
        # TODO Infected by age overview?
        for scenario in scenarioNames:
            avgImmunityRate = AgeImmunity.getAvgOverallImmunityRate(outputDir, scenario, poolSize)
            print("Avg overall immunity rate for {} with R0 {} is {}".format(scenario, R0, avgImmunityRate))
            # TODO cumulative cases per day?
            # TODO new cases per day?
            # TODO infected by age?
    end = time.perf_counter()
    totalTimeSeconds = end - start
    totalTimeMinutes = totalTimeSeconds / 60
    totalTimeHours = totalTimeMinutes / 60
    print("Total time: {} seconds or {} minutes or {} hours".format(totalTimeSeconds,
                                                                totalTimeMinutes,
                                                                totalTimeHours))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("--numDays", type=int, default=365, help="total simulation days")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 13], help="Values of R0 to test")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.numDays, args.R0s, args.extinctionThreshold, args.poolSize)

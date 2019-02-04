import argparse
import itertools
import time

from postprocessing import AgeImmunity, ExtinctionThreshold, InfectedByAge, OutbreakEvolution, OutbreakOccurrenceAndSize

def main(outputDir, years, R0s, numDays, extinctionThreshold, poolSize):
    start = time.perf_counter()
    AgeImmunity.createAgeDistributionPlots(outputDir, "data/pop_flanders600.csv")
    # Overview plot for outbreak occurrence probabilities
    OutbreakOccurrenceAndSize.createOutbreakOccurrenceOverviewPlot(outputDir,
                                            R0s, years, [str(y) for y in years],
                                            numDays, extinctionThreshold, poolSize)
    # Overview plot for outbreak sizes
    OutbreakOccurrenceAndSize.createFinalSizesSideBySidePlot(outputDir, R0s,
                                            years, numDays, extinctionThreshold, poolSize)
    for R0 in R0s:
        for year in years:
            # Calculate average overall immunity
            avgImmunityRate = AgeImmunity.getAvgOverallImmunityRate(outputDir,
                                            str(year) + "_R0_" + str(R0), poolSize)
            print("Average immunity level for {} with R0 {} is {}".format(year, R0, avgImmunityRate))
        scenarioNames = [str(y) + "_R0_" + str(R0) for y in years]
        scenarioDisplayNames = [str(y) for y in years]
        # Age-immunity plot
        AgeImmunity.createAgeImmunityPlot(outputDir, scenarioNames,
                                        scenarioDisplayNames, poolSize,
                                        "R0_" + str(R0) + "_AgeImmunityPlot")
        # Plot to determine extinction threshold
        ExtinctionThreshold.createFinalSizesHistogram(outputDir, scenarioNames,
                                        scenarioDisplayNames, numDays, poolSize,
                                        "R0_" + str(R0) + "_ExtinctionThreshold")
        # Infected by age plots
        InfectedByAge.createInfectedByAgeOverviewPlot(outputDir, scenarioNames,
                                        scenarioDisplayNames, extinctionThreshold,
                                        poolSize, "R0_" + str(R0) + "_InfectedByAge")
        InfectedByAge.createInfectedFractionByAgeOverviewPlot(outputDir, scenarioNames,
                                        scenarioDisplayNames, extinctionThreshold,
                                        poolSize, "R0_" + str(R0) + "_InfectedFractionByAge")
        InfectedByAge.createFractionOfInfectedByAgeOverviewPlot(outputDir, scenarioNames,
                                        scenarioDisplayNames, extinctionThreshold,
                                        poolSize, "R0_" + str(R0) + "_FractionOfInfectedByAge")

    end = time.perf_counter()
    totalTimeSeconds = end - start
    totalTimeMinutes = totalTimeSeconds / 60
    totalTimeHours = totalTimeMinutes / 60
    print("Total time: {} seconds or {} minutes or {} hours".format(totalTimeSeconds,
                                                                totalTimeMinutes,
                                                                totalTimeHours))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory with simulation output")
    parser.add_argument("--years", type=int, nargs="+", default=[2013, 2020], help="Years for which was simulated")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12], help="R0 values for which was simulated")
    parser.add_argument("--numDays", type=int, default=730, help="Total simulation days")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.years, args.R0s, args.numDays, args.extinctionThreshold, args.poolSize)

import argparse
import itertools
import time

from postprocessing import AgeImmunity, ExtinctionThreshold, OutbreakEvolution, OutbreakOccurrenceAndSize

def main(outputDir, years, R0s, numDays, extinctionThreshold, poolSize):
    start = time.perf_counter()
    for R0 in R0s:
        for year in years:
            scenarioName = str(year) + "_R0_" + str(R0)
            # Calculate average overall immunity
            avgImmunityRate = AgeImmunity.getAvgOverallImmunityRate(outputDir, scenarioName, poolSize)
            print("Avg overall immunity rate for {} with R0 {} is {}".format(year, R0, avgImmunityRate))
            # Create extinction threshold histogram
            ExtinctionThreshold.createFinalSizeHistogram(outputDir, scenarioName, numDays,
                                        poolSize, scenarioName + "_ExtinctionThreshold.png")
            # Create outbreak evolution plots
            OutbreakEvolution.createCumulativeCasesPerDayPlot(outputDir, scenarioName, numDays,
                                        extinctionThreshold, poolSize,
                                        scenarioName + "_CumulativeCases.png")
            OutbreakEvolution.createNewCasesPerDayPlot(outputDir, scenarioName, numDays,
                                        extinctionThreshold, poolSize,
                                        scenarioName + "_NewCases.png")
        scenarioNames = [str(y) + "_R0_" + str(R0) for y in years]
        scenarioDisplayNames = [str(y) for y in years]
        # Create age-immunity plot
        AgeImmunity.createAgeImmunityPlots(outputDir, scenarioNames, scenarioDisplayNames,
                                        poolSize, "R0_" + str(R0) + "_AgeImmunityPlot.png")
        ExtinctionThreshold.createFinalSizesHistogram(outputDir, scenarioNames, scenarioDisplayNames,
                                        numDays, poolSize, "R0_" + str(R0) + "_ExtinctionThreshold.png")
        OutbreakOccurrenceAndSize.createOutbreakOccurrencePlot(outputDir, [str(y) + "_R0_" + str(R0) for y in years],
                                        [str(y) for y in years], numDays, extinctionThreshold,
                                        poolSize, "R0_" + str(R0) + "_OutbreakOccurrence.png")
        OutbreakOccurrenceAndSize.createFinalSizesBoxplot(outputDir, [str(y) + "_R0_" + str(R0) for y in years],
                                        [str(y) for y in years], numDays, extinctionThreshold,
                                        poolSize, "R0_" + str(R0) + "_OutbreakSizes.png")
    '''
    #TODO Effective Rs -> VS R0'''
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
    parser.add_argument("--numDays", type=int, default=365, help="Total simulation days")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Threshold for epidemic extinction")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.years, args.R0s, args.numDays, args.extinctionThreshold, args.poolSize)

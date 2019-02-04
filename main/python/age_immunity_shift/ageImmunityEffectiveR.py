import argparse
import itertools
import time

from postprocessing import EffectiveR

def main(outputDir, years, R0s, poolSize):
    start = time.perf_counter()
    EffectiveR.createEffectiveROverviewPlot(outputDir, years,
                                        [str(y) for y in years], R0s, poolSize,
                                        "AllEffectiveR_Mean")
    EffectiveR.createEffectiveROverviewPlot(outputDir, years,
                                        [str(y) for y in years], R0s, poolSize,
                                        "AllEffectiveR_Median",
                                        stat="median")
    for R0 in R0s:
        scenarioNames = [str(y) + "_R0_" + str(R0) for y in years]
        scenarioDisplayNames = [str(y) for y in years]
        EffectiveR.createEffectiveRPlot(outputDir, scenarioNames,
                                        scenarioDisplayNames, poolSize,
                                        "R0_" + str(R0) + "_EffectiveR")
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
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.years, args.R0s, args.poolSize)

import argparse
import time

from postprocessing import EffectiveR

def main(outputDir, R0s, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM_NOCLUSTERING", "AGEDEPENDENT_NOCLUSTERING", "AGEDEPENDENT_CLUSTERING"]
    scenarioDisplayNames = ["A", "B", "C"]
    # TODO Overview of different R0 values vs effective Rs
    for R0 in R0s:
        scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
        EffectiveR.createEffectiveRPlot(outputDir, scenarioNamesFull,
                                            scenarioDisplayNames, poolSize,
                                            "Scenario",
                                            "R0_" + str(R0) + "_EffectiveR")
    end = time.perf_counter()
    totalTime = end - start
    hours = int(totalTime / 3600)
    minutes = int((totalTime - (hours * 3600)) / 60)
    seconds = (totalTime - (hours * 3600) - (minutes * 60))
    print("Total time: {} hours, {} minutes and {} seconds".format(hours, minutes, seconds))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output")
    parser.add_argument("--R0s", type=int, nargs="+", default=[12, 18], help="Values of R0 to analyze")
    parser.add_argument("--poolSize", type=int, default=8, help="Number of workers in pool for multiprocessing")
    args = parser.parse_args()
    main(args.outputDir, args.R0s, args.poolSize)

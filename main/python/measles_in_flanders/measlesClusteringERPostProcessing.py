import argparse
import time

from postprocessing import AgeImmunity
from postprocessing import EffectiveR

def main(outputDir, R0s, poolSize):
    start = time.perf_counter()
    scenarioNames = ["UNIFORM_NOCLUSTERING", "AGEDEPENDENT_NOCLUSTERING", "UNIFORM_CLUSTERING", "AGEDEPENDENT_CLUSTERING"]
    scenarioDisplayNames = ["A", "B", "C", "D"]
    # Overview of different R0 values vs effective Rs
    EffectiveR.createEffectiveROverviewPlot(outputDir, scenarioNames,
                                                scenarioDisplayNames, R0s,
                                                poolSize, "Scenario",
                                                "All_EffectiveR_Mean")
    EffectiveR.createEffectiveROverviewPlot(outputDir, scenarioNames,
                                                scenarioDisplayNames, R0s,
                                                poolSize, "Scenario",
                                                "All_EffectiveR_Median",
                                                stat="median")
    for R0 in R0s:
        scenarioNamesFull = [s + "_R0_" + str(R0) for s in scenarioNames]
        AgeImmunity.createAgeImmunityPlot(outputDir, scenarioNamesFull, scenarioDisplayNames,
                                            poolSize, "R0_" + str(R0) + "_AgeImmunityPlot",
                                            targetRatesFile="2020_measles_immunity.xml")
        EffectiveR.createEffectiveRPlot(outputDir, scenarioNamesFull,
                                            scenarioDisplayNames, poolSize,
                                            "Scenario",
                                            "R0_" + str(R0) + "_EffectiveR")
        for scenario in scenarioNamesFull:
            AgeImmunity.createHouseholdConstitutionPlot(outputDir, scenario,
                                                poolSize,
                                                "R0_" + str(R0) + "_" + scenario + "_HHConstitutionPlot")

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

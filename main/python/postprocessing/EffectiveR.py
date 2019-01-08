import multiprocessing
import os

from .Util import getRngSeeds

"""
import matplotlib.pyplot as plt

def main(outputDir, numDays):
            infectors = {}
            with open(transmissionsFile) as f:
                for line in f:
                    infectorId = int(transmissionInfo[2])
                    simDay = int(transmissionInfo[6])
                    if simDay < numDays and transmissionInfo[0] == "[TRAN]":
                        if infectorId in infectors:
                            infectors[infectorId] += 1
                        else:
                            infectors[infectorId] = 1
            avgR = sum(list(infectors.values())) / (getFinalOutbreakSize(outputDir, scenario, s, numDays - 7))
            scenarioRs.append(avgR)
        allRs.append(scenarioRs)
    plt.boxplot(allRs, labels=scenarioDisplayNames)
    plt.ylabel("Effective R")
    plt.savefig(os.path.join(outputDir, "EffectiveRs.png"))
    plt.clf()
"""

def getEffectiveR(outputDir, scenarioName, seed, numDays):
    transmissionsFile = os.path.join(outputDir, scenarioName + "_" + str(seed) + "_contact_log.txt")
    with open(transmissionsFile) as f:
        '''
        [TRAN INFECTOR_ID INFECTED_ID INFECTOR_AGE INFECTED_AGE CLUSTER_TYPE SIMULATION_DY]

                static void Trans(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                          ContactPoolType::Id type, unsigned short int sim_day)
        {
                logger->info("[TRAN] {} {} {} {} {} {}", p2->GetId(), p1->GetId(),
                		          p2->GetAge(), p1->GetAge(), ToString(type), sim_day);
        }
        '''
        for line in f:
            transmissionInfo = line.split(" ")
            print(transmissionInfo)

def createEffectiveRBoxplot(outputDir, scenarioNames, scenarioDisplayNames, numDays, poolSize, figName):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        with multiprocessing.Pool(processes=poolSize) as pool:
            effectiveRs = pool.starmap(getEffectiveR, [(outputDir, scenario, s, numDays) for s in seeds])

#TODO 2D effective R
#TODO 3D effective R

'''
def postprocessing(numRunsR0, numRunsFull, transmissionProbabilities0to1,
    transmissionProbabilitiesRestricted, clusteringLevels, extinctionThreshold, poolSize):
    outputDir = "."
    scenarioName = "AGEDEPENDENT"
    numDaysR0 = 30
    numDaysFull = 60
    if numRunsR0 > 0:
        print("R0 postprocessing")
        fitCoeffs = R0.getLnFit(outputDir, transmissionProbabilities0to1, poolSize)

        R0.createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir,
                                transmissionProbabilities0to1, fitCoeffs[0],
                                fitCoeffs[1], poolSize)

        R0.createTransmissionProbabilityVSSecondaryCasesBoxplots(outputDir,
                                transmissionProbabilities0to1, poolSize)

    if numRunsFull > 0:
        print("Full scenario postprocessing")
        AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            poolSize, targetLevelsDir="../../Workspace/MeaslesInFlanders_Paper/ProjectedImmunity")
        Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels, poolSize)
        Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels, poolSize, ageLim=36)

        EffectiveR.createEffectiveR3DScatterPlot(outputDir, scenarioName, transmissionProbabilitiesRestricted,
                            clusteringLevels, poolSize)
        EffectiveR.createEffectiveR3DBarPlot(outputDir, scenarioName, transmissionProbabilitiesRestricted,
                            clusteringLevels, poolSize)
        EffectiveR.createEffectiveRHeatmap(outputDir, scenarioName, transmissionProbabilitiesRestricted,
                            clusteringLevels, poolSize)

        EscapeProbability.createEscapeProbabilityHeatmapPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, poolSize)

        InfectedByAge.meanAgeOfInfectionHeatmap(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, poolSize)

        OutbreakOccurrence.createOutbreakProbability3DPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, extinctionThreshold, poolSize)
        OutbreakOccurrence.createOutbreakProbabilityHeatmap(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, extinctionThreshold, poolSize)
        OutbreakSize.createOutbreakSizes3DPlot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, clusteringLevels,
                            numDaysFull, extinctionThreshold, poolSize)
        for level in clusteringLevels:
            AgeImmunity.createAgeImmunityBoxplot(outputDir, scenarioName,
                            transmissionProbabilitiesRestricted, level, poolSize)

        for prob in transmissionProbabilitiesRestricted:
            EffectiveR.createEffectiveRPlot(outputDir, scenarioName, prob,
                            clusteringLevels, poolSize)
            OutbreakOccurrence.createOutbreakProbabilityPlot(outputDir, scenarioName, prob,
                            clusteringLevels, numDaysFull, extinctionThreshold, poolSize)
            OutbreakSize.createExtinctionThresholdHistogram(outputDir, scenarioName,
                            prob, clusteringLevels, numDaysFull, poolSize)
            OutbreakSize.createOutbreakSizesPlot(outputDir, scenarioName, prob,
                            clusteringLevels, numDaysFull, extinctionThreshold, poolSize)

            # TODO more scientific way to estimate extinction threshold?
            EscapeProbability.createEscapeProbabilityPlot(outputDir, scenarioName,
                            prob, clusteringLevels, numDaysFull, poolSize)
            for level in clusteringLevels:
                OutbreakEvolution.createNewCasesPerDayPlot(outputDir, scenarioName,
                                    prob, level, numDaysFull, poolSize)
                OutbreakEvolution.createCumulativeCasesPerDayPlot(outputDir, scenarioName,
                                    prob, level, numDaysFull, poolSize)
'''

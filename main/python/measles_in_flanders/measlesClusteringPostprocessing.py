from postprocessing import AgeImmunity, Clustering, EffectiveR, EscapeProbability, InfectedByAge, OutbreakOccurrence, OutbreakSize, R0

def r0Postprocessing(outputDir, transmissionProbabilities, poolSize):
    fitCoeffs = R0.getLnFit(outputDir, transmissionProbabilities, poolSize)
    R0.createTransmissionProbabilityVSSecondaryCasesScatterPlot(outputDir,
                                        transmissionProbabilities, fitCoeffs[0],
                                        fitCoeffs[1], poolSize)
    R0.createTransmissionProbabilityVSSecondaryCasesBoxplots(outputDir,
                                        transmissionProbabilities, poolSize)

def effectiveRPostprocessing(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, poolSize):

    EffectiveR.createEffectiveRHeatmap(outputDir, scenarioName,
                                            transmissionProbabilities,
                                            clusteringLevels, poolSize)
    EffectiveR.createEffectiveR3DBarPlot(outputDir, scenarioName,
                                            transmissionProbabilities,
                                            clusteringLevels, poolSize)
    for prob in transmissionProbabilities:
        EffectiveR.createEffectiveRBoxplot(outputDir, scenarioName,
                                            prob, clusteringLevels, poolSize)

def immunityProfilePostprocessing(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, poolSize):
    AgeImmunity.createAgeImmunityOverviewPlot(outputDir, scenarioName, transmissionProbabilities,
                                                clusteringLevels, poolSize,
                                                targetLevelsDir="../../Workspace/MeaslesInFlanders_Paper/ProjectedImmunity")
    Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize, ageLim=99)
    Clustering.createAssortativityCoefficientPlot(outputDir, scenarioName, transmissionProbabilities, clusteringLevels, poolSize, ageLim=35)
    for level in clusteringLevels:
        AgeImmunity.createAgeImmunityBoxplot(outputDir, scenarioName,
                                        transmissionProbabilities, level, poolSize)

def outbreakOccurrencePostprocessing(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    OutbreakOccurrence.createOutbreakProbability3DPlot(outputDir, scenarioName,
                        transmissionProbabilities, clusteringLevels,
                        numDays, extinctionThreshold, poolSize)
    OutbreakOccurrence.createOutbreakProbabilityHeatmap(outputDir, scenarioName,
                        transmissionProbabilities, clusteringLevels,
                        numDays, extinctionThreshold, poolSize)
    for prob in transmissionProbabilities:
        OutbreakOccurrence.createOutbreakProbabilityPlot(outputDir, scenarioName, prob,
                        clusteringLevels, numDays, extinctionThreshold, poolSize)

def outbreakSizePostprocessing(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, extinctionThreshold, poolSize):
    EscapeProbability.createEscapeProbabilityHeatmapPlot(outputDir, scenarioName,
                    transmissionProbabilities, clusteringLevels, numDays, poolSize)
    OutbreakSize.createOutbreakSizes3DPlot(outputDir, scenarioName,
                    transmissionProbabilities, clusteringLevels, numDays,
                    extinctionThreshold, poolSize)
    for prob in transmissionProbabilities:
        EscapeProbability.createEscapeProbabilityBoxplot(outputDir, scenarioName,
                        prob, clusteringLevels, numDays, poolSize)
        OutbreakSize.createOutbreakSizesBoxplot(outputDir, scenarioName, prob,
                        clusteringLevels, numDays, extinctionThreshold, poolSize)
        OutbreakSize.createExtinctionThresholdHistogram(outputDir, scenarioName,
                        prob, clusteringLevels, numDays, poolSize)

def agesOfInfectedPostprocessing(outputDir, scenarioName, transmissionProbabilities,
    clusteringLevels, numDays, poolSize):
        InfectedByAge.meanAgeOfInfectionHeatmap(outputDir, scenarioName,
                            transmissionProbabilities, clusteringLevels,
                            numDays, poolSize)

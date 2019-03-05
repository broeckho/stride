import csv
import os
import xml.etree.ElementTree as ET

MAX_AGE = 99

################################################################################
# Functions for generating rng seeds.                                          #
################################################################################

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def writeRngSeeds(scenarioName, R0, seeds):
    with open(scenarioName + "_R0_" + str(R0) + "_seeds.csv", "w") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

################################################################################
# Functions for calculating uniform immunity level in population.              #
################################################################################
def getImmunityLevels(xmlTree):
    age = 0
    levels = [0] * (MAX_AGE + 1)
    for child in xmlTree:
        if child.tag not in ["data_source", "data_manipulation"]:
            levels[age] = float(child.text)
            age += 1
    return levels

def calculateUniformImmunityLevel(populationFile, immunityFileChildren, immunityFileAdults):
    totalsByAge = [0] * (MAX_AGE + 1)
    with open(populationFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(row["age"])
            totalsByAge[age] += 1
    childLevels = getImmunityLevels(ET.parse(immunityFileChildren).getroot())
    adultLevels = getImmunityLevels(ET.parse(immunityFileAdults).getroot())
    immunityByAge = [x + y for x,y in zip(childLevels, adultLevels)]
    return sum([immunityByAge[i] * totalsByAge[i] for i in range(MAX_AGE + 1)]) / sum(totalsByAge)

def createUniformImmunityDistributionFiles(immunityLevel):
    uniformChildImmunity = ET.Element('immunity')
    uniformAdultImmunity = ET.Element('immunity')
    for age in range(18):
        elemChild = ET.SubElement(uniformChildImmunity, 'age' + str(age))
        elemChild.text = str(immunityLevel)
        elemAdult = ET.SubElement(uniformAdultImmunity, 'age' + str(age))
        elemAdult.text = str(0)
    for age in range(18, MAX_AGE + 1):
        elemChild = ET.SubElement(uniformChildImmunity, 'age' + str(age))
        elemChild.text = str(0)
        elemAdult = ET.SubElement(uniformAdultImmunity, 'age' + str(age))
        elemAdult.text = str(immunityLevel)

    ET.ElementTree(uniformChildImmunity).write(os.path.join('data', 'measles_uniform_child_immunity.xml'))
    ET.ElementTree(uniformAdultImmunity).write(os.path.join('data', 'measles_uniform_adult_immunity.xml'))

import argparse
import matplotlib.pyplot as plt
import os
import xml.etree.ElementTree as ET

def createFromSummary(fileName, maxAge):
    ages = []
    with open(fileName, encoding="ISO-8859-1") as f:
        next(f) # Skip line with municipality names
        for line in f:
            line = [float(x) for x in line.split(" ")]
            ages.append(sum(line) / len(line)) # calculate mean over all municipalities
        while len(ages) < (maxAge + 1):
            ages.append(ages[-1])
    return ages

def createFromMultipleFiles(dataDir, year, numMunicipalities, maxAge):
    ages = []
    immunityDir = os.path.join(dataDir, "BootstrapSusceptibility" + str(year))
    for i in range(maxAge + 1):
        ages.append([])
    for mun in range(1, numMunicipalities + 1):
        immunityFile = os.path.join(immunityDir, "bootstrap.susceptibility"
                                + str(year) + ".municipality" + str(mun) + ".txt")
        with open(immunityFile) as f:
            for line in f:
                line = [float(x) for x in line.split(" ")]
                for age in range(maxAge + 1):
                    if age < len(line):
                        ages[age].append(line[age])
                    else:
                        ages[age].append(line[len(line) - 1])
    ages = [sum(x) / len(x) for x in ages] # calculate means over all runs and municipalities
    return ages

def toFile(year, immunityRates, targetDir, maxAge):
    immunity = ET.Element('immunity')
    dataSource = ET.SubElement(immunity, 'data_source')
    dataSource.text = "Hens et al. (2015) Eurosurveillance"
    dataManipulation = ET.SubElement(immunity, 'data_manipulation')
    dataManipulation.text = "Average by age"
    for age in range(maxAge + 1):
        elem = ET.SubElement(immunity, 'age' + str(age))
        elem.text = str(immunityRates[age])
    ET.ElementTree(immunity).write(os.path.join(targetDir, str(year) + "_measles_immunity.xml"))

def plotTargetRates(rates, years):
    linestyles = ['-', '--', '-.', ':', '--', '--']
    dashes = [None, (2, 5), None, None, (5, 2), (1, 3)]
    colors = ['blue', 'orange', 'green', 'red', 'purple', 'brown']
    for i in range(len(rates)):
        d = dashes[i]
        if d is not None:
            plt.plot(rates[i], linestyle=linestyles[i], dashes=dashes[i], color=colors[i])
        else:
            plt.plot(rates[i], linestyle=linestyles[i], color=colors[i])
    plt.xlabel("Age")
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    plt.legend(years)
    plt.savefig("TargetRates.eps", format='eps', dpi=1000)
    plt.clf()

def main(dataDir, targetDir, years):
    maxAge = 99
    numMunicipalities = 593
    allRates = []
    for year in years:
        if year == 2013:
            ageImmunity = createFromSummary(os.path.join(dataDir, "Susceptibility_Belgium_2013.txt"), maxAge)
        else:
            ageImmunity = createFromMultipleFiles(dataDir, year, numMunicipalities, maxAge)
        allRates.append(ageImmunity)
        toFile(year, [(1 - x) for x in ageImmunity], targetDir, maxAge)
    plotTargetRates(allRates, years)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dataDir", type=str, help="Directory with data on immunity rates")
    parser.add_argument("targetDir", type=str, help="Where to save resulting immunity files")
    parser.add_argument("--years", type=int, nargs="+", default=[2013, 2020, 2025, 2030, 2035, 2040], help="For which years to create immunity files")
    args = parser.parse_args()
    main(args.dataDir, args.targetDir, args.years)

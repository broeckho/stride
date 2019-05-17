import argparse
import matplotlib.pyplot as plt
import numpy as np
import os

from postprocessing.Util import saveFig

def main(inputDir):
    ages = {}
    for age in range(86):
        ages[age] = []
    year = 2020
    yearDir = os.path.join(inputDir, "BootstrapSusceptibility" + str(year))
    for i in range(1, 501):
        immunityFile = os.path.join(yearDir, "bootstrap.susceptibility" + str(year) + ".municipality" + str(i) + ".txt")
        with open(immunityFile) as f:
            for line in f:
                line = line.split(" ")
                for age in range(len(line)):
                    ages[age].append(float(line[age]))
    means = []
    lower = []
    upper = []
    for age in ages:
        mean = sum(ages[age]) / len(ages[age])
        ll = np.percentile(ages[age], 2.5)
        hh = np.percentile(ages[age], 97.5)
        means.append(mean)
        lower.append(ll)
        upper.append(hh)
    for age in range(86, 100):
        means.append(means[-1])
        lower.append(lower[-1])
        upper.append(upper[-1])
    plt.plot(range(100), means, color="red")
    plt.plot(range(100), lower, linestyle="--", color="green")
    plt.plot(range(100), upper, linestyle="--", color="orange")
    plt.fill_between(range(100), lower, upper)
    plt.xlabel("Age (in years)")
    plt.xlim(0, 100)
    plt.ylabel("Fraction susceptible")
    plt.ylim(0, 1)
    plt.legend(["Mean", "Lower 2.5%", "Upper 2.5%"])
    saveFig(".", "ImmunityProjections")

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("inputDir", type=str, help="Directory containing immunity files.")
    args = parser.parse_args()
    main(args.inputDir)

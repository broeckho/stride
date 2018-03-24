#!/usr/bin/python
#############################################################################
#  This file is part of the Stride software.
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or any
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#############################################################################

"""
Pyhon code to create visualisation of infection spread on map.
Author: Elise Kuylen (2016)
"""

import sys
import smopy
import random
import csv
from math import log
from prepare_csv import prepare_csv_transmissions
from matplotlib import pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
from matplotlib import cm
from numpy.random import randn
from matplotlib.colors import ListedColormap


def create_map(district_file, pop_file, households_file, persons_file, transmissions_log):
    """
    Create .png map image for each simulation day in the given files.
    """

    ########################################################################
    # Gather information (location & population size) about the districts. #
    ########################################################################

    districts = {}
    districts[0] = (0, 0, 0)

    latitudes = []
    longitudes = []

    with open(district_file, 'r') as district_file:
        reader = csv.DictReader(district_file, delimiter=";")
        for row in reader:
            d_id = int(row['id'])
            latitude = float(row['latitude'].replace(",", "."))
            longitude = float(row['longitude'].replace(",", "."))
            pop_size = int(row['pop_size'])

            # Don't add districts with incorrect location data
            if abs(latitude) < 90 and abs(longitude) < 180:
                districts[d_id] = (latitude, longitude, pop_size)
                latitudes.append(latitude)
                longitudes.append(longitude)


    # Use Smopy to get map image for the given region (calculated from min/max latitudes/longitudes in districts file)
    region_map = smopy.Map((min(latitudes), min(longitudes), max(latitudes), max(longitudes)), z=9)


    ##################################################################
    # Gather information about persons and households in simulation. #
    ##################################################################

    # Check how long each person stays ill (= infectious) when infected.
    person_infectiousness = {}
    with open(persons_file, 'r') as person_file:
        reader = csv.DictReader(person_file, delimiter=';')
        for row in reader:
            person_infectiousness[int(row['id'])] = row['end_infectiousness']

    # For each person, fetch the household ID
    person_households = []
    with open (pop_file, 'r') as pop_file:
        reader = csv.DictReader(pop_file, delimiter=";")
        for row in reader:
            person_households.append(int(float(row['household_id'])))

    # For each household, fetch the district it belongs to.
    households = {}
    with open(households_file, 'r') as hh_file:
        reader = csv.DictReader(hh_file, delimiter=';')
        for row in reader:
            hh_id = int(row['id'])
            d_id = int(row['district_id'])
            households[hh_id] = d_id


    ########################################
    # Gather information about infections. #
    ########################################

    prepare_csv_transmissions(transmissions_log, "infections.csv")

    infections = {}
    max_infect = 0

    with open('./infections.csv', 'r') as infections_file:
        reader = csv.DictReader(infections_file, delimiter=",")
        for row in reader:
            person_id = int(row['person_id'])
            if (person_id < len(person_infectiousness) + 1):
                hh_id =  person_households[person_id]
                district = households[hh_id]
                start_infected = int(row['begin_infection'])
                end_infected = start_infected + int(person_infectiousness[person_id])

                for day in range(start_infected, end_infected + 1):
                    if day in infections:
                        # add extra infection
                        if district in infections[day]:
                            infections[day][district] += 1
                            if infections[day][district] > max_infect:
                                max_infect = infections[day][district]
                        else:
                            infections[day][district] = 1
                    else:
                        # add first infection
                        infections[day] = {}
                        infections[day][district] = 1

    #########################################################
    # Create the actual map images for each simulation day. #
    #########################################################

    for day in range(0, 100):
        ax = region_map.show_mpl()

        cases = False
        if (day in infections):
            cases = True
            day_infections = infections[day]

        for i, district_info in districts.iteritems():
            x, y = region_map.to_pixels(district_info[0], district_info[1])

            # Calculate circle size from district size
            district_size = district_info[2]
            circle_size = 0

            if (district_size <= 224):
                # first quartile
                circle_size = 2
            elif (district_size <= 327):
                # second quartile
                circle_size = 4
            elif (district_size <= 525):
                # third quartile
                circle_size = 8
            elif (district_size <= 5000):
                # 4th quartile split in two, to accentuate really big districts
               circle_size = 16
            else:
                circle_size = 32


            # no infections -> districts are not shown
            # otherwise: spectrum from red (few infections) to yellow (lot of infections)

            if (cases) and (i in day_infections):
                num_infections = day_infections[i]
                R = 1
                # normalize # of infections to number between 0 and 1
                #G = float(num_infections) / max_infect
                G = 1 - (float(num_infections) / district_size)
                B = 0

                col = (R, G, B)

                ax.plot(x, y, marker='o', color=col, ms=circle_size, mew=1)

        #############################################
        # Create legends for circle sizes & colors. #
        #############################################

        l1 = plt.scatter([],[], s=4, facecolors='none', edgecolors='black')
        l2 = plt.scatter([],[], s=16, facecolors='none', edgecolors='black')
        l3 = plt.scatter([],[], s=64, facecolors='none', edgecolors='black')
        l4 = plt.scatter([],[], s=256, facecolors='none', edgecolors='black')
        l5 = plt.scatter([],[], s=1024, facecolors='none', edgecolors='black')

        labels = ["< 224", "< 327", "< 525", "< 5000", "> 5000"]

        leg = plt.legend([l1, l2, l3, l4, l5], labels, ncol=5, frameon=False, fontsize=10,
        handlelength=2, loc = 8,
        handletextpad=1, title='District sizes', scatterpoints = 1, bbox_to_anchor=(0.5, -0.15))

        leg.get_title().set_position((0, -50))

        data_range = [0, 100]
        m = cm.ScalarMappable(cmap=cm.autumn_r)
        m.set_array(data_range)
        c = plt.colorbar(m, orientation="horizontal")
        c.set_label("Fraction of district infected")

        ###########################
        # Save map image to .png. #
        ###########################

        fig_title = "day " + str(day)
        plt.title(fig_title)
        fig_file = "map_" + str(day)  + ".png"
        plt.savefig(fig_file)


def main(argv):
    if (len(argv) == 5):
        try:
            create_map(argv[0], argv[1], argv[2], argv[3], argv[4])
        except KeyError:
            print "Note that all .csv input files should have delimiter ';'."

    else:
        print("Usage: python plot_maps.py <districts_file> <population_file> <households_file> <persons_file> <transmissions_log>")



if __name__ == "__main__":
    main(sys.argv[1:])


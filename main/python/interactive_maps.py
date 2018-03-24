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
Python code to create an interactive visual represenation of the spread of a disease.

Author: Elise Kuylen
"""

import sys
import smopy
import random
import csv
import numpy as np
from math import log
from prepare_csv import prepare_csv_transmissions
from matplotlib import cm
from matplotlib import pyplot as plt
from matplotlib.widgets import Slider, Button, RadioButtons


def create_slider(districts_file, population_file, households_file, persons_file, transmission_log):
    """
    Create plot with interactive slider.
    """
    #######################################
    # Gather information about districts. #
    #######################################

    districts = {}
    districts[0] = (0,0,0)

    latitudes = []
    longitudes = []

    with open(districts_file, 'r') as district_file:
        reader = csv.DictReader(district_file, delimiter=';')
        for row in reader:
            d_id = int(row['id'])
            x = float(row['latitude'].replace(",", "."))
            y = float(row['longitude'].replace(",", "."))
            pop_size = int(row['pop_size'])

            # Don't add districts with incorrect location data
            if abs(x) < 90 and abs(y) < 180:
                latitudes.append(x)
                longitudes.append(y)
                districts[d_id] = (x, y, pop_size)

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
    with open (population_file, 'r') as pop_file:
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

    prepare_csv_transmissions(transmission_log, "infections.csv")

    infections = {}
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
                        else:
                            infections[day][district] = 1
                    else:
                        # add first infection
                        infections[day] = {}
                        infections[day][district] = 1

    ##################
    # Create figure. #
    ##################

    fig, ax = plt.subplots()
    fig.canvas.set_window_title("Evolution of disease spread")

    plt.subplots_adjust(bottom=0.25)

    # Legend for circles sizes (representing district population sizes).
    l1 = plt.scatter([],[], s=4, facecolors='none', edgecolors='black')
    l2 = plt.scatter([],[], s=16, facecolors='none', edgecolors='black')
    l3 = plt.scatter([],[], s=64, facecolors='none', edgecolors='black')
    l4 = plt.scatter([],[], s=256, facecolors='none', edgecolors='black')
    l5 = plt.scatter([],[], s=1024, facecolors='none', edgecolors='black')

    labels = ["< 224", "< 327", "< 525", "< 5000", "> 5000"]

    leg = plt.legend([l1, l2, l3, l4, l5], labels, ncol=5, frameon=False, fontsize=10,
    handlelength=2, loc = 8, bbox_to_anchor = [0.5,-0.15],
    handletextpad=1, title='District sizes', scatterpoints = 1)
    leg.get_title().set_position((0, -35))

    # Scatterplot to represent districts + fraction of infected per district.
    region_map.show_mpl(ax)
    l = plt.scatter([], [], alpha=0.5)
    l.axes.get_xaxis().set_ticks([])
    l.axes.get_yaxis().set_ticks([])

    axcolor = 'lightgoldenrodyellow'
    ax_day = plt.axes([0.12, 0.1, 0.79, 0.03], axisbg=axcolor)
    sday = Slider(ax_day, 'Day', 0.1, 100.0, valinit=1, valfmt='%0.0f')

    plt.subplots_adjust(bottom=0.15)

    # Legend for colors (representing fraction of infected in district).
    data_range = [0, 100]
    m = cm.ScalarMappable(cmap=cm.autumn_r)
    m.set_array(data_range)
    c = plt.colorbar(m,ax=ax, shrink=0.5, orientation="horizontal")
    c.set_label("Fraction of district infected")

    ###############################
    # Update function for slider. #
    ###############################

    def update(val):
        """
        Update the plot according to the value of the slider.
        """
        day = int(round(sday.val))

        xs = []
        ys = []
        offsets = []
        sizes = []
        colors = []

        if (day in infections):
            day_infections = infections[day]

            for key, value in districts.iteritems():
                x, y = region_map.to_pixels((value[0], value[1]))
                district_size = value[2]
                circle_size = 0

                if (district_size <= 224):
                    circle_size = 4
                elif (district_size <= 327):
                    circle_size = 16
                elif (district_size <= 525):
                    circle_size = 64
                elif (district_size <= 5000):
                    circle_size = 256
                else:
                    circle_size = 1024

                if key in day_infections:
                    num_infections = day_infections[key]
                    xs.append(x)
                    ys.append(y)
                    offsets.append([x, y])
                    sizes.append(circle_size)
                    colors.append([1, 1 - (float(num_infections) / district_size), 0])

        l.set_offsets(offsets)
        l.set_sizes(sizes)
        l.set_facecolors(np.array(colors))
        fig.canvas.draw_idle()

    # Execute update() when value of slider changes
    sday.on_changed(update)
    plt.show()


def main(argv):
    if (len(argv) == 5):
        try:
            create_slider(argv[0], argv[1], argv[2], argv[3], argv[4])
        except KeyError:
            print "Note that all .csv input files should have delimiter ';'."

    else:
        print("Usage: python slider_maps.py <districts_file> <population_file> <households_file> <persons_file> <transmissions_log>")



if __name__ == "__main__":
    main(sys.argv[1:])

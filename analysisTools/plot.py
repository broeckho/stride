import sys
import os
import csv
from matplotlib import pyplot

cumulative = []
iterative = []
names = []
x = []

if not os.path.exists('plots'):
    os.mkdir('plots')

with open(sys.argv[1], 'r') as csvfile:
    rows = csv.reader(csvfile, delimiter=',')
    index = 0
    for row in rows:
        if index % 5 != 0:
            continue
        if len(cumulative) == 0:
            x = [0]
            for item in row:
                cumulative.append([0])
                iterative.append([0])
                names.append(item)
            continue
        for index in range(0, len(cumulative)):
            cumulative[index].append(int(row[index]))
            iterative[index].append(int(row[index])-int(iterative[index][len(iterative[index])-1]))
        x.append(len(x))
        index += 1

for index in range(0, len(names)):
    pyplot.plot(x, cumulative[index], label=names[index])
pyplot.xlabel('simulation day')
pyplot.ylabel('cummulative cases')
pyplot.savefig('plots/{}_cummulative'.format(sys.argv[2]))
pyplot.show()


for index in range(0, len(names)):
    pyplot.plot(x, iterative[index], label=names[index])
pyplot.xlabel('simulation day')
pyplot.ylabel('new cases per day')
pyplot.savefig('plots/{}_new'.format(sys.argv[2]))
pyplot.show()

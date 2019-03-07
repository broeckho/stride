import sys
import os
import numpy as np
import matplotlib.pyplot as plt

def readArguments(argv):
    filename = ''
    histname = ''
    path = ""
    pop = 0
    filenames = []
    showPlot = False
    
    i = 1
    while i < len(argv):
        if argv[i] == "-f":
            filenames.append(argv[i+1])         
            i=i+2
        elif argv[i] == "-h":
            histname = argv[i+1]
            i=i+2
        elif argv[i] == "-h":
            print("-f [filename]:                       input one filename")
            print("-fn [amount of files] [filenames]:   input multiple filenames at once")
            print("-h [histogram filename]:             histogram filename to write to: .jpg is added")
            i=i+1
        elif argv[i] == "-fn":
            n = int(argv[i+1])
            if len(argv) <= i+n:
                print("Not enough filenames?")
                sys.exit(1)
            for x in range(2, n+2):
                filenames.append(argv[i+x])
            i=i+2+n
            print(filenames)
        elif argv[i] == "--pop":
            pop = int(argv[i+1])
            i=i+2
        elif argv[i] == "--path":
            path = argv[i+1]
            i=i+2
        elif argv[i] == "-p":
            showPlot = True
            i=i+1
        else:
            print("Unknown input: ",argv[i]," -h for help")
            i=i+1
        
    error = False
    if len(filenames) <= 0:
        print("Error: input filename cannot be empty")
        error = True
    elif histname == '':
        histname = filenames[0] + "_histogram"
    if error:
        sys.exit(1)
   
    if path != "":
        for i in range(0,len(filenames)):
            filenames[i] = os.path.join(path,filenames[i])
         
    
    return (filenames, histname, pop, showPlot)
    

#start program    
filenames, histname, pop, showPlot = readArguments(sys.argv)

#count lines:
skiplines = []
for fname in filenames:
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    skiplines.append(i) #i+1 = total number of lines but we skip total-1

print(len(filenames))

data = np.genfromtxt(filenames[0], dtype = int, skip_header=skiplines[0], delimiter=",")
#data.reshape(1,len(data))
for i in range(1,len(filenames)):
    new_data = np.genfromtxt(filenames[i], dtype = int, skip_header=skiplines[i], delimiter=",")
    #new_data.reshape(1,len(data))
    data = np.concatenate((data,new_data))

if pop > 1:
    data = data/pop*100


#Create plot of all data
plt.hist(data, bins='auto')
if pop <= 1:
    plt.xlabel("Infected Cases")
else:
    plt.xlabel("%Infected Cases")
plt.ylabel("Frequency")
plt.title("Histogram of infected cases per outbreak")
plt.savefig(histname+"_all.jpg")
if(showPlot):
    plt.show()

#Remove large breakouts
mean=np.mean(data,axis=0)

#create mask to find all data smaller than mean:
mask=[x < mean for x in data]

smallBreakouts = data[mask]
print(len(data))
print(len(smallBreakouts))

plt.clf()
plt.hist(smallBreakouts, bins='auto')
if pop <= 1:
    plt.xlabel("Infected Cases")
else:
    plt.xlabel("%Infected Cases")
plt.ylabel("Frequency")
plt.title("Histogram of infected cases in small outbreaks")
plt.savefig(histname+"_small.jpg")
if(showPlot):
    plt.show()



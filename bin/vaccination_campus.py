import csv
import matplotlib.pyplot as plt
import os
import sys
import random
from multiprocessing import pool

from pystride.Event import Event, EventType
from pystride.PyController import PyController

xmlFile = "use_vaccinate_on_campus.xml"

def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, filename+"_cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})
        
def plotNewCases(outputPrefix, filename):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    for v in []:
        days = []
        newCasesPerDay = []
        prevCumulativeCases = 0 # Keep track of how many cases have been recorded until current time-step
        with open(os.path.join(outputPrefix + "_" + str(v), filename+"_cases.csv")) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                days.append(int(row["timestep"]))
                cumulativeCases = int(row["cases"])
                newCasesPerDay.append(cumulativeCases - prevCumulativeCases)
                prevCumulativeCases = cumulativeCases
        plt.plot(days, newCasesPerDay)
    plt.xlabel("Simulation day")
    plt.ylabel("New cases per day")
    plt.legend(["Vaccinated", "Unvaccinated"])
    plt.show()

def vaccinate(pop):
    vaccinations = 0
    students = 0
    for i in range(pop.size()):
        person = pop[i]
        age = person.GetAge()
        #vaccinate 18 to 26
        if person.IsInPool(2): #check if person is in pool college
            if age > 18 and age <= 26:  #check if student
                students += 1
                if person.GetHealth().IsSusceptible():
                    vaccinations += 1
                    person.GetHealth().SetImmune()  #vaccinate student                  
    print("Vaccinations: ", vaccinations)
    print("Students:     ", students)
                    
            
def check_days(simulator, event):
    if event.timestep == 8: #1week passed
        print("Vaccinating Students")
        vaccinate(simulator.GetPopulation())
        
        
def runSimulation_vaccination(seed, filename):
    # Configure simulation
    control = PyController(data_dir="data")
    # Load Configuration file:
    control.loadRunConfig(os.path.join("config", xmlFile))
    control.runConfig.setParameter("rng_seed", seed)
    # Register callbacks
    control.registerCallback(check_days, EventType.Stepped)
    #control.registerCallback(trackCases, [EventType.Stepped, filename])
    # Let controller run the simulation
    control.control()
    return (seed,control.runner.getSimulator().GetPopulation().GetInfectedCount())

def runSimulation_no_vaccination(seed, filename):
    # Configure simulation
    control = PyController(data_dir="data")
    # Load Configuration file:
    control.loadRunConfig(os.path.join("config", xmlFile))
    control.runConfig.setParameter("rng_seed", seed)
    # Register callbacks
    #control.registerCallback(trackCases, [EventType.Stepped,filename])
    # Let controller run the simulation
    control.control()
    return (seed,control.runner.getSimulator().GetPopulation().GetInfectedCount())
   
def writeToFile(filename, vaccinated, unvaccinated, outbreaks_vax, outbreaks_no_vax):
    f = open(filename + ".csv", "w+")
    f.write("outbreaks vaccination: " + str(outbreaks_vax) + "\n")
    f.write("outbreaks no vaccination: " + str(outbreaks_no_vax) + "\n")
    f.write("seed, vaccinated, unvaccinated\n")
    for i in range(0,len(vaccinated)):
        f.write(str(vaccinated[i][0]) + "," + str(vaccinated[i][1]) + "," + str(unvaccinated[i][1]) + "\n")
    f.close()    
   
def main(argv):
    if len(argv) != 5:
        print("vaccination_campus.py -f filename -n simulations")
        sys.exit(1)
    i = 1
    while i<len(argv):
        if(argv[i] == "-f"):
            filename = argv[i+1]
            i = i+2
        elif(argv[i] == "-n"):
            Nseeds = int(argv[i+1])
            i = i+2
        else:
            print("Unknown argument: ", argv[i])
            sys.exit(1)
            
    seeds = []
    outbreak_vax = 0
    outbreak_no_vax = 0
    
    for i in range(0,Nseeds):
        seeds.append(random.randint(0,100000000))   
    vaccinated = []
    unvaccinated = []
    
    for i in range(0,len(seeds)):
        print("START VACCINATED SIMULATION: ", i+1, "/", Nseeds)
        vaccinated.append(runSimulation_vaccination(seeds[i], filename))
        
    for i in range(0,len(seeds)):
        print("START UNVACCINATED SIMULATION: ", i+1, "/", Nseeds)
        unvaccinated.append(runSimulation_no_vaccination(seeds[i], filename))   

    print("vax: ", vaccinated)
    print("no vax: ", unvaccinated)
    
    for i in vaccinated:
        if i[1] > 1300:
            outbreak_vax += 1
    for i in unvaccinated:
        if i[1] > 1300:
            outbreak_no_vax += 1
            
    print("with vax: ", outbreak_vax/Nseeds, "/", outbreak_vax)
    print("no vax:   ", outbreak_no_vax/Nseeds, "/", outbreak_no_vax)
    
    writeToFile(filename, vaccinated, unvaccinated, outbreak_vax, outbreak_no_vax)
    
    data_vax = [x[1] for x in vaccinated]
    data_no_vax = [x[1] for x in unvaccinated]
    fig1, ax1 = plt.subplots()
    ax1.set_title("With student vaccinations")
    ax1.boxplot(data_vax)
    fig2, ax2 = plt.subplots()
    ax2.set_title("Without student vaccinations")
    ax2.boxplot(data_no_vax)
    plt.show()
    
    
    
if __name__=="__main__":
    main(sys.argv)
    

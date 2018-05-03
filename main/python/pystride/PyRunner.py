import time

from pystride.stride.stride import Population, Sim

from .Event import EventType, Event, SteppedEvent
from .Stopwatch import Stopwatch
from .Subject import Subject

class PyRunner(Subject):
    """
        Class responsible for the actual simulation loop.
        Functions as the 'model' in the MVC pattern.
    """
    def __init__(self):
        super().__init__()
        self.stopwatch = Stopwatch("run_clock")
        self.population = None
        self.simulator = None
        self.runConfig = None

    def getClock(self):
        return self.stopwatch

    def getConfig(self):
        return self.runConfig

    def setConfig(self, run_config):
        self.runConfig = run_config

    def getPopulation(self):
        return self.population

    def getSimulator(self):
        return self.simulator

    def setup(self, run_config):
        self.notifyObservers(Event(EventType.SetupBegin))
        self.stopwatch.start()
        print("Setup starting at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
        self.runConfig = run_config

        print("Starting population build")
        print("Population build done")
        self.population = Population.Create(self.runConfig.toString())
        print("Starting simulator build")
        self.simulator = Sim.Create(self.runConfig.toString(), self.population)
        print("Simulator build done")

        self.stopwatch.stop()
        self.notifyObservers(Event(EventType.SetupEnd))
        print("Setup finished at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

    def run(self):
        self.stopwatch.start()
        num_days = int(self.runConfig.getParameter("num_days"))
        print("Beginning actual simulation run at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
        self.notifyObservers(Event(EventType.AtStart))
        # Actual simulation loop
        for day in range(num_days):
            self.simulator.TimeStep()
            print("Stepped: timestep " + str(day))
            self.notifyObservers(SteppedEvent(day))
        self.notifyObservers(Event(EventType.AtFinished))
        self.stopwatch.stop()
        print("Simulation run finished at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

import time

from pystride.stride.pystride import Population, RnInfo, RnMan, Sim

from .Event import EventType, Event, SteppedEvent
from .Stopwatch import Stopwatch
from .Subject import Subject

class PyRunner(Subject):
    """
        Class responsible for driving the simulation loop.
        Functions as the 'model' in the MVC pattern.
    """
    def __init__(self):
        super().__init__()
        self.stopwatch = Stopwatch("run_clock")
        self.simulator = None
        self.runConfig = None

    def getClock(self):
        return self.stopwatch

    def getConfig(self):
        return self.runConfig

    def setConfig(self, run_config):
        self.runConfig = run_config

    def getSimulator(self):
        return self.simulator

    def setup(self, run_config):
        self.notifyObservers(Event(EventType.SetupBegin))
        self.stopwatch.start()
        self.runConfig = run_config
        # Create RnMan
        info = RnInfo(self.runConfig.getParameter("rng_seed", default="1,2,3,4"),
                        "", int(self.runConfig.getParameter("num_threads", default=1)))
        rnMan = RnMan(info)

        # Create Population
        population = Population.Create(self.runConfig.toString(), rnMan)
        # Create simulator
        self.simulator = Sim.Create(self.runConfig.toString(), population, rnMan)
        self.stopwatch.stop()
        self.notifyObservers(Event(EventType.SetupEnd))

    def run(self):
        self.stopwatch.start()
        numDays = int(self.runConfig.getParameter("num_days"))
        print("Beginning simulation run.")
        self.notifyObservers(Event(EventType.AtStart))
        # Actual simulation loop
        for day in range(numDays):
            self.simulator.TimeStep()
            self.notifyObservers(SteppedEvent(day))
        self.notifyObservers(Event(EventType.AtFinished))
        self.stopwatch.stop()
        print("Simulation run finished after {} seconds.".format(self.stopwatch.get()))

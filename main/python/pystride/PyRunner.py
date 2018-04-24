import time

from pystride.stride.stride import Simulator

from .Clock import Clock
from .Event import EventType, Event
from .Subject import Subject

class PyRunner(Subject):
    """
        Class responsible for the actual simulation loop.
        Functions as the 'model' in the MVC pattern.
    """
    def __init___(self):
        self.clock = Clock("run_clock")
        self.simulator = Simulator()
        # TODO output_prefix
        self.runConfig = None

    # TODO getClock()?
    # TODO getConfig() ?

    def getSimulator(self):
        return self.simulator

    def setup(self):
        self.notifyObservers(Event(EventType.SetupBegin))
        self.clock.start()
        # TODO status = True
        # TODO write configuration to filepath
        # TODO output prefix?
        print("Setup starting at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
        # TODO print config file path?
        # TODO build simulator
        #   log start builder
        #   SimulatorBuilder builder(config)
        #   self.simulator = builder.build()
        #   log end Building
        self.clock.stop()
        self.notifyObservers(Event(EventType.SetupEnd))
        print("Setup finished at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
        # TODO return status?
        pass

    def run(self):
        self.clock.start()
        num_days = self.runConfig.getParameter("num_days")
        # TODO log start?
        # TODO notify observers that sim is about to start
        #for day in range(num_days):
        #   TODO self.sim->TimeStep()
        #   TODO log time step
        #   TODO notify observers of timestep stepped

        # TODO notify observers that sim is Finished
        self.clock.stop()
        # TODO log finished ?
        pass

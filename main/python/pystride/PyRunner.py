from pystride.stride.stride import Simulator
from .Clock import Clock

class PyRunner:
    """
        Class responsible for the actual simulation loop.
        Functions as the 'model' in the MVC pattern.
    """
    def __init___(self):
        self.clock = Clock("run_clock")
        self.simulator = Simulator()
        # TODO output_prefix
        # TODO configuration

    # TODO getClock()?
    # TODO getConfig() ?

    # TODO getSimulator()

    def setup(self):
        # TODO notify observers of setup begin
        # TODO start clock
        # TODO status = True
        # TODO write configuration to filepath
        # TODO set logger / output prefix?
        # TODO log start setup + config file path
        # TODO build simulator
        #   log start builder
        #   SimulatorBuilder builder(config)
        #   self.simulator = builder.build()
        #   log end Building
        # TODO stop clock
        # TODO notify observers: setup finished
        # TODO log end setup
        # TODO return status?
        pass

    def run(self):
        # TODO clock.Start
        # TODO get num days
        # TODO log start?
        # TODO notify observers that sim is about to start
        #for day in range(num_days):
        #   TODO self.sim->TimeStep()
        #   TODO log time step
        #   TODO notify observers of timestep stepped

        # TODO notify observers that sim is Finished
        # TODO stop clock
        # TODO log finished ?
        pass

# Notes on the Stride project


## 2018/03/08 MVC, python, etc.
The C++ code has been refactored to reflect MVC more clearly. 

* The "M" is the "simulation run" (not the simulator!) with a start, timestapping, state info changing etc. Is is reflected in SimRunner.h/.cpp. It sets up the simulator, effects the computations and notifies all Viewers of events ocurring. The simulator setup deals with logging and building the simulator
* The "V" are (at the moment) the viewers that collect output at relevant time steps and writ it to file (see AdoptedViewer, CasesViewer, etc). In future there also be gui viewers, viewers for different fileformats, possibly for checkpointing etc. On exception the rule "M does not do output" is logging (to capture error message, debug info, detailed computational info that we capture directly for performance reasons e.g. contact logging ...)
*  The "C" is (at the moment) the commnandline controller (CliController.h/.cpp) that gathers all information for the run_config (commanline parameters & a file like run_config_default) and passes it to the SimRunner. The controller als instatiates the required viewers and registers them with the SimRunner. In the future there also may be a gui controlller

How does this fit in with the python environment? Clearly the python env needs to take on the resposibilities of the controller and needs to develop its own viewers. In the future we may hook up python directly with SimRunner, but for now we 'll leave StrideRunner in place and start stripping it of responsibilities that do not fit the the above MVC picture. That refers to (a) eliminating patches to the run_config is the StrideRunner setup, (b) eliminating the GenerateOutputFiles.

Anticipate that evolution when you develop scripts in the python environment and be ready to revisit old scripts if necessary.


## 2018/05/03 Conceptual structure & Python environment

In terms of modelling, the situation is now:
 
* A number of configuration and data files is required to run a simulation. The runConfiguration file is the root file. Aside from key parameters related to the simualtion algorithm, to the output requested etc., it will contain names of other configuration and data files (disease profile data, population data, etc.) that may be used in the simulation.

* Conceptually the steps are:
	* build a population using the runConfiguration; his population implements a demographic/geo-spatial model and neutral w.r.t. the diease that will be studied
	* build a simulation runner using the runConfiguration and population; prior to time stepping with the simulator the siumlation runner will seed population with health/vaccination/immunity profiles specific to the disease studied with the simulator
	* organise the ouput that has been requested via the runConfiguration	* run the simulator (number of steps according to the runConfiguration or step by step if you are using the Gui version of the application).

* To execute a simulation you start up the stride executable with *-e sim* or *--execute sim*. Use the *-h* or *--help* option to list all options and arguments that can be passed to the executable. The most important ones relate to specifying the file with the runConfiguration and commandline overrides of parameters in that file. 

* The execution of a simulation is organised internally as follows:
	* main parses commandline arguments, builds a runConfiguration property tree and instantiates a CliController with it	
	* the CliController goes through the conceptual steps outlined above, using Population (Persons and ContactPools), SimRunner (builds Sim, seeds the population, drives the simulator to take steps while sending notification to viewers at the apropriate moments) and Sim (logic for having contacts and transmissions in the population)
	* the CliController cleans up (nothing much there) and passes back to main for the exit

* Stride functionality is made available in python using SWIG and some high level driver scripts that are counterparts. The acutal bridge is defined through Swig's interface (.i) files. The c++/python contact points (there's an interface file for each - see directory *python/swig*) are: sim.i, population.i,  health.i and contactpooltype.i.). There is and additional interface file stride.i that deals with c++ exceptions. The high level python scripts are (see directory *python/pystride*) PyController and PyRunner. In addition there's some infrastructure for the Subject/Observer construct and for timing.
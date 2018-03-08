# Notes on the Stride project


## 2018/03/08 MVC, python, etc.
The C++ code has been refactored to reflect MVC more clearly. 

* The "M" is the "simulation run" (not the simulator!) with a start, timestapping, state info changing etc. Is is reflected in SimRunner.h/.cpp. It sets up the simulator, effects the computations and notifies all Viewers of events ocurring. The simulator setup deals with logging and building the simulator
* The "V" are (at the moment) the viewers that collect output at relevant time steps and writ it to file (see AdoptedViewer, CasesViewer, etc). In future there also be gui viewers, viewers for different fileformats, possibly for checkpointing etc. On exception the rule "M does not do output" is logging (to capture error message, debug info, detailed computational info that we capture directly for performance reasons e.g. contact logging ...)
*  The "C" is (at the moment) the commnandline controller (CliController.h/.cpp) that gathers all information for the run_config (commanline parameters & a file like run_config_default) and passes it to the SimRunner. The controller als instatiates the required viewers and registers them with the SimRunner. In the future there also may be a gui controlller

How does this fit in with the python environment? Clearly the python env needs to take on the resposibilities of the controller and needs to develop its own viewers. In the future we may hook up python directly with SimRunner, but for now we 'll leave StrideRunner in place and start stripping it of responsibilities that do not fit the the above MVC picture. That refers to (a) eliminating patches to the run_config is the StrideRunner setup, (b) eliminating the GenerateOutputFiles.

Anticipate that evolution when you develop scripts in the python environment and be ready to revisit old scripts if necessary.
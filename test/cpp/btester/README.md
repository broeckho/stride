# Notes on benchmarking


## 2018/04/24
When seting benchmarks the following parameters in the run configuration matter:

* The m\_contact\_log\_level: can be None, Transmissions, Susceptibles, All and have increasing amount of logging during the simulation.
* The size of the population obviously matters; for now we can switch between pop\_flanders600.csv and pop\_flanders_1600.csv for the population\_file. Not nearly good enough, but that 'll improve with the synthetic populations.
* The number of days is not such an interesting parameter: you need to trace the epidemic and once it's over you are done. Essentially not much to vary here.

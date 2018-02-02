#############################################################################
#            Running simulations for 'measles in Flanders' study            #
#############################################################################

from baseline import runBaseline

def main():
    # Run baseline simulations
    print("Running baseline simulations")
    runBaseline()
    # Run simulations with imitation-based vaccination behaviours
    print("Running simulations with imitation-behaviour")
    # Post-processing
    print("Post-processing...")

if __name__=="__main__":
    main()

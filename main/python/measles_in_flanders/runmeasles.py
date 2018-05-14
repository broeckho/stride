#############################################################################
#            Running simulations for 'measles in Flanders' study            #
#############################################################################

from pystride.PyController import PyController

def runRandom():
    control = PyController("../config/run_default.xml")
    # control.runConfig.setParameter("contact_log_level", "Susceptibles")
    # control.runConfig.setParameter("immunity_profile", "Random" + 35)
    # control.runConfig.setParameter("immunity_rate", ?)
    # control.runConfig.setParameter("num_days", 100) ?
    # control.runConfig.setParameter("num_participants_survey", 10 000)
    control.runConfig.setParameter("output_prefix", "Random")
    # vaccine_link_probability
    # vaccine_profile
    # vaccine_rate
    control.control()

def runAgeProfiles():
    control = PyController("../config/run_default.xml")
    # control.runConfig.setParameter("contact_log_level", "Susceptibles")
    # control.runConfig.setParameter("immunity_profile", "Random" + 35)
    # control.runConfig.setParameter("immunity_rate", ?)
    # control.runConfig.setParameter("num_days", 100) ?
    # control.runConfig.setParameter("num_participants_survey", 10 000)
    control.runConfig.setParameter("output_prefix", "AgeProfiles")
    # vaccine_link_probability
    # vaccine_profile
    # vaccine_rate
    control.control()

def runClustering():
    control = PyController("../config/run_default.xml")
    # control.runConfig.setParameter("contact_log_level", "Susceptibles")
    # control.runConfig.setParameter("immunity_profile", "Random" + 35)
    # control.runConfig.setParameter("immunity_rate", ?)
    # control.runConfig.setParameter("num_days", 100) ?
    # control.runConfig.setParameter("num_participants_survey", 10 000)
    control.runConfig.setParameter("output_prefix", "Clustering")
    # vaccine_link_probability
    # vaccine_profile
    # vaccine_rate
    control.control()

def main():
    print("Running simulations for Measles In Flanders study")
    runRandom()
    runAgeProfiles()
    runClustering()

if __name__=="__main__":
    main()

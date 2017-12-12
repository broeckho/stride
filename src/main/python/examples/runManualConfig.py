import pystride
from pystride.Simulation import Simulation

simulation = Simulation()

# Load disease from configuration file
simulation.loadDiseaseConfig("data/disease_influenza.xml")
simulation.diseaseConfig.setParameter("transmission/b0", 0.35)

# Configure simulation
simulation.runConfig.setParameter("rng_seed", 1)
simulation.runConfig.setParameter("r0", 5)
simulation.runConfig.setParameter("seeding_rate", 0.002)
simulation.runConfig.setParameter("seeding_age_min", 1)
simulation.runConfig.setParameter("seeding_age_max", 99)
simulation.runConfig.setParameter("immunity_rate", 0.8)
simulation.runConfig.setParameter("immunity_profile", "None")
simulation.runConfig.setParameter("vaccine_profile", "Random")
simulation.runConfig.setParameter("vaccine_rate", 0.8)
simulation.runConfig.setParameter("vaccine_link_probability", 0)
simulation.runConfig.setParameter("population_file", "data/pop_antwerp.csv")
simulation.runConfig.setParameter("num_days", 10)
simulation.runConfig.setParameter("output_prefix", "simulation1")
simulation.runConfig.setParameter("generate_person_file", 1)
simulation.runConfig.setParameter("num_participants_survey", 10)
simulation.runConfig.setParameter("start_date", "2017-01-01")
simulation.runConfig.setParameter("holidays_file", "data/holidays_flanders_2017.json")
simulation.runConfig.setParameter("age_contact_matrix_file", "data/contact_matrix_flanders_subpop.xml")
simulation.runConfig.setParameter("log_level", "Transmissions")

# Run simulation
simulation.run()

#!/bin/sh
mkdir -p gprof
# number of days
for i in 10 50 150 500 1000
do
	./bin/stride -o num_days=$i
	gprof ./bin/stride gmon.out > gprof/days_analysis_$i.txt
done
# population size
for i in 500 1000 10000 50000 100000
do
	./bin/stride -c file=run_generate_default_p$i.xml
	gprof ./bin/stride gmon.out > gprof/population_size_analysis_$i.txt
done
# imunity rate
for i in 0.2 0.4 0.6 0.8 0.99
do
	./bin/stride -o immunity_rate=$i
	gprof ./bin/stride gmon.out > gprof/imunity_rate_analysis_$i.txt
done
# seeding rate
for i in 0.000001 0.00001 0.0001 0.001 0.01
do
	./bin/stride -o seeding_rate=$i
	gprof ./bin/stride gmon.out > gprof/seeding_rate_analysis_$i.txt
done
# contact log mode
for i in All Susceptibles None Transmissions
do
	./bin/stride -o contact_log_level=$i
	gprof ./bin/stride gmon.out > gprof/contact_log_mode_$i.txt
done

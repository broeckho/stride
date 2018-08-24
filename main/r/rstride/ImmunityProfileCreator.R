#############################################################################
#  This file is part of the Stride software. 
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or any 
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2018, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
# BASED ON DATA FROM:
#
# Hens N, Abrams S, Santermans E, Theeten H, Goeyvaerts N, Lernout T, Leuridan E, 
# Van Kerckhove K, Goossens H, Van Damme P, Beutels P. Assessing the risk of 
# measles resurgence in a highly vaccinated population: Belgium anno 2013. Euro 
# Surveill. 2015;20(1):pii=20998.  
#
############################################

library(XML)

############################################
## LOAD AND RESHAPE DATA                  ##
############################################

# load data
all_data <- read.table('./data/susceptibility_measles_belgium_2013.csv',sep=',')  
num_age_data <- dim(all_data)[1]

# calculate the average susceptibility per age
susceptiblilty_profile <- rowSums(all_data) / dim(all_data)[2]

# newborns have 3 months maternally immunity => 1/4 protected, 3/4 not protected 
susceptiblilty_profile[1] <- 3/4

# extend to 99 years of age by repeating the last one (from 0 to 99 year = 100 categories)
num_age <- 100
susceptiblilty_profile <- c(susceptiblilty_profile,rep(susceptiblilty_profile[num_age_data],num_age-num_age_data))

# get immunity =  1 - suscetibility
immunity_profile <- 1-susceptiblilty_profile

# explore
plot(susceptiblilty_profile,ylim=0:1,type='l',lwd=7,ylab='susceptibility',xlab='age')
plot(immunity_profile,ylim=0:1,type='l',lwd=7,ylab='immunity',xlab='age')

############################################
## SAVE AS XML  	 	                      ##
############################################

# add age group as column names
names(immunity_profile) <- paste0('age',0:99)

# add info on data source and manipulation
immunity_data <- unlist(list(data_source = 'susceptibility_measles_belgium_2013.csv',
                             data_manipulation = "average by age",
                             round(immunity_profile,digits=4)))

# save as xml
.rstride$save_config_xml(immunity_data,'immunity','immunity_measles_belgium')




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
#  Copyright 2017, Willem L, Kuylen E & Broeckhove J
#############################################################################
# susceptiblilty AND SUSCEPTIBILITY
#
# BASED ON DATA FROM:
#
# Hens N, Abrams S, Santermans E, Theeten H, Goeyvaerts N, Lernout T, Leuridan E, 
# Van Kerckhove K, Goossens H, Van Damme P, Beutels P. Assessing the risk of 
# measles resurgence in a highly vaccinated population: Belgium anno 2013. Euro 
# Surveill. 2015;20(1):pii=20998.  
#
# Author: Lander Willem
# Last update: 05/10/2017  
#########################
rm(list=ls(all=TRUE))
gc()

tag <- '20171005'

############################################

#all_data <- read.table('../resources/data/susceptibility_measles_belgium_2013.csv',sep=',')  
all_data <- read.table('../data/susceptibility_measles_belgium_2013.csv',sep=',')  
dim(all_data)	   
num_age_data <- dim(all_data)[1]

susceptiblilty_profile <- rowSums(all_data) / dim(all_data)[2]
plot(susceptiblilty_profile,ylim=c(0,1),type='l')

# newborns have 3 months maternally immunity => 1/4 protected, 3/4 not protected 
susceptiblilty_profile[1] <- 3/4

# extend to 99 years of age (from 0 to 99 year, are 100 categories)
num_age <- 100
susceptiblilty_profile <- c(susceptiblilty_profile,rep(susceptiblilty_profile[num_age_data],num_age-num_age_data))
length(susceptiblilty_profile)	  

# uniform 
susceptiblilty_profile_uniform <- susceptiblilty_profile
susceptiblilty_profile_uniform[] <- susceptiblilty_profile[80]
susceptiblilty_profile_uniform[1] <- susceptiblilty_profile[1]

# scare
susceptiblilty_profile_scare <- susceptiblilty_profile
susceptiblilty_profile_scare[2:7] <- 0.92-(1:6)*0.02
susceptiblilty_profile_scare[8:9] <- 0.8
susceptiblilty_profile_scare[10:15] <- 0.8+(1:6)*0.02
susceptiblilty_profile_scare[16:(num_age)] <- susceptiblilty_profile[2:(num_age-14)]

# regular
susceptiblilty_profile_regular <- susceptiblilty_profile
susceptiblilty_profile_regular[3:(num_age)] <- susceptiblilty_profile[2:(num_age-1)]

# catchup program for 18-38 year old people
susceptiblilty_profile_catchup <- susceptiblilty_profile
catchup_ages <- 20:30
catchup_fraq <- (susceptiblilty_profile_catchup[catchup_ages])*0.8
susceptiblilty_profile_catchup[catchup_ages] <- susceptiblilty_profile_catchup[catchup_ages] - catchup_fraq

#susceptiblilty_profile_flu <- susceptiblilty_profile
# susceptiblilty_profile_flu[] <- 0.6

#susceptiblilty_profile_catchup[susceptiblilty_profile_catchup>0.99] <- 0.99


plot(susceptiblilty_profile,ylim=0:1,type='l',lwd=7,ylab='susceptibility',xlab='age')
lines(susceptiblilty_profile_uniform,col=2,lwd=4)
lines(susceptiblilty_profile_catchup,col=3,lwd=2)
legend('topright',c('current','uniform','catchup'),lwd=c(7,4,2),col=1:3)

immunity_profile <- 1-susceptiblilty_profile
immunity_profile_uniform <- 1-susceptiblilty_profile_uniform
immunity_profile_catchup <- 1-susceptiblilty_profile_catchup

plot(immunity_profile,ylim=0:1,type='l',lwd=7,ylab='immunity',xlab='age')
lines(immunity_profile_uniform,col=2,lwd=4)
lines(immunity_profile_catchup,col=3,lwd=2)
legend('bottomright',c('current','uniform','catchup'),lwd=c(7,4,2),col=1:3)



# pdf('susceptibility_estimate.pdf')
# plot(1-susceptiblilty_profile,ylim=0:1,type='l',ylab='susceptibility',xlab='age',lwd=4)
# dev.off()

############################################
## SAVE AS XML  	 	                      ##
############################################

library(XML)

top = newXMLNode("immunity_profile")
gen <- newXMLNode("current", parent = top)
for(i in 1:num_age){
  newXMLNode("proportion", immunity_profile[i], parent = gen)
}

gen <- newXMLNode("uniform", parent = top)
for(i in 1:num_age){
  newXMLNode("proportion", immunity_profile_uniform[i], parent = gen)
}

# gen <- newXMLNode("scare", parent = top)
# for(i in 1:num_age){
#   newXMLNode("proportion", susceptiblilty_profile_scare[i], parent = gen)
# }

# gen <- newXMLNode("regular", parent = top)
# for(i in 1:num_age){
#   newXMLNode("proportion", susceptiblilty_profile_regular[i], parent = gen)
# }

gen <- newXMLNode("catchup", parent = top)
for(i in 1:num_age){
  newXMLNode("proportion", immunity_profile_catchup[i], parent = gen)
}

gen <- newXMLNode("cocoon", parent = top)
for(i in 1:num_age){
  newXMLNode("proportion", immunity_profile[i], parent = gen)
}

# gen <- newXMLNode("flu", parent = top)
# for(i in 1:num_age){
#   newXMLNode("proportion", susceptiblilty_profile_flu[i], parent = gen)
# }


#saveXML(top, file="../resources/data/immunity_measles.xml",NewLineOnAttributes=T)
saveXML(top, file="../data/immunity_measles.xml",NewLineOnAttributes=T)


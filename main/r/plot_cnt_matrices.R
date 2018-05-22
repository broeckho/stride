## SOCIAL MIXR ~ INLA SMOOTHING
##
## Based on code from ERC HACKATHON 2017
#######################################
#setwd('..')
## Load packages
#install.packages("devtools")
library('devtools')

#install_github("sbfnk/socialmixr")
library('socialmixr')

#install.packages("INLA", repos=c(getOption("repos"), INLA="https://inla.r-inla-download.org/R/stable"), dep=TRUE)
source("lib/INLA/estimate_contact_intensities.R")
source("lib/INLA/plot_matrix.R")

library('ggplot2')

## EXAMPLE WITH POLYMOD DATA
# Estimate smoothed contact intensities
result_inla <- estimate_contact_intensities(survey = polymod, countries = "Belgium", max_age=81, show_summary = FALSE)

# Plot results 
pdf('polymod_belgium_inla.pdf'); 
plot_matrix(result_inla, what = 'matrix', contour = FALSE)
plot_matrix(result_inla, what = 'matrix_smooth_individual') # (with contours)
plot_matrix(result_inla, what = 'matrix_smooth_population')
dev.off()


(rowSums(result_inla$matrix)) / result_inla$demography$participants
sum(result_inla$demography$participants)
quantile(result_inla$matrix_smooth_population,0.1)
quantile(result_inla$matrix_smooth_individual,0.1)
quantile(result_inla$matrix,0.1)
sum(result_inla$demography$proportion)

plot(rowSums(result_inla$matrix))
plot(colSums(result_inla$matrix,na.rm=T))

part_age <- table(polymod$participants$part_age[polymod$participants$country == countries])
part_age <- hist(polymod$participants$part_age[polymod$participants$country == countries],-1:90 ,plot = F)$counts

contactdata <- contact_matrix(survey = polymod, countries = "Belgium", age.limits = 0:81, counts = T)
mean(rowSums(contactdata$matrix)/part_age[1:82],na.rm=T)

contactdata <- contact_matrix(survey = polymod, countries = "Belgium", age.limits = 0:81, counts = F)
mean(rowSums(contactdata$matrix),na.rm=T)
plot(rowSums(contactdata$matrix))


##############################################################
## STRIDE CONTACT DETALS
#sim_dir <- '../sim_output/20180424_151421/instance0_R2'
sim_dir <- './sim_output/20180503_203527_cnt/exp_weekday'

cnt_data  <- read.table(file.path(sim_dir,'contacts.csv'),sep=',',header=T)
part_data <- read.table(file.path(sim_dir,'participants.csv'),sep=',',header=T)
dim(part_data)

names(cnt_data)[1]   <- 'part_id'
names(part_data)[1]  <- 'part_id'

part_data <- part_data[,1:2]
part_data$country <- 'Belgium'
cnt_data$country <- 'Belgium'
part_data$year <- 2010
cnt_data$year <- 2010


# TEST
stride_data <- survey(participants=part_data,contacts=cnt_data)
cnt_matrix  <- contact_matrix(survey = stride_data, age.limits = 0:81, counts = F)
plot(0:81,rowSums(cnt_matrix$matrix))
lines(0:81,rowSums(cnt_matrix$matrix))

cnt_matrix_bootstrap  <- contact_matrix(survey = stride_data , age.limits = 0:81, counts = F,bootstrap = T, n = 20)
plot(0:81,rowSums(cnt_matrix_bootstrap$matrices[[1]]$matrix),col=0)
for(i in 1:length(cnt_matrix_bootstrap$matrices))
lines(0:81,rowSums(cnt_matrix_bootstrap$matrices[[i]]$matrix))

cnt_matrix_ref  <- contact_matrix(survey = polymod, countries = "BE", age.limits = 0:81, counts = F,bootstrap = T, n = 20)
for(i in 1:length(cnt_matrix_ref$matrices))
  lines(0:81,rowSums(cnt_matrix_ref$matrices[[i]]$matrix),col=2)

result_x <- result_work; tag_x <- 'WORK'
plot_results <- function(result_x, tag_x){
  plot_name <- file.path(sim_dir,paste0('cnt_',tag_x,'.pdf'))
  print(plot_name)
  pdf(plot_name,7,6)
  print(plot_matrix(result_x, what = 'matrix',title=paste(tag_x,'Contacts'),contour = F))
  plot(0:80,rowSums(result_x$matrix))
  lines(0:80,rowSums(result_x$matrix),lwd=2)
  print(plot_matrix(result_x, what = 'matrix_smooth_individual',title=paste(tag_x,'Contacts')))
  print(plot_matrix(result_x, what = 'matrix_smooth_population',title=paste(tag_x,'Contacts')))
  dev.off()
}


# Estimate smoothed contact intensities
# TOTAL
stride_data <- survey(participants=part_data,contacts=cnt_data)
result_all  <- estimate_contact_intensities(survey = stride_data, countries = "Belgium", max_age=80, show_summary = FALSE)
plot_results(result_all,'TOTAL')

# HOUSEHOLD
stride_data_subset <- survey(participants=part_data,contacts=cnt_data[cnt_data$cnt_home==1,])
result_home <- estimate_contact_intensities(survey = stride_data_subset, countries = 'BE', max_age=80, show_summary = FALSE)
plot_results(result_home,'HOUSEHOLD')

# SCHOOL
stride_data_subset <- survey(participants=part_data,contacts=cnt_data[cnt_data$cnt_school==1,])
result_school      <- estimate_contact_intensities(survey = stride_data_subset, countries = "Belgium", max_age=80, show_summary = FALSE)
plot_results(result_school,'SCHOOL')

# WORK
stride_data_subset <- survey(participants=part_data,contacts=cnt_data[cnt_data$cnt_work==1,])
result_work <- estimate_contact_intensities(survey = stride_data_subset, countries = "Belgium", max_age=80, show_summary = FALSE)
plot_results(result_work,'WORK')

# COMMUNITY: PRIMARY
stride_data_subset <- survey(participants=part_data,contacts=cnt_data[cnt_data$cnt_prim_comm==1,])
result_comm_prim <- estimate_contact_intensities(survey = stride_data_subset, countries = "Belgium", max_age=80, show_summary = FALSE)
plot_results(result_comm_prim,'COMMUNITY_PRIM')

# COMMUNITY: SECONDARY
stride_data_subset <- survey(participants=part_data,contacts=cnt_data[cnt_data$cnt_sec_comm==1,])
result_comm_sec <- estimate_contact_intensities(survey = stride_data_subset, countries = "Belgium", max_age=80, show_summary = FALSE)
plot_results(result_comm_sec,'COMMUNITY_SEC')




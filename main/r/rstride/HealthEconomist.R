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
#
#  Copyright 2019, Willem L, Kuylen E & Broeckhove J
#############################################################################

#############################################################################
# ESTIMATE COST-EFFECTIVENESS  ##
#############################################################################

calculate_cost_effectiveness <- function(project_dir){
  
  # load project output summary
  project_summary    <- .rstride$load_project_summary(project_dir)
  
  # get cost-benefit parameters
  cea_param <- get_cea_param()
  
  # retrieve all variable model parameters
  input_opt_design   <- .rstride$get_variable_model_param(project_summary)
  #col_intervention   <- colnames(input_opt_design)
  
  # adaptation when simulating multiple years: unify target group
  col_intervention   <- 'vaccine_rate' # focus on vaccine coverage
  row_base_year      <- project_summary$vaccine_min_age == min(project_summary$vaccine_min_age)
  project_summary$vaccine_min_age <- min(project_summary$vaccine_min_age)
  project_summary$vaccine_max_age <- min(project_summary$vaccine_max_age)
  
  # age-specific data => use "transmission data"
  data_transmission <- load(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_data_transmission.RData')))
  data_transmission <- get(data_transmission)
  
  # get infected seeds per run
  infected_seeds <- aggregate(is.na(infector_age) ~ exp_id,data=data_transmission,sum)
  names(infected_seeds)[2] <- 'num_index_cases'
  
  # select secondary cases
  #data_transmission <- data_transmission[!is.na(data_transmission$infector_id),]
  
  # burden of disease: age specific
  # data_transmission$age_cat   <- cut(data_transmission$part_age,cea_param$age_cat,right=F)
  # tbl_cases_total_age         <- as.data.frame(table(data_transmission$exp_id,data_transmission$age_cat))
  # cases_total_age             <- data.frame(matrix(tbl_cases_total_age$Freq,nrow=max(data_transmission$exp_id)))
  # names(cases_total_age)      <- paste0('cases_age_',levels(data_transmission$age_cat))
  # cases_total_age$cases_total <- rowSums(cases_total_age)
  # 
  data_cases <- as.data.frame(table(data_transmission$exp_id))
  names(data_cases) <- c('exp_id','num_cases')
  
  # burden of disease: per index case
  # index_cases_matrix <- matrix(rep(infected_seeds$num_index_cases,each=6),ncol=6,byrow = T)
  # cases_total_age <- cases_total_age / index_cases_matrix
  
  # add exp_id
  # cases_total_age$exp_id      <- 1:nrow(cases_total_age)
  # 
  # # reformat column names
  # names(cases_total_age) <- sub('\\[','',names(cases_total_age))
  # names(cases_total_age) <- sub('\\)','',names(cases_total_age))
  # names(cases_total_age) <- sub(',','_',names(cases_total_age))
  # 
  # casefinding => use "vaccination data"
  datafile_intervention <- file.path(project_dir,paste0(project_summary$run_tag[1],'_data_vaccination.RData'))
  if(file.exists(datafile_intervention)){
   data_intervention <- load(datafile_intervention)
   data_intervention <- get(data_intervention)
  } else{
    data_intervention <- data.frame(exp_id=0)
  }
  
  data_intervention_table <- data.frame(table(data_intervention$exp_id))
  names(data_intervention_table) <- c('exp_id','num_vaccines')

  # CREATE SUMMARY STATISTICS PER RUN
  outbreak_summary <- data.frame(exp_id            = project_summary$exp_id,
                                 population_size   = project_summary$population_size,
                                 seeding_rate      = project_summary$seeding_rate,
                                 vaccine_rate      = project_summary$vaccine_rate,
                                 vaccine_min_age   = project_summary$vaccine_min_age,
                                 vaccine_max_age   = project_summary$vaccine_max_age,
                                 intervention      = as.numeric(as.factor(project_summary[,col_intervention])))
  
  # add intervention data
  outbreak_summary <- merge(outbreak_summary,data_intervention_table,all=T)
  outbreak_summary$num_vaccines[is.na(outbreak_summary$num_vaccines)] <- 0
  
  # add number of infected seeds
  outbreak_summary <- merge(outbreak_summary,infected_seeds)
  
  # add burden of disease data
  outbreak_summary <- merge(outbreak_summary,data_cases)

  # intervention tag
  outbreak_summary$intervention_id  <- outbreak_summary$intervention
  outbreak_summary$intervention_tag <- paste0('coverage ',outbreak_summary$vaccine_rate,
                                              ' [',outbreak_summary$vaccine_min_age,
                                              '-',outbreak_summary$vaccine_max_age,']')
 
  # adjust number of vaccines => account only 'base_year'
  for(i_intervention in 1:max(outbreak_summary$intervention_id)){
    flag_intervention <- i_intervention == outbreak_summary$intervention_id
    flag_base_year    <- row_base_year & flag_intervention
    # note: use 'mean' to capture small stochastic differences  
    outbreak_summary$num_vaccines[flag_intervention] <- mean(outbreak_summary$num_vaccines[flag_base_year])
  }
  
  # get aggregated statistics
  outbreak_std_err <- aggregate(. ~ intervention_tag, data=outbreak_summary,std_err)
  outbreak_mean    <- aggregate(. ~ intervention_tag, data=outbreak_summary,mean)
  
  # set comparator
  flag_mean_current <- outbreak_mean$num_vaccines == 0
  outbreak_mean$intervention_tag[flag_mean_current] <- 'current'
  
  # AVERTED BURDEN
  outbreak_mean$cases_averted <- outbreak_mean$num_cases[flag_mean_current] - outbreak_mean$num_cases
  
  # program costs
  num_vaccines    <- outbreak_mean$num_vaccines
  cost_program    <- num_vaccines * (cea_param$price_dose+cea_param$price_admin_dose)
  
  # total cost averted
  outbreak_mean$medical_costs_averted <- outbreak_mean$cases_averted * cea_param$saving_per_averted_case
  outbreak_mean$incr_cost             <- cost_program - outbreak_mean$medical_costs_averted
  
  # total burden averted
  outbreak_mean$qaly_gain <- outbreak_mean$cases_averted * cea_param$QALYgain_per_averted_case
  
  # ICER
  outbreak_mean$icer <-  NA
  outbreak_mean$icer[!flag_mean_current] <- outbreak_mean$incr_cost[!flag_mean_current] /  outbreak_mean$qaly_gain[!flag_mean_current]
  
  pdf(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_ce_plane_icer.pdf')),10,5)
  par(mfrow=1:2)
  
  # CEA PLANE
  plot(outbreak_mean$qaly_gain,
       outbreak_mean$incr_cost,
       col=outbreak_mean$intervention,
       xlab='QALY gain',
       ylab='Incremental cost',
       pch=16,cex=2)
  abline(h=0,lty=2)
  abline(v=0,lty=2)
  legend('topleft',paste(outbreak_mean$intervention_tag),fill=outbreak_mean$intervention,cex=0.8)
  
  ## ICER
  plot(outbreak_mean$num_vaccines,
       outbreak_mean$icer,
       col=outbreak_mean$intervention,
       xlab='number of vaccine doses',
       ylab='icer',
       pch=16,cex=2)
  legend('topleft',outbreak_mean$intervention_tag[!flag_mean_current],fill=outbreak_mean$intervention_id[!flag_mean_current],cex=0.8)
  
  ## DISTRIBUTIONS
  par(mfrow=c(1,1))
  num_samples <- 10000
  hist_breaks <- seq(0,max(outbreak_mean$num_cases)*2,length=100)
  hist_xlim   <- range(c(0,outbreak_mean$num_cases*2))
  hist(rnorm(num_samples,outbreak_mean$num_cases[flag_mean_current],outbreak_std_err$num_cases[flag_mean_current]),
       hist_breaks,freq = F,xlim=hist_xlim,col=1,xlab='Cases (all ages)',main='')
  for(i_intervention in outbreak_mean$intervention_id[!flag_mean_current]){
    hist(rnorm(num_samples,outbreak_mean$num_cases[i_intervention],outbreak_std_err$num_cases[i_intervention]),
      hist_breaks,freq = F,add=T,col=alpha(i_intervention,0.4))
  }
  legend('topright',outbreak_mean$intervention_tag,fill=outbreak_mean$intervention_id,cex=0.8)
  
  dev.off()

}


get_cea_param <- function()
{
  
  # REFERENCE: Zwanziger, Szilagi & PadmaKaul, Health Service Research, 2001 Oct; 36(5): 885–909.
  # https://www.ncbi.nlm.nih.gov/pmc/articles/PMC1089266/pdf/hsresearch00006-0069.pdf
  data_cea <- list(saving_per_averted_case   = 2089, #dollar2001
                   QALYgain_per_averted_case = 0.086,
                   price_dose         = 6.3,                            # Beutels & Gay 2003 
                   price_admin_dose   = 7.3)                            # Beutels & Gay 2003 
  
  # # source: Carabin et al. (2003) The cost of measles in industrialised countries
  # data_cea <- list( reporting_prob     = 0.75,      # Carabin et al 2003 (Upper Income Country)
  #                   severe_case_prob   = 0.8,       # Carabin et al 2003 (Upper Income Country)
  #                   hospital_stay_days = 4,         # Carabin et al 2003 (Upper Income Country)
  #                   QALY_lost          = 6.8/365,   # Thorrington et al 2014 (UK)
  #                         
  #                   age_cat            = c(0  , 5,   10, 15, 20, 100),   # Beutels & Gay 2003 
  #                   cfr_per100000      = c(18 , 9.6, 32, 87.2, 92.6),    # Beutels & Gay 2003 
  #                   hosp_rate          = c(5.8, 1.7, 1.7, 1.6, 5.3)/100, # Beutels & Gay 2003 
  #                   direct_cost_case   = c(123, 112, 110, 111,148),      # Beutels & Gay 2003 
  #                   life_expectancy    = c(73, 68, 63, 58, 27),          # Beutels & Gay 2003 
  #                   price_dose         = 6.3,                            # Beutels & Gay 2003 
  #                   price_admin_dose   = 7.3)                            # Beutels & Gay 2003 
  # 
  # 
  # burden <- list(age_cat  = c(0  , 5,   10, 20, 30, 100),    #Perry & Halsey 2004
  #                total    = c(28730,6492,18580,9161,4068),   #Perry & Halsey 2004
  #                any      = c(11883,1173,2369,2656,1399),    #Perry & Halsey 2004
  #                hospital = c(7470,612,1612,2075,1107),      #Perry & Halsey 2004
  #                death    = c(97,9,18,26,27))
  #
  # # convert case fatality rate
  # data_cea$cfr_per_case <- data_cea$cfr_per100000/1e5
  # 
  # # unit cost: 2001USD
  # data_cost2001 <- data.frame(cost_hospital_day = 253,      # Carabin et al 2003 (BE)
  #                             cost_gp_visit     = 16,       # Carabin et al 2003 (BE)
  #                             cost_mmr_delivery = 4.3,      # Carabin et al 2003 (BE)
  #                             cost_mmr_dose     = 5)        # Carabin et al 2003 (BE)
  # 
  # cpi_2001USD <- 81.20256846   #2001USDA
  # cpi_2017USD <- 112.4115573   #2017USD
  # conversion_2001USDA_2017EURO <- 0.80          #2017EURO == 2017USD
  # data_cost <- data_cost2001 / cpi_2001USD * cpi_2017USD * conversion_2001USDA_2017EURO
     
  return(data_cea)
}

get_averted_burden <- function(project_dir){
  
  # load project output summary
  project_summary    <- .rstride$load_project_summary(project_dir)
  
  # # get cost-benefit parameters
  # cea_param <- get_cea_param()
  # 
  # retrieve all variable model parameters
  input_opt_design   <- .rstride$get_variable_model_param(project_summary)
  col_intervention   <- colnames(input_opt_design)
  project_summary[,col_intervention]
  
  project_summary$intervention_factor <- as.factor(project_summary[,col_intervention])
  project_summary$intervention_id     <- as.numeric(project_summary$intervention_factor)
  intervention_legend <- paste0(col_intervention,'_',levels(project_summary$intervention_factor))
  
  # age-specific data => use "transmission data"
  data_transmission <- load(file=file.path(project_dir,paste0(project_summary$run_tag[1],'_data_transmission.RData')))
  data_transmission <- get(data_transmission)
  
  # select secundary cases
  #data_transmission <- data_transmission[!is.na(data_transmission$infector_id),]
  
  # casefinding => use "vaccination data"
  datafile_intervention <- file.path(project_dir,paste0(project_summary$run_tag[1],'_data_vaccination.RData'))
  if(file.exists(datafile_intervention)){
    data_intervention <- load(datafile_intervention)
    data_intervention <- get(data_intervention)
  } else{
    data_intervention <- data.frame(exp_id=0)
  }
  
  tmp_tbl <- data.frame(table(data_intervention$exp_id))
  names(tmp_tbl) <- c('exp_id','num_vaccines')
  
  # count number of cases per index case
  outbreak_total_cases <- data.frame(table(data_transmission$id_index_case))
  names(outbreak_total_cases) <- c('id_index_case','total_cases')
  
  # get outbreak detals: experiment id and intervention
  outbreak_exp_id <- data.frame(unique(cbind(data_transmission$id_index_case,data_transmission$exp_id)))
  names(outbreak_exp_id) <- c('id_index_case','exp_id')
  outbreak_exp_id <- merge(outbreak_exp_id,project_summary[,c('exp_id','intervention_id')])
  
  # get vaccines per outbreak
  data_intervention
  # outbreak_total_vaccines <- data.frame(table(data_intervention$id_index_case))
  # names(outbreak_total_vaccines) <- c('id_index_case','total_vaccines')
  outbreak_total_vaccines <- data.frame(table(data_intervention$exp_id))
  names(outbreak_total_vaccines) <- c('exp_id','total_vaccines')
  
  # aggregate outbreak data
  outbreak_summary <- merge(outbreak_total_cases,outbreak_exp_id)
  outbreak_summary <- merge(outbreak_summary,outbreak_total_vaccines, all.x=T)
  outbreak_summary$total_vaccines[is.na(outbreak_summary$total_vaccines)] <- 0
  
  head(outbreak_summary)
  
  boxplot(total_cases~intervention_id,data=outbreak_summary)

  outbreak_summary_exp        <- aggregate(. ~ exp_id + intervention_id, data=outbreak_summary,sum) 
  outbreak_summary_index_case <- aggregate(. ~ id_index_case + intervention_id, data=outbreak_summary,sum) 
  
  boxplot(total_cases/52~intervention_id,data=outbreak_summary_exp)
  boxplot(total_cases~intervention_id,data=outbreak_summary_index_case)
  
  boxplot(total_vaccines/52~intervention_id,data=outbreak_summary_exp)
}

# help function
std_err <- function(x) sqrt(var(x)/length(x))



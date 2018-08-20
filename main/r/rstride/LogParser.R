#!/usr/bin/env Rscript
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
# Helpfunction(s) to parse the log file(s)
#
#############################################################################

##################################
## PARSE LOGFILE               ##
##################################

"DEVELOPMENT CODE"
if(0==1){
  #f_exp_dir <- file.path(output_dir,output_exp_dirs[i_exp])
  f_exp_dir <- './sim_output/20180730_113633/exp0001'
  contact_log_filename <- file.path(project_summary$output_prefix[i_exp],'contact_log.txt')
  
  
}

parse_contact_logfile <- function(contact_log_filename)
{

  # terminal message
  cat("PARSING LOGFILE:",contact_log_filename,fill=TRUE)
  
  # count the maximum number of columns in the logfile
  data_ncol <- max(count.fields(contact_log_filename, sep = " "))
  
  # load log file using a specified number of columns and fill empty columns
  # By default, the first record determines the number of columns, so info might get lost
  data_log  <- read.table(contact_log_filename, sep=' ',fill=T,col.names = paste0("V", seq_len(data_ncol)),stringsAsFactors = F)
 
  # Parse log file using the following tags tags: 
  # - PART    participant info
  # - PRIM    seed infection
  # - TRAN    transmission event
  # - CONT    contact event
  # 
  # note:
  # - drop the first column with the log tag
  
  
  ######################
  ## PARTICIPANT DATA ##
  ######################
  header_part         <- c('local_id', 'part_age', 'part_gender', 'school_id', 'workplace_id',
                           'is_susceptible','is_infected','is_infectious','is_recovered','is_immune',
                           'start_infectiousness','start_symptomatic','end_infectiousness','end_symptomatic')
  data_part           <- data_log[data_log[,1] == "[PART]",seq_len(length(header_part))+1]
  names(data_part)    <- header_part
  data_part[1,]
  
  # set 'true' and 'false' in the R-format
  data_part[data_part=="true"] <- TRUE
  data_part[data_part=="false"] <- FALSE
  
  # make sure, all values (exept the gender and booleans) are stored as integers
  data_part[,-c(3,6:10)] <- data.frame(apply(data_part[,-c(3,6:10)], 2, as.integer))
  apply(data_part, 2, typeof)
  
  
  #######################
  ## TRANSMISSION DATA ##
  #######################
  header_transm       <- c('local_id', 'new_infected_id', 'cnt_location','sim_day')
  data_transm         <- data_log[data_log[,1] == "[PRIM]" | data_log[,1] == "[TRAN]",seq_len(length(header_transm))+1]
  names(data_transm)  <- header_transm
  data_transm[1,]
  
  # make sure, all values are stored as integers
  data_transm[,-3] <- data.frame(apply(data_transm[,-3], 2, as.integer))
  apply(data_transm, 2, typeof)
  
  
  ######################
  ## CONTACT DATA     ##
  ###################### 
  header_cnt          <- c('local_id', 'part_age', 'cnt_age', 'cnt_home', 'cnt_school', 'cnt_work', 'cnt_prim_comm', 'cnt_sec_comm', 'sim_day')
  data_cnt            <- data_log[data_log[,1] == "[CONT]",seq_len(length(header_cnt))+1]
  names(data_cnt)     <- header_cnt
  data_cnt[1,]
  
  # make sure, all values are stored as integers
  data_cnt <- data.frame(apply(data_cnt,  2, as.integer))
  dim(data_cnt)
  

  ######################
  ## STORE DATA       ##
  ######################
  exp_dir <- dirname(contact_log_filename)
  save(data_part,file=file.path(exp_dir,'data_participants.RData'))
  save(data_transm,file=file.path(exp_dir,'data_transmission.RData'))
  save(data_cnt,file=file.path(exp_dir,'data_contacts.RData'))
  
  # terminal message
  cat("LOG PARSING COMPLETE",fill=TRUE)
  
}

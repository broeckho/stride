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
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#############################################################################


plot_social_contacts <- function(data_tag,project_dir)
{
  
  summary_data_all	<- read.table(paste(data_tag,'_summary.csv',sep=''),header=TRUE,sep=",",stringsAsFactors=F)
  
  ######################
  ## GET DATA       ##
  ######################
  
  exp_dir <- './experiments'
  exp_files <- dir(exp_dir)
  exp_cnt_files <- exp_files[grepl('_contacts.csv',exp_files)]
  
  i_file <- 1
  if(length(exp_cnt_files)>0)
  for(i_file in 1:length(exp_cnt_files))
  {
   
    exp_tag       <- paste0('exp',i_file-1)
    exp_path      <- paste0('./experiments/',exp_tag)
    
    cdata         <- read.table(paste0(exp_path,'_contacts.csv'),sep=',',header=T)
    pdata         <- read.table(paste0(exp_path,'_participants.csv'),sep=',',header=T)
    summary_data  <- summary_data_all[i_file,]
    num_days      <- as.double(summary_data$num_days[i_file])
    
    
    if(dim(cdata)[1]>0 && dim(pdata)[1]>0)
    {
      
      ## people without contacts
      dim(pdata)[1] - length(unique(cdata$local_id))
      
      ## employed and student population
      pdata$employed <- pdata$workplace_id != 0
      pdata$student  <- pdata$school_id != 0
      
      # tmp   <- table(pdata$part_age,pdata$employed)
      # barplot(as.numeric(tmp[,2]),main='employed',xlab='age',ylab='count')
      # tmp   <- table(pdata$part_age,pdata$student)
      # plot(as.numeric(tmp[,2]),main='enrolled in school',xlab='age',ylab='count')
       
      ## SETTINGS 
      L <- 80
      
      tag <- 'debug'
      cdata_flag <- cdata$cnt_school == 1
      pdata_flag <- pdata$student == 1
      bool_gen_pop <- FALSE
     
      plot_cnt_matrix <- function(cdata_flag,pdata_flag,tag,num_days, bool_gen_pop=TRUE)
        
      {
        
        # select participants
        cdata_flag <- cdata_flag & cdata$local_id %in% pdata$local_id[pdata_flag] 
        table(cdata$part_age[cdata_flag])
        
        
        # temporary max age
        L_temp <- max(cdata$part_age,L,cdata$cnt_age)+1

        # count contacts
        mij_tbl <- table(cdata$part_age[cdata_flag],cdata$cnt_age[cdata_flag])
        row_ind <- as.numeric(row.names(mij_tbl)) +1 # age 0 == index 1
        col_ind <- as.numeric(colnames(mij_tbl))  +1 # age 0 == index 1
        mij <- matrix(0,L_temp,L_temp)
        mij[row_ind,col_ind] <- mij_tbl

        # count participant per age
        if(sum(mij)==0)
        {
          row_ind <- 1:(L+1)
          col_ind <- 1:(L+1)
        }
        # pdata_flag_update   <- pdata_flag & pdata$part_age %in% ((min(row_ind,col_ind):max(row_ind,col_ind))-1)
        pdata_flag_update   <- pdata_flag
        p_ages_tbl          <- table(pdata$part_age[pdata_flag_update])

        row_ind <- as.numeric(names(p_ages_tbl)) +1
        p_ages <- matrix(0,L_temp,1)
        p_ages[row_ind] <- p_ages_tbl

        # remove age>L (age_column L+1)
        mij <- mij[1:(L+1),1:(L+1)]
        p_ages <- p_ages[1:(L+1)]

        # adjust for number of participants (if age is present)
        ages_present <- p_ages>0
        p_ages[ages_present]
        #mij[ages_present,ages_present] <- mij[ages_present,ages_present]/rep(p_ages[ages_present],each=sum(ages_present)) ## CORRECT??!

        #contacts/participant
        for(j in 1:(L+1))
        {
          if(p_ages[j]>0)
          {
            mij[j,] <- mij[j,] / p_ages[j]
          }
        }
        
        # # mx <- mij[1:5,1:5]
        # # px <- p_ages[1:5]
        # # mx[1,]/p_ages[1:5]
        # # make mij reciproce
        # i <- 20
        # j <- 1
        # if(bool_gen_pop)
        #   for(i in 1:(L+1))
        #   {
        #     for(j in i:(L+1))
        #     {
        #       if(p_ages[j]>0)
        #       {
        #         mij[i,j] <- (mij[i,j] + mij[j,i])/2 
        #         mij[j,i] <- mij[i,j]
        #       }
        #     }
        #   }
        
        # account for multiple days
          mij <- mij/num_days
        
        # count (weighted number of contacts per participant)
        num_part <- dim(pdata)[1]
        names(pdata)
        count_cnt_age <- matrix(NA,L+1,num_part)
        tmp_cdata_local_id <- cdata$local_id[cdata_flag]
        for (j in 1:num_part){
          if(pdata_flag[j] & pdata$part_age[j] < (L+1))
          {
            part_age <- pdata$part_age[j]
            part_id  <- pdata$local_id[j]
            part_num_cnt <- sum(tmp_cdata_local_id == part_id)
            
            count_cnt_age[part_age+1,j] <- part_num_cnt
          }
        }
        
        # account for multiple days
        count_cnt_age <- count_cnt_age/num_days
        
        # get ranges
        bxplot_values <- boxplot(t(count_cnt_age),outline=F,plot=F)
        
        # plot matrix 
        image(c(0:L),c(0:L),mij,xlab="age of respondent",ylab="age of contact",main=paste(tag))
        
        # plot number of contacts
        num_cnt_age <- rowSums(mij)
        plot(num_cnt_age,main=paste(tag),xlab='age',ylab='count',ylim=c(0,max(bxplot_values$stats,na.rm=T)))  
        
        boxplot(t(count_cnt_age),outline=F,xaxt='n',plot=T,add=T)
        lines(num_cnt_age,main=paste(tag),col=2,lwd=4)  
        
        return(mij)
      }
      
      pdf(paste0(data_tag,'_cnt_',exp_tag,'.pdf'))
      par(mfrow=c(2,2))
      ## TOTAL
      mij_total <- plot_cnt_matrix(cdata$part_age>=0,pdata$part_age>=0,'total',num_days)
      
      ## HOUSEHOLD
      mij_hh <- plot_cnt_matrix(cdata$cnt_home==1,pdata$part_age>=0,'household',num_days)
      
      ## SCHOOL
      mij_school <- plot_cnt_matrix(cdata$cnt_school==1,pdata$student==T,'school',num_days,FALSE)
      
      ## WORK
      mij_work <- plot_cnt_matrix(cdata$cnt_work==1,pdata$employed==T,'work',num_days,FALSE)
      
      ## PRIMARY COMMUNITY
      mij_prim_comm <- plot_cnt_matrix(cdata$cnt_prim_comm==1,pdata$part_age>=0,'prim_comm',num_days)
      dim(mij_prim_comm)
      
      ## SECUNDARY COMMUNITY
      mij_sec_comm <- plot_cnt_matrix(cdata$cnt_sec_comm==1,pdata$part_age>=0,'sec_comm',num_days)
      dim(mij_sec_comm)
      
      #data_tag <- 'ref_fl2010'
      data_tag <- 'ref_fl2010_15touch'
      
      # LOAD SURVEY DATA FROM FLANDERS AND FULLY CONNECTED HOUSEHOLDS
      #survey_mij_hh         <- read.table(file=paste0(project_dir,'/data/ref_miami_household_gam_mij_rec.csv'),sep=';',dec=',',header=T)
      survey_mij_hh         <- read.table(file=paste0(project_dir,'/data/',data_tag,'_regular_weekday_household_gam_mij_rec.csv'),sep=';',dec=',',header=T)
      survey_mij_school     <- read.table(file=paste0(project_dir,'/data/',data_tag,'_regular_weekday_school_student_age24_gam_mij_median.csv'),sep=';',dec=',',header=T)
      survey_mij_work       <- read.table(file=paste0(project_dir,'/data/',data_tag,'_regular_weekday_workplace_employed_age_gam_mij_median.csv'),sep=';',dec=',',header=T)
      survey_mij_community  <- read.table(file=paste0(project_dir,'/data/',data_tag,'_regular_weekday_community_gam_mij_rec.csv'),sep=';',dec=',',header=T)
      survey_mij_total      <- read.table(file=paste0(project_dir,'/data/',data_tag,'_regular_weekday_gam_mij_rec.csv'),sep=';',dec=',',header=T)

      survey_mij_school_weekend    <- survey_mij_school*0
      survey_mij_work_weekend      <- survey_mij_work*0
      survey_mij_community_weekend  <- read.table(file=paste0(project_dir,'/data/',data_tag,'_weekend_community_gam_mij_rec.csv'),sep=';',dec=',',header=T)
      survey_mij_total_weekend     <- read.table(file=paste0(project_dir,'/data/',data_tag,'_weekend_gam_mij_rec.csv'),sep=';',dec=',',header=T)
       
    
      ## COMPARE
      par(mfrow=c(3,2))
      
      plot(rowSums(survey_mij_total),main='total',xlab='age',ylab='contacts',type='l',ylim=c(-0.1,30))
      lines(rowSums(survey_mij_total_weekend),main='total',xlab='age',ylab='contacts',type='l',lty=2)
      points(rowSums(mij_total),col=2)
      legend('topright',c('week','weekend','model'),col=c(1,1,2),lty=c(1,2,0),pch=c(-1,-1,1),cex=0.8)
      
      plot(rowSums(survey_mij_hh),main='household',xlab='age',ylab='contacts',type='l')
      points(rowSums(mij_hh),col=2)
      legend('topright',c('week','weekend','model'),col=c(1,1,2),lty=c(1,2,0),pch=c(-1,-1,1),cex=0.8)
      
      plot(rowSums(survey_mij_school),main='school',xlab='age',ylab='contacts',type='l',ylim=c(-0.1,20))
      lines(rowSums(survey_mij_school_weekend),type='l',lty=2)
      points(rowSums(mij_school),col=2)
      legend('topright',c('week','weekend','model'),col=c(1,1,2),lty=c(1,2,0),pch=c(-1,-1,1),cex=0.8)
      
      plot(rowSums(survey_mij_work),main='work',xlab='age',ylab='contacts',type='l',ylim=c(-0.1,20))
      lines(rowSums(survey_mij_work_weekend),type='l',lty=2)
      points(rowSums(mij_work),col=2)
      legend('topright',c('week','weekend','model'),col=c(1,1,2),lty=c(1,2,0),pch=c(-1,-1,1),cex=0.8)
      
      plot(rowSums(survey_mij_community),main='primary community',xlab='age',ylab='contacts',type='l',ylim=c(-0.1,25))
      lines(rowSums(survey_mij_community_weekend),type='l',lty=2)
      points(rowSums(mij_prim_comm),col=2)
      legend('topright',c('week','weekend','model'),col=c(1,1,2),lty=c(1,2,0),pch=c(-1,-1,1),cex=0.8)
      
      plot(rowSums(survey_mij_community),main='secondary community',xlab='age',ylab='contacts',type='l',ylim=c(-0.1,25))
      lines(rowSums(survey_mij_community_weekend),type='l',lty=2)
      points(rowSums(mij_sec_comm),col=2)
      legend('topright',c('week','weekend','model'),col=c(1,1,2),lty=c(1,2,0),pch=c(-1,-1,1),cex=0.8)
      
      dev.off()
      
      print(paste('PLOT SOCIAL CONTACTS COMPLETE for:', data_tag, exp_path))
      
      # end if-clause 
    } 
    
  } # end for(logfiles)
  
}





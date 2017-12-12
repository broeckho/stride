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
# SYNTHETIC POPULATIONS
#
# Extract person data from the RTI database and create input files 
# for the stride project.
# 
# Version 3
#
# Author: Lander Willem
# Last update: 13/02/2017  
#########################
rm(list=ls(all=TRUE))
gc()

#setwd('./synth_pop_US')


######################
## GET DATA 	 	##
######################

getPDF = TRUE;

if(0==1){
  # BROOKLYN, NY
  data_tag <- "2010_ver1_36047"
  geo_tag <- "brooklyn"
  extract_data(data_tag,geo_tag)
  
  # NASSAU, NY
  data_tag<-"2010_ver1_36059"
  geo_tag <- "nassau_v3"
  extract_data(data_tag,geo_tag)
  
  # # NY
  data_tag<-"2010_ver1_36"
  geo_tag <- "NY"
  extract_data(data_tag,geo_tag)
  
  # UTAH, TEXAS
  data_tag <- "2010_ver1_49"
  geo_tag <- "utah"
  extract_data(data_tag,geo_tag)
  
  # # MIAMI, FLORIDA
  data_tag <- "2010_ver1_12086"
  geo_tag <- "miami"
  extract_data(data_tag,geo_tag)
  
  # # FLORIDA
  data_tag <- "2010_ver1_12"
  geo_tag <- "florida"
  extract_data(data_tag,geo_tag)
  
  # OKLAHOMA
  data_tag <- "2010_ver1_40109"
  geo_tag <- "oklahoma"
  extract_data(data_tag,geo_tag)
  
  # ALASKA
  data_tag <- "2010_ver1_02"
  geo_tag <- "alaska"
  extract_data(data_tag,geo_tag)
  
  # WASHINGTON
  data_tag <- "2010_ver1_53"
  geo_tag <- "washington"
  extract_data(data_tag,geo_tag)
  
  
  
}

extract_data <- function(data_tag,geo_tag)
{
  
  # GET DATA
  data<-read.table(paste(data_tag,"/",data_tag,"_synth_people.txt",sep=""),header=TRUE,sep=",",stringsAsFactors=FALSE)
  
  names(data)
  data <- data[,c(1,2,5,10,11)]
  
  dim(data)
  popSize <- dim(data)[1]
  names(data)
  
  ##############################################
  ## OVERLAPPING SCHOOL AND WORK MEMBERSHIP 	##
  ##############################################
  
  # Some people (#8617) from age 16y-18y have school and workplace id
  # FIX: assign them to the workplace
  sum(!is.na(data$sp_school_id[!is.na(data$sp_work_id)]))
  table(data$age[!is.na(data$sp_school_id) & !is.na(data$sp_work_id)])
  data$sp_school_id[!is.na(data$sp_work_id)] <- NA
  
  
  ######################
  ## EXPLORE		 	##
  ######################
  
  # open pdf stream
  if(getPDF){
    pdf(file=paste("pop_",geo_tag,"_plot.pdf",sep=""),width=9,height=9)
  }
  
  #pop size (metadata alaska: 620490)
  dim(data)[1]
  # nb households (metadata alaska: 234779)
  length(unique(data$sp_hh_id[!is.na(data$sp_hh_id)])) 
  # nb schools (metadata alaska: 560)
  length(unique(data$sp_school_id[!is.na(data$sp_school_id)]))
  # nb workplaces (metadata alaska: 32609)
  length(unique(data$sp_work_id[!is.na(data$sp_work_id)]))
  
  # age distribution
  tmp <- table(data$age)
  names(tmp)[c(1,length(tmp))]
  barplot(tmp,main=paste('ages',sep=""),xlab='age',ylab='freq')
  
  # hh size distribution
  tmp <- table(table(data$sp_hh_id[!is.na(data$sp_hh_id)]))
  names(tmp)[c(1,length(tmp))]
  barplot(tmp,main=paste('household size',sep=""),xlab='size',ylab='freq')
  
  # school size distribution
  tmp_school <- table(table(data$sp_school_id[!is.na(data$sp_school_id)]))
  names(tmp_school)[c(1,length(tmp_school))]
  plot(tmp_school,main=paste(geo_tag,': school size',sep=""),xlab='size',ylab='freq')
  
  # workplace size distribution
  tmp_work <- table(table(data$sp_work_id[!is.na(data$sp_work_id)]))
  #tmp <- table(table(data$hub_id[!is.na(data$hub_id)]))
  names(tmp_work)[c(1,length(tmp_work))]
  plot(tmp_work,main=paste(geo_tag,': workplace size',sep=""),xlab='size',ylab='freq')
  
  ##########################
  ## REFORMAT ID's     ##
  ##########################
  
  ## HELPER FUNCTION
  f_original_id <- data$primary_community
  getSortedId <- function(f_original_id){
    
    # create new variable
    new_id <- f_original_id 
    #start index
    start_id <- 1
    # get sequence of id's
    order_id <- order(new_id)
    # give incremental id to the sorted id's
    order_new_id <- cumsum(c(start_id,diff(new_id[order_id])>0))
    # copy new id's back to the original vector
    new_id[order_id] <- order_new_id
    
    return(new_id)
  }
  
  # id ranges, initial
  c(min(data$sp_id),max(data$sp_id))
  c(min(data$sp_hh_id),max(data$sp_hh_id))
  c(min(data$sp_school_id,na.rm=T),max(data$sp_school_id,na.rm=T))
  c(min(data$sp_work_id,na.rm=T),max(data$sp_work_id,na.rm=T))
  
  # adjust: start with id 1
  data$sp_id        <- getSortedId(data$sp_id)
  data$sp_hh_id     <- getSortedId(data$sp_hh_id)
  data$sp_school_id <- getSortedId(data$sp_school_id)
  data$sp_work_id   <- getSortedId(data$sp_work_id)
  
  # id ranges, final
  c(min(data$sp_id),max(data$sp_id))
  c(min(data$sp_hh_id),max(data$sp_hh_id))
  c(min(data$sp_school_id,na.rm=T),max(data$sp_school_id,na.rm=T))
  c(min(data$sp_work_id,na.rm=T),max(data$sp_work_id,na.rm=T))
  
  
  ##########################
  ## CREATE COMMUNITIES   ##
  ##########################
  
  f_cluster_id <- data$sp_hh_id
  f_cluster_id <- data$sp_work_id
  
  f_popSize <- dim(data)[1]
  names(data)
  
  # FUNCTION: get neighborhood based on given cluster_id's. Start from id 1 and loop untill max id and start new neighborhood if maximum number of members is reached.
  getCommunity <- function(f_cluster_id,f_popSize){
    
    communitySize = 2000;
    maxNbCommunities = round(f_popSize / communitySize)
    
    
    # 1. GET FIRST AND LAST cluster_id FOR EACH NEIGHBORHOOD
    community_start = rep(0,maxNbCommunities)
    community_end = rep(0,maxNbCommunities) 
    
    sizes <- table(f_cluster_id)
    counter <- 0
    communityId <- 1;
    community_start[1] <- 0    #start with first cluster
    i<-597
    for(i in 1:length(sizes)){
      
      counter = counter + sizes[i]
      
      if(counter > communitySize){
        counter <- 0;
        community_end[communityId] = i;
        
        # next neighborhood
        communityId = communityId + 1;
        community_start[communityId] = i+1;
        
      }	
    }
    community_end[communityId] <- length(sizes) # end with last cluster_id
    
    community_start[1:10]
    community_end[1:10]
    community_start==community_end
    community_end - community_start
    
    
    # ASSIGN THE cluster_id's TO THEIR community
    community_id = rep(NA,popSize)
    i<-1
    for(i in 1:maxNbCommunities){
      flag <- f_cluster_id >= community_start[i] & f_cluster_id <= community_end[i]
      sum(flag)
      community_id[flag] = i
      
    }
    sum(!is.na(community_id))
    sum(is.na(community_id))
    
    # RETURN
    return(community_id)
  }
  
  # GET HOME community
  data$primary_community <- getCommunity(data$sp_hh_id,popSize)
  tmp <- table(table(data$primary_community))
  #plot(tmp,main=paste('geo_tag,': 'home neighborhood size',sep=""),xlab='size',ylab='freq')
  barplot(tmp,main=paste('home neighborhood size',sep=""),xlab='size',ylab='freq',cex.names=0.8)
  
  
  # GET DAY community = HOME community
  data$secundary_community <- data$primary_community                                                           # NEW !!!
  tmp <- table(table(data$secundary_community[data$secundary_community>0]))
  #plot(tmp,main=paste(geo_tag,': day neighborhood size',sep=""),xlab='size',ylab='freq')
  #barplot(tmp,main=paste('home neighborhood size',sep=""),xlab='size',ylab='freq')
  
  
  hub_stat <- matrix(NA,ncol=2,nrow=5)
  hub_stat_legend <- matrix(NA,ncol=1,nrow=dim(hub_stat)[1])
  hub_stat[,1] <- c(2000,2000,2101,3001,4001)
  
  hub_stat[1,2] <- sum(tmp[as.numeric(names(tmp))<hub_stat[2,1]])
  hub_stat_legend[1] <- paste('<',hub_stat[1,1],sep="")
  
  for(i in 2:(dim(hub_stat)[1]-1)){
    hub_stat[i,2] <- sum(tmp[as.numeric(names(tmp))>=hub_stat[i,1] & as.numeric(names(tmp))<hub_stat[i+1,1]])
    hub_stat_legend[i] <- paste(hub_stat[i,1],hub_stat[i+1]-1,sep="-")
  }
  hub_stat[5,2] <- sum(tmp[as.numeric(names(tmp))>hub_stat[5,1]])
  hub_stat_legend[5] <- paste('>',hub_stat[4,1],sep="")
  
  row.names(hub_stat) <- hub_stat_legend
  barplot(hub_stat[,2], main=paste('day community size',sep=""),xlab='size',ylab='freq')
  legend(x=4,y=max(hub_stat[,2]),paste('max size:', row.names(tmp)[length(tmp)]))
  
  
  ############################
  ## REFORMAT COMMUNITY ID's	##
  ############################
  
  # id ranges, initial
  c(min(data$primary_community),max(data$primary_community))
  c(min(data$secundary_community),max(data$secundary_community))
  
  # adjust: start with id 0
  data$primary_community <- getSortedId(data$primary_community)
  data$secundary_community  <- getSortedId(data$secundary_community)
  
  
  # id ranges, final
  c(min(data$primary_community),max(data$primary_community))
  c(min(data$secundary_community),max(data$secundary_community))
  
  
  ##########################
  ## STORE DATASET 		##
  ##########################
  #-------------------------------------------------------------
  # f_out <- out_new
  # f_basis <- out_new$hh_id
  # f_nb <- -20
  plotIndices <- function(f_out, f_basis, f_nb){
    
    f_size <- dim(f_out)[1]
    if(f_nb>0){
      f_out <- f_out[1:f_nb,]
    } else {
      f_out <- f_out[(f_size+f_nb):f_size,]
    }
    steps <- c(1,diff(f_basis))
    if(abs(f_nb)>100){
      steps <- 0
    }
    par(mfrow=c(2,3))
    plot(f_out$hh_id,main='hh id'); abline(0,0,0,c(which(steps==1)),lty=3)
    plot(f_out$school_id,main='school id') ;abline(0,0,0,c(which(steps==1)),lty=3)
    plot(f_out$work_id,main='work id') ;abline(0,0,0,c(which(steps==1)),lty=3)
    plot(f_out$primary_community,main='primary community') ;abline(0,0,0,c(which(steps==1)),lty=3)
    plot(f_out$secundary_community,main='secondary community') ;abline(0,0,0,c(which(steps==1)),lty=3)
    
    par(mfrow=c(1,1))
  }
  
  #-------------------------------------------------------------
  
  out <- data.frame(cbind(data$age,data$sp_hh_id,data$sp_school_id,data$sp_work_id,data$primary_community,data$secundary_community))
  dim(out)
  names(out) <- c("age","hh_id","school_id","work_id","primary_community","secundary_community")
  
  # set NA id's to ""
  out[is.na(out)] <- 0
  
  # # ORIGINAL ORDER
  # out_new <- out
  # plotIndices(out_new, out_new$hh_id,-3000)
  # write.table(out_new,file=paste("pop_",geo_tag,"_orig.csv",sep=""),sep=",",row.names=FALSE)
  
  # SORT: primary_community, household
  index <- with(out, order(primary_community, hh_id))
  out_new <- out[index,]
  #plotIndices(out_new, out_new$primary_community,-4000)
  write.table(out_new,file=paste("pop_",geo_tag,"_home_hh.csv",sep=""),sep=",",row.names=FALSE)
  
 
  ###################### 
  ## COORDINATES      ##
  ######################
  
  # GET DATA
  hh_data     <- read.table(paste(data_tag,"/",data_tag,"_synth_households.txt",sep=""),header=TRUE,sep=",",stringsAsFactors=FALSE)
  
  # REFORMAT
  hh_out     <- data.frame(cbind(getSortedId(hh_data$sp_id),    hh_data$latitude,    hh_data$longitude,    hh_data$hh_size))
  names(hh_out) <- c('hh_id','latitude','longitude','size')
  
  # SAVE
  write.table(hh_out,file=paste("pop_",geo_tag,"_households.csv",sep=""),sep=",",row.names=FALSE)
  
  
  library(ggmap)
  
  sel <- sample(1:dim(hh_data)[1],1000)
  c_lat <- hh_out$latitude[sel]
  c_lon <- hh_out$longitude[sel]
  g_data <- data.frame(lon=c_lon,lat=c_lat)
  
  
  print(ggmap(get_map(location = apply(g_data,2,mean), zoom=9, color='bw'),extent = "device") + 
                geom_point(data=g_data, aes(x=lon, y=lat), color="red"))
  
  
  # close pdf stream
  if(getPDF){
    dev.off()
  }

} # end function
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
#  Copyright 2017, Willem L
#############################################################################
#
# AGGREGATE THE SUMMARY INFO FROM ALL RUNS IN THE OUTPUT FOLDER
#
#############################################################################
rm(list=ls())

# set the directory to aggregate
main_dir <- 'output'

# get all files and folder names in the "main_dir"
all_tags <- dir(main_dir)

# only include folders (with output)
all_tags <- all_tags[!grepl('.csv',all_tags)]

# collect all summary data
all_data <- NULL
for(i in 1:length(all_tags)){
  
  data_file      <- paste0('./',main_dir,'/',all_tags[i],'/',all_tags[i],'_summary.csv')
  data           <- read.table(data_file, header=TRUE,sep=",",stringsAsFactors=F)
  data$sim_tag   <- all_tags[i]
  all_data       <- rbind(all_data,data)
  print(data_file)
  
}
dim(all_data)

# write all data to a csv file
file_name <- paste0('./',main_dir,'/','all_summary.csv')
write.csv(all_data,file=file_name,row.names=F)

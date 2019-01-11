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
# R script to pre-process the social contact rates into Stride xml format
#
#############################################################################

# load (xml) help functions
source('./bin/rstride/misc.R')

if(0==1) # for debugging
{
   source('./Misc.R')
  .rstride$set_wd()

    ref_data_tag  <- 'ref_fl2010'
    generate_social_contact_data_file(ref_data_tag)
  # if(grepl('15touch',exp_summary$age_contact_matrix_file)){
  #   ref_data_tag <- 'ref_fl2010_15touch'
  # }
}

#############################################################################
# GENERATE XML FILE WITH SOCIAL CONTACT PATTERNS                           ##
#############################################################################

generate_social_contact_data_file <- function(ref_data_tag,cnt_matrices_lib,postfix=''){
  
  ref_data_tag
  data_dir <- './data'
  output_prefix <- paste0('contact_matrix_',ref_data_tag)
  output_prefix <- sub('ref_fl2010','flanders',output_prefix)

  # add postfix?
  if(nchar(postfix)>0){
    output_prefix <- paste(output_prefix,postfix,sep='_')
  }
  
  # complete file names
  cnt_matrices_opt <- file.path(data_dir,paste(ref_data_tag,t(cnt_matrices_lib),sep='_'))
 
  # check files 
  bool_file_exists <- (is.na(cnt_matrices_lib) | file.exists(cnt_matrices_opt))
  if(any(!bool_file_exists)){
    print(cnt_matrices_opt[!bool_file_exists]) 
    print('file does not exist => STOP')
    return(-1)
  }
  
  # store social contact context
  cnt_context_opt <- names(cnt_matrices_lib)
  
  # generate matrix with "no contacts"
  # load one and set all values to '0'
  tmp_cnt_matrix_file_name <- cnt_matrices_opt[which(!is.na(cnt_matrices_lib))[1]]
  absence_cnt_matrix       <- read.table(file=tmp_cnt_matrix_file_name,sep=';',dec=',',header=T)
  absence_cnt_matrix       <- absence_cnt_matrix*0
  
  # setup XML doc (to add prefix)
  xml_doc = newXMLDoc()
  
  cnt_matrix_xml  <- newXMLNode("matrices", doc = xml_doc)
  cnt_matrix_meta <- newXMLNode("raw_data_files", parent = cnt_matrix_xml)
  i_context <- 1
  for(i_context in 1:length(cnt_matrices_lib))
  {
    print(cnt_matrices_opt[i_context])
    
    # add input file name
    cnt_meta_data           <- newXMLNode(names(cnt_matrices_lib)[i_context], parent=cnt_matrix_meta)
    xmlValue(cnt_meta_data) <- cnt_matrices_opt[i_context]
    
    # start from contact matrix with all '0'
    survey_mij <- absence_cnt_matrix 
    
    # add contact data if given tot this function
    if(!is.na(cnt_matrices_lib[i_context])){
      survey_mij <- read.table(file=cnt_matrices_opt[i_context],sep=';',dec=',',header=T)
    }
    
    # add matrix to XML
    cnt_context <- newXMLNode(cnt_context_opt[i_context], parent=cnt_matrix_xml)
    for(i in 1:nrow(survey_mij)){
      
      participant <- newXMLNode("participant",parent=cnt_context)
      
      part_age  <- newXMLNode("age",parent=participant)
      xmlValue(part_age) <- paste(i)
      
      contacts  <- newXMLNode("contacts",parent=participant)
      
      for(j in 1:ncol(survey_mij)){
        contact  <- newXMLNode("contact",parent=contacts)
        age <- newXMLNode("age", parent=contact);
        xmlValue(age) <- j
        rate <- newXMLNode("rate", parent=contact);
        xmlValue(rate) <- paste(survey_mij[i,j])
      }
    }
  }
  
  # create filename
  filename <- paste0(output_prefix,'.xml')
  
  # xml prefix
  xml_prefix <- paste0(' This file is part of the Stride software [', format(Sys.time()), ']')
  
  # save as XML,
  # note: if we use an XMLdoc to include prefix, the line break dissapears...
  # fix: http://r.789695.n4.nabble.com/saveXML-prefix-argument-td4678407.html
  cat( saveXML( xml_doc, indent = TRUE, prefix = newXMLCommentNode(xml_prefix)),  file = filename) 
  
}


if(0==1){
  
  ref_data_tag  <- 'ref_fl2010'
  
  # 3 level mixing (conditional)
  cnt_matrices_lib <- data.frame(household            = 'regular_weekday_household_gam_mij_rec.csv',
                                 school               = 'regular_weekday_school_conditional_age23_teachers_gam_mij_rec_median.csv',
                                 work                 = 'regular_weekday_workplace_conditional_gam_mij_rec_median.csv',
                                 secondary_community  = 'regular_weekday_community_gam_mij_rec.csv',
                                 primary_community    = 'weekend_community_gam_mij_rec.csv')
  generate_social_contact_data_file(ref_data_tag,cnt_matrices_lib,postfix='conditional_3level')
  
  # 2 level mixing (conditional)
  cnt_matrices_lib <- data.frame(household            = 'regular_weekday_household_gam_mij_rec.csv',
                                 school               = NA,
                                 work                 = NA,
                                 secondary_community  = 'regular_weekday_community_2level_gam_mij_rec.csv',
                                 primary_community    = 'weekend_community_gam_mij_rec.csv')
  generate_social_contact_data_file(ref_data_tag,cnt_matrices_lib,postfix='conditional_2level')
  
  # 1 level mixing (conditional)
  cnt_matrices_lib <- data.frame(household            = NA,
                                 school               = NA,
                                 work                 = NA,
                                 secondary_community  = 'regular_weekday_gam_mij_rec.csv',
                                 primary_community    = 'weekend_gam_mij_rec.csv')
  generate_social_contact_data_file(ref_data_tag,cnt_matrices_lib,postfix='conditional_1level')
  
  

}


#/* ******************************************************************************
#*  Maya PDC Particle Geometry Export SOP Houdini Extension Makefile
#*
#* $Revision: 1.18 $
#* $Source: /dca/cvsroot/houdini/SOP_PDC/Makefile,v $
#* $Author: mstory $
#* $Date: 2012-06-13 23:17:02 $
#* $Header: /dca/cvsroot/houdini/SOP_PDC/Makefile,v 1.18 2012-06-13 23:17:02 mstory Exp $
#* $State: Exp $
#* $Id: Makefile,v 1.18 2012-06-13 23:17:02 mstory Exp $
#* $Locker:  $
#*
#*  See Change History at the end of the file.
#*
#*    Digital Cinema Arts (C) 2006-2012
#*
#* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License. 
#* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
#* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
#*
#***************************************************************************** */
# 
SHELL=/bin/csh

# VERSION=${HOUDINI_VERSION}
SRC_VER = 1.5.3
SHELL=/bin/csh

pdc_src = dump_PDC.C  Maya_PDC_File.C  endian.h  Maya_PDC_File.h  
import_src = SOP_PDC_Import.C SOP_PDC_Import.h SOP_PDC_Import_readPDCFile.C 
export_src = SOP_PDC_Export.C SOP_PDC_Export.h SOP_PDC_Export_writePDCFile.C 
src = ${pdc_src} ${import_src} ${export_src} 

all: dump_PDC SOP_PDC_Import SOP_PDC_Export 
#all: SOP_PDC_Import SOP_PDC_Export 

dump_PDC: ${pdc_src}
	$(CXX) ${CFLAGS} -g $@.C -o $@ 
ifeq ($(OS),Windows_NT)
	cp $@.exe ${HOME}/bin/
else
	cp $@ ${HOME}/bin/
endif

TAGINFO = $(shell (echo -n "Compiled on:" `date`"\n  by:" `whoami`@`hostname`"\n$(SESI_TAGINFO)") | sesitag -m)
CFLAGS := $(CFLAGS) ${TAGINFO} 
CFLAGS := $(CFLAGS) -DSOP_MAJOR_VER=1 -DSOP_MINOR_VER=53

Maya_PDC_File:
	$(CXX) -g -m64 -fPIC -c -Wno-deprecated -Wall -W -Wno-parentheses -Wno-sign-compare -Wno-reorder -Wno-uninitialized -Wunused -Wno-unused-parameter -O2 -o Maya_PDC_File.o Maya_PDC_File.C
	$(CXX) -shared Maya_PDC_File.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o Maya_PDC_File.so


SOP_PDC_Import: ${pdc_src} $(import_src)
	$(CXX) -g ${CFLAGS} -DDLLEXPORT= -D_GNU_SOURCE -DLINUX -DAMD64 -m64 -fPIC -DSIZEOF_VOID_P=8 -DSESI_LITTLE_ENDIAN -DENABLE_THREADS -DUSE_PTHREADS -D_REENTRANT -D_FILE_OFFSET_BITS=64 -c  -DGCC4 -DGCC3 -Wno-deprecated -I/opt/hfs${HOUDINI_VERSION}/toolkit/include -I/opt/hfs${HOUDINI_VERSION}/toolkit/include/htools -Wall -W -Wno-parentheses -Wno-sign-compare -Wno-reorder -Wno-uninitialized -Wunused -Wno-unused-parameter -O2 -DMAKING_DSO -o SOP_PDC_Import.o SOP_PDC_Import.C
	$(CXX) -shared SOP_PDC_Import.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o ./SOP_PDC_Import.so


SOP_PDC_Export: ${pdc_src} $(export_src)
	$(CXX) -g ${CFLAGS} -DDLLEXPORT= -D_GNU_SOURCE -DLINUX -DAMD64 -m64 -fPIC -DSIZEOF_VOID_P=8 -DSESI_LITTLE_ENDIAN -DENABLE_THREADS -DUSE_PTHREADS -D_REENTRANT -D_FILE_OFFSET_BITS=64 -c  -DGCC4 -DGCC3 -Wno-deprecated -I/opt/hfs${HOUDINI_VERSION}/toolkit/include -I/opt/hfs${HOUDINI_VERSION}/toolkit/include/htools -Wall -W -Wno-parentheses -Wno-sign-compare -Wno-reorder -Wno-uninitialized -Wunused -Wno-unused-parameter -O2 -DMAKING_DSO -o SOP_PDC_Export.o SOP_PDC_Export.C
	$(CXX) -shared SOP_PDC_Export.o -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -o ./SOP_PDC_Export.so

archive_src:
	rm -fr html/
	doxygen Doxy_config.txt
	tar zcf Houdini_PDC_src_v${SRC_VER}.tar.gz \
	Doxy_config.txt \
	Makefile \
	html \
	$(src)
	cp Houdini_PDC_src_v${SRC_VER}.tar.gz src_archive

release: 
	rm -fr html/
	doxygen Doxy_config.txt
	rm -fr SOP_PDC_v${SRC_VER}
	mkdir SOP_PDC_v${SRC_VER}
	cp -r ${src} Doxy_config.txt Makefile html/ SOP_PDC_v${SRC_VER}/
	cd SOP_PDC_v${SRC_VER}; \
	   tar zcf Houdini_PDC_src_v${SRC_VER}.tar.gz *
	cp -r distro/ SOP_PDC_v${SRC_VER}/
	tar zcf Houdini_PDC_v${SRC_VER}.tar.gz SOP_PDC_v${SRC_VER}/
		
clean:	
	rm *.o *.so

#/**********************************************************************************/
#//  $Log: Makefile,v $
#//  Revision 1.18  2012-06-13 23:17:02  mstory
#//  .
#//
#//  Revision 1.17  2012-06-13 14:48:30  mstory
#//  Added H12 mods from Frederic Servant.
#//
#//  Revision 1.16  2012-06-12 18:42:08  mstory
#//  .
#//
#//  Revision 1.15  2012-06-12 18:36:10  mstory
#//  Added tooltips and a few extra debug messages.
#//
#//  Revision 1.13  2011-01-23 04:28:23  mstory
#//  .
#//
#//  Revision 1.12  2011-01-23 04:24:05  mstory
#//  modified the exporter for H11.
#//
#//  Revision 1.11  2011-01-23 02:11:22  mstory
#//  Modified importer for H11.
#//
#//  Revision 1.10  2010-04-20 21:14:43  mstory
#//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
#//
#//  --mstory
#//
#//  Revision 1.9  2008/05/23 20:30:26  mstory
#//  Release #1
#//
#//  Revision 1.8  2008/05/23 18:08:06  mstory
#//  Added code to build releases properly.
#//
#//  Revision 1.7  2008/05/22 00:53:17  mstory
#//  Added most of the attribute creation code for the importer and attribute detection for the exporter.
#//
#//  Revision 1.6  2008/05/21 16:16:58  mstory
#//  .
#//
#/**********************************************************************************/

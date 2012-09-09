/* ******************************************************************************
*  Maya PDC file dump program
*
* $Revision: 1.9 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/dump_PDC.C,v $
* $Author: mstory $
* $Date: 2012-06-13 14:48:30 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/dump_PDC.C,v 1.9 2012-06-13 14:48:30 mstory Exp $
* $State: Exp $
* $Id: dump_PDC.C,v 1.9 2012-06-13 14:48:30 mstory Exp $
* $Locker:  $
*
*  See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2006-2012
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>

#include "Maya_PDC_File.h"
#include "Maya_PDC_File.C"

using namespace std;
using namespace dca;

#define DEBUG


int main(int argc, char *argv[])
{

   dca::Maya_PDC_File *myPDCFile;
   unsigned int current_frame;
   unsigned int current_point;

   std::cout.precision(4);

   std::cout << "dump_PDC -  ver. 2.0.0 -  Digital Cinema Arts (C) 2012" << endl;
   std::cout << "Dumping Maya PDC File" << endl << endl;

#ifdef SOP_MAJOR_VER
   std::cout << "Version: " << SOP_MAJOR_VER << "." << SOP_MINOR_VER << std::endl;
#endif

   myPDCFile = new dca::Maya_PDC_File(0, 0);

   if (!myPDCFile->openPDCFile(std::string (argv[1]), dca::pdcReadFile)) {
      myPDCFile->readHeader();
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.format = " << myPDCFile->pdc_header.format << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.formatVersion = " << myPDCFile->pdc_header.formatVersion << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.byteOrder = " << myPDCFile->pdc_header.byteOrder << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.extra1 = " << myPDCFile->pdc_header.extra1 << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.extra2 = " << myPDCFile->pdc_header.extra2 << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.numParticles = " << myPDCFile->pdc_header.numParticles << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.numAttributes = " << myPDCFile->pdc_header.numAttributes << std::endl;
      std::cout << endl;

      if (!myPDCFile->readAllData())
         myPDCFile->closePDCFile(dca::pdcReadFile);

      delete(myPDCFile);

      std::cout << std::endl << "Finished dumping PDC data" << std::endl;

      exit(0);
   }



}

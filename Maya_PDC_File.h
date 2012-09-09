/* ******************************************************************************
* Maya PDC Particle Geometry File
*
* $Revision: 1.12 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/Maya_PDC_File.h,v $
* $Author: mstory $
* $Date: 2012-06-13 14:48:30 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/Maya_PDC_File.h,v 1.12 2012-06-13 14:48:30 mstory Exp $
* $State: Exp $
* $Id: Maya_PDC_File.h,v 1.12 2012-06-13 14:48:30 mstory Exp $
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

#ifndef __Maya_PDC_File_h__
#define __Maya_PDC_File_h__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "endian.h"

// int myFunc(char *s, int i) { printf("TEST 123\n\n\n"); printf ("%s\n\n", s); return i * 10;}
int myFunc(int a, int i)
{
   std::cout << a;
   return i * a;
}

namespace dca {

const int pdcReadFile = 0;
const int pdcWriteFile = 1;
const int pdcAttrNameLengthMax = 1024;

enum {
   pdcDataInt = 0,
   pdcDataIntArray,
   pdcDataDouble,
   pdcDataDoubleArray,
   pdcDataVector,
   pdcDataVectorArray,

   pdcDataINVALID = (2^16)
};


class Maya_PDC_File {

private:

   int attribute_count;
   int particle_count;

public:

   // Maya PDC header struct size = 28 bytes
   struct Maya_PDC_Header_struct {
      char format[4];      // "P" "D" "C" " "
      int formatVersion;   // always 1
      int byteOrder;       // always 1 for big endian
      int extra1;          // Always 0
      int extra2;          // Always 0
      int numParticles;
      int numAttributes;
   } pdc_header;

   struct point_struct {
      float pos[3];
   };

   struct particle_data_struct {
      std::vector <int> int_data;
      std::vector <double> double_data;
      std::vector <struct point_struct> pts;
   };

   // Data types
   // 0 = integer, 1 = integer array
   // 2 = double, 3 = double array
   // 4 = vector, 5 = vector array
   struct Maya_PDC_data_struct {
      int attrNameLength;
      std::string attrName;
      int attrDataType;
      struct particle_data_struct data;
   } pdc_data;

   Maya_PDC_File(const int num_particles, const int num_attributes);
   ~Maya_PDC_File();

   int openPDCFile(std::string myFileName, int mode);
   int closePDCFile(int mode);

   int readHeader();
   int readData(int i);
   int readAllData();
   int readDataHeader();
   int readDataRecord();

   int writeHeader();
   int writeDataHeader();
   int writeDataRecord(int data);
   int writeDataRecord(double data);
   int writeDataRecord(point_struct data);

   std::string PDCFileName;
   std::ifstream PDCInFileStream;
   std::ofstream PDCOutFileStream;

};

}

#endif

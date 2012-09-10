/* ******************************************************************************
* Maya PDC Particle  File
*
* $RCSfile: Maya_PDC_File.C,v $
*
* Description : Implementation for the Maya PDC class
*
* $Revision: 1.19 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/Maya_PDC_File.C,v $
* $Author: mstory $
* $Date: 2012-06-13 14:48:30 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/Maya_PDC_File.C,v 1.19 2012-06-13 14:48:30 mstory Exp $
* $State: Exp $
* $Id: Maya_PDC_File.C,v 1.19 2012-06-13 14:48:30 mstory Exp $
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

#ifndef __Maya_PDC_File_C__
#define __Maya_PDC_File_C__

#include <UT/UT_Endian.h>
#include "Maya_PDC_File.h"

// #define DEBUG

namespace dca {


/* ******************************************************************************
///  Function Name : Maya_PDC_File()
*
*  Description : Constructor for the Maya PDC class
///
*
///  Input Arguments : None
///
///  Return Value : None
///
***************************************************************************** */
Maya_PDC_File::Maya_PDC_File(const int num_particles, const int num_attributes):
      particle_count(23), attribute_count(46)
{
   pdc_header.format[0]='P';
   pdc_header.format[1]='D';
   pdc_header.format[2]='C';
   pdc_header.format[3]=' ';
   pdc_header.numParticles = num_particles;
   pdc_header.numAttributes = num_attributes;
}



/* ******************************************************************************
*  Function Name : ~Maya_PDC_File()
*
/// Description : Destructor for the Maya PDC class object
///
*
/// Input Arguments : None
*
/// Return Value : None
*
***************************************************************************** */
Maya_PDC_File::~Maya_PDC_File()
{

   // Empty

}


/* ******************************************************************************
/// Function Name : openPDCFile()
*
/// Description :
///
*
/// Input Arguments : std::string myFileName, int mode
*
/// Return Value : int (return status)
*
***************************************************************************** */
int Maya_PDC_File::openPDCFile(std::string myFileName, int mode)
{

#ifdef DEBUG
   std::cout << "Maya_PDC_File::Maya_PDC_File(): " << myFileName << "\tmode: " << mode << std::endl;
#endif

   if (mode == pdcReadFile) {

      try {
         PDCInFileStream.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit );
         PDCInFileStream.open(myFileName.c_str(), std::ios::in | std::ios::binary);
#ifdef DEBUG
         std::cout << "Maya_PDC_File::openPDCFile(): Opened Maya PDC file for reading" << std::endl;
#endif
      } catch (std::ios_base::failure& e ) {
         std::cerr << "Maya_PDC_File::openPDCFile()-EXCEPTION: " << e.what() <<  std::endl;
         std::cerr << "Maya_PDC_File::openPDCFile(): Can't open Maya PDC file for reading" << std::endl;
         PDCInFileStream.clear();
         return 1;
      }
   }

   else {

      try {
         PDCOutFileStream.exceptions ( std::ofstream::eofbit | std::ofstream::failbit | std::ofstream::badbit );
         PDCOutFileStream.open(myFileName.c_str(), std::ios::out | std::ios::binary);
#ifdef DEBUG
         std::cout << "Maya_PDC_File::Maya_PDC_File(): Opened Maya PDC file for writing" << std::endl;
#endif
      } catch (std::ios_base::failure& e ) {
         std::cerr << "Maya_PDC_File::openPDCFile()-EXCEPTION: " << e.what() <<  std::endl;
         std::cerr << "Maya_PDC_File::openPDCFile(): Can't open Maya PDC file for writing" << std::endl;
         PDCOutFileStream.clear();
         return 1;
      }
   }

   return 0;
}


/* ******************************************************************************
/// Function Name : readHeader()
*
/// Description : Read Maya PDC header record
///
*
/// Input Arguments : None
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::readHeader()
{

   try {
      PDCInFileStream.read((char *)&pdc_header.format, 4);
      PDCInFileStream.read((char *)&pdc_header.formatVersion, sizeof(int));
      PDCInFileStream.read((char *)&pdc_header.byteOrder, sizeof(int));
      PDCInFileStream.read((char *)&pdc_header.extra1, sizeof(int));
      PDCInFileStream.read((char *)&pdc_header.extra2, sizeof(int));
      PDCInFileStream.read((char *)&pdc_header.numParticles, sizeof(int));
      PDCInFileStream.read((char *)&pdc_header.numAttributes, sizeof(int));

      UTswap_int32(pdc_header.formatVersion, pdc_header.formatVersion);
      UTswap_int32(pdc_header.byteOrder, pdc_header.byteOrder);
      UTswap_int32(pdc_header.extra1, pdc_header.extra1);
      UTswap_int32(pdc_header.extra2, pdc_header.extra2);
      UTswap_int32(pdc_header.numParticles, pdc_header.numParticles);
      UTswap_int32(pdc_header.numAttributes, pdc_header.numAttributes);

#ifdef DEBUG
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.format = " << pdc_header.format << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.formatVersion = " << pdc_header.formatVersion << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.byteOrder = " << pdc_header.byteOrder << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.extra1 = " << pdc_header.extra1 << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.extra2 = " << pdc_header.extra2 << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.numParticles = " << pdc_header.numParticles << std::endl;
      std::cout << "Maya_PDC_File::readHeader(): pdc_header.numAttributes = " << pdc_header.numAttributes << std::endl;
#endif
   } catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::readHeader(): EXCEPTION: " << e.what() << std::endl;
      PDCInFileStream.clear();
      PDCInFileStream.close();
      return 1;
   }

   return 0;
}



/* ******************************************************************************
/// Function Name : readData()
*
/// Description :
///
*
/// Input Arguments : None
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::readData(int i)
{

   this->particle_count = 7;

   std::cout << "Maya_PDC_File::readData(): " << i << " " << this->particle_count << std::endl << std::endl;

   return 0;
}



/* ******************************************************************************
/// Function Name : dumpAllData()
*
/// Description :
///
*
/// Input Arguments : None
*
/// Return Value : int
*
***************************************************************************** */
int Maya_PDC_File::dumpAllData()
{

   std::cout << "Maya_PDC_File::dumpAllData()" << std::endl;

   char temp_name[pdcAttrNameLengthMax];
   int data_type;
   int int_data;
   double double_data;
   int num_particles;

   try {

      for (int num_attrs = 0; num_attrs < pdc_header.numAttributes; num_attrs++) {

         std::cout << "Maya_PDC_File::dumpAllData(): attribute number = " << num_attrs << std::endl;

         PDCInFileStream.read((char *)&pdc_data.attrNameLength, sizeof(int));
         UTswap_int32(pdc_data.attrNameLength, pdc_data.attrNameLength);

         std::cout << "Maya_PDC_File::dumpAllData(): pdc_data.attrNameLength = " << pdc_data.attrNameLength << std::endl;

         // If attribute name is not too long, else throw exception
         if (pdc_data.attrNameLength < pdcAttrNameLengthMax) {
            PDCInFileStream.read((char *)temp_name, pdc_data.attrNameLength);
            pdc_data.attrName.assign(temp_name, pdc_data.attrNameLength);
         }
         std::cout << "Maya_PDC_File::dumpAllData(): pdc_data.attrName = " << pdc_data.attrName << std::endl;



//  ########################
// THIS IS A TEMP HACK TO HANDLE "ghostFrames" ATTRIBUTES
//  ########################

         if (pdc_data.attrName == std::string("ghostFrames")) {
//   PDCInFileStream.close();
            std::cout << "Maya_PDC_File::readDataHeader(): ghostFrames Attibute encountered !!!" << std::endl;
            std::cout << "Maya_PDC_File::readDataHeader(): pdc_data.attrName = " << pdc_data.attrName << std::endl;
            return 0;
         }


         PDCInFileStream.read((char *)&data_type, sizeof(int));
         UTswap_int32(data_type, data_type);

         std::cout << "Maya_PDC_File::dumpAllData(): data_type = " << data_type << std::endl;


         switch (data_type) {

         case pdcDataInt:
            PDCInFileStream.read((char *)&int_data, sizeof(int));
            UTswap_int32(int_data, int_data);
            pdc_data.data.int_data.push_back(int_data);
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataInt - size: " << pdc_data.data.int_data.size() << std::flush << std::endl;
            pdc_data.data.int_data.clear();
            break;

         case pdcDataIntArray:
            for (num_particles = 0; num_particles < pdc_header.numParticles; num_particles++) {
               PDCInFileStream.read((char *)&int_data, sizeof(int));
               UTswap_int32(int_data, int_data);
               pdc_data.data.int_data.push_back(int_data);
            }
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataIntArray - num_particles: " << num_particles << std::flush << std::endl;
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataIntArray - size: " << pdc_data.data.int_data.size() << std::flush  << std::endl;
            pdc_data.data.int_data.clear();
            break;

         case pdcDataDouble:
            PDCInFileStream.read((char *)&double_data, sizeof(double));
//            UTswapBytes(&double_data, &double_data, 8);
            swapdouble(&double_data);
            pdc_data.data.double_data.push_back(double_data);
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataDouble - size: " << pdc_data.data.double_data.size() << std::flush  << std::endl;
            pdc_data.data.double_data.clear();
            break;

         case pdcDataDoubleArray:
            for (num_particles = 0; num_particles < pdc_header.numParticles; num_particles++) {
               PDCInFileStream.read((char *)&double_data, sizeof(double));
//               UTswapBytes(&double_data, &double_data, 8);
               swapdouble(&double_data);
               pdc_data.data.double_data.push_back(double_data);
            }
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataDoubleArray - num_particles: " << num_particles << std::flush << std::endl;
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataDoubleArray - size: " << pdc_data.data.double_data.size() << std::flush  << std::endl;
            pdc_data.data.double_data.clear();
            break;

         case pdcDataVector:
            PDCInFileStream.read((char *)&double_data, sizeof(double));
//            UTswapBytes(&double_data, &double_data, 8);
            swapdouble(&double_data);
            pdc_data.data.double_data.push_back(double_data);
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataVector - size: " << pdc_data.data.double_data.size() << std::flush  << std::endl;
            pdc_data.data.double_data.clear();
            break;

         case pdcDataVectorArray:
            for (num_particles = 0; num_particles < pdc_header.numParticles; num_particles++) {
               for (unsigned int j = 0; j < 3; j++) {
                  PDCInFileStream.read((char *)&double_data, sizeof(double));
//                  UTswapBytes(&double_data, &double_data, 8);
                  swapdouble(&double_data);
                  pdc_data.data.double_data.push_back(double_data);
               }
            }
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataVectorArray - num_particles: " << num_particles << std::flush << std::endl;
            std::cout << "Maya_PDC_File::dumpAllData(): pdcDataVectorArray - size: " << pdc_data.data.double_data.size() << std::flush  << std::endl;
            pdc_data.data.double_data.clear();
            break;

         default:
            // THROW EXCEPTION

            std::cout << "Maya_PDC_File::dumpAllData(): data_type = " << data_type << " INVALID DATA TYPE" << std::endl << std::endl;
            PDCInFileStream.close();
            return 1;

            break;

         }

      }

   }

   catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::dumpAllData(): EXCEPTION: " << e.what() << std::endl;
      PDCInFileStream.clear();
      PDCInFileStream.close();
      return 1;
   }


   return 0;
}



/* ******************************************************************************
/// Function Name : writeHeader()
*
/// Description :
///
*
/// Input Arguments : None
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::writeHeader()
{

#ifdef DEBUG
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.format = " << pdc_header.format << std::endl;
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.formatVersion = " << pdc_header.formatVersion << std::endl;
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.byteOrder = " << pdc_header.byteOrder << std::endl;
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.extra1 = " << pdc_header.extra1 << std::endl;
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.extra2 = " << pdc_header.extra2 << std::endl;
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.numParticles = " << pdc_header.numParticles << std::endl;
   std::cout << "Maya_PDC_File::writeHeader(): pdc_header.numAttributes = " << pdc_header.numAttributes << std::endl;
#endif

   UTswap_int32(pdc_header.formatVersion, pdc_header.formatVersion);
   UTswap_int32(pdc_header.byteOrder, pdc_header.byteOrder);
   UTswap_int32(pdc_header.extra1, pdc_header.extra1);
   UTswap_int32(pdc_header.extra2, pdc_header.extra2);
   UTswap_int32(pdc_header.numParticles, pdc_header.numParticles);
   UTswap_int32(pdc_header.numAttributes, pdc_header.numAttributes);


   try {
      PDCOutFileStream.write((const char *)&pdc_header.format, 4);
      PDCOutFileStream.write((const char *)&pdc_header.formatVersion, sizeof(int));
      PDCOutFileStream.write((const char *)&pdc_header.byteOrder, sizeof(int));
      PDCOutFileStream.write((const char *)&pdc_header.extra1, sizeof(int));
      PDCOutFileStream.write((const char *)&pdc_header.extra2, sizeof(int));
      PDCOutFileStream.write((const char *)&pdc_header.numParticles, sizeof(int));
      PDCOutFileStream.write((const char *)&pdc_header.numAttributes, sizeof(int));
   } catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::writeHeader(): EXCEPTION: " << e.what() << std::endl;
      PDCOutFileStream.clear();
      PDCOutFileStream.close();
      return 1;
   }

   return 0;
}



/* ******************************************************************************
/// Function Name : writeDataHeader()
*
/// Description :
///
*
/// Input Arguments : None
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::writeDataHeader()
{

   int nameLength;
   int data_type;
   UTswap_int32(pdc_data.attrNameLength, nameLength);
   UTswap_int32(pdc_data.attrDataType, data_type);

#ifdef DEBUG
   std::cout << "Maya_PDC_File::writeDataHeader(): nameLength = " << pdc_data.attrNameLength << std::endl;
   std::cout << "Maya_PDC_File::writeDataHeader(): attrName = " << pdc_data.attrName << std::endl;
   std::cout << "Maya_PDC_File::writeDataHeader(): data_type = " << pdc_data.attrDataType << std::endl;
#endif

   try {
      PDCOutFileStream.write((const char *)&nameLength, sizeof(int));
      PDCOutFileStream.write((const char *)pdc_data.attrName.c_str(), pdc_data.attrNameLength);
      PDCOutFileStream.write((const char *)&data_type, sizeof(int));
   } catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::writeDataHeader(): EXCEPTION: " << e.what() << std::endl;
      PDCOutFileStream.clear();
      PDCOutFileStream.close();
      return 1;
   }


   return 0;
}



/* ******************************************************************************
/// Function Name : readDataHeader()
*
/// Description :
///
*
/// Input Arguments : None
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::readDataHeader()
{

   char temp_name[pdcAttrNameLengthMax];
   int data_type;

#ifdef DEBUG
//std::cout << "Maya_PDC_File::readDataHeader()" << std::endl;
#endif


   try {
      PDCInFileStream.read((char *)&pdc_data.attrNameLength, sizeof(int));
      UTswap_int32(pdc_data.attrNameLength, pdc_data.attrNameLength);

#ifdef DEBUG
      std::cout << "Maya_PDC_File::readDataHeader(): pdc_data.attrNameLength = " << pdc_data.attrNameLength << std::endl;
#endif

      // If attribute name is not too long, else throw exception
      if (pdc_data.attrNameLength < pdcAttrNameLengthMax) {
         PDCInFileStream.read((char *)temp_name, pdc_data.attrNameLength);
         pdc_data.attrName.assign(temp_name, pdc_data.attrNameLength);
      }

#ifdef DEBUG
      std::cout << "Maya_PDC_File::readDataHeader(): pdc_data.attrName = " << pdc_data.attrName << std::endl;
#endif


//  ########################
// THIS IS A TEMP HACK TO HANDLE "ghostFrames" ATTRIBUTES
//  ########################

      if (pdc_data.attrName == std::string("ghostFrames")) {
//   PDCInFileStream.close();
//   std::cout << "Maya_PDC_File::readDataHeader(): ghostFrames Attibute encountered !!!" << std::endl;
//   std::cout << "Maya_PDC_File::readDataHeader(): pdc_data.attrName = " << pdc_data.attrName << std::endl;
         return 0;
      }




      // Read the data tpe for this attribute
      PDCInFileStream.read((char *)&data_type, sizeof(int));
      UTswap_int32(data_type, data_type);

#ifdef DEBUG
      std::cout << "Maya_PDC_File::readDataHeader(): data_type = " << data_type << std::endl;
#endif


      // Set the attribute data type
      switch (data_type) {

      case pdcDataInt:
#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): pdcDataInt " << std::endl;
#endif
         pdc_data.attrDataType = pdcDataInt;
         break;

      case pdcDataIntArray:
#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): pdcDataIntArray" << std::endl;
#endif
         pdc_data.attrDataType = pdcDataIntArray;
         break;

      case pdcDataDouble:
#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): pdcDataDouble" << std::endl;
#endif
         pdc_data.attrDataType = pdcDataDouble;
         break;

      case pdcDataDoubleArray:
#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): pdcDataDoubleArray" << std::endl;
#endif
         pdc_data.attrDataType = pdcDataDoubleArray;
         break;

      case pdcDataVector:
#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): pdcDataVector" << std::endl;
#endif
         pdc_data.attrDataType = pdcDataVector;
         break;

      case pdcDataVectorArray:
#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): pdcDataVectorArray" << std::endl;
#endif
         pdc_data.attrDataType = pdcDataVectorArray;
         break;


         // TODO: THROW EXCEPTION!!!
      default:

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataHeader(): INVALID DATA TYPE" << std::endl << std::endl;
#endif
         pdc_data.attrDataType = pdcDataINVALID;

         break;

      }

   } // try

   catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::readDataHeader(): EXCEPTION: " << e.what() << std::endl;
      PDCInFileStream.clear();
      PDCInFileStream.close();
      return 1;
   }

   return 0;
}



/* ******************************************************************************
/// Function Name : readDataRecord()
*
/// Description :
///
*
/// Input Arguments :
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::readDataRecord()
{

   int int_data;
   double double_data;

   struct vec_struct {
      double x;
      double y;
      double z;
   } vec_struct_data;

   union vec_union {
      double vec_array[3];
      vec_struct vec;
   } vec_data;

   struct point_struct pt;

#ifdef DEBUG
//std::cout << "Maya_PDC_File::readDataRecord()" << std::endl;
#endif

   try {

      switch (pdc_data.attrDataType) {

      case pdcDataInt:
         // Clear the container
         pdc_data.data.int_data.clear();

         PDCInFileStream.read((char *)&int_data, sizeof(int));
         UTswap_int32(int_data, int_data);
         pdc_data.data.int_data.push_back(int_data);

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): pdcDataInt - size: " << pdc_data.data.int_data.size() << std::endl;
#endif

         break;

      case pdcDataIntArray:
         // Clear the container
         pdc_data.data.int_data.clear();
         for (int particle_num = 0; particle_num < pdc_header.numParticles; particle_num++) {
            PDCInFileStream.read((char *)&int_data, sizeof(int));
            UTswap_int32(int_data, int_data);
            pdc_data.data.int_data.push_back(int_data);
         }

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): pdcDataIntArray - size: " << pdc_data.data.int_data.size() << std::endl;
#endif

         break;

      case pdcDataDouble:
         // Clear the container
         pdc_data.data.double_data.clear();
         PDCInFileStream.read((char *)&double_data, sizeof(double));
         swapdouble(&double_data);
//         UTswapBytes(&double_data, &double_data, 8);

         pdc_data.data.double_data.push_back(double_data);

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): pdcDataDouble - size: " << pdc_data.data.double_data.size() << std::endl;
#endif

         break;

      case pdcDataDoubleArray:
         // Clear the container
         pdc_data.data.double_data.clear();
         for (int particle_num = 0; particle_num < pdc_header.numParticles; particle_num++) {
            PDCInFileStream.read((char *)&double_data, sizeof(double));
            swapdouble(&double_data);
//            UTswapBytes(&double_data, &double_data, 8);
            pdc_data.data.double_data.push_back(double_data);
         }

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): pdcDataDoubleArray - size: " << pdc_data.data.double_data.size() << std::endl;
#endif

         break;

      case pdcDataVector:
         // Clear the container
         pdc_data.data.double_data.clear();

         PDCInFileStream.read((char *)&vec_data, sizeof(vec_struct_data));
//         UTswapBytes(&vec_data.vec.x, &vec_data.vec.x, 8);
//         UTswapBytes(&vec_data.vec.x, &vec_data.vec.y, 8);
//         UTswapBytes(&vec_data.vec.x, &vec_data.vec.z, 8);
         swapdouble(&vec_data.vec.x);
         swapdouble(&vec_data.vec.y);
         swapdouble(&vec_data.vec.z);
         pt.pos[0] = vec_data.vec.x;
         pt.pos[1] = vec_data.vec.y;
         pt.pos[2] = vec_data.vec.z;
         pdc_data.data.pts.push_back(pt);

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): pdcDataVector - size: " << pdc_data.data.double_data.size() << std::endl;
#endif

         break;

      case pdcDataVectorArray:
         // Clear the container
         pdc_data.data.pts.clear();

         for (int particle_num = 0; particle_num < pdc_header.numParticles; particle_num++) {
            PDCInFileStream.read((char *)&vec_data, sizeof(vec_struct_data));
//            UTswapBytes(&vec_data.vec.x, &vec_data.vec.x, 8);
//            UTswapBytes(&vec_data.vec.x, &vec_data.vec.y, 8);
//            UTswapBytes(&vec_data.vec.x, &vec_data.vec.z, 8);
            swapdouble(&vec_data.vec.x);
            swapdouble(&vec_data.vec.y);
            swapdouble(&vec_data.vec.z);
            pt.pos[0] = vec_data.vec.x;
            pt.pos[1] = vec_data.vec.y;
            pt.pos[2] = vec_data.vec.z;
            pdc_data.data.pts.push_back(pt);

         }

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): pdcDataVectorArray - size: " << pdc_data.data.double_data.size() << std::endl;
#endif

         break;

      default:

#ifdef DEBUG
         std::cout << "Maya_PDC_File::readDataRecord(): INVALID DATA TYPE" << std::endl << std::endl;
#endif

         break;

      }

   }

   catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::readDataRecord(): EXCEPTION: " << e.what() << std::endl;
      PDCInFileStream.clear();
      PDCInFileStream.close();
      return 1;
   }


   return 0;
}



/* ******************************************************************************
/// Function Name : writeDataRecord(int data)
*
/// Description :
///
*
/// Input Arguments : int data
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::writeDataRecord(int data)
{

#ifdef DEBUG
   std::cout << "Maya_PDC_File::writeDataRecord(int data): " << std::endl;
#endif

   int int_data;
   try {
      UTswap_int32(data, int_data);
      PDCOutFileStream.write((char *)&int_data, sizeof(int));
   }

   catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::writeDataRecord(int data): EXCEPTION: " << e.what() << std::endl;
      PDCOutFileStream.clear();
      PDCOutFileStream.close();
      return 1;
   }


   return 0;
}



/* ******************************************************************************
*  Function Name : writeDataRecord(double data)
*
*  Description :
*
*
*  Input Arguments : double data
*
*  Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::writeDataRecord(double data)
{

#ifdef DEBUG
   std::cout << "Maya_PDC_File::writeDataRecord(double data): " << std::endl;
#endif

   try {
      double dbl_data = data;
//      UTswapBytes(&dbl_data, &dbl_data, 8);
      swapdouble(&dbl_data);
      PDCOutFileStream.write((char *)&dbl_data, sizeof(double));
   }

   catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::writeDataRecord(double data): EXCEPTION: " << e.what() << std::endl;
      PDCOutFileStream.clear();
      PDCOutFileStream.close();
      return 1;
   }


   return 0;
}



/* ******************************************************************************
*  Function Name : writeDataRecord(point_struct data)
*
/// Description :
///
*
/// Input Arguments : point_struct data
*
/// Return Value : int
*
***************************************************************************** */
inline int Maya_PDC_File::writeDataRecord(point_struct data)
{

#ifdef DEBUG
   std::cout << "Maya_PDC_File::writeDataRecord(point_struct data): " << std::endl;
#endif

   struct data_struct {
      double x;
      double y;
      double z;
   } myData;


   try {

      myData.x = static_cast<double>(data.pos[0]);
      myData.y = static_cast<double>(data.pos[1]);
      myData.z = static_cast<double>(data.pos[2]);
      swapdouble(&myData.x);
      swapdouble(&myData.y);
      swapdouble(&myData.z);
//      UTswapBytes(&myData.x, &myData.x, 8);
//      UTswapBytes(&myData.y, &myData.y, 8);
//      UTswapBytes(&myData.z, &myData.z, 8);

      PDCOutFileStream.write((char *)&myData, sizeof(myData));
   }

   catch (std::ios_base::failure& e ) {
      std::cerr << "Maya_PDC_File::writeDataRecord(point_struct data): EXCEPTION: " << e.what() << std::endl;
      PDCOutFileStream.clear();
      PDCOutFileStream.close();
      return 1;
   }


   return 0;
}




/* ******************************************************************************
/// Function Name : closePDCFile()
*
/// Description : Close the Maya PDC file
///
*
/// Input Arguments : mode
*
/// Return Value : int
*
***************************************************************************** */
int Maya_PDC_File::closePDCFile(int mode)
{

#ifdef DEBUG
   std::cout << "Maya_PDC_File::closePDCFile(): Closing Maya PDC file" << std::endl;
#endif
   if (mode == pdcReadFile) {

      try {
         PDCInFileStream.close();
      } catch (std::ios_base::failure& e ) {
         std::cerr << "Maya_PDC_File::closePDCFile(): EXCEPTION: " << e.what() << std::endl;
//         PDCInFileStream.clear();
         return 1;
      }
   }

   else {

      try {
         PDCOutFileStream.close();
      }

      catch (std::ios_base::failure& e ) {
         std::cerr << "Maya_PDC_File::closePDCFile(): EXCEPTION: " << e.what() << std::endl;
         //       PDCOutFileStream.clear();
         return 1;
      }
   }


   return 0;
}




}


#endif


/**********************************************************************************/
//  $Log: Maya_PDC_File.C,v $
//  Revision 1.19  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.18  2012-06-12 18:36:10  mstory
//  Added tooltips and a few extra debug messages.
//
//  Revision 1.17  2011-01-23 04:27:28  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.16  2011-01-23 02:11:22  mstory
//  Modified importer for H11.
//
//  Revision 1.15  2010-04-21 23:07:18  mstory
//  Finshed modification final changes to version 1.5.0.  Ready for release.
//
//  Revision 1.14  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.13  2008/06/21 20:14:02  mstory
//  First release.
//
//  Revision 1.12  2008/05/23 07:26:33  mstory
//  Added most of the rest of the attribute exporting.  Needs to lookup entire attr dictionary, currently only
//  exporting "stamdard" attributes.
//
//  Revision 1.11  2008/05/22 01:09:35  mstory
//  *** empty log message ***
//
//  Revision 1.10  2008/05/22 00:53:17  mstory
//  Added most of the attribute creation code for the importer and attribute detection for the exporter.
//
//  Revision 1.9  2008/05/21 22:47:51  mstory
//  Writing point data (vector arrays)
//
//  Revision 1.8  2008/05/21 19:23:21  mstory
//  .
//
//  Revision 1.7  2008/05/21 16:15:30  mstory
//  .
//
//  Revision 1.6  2008/05/21 16:08:43  mstory
//  .
//
//  Revision 1.5  2008/05/21 07:17:38  mstory
//  Reading particles now.  Exporting header.  Needs more work to finish export attributes.
//
//  Revision 1.4  2008/05/21 04:51:58  mstory
//  Added code for attribute creation when reading PDC files.
//
//  Revision 1.3  2008/05/21 03:27:35  mstory
//  .
//
//  Revision 1.2  2008/05/20 23:11:42  mstory
//  Added read data record, SOP Import is creating points.
//  .
//
/**********************************************************************************/

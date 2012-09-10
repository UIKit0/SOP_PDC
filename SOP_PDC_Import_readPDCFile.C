/* ******************************************************************************
*  Read Maya PDC Particle Geometry File
*
* $Revision: 1.17 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Import_readPDCFile.C,v $
* $Author: mstory $
* $Date: 2012-09-03 16:56:05 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Import_readPDCFile.C,v 1.17 2012-09-03 16:56:05 mstory Exp $
* $State: Exp $
* $Id: SOP_PDC_Import_readPDCFile.C,v 1.17 2012-09-03 16:56:05 mstory Exp $
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

#include <UT/UT_Version.h>

/* ******************************************************************************
*  Function Name : ReadPDCSDFile()
*
*
*  Input Arguments : OP_Context &context
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR SOP_PDC_Import::ReadPDCFile(OP_Context &context)
{

   UT_Interrupt   *boss;
   GEO_Point *ppt;
   UT_XformOrder xformOrder(UT_XformOrder::SRT);
   UT_Matrix4 work_matrix;

   try {

#ifdef DEBUG
      std::cout << "myFileName:" << (const char *)myFileName << std::endl;
#endif

      // Check to see that there hasn't been a critical error in cooking the SOP.
      if (error() < UT_ERROR_ABORT) {

         gdp->clearAndDestroy();

         // Start the interrupt server
         boss = UTgetInterrupt();
         boss->opStart("Importing PDC File");

         // Open the PDC file
         if (myPDCFile->openPDCFile(myFileName.toStdString(), dca::pdcReadFile))
            throw SOP_PDC_Import_Exception(canNotOpenPDCFileForReading, exceptionError);

         // read the PDC file header
         if (myPDCFile->readHeader())
            throw SOP_PDC_Import_Exception(canNotReadPDCFileHeader, exceptionError);

         /*
         #ifdef DEBUG
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.format = " << myPDCFile->pdc_header.format << std::endl;
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.formatVersion = " << myPDCFile->pdc_header.formatVersion << std::endl;
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.byteOrder = " << myPDCFile->pdc_header.byteOrder << std::endl;
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.extra1 = " << myPDCFile->pdc_header.extra1 << std::endl;
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.extra2 = " << myPDCFile->pdc_header.extra2 << std::endl;
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.numParticles = " << myPDCFile->pdc_header.numParticles << std::endl;
         std::cout << "SOP_PDC_Import::ReadPDCFile(): pdc_header.numAttributes = " << myPDCFile->pdc_header.numAttributes << std::endl;
         #endif
         */

         // Add a point for each particle
         for (int i = 0; i < myPDCFile->pdc_header.numParticles; i++) {

            if (boss->opInterrupt())
               throw SOP_PDC_Import_Exception(cookInterrupted, exceptionWarning);
            // Append a point the geometry detail
            ppt = gdp->appendPointElement();
         }

#ifdef DEBUG
         std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): Finished adding points: " << (long int)gdp->points().entries() <<  std::endl;
#endif

         // For each attribute, read the type and add the appropriate attribute
         for (int attr_num = 0; attr_num < myPDCFile->pdc_header.numAttributes; attr_num++) {

#ifdef DEBUG
            std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile():  attribute number: " << attr_num <<  std::endl;
#endif

            // Read the data record header (attribute name length and attribute name)
            if (myPDCFile->readDataHeader())
               throw SOP_PDC_Import_Exception(canNotReadPDCFileDataHeader, exceptionError);

#ifdef DEBUG
            std::cout << "SOP_PDC_Import::ReadPDCFile():  attribute name: " << myPDCFile->pdc_data.attrName <<  std::endl;
#endif


// ##################################
// HACK ALERT!!!
// ##################################

            // If this attribute is "ghostFrames", abort the import (until it's later determined how to handle the attribute)
            if (myPDCFile->pdc_data.attrName == std::string("ghostFrames")) {

               // We really should throw an exception, but this is an unknown attribute that "appears" harmless at the end of some PDC files
//
//              throw SOP_PDC_Import_Exception(ghostFramesAttrEncountered, exceptionWarning);

               // Close the PDC file
               if (myPDCFile->closePDCFile(dca::pdcReadFile))
                  throw SOP_PDC_Import_Exception(canNotCloseThePDCFile, exceptionError);

               // We're done
               boss->opEnd();

               return error();
            }



            // Read the data record
            if (myPDCFile->readDataRecord())
               throw SOP_PDC_Import_Exception(canNotReadPDCFileDataRecord, exceptionError);


            // If this attribute is "position", set the points position
            if (myPDCFile->pdc_data.attrName == "position") {
#ifdef DEBUG
               std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile():  Setting point positions" <<  std::endl;
#endif
               for (int i = 0; i < myPDCFile->pdc_header.numParticles; i++) {

                  if (boss->opInterrupt())
                     throw SOP_PDC_Import_Exception(cookInterrupted, exceptionWarning);

//                  std::cout << "SOP_PDC_Import::ReadPDCFile():  myPDCFile->pdc_data.data.pts[i].pos[0]" << myPDCFile->pdc_data.data.pts[i].pos[0] <<  std::endl;

//                  ppt->getPos().assign((const float)myPDCFile->pdc_data.data.pts[i].pos[0],
                  gdp->points()[i]->setPos(myPDCFile->pdc_data.data.pts[i].pos[0],
                                           myPDCFile->pdc_data.data.pts[i].pos[1],
                                           myPDCFile->pdc_data.data.pts[i].pos[2],
                                           1.0);

               }

               // Clear the container
               myPDCFile->pdc_data.data.pts.clear();
#ifdef DEBUG
               std::cout << "SOP_PDC_Import::ReadPDCFile():  Cleared data container" <<  std::endl;
#endif

            }


            else {

               GA_RWAttributeRef attrRef;
               GA_RWHandleI attrIntHandle;
               GA_RWHandleF attrFloatHandle;
               GA_RWHandleV3 attrVector3Handle;
               // Add attributes based on the PDC data type
               switch (myPDCFile->pdc_data.attrDataType) {


                  // Add an int detail attribute
               case dca::pdcDataInt:
#ifdef DEBUG
                  std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): pdcDataInt " << std::endl;
#endif

                  attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, myPDCFile->pdc_data.attrName.c_str(), 1);
                  if (attrRef.isValid()) {
                     attrIntHandle.bind(attrRef.getAttribute());
                     attrIntHandle.set(0, myPDCFile->pdc_data.data.int_data[0]);
                  } else
                     throw SOP_PDC_Import_Exception(canNotCreateDetailIntAttribute, exceptionError);

                  break;


                  // Add an int point attribute
               case dca::pdcDataIntArray:
#ifdef DEBUG
                  std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): pdcDataIntArray" << std::endl;
#endif

                  attrRef = gdp->addIntTuple(GA_ATTRIB_POINT, myPDCFile->pdc_data.attrName.c_str(), 1);
                  if (attrRef.isInvalid())
                     throw SOP_PDC_Import_Exception(canNotCreatePointIntArrayAttribute, exceptionError);

                  attrIntHandle.bind(attrRef.getAttribute());

                  for (int i = 0; i < myPDCFile->pdc_header.numParticles; i++) {
                     if (boss->opInterrupt())
                        throw SOP_PDC_Import_Exception(cookInterrupted, exceptionWarning);
//std::cout << "SOP_PDC_Import::ReadPDCFile(): Particle Data: " << i << " " << myPDCFile->pdc_data.data.int_data[i] << std::endl;
                     attrIntHandle.set(gdp->pointOffset(i), myPDCFile->pdc_data.data.int_data[i]);
                  }

                  break;


                  // Add a double (float) detail attribute
               case dca::pdcDataDouble:
#ifdef DEBUG
                  std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): pdcDataDouble" << std::endl;
#endif

                  attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, myPDCFile->pdc_data.attrName.c_str(), 1);
                  if (attrRef.isValid()) {
                     attrFloatHandle.bind(attrRef.getAttribute());
                     attrFloatHandle.set(0, myPDCFile->pdc_data.data.double_data[0]);
                  } else
                     throw SOP_PDC_Import_Exception(canNotCreateDetailDoubleAttribute, exceptionError);

                  break;


                  // Add a double (float) point attribute
               case dca::pdcDataDoubleArray:
#ifdef DEBUG
                  std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): pdcDataDoubleArray" << std::endl;
#endif

                  attrRef = gdp->addFloatTuple(GA_ATTRIB_POINT, myPDCFile->pdc_data.attrName.c_str(), 1);
                  if (attrRef.isInvalid())
                     throw SOP_PDC_Import_Exception(canNotCreatePointDoubleArrayAttribute, exceptionError);

                  attrFloatHandle.bind(attrRef.getAttribute());

                  for (int i = 0; i < myPDCFile->pdc_header.numParticles; i++) {

                     if (boss->opInterrupt())
                        throw SOP_PDC_Import_Exception(cookInterrupted, exceptionWarning);
//std::cout << "SOP_PDC_Import::ReadPDCFile(): Particle Data: " << i << " " << myPDCFile->pdc_data.data.double_data[i] << std::endl;

                     attrFloatHandle.set(gdp->pointOffset(i), myPDCFile->pdc_data.data.double_data[i]);
                  }

                  break;


                  // Add a vector detail attribute
               case dca::pdcDataVector:
#ifdef DEBUG
                  std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): pdcDataVector" << std::endl;
#endif

                  attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, myPDCFile->pdc_data.attrName.c_str(), 3);
                  if (attrRef.isValid()) {
                     attrVector3Handle.bind(attrRef.getAttribute());
                     attrVector3Handle.set(0, UT_Vector3(myPDCFile->pdc_data.data.pts[0].pos));
                  } else
                     throw SOP_PDC_Import_Exception(canNotCreateDetailVectorAttribute, exceptionError);

                  break;


                  // Add a vector point attribute
               case dca::pdcDataVectorArray:
#ifdef DEBUG
                  std::cout << std::endl << "SOP_PDC_Import::ReadPDCFile(): pdcDataVectorArray" << std::endl;
#endif

                  attrRef = gdp->addFloatTuple(GA_ATTRIB_POINT, myPDCFile->pdc_data.attrName.c_str(), 3);
                  if (attrRef.isInvalid())
                     throw SOP_PDC_Import_Exception(canNotCreatePointVectorArrayAttribute, exceptionError);

                  attrVector3Handle.bind(attrRef.getAttribute());

                  for (int i = 0; i < myPDCFile->pdc_header.numParticles; i++) {

                     if (boss->opInterrupt())
                        throw SOP_PDC_Import_Exception(cookInterrupted, exceptionWarning);

//std::cout << "SOP_PDC_Import::ReadPDCFile(): Particle Data: " << i << " " << myPDCFile->pdc_data.data.pts[i].pos[0] << " " << myPDCFile->pdc_data.data.pts[i].pos[1] << " "  << myPDCFile->pdc_data.data.pts[i].pos[2] << std::endl;
                     attrVector3Handle.set(gdp->pointOffset(i), UT_Vector3(myPDCFile->pdc_data.data.pts[i].pos));
                  }

                  break;

               default:

#ifdef DEBUG
                  std::cout << "SOP_PDC_Import::ReadPDCFile(): INVALID DATA TYPE" << std::endl << std::endl;
#endif
//                     throw SOP_PDC_Import_Exception(invalidPDCDataType, exceptionError);
                  throw SOP_PDC_Import_Exception(invalidPDCDataType, exceptionWarning);

                  break;

               }

               // Add local variable for this attribute
               UT_String attrVar = (const char *)myPDCFile->pdc_data.attrName.c_str();
               attrVar.toUpper();
               gdp->addVariableName((const char *)myPDCFile->pdc_data.attrName.c_str(), (const char *)attrVar);

            } // else (not position attribute)

         } // for each attribute ...

         // Close the PDC file
         if (myPDCFile->closePDCFile(dca::pdcReadFile))
            throw SOP_PDC_Import_Exception(canNotCloseThePDCFile, exceptionError);

         // We're done
         boss->opEnd();
      }


   }


// Exception handler
   catch (SOP_PDC_Import_Exception e) {
      e.what();

      if (e.getSeverity() == exceptionWarning)
         addWarning(SOP_MESSAGE, errorMsgsPDCImport[e.getErrorCode()]);
      else if (e.getSeverity() == exceptionError)
         addError(SOP_MESSAGE, errorMsgsPDCImport[e.getErrorCode()]);

      boss->opEnd();

      if (myPDCFile->PDCInFileStream.is_open()) {
         // Close the Maya PDC file
         if (myPDCFile->closePDCFile(dca::pdcReadFile)) {
            addError(SOP_MESSAGE, "Can't close Maya PDC file after SOP_PDC_Import_Exception exception was thrown");
            return error();
         }
      }
      return error();
   }


   return error();
}


/**********************************************************************************/
//  $Log: SOP_PDC_Import_readPDCFile.C,v $
//  Revision 1.17  2012-09-03 16:56:05  mstory
//  .
//
//  Revision 1.16  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.15  2011-01-23 04:27:29  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.14  2011-01-23 04:24:05  mstory
//  modified the exporter for H11.
//
//  Revision 1.13  2011-01-23 02:11:22  mstory
//  Modified importer for H11.
//
//  Revision 1.12  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.11  2008/06/21 20:14:03  mstory
//  First release.
//
//  Revision 1.10  2008/05/23 20:15:13  mstory
//  Cleaned up for a clean compile.
//
//  Revision 1.9  2008/05/23 07:26:33  mstory
//  Added most of the rest of the attribute exporting.  Needs to lookup entire attr dictionary, currently only
//  exporting "stamdard" attributes.
//
//  Revision 1.8  2008/05/22 00:53:18  mstory
//  Added most of the attribute creation code for the importer and attribute detection for the exporter.
//
//  Revision 1.7  2008/05/21 16:15:30  mstory
//  .
//
//  Revision 1.6  2008/05/21 07:17:38  mstory
//  Reading particles now.  Exporting header.  Needs more work to finish export attributes.
//
//  Revision 1.5  2008/05/21 04:51:58  mstory
//  Added code for attribute creation when reading PDC files.
//
//  Revision 1.4  2008/05/21 03:27:35  mstory
//  .
//
//  Revision 1.3  2008/05/20 23:11:42  mstory
//  Added read data record, SOP Import is creating points.
//  .
//
/**********************************************************************************/

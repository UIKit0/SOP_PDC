/* ******************************************************************************
*  Maya PDC Particle Geometry file exporter for Houdini 
*
* $Revision: 1.21 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Export_writePDCFile.C,v $
* $Author: mstory $
* $Date: 2012-06-13 14:48:30 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Export_writePDCFile.C,v 1.21 2012-06-13 14:48:30 mstory Exp $
* $State: Exp $
* $Id: SOP_PDC_Export_writePDCFile.C,v 1.21 2012-06-13 14:48:30 mstory Exp $
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
*  Function Name :  writePDCFile()
*
*  Description : Write the Maya PDC Particle file 
*
*  Input Arguments : OP_Context &context
*
*  Return Value : enumErrorList error_num
*
***************************************************************************** */
int SOP_PDC_Export::writePDCFile(OP_Context &context) {
#if UT_MAJOR_VERSION_INT >= 12
   float now = context.getTime();
#else
   float  now = context.myTime;
#endif

   UT_Vector3   vec; 

   int cur_frame, start_frame, end_frame;
   long int  save_frame = context.getFrame();
   UT_Vector3 normal, p;
   UT_Vector4 pos;
   GEO_Point *ppt;
   UT_Interrupt   *boss;

   // If this cook was not inititiated by the user pressing the 
   // "Write the File" button (the display flag was set), do not write the file.
   if(!calledFromCallback) { 
      if (lockInputs(context) >= UT_ERROR_ABORT) {
         return(canNotLockInputsInWritePDCFile);
      }
      // Duplicate the geometry from the first input
      duplicateSource(0, context);
      unlockInputs();
      // Restore the frame 
      context.setFrame((long)save_frame);

      return 0;
   }

   // Reset the flag for the next button pressed event
   calledFromCallback = false;

   long int frame_offset = abs(int(myBeginEnd[0]));

   if(int(myBeginEnd[0]) < 0) {
      start_frame = int(myBeginEnd[0]) + frame_offset;
      end_frame = int(myBeginEnd[1]) + frame_offset;
   }
   else {
      start_frame = int(myBeginEnd[0]);
      end_frame = int(myBeginEnd[1]);
   }

#ifdef DEBUG
cout << "SOP_PDC_Export::writePDCFile(): start_frame: " << start_frame << endl;
cout << "SOP_PDC_Export::writePDCFile(): end_frame: " << end_frame << endl;
cout << "SOP_PDC_Export::writePDCFile(): frame_offset: " << frame_offset << endl;
#endif

UT_WorkArgs myAttrArgs;

//myAttrs.parse(myAttrArgs);
myAttrs.tokenize(myAttrArgs, " ");

// cout << "argc: " << myAttrArgs.getArgc() << endl;
// 
//    for (int tok = 0; tok < myAttrArgs.getArgc(); tok++) {
//       cout << "arg: " << myAttrArgs[tok] << " " << tok << endl;
//    }


   try {

         // Start the interrupt server
         boss = UTgetInterrupt();
         boss->opStart("Exporting PDC File");

      // For each frame in our animation ...
      for (cur_frame = start_frame; cur_frame <= end_frame; cur_frame++) {

         context.setFrame((long)cur_frame);
#if UT_MAJOR_VERSION_INT >= 12
         now = context.getTime();
#else
         now = context.myTime;
#endif


         // Evaluate the GUI parameters
         FNAME(myFileName, now);

#ifdef DEBUG
cout << "SOP_PDC_Export::writePDCFile(): myFileName: " << myFileName << endl;
cout << "SOP_PDC_Export::writePDCFile(): cur_frame: " << cur_frame << endl;
#endif

cout << "SOP_PDC_Export::writePDCFile(): Current Frame: " << cur_frame <<  endl;


         if (lockInputs(context) >= UT_ERROR_ABORT) 
            throw SOP_PDC_Export_Exception(canNotLockInputsInWritePDCFile, exceptionError);
   
         if (boss->opInterrupt()) 
            throw SOP_PDC_Export_Exception(cookInterrupted, exceptionWarning);

         // Check to see that there hasn't been a critical error in cooking the SOP.
         if (error() < UT_ERROR_ABORT) {

            // Get current time and duplicate the geometry
#if UT_MAJOR_VERSION_INT >= 12
         now = context.getTime();
#else
         now = context.myTime;
#endif
            duplicateSource(0, context);

            if(myPDCFile->openPDCFile(myFileName.toStdString(), dca::pdcWriteFile)) 
               throw SOP_PDC_Export_Exception(canNotOpenPDCFileForWriting, exceptionError);

#ifdef DEBUG
cout << "SOP_PDC_Export::writePDCFile(): Opened PDC file for writing" << endl;
#endif

            typedef struct attr_found_struct attrType;
            attrType attr;

            std::map<std::string, attrType> attrMapFound;
            std::map<std::string, attrType>::iterator attrMapFoundIter;

            std::map<std::string, attrType> attrMap;
            std::map<std::string, attrType>::iterator attrMapIter;


            for (int tok = 0; tok < myAttrArgs.getArgc(); tok++) {
//               cout << "Creating attribute entry: " << myAttrArgs[tok] << " " << tok << endl;
               attr.attrName = myAttrArgs[tok];
               attrMap[myAttrArgs[tok]] = attr;
            }


//          GEO_PointAttribDict myPointAttrDict = gdp->pointAttribs();    
//          cout << "myPointAttrDict.getSize(): " << myPointAttrDict.getSize() << endl;

            uint numAttrsInGeo = 0;
#if UT_MAJOR_VERSION_INT >= 12
            GA_ROAttributeRef attrRef;
            GA_ROAttributeRef tmpAttr;
#else
            GB_Attribute *tmpAttr;
    #if UT_MAJOR_VERSION_INT >= 10
            GB_AttributeRef attrRef;
    #endif
#endif
 
            for (attrMapIter=attrMap.begin(); attrMapIter != attrMap.end(); attrMapIter++) {
// cout << "Looking for attribute: " << (*attrMapIter).first << endl;
#if UT_MAJOR_VERSION_INT >= 12
               tmpAttr = gdp->findPointAttribute((*attrMapIter).first.c_str());
#else
               tmpAttr = gdp->pointAttribs().find(UT_String(((*attrMapIter).first).c_str()));
#endif
               //  Look for attributes, if found, increment attr counter 
#if UT_MAJOR_VERSION_INT >= 12
               if (tmpAttr.isValid()) {
#else
               if(tmpAttr != NULL) {
#endif
                  attrMapFound[(const char *)(*attrMapIter).first.c_str()] = (*attrMapIter).second;
                  attr.attrName = attrMapFound[(const char *)(*attrMapIter).first.c_str()].attrName;
#if UT_MAJOR_VERSION_INT >= 12
                  attr.attrType = tmpAttr.getStorageClass();
                  attr.attrSize = tmpAttr.getTupleSize();
#else
                  attr.attrType = static_cast<GB_AttribType>(tmpAttr->getType());

                  switch (attr.attrType) {
                     case GB_ATTRIB_FLOAT: 
                        attr.attrSize = (tmpAttr->getSize() / SIZEOF_FLOAT);
                        break;

                     case GB_ATTRIB_INT: 
                        attr.attrSize = (tmpAttr->getSize() / SIZEOF_INT);
                        break;

                     case GB_ATTRIB_VECTOR: 
                        attr.attrSize = (tmpAttr->getSize() / SIZEOF_FLOAT);
                        break;

/*                     case GB_ATTRIB_STRING:
                        break;
*/
                     case GB_ATTRIB_MIXED:
                        break;
                     case GB_ATTRIB_INDEX:
                        break;
                  }
#endif
                  attrMapFound[(const char *)(*attrMapIter).first.c_str()] = attr;

                  numAttrsInGeo++;

//cout << "Attribute: " << attrMapFound[(const char *)(*attrMapIter).first.c_str()].attrName;
//cout << " Attribute Type: " << attrMapFound[(const char *)(*attrMapIter).first.c_str()].attrType << " numAttrsInGeo: " << numAttrsInGeo << endl;
               }
            } // end for loop ...

#ifdef DEBUG
for (attrMapFoundIter=attrMapFound.begin(); attrMapFoundIter != attrMapFound.end(); attrMapFoundIter++) 
      cout << "### attribute: " << (*attrMapFoundIter).second.attrName << " "  << (*attrMapFoundIter).second.attrType << endl;
#endif


            // Initiliaze PDC header data struct
            myPDCFile->pdc_header.format[0]='P';
            myPDCFile->pdc_header.format[1]='D';
            myPDCFile->pdc_header.format[2]='C';
            myPDCFile->pdc_header.format[3]=' ';
            myPDCFile->pdc_header.formatVersion = 1;
            myPDCFile->pdc_header.byteOrder = 1;
            myPDCFile->pdc_header.extra1 = 0;
            myPDCFile->pdc_header.extra2 = 0;
            myPDCFile->pdc_header.numParticles = static_cast<int>(gdp->points().entries());
            myPDCFile->pdc_header.numAttributes = numAttrsInGeo + 1;


            // Write the file header
            if(myPDCFile->writeHeader()) 
               throw SOP_PDC_Export_Exception(canNotWritePDCFileHeader, exceptionError);

#ifdef DEBUG
cout << "SOP_PDC_Export::writePDCFile(): Wrote PDC file header" << endl;
#endif

            // Write out other attributes
            for (attrMapFoundIter=attrMapFound.begin(); attrMapFoundIter != attrMapFound.end(); attrMapFoundIter++) {

// cout << "SOP_PDC_Export::writePDCFile(): ATTR: " << ((*attrMapFoundIter).first).c_str() << " " 
//         << static_cast<GB_AttribType>((*attrMapFoundIter).second.attrType) << endl;


#if UT_MAJOR_VERSION_INT >= 12
               attrRef = gdp->findPointAttribute(((*attrMapFoundIter).first).c_str());
               if (attrRef.isValid()) {
#elif UT_MAJOR_VERSION_INT >= 10
               attrRef = gdp->pointAttribs().getOffset(UT_String(((*attrMapFoundIter).first).c_str()), 
                                             static_cast<GB_AttribType>((*attrMapFoundIter).second.attrType));
               if (attrRef.isValid()) {
#else
               int attrOffset = gdp->pointAttribs().getOffset(UT_String(((*attrMapFoundIter).first).c_str()),
                                             static_cast<GB_AttribType>((*attrMapFoundIter).second.attrType));
               if (attrOffset >= 0) {
#endif


// cout << "SOP_PDC_Export::writePDCFile(): Found attribute: " << (*attrMapFoundIter).first << endl;

                  // Set the attribute header
                  myPDCFile->pdc_data.attrNameLength = (*attrMapFoundIter).first.length();
                  myPDCFile->pdc_data.attrName = (*attrMapFoundIter).first;

// cout << "SOP_PDC_Export::writePDCFile(): AttrType: " << (*attrMapFoundIter).second.attrType << endl;
            
                  switch ((*attrMapFoundIter).second.attrType) {

#if UT_MAJOR_VERSION_INT >= 12
                     case GA_STORECLASS_INT: myPDCFile->pdc_data.attrDataType = dca::pdcDataIntArray;
#else
                     case GB_ATTRIB_INT: myPDCFile->pdc_data.attrDataType = dca::pdcDataIntArray;
#endif
                        // Write the data header for the attribute
                        if(myPDCFile->writeDataHeader()) 
                           throw SOP_PDC_Export_Exception(canNotWritePDCDataHeader, exceptionError);

#if UT_MAJOR_VERSION_INT >= 12
                        GA_FOR_ALL_GPOINTS(gdp, ppt) {
#else
                        FOR_ALL_GPOINTS(gdp, ppt) {
#endif
                           if (boss->opInterrupt()) 
                              throw SOP_PDC_Export_Exception(cookInterrupted, exceptionWarning);

                           // Write attribute data to disk
#if UT_MAJOR_VERSION_INT >= 12
                           if(myPDCFile->writeDataRecord(static_cast<int>(ppt->getValue<int>(attrRef, 0))))
                              throw SOP_PDC_Export_Exception(canNotWritePDCFileData, exceptionError);
#else
                           if(myPDCFile->writeDataRecord(static_cast<int>(*ppt->castAttribData<int>(attrOffset))))
                              throw SOP_PDC_Export_Exception(canNotWritePDCFileData, exceptionError);
#endif
                        }

                     break;

#if UT_MAJOR_VERSION_INT >= 12
                     case GA_STORECLASS_FLOAT:
                         if ((*attrMapFoundIter).second.attrSize == 1) {
#else
                     case GB_ATTRIB_FLOAT:
#endif
                        myPDCFile->pdc_data.attrDataType = dca::pdcDataDoubleArray;

                        // Write the data header for the attribute
                        if(myPDCFile->writeDataHeader()) 
                           throw SOP_PDC_Export_Exception(canNotWritePDCDataHeader, exceptionError);

#if UT_MAJOR_VERSION_INT >= 12
                        GA_FOR_ALL_GPOINTS(gdp, ppt) {
#else
                        FOR_ALL_GPOINTS(gdp, ppt) {
#endif
                            if (boss->opInterrupt())
                              throw SOP_PDC_Export_Exception(cookInterrupted, exceptionWarning);

                           // Write attribute data to disk
#if UT_MAJOR_VERSION_INT >= 12
                           if(myPDCFile->writeDataRecord(static_cast<double>(ppt->getValue<float>(attrRef, 0))))
                              throw SOP_PDC_Export_Exception(canNotWritePDCFileData, exceptionError);
#else
                           if(myPDCFile->writeDataRecord(static_cast<double>(*ppt->castAttribData<float>(attrOffset))))
                              throw SOP_PDC_Export_Exception(canNotWritePDCFileData, exceptionError);
#endif
                        }

#if UT_MAJOR_VERSION_INT >= 12
                        }
                        else {
#else
                     break;

                     case GB_ATTRIB_VECTOR:
#endif
                        myPDCFile->pdc_data.attrDataType = dca::pdcDataVectorArray;

                        // Write the data header for the attribute
                        if(myPDCFile->writeDataHeader()) 
                           throw SOP_PDC_Export_Exception(canNotWritePDCDataHeader, exceptionError);

#if UT_MAJOR_VERSION_INT >= 12
                        GA_FOR_ALL_GPOINTS(gdp, ppt) {
#else
                        FOR_ALL_GPOINTS(gdp, ppt) {
#endif
                            if (boss->opInterrupt())
                              throw SOP_PDC_Export_Exception(cookInterrupted, exceptionWarning);


                           struct dca::Maya_PDC_File::point_struct pt;
#if UT_MAJOR_VERSION_INT >= 10
                           UT_Vector3 attrVec = ppt->getValue<UT_Vector3>(attrRef);
//                           UT_Vector3 attrVec = UT_Vector3(0,0,0);
#else
                           UT_Vector3 attrVec = *ppt->castAttribData<UT_Vector3>(attrOffset);
#endif
                           pt.pos[0] = attrVec.x();
                           pt.pos[1] = attrVec.y();
                           pt.pos[2] = attrVec.z();

                           // Write attribute data to disk
                           if(myPDCFile->writeDataRecord(pt))
                              throw SOP_PDC_Export_Exception(canNotWritePDCFileData, exceptionError);
                        }
#if UT_MAJOR_VERSION_INT >= 12
                        }
#endif
                     break;

                     default:
                     break;

                     } // switch()
//cout << "SOP_PDC_Export::writePDCFile(): Wrote attribute: " << (*attrMapFoundIter).first << endl;

                  } // if attr found

               }



            // Set the position attribute header
            myPDCFile->pdc_data.attrNameLength = std::string("position").length();
            myPDCFile->pdc_data.attrName = std::string("position");
            myPDCFile->pdc_data.attrDataType = dca::pdcDataVectorArray;

            // Write the data header for the particle position
            if(myPDCFile->writeDataHeader()) 
               throw SOP_PDC_Export_Exception(canNotWritePDCDataHeader, exceptionError);

#ifdef DEBUG
cout << "SOP_PDC_Export::writePDCFile(): Wrote PDC data header" << endl;
#endif

            struct dca::Maya_PDC_File::point_struct pt;

            // Iterate through all points and write their positions to disk 
#if UT_MAJOR_VERSION_INT >= 12
            GA_FOR_ALL_GPOINTS(gdp, ppt) {
#else
            FOR_ALL_GPOINTS(gdp, ppt) {
#endif

               if (boss->opInterrupt()) 
                  throw SOP_PDC_Export_Exception(cookInterrupted, exceptionWarning);

               pos = ppt->getPos();
               pt.pos[0] = pos.x();
               pt.pos[1] = pos.y();
               pt.pos[2] = pos.z();

               // Write point position data to disk
               if(myPDCFile->writeDataRecord(pt))
                  throw SOP_PDC_Export_Exception(canNotWritePDCFileData, exceptionError);

            }



            // We're done, close the file
            if(myPDCFile->closePDCFile(dca::pdcWriteFile))
               throw SOP_PDC_Export_Exception(canNotCloseThePDCFile, exceptionError);
#ifdef DEBUG
cout << "SOP_PDC_Export::writePDCFile(): Closed PDC file" << endl;
#endif

            // We're done with this frame
            boss->opEnd();
        }// for (cur_frame)

        unlockInputs();

    } 

std::cout << "SOP_PDC_Export::writePDCFile(): Finished" << endl;

} // try



 // Exception handler
 catch (SOP_PDC_Export_Exception e) {
   e.what();
 
   if(e.getSeverity() == exceptionWarning)
      addWarning(SOP_MESSAGE, errorMsgsPDCExport[e.getErrorCode()]);
   else if(e.getSeverity() == exceptionError)
      addError(SOP_MESSAGE, errorMsgsPDCExport[e.getErrorCode()]);

   boss->opEnd();
 	unlockInputs();
   context.setFrame((long)save_frame);

   if(myPDCFile->PDCOutFileStream.is_open()) {
      // Close the RF SD file
      if(myPDCFile->closePDCFile(dca::pdcWriteFile)) {
         addError(SOP_MESSAGE, "Can't close Maya PDC file after SOP_PDC_Export_Exception exception was thrown");
         return error();
      }
   }
   return error();
}


   // Restore the frame 
   context.setFrame((long)save_frame);

   return 0;
}


/**********************************************************************************/
//  $Log: SOP_PDC_Export_writePDCFile.C,v $
//  Revision 1.21  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.20  2012-06-12 18:36:10  mstory
//  Added tooltips and a few extra debug messages.
//
//  Revision 1.19  2011-01-23 04:27:29  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.18  2011-01-23 04:24:05  mstory
//  modified the exporter for H11.
//
//  Revision 1.17  2010-04-21 23:07:18  mstory
//  Finshed modification final changes to version 1.5.0.  Ready for release.
//
//  Revision 1.16  2010-04-21 19:45:37  mstory
//  Finshed modification so useer can select which attribtes are exported.
//
//  Revision 1.15  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.14  2008/06/21 20:14:03  mstory
//  First release.
//
//  Revision 1.13  2008/05/23 20:15:13  mstory
//  Cleaned up for a clean compile.
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
//  Revision 1.3  2008/05/20 23:11:42  mstory
//  Added read data record, SOP Import is creating points.
//  .
//
/**********************************************************************************/

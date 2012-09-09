/* ******************************************************************************
* Maya PDC Particle Houdini Import SOP
*
* Description : This plugin will import Maya PDC Particle Geometry data into Houdini
*
* $Revision: 1.12 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Import.h,v $
* $Author: mstory $
* $Date: 2012-06-13 14:48:30 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Import.h,v 1.12 2012-06-13 14:48:30 mstory Exp $
* $State: Exp $
* $Id: SOP_PDC_Import.h,v 1.12 2012-06-13 14:48:30 mstory Exp $
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


#ifndef __SOP_PDC_Import_h__
#define __SOP_PDC_Import_h__


// File name
#define ARG_PDC_IMPORT_FNAME (myParmBase + 0)

const int NUM_GUI_PARMS = 1;


enum enumExceptionSeverity {
   exceptionNone = 0,
   exceptionWarning,
   exceptionError,
   exceptionCritical
};

// An enum for all the error messages
enum enumErrorList {

   export_success = 0,
   export_fail,
   cookInterrupted,

   endFrameMustBeGreaterThanBeginningFrame,

   canNotOpenPDCFileForReading,
   canNotOpenPDCFileForWriting,
   canNotReadPDCFileHeader,
   canNotReadPDCFileDataHeader,
   canNotReadPDCFileDataRecord,
   invalidPDCDataType,
   ghostFramesAttrEncountered,
   canNotLockInputsInWritePDCFile,
   canNotCloseThePDCFile,
   canNotCreateDetailIntAttribute,
   canNotCreateDetailDoubleAttribute,
   canNotCreateDetailVectorAttribute,
   canNotCreatePointIntAttribute,
   canNotCreatePointDoubleAttribute,
   canNotCreatePointVectorAttribute,
   canNotCreatePointIntArrayAttribute,
   canNotCreatePointDoubleArrayAttribute,
   canNotCreatePointVectorArrayAttribute,

   NUM_ERRORS
};


UT_String errorMsgsPDCImport[NUM_ERRORS];



/* ******************************************************************************
*  Class Name : OP_PDC_Import_Operator()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
class OP_PDC_Import_Operator : public OP_Operator {
public:
   OP_PDC_Import_Operator();
   virtual ~OP_PDC_Import_Operator();
   virtual bool getHDKHelp(UT_String &str) const;

};



/* ******************************************************************************
*  Class Name : SOP_PDC_Import_Exception()
*
*  Description :  Exception class for Maya PDC Import SOP
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
class SOP_PDC_Import_Exception {
   std::string e_msg;
   enumErrorList e_code;
   enumExceptionSeverity severity;

public:
   SOP_PDC_Import_Exception(enumErrorList code, enumExceptionSeverity severity);
//   ~SOP_PDC_Import_Exception();

   void what() {
      std::cout << "SOP_PDC_Import_Exception::what() - Maya PDC Import exception:  " << e_msg << endl;
   }
   enumErrorList getErrorCode() {
      return e_code;
   }
   enumExceptionSeverity getSeverity() {
      return severity;
   }
};



/* ******************************************************************************
*  Class Name : SOP_PDC_Import()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
class SOP_PDC_Import : public SOP_Node {
public:
   static OP_Node      *myConstructor(OP_Network*, const char *, OP_Operator *);

   static PRM_Template      myTemplateList[];
   static CH_LocalVariable  myVariables[];

protected:
   SOP_PDC_Import(OP_Network *net, const char *name, OP_Operator *op);
   virtual ~SOP_PDC_Import();

   virtual unsigned    disableParms();
   virtual OP_ERROR    cookMySop(OP_Context &context);
   virtual float       getVariableValue(int index, int thread);

private:

// File name
   void    FNAME(UT_String &label, float t) {
      evalString(label, ARG_PDC_IMPORT_FNAME, 0, t);
   }


   OP_ERROR ReadPDCFile(OP_Context &context);

   short   myParmBase; // parameter offsets

   dca::Maya_PDC_File  *myPDCFile;

   UT_String  myFileName;
   UT_String  mySOPVersion;

   int     myFileType;
   int     myCurrPoint;
   int     myTotalPoints;
};

#endif


/**********************************************************************************/
//  $Log: SOP_PDC_Import.h,v $
//  Revision 1.12  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.11  2011-01-23 04:27:29  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.10  2011-01-23 04:24:05  mstory
//  modified the exporter for H11.
//
//  Revision 1.9  2011-01-23 02:11:22  mstory
//  Modified importer for H11.
//
//  Revision 1.8  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.7  2008/06/21 20:14:03  mstory
//  First release.
//
//  Revision 1.6  2008/05/21 16:15:30  mstory
//  .
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

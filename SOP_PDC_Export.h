/* ******************************************************************************
*  Maya PDC Particle Geometry Export SOP Houdini Extension
*
* $Revision: 1.15 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Export.h,v $
* $Author: mstory $
* $Date: 2012-06-13 14:48:30 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Export.h,v 1.15 2012-06-13 14:48:30 mstory Exp $
* $State: Exp $
* $Id: SOP_PDC_Export.h,v 1.15 2012-06-13 14:48:30 mstory Exp $
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

#ifndef __SOP_PDC_Export_h__
#define __SOP_PDC_Export_h__

#include <UT/UT_Version.h>

static char const rcsid[] =
   "$Id: SOP_PDC_Export.h,v 1.15 2012-06-13 14:48:30 mstory Exp $";

#define ARG_PDC_EXPORT_FNAME (myParmBase + 0)
#define ARG_PDC_EXPORT_BEGIN_END (myParmBase + 1)
#define ARG_PDC_EXPORT_ATTR (myParmBase + 2)

const uint SIZEOF_FLOAT = 4;
const uint SIZEOF_INT = 4;
const uint NUM_GUI_PARMS = 3;

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
   canNotOpenPDCFileForWriting,
   canNotWritePDCFileHeader,
   canNotLockInputsInWritePDCFile,
   canNotWritePDCDataHeader,
   canNotWritePDCFileData,
   canNotCloseThePDCFile,

   NUM_ERRORS
};


UT_String errorMsgsPDCExport[NUM_ERRORS];


/* ******************************************************************************
*  Class Name : OP_PDC_Export_Operator()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
class OP_PDC_Export_Operator : public OP_Operator {
public:
   OP_PDC_Export_Operator();
   virtual ~OP_PDC_Export_Operator();
   virtual bool getHDKHelp(UT_String &str) const;

};



/* ******************************************************************************
*  Class Name : SOP_PDC_Export_Exception()
*
*  Description :  Exception class for Maya PDC Export SOP
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
class SOP_PDC_Export_Exception {
   std::string e_msg;
   enumErrorList e_code;
   enumExceptionSeverity severity;

public:
   SOP_PDC_Export_Exception(enumErrorList code, enumExceptionSeverity severity);
//   ~SOP_PDC_Export_Exception();

   void what() {
      std::cout << "SOP_PDC_Export_Exception::what() - Maya PDC Export exception:  " << e_msg << endl;
   }
   enumErrorList getErrorCode() {
      return e_code;
   }
   enumExceptionSeverity getSeverity() {
      return severity;
   }
};



/* ******************************************************************************
*  Class Name : SOP_PDC_Export()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
class SOP_PDC_Export : public SOP_Node {
public:
   SOP_PDC_Export(OP_Network *net, const char *name, OP_Operator *op);

   virtual ~SOP_PDC_Export();

   static PRM_Template    myTemplateList[];
   static OP_Node         *myConstructor(OP_Network*, const char *, OP_Operator *);

   struct attr_found_struct {
      UT_String attrName;
#if UT_MAJOR_VERSION_INT >= 12
      GA_StorageClass attrType;
#else
      GB_AttribType attrType;
#endif
      int   attrSize;
   };

protected:
   virtual unsigned      disableParms();
   virtual const char    *inputLabel(unsigned idx) const;

   virtual OP_ERROR cookMySop(OP_Context &context);

private:

   // Functions for GUI widgets
   void    FNAME(UT_String &label, float t) {
      evalString(label, ARG_PDC_EXPORT_FNAME, 0, t);
   }
   void    BEGIN_END (float *val, float t) {
      evalFloats(ARG_PDC_EXPORT_BEGIN_END, val, t);
   }
   void    ATTR(UT_String &label, float t) {
      evalString(label, ARG_PDC_EXPORT_ATTR, 0, t);
   }

   // callback function
   static int writeTheFile(void *data, int index, float time, const PRM_Template *tplate );

   //
   int writePDCFile(OP_Context &context);

   bool                 calledFromCallback;
   OP_ERROR             myCallBackError;
   enum enumErrorList   myCallBackFlags;

   UT_String      myFileName;
   float          myBeginEnd[2];
   UT_String      myAttrs;

   bool myEchoData;
   dca::Maya_PDC_File *myPDCFile;

   UT_String mySOPVersion;

   short myParmBase;
};

#endif

/**********************************************************************************/
//  $Log: SOP_PDC_Export.h,v $
//  Revision 1.15  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.14  2011-01-23 04:27:29  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.13  2011-01-23 04:24:05  mstory
//  modified the exporter for H11.
//
//  Revision 1.12  2010-04-21 19:45:37  mstory
//  Finshed modification so useer can select which attribtes are exported.
//
//  Revision 1.11  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.10  2008/05/23 07:26:33  mstory
//  Added most of the rest of the attribute exporting.  Needs to lookup entire attr dictionary, currently only
//  exporting "stamdard" attributes.
//
//  Revision 1.9  2008/05/22 00:53:17  mstory
//  Added most of the attribute creation code for the importer and attribute detection for the exporter.
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

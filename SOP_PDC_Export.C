/* ******************************************************************************
*  Maya PDC Geometry Export SOP Houdini Extension
*
*
* $Revision: 1.17 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Export.C,v $
* $Author: mstory $
* $Date: 2012-09-03 16:56:04 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Export.C,v 1.17 2012-09-03 16:56:04 mstory Exp $
* $State: Exp $
* $Id: SOP_PDC_Export.C,v 1.17 2012-09-03 16:56:04 mstory Exp $
* $Locker:  $
*
*  Version 1.5.3
*  Date: Jun 22, 2012
*  Author: Mark Story
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

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <map>


#include <UT/UT_DSOVersion.h>
#include <UT/UT_EnvControl.h>
#include <UT/UT_Math.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <UT/UT_DMatrix4.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <PRM/PRM_Include.h>
#include <GEO/GEO_Point.h>
#include <OP/OP_Operator.h>
#include <OP/OP_Director.h>
#include <OP/OP_OperatorTable.h>
#include <SOP/SOP_Guide.h>
#include <SOP/SOP_Node.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_WorkArgs.h>
#include <UT/UT_Version.h>

#if UT_MAJOR_VERSION_INT >= 12
#include <GA/GA_AttributeRef.h>
#else
#include <GB/GB_AttributeDict.h>
#if UT_MAJOR_VERSION_INT >= 10
#include <GB/GB_AttributeRef.h>
#endif
#endif


// #define DEBUG

#include "Maya_PDC_File.h"
#include "Maya_PDC_File.C"
#include "SOP_PDC_Export.h"
#include "SOP_PDC_Export_writePDCFile.C"



/* ******************************************************************************
*  Function Name : SOP_PDC_Export_Exception()
*
*  Description : Constructor for an "Maya PDC Export Operator Exception" object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_PDC_Export_Exception::SOP_PDC_Export_Exception(enumErrorList code, enumExceptionSeverity sev)
{

//   cout << "SOP_PDC_Export_Exception: in constructor ... " << endl;

   this->e_msg = errorMsgsPDCExport[code].toStdString();
   this->e_code = code;
   this->severity = sev;

};




//SOP_PDC_Export_Exception::~SOP_PDC_Export_Exception() {

//   cout << "SOP_PDC_Export_Exception: in destructor ... " << endl;

//   };




/* ******************************************************************************
*  Function Name : OP_PDC_Export_Operator()
*
*  Description : Constructor for an "Maya PDC Export Operator"  object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_PDC_Export_Operator::OP_PDC_Export_Operator()
      : OP_Operator("PDC_export",
                    "Maya PDC Export",
                    SOP_PDC_Export::myConstructor,
                    SOP_PDC_Export::myTemplateList,
                    1,
                    1,
                    0)
{
}



/* ******************************************************************************
*  Function Name : OP_PDC_Export_Operator()
*
*  Description :  Destructor for a OP_PDC_Export_Operator object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_PDC_Export_Operator::~OP_PDC_Export_Operator()
{
}

/* ******************************************************************************
*  Function Name : OP_PDC_Export_Operator::getHDKHelp()
*
*  Description : Get HDK help.
*
*  Input Arguments : UT_String &help
*
*  Return Value :
*
***************************************************************************** */

bool OP_PDC_Export_Operator::getHDKHelp(UT_String &help) const
{
   help  = "<html><body>";

   help  = "<h3>Maya PDC Geometry Export SOP</h3>";

   help += "<br>";


   help += "</body></html>";

   // Note: HDK developers are discouraged from embedding help
   // in their C files and should use external help when possible.

   // The getHDKHelp() must return true if it is to be used in place of
   // other forms of help (eg. custom, OTL, or Houdini help).
   return true;
}



/* ******************************************************************************
*  Function Name :  newSopOperator()
*
*  Description :
*
*  Input Arguments : OP_OperatorTable *table
*
*  Return Value :
*
***************************************************************************** */
void newSopOperator(OP_OperatorTable *table)
{
   table->addOperator(new OP_PDC_Export_Operator());
}



/* ******************************************************************************
*  Function Name : Data Area
*
*  Description :
*
***************************************************************************** */

#define NPARMS_FOLDER_1 3

static PRM_Default  switcherDef[] = {
   PRM_Default(NPARMS_FOLDER_1,    "Setup"),
};


static PRM_Name        		names[] = {
   // Setup parameters
   PRM_Name("file_name",	"File Name"),
   PRM_Name("start_end",	"Start/End"),
   PRM_Name("attrs",	         "Attributes"),
   PRM_Name("write_file",  "Write PDC File"),

   PRM_Name(0)
};

// Defaults
static PRM_Default nameDefault_filename(0,"untitled");
static PRM_Default nameDefault_attr(0,"v");
static PRM_Default dataDefault0(0.0);
static PRM_Default dataDefault1(1.0);
static PRM_Default dataDefault0_1(0.1);
static PRM_Default dataDefault100(100.0);
static PRM_Default dataDefault_100(-100.0);
static PRM_Default dataDefault1000(1000.0);
static PRM_Default dataDefault_1000(-1000.0);
static PRM_Default versionDefault(1);

// Ranges
static PRM_Range  animRange(PRM_RANGE_UI, 1, PRM_RANGE_UI, 240);
static PRM_Range  dataRange1000(PRM_RANGE_UI, -1000, PRM_RANGE_UI, 1000);
static PRM_Range  dataRange100(PRM_RANGE_UI, -100, PRM_RANGE_UI, 100);


// Build the GUI template
PRM_Template SOP_PDC_Export::myTemplateList[] = {

   // Filename of the file to be exported
   PRM_Template(PRM_FILE, 1, &names[0], &nameDefault_filename, 0, 0, 0, 0, 1, "The Filename of the PDC file to import"),
   // Animation Range
   PRM_Template(PRM_BEGINEND_J,2, &names[1], PRMoneDefaults, 0, &animRange),
   // Attributes to export
   PRM_Template(PRM_STRING, 1, &names[2]),

   // Write the file button
   PRM_Template(PRM_CALLBACK, 1, &names[3], 0, 0, 0, SOP_PDC_Export::writeTheFile),

   PRM_Template()
};



/* ******************************************************************************
*  Function Name :  myConstructor()
*
*  Description : "My Constructor"
*
*  Input Arguments : OP_Network *net, const char *name, OP_Operator *op
*
*  Return Value : OP_Node *
*
***************************************************************************** */
OP_Node * SOP_PDC_Export::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
   return new SOP_PDC_Export(net, name, op);
}


/* ******************************************************************************
*  Function Name :  Constructor()
*
*  Description : The Constructor
*
*  Input Arguments : OP_Network *net, const char *name, OP_Operator *op
*
*  Return Value :
*
***************************************************************************** */
SOP_PDC_Export::SOP_PDC_Export(OP_Network *net, const char *name, OP_Operator *op)
      : SOP_Node(net, name, op)
{
   // Initialize this object
   myParmBase = getParmList()->getParmIndex( names[0].getToken() );
   calledFromCallback = false;

   myCallBackFlags = (enumErrorList)0;

#ifdef SOP_MAJOR_VER
   mySOPVersion = SOP_MAJOR_VER +  "." + SOP_MINOR_VER;
//   std::cout << "Version: " << SOP_MAJOR_VER << "." << SOP_MINOR_VER << std::endl;
#endif

   errorMsgsPDCExport[endFrameMustBeGreaterThanBeginningFrame] = "End frame must be greater than beginning frame!";
   errorMsgsPDCExport[canNotOpenPDCFileForWriting] = "Can't open Maya PDC file for writing";

   errorMsgsPDCExport[canNotWritePDCFileHeader] = "Can not write the PDC file header";
   errorMsgsPDCExport[canNotLockInputsInWritePDCFile] = "Can not lock inputs when trying to write PDC file";
   errorMsgsPDCExport[canNotWritePDCDataHeader] = "Can not write the PDC data header to file";
   errorMsgsPDCExport[canNotWritePDCFileData] = "Can not write the PDC data to file";
   errorMsgsPDCExport[canNotCloseThePDCFile] = "Can not close PDC file";

   myPDCFile = new dca::Maya_PDC_File(0,0);

   disableParms();

//    mySopFlags.setNeedGuide1(1);
}


/* ******************************************************************************
*  Function Name :  ~SOP_PDC_Export()
*
*  Description :
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
// Destructor
SOP_PDC_Export::~SOP_PDC_Export()
{
   delete(myPDCFile);
}




/* ******************************************************************************
*  Function Name :  writeTheFile()
*
*  Description : Handle the "Write PDC file" button event
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_PDC_Export::writeTheFile(void *data, int index,
                                 float time, const PRM_Template *tplate )
{

#ifdef DEBUG
   cout << "writeTheFile() - Writing the Maya PDC File" << endl;
#endif

   // Set the callback flag true and reset the error flag as well
   SOP_PDC_Export *me = (SOP_PDC_Export *) data;
   me->calledFromCallback = true;
   me->myCallBackFlags = (enumErrorList)0;

   OP_Context myContext(time);

#if UT_MAJOR_VERSION_INT >= 10
   myContext.setData(static_cast<OP_ContextData*>(data));
#else
   myContext.setData(data, OP_GEOMETRY_DATA);
#endif
   me->myCallBackError = me->cookMe(myContext);

   return 1;
}



/* ******************************************************************************
*  Function Name :  disableParms()
*
*  Description : Disable or enable GUI parms depending what file the user is creating
*
*  Input Arguments : None
*
*  Return Value : unsigned num_of_parms_changed
*
***************************************************************************** */
unsigned SOP_PDC_Export::disableParms()
{
   unsigned changed = 0;

   // First turn them all on
   for (int i=0; i <= NUM_GUI_PARMS; i++)
      enableParm(i, 1);

#ifdef DEBUG
   cout << "disableParms()-changed: " << changed << endl;
#endif

   return changed;
}



/* ******************************************************************************
*  Function Name :  cookMySop()
*
*  Description :
*
*  Input Arguments : OP_Context &context
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR SOP_PDC_Export::cookMySop(OP_Context &context)
{
   UT_Vector3 trans, rot, scale, up, shear;
   float *myBeginEnd_ptr = myBeginEnd;
#if UT_MAJOR_VERSION_INT >= 12
   float now = context.getTime();
#else
   float now = context.myTime;
#endif
   disableParms();

   BEGIN_END(myBeginEnd_ptr, now);
   ATTR(myAttrs, now);


#ifdef DEBUG
   std::cout << "SOP_PDC_Export::cookMySop()" << std::endl;
   std::cout << "SOP_PDC_Export::cookMySop() - myAttrs: " << myAttrs << std::endl;
#endif

   writePDCFile(context);

   return error();
}



/* ******************************************************************************
*  Function Name :  inputLabel()
*
*  Description :  Provides for a readable label when the user middle clicks on the input connector
*
*  Input Arguments : unsigned
*
*  Return Value : const char *
*
***************************************************************************** */
const char * SOP_PDC_Export::inputLabel(unsigned) const
{
   return "Geometry to Export to Maya";
}



/**********************************************************************************/
//  $Log: SOP_PDC_Export.C,v $
//  Revision 1.17  2012-09-03 16:56:04  mstory
//  .
//
//  Revision 1.16  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.15  2012-06-12 18:36:10  mstory
//  Added tooltips and a few extra debug messages.
//
//  Revision 1.13  2011-01-23 04:27:28  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.12  2011-01-23 04:24:05  mstory
//  modified the exporter for H11.
//
//  Revision 1.11  2011-01-23 02:11:22  mstory
//  Modified importer for H11.
//
//  Revision 1.10  2010-04-21 19:45:37  mstory
//  Finshed modification so useer can select which attribtes are exported.
//
//  Revision 1.9  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.8  2008/05/22 01:09:35  mstory
//  *** empty log message ***
//
//  Revision 1.7  2008/05/21 22:47:51  mstory
//  Writing point data (vector arrays)
//
//  Revision 1.6  2008/05/21 16:15:30  mstory
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

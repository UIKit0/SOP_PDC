/* ******************************************************************************
*  Maya PDC Particle Geometry Import SOP Houdini Extension
*
* $Revision: 1.17 $
* $Source: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Import.C,v $
* $Author: mstory $
* $Date: 2012-09-03 16:56:04 $
* $Header: /dca/cvsroot/houdini/SOP_PDC/SOP_PDC_Import.C,v 1.17 2012-09-03 16:56:04 mstory Exp $
* $State: Exp $
* $Id: SOP_PDC_Import.C,v 1.17 2012-09-03 16:56:04 mstory Exp $
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
#include <UT/UT_String.h>
#include <UT/UT_Endian.h>
#include <CH/CH_LocalVariable.h>

#include <GA/GA_AttributeRef.h>

// #define DEBUG

#include "Maya_PDC_File.h"
#include "Maya_PDC_File.C"
#include "SOP_PDC_Import.h"
#include "SOP_PDC_Import_readPDCFile.C"


/* ******************************************************************************
*  Function Name : SOP_PDC_Import_Exception()
*
*  Description : Constructor for an "Maya PDC Import Operator Exception" object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_PDC_Import_Exception::SOP_PDC_Import_Exception(enumErrorList code, enumExceptionSeverity sev)
{

//   cout << "SOP_PDC_Import_Exception: in constructor ... " << endl;

   this->e_msg = errorMsgsPDCImport[code].toStdString();
   this->e_code = code;
   this->severity = sev;

};




//SOP_PDC_Import_Exception::~SOP_PDC_Import_Exception() {

//   cout << "SOP_PDC_Import_Exception: in destructor ... " << endl;

//   };


/* ******************************************************************************
*  Function Name : OP_PDC_Import_Operator()
*
*  Description : Constructor for an "Maya PDC Particle Import Operator" object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_PDC_Import_Operator::OP_PDC_Import_Operator()
      : OP_Operator("PDC_import",
                    "Maya PDC Import",
                    SOP_PDC_Import::myConstructor,
                    SOP_PDC_Import::myTemplateList,
                    0,
                    0,
                    SOP_PDC_Import::myVariables,
                    OP_FLAG_GENERATOR)
{
}



/* ******************************************************************************
*  Function Name : ~OP_PDC_Import_Operator()
*
*  Description : Destructor for an "Maya PDC Particle Import Operator"  object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_PDC_Import_Operator::~OP_PDC_Import_Operator()
{
}

/* ******************************************************************************
*  Function Name : OP_PDC_Import_Operator::getHDKHelp()
*
*  Description : Get HDK help.
*
*  Input Arguments : UT_String &help
*
*  Return Value :
*
***************************************************************************** */

bool OP_PDC_Import_Operator::getHDKHelp(UT_String &help) const
{
   help  = "<html><body>";

   help  = "<h3>Maya PDC Particle Geometry</h3>";
   help += "Maya PDC Particle Geometry will ";


   help += "<br>";
   help += "<b>Local Variables:</b><br>";
   help += "   N/A<br>";
   help += "<br>";
   help += "<b>Version:</b><br>";
   help += "1.0.0<br>";
   help += "<br>";
   help += "Digital Cinema Arts: Houdini/Maya PDC Particle Geometry web page:<br>";
   help += "<a href=\"http://www.digitalcinemaarts.com/dev/pdc/\">www.digitalcinemaarts.com/dev/pdc/</a><br>";
   help += "<br>";

   help += "</body></html>";

   // Note: HDK developers are discouraged from embedding help
   // in their C files and should use external help when possible.

   // The getHDKHelp() must return true if it is to be used in place of
   // other forms of help (eg. custom, OTL, or Houdini help).
   return true;
}



/* ******************************************************************************
*  Function Name : newSopOperator()
*
*  Description :
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
void newSopOperator(OP_OperatorTable *table)
{
   table->addOperator(new OP_PDC_Import_Operator());
}


/* ******************************************************************************
*  Data Area
*
*  Description : Various structures for the SOP, primarily for the interface
*
***************************************************************************** */
#define NPARMS_FOLDER_1 1

static PRM_Default  switcherDef[] = {
   PRM_Default(NPARMS_FOLDER_1,    "Setup"),
};

static PRM_Name     names[] = {
   PRM_Name("fname",   "File Name"),

   PRM_Name(0)
};


// Defaults
static PRM_Default nameDefault1(0,"untitled.pdc");


// Built the parameter template
PRM_Template SOP_PDC_Import::myTemplateList[] = {

   // File name
   PRM_Template(PRM_FILE, 1, &names[0], &nameDefault1,0),


   PRM_Template()
};


// Local var enum
enum {
   VAR_PT,
   VAR_NPT
};

// Local variable array
CH_LocalVariable SOP_PDC_Import::myVariables[] = {
   { "PT",     VAR_PT, 0 },
   { "NPT",    VAR_NPT, 0 },
   { 0, 0, 0 },
};



/* ******************************************************************************
*  Function Name : getVariableValue()
*
*  Description : Get out local variables
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
float SOP_PDC_Import::getVariableValue(int index, int)
{
   if (myCurrPoint < 0) return 0;

   switch (index) {
   case VAR_PT:
      return myCurrPoint;
   case VAR_NPT:
      return myTotalPoints;
   }

   return 0;
}



/* ******************************************************************************
*  Function Name : myConstructor()
*
*  Description : myConstructor for an "Maya PDC Import Operator"  object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */

OP_Node * SOP_PDC_Import::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
   return new SOP_PDC_Import(net, name, op);
}



/* ******************************************************************************
*  Function Name : SOP_PDC_Import()
*
*  Description : Constructor for a "Maya PDC" import SOP object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_PDC_Import::SOP_PDC_Import(OP_Network *net, const char *name, OP_Operator *op)
      : SOP_Node(net, name, op)
{
   OP_Node::flags().timeDep = 1;

   myCurrPoint = -1;			// To prevent garbage values from being returned
   myTotalPoints = 0;      // Set the NPT local variable value
   myParmBase = getParmList()->getParmIndex( names[0].getToken() );

   errorMsgsPDCImport[cookInterrupted] = "Cooking interrupted";
   errorMsgsPDCImport[endFrameMustBeGreaterThanBeginningFrame] = "End frame must be less the beginning frame";

   errorMsgsPDCImport[endFrameMustBeGreaterThanBeginningFrame] = "End frame must be greater than beginning frame!";
   errorMsgsPDCImport[canNotReadPDCFileDataHeader] = "Can't read data header";
   errorMsgsPDCImport[canNotReadPDCFileDataRecord] = "Can't read data record";

   errorMsgsPDCImport[canNotOpenPDCFileForReading] = "Can't open Maya PDC file for reading";
   errorMsgsPDCImport[canNotOpenPDCFileForWriting] = "Can't open Maya PDC file for writing";
   errorMsgsPDCImport[invalidPDCDataType] = "Invalid PDC data type while reading PDC file";
   errorMsgsPDCImport[ghostFramesAttrEncountered] = "Invalid PDC data type encounted: \"ghostFramesAttr\" while reading PDC file";

   errorMsgsPDCImport[canNotReadPDCFileHeader] = "Can't read Maya PDC file header";
   errorMsgsPDCImport[canNotLockInputsInWritePDCFile] = "Can't lock inputs";
   errorMsgsPDCImport[canNotCloseThePDCFile] = "Can't close the Maya PDC file";
   errorMsgsPDCImport[canNotCreatePointIntAttribute] = "Can't create point int attribute";
   errorMsgsPDCImport[canNotCreatePointDoubleAttribute] = "Can't create point double attribute";
   errorMsgsPDCImport[canNotCreatePointVectorAttribute] = "Can't create point vector attribute";
   errorMsgsPDCImport[canNotCreatePointIntArrayAttribute] = "Can't create point int array attribute";
   errorMsgsPDCImport[canNotCreatePointDoubleArrayAttribute] = "Can't create point double array attribute";
   errorMsgsPDCImport[canNotCreatePointVectorArrayAttribute] = "Can't create point vector arrat attribute";
   errorMsgsPDCImport[canNotCreateDetailIntAttribute] = "Can't create detail int attribute";
   errorMsgsPDCImport[canNotCreateDetailDoubleAttribute] = "Can't create detail double attribute";
   errorMsgsPDCImport[canNotCreateDetailVectorAttribute] = "Can't create detail vector attribute";


#ifdef SOP_MAJOR_VER
   mySOPVersion = SOP_MAJOR_VER +  "." + SOP_MINOR_VER;
//   std::cout << "Version: " << SOP_MAJOR_VER << "." << SOP_MINOR_VER << std::endl;
#endif


   myPDCFile = new dca::Maya_PDC_File(0,0);


   disableParms();
}



/* ******************************************************************************
*  Function Name : ~SOP_PDC_Import()
*
*  Description : Destructor for a "Maya PDC Particle Geometry" import SOP object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_PDC_Import::~SOP_PDC_Import()
{

   delete(myPDCFile);

}



/* ******************************************************************************
*  Function Name : disableParms()
*
*  Description : Disable (and re-enable) parameters after the use makes a selection
*
*  Input Arguments : None
*
*  Return Value : unsigned num_of_parms_changed
*
***************************************************************************** */
unsigned SOP_PDC_Import::disableParms()
{
   unsigned changed = 0;


   // First turn them all on
   for (int i=0; i <= NUM_GUI_PARMS; i++)
      enableParm(i, 1);

#ifdef DEBUG
   cout << "disableParms() - changed: " << changed << endl;
#endif

   return changed;
}



/* ******************************************************************************
*  Function Name : cookMySop()
*
*  Description : Cook this SOP node
*
*  Input Arguments : None
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR SOP_PDC_Import::cookMySop(OP_Context &context)
{
   float		 	now;
   char GUI_str[4];

   now = context.getTime();
   OP_Node::flags().timeDep = 1;
   disableParms();

   myTotalPoints = 0;			// Set the NPT local variable value
   myCurrPoint   = 0;			// Initialize the PT local variable

   FNAME(myFileName, now);

   disableParms();
   sprintf(GUI_str, "%s", "");
   // setString((UT_String)GUI_str, ARG_PDC_IMPORT_VER, 0, now);
//   setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_PDC_IMPORT_VER, 0, now);


   ReadPDCFile(context);

   myCurrPoint = -1;
   return error();
}


/**********************************************************************************/
//  $Log: SOP_PDC_Import.C,v $
//  Revision 1.17  2012-09-03 16:56:04  mstory
//  .
//
//  Revision 1.16  2012-06-13 23:17:02  mstory
//  .
//
//  Revision 1.15  2012-06-13 14:48:30  mstory
//  Added H12 mods from Frederic Servant.
//
//  Revision 1.14  2011-01-23 04:27:29  mstory
//  updated version and date of 1.51. release.
//
//  Revision 1.13  2011-01-23 04:24:05  mstory
//  modified the exporter for H11.
//
//  Revision 1.12  2011-01-23 02:11:22  mstory
//  Modified importer for H11.
//
//  Revision 1.11  2010-04-20 21:14:43  mstory
//  MOst changes finished for the exporter to handle user provided attrs to write into PDC file.
//
//  --mstory
//
//  Revision 1.10  2008/06/21 20:14:03  mstory
//  First release.
//
//  Revision 1.9  2008/05/23 07:26:33  mstory
//  Added most of the rest of the attribute exporting.  Needs to lookup entire attr dictionary, currently only
//  exporting "stamdard" attributes.
//
//  Revision 1.8  2008/05/22 00:53:17  mstory
//  Added most of the attribute creation code for the importer and attribute detection for the exporter.
//
//  Revision 1.7  2008/05/21 22:47:51  mstory
//  Writing point data (vector arrays)
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

/* ******************************************************************************
*  Endian functions
*
* $RCSfile: endian.h,v $
*
*  Description : Various functions to handle endianism.
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_PDC/endian.h,v $
*
* $Author: mstory $
*
* See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2006-2012
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */


/*

Def's and functions for endianism

*/

// 8 bit unsigned char
typedef unsigned char  byte;

// 16 bit unsigned integer
typedef unsigned short uint16;

// 32 bit unsigned integer
typedef unsigned int   uint32;

// 16 bit signed integer
typedef signed short   int16;

// 32 bit signed integer
typedef signed int     int32;

#ifndef FAST_FLOAT
// real can be float or double
typedef double         real;
#else
// real can be float or double
typedef float          real;
#endif

#ifndef NULL
// NULL pointer
#define NULL (0)
#endif


uint32 convertu32(const uint32 *data)
{
   uint32 val;

   const byte *b = (const byte *) data;
   val =  b[3]<<24 | b[2]<<16 | b[1]<<8 | b[0];
   return val;
}


uint16 convertu16(const uint16 *data)
{
   uint16 val;

   const byte *b = (const byte *) data;
   val =  b[1]<<8 | b[0];
   return val;
}

int16 convert16(const int16 *data)
{
   int16  tmp;
   char *src, *dst;

   src = (char*)data;
   dst = (char*)&tmp;

   dst[0] = src[1];
   dst[1] = src[0];

   return tmp;
}


int32 convert32(const int32 *data)
{
   int32  tmp;
   char *src, *dst;

   src = (char*)data;
   dst = (char*)&tmp;

   dst[0] = src[3];
   dst[1] = src[2];
   dst[2] = src[1];
   dst[3] = src[0];

   return tmp;
}



static void swapfloat(float *f)
{
   float tmp;
   char *src, *dst;

   src = (char*)f;
   dst = (char*)&tmp;
   dst[0] = src[3];
   dst[1] = src[2];
   dst[2] = src[1];
   dst[3] = src[0];
   *f = tmp;
}


static void swapdouble(double *d)
{
   double tmp;
   char *src, *dst;

   src = (char*)d;
   dst = (char*)&tmp;
   dst[0] = src[7];
   dst[1] = src[6];
   dst[2] = src[5];
   dst[3] = src[4];
   dst[4] = src[3];
   dst[5] = src[2];
   dst[6] = src[1];
   dst[7] = src[0];
   *d = tmp;
}



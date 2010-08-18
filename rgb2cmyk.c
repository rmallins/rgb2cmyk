/******************************************************************************
 *    rgb2cmyk - a command line utility to convert raw RGB bitmaps to CMYK
 *    Copyright (C) 2006, 2010  Robin Mallinson 
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * DESCRIPTION : This utility takes a raw binary rgb image file as input, and 
 *               outputs a CMYK equivalent image to stdout.
 *               It uses the algorithm documented here:
 *
 *                  http://en.wikipedia.org/w/index.php?title=CMYK_color_model&oldid=147065274#Conversions
 *
 *               Use it as follows:
 *
 *                  > rgb2cmyk rgbfile.raw > cmykfile.raw
 *
 *               R.Mallinson 24/11/06
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
typedef unsigned char       uint8_t;      /* unsigned 8 BIT value  */
typedef unsigned int        uint32_t;     /* unsigned 32 BIT value */
#define RGB_SIZE  (3)
#define CMYK_SIZE (4)

static
int minimum(int a, int b, int c)
{
   int min = a;
   if ( b < min ) min = b;
   if ( c < min ) min = c;
   return (min);
}

// takes a 3 byte RGB array, and converts to CMYK colourspace,
// printing binary data to stdout.
#define CMPMAX    (0xFF)   // component max value = 255
static
void RgbToCmyk( uint8_t* rgb_store )
{
   int R = (int)rgb_store[0];
   int G = (int)rgb_store[1];
   int B = (int)rgb_store[2];
   
   int iC,iM,iY,iK, C,M,Y,K;
   
   iC = CMPMAX-R; iM = CMPMAX-G; iY = CMPMAX-B;
   K = minimum(iC,iM,iY);   
   
   if (K != CMPMAX)
   {
      iK = CMPMAX-K;      // or minimum(R,G,B)
   
      C = ( CMPMAX * (iC-K) ) / iK;
      M = ( CMPMAX * (iM-K) ) / iK;
      Y = ( CMPMAX * (iY-K) ) / iK;
   }
   else // would have divided by 0
   {
      C = M = Y = 0;
   }
   
   (void)putchar(C);
   (void)putchar(M);
   (void)putchar(Y);
   (void)putchar(K);
}

// Store an R or G or B value into array.  Once the array is full
// convert to CMYK and display the data to stdout.
static
void rgb_store(int c)
{   
   static uint8_t rgb_store[RGB_SIZE];
   static int idx = 0;

   rgb_store[idx++] = (uint8_t)c;
   if (idx == RGB_SIZE)
   {
      RgbToCmyk(rgb_store);         //convert to CMYK and print
      idx = 0;                      // prepare for next RGB
   }
}

static
void ReadFile (char* name)
{
   FILE* inFile;
   int c = 0;

   if (name == 0)
   {  // if no filename provided, use stdin for data
      inFile = stdin;
   }
   else
   {  // use filename for data
      inFile = fopen (name, "rb");     // open for binary read
   }

   if (inFile != NULL)
   {
      c = fgetc(inFile);
      while ( c != EOF)
      {
         rgb_store(c);
         c = fgetc(inFile);
      }
      (void)fclose (inFile);
   }
   else
   {
      printf("fileopen failed!\n");
   }
}

int main (int argc, char* argv[])
{
   if (argc == 2)
   {
      ReadFile(argv[1]);
   }
   else
   {
     ReadFile(0);       // use stdout
   }
   return 0;
}

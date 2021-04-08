#ifndef OPENTEXTURES_H
#define OPENTEXTURES_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <stdio.h>
#include <setjmp.h>

//#include "jpeglib.h"

//#include <windows.h>

class OpenTextures
{
   public:

      OpenTextures(){};
      ~OpenTextures(){};

      static void loadTextureRAW( const char * filename, int wrap );
      static void loadTextureBMP( const char * szFileName, int wrap );

      static void read_JPEG_file ( char * filename, int wrap );

} typedef OT;

#endif


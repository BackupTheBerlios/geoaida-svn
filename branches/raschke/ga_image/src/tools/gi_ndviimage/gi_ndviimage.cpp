/* TODO: header missing */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <getopt.h>
#include <gaimage.h>
#include <gaalgo.h>

using namespace Ga;

void Usage(int argc, char **argv)
{
	printf("Usage:\n"
         "  %s -h <src> \n"
         "    -h - Calculates ndvi image from irrg image)\n",
         argv[0]);
	exit(0);
}

#ifdef linux
extern "C" {
int __isnanf(float);
	   }
#ifndef NAN
#define NAN sqrt(-1)
#endif
#endif

#define sqr(x) ((x)*(x))


int main(int argc, char **argv)
{
  bool hsv;
  {
    int opt;
    char options[] = "h";
    while ((opt = getopt (argc, argv, options)) != -1) {
      switch (opt) {
      case 'h':
        hsv=true;
        break;
      }
    }
  }
	if (argc-optind<2)
		Usage(argc,argv);
	
   const char* infile=argv[optind++];
   const char* out=argv[optind++];

   Image im;
   im.read(infile);   
   if (im.isEmpty()) {
      fprintf(stderr,"Can't open %s\n",argv[1]);
      return -1;
   }
   

   
   Image result;
   result=toNDVI(im);
   
   result.write(out,0);

}

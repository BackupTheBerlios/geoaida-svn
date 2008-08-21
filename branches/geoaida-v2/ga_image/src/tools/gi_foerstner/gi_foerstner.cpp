/***************************************************************************
                          gi_foerstner.cpp  - Förstner operator
                             -------------------
    begin                : Fri Aug 15 2008
    copyright            : (C) 2008 by Karsten Vogt
    email                : vogt@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <gaimage.h>
#include <string>

float QLim = 0.75;
float WLimF = 1.5;
float WLimC = 5;
float WLim;

int FilterSize = 7;
int NonMaximaBoxSize = 7;

void Usage(int argc, char **argv)
{
	printf("Usage:\n"
         "  %s <infile> <outfile> [-q %%f] [-w %%f] [-f %%i] [-m %%i]\n\n\
	-q :  Minimal roundness of error ellipse [0, 1] (Standard = 0.75)\n\
	-w :  Maximal size of error ellipse (Standard = 1.5)\n\
	-f :  Size of low-pass filter kernel (Standard = 7)\n\
	-m :  Window size of non-maxima suppression (Standard = 7)\n\n",
	argv[0]);

	exit(0);
}

Ga::Image *image;
Ga::Image *result;
int *Gu, *Gv, *Guv;

void computeStructureTensor(int *, int *, int *);
void computeBoxFilter(int *, int);
void suppressNonMaxima(int);

int main(int argc, char **argv)
{
	// Process command line
	if (argc < 3)
		Usage(argc, argv);

	std::string inputFilename = argv[1];
	std::string outputFilename = argv[2];

	for (int i = 3; i < argc - 1; i += 2)
	{
		if (std::string(argv[i]) == std::string("-q"))
			QLim = atof(argv[i+1]);
		else if (std::string(argv[i]) == std::string("-w"))
			WLimF = atof(argv[i+1]);
		else if (std::string(argv[i]) == std::string("-f"))
			FilterSize = atoi(argv[i+1]);
		else if (std::string(argv[i]) == std::string("-m"))
			NonMaximaBoxSize = atoi(argv[i+1]);
	}

	// Load image
	image = new Ga::Image(inputFilename);

	// Calculate tensor
	Gu = new int[image->sizeX() * image->sizeY()];
	Gv = new int[image->sizeX() * image->sizeY()];
	Guv = new int[image->sizeX() * image->sizeY()];

	computeStructureTensor(Gu, Gv, Guv);

	// Apply boxfilter
	computeBoxFilter(Gu, FilterSize);
	computeBoxFilter(Gv, FilterSize);
	computeBoxFilter(Guv, FilterSize);

	// Calculate interest values
	result = new Ga::Image(typeid(float), image->sizeX(), image->sizeY());

	double wmean = 0;
	double normalization = 0;

	for (int y = 0; y < result->sizeY(); y++)
	{
		for (int x = 0; x < result->sizeX(); x++)
		{
			int index = y * image->sizeX() + x;

			float trace = Gu[index] + Gv[index];
			float determinant = (Gu[index] * Gv[index]) - (Guv[index] * Guv[index]);

			float w = determinant / trace;
			float q = (4 * determinant) / (trace * trace);

			// Test for qlim and calculate wlim
			if (q > QLim)
			{
				result->setPixel(x, y, w);
				wmean += w;
				normalization++;
			}
			else
				result->setPixel(x, y, 0);
		}
	}

	WLim = (wmean / normalization) * WLimF;

	for (int y = 0; y < result->sizeY(); y++)
	{
		for (int x = 0; x < result->sizeX(); x++)
		{
			int index = y * image->sizeX() + x;

			float trace = Gu[index] + Gv[index];
			float determinant = (Gu[index] * Gv[index]) - (Guv[index] * Guv[index]);

			float w = determinant / trace;

			// Test for wlim
			if (w <= WLim)
				result->setPixel(x, y, 0);
		}
	}

	// Suppress local non-maxima
	suppressNonMaxima(NonMaximaBoxSize);

	// Write result image
	result->write(outputFilename.c_str());

	// Release memory
	delete image;
	delete result;
	delete[] Gu;
	delete[] Gv;
	delete[] Guv;

	// Return
	return 0;
}

void computeStructureTensor(int *destU, int *destV, int *destUV)
{
	Ga::ImageBase *pimage = image->pImage();

	for (int y = 0; y < image->sizeY(); y++)
	{
		for (int x = 0; x < image->sizeX(); x++)
		{
			int index = y * image->sizeX() + x;

			destU[index] = 0;
			destV[index] = 0;
			destUV[index] = 0;

			for (int c = 0; c < image->noChannels(); c++)
			{
				// Roberts cross
				int du = pimage->getPixelAsDoubleFast(x, y, c) - pimage->getPixelAsDoubleFast(x + 1, y + 1, c);
				int dv = pimage->getPixelAsDoubleFast(x + 1, y, c) - pimage->getPixelAsDoubleFast(x, y + 1, c);

				// Sobel operator
				//int du =	(pimage->getPixelAsDoubleFast(x - 1, y - 1, c) + 2*pimage->getPixelAsDoubleFast(x - 1, y, c) + pimage->getPixelAsDoubleFast(x - 1, y + 1, c)) -
				//			(pimage->getPixelAsDoubleFast(x + 1, y - 1, c) + 2*pimage->getPixelAsDoubleFast(x + 1, y, c) + pimage->getPixelAsDoubleFast(x + 1, y + 1, c));
				//int dv =	(pimage->getPixelAsDoubleFast(x - 1, y - 1, c) + 2*pimage->getPixelAsDoubleFast(x, y - 1, c) + pimage->getPixelAsDoubleFast(x + 1, y - 1, c)) -
				//			(pimage->getPixelAsDoubleFast(x - 1, y + 1, c) + 2*pimage->getPixelAsDoubleFast(x, y + 1, c) + pimage->getPixelAsDoubleFast(x + 1, y + 1, c));

				destU[index] += du * du;
				destV[index] += dv * dv;
				destUV[index] += du * dv;
			}
		}
	}
}

void computeBoxFilter(int *dest, int filtersize)
{
	int filtersize2 = filtersize >> 1;

	// Horizontal filter
	for (int y = 0; y < image->sizeY(); y++)
	{
		int index = y * image->sizeX();

		// Init cbuffer
		int cbuffer[filtersize];
		int cbufferpos = 0;

		for (int i = 0; i < filtersize; i++)
			cbuffer[i] = 0;

		for (int i = 0; i <= filtersize2; i++)
			cbuffer[cbufferpos++] = dest[index + i];

		// Calculate sum
		int sum = 0;
		for (int i = 0; i < filtersize; i++)
			sum += cbuffer[i];

		// Apply filter
		for (int x = 0; x < image->sizeX(); x++)
		{
			sum -= cbuffer[cbufferpos];
			cbuffer[cbufferpos] = (x + filtersize2 < image->sizeX()) ? dest[index + filtersize2] : 0;
			sum += cbuffer[cbufferpos];
			cbufferpos = (cbufferpos + 1) % filtersize;

			dest[index] = sum / filtersize;

			index++;
		}
	}

	// Vertical filter
	for (int x = 0; x < image->sizeX(); x++)
	{
		int index = x;

		// Init cbuffer
		int cbuffer[filtersize];
		int cbufferpos = 0;

		for (int i = 0; i < filtersize; i++)
			cbuffer[i] = 0;

		for (int i = 0; i <= filtersize2; i++)
			cbuffer[cbufferpos++] = dest[index + i * image->sizeX()];

		// Calculate sum
		int sum = 0;
		for (int i = 0; i < filtersize; i++)
			sum += cbuffer[i];

		// Apply filter
		for (int y = 0; y < image->sizeY(); y++)
		{
			sum -= cbuffer[cbufferpos];
			cbuffer[cbufferpos] = (y + filtersize2 < image->sizeY()) ? dest[index + filtersize2 * image->sizeX()] : 0;
			sum += cbuffer[cbufferpos];
			cbufferpos = (cbufferpos + 1) % filtersize;

			dest[index] = sum / filtersize;

			index += image->sizeX();
		}
	}
}

void suppressNonMaxima(int boxsize)
{
	Ga::ImageBase *pimage = result->pImage();

	for (int radius = 1; radius <= boxsize / 2; radius++)
	{
		for (int y = 0; y < result->sizeY(); y++)
		{
			for (int x = 0; x < result->sizeX(); x++)
			{
				float value = pimage->getPixelAsDoubleFast(x, y);
				if (value == 0)
					continue;

				for (int by = y - radius; by <= y + radius; by++)
				{
					for (int bx = x - radius; bx <= x + radius; bx++)
					{
						if ((bx == x) && (by == y))
							continue;

						if (pimage->getPixelAsDoubleFast(bx, by) >= value)
						{
							pimage->setPixelToDouble(x, y, 0);
							goto NextPixel;
						}
					}
				}

			NextPixel:
				continue;
			}
		}
	}
}


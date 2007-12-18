/***************************************************************************
                          gi_test_helper.cpp  -  Unit Test helper tool
                             -------------------
    begin                : 2007-12-14
    copyright            : (C) 2007 by Julian Raschke
    email                : raschke@tnt.uni-hannover.de
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
#include <gaalgo.h>
using namespace Ga;

int main(int argc, char **argv)
{
  if (argc < 3) {
    printf("Usage:\n  %s <function> ..\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  if (!strcmp(argv[1], "--assert-equal")) {
    if (argc < 4) {
      printf("Usage:\n  %s --assert-equal <first-file> <second-file>\n", argv[0]);
      return EXIT_FAILURE;
    }

    Image first(argv[2]);
    Image second(argv[3]);
  
    if (first.noChannels() != second.noChannels())
    {
      printf("Channel count mismatch (%d : %d)\n",
        first.noChannels(), second.noChannels());
      return EXIT_FAILURE;
    }
  
    if (first.sizeX() != second.sizeX())
    {
      printf("Width mismatch (%d : %d)\n",
        first.sizeX(), second.sizeX());
      return EXIT_FAILURE;
    }
  
    if (first.sizeY() != second.sizeY())
    {
      printf("Height mismatch (%d : %d)\n",
        first.sizeY(), second.sizeY());
      return EXIT_FAILURE;
    }
  
    for (int ch = 0; ch < first.noChannels(); ++ch)
      if (!std::equal(first.constBegin(0, ch),
                      first.constBegin(first.sizeY(), ch),
                      second.constBegin(0, ch)))
      {
        printf("Content mismatch (channel %d)\n", ch);
        return EXIT_FAILURE;
      }
  }
  else if (!strcmp(argv[1], "--copy")) {
    if (argc < 4) {
      printf("Usage:\n  %s --copy <source> <dest>\n", argv[0]);
      return EXIT_FAILURE;
    }

    Image image(argv[2]);
    image.write(argv[3]);
  }
  else
  {
    printf("Invalid command.\n");
    return EXIT_FAILURE;
  }
}

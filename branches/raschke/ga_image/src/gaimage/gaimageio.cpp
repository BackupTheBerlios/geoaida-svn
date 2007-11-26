/****************************************************************************
                          gaimageio.h - Image I/O implementation
                             -------------------
    begin                : 2007-10-29
    copyright            : (C) 2007 TNT, Uni Hannover
    authors              : Julian Raschke
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

#include "gaimageio.h"

std::auto_ptr<Ga::ImageIO> Ga::ImageIO::create(const std::string& filename,
  FileType filetype, int sizeX, int sizeY, int channels)
{
  return std::auto_ptr<Ga::ImageIO>();
}

std::auto_ptr<Ga::ImageIO> Ga::ImageIO::reopen(const std::string& filename)
{
  return std::auto_ptr<Ga::ImageIO>();
}

Ga::ImageIO::~ImageIO()
{
}

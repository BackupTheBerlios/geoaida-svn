/***************************************************************************
                          gacachefile.cpp  -  disk file wrapper for
                          					  caching system
                             -------------------
    begin                : 2007-05-28
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

#include "gacache.h"
#include <sstream>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

Ga::CacheFile::CacheFile(Size blockSize)
: blockSize(blockSize)
{
	ostringstream stream;
	stream << "/tmp/gacachefile_" << blockSize << ".tmp";
    filename = stream.str();
	
	fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0600);
	if (fd < 0)
		throw runtime_error("Could not open ga_image cache file: " + filename);
}

Ga::CacheFile::~CacheFile()
{
    assert(find(marked.begin(), marked.end(), true) == marked.end());
	close(fd);
    unlink(filename.c_str());
}

Ga::CacheFile& Ga::CacheFile::get(Size blockSize)
{
	typedef tr1::shared_ptr<CacheFile> FilePtr;

	static map<Size, FilePtr> files;

	FilePtr& ptr = files[blockSize];
	if (!ptr)
		ptr.reset(new CacheFile(blockSize));
	return *ptr;
}

unsigned Ga::CacheFile::store(const void* buffer)
{
	// TODO: large file support (fpos_t), error checking.
	
	// Find a free entry.
	unsigned index;
	vector<bool>::iterator it =
		find(marked.begin(), marked.end(), false);
	if (it != marked.end())
	{
		index = it - marked.begin();
		(*it) = true;
	}
	else
	{
		index = marked.size();
		marked.push_back(true);
	}
	
	// Now go write the data!
	lseek(fd, index * blockSize, SEEK_SET);
	write(fd, buffer, blockSize);
	return index;
}

void Ga::CacheFile::recover(unsigned index, void* buffer)
{
	// TODO: large file support (fpos_t), error checking.
	
	assert(marked.at(index));
	
	// Read stored data from the file.
	lseek(fd, index * blockSize, SEEK_SET);
	read(fd, buffer, blockSize);
	dismiss(index);
}

void Ga::CacheFile::dismiss(unsigned index)
{
	assert(marked.at(index));
	marked[index] = false;
	// TODO: If this was the last block, free disk space again.
}

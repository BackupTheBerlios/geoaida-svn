////////////////////////////////////////////////////////////////////////////////
///
/// \file		gacache.h - Disk file wrapper for caching system.
/// \brief		GeoAIDA Image Algorithms
///
/// \date		2007-05-28
/// \author		Julian Raschke (julian@raschke.de)
///
////////////////////////////////////////////////////////////////////////////////

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
#ifndef WIN32
#include <fcntl.h>
#include <unistd.h>
#endif
using namespace std;

Ga::CacheFile::CacheFile(Size blockSize)
: blockSize(blockSize)
{
#ifdef WIN32
	TCHAR tempPath[MAX_PATH];
	DWORD ret = GetTempPath(MAX_PATH, tempPath);
	if (ret == 0 || ret >= MAX_PATH)
		throw std::runtime_error("Could not get temp path");

	ostringstream stream;
	stream << tempPath << "\gacachefile_" << blockSize << ".tmp";
    filename = stream.str();
	
	DWORD access = GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = FILE_SHARE_READ;
    DWORD creationDisp = OPEN_ALWAYS;

    file = CreateFile(filename.c_str(), access, shareMode, 0,
        creationDisp, FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (file == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not open ga_image cache file: " + filename);
#else
	ostringstream stream;
	stream << "/tmp/gacachefile_" << blockSize << ".tmp";
    filename = stream.str();
	
	fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0600);
	if (fd < 0)
		throw runtime_error("Could not open ga_image cache file: " + filename);
#endif
}

Ga::CacheFile::~CacheFile()
{
    assert(find(marked.begin(), marked.end(), true) == marked.end());
#ifdef WIN32
	CloseHandle(file);
#else
	close(fd);
    unlink(filename.c_str());
#endif
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
#ifdef WIN32
    if (SetFilePointer(file, index * blockSize, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		throw std::runtime_error("Error setting the file pointer");
    DWORD dummy;
    WriteFile(file, buffer, blockSize, &dummy, 0);
#else
	lseek(fd, index * blockSize, SEEK_SET);
	write(fd, buffer, blockSize);
#endif
	return index;
}

void Ga::CacheFile::recover(unsigned index, void* buffer)
{
	// TODO: large file support (fpos_t), error checking.
	
	assert(marked.at(index));
	
	// Read stored data from the file.
#ifdef WIN32
    if (SetFilePointer(file, index * blockSize, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		throw std::runtime_error("Error setting the file pointer");
    DWORD dummy;
    ReadFile(file, buffer, blockSize, &dummy, 0);
#else
	lseek(fd, index * blockSize, SEEK_SET);
	read(fd, buffer, blockSize);
#endif
	dismiss(index);
}

void Ga::CacheFile::dismiss(unsigned index)
{
	assert(marked.at(index));
	marked[index] = false;
	// TODO: If this was the last block, free disk space again.
}

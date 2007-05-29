/***************************************************************************
                          gacache.h  -  caching system that uses both
										the heap and disk files.
                             -------------------
    begin                : 2007-05-19
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

#ifndef _GA_CACHE_H
#define _GA_CACHE_H

#include <cstddef>
#include <cstdlib>
#include <list>
#include <vector>
#include <tr1/memory>

namespace Ga
{
	// Size type for single allocations.
	typedef std::size_t Size;
	// Size type for sums of allocations.
	typedef unsigned long long LargeSize;
	
	// Cache metrics.
	const LargeSize HEAP_USAGE = 512 * 1024*1024;

	// Used by Block to store and recover chunks of memory.
	class CacheFile
	{
		Size blockSize;
		int fd;
		std::vector<bool> marked;
		
		explicit CacheFile(Size blockSize);	
		
		// Copying forbidden.
		CacheFile(const CacheFile&);
		CacheFile& operator=(const CacheFile&);
		
	public:
		static CacheFile& get(Size blockSize);
		~CacheFile();
		unsigned store(const void* buffer);
		void recover(unsigned index, void* buffer);
		void dismiss(unsigned index);
	};

	class Block
	{
		unsigned lastAccess;
		
		Size size;
		unsigned lockCount;
		bool dirty;
		
		// Invariant: Either memory points to allocated memory, or if it is null,
		// fileIndex is a valid (not yet recovered) index into the cache file.
		void* memory;
		unsigned fileIndex;
		
		// Copying explicitly forbidden; handles would dangle.
		Block(const Block& other);
		Block& operator=(const Block& other);
		
	public:
		explicit Block(Size size)
		: lastAccess(0), lockCount(0), size(size), memory(0), dirty(false)
		{
			// Global memory management already knows about this.
			memory = malloc(size);
		}
		
		~Block()
		{
			if (memory)
				free(memory);
			else
				CacheFile::get(size).dismiss(fileIndex);
		}
		
		Size getSize() const
		{
			return size;
		}
		
		unsigned getLastAccess() const
		{
			return lastAccess;
		}
		
		void setLastAccess(unsigned val)
		{
			lastAccess = val;
		}
		
		void markDirty()
		{
			dirty = true;
		}
		
		void markClean()
		{
			dirty = false;
		}
		
		bool isDirty() const
		{
			return dirty;
		}
		
		void* lock();
		// defined below due to order of declarations
		
		void unlock()
		{
			assert(lockCount > 0);
			--lockCount;
		}
		
		bool isLocked() const
		{
			return lockCount > 0;
		}
		
		bool isOnDisk() const
		{
			return memory == 0;
		}
		
		void writeToDisk()
		{
			fileIndex = CacheFile::get(size).store(memory);
			free(memory);
			memory = 0;
		}
	};

	// Minimalistic wrapper around a Block pointer.
	class BlockHandle
	{
		typedef std::tr1::shared_ptr<Block> Ptr;
		Ptr ptr;
		
	public:
		explicit BlockHandle(const Ptr& ptr = Ptr())
		: ptr(ptr)
		{	
		}
		
		bool isEmpty() const
		{
			return !ptr;
		}
		
		const void* lockR()
		{
			assert(!isEmpty());
			return ptr->lock();
		}
		
		void* lockRW()
		{
			assert(!isEmpty());
			if (ptr.use_count() > 1)
				; /* clone */
			ptr->markDirty();
			return ptr->lock();
		}
		
		void unlock()
		{
			assert(!isEmpty());
			ptr->unlock();
		}
	};
	
	class Cache
	{
		typedef std::list<std::tr1::weak_ptr<Block> > Blocks;
		Blocks blocks;
		
		LargeSize heap, total;
		
		Cache()
		: total(0), heap(0)
		{
		}
		
	public:
		static Cache& get();
		
		BlockHandle alloc(Size size);
		void requestDiskToHeap(Size size);
		unsigned sortAndCountBlocks();
		
		LargeSize totalUsage()
		{
			return total;
		}
		
		LargeSize heapUsage()
		{
			return heap;
		}
		
		LargeSize diskUsage()
		{
			return total - heap;
		}
	};

	inline void* Block::lock()
	{
		static unsigned accessCounter = 0;
		
		// Mark as being accessed recently.
		lastAccess = ++accessCounter;
		
		// Access counter overrun (VERY worst case):
		// Normalize all blocks' last access time, start counting from there on.
		if (accessCounter == 0)
			accessCounter = Cache::get().sortAndCountBlocks();

		// If on disk, load again.
		if (!memory)
		{
			// Notify global memory management about our plans.
			Cache::get().requestDiskToHeap(size);

			memory = malloc(size);
			CacheFile::get(size).recover(fileIndex, memory);
		}	

		++lockCount;

		return memory;
	}
}

#endif

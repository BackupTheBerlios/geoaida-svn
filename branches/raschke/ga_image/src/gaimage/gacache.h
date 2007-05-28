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
#include <tr1/memory>

namespace Ga 
{
	// Size type for single allocations.
	typedef std::size_t Size;
	// Size type for sums of allocations.
	typedef unsigned long long LargeSize;
	
	// Cache metrics.
	const LargeSize HEAP_USAGE = 512 * 1024*1024;

	class Block
	{
		unsigned lastAccess;
		
		Size size;
		void* memory;
		bool dirty;
		
		// Copying explicitly forbidden; handles would dangle.
		Block(const Block& other);
		Block& operator=(const Block& other);
		
	public:
		explicit Block(Size size)
		: lastAccess(0), size(size), memory(0), dirty(false)
		{
			memory = malloc(size);
		}
		
		~Block()
		{
			free(memory);
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
		}
		
		bool isOnDisk() const
		{
			return memory == 0;
		}
		
		void writeToDisk()
		{
			throw "nyi";
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
		lastAccess = ++accessCounter;
		if (accessCounter = 0)
			accessCounter = Cache::get().sortAndCountBlocks();

		return memory;
	}

}

#endif

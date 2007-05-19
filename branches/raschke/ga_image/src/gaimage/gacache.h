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
#include <map>
#include <tr1/memory>

namespace Ga 
{
	// Size type for single allocations.
	typedef std::size_t Size;
	// Size type for sums of allocations.
	typedef unsigned long long LargeSize;

	class Block;

	class BlockHandle
	{
		typedef std::tr1::shared_ptr<Block> Ptr;
		Ptr ptr;
		
	public:
		BlockHandle(const Ptr& ptr = Ptr())
		: ptr(ptr)
		{	
		}
		
		const void* lockR()
		{
			return ptr->lockR();
		}
		
		void* lockRW()
		{
			if (!ptr->isWritable())
				; /* clone */
			return ptr->lockRW();
		}
		
		void unlock()
		{
			ptr->unlock();
		}
	};
	
	// Blatantly incomplete sketch.
	class BlockFile
	{
		Size blockSize;
		std::vector<Block> blocks;

	public:		
		BlockFile(Size blockSize) 
		: blockSize(blockSize)
		{
		}
		
		// BlockFiles may only be moved around before they are filled.
		// This ensures that Cache doesn't do anything costly with them.
		BlockFile(const BlockFile& other)
		: blockSize(other.blockSize)
		{
			assert(other.total() == 0);
		}
		BlockFile& operator=(const BlockFile& other)
		{
			blockSize = other.blockSize;
			assert(total() == 0);
			assert(other.total() == 0);
			return *this;
		}
		
		LargeSize() size() const
		{
			return 0;
		}
	};
	
	class Cache
	{
		typedef std::map<Size, BlockFile> BlockFiles;
		BlockFiles files;
		
		typedef std::list<std::tr1::weak_ptr<Block> > Blocks;
		Blocks blocks;
		
		Cache()
		{
		}
		
	public:
		static Cache& get() 
		{
			static Cache cache;
			return cache;
		}
		
		BlockHandle alloc(Size size)
		{
			std::tr1::shared_ptr<Block> newBlock(new Block(size));
			blocks.push_back(Blocks::value_type(newBlock));
			return BlockHandle(newBlock);
		}
		
		LargeSize total()
		{
			TODO;
		}
		
		LargeSize heap()
		{
			return TODO;
		}

		LargeSize disk()
		{
			return total() - heap();
		}
	};
	
	class Block
	{
		unsigned accessId;
		Size size;
		void* memory;
		bool dirty;
		
		// Copying explicitly forbidden; handles would dangle.
		Block(const Block& other);
		Block& operator=(const Block& other);
		
		void markAccess()
		{
			static unsigned accessCounter = 0;
			
			accessId = ++accessCounter;
			
			// TODO:
			// Normalize access ids.
			assert(accessCounter != 0);
		}
		
	public:
		Block(Size size)
		: accessId(0), size(size), memory(0), dirty(false)
		{
			memory = malloc(size);
		}
		
		~Block()
		{
			free(memory);
		}
		
		bool isWritable() const
		{
			return refCount == 1;
		}
		
		const void* lockR() const
		{
			markAccess();
			return memory;
		}
		
		void* lockRW()
		{
			assert(refCount == 1);
			markAccess();
			return memory;
		}
		
		void unlock()
		{
		}
	};
}

#endif

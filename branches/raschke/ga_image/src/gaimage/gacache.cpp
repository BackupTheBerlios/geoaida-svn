/***************************************************************************
                          gacache.cpp  -  main access class for the
                          				  caching system's users
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
#include <stdexcept>
#include <tr1/functional>
using namespace std;

Ga::Cache& Ga::Cache::get()
{
	static Cache cache;
	return cache;
}

namespace
{
	void deleteAndReduceUsage(Ga::Block* block, Ga::LargeSize* total,
		Ga::LargeSize* heap)
	{
		(*total) -= block->getSize();
		if (!block->isOnDisk())
			(*heap) -= block->getSize();
		delete block;
	}
}

Ga::BlockHandle Ga::Cache::alloc(Size size)
{
	total += size;
	heap += size;
	
	tr1::shared_ptr<Block> newBlock(new Block(size),
		tr1::bind(deleteAndReduceUsage, tr1::placeholders::_1,
			&total, &heap));
	blocks.push_back(Blocks::value_type(newBlock));
	return BlockHandle(newBlock);
}

void Ga::Cache::requestDiskToHeap(Size size)
{
	if (heap + size > HEAP_USAGE)
	{
		// Clean up for easier processing.
		compactBlocks();

		// Things must be written to disk before this may happen.
		Blocks::iterator iter = blocks.end();
		--iter;
		do 
		{
			tr1::shared_ptr<Block> block(*iter);
			
			if (block->isLocked())
				throw std::runtime_error("Too many blocks locked at once - lock leak?");
			
			if (!block->isOnDisk())
			{
				block->writeToDisk();
				heap -= block->getSize();
			}
		} while (heap + size > HEAP_USAGE && iter != blocks.begin());
	}

	heap += size;
}

namespace 
{
	bool isFirstBlockMoreImportant(tr1::weak_ptr<Ga::Block> first, tr1::weak_ptr<Ga::Block> second)
	{
		// Move expired blocks to the back.
		if (second.expired())
			return true;
		if (first.expired())
			return false;
		tr1::shared_ptr<Ga::Block> realFirst(first);
		tr1::shared_ptr<Ga::Block> realSecond(second);
		// Move locked blocks to the front.
		if (realFirst->isLocked())
			return true;
		if (realSecond->isLocked())
			return false;
		return realFirst->getLastAccess() > realSecond->getLastAccess();
	}
}

void Ga::Cache::compactBlocks()
{
	blocks.sort(isFirstBlockMoreImportant);
	while (!blocks.empty() && blocks.back().expired())
		blocks.pop_back();
}

unsigned Ga::Cache::normalizeAndCountBlocks()
{
	compactBlocks();
	unsigned id = 0;
	for (Blocks::iterator it = blocks.begin(); it != blocks.end(); ++it, ++id)
		it->setLastAccess(id);
	return blocks.size();
}

/***************************************************************************
                          gacache.cpp  -  caching system that uses both
										  the heap and disk files.
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
#include <tr1/functional>

Ga::Cache& Ga::Cache::get()
{
	static Cache cache;
	return cache;
}

namespace
{
	void deleteAndReduceUsage(Ga::Block* block, Ga::LargeSize* usage)
	{
		(*usage) -= block->getSize();
		delete block;
	}
}

Ga::BlockHandle Ga::Cache::alloc(Size size)
{
	total += size;
	heap += size;
	
	std::tr1::shared_ptr<Block> newBlock(new Block(size),
		std::tr1::bind(deleteAndReduceUsage, std::tr1::placeholders::_1, &total));
	blocks.push_back(Blocks::value_type(newBlock));
	return BlockHandle(newBlock);
}

unsigned Ga::Cache::sortAndCountBlocks()
{
	return blocks.size();
}

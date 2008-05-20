////////////////////////////////////////////////////////////////////////////////
///
/// \file		gacache.h - Caching system that uses both the heap and disk files.
/// \brief		GeoAIDA Image Algorithms
///
/// \date		2007-05-19
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

#ifndef _GA_CACHE_H
#define _GA_CACHE_H

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <list>
#include <string>
#include <vector>
#include <cassert>
#include <tr1/memory>
#ifdef WIN32
#include <windows.h>
#endif

namespace Ga
{
	/// Size type for single allocations.
	/// Also used by images for pixel dimensions. (Equivalent!)
	typedef std::size_t Size;
	/// Size type for sums of allocations.
	/// Also used by images for pixel counts. (Equivalent!)
	typedef unsigned long long LargeSize;
	/// Different type for pixel counts.
  typedef signed long long LargeDiff;

  // Size type for sums of allocations
	
	/// Cache metrics.
	const LargeSize HEAP_USAGE = 512 * 1024*1024;

	/// Used by Block to store and recover chunks of memory.
	class CacheFile
	{
		Size blockSize;
		std::string filename;
#ifdef WIN32
		HANDLE file;
#else
		int fd;
#endif
		std::vector<bool> marked;
		
		explicit CacheFile(Size blockSize);
		
		CacheFile(const CacheFile&);            //< Copying forbidden.
		CacheFile& operator=(const CacheFile&); //< Assignment forbidden.
		
	public:
	  /// Almost a singleton: Get the CacheFile instance for a given block size.
		static CacheFile& get(Size blockSize);
		~CacheFile();
		
		/// Returns the index (handle) of a buffer that is saved to disk.
		unsigned store(const void* buffer);
		
		/// Reloads memory from disk, given an index (handle) and a target buffer.
		/// The block is removed from disk by calling this method, no need to
		/// call dismiss.
		void recover(unsigned index, void* buffer);
		
		/// Tells the CacheFile that it can forget about a block of memory that was
		/// saved to disk.
		void dismiss(unsigned index);
	};

  /// Block of memory that can be written to, and retrieved from disk.
	class Block
	{
		unsigned lastAccess;
		
		Size size;
		unsigned lockCount;
		bool dirty;
		
		// Invariant: Either memory points to allocated memory, or if it is null,
		// fileIndex is a valid (not yet recovered) index into the cache file.
		
		char* memory;
		unsigned fileIndex;
		
		Block(const Block& other);            //< Copying explicitly forbidden; handles would dangle.
		Block& operator=(const Block& other); //< Assignment explicitly forbidden; handles would dangle.
		
	public:
		explicit Block(Size size)
		: lastAccess(0), size(size), lockCount(0), dirty(false)
		{
			// Global memory management already knows about this.
			memory = new char[size];
		}
		
		~Block()
		{
			// Unlocking properly is a critical priority.
			assert(!isLocked());
			
			if (memory)
				delete[] memory;
			else
				CacheFile::get(size).dismiss(fileIndex);
		}
		
		Size getSize() const
		{
			return size;
		}
		
		/// Used for deciding which blocks to write to disk first.
		unsigned getLastAccess() const
		{
			return lastAccess;
		}
		
		/// Used for deciding which blocks to write to disk first.
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
		
		/// Locks the block and returns its memory position, valid until the next
		/// call to unlock(). This implies that a locked block will never be moved
		/// to file storage.
		/// NOTE: If too many blocks are locked, it is possible that more than
		/// heap space than defined by HEAP_USAGE is used.
		void* lock();
		// Defined below for compiler reasons.
				
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
			delete[] memory;
			memory = 0;
		}
	};

	/// Minimalistic wrapper around a Block.
	/// Implements copy-on-write semantics.
	class BlockHandle
	{
		typedef std::tr1::shared_ptr<Block> Ptr;
		Ptr ptr;
		// Locks on the pointee from THIS very handle
    unsigned lockCount;
    // Memory, if locked
    void* data;
		
		void clonePointee();
		// Defined below due to order of definitions.
		
	public:
		explicit BlockHandle(const Ptr& ptr = Ptr())
		: ptr(ptr), lockCount(0)
		{
		}
		
    BlockHandle(const BlockHandle& other)
    : ptr(other.ptr), lockCount(0)
    {
    }
    
    BlockHandle& operator=(const BlockHandle& other)
    {
      BlockHandle(other).swap(*this);
      return *this;
    }
    
    ~BlockHandle()
    {
      assert(lockCount == 0);
    }
		
		void swap(BlockHandle& other)
		{
      assert(lockCount == 0);
      assert(other.lockCount == 0);
      ptr.swap(other.ptr);
		}
		
		bool isEmpty() const
		{
			return !ptr;
		}
		
		/// Locks the target block for reading.
		void lockR()
		{
			assert(!isEmpty());

      lockCount += 1;
			data = ptr->lock();
		}
		
		/// Locks the target block for reading and writing.
		void lockRW()
		{
			assert(!isEmpty());
			
			if (ptr.use_count() > 1)
				// Need to clone the old block.
        clonePointee();
			
			ptr->markDirty();
      lockCount += 1;
			data = ptr->lock();
		}
		
		/// Returns whether the target block has been changed.
		bool isDirty() const
	  {
      return ptr->isDirty();
	  }
	  
	  /// Removes the dirty mark from the target block.
	  void markClean()
	  {
      return ptr->markClean();
	  }
				
		/// Unlocks the target block.
		void unlock()
		{
			assert(!isEmpty());
      assert(lockCount > 0);
			ptr->unlock();
      lockCount -= 1;
		}
		
		/// If locked, returns a pointer to the block's data.
		void* getData() const
		{
      assert(!isEmpty());
      assert(lockCount > 0);
      return data;
		}
	};
	
	/// Little RAII helper that unlocks a block upon leaving its scope.
	class BlockLock
	{
    BlockHandle& handle;
    BlockLock(const BlockLock& other); //< prevent copying
    BlockLock& operator=(const BlockLock& other); //< prevent assignment (shouldn't be possible with a reference member anyway)
    
  public:
    BlockLock(BlockHandle& handle) : handle(handle) {}
    ~BlockLock() { handle.unlock(); }
  };
	
	/// Singleton cache that knows about all allocations and their sizes, and thus
	/// can tell blocks to move to disk storage as a reaction to new allocations.
	class Cache
	{
		typedef std::list<std::tr1::weak_ptr<Block> > Blocks;
		Blocks blocks;
		
		LargeSize heap, total;
		
		Cache()
		: heap(0), total(0)
		{
		}
		
	public:
		static Cache& get();
    ~Cache();
		
		/// Allocates a new block of memory.
		BlockHandle alloc(Size size);

		/// Called internally by the caching system before reloading a block from disk.
		void requestDiskToHeap(Size size);
		
		/// Called internally to sort blocks by importance and delete expired block information.
		void compactBlocks();
		
		// Called by Block in the case of an access id overflow (i.e. more than 2^32 calls to lock):
		// First calls compactBlocks, then sets each block's last access id to its position
		// in the list, and returns the number of blocks.
		unsigned normalizeAndCountBlocks();
		
		/// Memory in use both on the heap and on disk.
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
			accessCounter = Cache::get().normalizeAndCountBlocks();

		// If on disk, load again.
		if (!memory)
		{
			// Notify global memory management about our plans.
			Cache::get().requestDiskToHeap(size);

			memory = new char[size];
			CacheFile::get(size).recover(fileIndex, memory);
		}

		++lockCount;

		return memory;
	}
	
	inline void BlockHandle::clonePointee()
	{
	  // First, unlock the old pointee.
    for (unsigned i = 0; i < lockCount; ++i)
      ptr->unlock();
	  
		BlockHandle clone = Cache::get().alloc(ptr->getSize());
		clone.lockRW();
		try
		{
      lockR();
			std::memcpy(clone.getData(), getData(), ptr->getSize());
			unlock();
		}
		catch (...)
		{
			clone.unlock();
			throw;
		}
		clone.unlock();
    ptr = clone.ptr;
    
    // Now start with locking the new client again, to
    // retain validity of previous lockR calls.
    for (unsigned i = 0; i < lockCount; ++i)
      data = ptr->lock();
	}
}

#endif

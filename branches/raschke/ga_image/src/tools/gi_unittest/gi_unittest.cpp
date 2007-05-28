#include <cassert>
#include <gacache.h>

using namespace Ga;

int main()
{
	// Cache tests.
	
	const Size MB = 1024*1024;
	
	assert(Cache::get().totalUsage() == 0);
	assert(Cache::get().heapUsage() == 0);
	assert(Cache::get().diskUsage() == 0);
	
	BlockHandle handle = Cache::get().alloc(100 * MB);
	
	assert(Cache::get().totalUsage() == 100 * MB);
	assert(Cache::get().heapUsage() == 100 * MB);
	assert(Cache::get().diskUsage() == 0);
	
	handle = BlockHandle();
	
	assert(Cache::get().totalUsage() == 0);
	
	// Image tests to follow.
}

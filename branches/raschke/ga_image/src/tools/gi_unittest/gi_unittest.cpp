#include <cassert>
#include <gacache.h>

using namespace Ga;

int main()
{
	// Cache tests.
	
	const Size MB = 1024*1024;
	
	assert(Cache::get().total() == 0);
	assert(Cache::get().heap() == 0);
	assert(Cache::get().disk() == 0);
	
	BlockHandle handle = Cache::get().alloc(100 * MB);
	
	assert(Cache::get().total() == 100 * MB);
	assert(Cache::get().heap() == 100 * MB);
	assert(Cache::get().disk() == 0);
	
	// Image tests to follow.
}

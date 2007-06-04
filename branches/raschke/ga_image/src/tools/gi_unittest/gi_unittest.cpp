#include <cassert>
#include <gacache.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace Ga;

void beginTest(const std::string& test)
{
    std::cout << test << std::endl;
}

void assertCacheUsage(LargeSize total, LargeSize heap, LargeSize disk)
{
    using namespace std;
    ostringstream usage, expectation;
    usage << Cache::get().totalUsage() << "/" 
          << Cache::get().heapUsage() << "/" 
          << Cache::get().diskUsage();
    expectation << total << "/" << heap << "/" << disk;
    if (usage.str() != expectation.str())
        throw logic_error("Expected usage " + expectation.str() + " but was " + usage.str());
}

int main()
{
	const LargeSize MB = 1024*1024;
	
    beginTest("Cache empty on startup?");
    assertCacheUsage(0, 0, 0);
	
    beginTest("Simple allocation.");
    BlockHandle handle = Cache::get().alloc(100 * MB);
    assertCacheUsage(100 * MB, 100 * MB, 0);

    beginTest("Allocations where something needs to be swapped out.");
    BlockHandle handle2 = Cache::get().alloc(300 * MB);
	BlockHandle handle3 = Cache::get().alloc(300 * MB);
    assertCacheUsage(700 * MB, 400 * MB, 300 * MB);
	
    beginTest("Overusing the heap.");
    handle2 = Cache::get().alloc(600 * MB);
	assertCacheUsage(1000 * MB, 600 * MB, 400 * MB);

    beginTest("Cleaning up.");
	handle = handle2 = handle3 = BlockHandle();
    assertCacheUsage(0, 0, 0);
    
    // TODO:
    // Tests for access id's.

    std::cout << "Cache passed tests." << std::endl;
}

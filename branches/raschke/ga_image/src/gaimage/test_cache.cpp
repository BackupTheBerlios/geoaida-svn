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
  
  beginTest("Random scribbling.");
  int* x1 = (int*)handle.lockRW(); *x1 = 0; handle.unlock();
  int* x2 = (int*)handle2.lockRW(); *x2 = 1; handle2.unlock();
  const int* y1 = (const int*)handle.lockR(); assert(*y1 == 0); handle.unlock();
  const int* y2 = (const int*)handle2.lockR(); assert(*y2 == 1); handle2.unlock();
	
  beginTest("Overusing the heap.");
  handle2 = Cache::get().alloc(600 * MB);
	assertCacheUsage(1000 * MB, 600 * MB, 400 * MB);
	
	beginTest("Reference sharing.");
	handle3 = handle2 = handle;
	assertCacheUsage(100 * MB, 0 * MB, 100 * MB);

	beginTest("Copy-on-write from disk source.");
	handle2.lockRW();
	handle2.unlock();
	assertCacheUsage(200 * MB, 200 * MB, 0 * MB);

	beginTest("Copy-on-write from heap source.");
	handle3.lockRW();
	handle3.unlock();
	assertCacheUsage(300 * MB, 300 * MB, 0 * MB);

  beginTest("Cleaning up.");
	handle = handle2 = handle3 = BlockHandle();
  assertCacheUsage(0, 0, 0);
  
  // TODO:
  // Tests for access id's.

  std::cout << "Cache passed tests." << std::endl;
}

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "BenchmarkUtils.h"
#include "Blueprints\AllocatorDefaultBenchmarks.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneBase.h"
#include "HeapZone\HeapZoneScope.h"
#include "MemoryCommand.h"
#include "MemoryConstants.h"
#include "TestConstants.h"

/**
 * Benchmarks the MemoryCommand->HeapZone->Allocator flow (ie, the 'real world' flow).
 * 
 * Note:
 *	- As of 30/11/2023 these benchmarks are the same ones that are in AllocatorBenchmarks to compare.
 *    However, in the future we may want others (hence the file split).
*/

#ifdef NA_BENCHMARKS
#	define NA_SETUP_MEMORY_COMMAND_FOR_BENCHMARK(wrapperVariableName) \
		MemoryCommand memoryCommand = {}; \
		auto wrapperVariableName = MemoryCommandWrapper{memoryCommand}; \
		NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(HeapZone<DefaultFreeListAllocator>, HeapZoneBase::c_NoParent, \
			blueprints::c_BenchmarkHeapZoneSize, "TestHeapZone", c_NullMemoryTag, &memoryCommand)
#endif // ifdef NA_BENCHMARKS

namespace nabi_allocator::tests
{
#ifdef NA_BENCHMARKS
#	define NA_FIXTURE_NAME NA_BENCHMARK_FIXTURE_NAME(MemoryCommandBenchmarks)

	bool constexpr c_PrintBenchmarkResults = true; 

	// In order to make the MemoryCommand work with the allocator benchmarks, we have to wrap the command in a 'heap zone'.
	// I think quickly doing this bit of jank is better than spending a chunk of time refactoring the AllocatorDefaultBenchmarks...
	class MemoryCommandWrapper : HeapZoneBase
	{
	public:
		MemoryCommandWrapper(MemoryCommand& memoryCommand)
			: m_MemoryCommand(memoryCommand)
		{
		}

		[[nodiscard]] inline void* Allocate(AllocationInfo const& allocationInfo) override
		{
			return m_MemoryCommand.Allocate(allocationInfo.m_NumBytes);
		}

		inline void Free(void* const memory) override
		{
			m_MemoryCommand.Free(memory);
		}

		inline void Reset() override
		{
			NA_ASSERT(m_MemoryCommand.GetHeapZoneScopeCount() == 1u, "The benchmark memory command has more than one heap zone scope");
			HeapZoneScope const* const topHeapZoneScope = m_MemoryCommand.GetTopHeapZoneScope();

			if (topHeapZoneScope)
			{
				topHeapZoneScope->GetHeapZone()->Reset();
			}
			else
			{
				// The program won't crash, but some other asserts will fire about heapzones not having all their memory freed
				NA_ASSERT_FAIL("The top heap zone scope of a benchmark memory command is null!");
			}
		}

	private:
		MemoryCommand& m_MemoryCommand;
	};  

	NA_BENCHMARK(NA_FIXTURE_NAME, MemoryCommandAllocThenFree)
	{
		NA_SETUP_MEMORY_COMMAND_FOR_BENCHMARK(memoryCommandWrapper);
		blueprints::AllocatorAllocThenFree(memoryCommandWrapper, c_PrintBenchmarkResults);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, MemoryCommandVaryingSizeAllocThenFree)
	{
		NA_SETUP_MEMORY_COMMAND_FOR_BENCHMARK(memoryCommandWrapper);
		blueprints::AllocatorAllocThenFree(memoryCommandWrapper, c_PrintBenchmarkResults);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests

#ifdef NA_BENCHMARKS
#	undef NA_SETUP_MEMORY_COMMAND_FOR_BENCHMARK
#endif // ifdef NA_BENCHMARKS

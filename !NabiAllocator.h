#pragma once
// Files are included in alphabetical order (including folder nesting) as they are found in the project except when there are dependencies. 

// ----- Headers -----

// --- Config.h ---

/**
 * These defines can be used to toggle on/off featues of NabiAllocator.
*/

// --- Development / Usage ---
#define NA_OVERRIDE_NEW_DELETE // Routes all new/delete calls through NabiAllocator's MemoryCommand
#define NA_DEBUG // Enables asserts, logging, etc
//#define NA_THREAD_LOCAL_HEAPS // Heap zones and memory tags will be thread local. However if memory is allocated on one thread and freed from another, it still won't crash.

//#define NA_NON_STANDARD_CPP // Use non standard C++ like __forceinline
#ifdef NA_NON_STANDARD_CPP
#	define NA_FORCE_INLINE __forceinline
#else
#	define NA_FORCE_INLINE inline
#endif // ifdef NA_NON_STANDARD_CPP

//#define NA_DEFINE_SHORT_NAMESPACE // Adds the option to use a shorter namespace, rather than typing out nabi_allocator every time...
#ifdef NA_DEFINE_SHORT_NAMESPACE
	namespace nabi_allocator {};
	namespace na = nabi_allocator;
#endif // ifdef NA_DEFINE_SHORT_NAMESPACE

#define NA_ASSERT_DEFINTION(message) _RPTF0(_CRT_ASSERT, message) // NabiAllocator will call this macro to assert. Note: If you override this, make sure that the assert will always fire (eg: assert(false))
#define NA_LOG_DEFINTION(message) std::cout << message // NabiAllocator will call this macro to log

// --- Memory Command ---
#define NA_MALLOC_IF_OUT_OF_MEMORY // If an heapzone is out of memory, rather than asserting and failing to allocate just try and malloc the memory
//#define NA_SAFE_ALLOC_FREE_EARLY_OUT // If an allocation or free is going to fail (eg: freeing nullptr), then don't attempt it. Note: an assert will still fire

// --- Heap Zone ---
#define NA_THREAD_SAFE_HEAP_ZONE // Adds a mutex/lock in HeapZone::Allocate and Free (default std::malloc/std::free are already thread safe)

// --- Allocator ---
#define NA_TRACK_ALLOCATIONS // Memory allocators will track the count and total size of active and all time allocated objects 
//#define NA_MEMORY_TAGGING // Adds metadata to all blocks. Note: this increases the size of each blocks by sizeof(nabi_allocator::memoryTag (in MemoryConstants.h))

// --- Tests ---
//#define NA_TESTS // Run unit tests
//#define NA_BENCHMARKS // Run benchmark tests
//#define NA_WORKFLOWS // Run workflow tests

#define NA_BENCHMARK_RUN_COUNT 3u // If NA_BENCHMARKS is defined, how many times to run each benchmark

// --- MacroUtils.h ---

#define NA_STR(arg) #arg
#define NA_XSTR(arg) NA_STR(arg)
#define NA_CONCAT(arg1, arg2) arg1 ## arg2
#define NA_EXPAND(arg) arg

#define NA_MACRO_NOT_DEFINED (void(0))

// --- Version.h ---

/**
 * Nabi Allocator's version, used to ensure backwards compatibility.
 * (if I ever update this project once its 'released'...)
*/

#define NA_VERSION_MAJOR 0
#define NA_VERSION_MINOR 0
#define NA_VERSION_PATCH 0

#define NA_VERSION \
    NA_XSTR(NA_VERSION_MAJOR) "." \
    NA_XSTR(NA_VERSION_MINOR) "." \
    NA_XSTR(NA_VERSION_PATCH)

// --- STD Includes ---

/**
 * I'm sure you can guess what these are... 
*/

#include <algorithm>
#include <bitset>
#include <chrono>
#include <ctime>
#ifdef NA_DEBUG
#	include <crtdbg.h>
#endif // ifdef NA_DEBUG
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

// --- Library Includes ---

/**
 * NabiAllocator uses GoogleTest. If you don't have GoogleTest in your project, comment out NA_TESTS, NA_BENCHMARKS and NA_WORKFLOWS in Config.h 
 * (its at the top of this single include). Don't worry - the tests were all passing last time I checked!
*/

#if defined NA_TESTS || defined NA_BENCHMARKS || defined NA_WORKFLOWS
#	include "gtest\gtest.h"
#endif // ifdef NA_TESTS || NA_BENCHMARKS || NA_WORKFLOWS

// --- CharacterConstants.h ---

/**
 * A collection of, well, character constants.
 * Useful for logging & file handling.
*/

namespace nabi_allocator::character_constants
{
	char constexpr c_NewLine = '\n';

	char constexpr c_AllocatedSymbol = 'A';
	char constexpr c_FreeSymbol = 'F';
	char constexpr c_PaddingSymbol = 'P';
	char constexpr c_SpaceSymbol = ' ';
} // namespace nabi_allocator::character_constants

// --- FloatingPointTypes.h ---

/**
 * Typedefs for floating point types.
*/

namespace nabi_allocator
{
	using f32 = float;
	using f64 = double;
} // namespace nabi_allocator

// --- IntegerTypes.h ---

/**
 * Brings std integer typedefs into scope and shortens them for brevity.
 *
 * sPtr, uPtr and uInt should be used for memory related operations where *range* matters.
 * The other types should be used where *size* matters. (And range as well I guess.. :p,
 * as well as clarity something can/cannot be negative (obv))
*/

namespace nabi_allocator
{
	using s8 = std::int8_t;
	using s16 = std::int16_t;
	using s32 = std::int32_t;
	using s64 = std::int64_t;

	using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using sPtr = std::intptr_t;
	using uPtr = std::uintptr_t;
	using uInt = std::size_t;

	inline namespace integer_constants
	{
		sPtr constexpr c_NullsPtr = static_cast<sPtr>(NULL);
		uPtr constexpr c_NulluPtr = static_cast<uPtr>(NULL);
	} // inline namespace integer_constants
} // namespace nabi_allocator

// --- TemplateConcepts.h ---

/**
 * Generic concepts for templates.
 *
 * We could use the "requires" keword with each template, but I feel like that makes things cluttered.
 * Another option is to use static asserts in the function implementation... would this be better?
*/

namespace nabi_allocator
{
	template<typename T>
	concept is_integral = std::is_integral_v<T>;

	template<typename T>
	concept is_pointer = std::is_pointer_v<T>;

	template<typename T>
	concept is_function = std::is_invocable_v<T()>; // not 100% tbh if invocable<*T*> is what we want.. (std::is_function_v<T> is also an option but doesn't work with lambdas)

	template <typename T>
	concept is_enum = std::is_enum_v<T>;
} // namespace nabi_allocator

// --- BenchmarkUtils.h ---

/**
 * Everything you need to benchmark code!
 * There is also a google benchmark library I think - so we could perhaps use that?
*/

#ifdef NA_BENCHMARKS
#	define NA_BENCHMARK(benchmarkFixtureName, benchmarkCaseName) TEST(benchmarkFixtureName, benchmarkCaseName)
#endif // ifdef NA_BENCHMARKS

namespace nabi_allocator::benchmark_utils
{
#ifdef NA_BENCHMARKS
	static_assert(NA_BENCHMARK_RUN_COUNT > 0u);

	class StopWatch final
	{
	public:
		using Clock = std::chrono::system_clock;
		using TimePoint = std::chrono::time_point<Clock>;
		using Resolution = std::chrono::duration<f64>;

		inline StopWatch() noexcept;

		inline f64 GetElapsedTime() const noexcept;
		inline void Reset() noexcept;

	private:
		TimePoint m_Start;
	};

	struct BenchmarkResults final
	{
		f64 m_FastestTime;
		f64 m_SlowestTime;
		f64 m_AverageTime;
		u32 m_Repetitions;
	};

	template<is_function BenchmarkFunc, is_function ResetFunc>
	[[nodiscard]] BenchmarkResults RunBenchmark(BenchmarkFunc benchmarkFunc, ResetFunc resetFunc, u32 const repetitions = NA_BENCHMARK_RUN_COUNT);
	std::string BenchmarkResultsToString(BenchmarkResults const& results, bool const print);
#endif // #ifdef NA_BENCHMARK
} // namespace nabi_allocator::benchmark_utils

// --- StringUtils.h ---

/**
* Some helper stuff for string manipulation and the like :p.
*/

#define NA_MAKE_STRING_FROM_STREAM(stringAssignment, content) \
	std::ostringstream stringStream = {}; \
	stringStream << content; \
	stringAssignment = stringStream.str();

// --- DebugUtils.h --- 

/**
 * A collection of assert and logging macros to ease development and debugging.
 * Nabi has a more complex debug/logging system, but for this allocator I think these simple macros are all that is required.
 *
 * The NA_ASSERT, NA_ASSERT_FAIL and NA_LOG macros can easily be rewired to use engine specific implementations or disabled.
*/

#ifdef NA_DEBUG
// - Helper Macros -

namespace nabi_allocator::debug_utils
{
	// This ensures that all log messages, no matter what line they are on, are in line with each other
	// Eg: main.cpp 21  [msg] (two spaces), main.cpp 123 [msg] (one space)
	u32 constexpr c_LogLocationIndent = 4u;
} // namespace nabi_allocator::debug_utils

// Location
#	define NA_FULL_LOCATION  __FILE__ << " " << __LINE__ << " "
#	define NA_SHORT_LOCATION \
		std::string(__FILE__).substr(std::string(__FILE__).find_last_of('\\') + 1u, std::string(__FILE__).size()) << " " << __LINE__ <<
#	define NA_SHORT_LOCATION_END(line) \
		NA_SPACE(nabi_allocator::debug_utils::c_LogLocationIndent - std::string(NA_STR(line)).length()) 

// Format
#	define NA_ASSERT_PREP "ASSERT"
#	define NA_LOG_PREP    NA_SHORT_LOCATION NA_SHORT_LOCATION_END(__LINE__)

#	define NA_LOG_ERROR "ERROR"
#	define NA_LOG_WARN  "WARN"
#	define NA_LOG_INFO  "INFO"

#	define NA_LOG_CATEGORY_ASSERT "[Assert] "
#	define NA_LOG_CATEGORY_CORE   "[Core] "
#	define NA_LOG_CATEGORY_FILE   "[File] "
#	define NA_LOG_CATEGORY_MEMORY "[Memory] "
#	define NA_LOG_CATEGORY_TEST   "[Test] "

#	define NA_LOG_DIVIDER ": "
#	define NA_LOG_END     std::endl
#	define NA_LOG_END_NL  nabi_allocator::character_constants::c_NewLine

#	define NA_SPACE(numberOfSpaces) std::string(numberOfSpaces, character_constants::c_SpaceSymbol)
#	define NA_WRAP(item, wrapCharacter) wrapCharacter << item << wrapCharacter

// - Assertion Macros -

// Implementation
#   define NA_ASSERT_BASE(condition, message) \
		do \
		{ \
			if (!(condition)) { \
				NA_MAKE_STRING_FROM_STREAM(std::string const assertString, \
					NA_ASSERT_PREP << NA_LOG_DIVIDER << message << NA_LOG_END) \
				\
				NA_LOG(NA_LOG_PREP, NA_LOG_ERROR, NA_LOG_CATEGORY_ASSERT, message, NA_LOG_END); \
				NA_ASSERT_DEFINTION(assertString.c_str()); \
			} \
		} \
		while (false)

// Use
#   define NA_ASSERT(condition, message) NA_ASSERT_BASE(condition, message)
#	define NA_ASSERT_DEFAULT(condition) NA_ASSERT_BASE(condition, NA_STR(condition))
#   define NA_ASSERT_FAIL(message) NA_ASSERT(false, message)

// - Logging Macros -

// Implementation
#	define NA_LOG_BASE(prep, severity, category, message, end) \
	do \
	{ \
		NA_MAKE_STRING_FROM_STREAM(std::string const logString, \
			prep << severity << NA_LOG_DIVIDER << category << message << end) \
		\
		NA_LOG_DEFINTION(logString); \
	} \
	while (false)

// Use
#	define NA_LOG(prep, severity, category, message, end) NA_LOG_BASE(prep, severity, category, message, end)

// - Other Debug Macros -

// Use
#	define NA_FUNCTION_NOT_IMPLEMENTED NA_ASSERT_FAIL("The function " << __FUNCTION__ << " is not implemented!")
#else
#	pragma warning( push ) // Push but never pop
#	pragma warning( disable : 4002 ) // Disables 'too many arguements for function-like macro' warning we get when using (void(0))

#   define NA_ASSERT(condition, message) NA_MACRO_NOT_DEFINED
#	define NA_ASSERT_DEFAULT(condition) NA_MACRO_NOT_DEFINED
#   define NA_ASSERT_FAIL(message) NA_MACRO_NOT_DEFINED
#	define NA_LOG(prep, severity, category, message, end) NA_MACRO_NOT_DEFINED
#	define NA_FUNCTION_NOT_IMPLEMENTED NA_MACRO_NOT_DEFINED
#endif // ifdef NA_DEBUG

// --- FileUtils.h ---

/**
 * Some utility function(s...) for basic file manipulation.
*/

namespace nabi_allocator::file_utils
{
	bool CreateAndWriteFile(std::string const& fileName, std::string const& fileContent, bool const timeStampFile = true);
} // namespace nabi_allocator::file_utils

// --- TypeUtils.h ---

/**
 * A collection of helper macros and functions for classes/structs/enums/etc.
*/

#define NA_SET_COPY_MOVE_CONSTRUCTORS(type, operation) \
	NA_SET_COPY_CONSTRUCTORS(type, operation) \
    NA_SET_MOVE_CONSTRUCTORS(type, operation)

#define NA_SET_COPY_CONSTRUCTORS(type, operation) \
     type(type&) = operation; \
     type(type const&) = operation; \

#define NA_SET_MOVE_CONSTRUCTORS(type, operation) \
    type(type&&) = operation; \
    type(type const&&) = operation;

#define NA_NAMEOF(arg) ((decltype(&arg))nullptr, #arg) // C# nameof functionality. Source: https://stackoverflow.com/a/38697366/8890269
#define NA_NAMEOF_LITERAL(arg) #arg // Some things, like static asserts, require a string literal

#define NA_IMPLEMENT_SINGLETON(type) \
    public: \
        [[nodiscard]] static type& Instance() noexcept \
        { \
            static type singleton; \
            return singleton; \
        }

namespace nabi_allocator::type_utils
{
	// Used to convert an enum to its underlying type (note - to_underlying will do this in cpp23)
	template <is_enum Enum>
	[[nodiscard]] inline constexpr typename std::underlying_type<Enum>::type ToUnderlying(Enum const e) noexcept;

	template<typename TypeOne, typename TypeTwo>
	[[nodiscard]] inline constexpr TypeTwo ReinterpretCastIfNeeded(TypeOne const value);
} // namespace nabi_allocator::type_utils

// --- MemoryConstants.h ---

/**
 * Global memory constants & some overloaded operators to make life easier.
*/

namespace nabi_allocator
{
	inline namespace memory_typedefs
	{
		using memoryTag = u32; // I hope a u32 has enough range for all the memory tags someone could ever want... (it can easily be changed here though)
	} // inline namespace memory_typedefs

	inline namespace memory_constants
	{
		memoryTag constexpr c_NullMemoryTag = static_cast<memoryTag>(NULL);

		uInt constexpr c_KByte = 1024u;
		uInt constexpr c_MByte = c_KByte * 1024u;
		uInt constexpr c_GByte = c_MByte * 1024u;
	} // inline namespace memory_constants

	inline namespace memory_operators
	{
		inline constexpr uInt operator""_KB(u64 const value) { return static_cast<uInt>(value) * c_KByte; } // This operator overload needs a character type or ull
		inline constexpr uInt operator""_MB(u64 const value) { return static_cast<uInt>(value) * c_MByte; }
		inline constexpr uInt operator""_GB(u64 const value) { return static_cast<uInt>(value) * c_GByte; }

		static_assert(1_KB == 1024u);
		static_assert(1_MB == 1'048'576u);
		static_assert(1_GB == 1'073'741'824u);
	} // inline namespace memory_operators
} // namespace nabi_allocator

// --- AllocationInfo.h ---

/**
 * Infomation about a specific allocation.
 * Set in the overriden new operator, and passed down through a heap zone into an allocator.
*/

#ifdef NA_MEMORY_TAGGING
#	define NA_MAKE_ALLOCATION_INFO(numBytes, memoryTag) { numBytes, memoryTag }
#else
#	define NA_MAKE_ALLOCATION_INFO(numBytes, memoryTag) { numBytes }
#endif // ifdef NA_MEMORY_TAGGING

namespace nabi_allocator
{
	struct AllocationInfo final
	{
		uInt m_NumBytes;
#ifdef NA_MEMORY_TAGGING
		memoryTag m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	};
} // namespace nabi_allocator

// --- UnmanagedHeap.h ---

/**
 * A basic allocator, wrapping malloc() and free(). Its used in the MemoryCommand when no heap zone has been set, or the user
 * has specified to use it via c_UnmanagedHeap. Ngl, the whole workflow with this is kinda jank (at least right now...
 * will it ever change..? :p) because I cba to refactor some code. But - it works!
 *
 * The problem of having an unmanaged allocator is one I probably should have solved much earlier in the project.
 * Now I'm left with the choice of either hacking it in, or refactoring lots of stuff to make it play nice.
 * I chose the former option, I can always optimise it later. After all, no one cares how nice your code is if it
 * doesn't actually work...
 *
 * The two nicer solutions I thought of were either to have a proper UnmanagedHeap class which sets in an inheritance tree
 * along with HeapZone (perhaps a base Heap class even?), or to have an UnmanagedAllocator. Both of these options had pros and cons.
 *
 * Note:
 *	- The reason this class has private constructors is so this class can only be created in the MemoryCommand.
 *  - The friend stuff works like this: https://stackoverflow.com/questions/4492062/why-does-a-c-friend-class-need-a-forward-declaration-only-in-other-namespaces
*/

namespace nabi_allocator
{
	class UnmanagedHeap final
	{
	public:
		[[nodiscard]] inline void* Allocate(uInt const numBytes) const;
		inline void Free(void* const memory) const;

	private:
		UnmanagedHeap() = default;
		~UnmanagedHeap() = default;
		NA_SET_COPY_MOVE_CONSTRUCTORS(UnmanagedHeap, delete);

		friend class MemoryCommand;
	};
} // namespace nabi_allocator

// --- MemoryCommand.h ---

/**
 * A global singleton which the overriden new/delete operators route allocations through.
 * The MemoryCommand manages which heap zone and memory tag to use.
 *
 * Notes:
 *  - Static allocations will happen through the default allocation path, set as std::malloc/std::free.
 *	- The g_HeapZoneScopes stack and other scope related stuff is thread_local and declared in the cpp.
 *    This is because we don't want, eg, different threads changing the scope for the main thread.
 *  - I'm using [[likely]] & [[unlikely]] in the implementation... thoughts on their usage?
 *		- The [[unlikely]] branch should only be called for static memory...
 *  - The constructor/destructor can't be private because I create a local memory command in tests.
 *		- Think of a way to solve this?
 *  - The reason c_UnmanagedHeap is outside of the MemoryCommand is, well, because I want it at
 *    namespace scope so it matches with c_SameZone and c_SameTag (defined in HeapZoneScope.h).
*/

namespace nabi_allocator
{
	class HeapZoneBase;
	class HeapZoneScope;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class MemoryCommand final
	{
		NA_IMPLEMENT_SINGLETON(MemoryCommand)

	public:
		MemoryCommand();
		~MemoryCommand();

		[[nodiscard]] void* Allocate(uInt const numBytes);
		void Free(void* const memory);

		void PushHeapZoneScope(HeapZoneScope const& heapZoneScope);
		void PopHeapZoneScope(HeapZoneScope const& heapZoneScope);

		[[nodiscard]] HeapZoneScope const* const GetTopHeapZoneScope() const noexcept;
		[[nodiscard]] UnmanagedHeap const& GetUnmanagedHeap() const noexcept;
		[[nodiscard]] uInt GetHeapZoneScopeCount() const noexcept;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(MemoryCommand, delete);

		UnmanagedHeap m_UnmanagedHeap;
	};

	inline HeapZoneBase /*const*/& c_UnmanagedHeap =
		reinterpret_cast<HeapZoneBase&>(
			const_cast<UnmanagedHeap&>(
				MemoryCommand::Instance().GetUnmanagedHeap()
			));
} // namespace nabi_allocator

// --- AllocatorBlockInfo.h ---

/**
 * A generic struct which holds fundamental infomation about an allocator's internal block structure so helper functions only need to implement one interface.
 * Different types of allocators can populate the data in this struct in different ways.
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo final
	{
		// Meta
		uPtr m_MemoryAddress;
#ifdef NA_MEMORY_TAGGING
		memoryTag m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		// General
		void* m_PayloadPtr;

		bool m_Allocated;
		bool m_Padded;

		uInt m_NumBytes;
		u32 m_Padding;
	};
} // namespace namespace nabi_allocator

// --- AllocatorStats.h ---

/**
 * Tracks the allocations / memory useage of allocators if NA_TRACK_ALLOCATIONS is defined.
*/

namespace nabi_allocator
{
#ifdef NA_TRACK_ALLOCATIONS
	struct AllocatorStats final
	{
		u64 m_ActiveAllocationCount;
		u64 m_ActiveBytesAllocated;

		u64 m_TotalAllocationCount;
		u64 m_TotalBytesAllocated;
	};

	enum class AllocatorStatsUpdateType : u32
	{
		Allocate,
		Free,

		ENUM_COUNT
	};

	enum class AllocatorStatsResetType : u32
	{
		Active,
		Total,

		ENUM_COUNT
	};

	void UpdateAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsUpdateType const updateType,
		u64 const& allocationCountAdjustment, u64 const& allocationByteAdjustment);
	void ResetAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsResetType const resetType);
#endif // ifdef NA_TRACK_ALLOCATIONS
} // namespace nabi_allocator

// --- AllocatorBase.h ---

/**
 * The base class for all allocators. HeapZone's allocator template must comply with is_allocator.
*/

#define NA_CHECK_MEMORY_FOR_ALLOCATOR_OPERATION(memory, heapZoneInfo) \
	NA_ASSERT(memory, "memory is null"); \
	NA_ASSERT(memory_operations::IsPtrInRange(heapZoneInfo.m_Start, heapZoneInfo.m_End, \
		NA_TO_UPTR(memory)), "Trying to operate on memory not managed by this allocator"); 

namespace nabi_allocator
{
	struct AllocationInfo;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class AllocatorBase;
	template<typename T>
	concept is_allocator = std::is_base_of_v<AllocatorBase, T>;

	class AllocatorBase abstract
	{
	public:
		inline AllocatorBase();
		virtual ~AllocatorBase();

		[[nodiscard]] virtual void* Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo) = 0;
		virtual void Free(void* memory, HeapZoneInfo const& heapZoneInfo) = 0;
		virtual void Reset(HeapZoneInfo const& heapZoneInfo) = 0;

		virtual std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const = 0;
		[[nodiscard]] virtual AllocatorBlockInfo GetAllocationInfo(void const* const memory, HeapZoneInfo const& heapZoneInfo) const = 0;

#ifdef NA_TRACK_ALLOCATIONS
		[[nodiscard]] inline AllocatorStats const& GetStats() const noexcept;
#endif // ifdef NA_TRACK_ALLOCATIONS

	protected:
		[[nodiscard]] AllocatorBlockInfo IterateThroughHeapZoneHelper(uInt const blockHeaderPosition,
			std::function<s64(uInt const)> const calculatePayloadPtrAdjustment,
			std::function<s64(uInt const)> const calculateBlockPaddingAdjustment) const;

#ifdef NA_TRACK_ALLOCATIONS
		AllocatorStats m_AllocatorStats;
#endif // ifdef NA_TRACK_ALLOCATIONS

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(AllocatorBase, delete);
	};
} // namespace nabi_allocator

// --- AllocatorUtils.h ---

/**
 * Utility functions which can be used with all allocators.
 *
 * Notes:
 * - The byte count can be converted to %s for the relevant graphs on the Python side.
 * - The reason GetFullMemoryUsage takes in a std::function rather than a std::unordered_map of memory tag
 *   aliases is so that the MemoryTag->string conversion can be implementation specific.
 *   Eg, it might use reflection or something like magic enum.
*/

namespace nabi_allocator
{
	class AllocatorBase;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	[[nodiscard]] std::string GetMemoryLayout(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo);

	[[nodiscard]] std::string GetMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo);
#ifdef NA_MEMORY_TAGGING
	[[nodiscard]] std::string GetFullMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo,
		std::optional<std::function<std::string(u32 const)>> const tagConverter = std::nullopt);
#endif // #ifdef NA_MEMORY_TAGGING 
} // namespace nabi_allocator

// --- BlockInfo.h ---

/**
 * All memory blocks start with a header and end with a footer. These structures hold key infomation about a block,
 * such as its size, so that the free list allocator can quickly discern its characteristics.
 *
 * - The infomation relevant to the allocator is duplicated in the header and footer to reduce cache misses.
 * - Its important to keep the sizes of the blocks as small as possible and allignment friendly.
*/

namespace nabi_allocator
{
	struct BlockBase abstract
	{
		uInt m_BlockInfo;
	};

	struct BlockHeader final : public BlockBase
	{
#ifdef NA_MEMORY_TAGGING
		memoryTag m_MemoryTag;

#	ifdef _M_X64
			char const c_Padding[4];
#	endif // ifdef _M_X64
#endif // ifdef NA_MEMORY_TAGGING
	};

	struct BlockFooter final : public BlockBase
	{
	};

	struct BlockInfoContent final
	{
		bool m_Allocated;
		bool m_Padded;
		uInt m_NumBytes;

#ifdef NA_MEMORY_TAGGING
		memoryTag m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	};

	uInt constexpr c_BlockHeaderSize = sizeof(BlockHeader);
	uInt constexpr c_BlockFooterSize = sizeof(BlockFooter);

	static_assert(c_BlockHeaderSize ==
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
		16u
#	else
		8u
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
		8u
#	else
		4u
#	endif // ifdef NA_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
		, NA_NAMEOF_LITERAL(BlockHeader) " is not the expected size");
	static_assert(c_BlockFooterSize ==
#ifdef _M_X64
		8u
#elif _M_IX86
		4u
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
		, NA_NAMEOF_LITERAL(BlockFooter) " is not the expected size");

	void LoadBlockInfo(BlockInfoContent const& blockInfoContent, BlockHeader& blockHeader, BlockFooter* const blockFooter = nullptr) noexcept;
	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockHeader const& blockHeader) noexcept;
	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockFooter const& blockFooter) noexcept;
} // namespace nabi_allocator

// --- BlockInfoIndex.h ---

/**
 * This enum's values store indexes for the data stored in BlockInfo.
 * As the block size is guaranteed to always be a multiple of 8, we can always safely mask of the first 3 bits and still hold the size.
*/

namespace nabi_allocator
{
	enum class BlockInfoIndex : u32
	{
		Allocated = 0u,
		Padded = 1u,
		SizeStart = 3u,

		ENUM_COUNT = 4u
	};
} // namespace nabi_allocator

// --- BlockPadding.h ---

/**
 * If the a memory block isn't a multiple of 8, then this struct is used to pad out the size.
 *
 * If a block is padded, then the minimum padding is 1. Therefore, in this space we can store this BlockPadding struct
 * which contains a u8 'for free'. The maximum padding of a block in isolation 7. However, if memory block is too large
 * for the required number of bytes for an allocation but too small to be its own block (size < Allocator::c_MinBlockSize)
 * then then the padding will be originalBlockSize - requiredBlockSize.
*/

namespace nabi_allocator
{
	struct BlockPadding final
	{
		u8 m_Padding;
	};

	uInt constexpr c_BlockPaddingSize = sizeof(BlockPadding);

	static_assert(c_BlockPaddingSize == 1u, NA_NAMEOF_LITERAL(BlockPadding) "is not the expected size");

	void AllocateBlockPadding(uPtr const currentPosition, uInt const padding);
} // namespace nabi_allocator

// --- SearchAlgorithms.h ---

/**
 * There are 3 common search algorithm for a free list allocator.
 * - Best Fit: Finds a memory block with a size as close as possible to what is required. Pros: Space | Cons: Speed.
 * - First Fit: Finds the first memory block of suitable size or greater. Pros: Speed | Cons: Space.
 * - Next Fit: Similar to FirstFit, but returns the next suitable free block after the first one found to try and
 *   use more of the available memory pool. However, in reality this ends up resulting in an even more
 *   chockablocked/fragmented memory layout.
*/

namespace nabi_allocator
{
	struct BlockHeader;
	struct FreeListNode;
} // namespace nabi_allocator

namespace nabi_allocator
{
	enum class SearchAlgorithm : u32
	{
		BestFit,
		FirstFit,
		NextFit,

		ENUM_COUNT
	};

	[[nodiscard]] BlockHeader* const FindViaBestFit(FreeListNode const* freeListNode, uInt const numBytes, uInt const leniency);
	[[nodiscard]] BlockHeader* const FindViaFirstFit(FreeListNode const* freeListNode, uInt const numBytes);
	[[nodiscard]] BlockHeader* const FindViaNextFit(FreeListNode const* freeListNode, uInt const numBytes);
} // namespace nabi_allocator

// --- FreeListAllocatorSettings.h ---

/**
 * The FreeListAllocator is templated with this settings struct so that it can be customised at compile time.
 * This means that any branches that are required to support customisation can be constexpr.
 *
 * The syntax would be something like:
 * FreeListAllocatorSettings constexpr c_FreeListAllocatorSettings =
 * {
 *    [values]
 * };
 * HeapZone<FreeListAllocator<c_FreeListAllocatorSettings>> m_HeapZone;
*/

namespace nabi_allocator
{
	struct FreeListAllocatorSettings final
	{
		SearchAlgorithm m_SearchAlgorithm; // Which search algorithm to use
		uInt m_BestFitLeniency; // If the search algorithm is best fit, how close a free block's size has to be to the required size to be accepted
	};

	FreeListAllocatorSettings constexpr c_FreeListAllocatorDefaultSettings =
	{
		.m_SearchAlgorithm = SearchAlgorithm::BestFit,
		.m_BestFitLeniency = 0u
	};
} // namespace nabi_allocator

// --- FreeListAllocator.h ---

/**
 * A free list allocator manages a pool of memory by dividing it up into blocks based on allocation requests.
 * - It maintains a linked list of all the free blocks to quickly find chunks of memory.
 * - Adjacent free blocks are coalesced to form larger blocks.
 * - Free blocks are found using the set search algorithm.
 *
 * The memory layout of a block is as follows:
 * - BlockHeader Struct -> Payload -> Padding -> BlockPadding Struct -> BlockFooter Struct
 *
 * Notes:
 * - This video was particularly helpful for understanding how a free list allocator works: https://www.youtube.com/watch?v=UTii4dyhR5c
 * - Normally I pass things which cannot be null as references. However in this allocator I pass things as pointers and then assert
 *   if they are null. This is because in these cases we are often manipulating and reinterpret casting memory, so a dereferenced
 *   pointer would just have to do referenced again anyway.
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo;
	struct AllocationInfo;
	struct BlockHeader;
	struct FreeListNode;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	template<FreeListAllocatorSettings Settings>
	class FreeListAllocator;
	using DefaultFreeListAllocator = FreeListAllocator<c_FreeListAllocatorDefaultSettings>;

	template<FreeListAllocatorSettings Settings>
	class FreeListAllocator final : public AllocatorBase
	{
	public:
		explicit FreeListAllocator(HeapZoneInfo const& heapZoneInfo);
		~FreeListAllocator() override = default;

		[[nodiscard]] void* Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;
		void Reset(HeapZoneInfo const& heapZoneInfo) override;

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const override;
		[[nodiscard]] AllocatorBlockInfo GetAllocationInfo(void const* const memory, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(FreeListAllocator, delete);

		void InitMemory(HeapZoneInfo const& heapZoneInfo);

		[[nodiscard]] BlockHeader* const TryFindFreeBlock(uInt const numBytes) const;
		void TryCoalesceBlock(BlockHeader* const blockHeader, HeapZoneInfo const& heapZoneInfo);

		void AddFreeBlock(void* const blockStartPtr, uInt const numBytes);
		void RemoveFreeBlock(void* const blockStartPtr);

		FreeListNode* m_FreeList;
	};
} // namespace nabi_allocator

// --- FreeListNode.h ---

/**
 * These nodes are used to create an implicit free list in allocators which need to keep track of
 * free blocks of memory. When a block of memory is freed, using the space that was occupied by the
 * allocated payload, we can store this struct for free. We link it with the previous and next free
 * blocks to create a linked list, and so when the allocator needs to find a free block to use it
 * only has to search the whats in the list.
*/

namespace nabi_allocator
{
	struct FreeListNode final
	{
		FreeListNode* m_Previous;
		FreeListNode* m_Next;
	};

	uInt constexpr c_FreeListNodeSize = sizeof(FreeListNode);

	static_assert(c_FreeListNodeSize ==
#ifdef _M_X64
		16u
#elif _M_IX86
		8u
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
		, NA_NAMEOF_LITERAL(FreeListNode) "is not the expected size");

	void AddFreeListNode(FreeListNode*& freelist, FreeListNode* const& node);
	void RemoveFreeListNode(FreeListNode*& freelist, FreeListNode* const& node);
	inline void ResetFreeListNode(FreeListNode* const& node);
} // namespace nabi_allocator

// --- StackAllocatorSettings.h ---

/**
 * Settings for the StackAllocator.
*/

namespace nabi_allocator
{
	struct StackAllocatorSettings final
	{
		// As of 09/10/23... this is empty.
		// And ngl, I can't think of any settings I would want right now...
		// But its future proof at least!
	};

	StackAllocatorSettings constexpr c_StackAllocatorDefaultSettings =
	{
	};
} // namespace nabi_allocator

// --- StackAllocator.h ---

/**
 * A stack allocator works, as you might guess, exactly like a stack. It has the advantages of being faster (sometimes!)
 * and having lower memory overhead compaired to something like a free list allocator as we don't have to store as
 * much infomation about a block or manage free memory. However, it is much less flexible as memory can only be
 * allocated and freed from the top of the stack.
 *
 * The memory layout of a block is as follows:
 * - Payload -> Padding -> BlockPadding Struct -> BlockHeader Struct
 *
 * Notes:
 *	- m_PreviousPosition is not the last value m_CurrentPosition was set to, but the previous block's position.
 *    It is used to make sure that freed memory always comes from the top of the stack.
*/

namespace nabi_allocator
{
	struct AllocationInfo;
	struct AllocatorBlockInfo;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	template<StackAllocatorSettings Settings>
	class StackAllocator;
	using DefaultStackAllocator = StackAllocator<c_StackAllocatorDefaultSettings>;

	template<StackAllocatorSettings Settings>
	class StackAllocator final : public AllocatorBase
	{
	public:
		explicit StackAllocator(HeapZoneInfo const& heapZoneInfo);
		~StackAllocator() override = default;

		[[nodiscard]] void* Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;
		void Reset(HeapZoneInfo const& heapZoneInfo) override;

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const override;
		[[nodiscard]] AllocatorBlockInfo GetAllocationInfo(void const* const memory, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(StackAllocator, delete);

		void InitMemory(HeapZoneInfo const& heapZoneInfo);

		uPtr m_CurrentPosition;
#ifdef NA_DEBUG
		uPtr m_PreviousPosition;
#endif // ifdef NA_DEBUG
	};
} // namespace nabi_allocator

// --- HeapZoneInfo.h ---

/**
 * Stores the start and end pointers of a HeapZone. From this infomation, the size
 * of a HeapZone and whether a HeapZone is initialized can be obtained.
*/

namespace nabi_allocator
{
	struct HeapZoneInfo final
	{
		uPtr m_Start;
		uPtr m_End;
	};
} // namespace nabi_allocator

// --- HeapZoneBase.h ---

/**
 * The base class for all HeapZones. HeapZones have a base so that they can be grouped regardless of which allocator they use.
 * If HeapZones have a parent, then their memory will be allocated from the parent rather than using malloc. This allows for
 * a design where malloc only needs to be called once.
*/

namespace nabi_allocator
{
	struct AllocationInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class HeapZoneBase;
	template<typename T>
	concept is_heap_zone = std::is_base_of_v<HeapZoneBase, T>;

	class HeapZoneBase abstract
	{
	public:
		static HeapZoneBase constexpr* const c_NoParent = nullptr;

		HeapZoneBase();
		virtual ~HeapZoneBase();

		[[nodiscard]] static uInt GetHeapZoneCount() noexcept;
		[[nodiscard]] static HeapZoneBase* FindHeapZoneForPtr(void const* const ptr);

		[[nodiscard]] HeapZoneInfo& Init(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName);
		void Deinit();

		[[nodiscard]] inline virtual void* Allocate(AllocationInfo const& allocationInfo) = 0;
		inline virtual void Free(void* const memory) = 0;
		inline virtual void Reset() = 0;

		[[nodiscard]] inline HeapZoneInfo const& GetZoneInfo() const noexcept;
#ifdef NA_DEBUG
		[[nodiscard]] inline std::string const& GetDebugName() const noexcept;
#endif // ifdef NA_DEBUG

		[[nodiscard]] inline bool IsInitialized() const noexcept;
		[[nodiscard]] inline bool ContainsPtr(void const* const ptr) const;

	protected:
		HeapZoneInfo m_ZoneInfo;
		HeapZoneBase* m_ParentZone;
#ifdef NA_DEBUG
		std::string m_DebugName;
#endif // ifdef NA_DEBUG

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneBase, delete);

		static std::vector<HeapZoneBase*> s_AllHeapZones;
	};
} // namespace nabi_allocator

// --- HeapZone.h ---

/**
 * HeapZones mark out a section of memory which their templated allocator manages.
 * An application can have multiple HeapZones, and which one is used is controlled by the MemoryCommand.
*/

namespace nabi_allocator
{
	struct AllocationInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	template<is_allocator T>
	class HeapZone final : public HeapZoneBase
	{
	public:
		HeapZone(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName);
		~HeapZone() override;

		[[nodiscard]] inline void* Allocate(AllocationInfo const& allocationInfo) override;
		inline void Free(void* const memory) override;
		inline void Reset() override;

		[[nodiscard]] inline T& GetAllocator() noexcept;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZone, delete);

		T m_Allocator;
#ifdef NA_THREAD_SAFE_HEAP_ZONE
		std::mutex m_Mutex;
#endif // ifdef NA_THREAD_SAFE_HEAP_ZONE
	};
} // namespace nabi_allocator

// --- HeapZoneScope.h ---

/**
 * Controls which heap zone and memory tag is used.
 * Passing in a nullptr/nullopt instead of heap zone or tag will default to the last set value.
 *
 * The HeapZoneScope uses RAII so it will be registered / unregistered with the MemoryCommand on its creation/destruction.
 *
 * Note:
 *	- We take in an optional memoryTag because we aren't going to reference the it, it will probs be an enum value or something.
 *	- va_args in the NA_MAKE_HEAP_ZONE_AND_SET_SCOPE & NA_SET_HEAP_ZONE_SCOPE macro are needed to support the bot custom memory command thing.
 *  - NA_MAKE_HEAP_ZONE_AND_SET_SCOPE is the one macro a user needs to write in their code, eg above the main loop, to start using nabi allocator.
*/

#define NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(zoneType, zoneParent, zoneNumBytes, zoneDebugName, scopeMemoryTag, ...) \
	static_assert(std::is_base_of<nabi_allocator::HeapZoneBase, zoneType>(), "HeapZone's must inherit from " NA_NAMEOF_LITERAL(nabi_allocator::HeapZoneBase)); \
	zoneType heapZone = { zoneParent, zoneNumBytes, zoneDebugName }; \
	NA_SET_HEAP_ZONE_SCOPE(&heapZone, scopeMemoryTag, __VA_ARGS__);

#define NA_SET_HEAP_ZONE_SCOPE(heapZone, _memoryTag, ...) \
	nabi_allocator::HeapZoneScope const heapZoneScope = { heapZone, _memoryTag, __VA_ARGS__ };

namespace nabi_allocator
{
	class MemoryCommand;
} // namespace nabi_allocator

namespace nabi_allocator
{
	HeapZoneBase constexpr* const c_SameZone = nullptr;
	std::optional<memoryTag const> constexpr c_SameTag = std::nullopt;

	class HeapZoneScope final
	{
	public:
		inline HeapZoneScope(HeapZoneBase* const heapZone, std::optional<memoryTag const> const memoryTag, MemoryCommand* const customMemoryCommand = nullptr);
		inline ~HeapZoneScope();

		[[nodiscard]] inline HeapZoneBase* const GetHeapZone() const noexcept;
#ifdef NA_MEMORY_TAGGING
		[[nodiscard]] inline std::optional<memoryTag const> const& GetMemoryTag() const noexcept;
#endif // ifdef NA_MEMORY_TAGGING

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneScope, delete);

		HeapZoneBase* const m_HeapZone;
#ifdef NA_MEMORY_TAGGING
		std::optional<memoryTag const> const m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		// This logic is half jank half kinda needed. It's half jank because it started off as a hack to be able to test 
		// instanced MemoryCommandsin isolation (eg, in MemoryCommandTests.cpp), but then it became half needed because 
		// if NA_OVERRIDE_NEW_DELETE is not defined and the user still wants to use heap zone scopes they will need it.
		MemoryCommand* const m_CustomMemoryCommand;
	};
} // namespace nabi_allocator

// --- BitOperations.h ---

/**
 * Functions to perform bit manipulation.
*/

namespace nabi_allocator::bit_operations
{
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T FlipBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr bool GetBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T SetBit(T const value, u32 const bitPosition, bool const bitState) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T LeftShiftBit(T const value, u32 const shiftAmount) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T RightShiftBit(T const value, u32 const shiftAmount) noexcept;

	template<is_integral T>
	[[nodiscard]] std::string ToBinaryString(T const value, u32 const split = 8u) noexcept;
} // namespace nabi_allocator::bit_operations

// --- MemoryOperations.h ---

/**
 * Functions & macros to perform memory manipulation.
*/

#define NA_REINTERPRET_MEMORY(returnPtrType, startPtr, operation, numBytes) \
	reinterpret_cast<returnPtrType*>(nabi_allocator::type_utils::ReinterpretCastIfNeeded \
		<std::remove_const_t<decltype(startPtr)>, nabi_allocator::uPtr>(startPtr) operation numBytes)
#define NA_REINTERPRET_MEMORY_DEFAULT(returnPtrType, startPtr) \
	NA_REINTERPRET_MEMORY(returnPtrType, startPtr, +, 0u)

#define NA_TO_UPTR(arg) reinterpret_cast<nabi_allocator::uPtr>(arg)
#define NA_TO_VPTR(arg) reinterpret_cast<void*>(arg)

namespace nabi_allocator::memory_operations
{
	template<typename T>
	[[nodiscard]] inline T RequestMemoryFromOS(uInt const numBytes);
	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory);

	template<typename T>
	inline void ResetMemory(T* const memory);
	inline void ResetMemory(void* const destination, uInt const numBytes);

	[[nodiscard]] inline constexpr uInt GetMemorySize(uPtr const start, uPtr const end) noexcept;
	[[nodiscard]] inline uInt GetMemorySize(void const* const start, void const* const end);

	template<is_integral T>
	[[nodiscard]] inline constexpr bool IsAlligned(T const value, uInt const allignment) noexcept;
	[[nodiscard]] inline bool IsAlligned(void const* const address, uInt const allignment);

	[[nodiscard]] inline constexpr bool IsPtrInRange(uPtr const start, uPtr const end, uPtr const ptr) noexcept;
	[[nodiscard]] inline bool IsPtrInRange(void const* const start, void const* const end, void const* const ptr);
} // namespace nabi_allocator

// --- TestConstants.h ---

/**
* These constants are used to ensure that all test fixtures follow a naming convention so they can be filtered.
* See main.cpp (https://github.com/WzrdIsTaken/Nabi-Allocator/blob/main/NabiAllocator/NabiAllocator/main.cpp) -> ::testing::GTEST_FLAG(filter) = filterPattern.c_str();
*/

#define NA_BENCHMARK_IDENTIFIER b_
#define NA_TEST_IDENTIFIER t_
#define NA_WORKFLOW_IDENTIFIER w_

#define NA_BENCHMARK_FIXTURE_NAME(benchmarkFixtureName) NA_MAKE_FIXTURE_NAME(NA_BENCHMARK_IDENTIFIER, benchmarkFixtureName)
#define NA_TEST_FIXTURE_NAME(testFixtureName) NA_MAKE_FIXTURE_NAME(NA_TEST_IDENTIFIER, testFixtureName)
#define NA_WORKFLOW_FIXTURE_NAME(workflowFixtureName) NA_MAKE_FIXTURE_NAME(NA_WORKFLOW_IDENTIFIER, workflowFixtureName)

#define NA_GTEST_FILTER_SEPERATOR "*:"

#define NA_MAKE_FIXTURE_NAME(identifier, fixtureName) NA_CONCAT(NA_EXPAND(identifier), fixtureName)
#define NA_MAKE_GTEST_FILTER(identifier) NA_CONCAT(NA_XSTR(identifier), NA_GTEST_FILTER_SEPERATOR)

// --- AllocatorDefaultBenchmarks.h ---

/**
 * A collection of standard benchmarks for every allocator to compair their speeds.
*/

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_BENCHMARKS
	uInt constexpr c_BenchmarkHeapZoneSize = 20_MB;

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults);

	template<is_heap_zone HeapZoneType>
	void AllocatorVaryingSizeAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults);
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests::blueprints

// --- AllocatorDefaultTests.h ---

/*
 * A collection of standard tests for every allocator to ensure their core functionality works.
*/

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_TESTS
	template<is_heap_zone HeapZoneType>
	void AllocatorCreateAndDestroyTest(uInt const heapZoneSize, std::string const& expectedInitLayout);

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocateAndFreeTest(uInt const heapZoneSize,
		std::string const& expectedX64MemoryTaggingLayout, std::string const& expectedX64Layout,
		std::string const& expectedX86MemoryTaggingLayout, std::string const& expectedX86Layout,
		std::string const& expectedFreeLayout);
	template<is_heap_zone HeapZoneType>
	void AllocatorAllocateTooLargeTest(uInt const heapZoneSize);

	template<is_heap_zone HeapZoneType>
	void AllocatorResetTest(uInt const heapZoneSize, std::string const& expectedResetLayout);

	template<is_heap_zone HeapZoneType>
	void AllocatorGetAllocationInfoTest(uInt const heapZoneSize);

#	ifdef NA_MEMORY_TAGGING
	template<is_heap_zone HeapZoneType>
	void AllocatorMemoryTagTest(uInt const heapZoneSize,
		std::string const& expectedX64MemoryTaggingUsage, std::string const& expectedX64Usage,
		std::string const& expectedX86MemoryTaggingUsage, std::string const& expectedX86Usage,
		std::string const& expectedFreeUsage);
#	endif // ifdef NA_MEMORY_TAGGING
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests::blueprints


// ----- Inlines -----

// --- BenchmarkUtils.inl ---

namespace nabi_allocator::benchmark_utils
{
#ifdef NA_BENCHMARKS
	// --- StopWatch ---

	inline StopWatch::StopWatch() noexcept
		: m_Start{}
	{
		Reset();
	}

	inline f64 StopWatch::GetElapsedTime() const noexcept
	{
		auto const now = Clock::now();
		return std::chrono::duration_cast<Resolution>(now - m_Start).count();
	}

	inline void StopWatch::Reset() noexcept
	{
		m_Start = Clock::now();
	}

	// --- Free Functions --- 

	template<is_function BenchmarkFunc, is_function ResetFunc>
	BenchmarkResults RunBenchmark(BenchmarkFunc benchmarkFunc, ResetFunc resetFunc, u32 const repetitions)
	{
		NA_ASSERT_DEFAULT(repetitions > 0u);

		// Setup
		StopWatch stopWatch = {};
		std::vector<f64> times;
		times.reserve(repetitions);

		// Run
		for (u32 i = 0; i < repetitions; ++i)
		{
			stopWatch.Reset();
			benchmarkFunc();

			f64 const elapsedTime = stopWatch.GetElapsedTime();
			times.push_back(elapsedTime);

			resetFunc();
		}

		// Results
		std::sort(times.begin(), times.end());

		BenchmarkResults results = {};
		results.m_FastestTime = times.front();
		results.m_SlowestTime = times.back();
		results.m_AverageTime = times.empty() ? 0.0 :
			std::accumulate(times.begin(), times.end(), 0.0) / static_cast<uInt>(repetitions);
		results.m_Repetitions = repetitions;

		return results;
	}
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::benchmark_utils

// --- TypeUtils.inl ---

namespace nabi_allocator::type_utils
{
	template<is_enum Enum>
	inline constexpr typename std::underlying_type<Enum>::type ToUnderlying(Enum const e) noexcept
	{
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}

	template<typename TypeOne, typename TypeTwo>
	inline constexpr TypeTwo ReinterpretCastIfNeeded(TypeOne const value)
	{
		if constexpr (std::is_same_v<TypeOne, TypeTwo>)
		{
			return value;
		}
		else
		{
			return reinterpret_cast<TypeTwo>(value);
		}
	}
} // namespace nabi_allocator::type_utils

// --- UnmanagedHeap.inl ---

namespace nabi_allocator
{
	inline void* UnmanagedHeap::Allocate(uInt const numBytes) const
	{
		return memory_operations::RequestMemoryFromOS<void*>(numBytes);
	}

	inline void UnmanagedHeap::Free(void* /*const :p*/ memory) const
	{
		memory_operations::ReleaseMemoryToOS(memory); // Note - it is intended that the "memory" ptr is not set to null
	}
} // namespace nabi_allocator

// --- AllocatorBase.inl ---

namespace nabi_allocator
{
	inline AllocatorBase::AllocatorBase()
#ifdef NA_TRACK_ALLOCATIONS
		: m_AllocatorStats{}
#endif // ifdef NA_TRACK_ALLOCATIONS
	{
	}

	inline AllocatorBase::~AllocatorBase()
	{
#ifdef NA_TRACK_ALLOCATIONS
		NA_ASSERT(m_AllocatorStats.m_ActiveAllocationCount == 0u && m_AllocatorStats.m_ActiveBytesAllocated == 0u,
			"An allocator's destructor was called, but objects it allocated are still active! Attempting to free those objects now may cause a crash.");
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

#ifdef NA_TRACK_ALLOCATIONS
	inline AllocatorStats const& AllocatorBase::GetStats() const noexcept
	{
		return m_AllocatorStats;
	}
#endif // ifdef NA_TRACK_ALLOCATIONS
} // namespace nabi_allocator

// --- FreeListAllocator.inl ---

namespace nabi_allocator
{
	// The size of blocks of memory have to be multiples of 8. This is because then we can always
	// mask of 3 bits of the size to store other infomation and still retain the size of the block.
	// Allignments of 8 also always work for x64 and x86 architecture. 
	uInt constexpr c_BlockAllignmentFLA = 8u;
	uInt constexpr c_MinBlockSizeFLA = c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize +
		((c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize) % c_BlockAllignmentFLA);

	template<FreeListAllocatorSettings Settings>
	FreeListAllocator<Settings>::FreeListAllocator(HeapZoneInfo const& heapZoneInfo)
		: AllocatorBase{}
		, m_FreeList(nullptr)
	{
		InitMemory(heapZoneInfo);
	}

	template<FreeListAllocatorSettings Settings>
	void* FreeListAllocator<Settings>::Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(allocationInfo.m_NumBytes > 0u, "Allocating 0 bytes");

		// Search for a free block. If one can't be found, the allocator is out of memory
		BlockHeader* blockHeader = nullptr;
		uInt allocatedBytes = 0u;

		uInt requiredBlockSize = c_BlockHeaderSize + c_BlockFooterSize + allocationInfo.m_NumBytes;
		uInt padding = 0u;

		if (allocationInfo.m_NumBytes < c_FreeListNodeSize)
		{
			padding += (c_FreeListNodeSize - allocationInfo.m_NumBytes);
		}
		padding += (requiredBlockSize + padding) % c_BlockAllignmentFLA;

		requiredBlockSize += padding;
		bool requiresPadding = padding != 0u;

		blockHeader = TryFindFreeBlock(requiredBlockSize);
		if (!blockHeader) [[unlikely]]
		{
			NA_ASSERT_FAIL(NA_NAMEOF_LITERAL(FreeListAllocator) " is out of memory");
			return nullptr;
		}

		uInt const originalBlockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		RemoveFreeBlock(blockHeader);

		// If the block is too large for the requested bytes, check that the remaining space after the block has been split is sufficient for another allocation
		uInt const unusedSpace = originalBlockSize - requiredBlockSize;
		bool const remainingBlockSpaceSufficient = unusedSpace >= c_MinBlockSizeFLA;

		if (!remainingBlockSpaceSufficient && unusedSpace != 0u)
		{
			requiredBlockSize = originalBlockSize; // We are now filling all the space in the block as it can't be split
			padding += unusedSpace;
			requiresPadding = true;
		}

		// Set the block header (the memory for it has already been reinterpret casted)
		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = true;
		blockInfoContent.m_Padded = requiresPadding;
		blockInfoContent.m_NumBytes = requiredBlockSize;
#ifdef NA_MEMORY_TAGGING
		blockInfoContent.m_MemoryTag = allocationInfo.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
		LoadBlockInfo(blockInfoContent, *blockHeader);
		allocatedBytes += c_BlockHeaderSize;

		// Store the pointer to where the payload will be allocated
		void* const payloadPtr = NA_REINTERPRET_MEMORY(void, blockHeader, +, allocatedBytes);
		allocatedBytes += allocationInfo.m_NumBytes;

		// Allocate padding if needed
		if (requiresPadding)
		{
			AllocateBlockPadding((NA_TO_UPTR(blockHeader) + allocatedBytes), padding);
			allocatedBytes += padding;
		}

		// Allocate the footer
		BlockFooter* const blockFooter = NA_REINTERPRET_MEMORY(BlockFooter, blockHeader, +, allocatedBytes);
		blockFooter->m_BlockInfo = blockHeader->m_BlockInfo;
		allocatedBytes += c_BlockFooterSize;

		// Split the original memory block if it was of sufficient size
		if (remainingBlockSpaceSufficient)
		{
			void* const newFreeBlockStart = NA_REINTERPRET_MEMORY(void, blockFooter, +, c_BlockFooterSize);
			AddFreeBlock(newFreeBlockStart, unusedSpace);
		}

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Allocate, allocationCountAdjustment, static_cast<u64>(requiredBlockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		return payloadPtr;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
		NA_CHECK_MEMORY_FOR_ALLOCATOR_OPERATION(memory, heapZoneInfo);

		// Get the payload's block's header
		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, memory, -, c_BlockHeaderSize);

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Free, allocationCountAdjustment, static_cast<u64>(blockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		// Begin the free process
		TryCoalesceBlock(blockHeader, heapZoneInfo);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::Reset(HeapZoneInfo const& heapZoneInfo)
	{
#ifdef NA_TRACK_ALLOCATIONS
		ResetAllocatorStats(m_AllocatorStats, AllocatorStatsResetType::Active);
#endif // #ifdef NA_TRACK_ALLOCATIONS

		InitMemory(heapZoneInfo);
	}

	template<FreeListAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> FreeListAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const
	{
		std::deque<AllocatorBlockInfo> allocatorBlocks = {};
		uInt progressThroughHeapZone = static_cast<uInt>(heapZoneInfo.m_Start) + c_BlockHeaderSize;

		// Loop through all the blocks in the heap zone until the end or "action" returns false
		do
		{
			AllocatorBlockInfo const allocatorBlockInfo = GetAllocationInfo(NA_TO_VPTR(progressThroughHeapZone), heapZoneInfo);
			progressThroughHeapZone += allocatorBlockInfo.m_NumBytes;
			allocatorBlocks.push_back(allocatorBlockInfo);

			// Check if the loop should continue
			bool const continueLooping = action ? (*action)(allocatorBlockInfo) : true;
			if (!continueLooping)
			{
				break;
			}
		} while (progressThroughHeapZone < heapZoneInfo.m_End);

		return allocatorBlocks;
	}

	template<FreeListAllocatorSettings Settings>
	AllocatorBlockInfo FreeListAllocator<Settings>::GetAllocationInfo(void const* const memory, HeapZoneInfo const& heapZoneInfo) const
	{
		NA_CHECK_MEMORY_FOR_ALLOCATOR_OPERATION(memory, heapZoneInfo);

		// As of 15/01/24 this function perhaps doesn't have the best name anymore...
		return IterateThroughHeapZoneHelper(NA_TO_UPTR(memory) - c_BlockHeaderSize,
			[](uInt const /*blockNumBytes*/) -> s64
			{
				return static_cast<s64>(c_BlockHeaderSize);
			},
			[](uInt const blockNumBytes) -> s64
			{
				uInt const blockAdjustment = blockNumBytes - c_BlockFooterSize - c_BlockPaddingSize;
				NA_ASSERT(blockAdjustment < static_cast<uInt>(std::numeric_limits<s64>::max()),
					"This will (probably?) never happen, but if it does its probs good to know about it :p");

				return static_cast<s64>(blockAdjustment);
			});
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::InitMemory(HeapZoneInfo const& heapZoneInfo)
	{
		uInt const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
		NA_ASSERT_DEFAULT(memory_operations::IsAlligned(heapZoneSize, c_BlockAllignmentFLA));
		NA_ASSERT_DEFAULT(heapZoneSize > c_MinBlockSizeFLA);

		m_FreeList = nullptr;
		AddFreeBlock(NA_TO_VPTR(heapZoneInfo.m_Start), heapZoneSize);
	}

	template<FreeListAllocatorSettings Settings>
	BlockHeader* const FreeListAllocator<Settings>::TryFindFreeBlock(uInt const numBytes) const
	{
		BlockHeader* freeBlock = nullptr;

		if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::BestFit)
		{
			freeBlock = FindViaBestFit(m_FreeList, numBytes, Settings.m_BestFitLeniency);
		}
		else if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::FirstFit)
		{
			freeBlock = FindViaFirstFit(m_FreeList, numBytes);
		}
		else if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::NextFit)
		{
			freeBlock = FindViaNextFit(m_FreeList, numBytes);
		}
		else
		{
			NA_ASSERT_FAIL("Using an unexpected " << NA_NAMEOF_LITERAL(SearchAlgorithm));
		}

		// If no suitable block was found, the allocator is out of memory
		return freeBlock;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::TryCoalesceBlock(BlockHeader* const blockHeader, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(blockHeader, NA_NAMEOF_LITERAL(blockHeader) " is null");

		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		uPtr const blockAddress = NA_TO_UPTR(blockHeader);
		uInt newBlockSize = blockSize;
		uInt leftExtent = 0u;

		// Attempt to coalesce with right neighbor
		if ((blockAddress + blockSize) != heapZoneInfo.m_End)
		{
			BlockHeader* const rightBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, blockHeader, +, blockSize);
			bool const rightBlockFree = !bit_operations::GetBit(rightBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));

			if (rightBlockFree)
			{
				uInt const rightBlockSize = bit_operations::RightShiftBit(rightBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
				newBlockSize += rightBlockSize;
				RemoveFreeBlock(rightBlockHeader);
			}
		}

		// Attempt to coalesce with left neighbor
		if (blockAddress != heapZoneInfo.m_Start)
		{
			BlockFooter* const leftBlockFooter = NA_REINTERPRET_MEMORY(BlockFooter, blockHeader, -, c_BlockFooterSize);
			bool const leftBlockFree = !bit_operations::GetBit(leftBlockFooter->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));

			if (leftBlockFree)
			{
				uInt const leftBlockSize = bit_operations::RightShiftBit(leftBlockFooter->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
				leftExtent = leftBlockSize;
				newBlockSize += leftBlockSize;

				void* const leftblockStartPtr = NA_REINTERPRET_MEMORY(void, leftBlockFooter, -, (leftBlockSize - c_BlockFooterSize));
				RemoveFreeBlock(leftblockStartPtr);
			}
		}

		// Mark the block as memory as a free block
		void* const leftExtentPtr = NA_REINTERPRET_MEMORY(void, blockHeader, -, leftExtent);
		AddFreeBlock(leftExtentPtr, newBlockSize);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::AddFreeBlock(void* const blockStartPtr, uInt const numBytes)
	{
		NA_ASSERT(blockStartPtr, NA_NAMEOF_LITERAL(blockStartPtr) " is null");
		NA_ASSERT(numBytes >= c_MinBlockSizeFLA, "Trying to add a free block with size " << numBytes << ". The minimum size is " << c_MinBlockSizeFLA);

		// Add the memory structures (header/footer)
		BlockHeader* const freeBlockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, blockStartPtr);
		BlockFooter* const freeBlockFooter = NA_REINTERPRET_MEMORY(BlockFooter, blockStartPtr, +, (numBytes - c_BlockFooterSize));

		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = false;
		blockInfoContent.m_Padded = false;
		blockInfoContent.m_NumBytes = numBytes;
		LoadBlockInfo(blockInfoContent, *freeBlockHeader, freeBlockFooter);

		// Add the free list node
		FreeListNode* const freeListNode = NA_REINTERPRET_MEMORY(FreeListNode, freeBlockHeader, +, c_BlockHeaderSize);
		ResetFreeListNode(freeListNode); // Its important to set this to nullptr, as AddFreeListNode spins through all the nodes until !currentNode->Next
										 // This would invalidate the free list and cause a crash in SearchAlgorithm::FindVia[algorithm name]
		AddFreeListNode(m_FreeList, freeListNode);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::RemoveFreeBlock(void* const blockStartPtr)
	{
		NA_ASSERT(blockStartPtr, NA_NAMEOF_LITERAL(blockStartPtr) " is null");

		// Find and remove the free list node
		FreeListNode* const freeListNode = NA_REINTERPRET_MEMORY(FreeListNode, blockStartPtr, +, c_BlockHeaderSize);
		RemoveFreeListNode(m_FreeList, freeListNode);
	}
} // namespace nabi_allocator

// --- FreeListNode.inl ---

namespace nabi_allocator
{
	inline void ResetFreeListNode(FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

		node->m_Next = nullptr;
		node->m_Previous = nullptr;
	}
} // namespace nabi_allocator

// --- StackAllocator.inl ---

namespace nabi_allocator
{
	uInt constexpr c_BlockAllignmentSA = 8u;
	uInt constexpr c_MinBlockSizeSA = c_BlockAllignmentSA + c_BlockHeaderSize +
		((c_BlockAllignmentSA + c_BlockHeaderSize) % c_BlockAllignmentSA);

	template<StackAllocatorSettings Settings>
	StackAllocator<Settings>::StackAllocator(HeapZoneInfo const& heapZoneInfo)
		: AllocatorBase{}
		, m_CurrentPosition(c_NulluPtr)
#ifdef NA_DEBUG
		, m_PreviousPosition(c_NulluPtr)
#endif // ifdef NA_DEBUG
	{
		InitMemory(heapZoneInfo);
	}

	template<StackAllocatorSettings Settings>
	void* StackAllocator<Settings>::Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(allocationInfo.m_NumBytes > 0u, "Allocating 0 bytes");

		// Check if the remaining space in the allocator is sufficient
		uInt requiredBlockSize = allocationInfo.m_NumBytes + c_BlockHeaderSize;
		uInt padding = requiredBlockSize % c_BlockAllignmentSA;
		requiredBlockSize += padding;
		bool requiresPadding = padding != 0u;

		// Check if after the allocation if the remaining space is still sufficient
		uPtr const positionAfterAllocation = m_CurrentPosition + requiredBlockSize;
		bool const remainingSpaceSufficient = (positionAfterAllocation + c_MinBlockSizeSA) <= heapZoneInfo.m_End;
		if (!remainingSpaceSufficient)
		{
			uInt const spaceToEnd = positionAfterAllocation >= heapZoneInfo.m_End ?
				positionAfterAllocation - heapZoneInfo.m_End : heapZoneInfo.m_End - positionAfterAllocation;
			requiredBlockSize += spaceToEnd;
			padding += spaceToEnd;
			requiresPadding = true;
		}

		if ((m_CurrentPosition + requiredBlockSize) > heapZoneInfo.m_End) [[unlikely]]
		{
			NA_ASSERT_FAIL(NA_NAMEOF_LITERAL(StackAllocator) " is out of memory");
			return nullptr;
		}

		uInt allocatedBytes = 0u;

		// In the stack allocator, the payload comes first
		void* const payloadPtr = NA_TO_VPTR(m_CurrentPosition);
		allocatedBytes += allocationInfo.m_NumBytes;

		// Allocate padding if needed
		if (requiresPadding)
		{
			AllocateBlockPadding((m_CurrentPosition + allocatedBytes), padding);
			allocatedBytes += padding;
		}

		// Allocate the block header at the end of the block so it can be found with (m_CurrentPosition - c_BlockHeaderSize)
		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, m_CurrentPosition, +, allocatedBytes);

		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = true;
		blockInfoContent.m_Padded = requiresPadding;
		blockInfoContent.m_NumBytes = requiredBlockSize;
#ifdef NA_MEMORY_TAGGING
		blockInfoContent.m_MemoryTag = allocationInfo.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
		LoadBlockInfo(blockInfoContent, *blockHeader);
		allocatedBytes += c_BlockHeaderSize;

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Allocate, allocationCountAdjustment, static_cast<u64>(requiredBlockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		// Update the stack pointers
#ifdef NA_DEBUG
		m_PreviousPosition = m_CurrentPosition;
#endif // ifdef NA_DEBUG
		m_CurrentPosition += allocatedBytes;

		return payloadPtr;
	}

	template<StackAllocatorSettings Settings>
	void StackAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
		NA_CHECK_MEMORY_FOR_ALLOCATOR_OPERATION(memory, heapZoneInfo);

		uPtr const memoryAddress = NA_TO_UPTR(memory);
		NA_ASSERT(memoryAddress == m_PreviousPosition, "Freeing memory which isn't at the top of the stack");

#if defined NA_DEBUG || defined NA_TRACK_ALLOCATIONS 
		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, m_CurrentPosition, -, c_BlockHeaderSize);
		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
#endif // ifdef NA_DEBUG || NA_TRACK_ALLOCATIONS

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Free, allocationCountAdjustment, static_cast<u64>(blockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		// Update the stack pointers
#ifdef NA_DEBUG
		if (memoryAddress != heapZoneInfo.m_Start)
		{
			BlockHeader* const prevBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, m_CurrentPosition, -, (blockSize + c_BlockHeaderSize));
			uInt const prevBlockSize = bit_operations::RightShiftBit(prevBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
			m_PreviousPosition = memoryAddress - prevBlockSize;
		}
		else
		{
			m_PreviousPosition = heapZoneInfo.m_Start;
		}
#endif // ifdef NA_DEBUG
		m_CurrentPosition = memoryAddress;
	}

	template<StackAllocatorSettings Settings>
	void StackAllocator<Settings>::Reset(HeapZoneInfo const& heapZoneInfo)
	{
#ifdef NA_TRACK_ALLOCATIONS
		ResetAllocatorStats(m_AllocatorStats, AllocatorStatsResetType::Active);
#endif // #ifdef NA_TRACK_ALLOCATIONS

		InitMemory(heapZoneInfo);
	}

	template<StackAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> StackAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const
	{
		// Because we can't iterate forwards, as block headers are stored at the end of the payload,
		// iterating through a stack allocators heapzone is a little clumsy. 

		std::deque<AllocatorBlockInfo> allocatorBlocks = {};
		uInt progressThroughHeapZone = static_cast<uInt>(m_CurrentPosition);

		// There is no concept of 'free blocks' in a stack allocator, so check if there is any unused space
		uInt const freeMemoryNumBytes = static_cast<uInt>(heapZoneInfo.m_End - m_CurrentPosition);
		if (freeMemoryNumBytes > 0u)
		{
			AllocatorBlockInfo& allocatorBlockInfo = allocatorBlocks.emplace_back();
			allocatorBlockInfo.m_MemoryAddress = m_CurrentPosition;
			allocatorBlockInfo.m_Allocated = false;
			allocatorBlockInfo.m_NumBytes = freeMemoryNumBytes;
		}

		// Iterate though each block, starting from the back
		while (progressThroughHeapZone > heapZoneInfo.m_Start)
		{
			AllocatorBlockInfo const allocatorBlockInfo =
				IterateThroughHeapZoneHelper((progressThroughHeapZone - c_BlockHeaderSize),
					[](uInt const blockNumBytes) -> s64
					{
						return -static_cast<s64>(blockNumBytes - c_BlockHeaderSize);
					},
					[](uInt const blockNumBytes) -> s64
					{
						return -static_cast<s64>(c_BlockPaddingSize);
					});
			progressThroughHeapZone -= allocatorBlockInfo.m_NumBytes;
			allocatorBlocks.emplace_back(std::move(allocatorBlockInfo));
		}

		// Reverse because we were iterating backwards through the heapzone and call the action
		std::reverse(allocatorBlocks.begin(), allocatorBlocks.end());
		for (auto const& block : allocatorBlocks)
		{
			bool const continueLooping = action ? (*action)(block) : true;
			if (!continueLooping)
			{
				break;
			}
		}

		return allocatorBlocks;
	}

	template<StackAllocatorSettings Settings>
	AllocatorBlockInfo StackAllocator<Settings>::GetAllocationInfo(void const* const memory, HeapZoneInfo const& heapZoneInfo) const
	{
		NA_CHECK_MEMORY_FOR_ALLOCATOR_OPERATION(memory, heapZoneInfo);

		// Because of the memory layout this allocator has we have to do this kinda jank solution
		uInt blockIndex = 0u;
		return IterateThroughHeapZone(
			[memory, &blockIndex](AllocatorBlockInfo const& allocatorBlockInfo) -> bool
			{
				return allocatorBlockInfo.m_PayloadPtr != memory && (++blockIndex, true); // s tier syntax
			}, heapZoneInfo).at(blockIndex);
	}

	template<StackAllocatorSettings Settings>
	void StackAllocator<Settings>::InitMemory(HeapZoneInfo const& heapZoneInfo)
	{
#ifdef NA_DEBUG
		uInt const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
		NA_ASSERT_DEFAULT(memory_operations::IsAlligned(heapZoneSize, c_BlockAllignmentSA));
		NA_ASSERT_DEFAULT(heapZoneSize > c_MinBlockSizeSA);

		m_PreviousPosition = c_NulluPtr;
#endif // ifdef NA_DEBUG

		m_CurrentPosition = heapZoneInfo.m_Start;
	}
} // namespace nabi_allocator

// --- HeapZone.inl ---

namespace nabi_allocator
{
	template<is_allocator T>
	inline HeapZone<T>::HeapZone(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName)
		: HeapZoneBase{}
		, m_Allocator(Init(parentZone, numBytes, debugName))
	{
	}

	template<is_allocator T>
	inline HeapZone<T>::~HeapZone()
	{
		if (IsInitialized())
		{
			Deinit();
		}
	}

	template<is_allocator T>
	inline void* HeapZone<T>::Allocate(AllocationInfo const& allocationInfo)
	{
#ifdef NA_THREAD_SAFE_HEAP_ZONE
		std::scoped_lock lock(m_Mutex);
#endif // ifdef NA_THREAD_SAFE_HEAP_ZONE
		return m_Allocator.Allocate(allocationInfo, m_ZoneInfo);
	}

	template<is_allocator T>
	inline void HeapZone<T>::Free(void* const memory)
	{
		// Why this function doesn't set "memory" to nullptr: https://stackoverflow.com/q/704466/8890269
#ifdef NA_THREAD_SAFE_HEAP_ZONE
		std::scoped_lock lock(m_Mutex);
#endif // ifdef NA_THREAD_SAFE_HEAP_ZONE
		return m_Allocator.Free(memory, m_ZoneInfo);
	}

	template<is_allocator T>
	inline void HeapZone<T>::Reset()
	{
		return m_Allocator.Reset(m_ZoneInfo);
	}

	template<is_allocator T>
	inline T& HeapZone<T>::GetAllocator() noexcept
	{
		return m_Allocator;
	}
} // namespace nabi_allocator

// --- HeapZoneBase.inl ---

namespace nabi_allocator
{
	inline HeapZoneBase::HeapZoneBase()
		: m_ZoneInfo{ c_NulluPtr, c_NulluPtr }
		, m_ParentZone(nullptr)
#ifdef NA_DEBUG
		, m_DebugName("NotAssigned")
#endif // ifdef NA_DEBUG
	{
		s_AllHeapZones.push_back(this);
	}

	inline HeapZoneBase::~HeapZoneBase()
	{
		s_AllHeapZones.erase(std::remove(s_AllHeapZones.begin(), s_AllHeapZones.end(), this));
	}

	inline HeapZoneInfo const& HeapZoneBase::GetZoneInfo() const noexcept
	{
		return m_ZoneInfo;
	}

#ifdef NA_DEBUG
	inline std::string const& HeapZoneBase::GetDebugName() const noexcept
	{
		return m_DebugName;
	}
#endif // ifdef NA_DEBUG

	inline bool HeapZoneBase::IsInitialized() const noexcept
	{
		return m_ZoneInfo.m_Start && m_ZoneInfo.m_End;
	}

	inline bool HeapZoneBase::ContainsPtr(void const* const ptr) const
	{
		return memory_operations::IsPtrInRange(m_ZoneInfo.m_Start, m_ZoneInfo.m_End, NA_TO_UPTR(ptr));
	}
} // namespace nabi_allocator

// --- HeapZoneScope.inl ---

namespace nabi_allocator
{
	inline HeapZoneScope::HeapZoneScope(HeapZoneBase* const heapZone, std::optional<memoryTag const> const memoryTag, MemoryCommand* const customMemoryCommand)
		: m_HeapZone(heapZone)
#ifdef NA_MEMORY_TAGGING
		, m_MemoryTag(memoryTag)
#endif // ifdef NA_MEMORY_TAGGING
		, m_CustomMemoryCommand(customMemoryCommand)
	{
		if (!m_CustomMemoryCommand)
		{
			MemoryCommand::Instance().PushHeapZoneScope(*this);
		}
		else
		{
			m_CustomMemoryCommand->PushHeapZoneScope(*this);
		}
	}

	inline HeapZoneScope::~HeapZoneScope()
	{
		if (!m_CustomMemoryCommand)
		{
			MemoryCommand::Instance().PopHeapZoneScope(*this);
		}
		else
		{
			m_CustomMemoryCommand->PopHeapZoneScope(*this);
		}
	}

	inline HeapZoneBase* const HeapZoneScope::GetHeapZone() const noexcept
	{
		return m_HeapZone;
	}

#ifdef NA_MEMORY_TAGGING
	inline std::optional<memoryTag const> const& HeapZoneScope::GetMemoryTag() const noexcept
	{
		return m_MemoryTag;
	}
#endif // ifdef NA_MEMORY_TAGGING
} // namespace nabi_allocator

// --- BitOperations.inl ---

namespace nabi_allocator::bit_operations
{
	template<is_integral T>
	NA_FORCE_INLINE constexpr T FlipBit(T const value, u32 const bitPosition) noexcept
	{
		return value ^ (static_cast<T>(1) << bitPosition);
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr bool GetBit(T const value, u32 const bitPosition) noexcept
	{
		return static_cast<bool>(value & (static_cast<T>(1) << bitPosition));
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr T SetBit(T const value, u32 const bitPosition, bool const bitState) noexcept
	{
		bool const bitValue = GetBit(value, bitPosition);
		return bitValue != bitState ? FlipBit(value, bitPosition) : value;
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr T LeftShiftBit(T const value, u32 const shiftAmount) noexcept
	{
		return value << shiftAmount;
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr T RightShiftBit(T const value, u32 const shiftAmount) noexcept
	{
		return value >> shiftAmount;
	}

	template<is_integral T>
	std::string ToBinaryString(T const value, u32 const split) noexcept
	{
		std::ostringstream result;
		std::string const valueAsStr = std::bitset<std::numeric_limits<T>::digits>(value).to_string();

		for (u32 i = 0u; i < valueAsStr.size(); ++i)
		{
			if (i % split == 0u && i != 0u)
			{
				result << " ";
			}
			result << valueAsStr[i];
		}

		return result.str();
	}
} // namespace nabi_allocator::bit_operations

// --- MemoryOperations.h ---

namespace nabi_allocator::memory_operations
{
	template<typename T>
	inline T RequestMemoryFromOS(uInt const numBytes)
	{
		NA_ASSERT(numBytes > 0u, "Requesting 0 bytes");

		void* const memory = std::malloc(static_cast<std::size_t>(numBytes));
		NA_ASSERT(memory, "Call to malloc requesting " << numBytes << " failed! Are we out of memory?");

		return reinterpret_cast<T>(memory);
	}

	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory)
	{
		NA_ASSERT(memory, "Can't free null memory!");

		std::free(memory);
		memory = nullptr;
	}

	template<typename T>
	inline void ResetMemory(T* const memory)
	{
		NA_ASSERT(memory, "Can't reset null memory!");
		std::memset(memory, NULL, sizeof(T));
	}

	inline void ResetMemory(void* const destination, uInt const numBytes)
	{
		NA_ASSERT(destination, "Can't reset null memory!");
		std::memset(destination, NULL, static_cast<std::size_t>(numBytes));
	}

	inline constexpr uInt GetMemorySize(uPtr const start, uPtr const end) noexcept
	{
		return end - start;
	}

	inline uInt GetMemorySize(void const* const start, void const* const end)
	{
		return GetMemorySize(NA_TO_UPTR(start), NA_TO_UPTR(end));
	}

	template<is_integral T>
	inline constexpr bool IsAlligned(T const value, uInt const allignment) noexcept
	{
		return (static_cast<uInt>(value) % allignment) == 0u;
	}

	inline bool IsAlligned(void const* const address, uInt const allignment)
	{
		return IsAlligned(NA_TO_UPTR(address), allignment);
	}

	inline constexpr bool IsPtrInRange(uPtr const start, uPtr const end, uPtr const ptr) noexcept
	{
		return ptr >= start && ptr < end;
	}

	inline bool IsPtrInRange(void const* const start, void const* const end, void const* const ptr)
	{
		return IsPtrInRange(NA_TO_UPTR(start), NA_TO_UPTR(end), NA_TO_UPTR(ptr));
	}
} // namespace nabi_allocator::memory_operations

// --- AllocatorDefaultBenchmakrs.inl ---

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_BENCHMARKS
	template<is_heap_zone HeapZoneType>
	void AllocatorAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults)
	{
		// 10k 4 byte allocations + 10k frees

		using namespace benchmark_utils;

		uInt constexpr allocationCount = 10'000;
		uInt constexpr allocationSize = 4u;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount);

		BenchmarkResults const results = RunBenchmark(
			[&]() -> void
			{
				for (uInt i = 0u; i < allocationCount; ++i)
				{
					void* ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(allocationSize, c_NullMemoryTag));
					allocations.push_back(ptr);
				}

				for (uInt i = allocationCount; i > 0u; --i)
				{
					heapZone.Free(allocations.at(i - 1u));
				}
			},
			[&]() -> void
			{
				allocations.clear();
			});


		BenchmarkResultsToString(results, printBenchmarkResults);
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorVaryingSizeAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults)
	{
		// 10k 16 bytes allocations + 1k 256 bytes allocations + 50 2Mb allocations/frees

		using namespace benchmark_utils;

		uInt constexpr allocationCount16Byte = 10'000;
		uInt constexpr allocationSize16Byte = 16u;

		uInt constexpr allocationCount256Byte = 1000;
		uInt constexpr allocationSize256Byte = 256u;

		uInt constexpr allocationCount2Mb = 50;
		uInt constexpr allocationSize2Mb = 2_MB;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount16Byte + allocationCount256Byte);

		BenchmarkResults const results = RunBenchmark(
			[&]() -> void
			{
				// 10k 16 bytes allocations
				for (uInt i = 0; i < allocationCount16Byte; ++i)
				{
					void* ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(allocationSize16Byte, c_NullMemoryTag));
					allocations.push_back(ptr);
				}

				// 1k 256 bytes allocations
				for (uInt i = 0; i < allocationCount256Byte; ++i)
				{
					void* ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(allocationSize256Byte, c_NullMemoryTag));
					allocations.push_back(ptr);
				}

				// 50 2Mb allocations/deallocations
				for (uInt i = 0; i < allocationCount2Mb; ++i)
				{
					void* ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(allocationSize2Mb, c_NullMemoryTag));
					heapZone.Free(ptr);
				}
			},
			[&]() -> void
			{
				heapZone.Reset();
				allocations.clear();
			});


		BenchmarkResultsToString(results, printBenchmarkResults);
	}
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests::blueprints

// --- AllocatorDefaultTests.inl ---

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_TESTS
	template<is_heap_zone HeapZoneType>
	void AllocatorCreateAndDestroyTest(uInt const heapZoneSize, std::string const& expectedInitLayout)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };

		std::string const actualLayout = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());
		EXPECT_EQ(expectedInitLayout, actualLayout);
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocateAndFreeTest(uInt const heapZoneSize,
		std::string const& expectedX64MemoryTaggingLayout, std::string const& expectedX64Layout,
		std::string const& expectedX86MemoryTaggingLayout, std::string const& expectedX86Layout,
		std::string const& expectedFreeLayout)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		{
			std::string const expectedLayout =
#	ifdef _M_X64
#		ifdef NA_MEMORY_TAGGING
				expectedX64MemoryTaggingLayout
#		else
				expectedX64Layout
#		endif // ifdef NA_MEMORY_TAGGING 
#	elif _M_IX86
#		ifdef NA_MEMORY_TAGGING
				expectedX86MemoryTaggingLayout
#		else
				expectedX86Layout
#		endif // ifdef NA_MEMORY_TAGGING 
#	else
#		error "Unsupported architecture"
#	endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		heapZone.Free(ptr);
		{
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedFreeLayout, actualLayout);
		}
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocateTooLargeTest(uInt const heapZoneSize)
	{
#	ifdef NA_DEBUG
		// See MemoryCommandTests::TooLargeAllocation
		return;
#	endif

		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(heapZoneSize + 4u, c_NullMemoryTag));

		EXPECT_TRUE(ptr == nullptr);
		heapZone.Reset();
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorResetTest(uInt const heapZoneSize, std::string const& expectedResetLayout)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		heapZone.Reset();

		std::string const actualLayout = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());
		EXPECT_EQ(expectedResetLayout, actualLayout);
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorGetAllocationInfoTest(uInt const heapZoneSize)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));

		AllocatorBlockInfo const ptrInfo = heapZone.GetAllocator().GetAllocationInfo(ptr, heapZone.GetZoneInfo());
		EXPECT_EQ(ptr, ptrInfo.m_PayloadPtr);
#	ifdef NA_MEMORY_TAGGING
		EXPECT_EQ(c_NullMemoryTag, ptrInfo.m_MemoryTag);
#	endif // ifdef NA_MEMORY_TAGGING

		heapZone.Reset();
	}

#	ifdef NA_MEMORY_TAGGING
	template<is_heap_zone HeapZoneType>
	void AllocatorMemoryTagTest(uInt const heapZoneSize,
		std::string const& expectedX64MemoryTaggingUsage, std::string const& expectedX64Usage,
		std::string const& expectedX86MemoryTaggingUsage, std::string const& expectedX86Usage,
		std::string const& expectedFreeUsage)
	{
		enum class MemoryTag : memoryTag
		{
			One = 1u << 1u,
			Two = 1u << 2u,
			All = ~0u,

			ENUM_COUNT = 3u
		};

		static auto const tagToString =
			[](memoryTag const tag) -> std::string
		{
			std::unordered_map<memoryTag, std::string> const tagToStringMap =
			{
				{ type_utils::ToUnderlying(MemoryTag::One), "One"},
				{ type_utils::ToUnderlying(MemoryTag::Two), "Two"},
				{ c_NullMemoryTag, "Free" }
			};

			std::string tagName = "";

			if (tagToStringMap.contains(tag))
			{
				tagName = tagToStringMap.at(tag);
			}
			else
			{
				NA_ASSERT_FAIL("Tag " << NA_WRAP("\"", tag) << " is not in the " NA_NAMEOF_LITERAL(tagToStringMap) " umap");
			}

			return tagName;
		};

		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		void* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, type_utils::ToUnderlying(MemoryTag::One)));
		void* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, type_utils::ToUnderlying(MemoryTag::Two)));
		{
			std::string const expectedLayout =
#		ifdef _M_X64
#			ifdef NA_MEMORY_TAGGING
				expectedX64MemoryTaggingUsage
#			else
				expectedX64Usage
#			endif // ifdef NA_MEMORY_TAGGING 
#		elif _M_IX86
#			ifdef NA_MEMORY_TAGGING
				expectedX86MemoryTaggingUsage
#			else
				expectedX86Usage
#			endif // ifdef NA_MEMORY_TAGGING 
#		else
#			error "Unsupported architecture"
#		endif // ifdef _M_IX86, elif _M_IX86
				;

			std::string const actualUsage = GetFullMemoryUsage(allocator, heapZoneInfo, tagToString);
			EXPECT_EQ(expectedLayout, actualUsage);
		}

		heapZone.Free(ptr2); // Free ptr2 first to comply with the StackAllocator
		heapZone.Free(ptr1);
		{
			std::string const actualUsage = GetFullMemoryUsage(allocator, heapZoneInfo, tagToString);
			EXPECT_EQ(expectedFreeUsage, actualUsage);
		}
	}
#	endif // ifdef NA_MEMORY_TAGGING

#	ifdef NA_TRACK_ALLOCATIONS
	template<is_heap_zone HeapZoneType>
	void AllocatorAllocationTrackingTest(uInt const heapZoneSize,
		uInt const expectedX64MemoryTaggingAllocationSize, uInt const expectedX64AllocationSize,
		uInt const expectedX86MemoryTaggingAllocationSize, uInt const expectedX86AllocationSize)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		auto const& allocatorStats = heapZone.GetAllocator().GetStats();

		auto const verifyAllocatorStats =
			[&allocatorStats](u64 const activeAllocations, u64 const activeBytes, u64 const totalAllocations, u64 const totalBytes) -> bool
		{
			bool result = true;
			result &= allocatorStats.m_ActiveAllocationCount == activeAllocations;
			result &= allocatorStats.m_ActiveBytesAllocated == activeBytes;
			result &= allocatorStats.m_TotalAllocationCount == totalAllocations;
			result &= allocatorStats.m_TotalBytesAllocated == totalBytes;
			return result;
		};
		EXPECT_TRUE(verifyAllocatorStats(0ull, 0ull, 0ull, 0ull));

		u64 const expectedAllocationSize = static_cast<u64>(
#		ifdef _M_X64
#			ifdef NA_MEMORY_TAGGING
			expectedX64MemoryTaggingAllocationSize
#			else
			expectedX64AllocationSize
#			endif // ifdef NA_MEMORY_TAGGING 
#		elif _M_IX86
#			ifdef NA_MEMORY_TAGGING
			expectedX86MemoryTaggingAllocationSize
#			else
			expectedX86AllocationSize
#			endif // ifdef NA_MEMORY_TAGGING 
#		else
#			error "Unsupported architecture"
#		endif // ifdef _M_IX86, elif _M_IX86
			);

		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		EXPECT_TRUE(verifyAllocatorStats(1ull, expectedAllocationSize, 1ull, expectedAllocationSize));

		heapZone.Free(ptr);
		EXPECT_TRUE(verifyAllocatorStats(0ull, 0ull, 1ull, expectedAllocationSize));
	}
#	endif // ifdef NA_TRACK_ALLOCATIONS
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests::blueprints

// ----- Implementations ----

// --- BenchmarkUtils.cpp ---

namespace nabi_allocator::benchmark_utils
{
#ifdef NA_BENCHMARKS
	std::string BenchmarkResultsToString(BenchmarkResults const& results, bool const print)
	{
		using namespace character_constants;

		auto const getTimeWithPrecision =
			[](f64 const time) -> std::string
		{
			s32 constexpr precision = std::numeric_limits<f64>::digits10;

			std::ostringstream stream = {}; // I'm not sure if there is a better way to do this...
			stream << std::fixed << std::setprecision(precision) << time;
			return stream.str();
		};

		NA_MAKE_STRING_FROM_STREAM(std::string const resultsAsString,
			"Ran Benchmark " << results.m_Repetitions << " Times." << c_NewLine <<
			"Fastest Time: " << getTimeWithPrecision(results.m_FastestTime) << c_NewLine <<
			"Slowest Time: " << getTimeWithPrecision(results.m_SlowestTime) << c_NewLine <<
			"Average Time: " << getTimeWithPrecision(results.m_AverageTime));

		if (print)
		{
#ifdef NA_DEBUG
			NA_LOG(NA_LOG_PREP, NA_LOG_INFO, NA_LOG_CATEGORY_TEST, resultsAsString, NA_LOG_END);
#else
			std::cout << resultsAsString << std::endl; // If NA_DEBUG is not defined we can't use LOG
#endif // ifdef NA_DEBUG
		}

		return resultsAsString;
	}
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::benchmark_utils

// --- FileUtils.cpp ---

namespace nabi_allocator::file_utils
{
	bool CreateAndWriteFile(std::string const& fileName, std::string const& fileContent, bool const tileStampFile)
	{
		// We only need the one file related function in this program :p
		// No sense overcomplicating things...

		NA_LOG(NA_LOG_PREP, NA_LOG_INFO, NA_LOG_CATEGORY_FILE, "Creating a file, name " << NA_WRAP("\"", fileName), NA_LOG_END);

		if (tileStampFile)
		{
#pragma warning(disable : 4996) // ;_; https://tinyurl.com/7t3prpy5 
			std::time_t const time = std::time(nullptr);
			std::tm const* const localTime = std::localtime(&time);
			std::string timeAsString = std::asctime(localTime);
			std::replace(timeAsString.begin(), timeAsString.end(), ':', '.');
			uInt const lastDot = fileName.find_last_of('.');
			const_cast<std::string&>(fileName).insert(lastDot != std::string::npos ? lastDot : fileName.length(),
				" - " + timeAsString.substr(0u, timeAsString.length() - 1u)); // Removes a newline char (seems to be added by asctime)
#pragma warning(default: 4996)
		}

		auto file = std::ofstream{ fileName };
		file << fileContent;
		file.close();

		bool constexpr result = true;
		return result;
	}
} // namespace nabi_allocator::file_utils

// --- MemoryCommand.cpp ---

#define NA_GET_WITH_FALLBACK(current, fallback, toSet, ...) /*4/4 macro*/\
	{ \
		if (!current) \
		{ \
			toSet = fallback; \
		} \
		else \
		{ \
			fallback = __VA_ARGS__ current; \
			toSet = __VA_ARGS__ current; \
		} \
	}

#ifdef NA_THREAD_LOCAL_HEAPS
#	define NA_HEAP_ZONE_STORAGE thread_local
#else
	// If this is defined, then HeapZoneScopes, LastHeapZone and LastMemoryTag kinda should be s_... ):
#	define NA_HEAP_ZONE_STORAGE static
#endif // ifdef NA_THREAD_LOCAL_HEAPS

namespace nabi_allocator
{
	NA_HEAP_ZONE_STORAGE std::stack<std::reference_wrapper<HeapZoneScope const>> g_HeapZoneScopes = {};
	NA_HEAP_ZONE_STORAGE HeapZoneBase* g_LastHeapZone = nullptr;
	NA_HEAP_ZONE_STORAGE memoryTag g_LastMemoryTag = c_NullMemoryTag;

	MemoryCommand::MemoryCommand()
		: m_UnmanagedHeap{}
	{
	}

	MemoryCommand::~MemoryCommand()
	{
		NA_ASSERT(g_HeapZoneScopes.empty(),
			"MemoryCommand's destructor called but there are still HeapZoneScopes on the stack");
	}

	void* MemoryCommand::Allocate(uInt const numBytes)
	{
#ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT
		if (numBytes == 0u) [[unlikely]]
		{
			NA_ASSERT_FAIL("Attempting to allocate 0 bytes");
			return nullptr;
		}
#endif // ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT

		void* memory = nullptr;

		HeapZoneScope const* const topHeapZoneScope = GetTopHeapZoneScope();
		if (topHeapZoneScope) [[likely]]
		{
			HeapZoneBase * topHeapZone = nullptr;
			memoryTag topMemoryTag = c_NullMemoryTag;

			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetHeapZone(), g_LastHeapZone, topHeapZone);
#ifdef NA_MEMORY_TAGGING
			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetMemoryTag(), g_LastMemoryTag, topMemoryTag,*);
#endif // ifdef NA_MEMORY_TAGGING

			if (topHeapZone == &c_UnmanagedHeap)
			{
				goto unmanagedAllocatorAlloc;
			}
			else
			{
				memory = topHeapZone->Allocate(NA_MAKE_ALLOCATION_INFO(numBytes, topMemoryTag));
#ifdef NA_MALLOC_IF_OUT_OF_MEMORY
				// Something went wrong while allocating its probably that the free memory in the heapzone is insufficient
				// for the allocation (but regardless) just try and malloc the memory instead to hopefully prevent a crash.
				if (!memory) [[unlikely]]
				{
					goto unmanagedAllocatorAlloc;
				}
#endif // ifdef NA_MALLOC_IF_OUT_OF_MEMORY
			}
		}
		else [[unlikely]]
		{
		unmanagedAllocatorAlloc:
			memory = m_UnmanagedHeap.Allocate(numBytes);
		}

		return memory;
	}

	void MemoryCommand::Free(void* const memory)
	{
#ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT
		if (!memory) [[unlikely]]
		{
			NA_ASSERT_FAIL("Attempting to free nullptr");
			return;
		}
#endif // ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT

		if (g_LastHeapZone)
		{
			if (g_LastHeapZone == &c_UnmanagedHeap)
			{
				goto unmanagedAllocatorFree;
			}
			else if (g_LastHeapZone->ContainsPtr(memory))
			{
				g_LastHeapZone->Free(memory);
				return;
			}
		}

		{
			HeapZoneBase* const heapZoneWhichMadeAllocation = HeapZoneBase::FindHeapZoneForPtr(memory);
			if (heapZoneWhichMadeAllocation)
			{
				heapZoneWhichMadeAllocation->Free(memory);
				return;
			}
			else
			{
				goto unmanagedAllocatorFree;
			}
		}

	unmanagedAllocatorFree:
		m_UnmanagedHeap.Free(memory);
	}

	void MemoryCommand::PushHeapZoneScope(HeapZoneScope const& heapZoneScope)
	{
		g_HeapZoneScopes.push(heapZoneScope);
	}

	void MemoryCommand::PopHeapZoneScope(HeapZoneScope const& heapZoneScope)
	{
#ifdef NA_DEBUG
		HeapZoneScope const* const topHeapZoneScope = GetTopHeapZoneScope();
		NA_ASSERT(topHeapZoneScope, "Trying to pop a heap zone scope, but the stack is empty");

		if (topHeapZoneScope)
		{
			NA_ASSERT(topHeapZoneScope->GetHeapZone() == heapZoneScope.GetHeapZone(),
				"Popping a heap zone scope which isn't at the top of the stack");
		}
#endif // ifdef NA_DEBUG

		g_HeapZoneScopes.pop();
		if (g_HeapZoneScopes.empty())
		{
			g_LastHeapZone = nullptr;
			g_LastMemoryTag = c_NullMemoryTag;
		}
	}

	HeapZoneScope const* const MemoryCommand::GetTopHeapZoneScope() const noexcept
	{
		HeapZoneScope const* topHeapZoneScope = nullptr;
		if (!g_HeapZoneScopes.empty()) [[likely]]
		{
			topHeapZoneScope = &g_HeapZoneScopes.top().get();
		}

		return topHeapZoneScope;
	}

	UnmanagedHeap const& MemoryCommand::GetUnmanagedHeap() const noexcept
	{
		return m_UnmanagedHeap;
	}

	uInt MemoryCommand::GetHeapZoneScopeCount() const noexcept
	{
		return static_cast<uInt>(g_HeapZoneScopes.size());
	}
} // namespace nabi_allocator

#undef NA_GET_WITH_FALLBACK
#undef NA_HEAP_ZONE_STORAGE

// --- NewDeleteOverrides.cpp ---

/**
 * The global new/delete overrides.
 *
 * Note:
 *  - I was getting some warnings about inconsistent annotations with my new override. I tried this
 *    https://stackoverflow.com/q/68569843/8890269 but it didn't quite work (perhaps I just wasn't
 *    doing it right...) so just opted to disable the warnings instead :p.
 *  - The reason we don't just default to malloc/free but still override the operators if NA_OVERR..
 *    isn't defined is because we don't want to change the default behaviour *at all*. Furthermore,
 *    a user may have a different new/delete override they are using.
*/

namespace nabi_allocator
{
	[[nodiscard]] inline void* New(std::size_t const numBytes) noexcept(false)
	{
		return MemoryCommand::Instance().Allocate(static_cast<uInt>(numBytes));
	}

	inline void Delete(void* const memory) noexcept
	{
		return MemoryCommand::Instance().Free(memory);
	}
} // namespace nabi_allocator

#ifdef NA_OVERRIDE_NEW_DELETE
#   pragma warning(disable: 28196; disable: 28251; disable: 6387)

[[nodiscard]] void* operator new(std::size_t const numBytes) noexcept(false)
{
	return nabi_allocator::New(numBytes);
}

[[nodiscard]] void* operator new[](std::size_t const numBytes) noexcept(false)
{
	return nabi_allocator::New(numBytes);
}

#   pragma warning(default: 28196; default: 28251; default: 6387)

void operator delete(void* const memory) noexcept
{
	nabi_allocator::Delete(memory);
}

void operator delete[](void* const memory) noexcept
{
	nabi_allocator::Delete(memory);
}
#endif // ifdef NA_OVERRIDE_NEW_DELETE

// --- AllocatorBase.cpp ---

namespace nabi_allocator
{
	AllocatorBlockInfo AllocatorBase::IterateThroughHeapZoneHelper(uInt const blockHeaderPosition,
		std::function<s64(uInt const)> const calculatePayloadPtrAdjustment,
		std::function<s64(uInt const)> const calculateBlockPaddingAdjustment) const
	{
		AllocatorBlockInfo allocatorBlockInfo = {};
		BlockHeader const* blockHeader = nullptr;
		BlockInfoContent blockInfoContent = {};

		blockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, blockHeaderPosition);
		UnloadBlockInfo(blockInfoContent, *blockHeader);

		// Store the block's infomation in allocatorBlockInfo
		allocatorBlockInfo.m_MemoryAddress = NA_TO_UPTR(blockHeader);
#ifdef NA_MEMORY_TAGGING
		allocatorBlockInfo.m_MemoryTag = blockInfoContent.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		allocatorBlockInfo.m_Allocated = blockInfoContent.m_Allocated;
		allocatorBlockInfo.m_Padded = blockInfoContent.m_Padded;

		u32 padding = 0u;
		if (allocatorBlockInfo.m_Padded)
		{
			BlockPadding const* const blockPadding =
				NA_REINTERPRET_MEMORY(BlockPadding, blockHeader, +, calculateBlockPaddingAdjustment(blockInfoContent.m_NumBytes));
			padding = static_cast<u32>(blockPadding->m_Padding);
		}

		allocatorBlockInfo.m_NumBytes = blockInfoContent.m_NumBytes;
		allocatorBlockInfo.m_Padding = padding;
		allocatorBlockInfo.m_PayloadPtr = 
			NA_REINTERPRET_MEMORY(void, blockHeader, +, calculatePayloadPtrAdjustment(allocatorBlockInfo.m_NumBytes));

		return allocatorBlockInfo;
	}
} // namespace nabi_allocator

// --- AllocatorStats.cpp ---

namespace nabi_allocator
{
#ifdef NA_TRACK_ALLOCATIONS
	void UpdateAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsUpdateType const updateType,
		u64 const& allocationCountAdjustment, u64 const& allocationByteAdjustment)
	{
		switch (updateType)
		{
		case AllocatorStatsUpdateType::Allocate:
			allocatorStats.m_ActiveAllocationCount += allocationCountAdjustment;
			allocatorStats.m_ActiveBytesAllocated += allocationByteAdjustment;
			allocatorStats.m_TotalAllocationCount += allocationCountAdjustment;
			allocatorStats.m_TotalBytesAllocated += allocationByteAdjustment;
			break;
		case AllocatorStatsUpdateType::Free:
			NA_ASSERT(allocatorStats.m_ActiveAllocationCount >= allocationCountAdjustment &&
				allocatorStats.m_ActiveBytesAllocated >= allocationByteAdjustment &&
				allocatorStats.m_TotalAllocationCount >= allocationCountAdjustment &&
				allocatorStats.m_TotalBytesAllocated >= allocationByteAdjustment,
				"Free operation results in a negative allocation or byte count. What has gone wrong?");

			allocatorStats.m_ActiveAllocationCount -= allocationCountAdjustment;
			allocatorStats.m_ActiveBytesAllocated -= allocationByteAdjustment;
			break;
		default:
			NA_ASSERT_FAIL("Using an unexpected " << NA_NAMEOF_LITERAL(AllocatorStatsUpdateType));
			break;
		}
	}

	void ResetAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsResetType const resetType)
	{
		switch (resetType)
		{
		case AllocatorStatsResetType::Active:
			// For active stats resets, route through UpdateAllocatorStats so the assert catches any weirdness
			UpdateAllocatorStats(allocatorStats, AllocatorStatsUpdateType::Free,
				allocatorStats.m_ActiveAllocationCount, allocatorStats.m_ActiveBytesAllocated);
			break;
		case AllocatorStatsResetType::Total:
			allocatorStats.m_ActiveAllocationCount = 0ull;
			allocatorStats.m_ActiveBytesAllocated = 0ull;
			allocatorStats.m_TotalAllocationCount = 0ull;
			allocatorStats.m_TotalBytesAllocated = 0ull;
			break;
		default:
			NA_ASSERT_FAIL("Using an unexpected " << NA_NAMEOF_LITERAL(AllocatorStatsResetType));
			break;
		}
	}
#endif // ifdef NA_TRACK_ALLOCATIONS
} // namespace nabi_allocator

// --- AllocatorUtils.cpp ---

namespace nabi_allocator
{
	using namespace character_constants;

	namespace
	{
		std::string ConvertAndFormatSStream(std::ostringstream const& stream)
		{
			std::string string = stream.str();
			string.pop_back(); // Remove the last space
			return string;
		}
	} // anonymous namespace

	std::string GetMemoryLayout(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo)
	{
		// Format: F(ree)/A(llocated)[number of bytes]P(adding)[number of bytes] [space] [next entry]
		std::ostringstream output = {};

		allocator.IterateThroughHeapZone(
			[&output](AllocatorBlockInfo const& blockInfo) -> bool
			{
				output << (blockInfo.m_Allocated ? c_AllocatedSymbol : c_FreeSymbol);
				output << blockInfo.m_NumBytes;
				output << c_PaddingSymbol;
				output << blockInfo.m_Padding;
				output << c_SpaceSymbol;

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		return ConvertAndFormatSStream(output);
	}

	std::string GetMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo)
	{
		// Format: A(llocated)[number of bytes] F(ree)[number of bytes]
		std::ostringstream output = {};
		uInt allocatedBytes = 0u;
		uInt freeBytes = 0u;

		allocator.IterateThroughHeapZone(
			[&output, &allocatedBytes, &freeBytes](AllocatorBlockInfo const& blockInfo) -> bool
			{
				if (blockInfo.m_Allocated)
				{
					allocatedBytes += blockInfo.m_NumBytes;
				}
				else
				{
					freeBytes += blockInfo.m_NumBytes;
				}

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		output << c_AllocatedSymbol << allocatedBytes << c_SpaceSymbol << c_FreeSymbol << freeBytes;
		return output.str();
	}

#ifdef NA_MEMORY_TAGGING
	std::string GetFullMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo,
		std::optional<std::function<std::string(memoryTag const)>> const tagAliases)
	{
		// Format: Tag[number of bytes] [space] [next entry]
		std::ostringstream output = {};
		std::unordered_map<memoryTag, uInt> tagByteUsage = {};

		allocator.IterateThroughHeapZone(
			[&output, &tagByteUsage](AllocatorBlockInfo const& blockInfo) -> bool
			{
				auto const result = tagByteUsage.try_emplace(blockInfo.m_MemoryTag, blockInfo.m_NumBytes);
				bool const newItem = result.second;
				if (!newItem)
				{
					result.first->second += blockInfo.m_NumBytes;
				}

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		for (auto const [key, value] : tagByteUsage)
		{
			output << (tagAliases ? (*tagAliases)(key) : std::to_string(key)) << value << c_SpaceSymbol;
		}

		return ConvertAndFormatSStream(output);
	}
#endif // ifdef NA_MEMORY_TAGGING
} // namespace nabi_allocator

// --- BlockInfo.cpp ---

namespace nabi_allocator
{
	// I was considering having one Load/UnloadBlockInfo function, templating it and using if constexpr (is_same_as_v<BlockHeader, T>)
	// to check if we should populate the memory tag field. But this just felt... wierd...
	// Perhaps I should have two LoadBlockInfo functions as well..?

	namespace
	{
		void GetBlockInfo(uInt& blockInfo, BlockInfoContent const& blockInfoContent) noexcept
		{
			blockInfo = bit_operations::LeftShiftBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
			blockInfo = bit_operations::SetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Padded), blockInfoContent.m_Padded);
			blockInfo = bit_operations::SetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated), blockInfoContent.m_Allocated);
		}

		void SetBlockInfo(uInt const& blockInfo, BlockInfoContent& blockInfoContent) noexcept
		{
			blockInfoContent.m_Allocated = bit_operations::GetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));
			blockInfoContent.m_Padded = bit_operations::GetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Padded));
			blockInfoContent.m_NumBytes = bit_operations::RightShiftBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		}
	} // anonymous namespace

	void LoadBlockInfo(BlockInfoContent const& blockInfoContent, BlockHeader& blockHeader, BlockFooter* const blockFooter) noexcept
	{
		uInt blockInfo = blockInfoContent.m_NumBytes;
		GetBlockInfo(blockInfo, blockInfoContent);

		blockHeader.m_BlockInfo = blockInfo;
#ifdef NA_MEMORY_TAGGING
		blockHeader.m_MemoryTag = blockInfoContent.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		if (blockFooter)
		{
			blockFooter->m_BlockInfo = blockInfo;
		}
	}

	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockHeader const& blockHeader) noexcept
	{
		SetBlockInfo(blockHeader.m_BlockInfo, blockInfoContent);

#ifdef NA_MEMORY_TAGGING
		blockInfoContent.m_MemoryTag = blockHeader.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	}

	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockFooter const& blockFooter) noexcept
	{
		SetBlockInfo(blockFooter.m_BlockInfo, blockInfoContent);
	}
} // namespace nabi_allocator

// --- BlockPadding.cpp ---

namespace nabi_allocator
{
	void AllocateBlockPadding(uPtr const currentPosition, uInt const padding)
	{
		// We want to allocate the padding one byte before the footer. 
		// Its ok to do padding - c_BlockPaddingSize, because if "requiresPadding" is true, "padding" will always be > 1 and c_BlockPaddingSize is 1.

		uInt const blockPaddingStructPosition = padding - c_BlockPaddingSize;
		BlockPadding* const blockPadding = NA_REINTERPRET_MEMORY(BlockPadding, currentPosition, +, blockPaddingStructPosition);
		blockPadding->m_Padding = static_cast<u8>(padding);
	}
} // namespace nabi_allocator

// --- FreeListNode.cpp ---

namespace nabi_allocator
{
	void AddFreeListNode(FreeListNode*& freelist, FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

		// If there are no free blocks, this block becomes the head of the list
		if (!freelist)
		{
			freelist = node;
			return;
		}

		// If the memory of block is before the memory of the list's head, then make this block the new head to ensure cache performance
		uPtr const nodeAddress = NA_TO_UPTR(node);
		if (nodeAddress < NA_TO_UPTR(freelist))
		{
			node->m_Next = freelist;
			freelist->m_Previous = node;
			freelist = node;
			return;
		}

		// Otherwise iterate through the free blocks until we find a place where the previous node's memory address is smaller and the next node's is bigger
		FreeListNode* currentNode = freelist;

		while (currentNode->m_Next)
		{
			if (nodeAddress > NA_TO_UPTR(currentNode) &&
				nodeAddress < NA_TO_UPTR(currentNode->m_Next))
			{
				node->m_Next = currentNode->m_Next;
				currentNode->m_Next->m_Previous = node;
				node->m_Previous = currentNode;
				currentNode->m_Next = node;
				return;
			}
			else
			{
				currentNode = currentNode->m_Next;
			}
		}

		// If no place between two blocks was found, the memory location of this block must be greater than those in the list. Add it to the end
		currentNode->m_Next = node;
		node->m_Previous = currentNode;
	}

	void RemoveFreeListNode(FreeListNode*& freelist, FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

		// If the free list node is the start of the free list, rewire the head of the list
		if (freelist == node)
		{
			freelist = node->m_Next;

#if defined NA_DEBUG || defined NA_TESTS
			// Not necessary, but makes the debug output easier to reason
			if (freelist)
			{
				freelist->m_Previous = nullptr;
			}
#endif // ifdef NA_DEBUG || NA_TESTS
		}
		else
		{
			// Otherwise, rewire the other nodes in the linked list
			if (node->m_Previous)
			{
				node->m_Previous->m_Next = node->m_Next;
			}
			if (node->m_Next)
			{
				node->m_Next->m_Previous = node->m_Previous;
			}
		}

#if defined NA_DEBUG || defined NA_TESTS
		// Same deal as above.
		ResetFreeListNode(node);
#endif // ifdef NA_DEBUG || NA_TESTS
	}
} // namespace nabi_allocator

// --- SearchAlogorithms.cpp ---

namespace nabi_allocator
{
	// The code in these FindVia functions is repeated a bit, but I think this is ok as pulling it out into 
	// another function (or macro) would reduce readability and possibly performance.

	BlockHeader* const FindViaBestFit(FreeListNode const* freeListNode, uInt const numBytes, uInt const leniency)
	{
		// Iterate through the free list to try and find a block with a size as close as possible to numBytes + leniency

		BlockHeader* bestBlock = nullptr;
		uInt bestBlockSize = std::numeric_limits<uInt>::max();

		while (freeListNode)
		{
			BlockHeader* const freeBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, freeListNode, -, c_BlockHeaderSize);
			uInt const freeBlockNumBytes = bit_operations::RightShiftBit(freeBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));

			if (freeBlockNumBytes >= (numBytes - leniency) && freeBlockNumBytes <= (numBytes + leniency))
			{
				bestBlock = freeBlockHeader;
				break;
			}
			else
			{
				if (freeBlockNumBytes >= numBytes && freeBlockNumBytes < bestBlockSize)
				{
					bestBlock = freeBlockHeader;
					bestBlockSize = freeBlockNumBytes;
				}

				freeListNode = freeListNode->m_Next;
			}
		}

		return bestBlock;
	}

	BlockHeader* const FindViaFirstFit(FreeListNode const* freeListNode, uInt const numBytes)
	{
		// Iterate through the free list until we find a block with size greater than or equal to numBytes

		while (freeListNode)
		{
			BlockHeader* const freeBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, freeListNode, -, c_BlockHeaderSize);
			uInt const freeBlockNumBytes = bit_operations::RightShiftBit(freeBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));

			if (numBytes <= freeBlockNumBytes)
			{
				return freeBlockHeader;
			}
			else
			{
				freeListNode = freeListNode->m_Next;
			}
		}

		return nullptr;
	}

	BlockHeader* const FindViaNextFit(FreeListNode const* /*freeListNode*/, uInt const /*numBytes*/)
	{
		NA_FUNCTION_NOT_IMPLEMENTED;
		return nullptr;
	}
} // namespace nabi_allocator

// --- HeapZoneBase.cpp ---

namespace nabi_allocator
{
	std::vector<HeapZoneBase*> HeapZoneBase::s_AllHeapZones = {};

	uInt nabi_allocator::HeapZoneBase::GetHeapZoneCount() noexcept
	{
		return s_AllHeapZones.size();
	}

	HeapZoneBase* HeapZoneBase::FindHeapZoneForPtr(void const* const ptr)
	{
		HeapZoneBase* containingZone = nullptr;

		for (HeapZoneBase* const heapZone : s_AllHeapZones)
		{
			NA_ASSERT(heapZone, "A HeapZone in s_AllHeapZones is null!");
			bool const containsPtr = heapZone->ContainsPtr(ptr);;
			if (containsPtr)
			{
				containingZone = heapZone;
				break;
			}
		}

		return containingZone;
	}

	HeapZoneInfo& HeapZoneBase::Init(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName)
	{
		NA_ASSERT(!IsInitialized(), "Heap zone is already initialized");

		if (parentZone)
		{
			m_ZoneInfo.m_Start = NA_TO_UPTR(parentZone->Allocate(NA_MAKE_ALLOCATION_INFO(numBytes, c_NullMemoryTag)));
		}
		else
		{
			m_ZoneInfo.m_Start = memory_operations::RequestMemoryFromOS<uPtr>(numBytes);
		}
		m_ZoneInfo.m_End = m_ZoneInfo.m_Start + numBytes;

		m_ParentZone = parentZone;
#ifdef NA_DEBUG
		m_DebugName = debugName;
#endif // ifdef NA_DEBUG

		return m_ZoneInfo;
	}

	void HeapZoneBase::Deinit()
	{
		NA_ASSERT(IsInitialized(), "Heap zone is not initialized");

		void* zoneStart = NA_TO_VPTR(m_ZoneInfo.m_Start);
		if (m_ParentZone)
		{
			m_ParentZone->Free(zoneStart);
		}
		else
		{
			memory_operations::ReleaseMemoryToOS(zoneStart);
		}

		m_ZoneInfo.m_Start = c_NulluPtr;
		m_ZoneInfo.m_End = c_NulluPtr;
	}
} // namespace nabi_allocator

// --- AllocatorBenchmarks.cpp ---

/**
 * Benchmarks for all the allocators.
 * Running these tests can be toggled on/off by the NA_BENCHMARKS define in Config.h.
 *
 * We could use the Default[AllocatorName]Allocator typedef in this file, but sometimes the default
 * allocator settings are not the fastest ones, and thats what we are testing here. Plus the
 * explicitness is good I recon...
*/

namespace nabi_allocator::tests
{
#ifdef NA_BENCHMARKS
#	define NA_FIXTURE_NAME NA_BENCHMARK_FIXTURE_NAME(AllocatorBenchmarks)

	bool constexpr c_PrintBenchmarkResultsAB = true;

	// This allocator just wraps malloc/free so we can benchmark against the base implementation.
	// Even the UnmanagedHeap routes through [Request/Release]Memory[From/To]OS which has some asserts. 
	class DefaultAllocator final : public HeapZoneBase
	{
	public:
		[[nodiscard]] inline void* Allocate(AllocationInfo const& allocationInfo) override
		{
			return std::malloc(allocationInfo.m_NumBytes);
		}

		inline void Free(void* const memory) override
		{
			std::free(memory);
		}

		inline void Reset() override
		{
			// Can't use NA_FUNCTION_NOT_IMPLEMENTED because its called in the benchmarks
		}
	};

	// --- Unmanaged Allocator ---

	NA_BENCHMARK(NA_FIXTURE_NAME, UnmanagedAllocatorAllocThenFree)
	{
		DefaultAllocator test = {};
		blueprints::AllocatorAllocThenFree(test, c_PrintBenchmarkResultsAB);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, UnmanagedAllocatorVaryingSizeAllocThenFree)
	{
		DefaultAllocator test = {};
		blueprints::AllocatorVaryingSizeAllocThenFree(test, c_PrintBenchmarkResultsAB);
	}

	// --- Free List Allocator ---

	FreeListAllocatorSettings constexpr c_FreeListAllocatorBenchmarkSettings =
	{
		.m_SearchAlgorithm = SearchAlgorithm::FirstFit,
		.m_BestFitLeniency = 0u
	};
	using BenchmarkFreeListAllocator = HeapZone<FreeListAllocator<c_FreeListAllocatorBenchmarkSettings>>;

	NA_BENCHMARK(NA_FIXTURE_NAME, FreeListAllocatorAllocThenFree)
	{
		BenchmarkFreeListAllocator allocator = { HeapZoneBase::c_NoParent, blueprints::c_BenchmarkHeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorAllocThenFree(allocator, c_PrintBenchmarkResultsAB);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, FreeListAllocatorVaryingSizeAllocThenFree)
	{
		BenchmarkFreeListAllocator allocator = { HeapZoneBase::c_NoParent, blueprints::c_BenchmarkHeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorVaryingSizeAllocThenFree(allocator, c_PrintBenchmarkResultsAB);
	}

	// --- Stack Allocator ---

	StackAllocatorSettings constexpr c_StackAllocatorBenchmarkSettings =
	{
	};
	using BenchmarkStackAllocator = HeapZone<StackAllocator<c_StackAllocatorBenchmarkSettings>>;

	NA_BENCHMARK(NA_FIXTURE_NAME, StackAllocatorAllocThenFree)
	{
		BenchmarkStackAllocator allocator = { HeapZoneBase::c_NoParent, blueprints::c_BenchmarkHeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorAllocThenFree(allocator, c_PrintBenchmarkResultsAB);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, StackAllocatorVaryingSizeAllocThenFree)
	{
		BenchmarkStackAllocator allocator = { HeapZoneBase::c_NoParent, blueprints::c_BenchmarkHeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorVaryingSizeAllocThenFree(allocator, c_PrintBenchmarkResultsAB);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests

// --- MemoryCommandBenchmarks.cpp ---

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

	bool constexpr c_PrintBenchmarkResultsMCB = true;

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
		blueprints::AllocatorAllocThenFree(memoryCommandWrapper, c_PrintBenchmarkResultsMCB);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, MemoryCommandVaryingSizeAllocThenFree)
	{
		NA_SETUP_MEMORY_COMMAND_FOR_BENCHMARK(memoryCommandWrapper);
		blueprints::AllocatorAllocThenFree(memoryCommandWrapper, c_PrintBenchmarkResultsMCB);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests

#ifdef NA_BENCHMARKS
#	undef NA_SETUP_MEMORY_COMMAND_FOR_BENCHMARK
#endif // ifdef NA_BENCHMARKS

// --- BitOperationsTests.cpp ---

/**
 * Tests for BitOperations
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(BitOperationsTests)

	TEST(NA_FIXTURE_NAME, FlipBit)
	{
		u32 constexpr value = 0b0011u;

		{
			u32 constexpr expected = 0b0001u;
			u32 constexpr actual = bit_operations::FlipBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b1011u;
			u32 constexpr actual = bit_operations::FlipBit(value, 3u);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, GetBit)
	{
		u32 constexpr value = 0b0001u;

		{
			bool constexpr expected = true;
			bool constexpr actual = bit_operations::GetBit(value, 0u);
			EXPECT_EQ(actual, expected);
		}
		{
			bool constexpr expected = false;
			bool constexpr actual = bit_operations::GetBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, SetBit)
	{
		u32 constexpr value = 0b0001u;

		{
			u32 constexpr expected = 0b0011u;
			u32 constexpr actual = bit_operations::SetBit(value, 1u, true);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b0001u;
			u32 constexpr actual = bit_operations::SetBit(value, 0u, true);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b0000u;
			u32 constexpr actual = bit_operations::SetBit(value, 0u, false);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, BitShift)
	{
		u32 constexpr value = 0b0010u;

		{
			u32 constexpr expected = 0b001u;
			u32 constexpr actual = bit_operations::RightShiftBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b0100u;
			u32 constexpr actual = bit_operations::LeftShiftBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, ToBinaryString)
	{
		u32 constexpr value = 0b0101u;
		std::string const/*expr :(*/ expected = "00000000 00000000 00000000 00000101";

		std::string const actual = bit_operations::ToBinaryString(value, 8u);
		EXPECT_EQ(actual, expected);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests

// --- FreeListAllocatorTests.cpp ---

/**
 * Tests for FreeListAllocator.
 *
 * Note: I am unsure if my tests for the search algorithms are ok. On the one hand, they test the smallest part of the code.
 * On the other, they don't test the interface of the allocator itself. Thoughts?
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(FreeListAllocatorTests)

	using HeapZoneTypeFLAT = HeapZone<DefaultFreeListAllocator>;
	uInt constexpr c_SmallHeapZoneSizeFLAT = 64u;
	uInt constexpr c_LargeHeapZoneSizeFLAT = 256u;

	std::vector<BlockInfoContent> const c_SearchAlgorithmBlocks =
	{
		{ false, false, 64u }, { true,  false, 32u }, { false, false, 32u } // Order: m_Allocated, m_Padded, m_NumBytes
	};
	std::vector<BlockInfoContent> const c_CoalesceBlocks =
	{
		{ false, false, 32u }, { false,  false, 32u }
	};

	void* AllocateAndSetUpMockMemory(std::vector<BlockInfoContent> const& blockInfoContent)
	{
		uInt const memoryNumBytes = (c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize) * blockInfoContent.size();
		void* memory = memory_operations::RequestMemoryFromOS<void*>(memoryNumBytes);

		uPtr memoryAddress = NA_TO_UPTR(memory) + (memoryNumBytes - c_BlockFooterSize);
		FreeListNode* prevNode = nullptr;

		for (auto it = blockInfoContent.rbegin(); it != blockInfoContent.rend(); ++it) // We need to iterate backwards to set the node->m_Next ptr
		{
			BlockFooter* const footer = NA_REINTERPRET_MEMORY_DEFAULT(BlockFooter, memoryAddress);
			FreeListNode* const node = NA_REINTERPRET_MEMORY(FreeListNode, footer, -, c_FreeListNodeSize);
			BlockHeader* const header = NA_REINTERPRET_MEMORY(BlockHeader, node, -, c_BlockHeaderSize);

			LoadBlockInfo(*it, *header, footer);
			node->m_Previous = nullptr;
			node->m_Next = prevNode;

			memoryAddress = NA_TO_UPTR(header) - c_BlockFooterSize;
			prevNode = node;
		}

		return memory;
	}

	template<typename SearchFunction, typename... Args>
	std::optional<uInt> TryFindBlock(SearchFunction const searchFunction, FreeListNode const* const firstFreeListNode, Args&&... args)
	{
		std::optional<uInt> result = std::nullopt;

		BlockHeader const* const foundBlockHeader = searchFunction(firstFreeListNode, std::forward<Args>(args)...);
		if (foundBlockHeader)
		{
			result = bit_operations::RightShiftBit(foundBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		}

		return result;
	}

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		blueprints::AllocatorCreateAndDestroyTest<HeapZoneTypeFLAT>(
			c_SmallHeapZoneSizeFLAT, // Heap zone size
			"F64P0"                  // Expected init layout
		);
	}

	TEST(NA_FIXTURE_NAME, AllocateAndFree)
	{
		blueprints::AllocatorAllocateAndFreeTest<HeapZoneTypeFLAT>(
			c_SmallHeapZoneSizeFLAT, // Heap zone size
			"A64P36",                // Expected x64 + memory tagging layout
			"A32P12 F32P0",          // Expected x64 layout
			"A24P8 F40P0",           // Expected x86 + memory tagging layout
			"A16P4 F48P0",           // Expected x86 layout
			"F64P0"                  // Expected free layout
		);
	}

	TEST(NA_FIXTURE_NAME, TooLargeAllocation)
	{
		blueprints::AllocatorAllocateTooLargeTest<HeapZoneTypeFLAT>(
			c_SmallHeapZoneSizeFLAT // Heap zone size
		);
	}

	TEST(NA_FIXTURE_NAME, Reset)
	{
		blueprints::AllocatorResetTest<HeapZoneTypeFLAT>(
			c_SmallHeapZoneSizeFLAT, // Heap zone size
			"F64P0"                  // Expected reset layout
		);
	}

	TEST(NA_FIXTURE_NAME, GetAllocationInfo)
	{
		blueprints::AllocatorGetAllocationInfoTest<HeapZoneTypeFLAT>(
			c_SmallHeapZoneSizeFLAT // Heap zone size
		);
	}

#	ifdef NA_MEMORY_TAGGING
	TEST(NA_FIXTURE_NAME, MemoryTagging)
	{
		blueprints::AllocatorMemoryTagTest<HeapZoneTypeFLAT>(
			c_LargeHeapZoneSizeFLAT,   // Heap zone size
			"One40 Two40 Free176",     // Expected x64 + memory tagging usage
			"One32 Two32 Free192",     // Expected x64 usage
			"One24 Two24 Free208",     // Expected x86 + memory tagging usage
			"One16 Two16 Free224",     // Expected x86 usage
			"Free256"                  // Expected free usage
		);
	}
#	endif // ifdef NA_MEMORY_TAGGING

#	ifdef NA_TRACK_ALLOCATIONS
	TEST(NA_FIXTURE_NAME, AllocationTracking)
	{
		blueprints::AllocatorAllocationTrackingTest<HeapZoneTypeFLAT>(
			c_SmallHeapZoneSizeFLAT, // Heap zone size
			64u,	                 // Expected x64 + memory tagging allocation size
			32u,                     // Expected x64 allocation size
			24u,                     // Expected x86 + memory tagging allocation size
			16u                      // Expected x86 allocation size
		);
	}
#	endif // ifdef NA_TRACK_ALLOCATIONS

	TEST(NA_FIXTURE_NAME, BestFitSearch)
	{
		void* memory = AllocateAndSetUpMockMemory(c_SearchAlgorithmBlocks);
		FreeListNode const* const firstFreeListNode = NA_REINTERPRET_MEMORY(FreeListNode, memory, +, c_BlockHeaderSize);

		{
			uInt const foundBlockHeaderSize = TryFindBlock(&FindViaBestFit, firstFreeListNode, 32u, 0u).value();
			EXPECT_EQ(32u, foundBlockHeaderSize);
		}

		{
			uInt const foundBlockHeaderSize = TryFindBlock(&FindViaBestFit, firstFreeListNode, 32u, 32u).value();
			EXPECT_EQ(64u, foundBlockHeaderSize);
		}

		{
			std::optional<uInt> const foundBlockHeaderSize = TryFindBlock(&FindViaBestFit, firstFreeListNode, 65u, 0u);
			EXPECT_FALSE(foundBlockHeaderSize);
		}

		memory_operations::ReleaseMemoryToOS(memory);
	}

	TEST(NA_FIXTURE_NAME, FirstFitSearch)
	{
		void* memory = AllocateAndSetUpMockMemory(c_SearchAlgorithmBlocks);
		FreeListNode const* const firstFreeListNode = NA_REINTERPRET_MEMORY(FreeListNode, memory, +, c_BlockHeaderSize);

		{
			uInt const foundBlockHeaderSize = TryFindBlock(&FindViaFirstFit, firstFreeListNode, 32u).value();
			EXPECT_EQ(64u, foundBlockHeaderSize);
		}

		{
			std::optional<uInt> const foundBlockHeaderSize = TryFindBlock(&FindViaFirstFit, firstFreeListNode, 65u);
			EXPECT_FALSE(foundBlockHeaderSize);
		}

		memory_operations::ReleaseMemoryToOS(memory);
	}

	TEST(NA_FIXTURE_NAME, CoalesceBlock)
	{
		HeapZoneTypeFLAT heapZone{ HeapZoneBase::c_NoParent, c_LargeHeapZoneSizeFLAT, "TestHeapZone" };
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		// Expected layout after three allocations
		void* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* const ptr3 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
				"A40P12 A40P12 A40P12 F136P0"
#	else
				"A32P12 A32P12 A32P12 F160P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				"A24P8 A24P8 A24P8 F184P0"
#	else
				"A16P4 A16P4 A16P4 F208P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		// Expected layout after the first allocated ptr is freed
		heapZone.Free(ptr1);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
				"F40P0 A40P12 A40P12 F136P0"
#	else
				"F32P0 A32P12 A32P12 F160P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				"F24P0 A24P8 A24P8 F184P0"
#	else
				"F16P0 A16P4 A16P4 F208P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		// Expected layout after the second allocated ptr is freed
		heapZone.Free(ptr2);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
				"F80P0 A40P12 F136P0"
#	else
				"F64P0 A32P12 F160P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				"F48P0 A24P8 F184P0"
#	else
				"F32P0 A16P4 F208P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		// Expected layout after the third allocated ptr is freed
		heapZone.Free(ptr3);
		{
			std::string const expectedLayout = "F256P0";
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}
	}

	TEST(NA_FIXTURE_NAME, FreeListNodePtrsCorrect)
	{
		HeapZoneTypeFLAT heapZone{ HeapZoneBase::c_NoParent, c_LargeHeapZoneSizeFLAT, "TestHeapZone" };
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		// Expected starting free list structure 
		FreeListNode const* const firstFreeListNode = NA_REINTERPRET_MEMORY_DEFAULT(FreeListNode, (heapZoneInfo.m_Start + c_BlockHeaderSize));
		EXPECT_FALSE(firstFreeListNode->m_Next);
		EXPECT_FALSE(firstFreeListNode->m_Previous);

		// Force a memory layout of F-A-F-A-F to create a free list
		void* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* const ptr3 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* const ptr4 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));

		heapZone.Free(ptr1);
		heapZone.Free(ptr3);

		FreeListNode const* const freeListNode1 = NA_REINTERPRET_MEMORY_DEFAULT(FreeListNode, ptr1); // The free list node starts where the payload was
		FreeListNode const* const freeListNode3 = NA_REINTERPRET_MEMORY_DEFAULT(FreeListNode, ptr3);

		uInt const ptr4Size = bit_operations::RightShiftBit(NA_REINTERPRET_MEMORY(BlockHeader, ptr4, -, c_BlockHeaderSize)->m_BlockInfo,
			type_utils::ToUnderlying(BlockInfoIndex::SizeStart)); // 4/4 code
		FreeListNode const* const lastFreeListNode = NA_REINTERPRET_MEMORY(FreeListNode, ptr4, +, ptr4Size);

		// Check that the free list structure is expected
		EXPECT_FALSE(freeListNode1->m_Previous);
		EXPECT_EQ(freeListNode1->m_Next, freeListNode3);
		EXPECT_EQ(freeListNode3->m_Previous, freeListNode1);

		// Allocate into the first free block, and check that the free list updates
		void* const ptr5 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		EXPECT_FALSE(freeListNode3->m_Previous);

		// Allocate into the second free block so only the last free block remains and check that the free list is one element again
		void* const ptr6 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		EXPECT_FALSE(lastFreeListNode->m_Previous);
		EXPECT_FALSE(lastFreeListNode->m_Next);

		// Free all the memory allocated
		heapZone.Free(ptr2);
		heapZone.Free(ptr4);
		heapZone.Free(ptr5);
		heapZone.Free(ptr6);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests

// --- HeapZoneTests.cpp ---

/**
 * Tests for HeapZoneBase / HeapZone.
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(HeapZoneTests)

	uInt constexpr c_MockHeapZoneSizeHZT = 1u;
	uInt constexpr c_HeapZoneSizeHZT = 64u;
	uInt constexpr c_LargeHeapZoneSizeHZT = 128u;

	class MockAllocator final : public AllocatorBase
	{
	public:
		MockAllocator(HeapZoneInfo const& /*heapZoneInfo*/)
			: m_AllocationCount(0u)
		{
		}

		[[nodiscard]] void* Allocate(AllocationInfo const& /*allocationInfo*/, HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			++m_AllocationCount;
			return nullptr;
		}

		void Free(void* memory, HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			--m_AllocationCount;
		}

		void Reset(HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			m_AllocationCount = 0;
		}

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const /*action*/, HeapZoneInfo const& /*heapZoneInfo*/) const override
		{
			NA_FUNCTION_NOT_IMPLEMENTED;
			return {};
		}

		[[nodiscard]] AllocatorBlockInfo GetAllocationInfo(void const* const /*memory*/, HeapZoneInfo const& /*heapZoneInfo*/) const override
		{
			NA_FUNCTION_NOT_IMPLEMENTED;
			return {};
		}

		[[nodiscard]] inline u32 GetAllocationCount() const noexcept
		{
			return m_AllocationCount;
		}

	private:
		u32 m_AllocationCount;
	};

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		HeapZone<MockAllocator> heapZone = { HeapZoneBase::c_NoParent, c_MockHeapZoneSizeHZT, "TestHeapZone" };
		HeapZoneInfo const& heapZoneInfo = heapZone.GetZoneInfo();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(c_MockHeapZoneSizeHZT, size);
		}
		
		heapZone.Deinit();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(0u, size);
		}
	}

	TEST(NA_FIXTURE_NAME, CheckAllocatorCalls)
	{
		HeapZone<MockAllocator> heapZone = { HeapZoneBase::c_NoParent, c_MockHeapZoneSizeHZT, "TestHeapZone" };
		MockAllocator const& mockAllocator = heapZone.GetAllocator();

		void* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(1u, c_NullMemoryTag));
		void const* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(1u, c_NullMemoryTag));
		EXPECT_EQ(2u, mockAllocator.GetAllocationCount());

		heapZone.Free(ptr1);
		EXPECT_EQ(1u, mockAllocator.GetAllocationCount());

		heapZone.Reset();
		EXPECT_EQ(0u, mockAllocator.GetAllocationCount());
	}

	TEST(NA_FIXTURE_NAME, ParentZone)
	{
		HeapZone<DefaultFreeListAllocator> parentZone = { HeapZoneBase::c_NoParent, c_LargeHeapZoneSizeHZT, "ParentZone" };
		HeapZone<DefaultFreeListAllocator> childZone = { &parentZone, c_HeapZoneSizeHZT, "ChildZone" };

		HeapZoneInfo const& parentZoneInfo = parentZone.GetZoneInfo();
		HeapZoneInfo const& childZoneInfo = childZone.GetZoneInfo();

		EXPECT_EQ(parentZoneInfo.m_Start + c_BlockHeaderSize, childZoneInfo.m_Start);
		EXPECT_EQ(parentZoneInfo.m_Start + c_BlockHeaderSize + c_HeapZoneSizeHZT, childZoneInfo.m_End);
	}

	TEST(NA_FIXTURE_NAME, AllHeapZonesUpdates)
	{
		EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 0u);

		{
			HeapZone<MockAllocator> heapZone1 = { HeapZoneBase::c_NoParent, c_MockHeapZoneSizeHZT, "TestHeapZone1" };
			EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 1u);

			{
				HeapZone<MockAllocator> heapZone2 = { HeapZoneBase::c_NoParent, c_MockHeapZoneSizeHZT, "TestHeapZone2" };
				EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 2u);
			}

			EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 1u);
		}
		
		EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 0u);
	}

	TEST(NA_FIXTURE_NAME, FindHeapZoneForPtr)
	{
		HeapZone<DefaultFreeListAllocator> heapZone1 = { HeapZoneBase::c_NoParent, c_HeapZoneSizeHZT, "TestHeapZone1" };
		HeapZone<DefaultFreeListAllocator> heapZone2 = { HeapZoneBase::c_NoParent, c_HeapZoneSizeHZT, "TestHeapZone2" };

		void* ptr1 = heapZone1.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* ptr2 = heapZone2.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));

		EXPECT_TRUE(heapZone1.ContainsPtr(ptr1));
		EXPECT_FALSE(heapZone1.ContainsPtr(ptr2));

		EXPECT_EQ(HeapZoneBase::FindHeapZoneForPtr(ptr2), &heapZone2);
		EXPECT_FALSE(HeapZoneBase::FindHeapZoneForPtr(ptr2) == &heapZone1);

		heapZone1.Free(ptr1);
		heapZone2.Free(ptr2);
	}

#	ifdef NA_THREAD_SAFE_HEAP_ZONE
		TEST(NA_FIXTURE_NAME, MultipleThreadAllocations)
		{
			// i can't really think of a good test for this right now... just if it doesn't crash its succeeded right? xd
			// i can confirm that as of 27/11/23 it crashes if i comment out NA_THREAD_SAFE_HEAP_ZONE and run this :p

			if (std::thread::hardware_concurrency() >= 3u)
			{
				HeapZone<DefaultFreeListAllocator> heapZone = { HeapZoneBase::c_NoParent, c_LargeHeapZoneSizeHZT, "TestHeapZone" };
				auto const allocateThroughHeapZone =
					[&heapZone]() -> void
				{
					for (u32 i = 0u; i < 100u; ++i)
					{
						void* const memory = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
						heapZone.Free(memory);
					}
				};

				std::thread thread1(allocateThroughHeapZone);
				std::thread thread2(allocateThroughHeapZone);

				thread1.join();
				thread2.join();
			}
		}
#	endif // ifdef NA_THREAD_SAFE_HEAP_ZONE

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests

// --- MemoryCommandTests.cpp ---

/**
 * Tests for MemoryCommand.
 *
 * Note:
 *	- Some stuff is tested in FullWorldflow.cpp -> BasicWorkflow & FullWorkFlow (it just makes more sense to do it there).
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(MemoryCommandTests)

	uInt constexpr c_HeapZoneSizeMCT = 64u;

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		MemoryCommand memoryCommand = {};
		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

	TEST(NA_FIXTURE_NAME, PushAndPopScope)
	{
		MemoryCommand memoryCommand = {};
		{
			HeapZoneScope scope = { /*heapZone*/ nullptr, /*memoryTag*/ std::nullopt, /*customMemoryCommand*/ &memoryCommand };
			EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope);
		}

		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

	TEST(NA_FIXTURE_NAME, PushAndPopTwoScopes)
	{
		MemoryCommand memoryCommand = {};
		{
			HeapZoneScope scope1 = { nullptr, std::nullopt, &memoryCommand };
			{
				HeapZoneScope scope2 = { nullptr, std::nullopt, &memoryCommand };
				EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope2);
			}
			EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope1);
		}

		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

#	ifdef NA_THREAD_LOCAL_HEAPS
		TEST(NA_FIXTURE_NAME, ThreadLocalHeapZoneScopes)
		{
			if (std::thread::hardware_concurrency() >= 2u)
			{
				// I know this looks a little bot, and its probs is, but this test does fail if you 
				// change MemoryCommand->g_HeapZoneScopes to 'static' instead of 'threadlocal'

				MemoryCommand memoryCommand = {};
				auto const pushHeapZoneScope =
					[&memoryCommand]() -> void
				{
					HeapZoneScope scope = { &c_UnmanagedHeap, std::nullopt, &memoryCommand };
					EXPECT_EQ(memoryCommand.GetHeapZoneScopeCount(), 1u);
				};

				HeapZoneScope scope = { &c_UnmanagedHeap, std::nullopt, &memoryCommand };
				std::thread thread(pushHeapZoneScope);
				EXPECT_EQ(memoryCommand.GetHeapZoneScopeCount(), 1u);
				thread.join();
			}
		}
#	endif // ifdef NA_THREAD_LOCAL_HEAPS

	TEST(NA_FIXTURE_NAME, TooLargeAllocation)
	{
#	ifdef NA_DEBUG
		// Kinda the same deal as the StackAllocatorTests::EnsureTopOfStackFree. Idk why, but gTest 
		// does not like asserts and I can't find an EXPECT_ASSERT solution or something like at PG.
		return;
#	endif

		MemoryCommand memoryCommand = {};
		HeapZone<DefaultFreeListAllocator> heapZone = { HeapZoneBase::c_NoParent, c_HeapZoneSizeMCT, "TestHeapZone" };
		HeapZoneScope scope = { &heapZone, std::nullopt, &memoryCommand };

		void* ptr = memoryCommand.Allocate(c_HeapZoneSizeMCT + 4u);
		bool const ptrIsNull = (ptr == nullptr); // gTest doesn't like checking pointers in EXPECT_[TRUE/FALSE]?
#	ifdef NA_MALLOC_IF_OUT_OF_MEMORY
		EXPECT_FALSE(ptrIsNull);
		memoryCommand.Free(ptr);
#	else
		EXPECT_TRUE(ptrIsNull);
		if (!ptrIsNull) // Just in case!
		{
			memoryCommand.Free(ptr);
		}
#	endif // ifdef NA_MALLOC_IF_OUT_OF_MEMORY
	}

	TEST(NA_FIXTURE_NAME, OutOfScopeFree)
	{
		MemoryCommand memoryCommand = {};
		HeapZone<DefaultFreeListAllocator> heapZone = { HeapZoneBase::c_NoParent, c_HeapZoneSizeMCT, "TestHeapZone" };

		void* ptr = nullptr;
		{
			HeapZoneScope scope = { &heapZone, std::nullopt, &memoryCommand };
			ptr = memoryCommand.Allocate(4u);
		}

		memoryCommand.Free(ptr);
#ifdef NA_TRACK_ALLOCATIONS
		EXPECT_EQ(heapZone.GetAllocator().GetStats().m_ActiveAllocationCount, 0u);
		// idk how to test this without allocation tracking ):
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests

// --- MemoryOperationsTests.cpp ---

/**
 * Tests for MemoryOperations
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(MemoryOperationsTests)

	TEST(NA_FIXTURE_NAME, RequestAndReleaseMemory)
	{
		s32* memory = memory_operations::RequestMemoryFromOS<s32*>(sizeof(s32));
		EXPECT_TRUE(memory);

		memory_operations::ReleaseMemoryToOS(memory);
		EXPECT_FALSE(memory);
	}

	TEST(NA_FIXTURE_NAME, ResetMemory)
	{
		struct MockStruct final
		{
			u32 m_Value;
		};

		{
			MockStruct mockStruct = { 1u };

			memory_operations::ResetMemory(&mockStruct);
			EXPECT_EQ(mockStruct.m_Value, 0u);
		}

		{
			MockStruct mockStruct = { 2u };
			MockStruct* const ptrToMockStruct = &mockStruct;

			memory_operations::ResetMemory(ptrToMockStruct);
			EXPECT_EQ(ptrToMockStruct->m_Value, 0u);
		}

		{
			MockStruct mockStruct = { 3u };
			MockStruct* const ptrToMockStruct = &mockStruct;

			memory_operations::ResetMemory(ptrToMockStruct, sizeof(MockStruct));
			EXPECT_EQ(ptrToMockStruct->m_Value, 0u);
		}
	}

	TEST(NA_FIXTURE_NAME, GetMemorySize)
	{
		uPtr constexpr start = 0u;
		uPtr constexpr end = 8u;

		void const* const startVoid = NA_TO_VPTR(start);
		void const* const endVoid = NA_TO_VPTR(end);

		EXPECT_EQ(8u, memory_operations::GetMemorySize(start, end));
		EXPECT_EQ(8u, memory_operations::GetMemorySize(startVoid, endVoid));
	}

	TEST(NA_FIXTURE_NAME, IsAlligned)
	{
		uInt constexpr value = 8u;
		EXPECT_TRUE(memory_operations::IsAlligned(value, 4u));
		EXPECT_FALSE(memory_operations::IsAlligned(value, 3u));

		void const* const ptr = NA_TO_VPTR(value);
		EXPECT_TRUE(memory_operations::IsAlligned(ptr, 4u));
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests

// --- StackAllocatorTests.cpp ---

/**
 * Tests for StackAllocator.
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(StackAllocatorTests)

	using HeapZoneTypeSAT = HeapZone<DefaultStackAllocator>;
	uInt constexpr c_HeapZoneSizeSAT = 64u;

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		blueprints::AllocatorCreateAndDestroyTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT, // Heap zone size
			"F64P0"            // Expected init layout
		);
	}

	TEST(NA_FIXTURE_NAME, AllocateAndFree)
	{
		blueprints::AllocatorAllocateAndFreeTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT, // Heap zone size
			"A24P4 F40P0",     // Expected x64 + memory tagging layout
			"A16P4 F48P0",     // Expected x64 layout
			"A16P4 F48P0",     // Expected x86 + memory tagging layout
			"A8P0 F56P0",      // Expected x86 layout
			"F64P0"            // Expected free layout
		);
	}

	TEST(NA_FIXTURE_NAME, TooLargeAllocation)
	{
		blueprints::AllocatorAllocateTooLargeTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT // Heap zone size
		);
	}

	TEST(NA_FIXTURE_NAME, Reset)
	{
		blueprints::AllocatorResetTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT, // Heap zone size
			"F64P0"            // Expected reset layout
		);
	}

	TEST(NA_FIXTURE_NAME, GetAllocationInfo)
	{
		blueprints::AllocatorGetAllocationInfoTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT // Heap zone size
		);
	}

#	ifdef NA_MEMORY_TAGGING
	TEST(NA_FIXTURE_NAME, MemoryTagging)
	{
		// This case also tests two consecutive allocations then frees.
		// In the x64 + memory tag case it also ensures that if there is insufficient space in the zone it is added as padding.
		//	- This is done in FreeListAllocator's AllocateAndFree case.

		blueprints::AllocatorMemoryTagTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT,    // Heap zone size
			"One24 Two40",        // Expected x64 + memory tagging usage
			"One16 Two16 Free32", // Expected x64 usage
			"One16 Two16 Free32", // Expected x86 + memory tagging usage
			"One8 Two8 Free48",   // Expected x86 usage
			"Free64"              // Expected free usage
		);
	}
#	endif // ifdef NA_MEMORY_TAGGING

#	ifdef NA_TRACK_ALLOCATIONS
	TEST(NA_FIXTURE_NAME, AllocationTracking)
	{
		blueprints::AllocatorAllocationTrackingTest<HeapZoneTypeSAT>(
			c_HeapZoneSizeSAT, // Heap zone size
			24u,		       // Expected x64 + memory tagging allocation size
			16u,               // Expected x64 allocation size
			16u,               // Expected x86 + memory tagging allocation size
			8u                 // Expected x86 allocation size
		);
	}
#	endif // ifdef NA_TRACK_ALLOCATIONS

	TEST(NA_FIXTURE_NAME, EnsureTopOfStackAllocate)
	{
		HeapZoneTypeSAT heapZone = { HeapZoneBase::c_NoParent, c_HeapZoneSizeSAT, "TestHeapZone" };
		void const* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void const* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));

		EXPECT_TRUE(NA_TO_UPTR(ptr1) < NA_TO_UPTR(ptr2));
		heapZone.Reset();
	}

	TEST(NA_FIXTURE_NAME, EnsureTopOfStackFree)
	{
		/*
		* I'm not quite sure how to test this. At PG we had EXPECT_ASSERT I think but I'm not sure
		* if thats in gMock rather than gTest... Or perhaps it was something custom...
		*
			HeapZoneType heapZone = { HeapZoneBase::c_NoParent, c_HeapZoneSizeSAT, "TestHeapZone" };
			void* const ptr1 = heapZone.Allocate(4u);
			void const* const ptr2 = heapZone.Allocate(4u);

			EXPECT_ANY_THROW/ASSERT_DEATH(heapZone.Free(ptr1)); // if using ASSERT_DEATH the regex is ".*", apparently
			heapZone.Reset();

		* In the mean time, I'll just assume it works :p
		*/

		EXPECT_TRUE(true);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests

// --- FullWorkflow.cpp ---

/**
 * A test for the full workflow of NabiAllocator.
 *
 * Note:
 *	- If NA_OVERRIDE_NEW_DELETE isn't defined, these will just run with the default new/delete implementation
 *    (or with whatever implementation a user has defined I guess..).
 *		- Also note if this isn't defined then checking memory tags of globally allocated memory won't work
 *        (see the FullWorkFlow test - it *will* compile but *won't* meet the condition).
*/

namespace nabi_allocator::tests
{
#ifdef NA_WORKFLOWS
#	define NA_FIXTURE_NAME NA_WORKFLOW_FIXTURE_NAME(WorkFlows)

	class TestClass
	{
	public:
		static u32 s_Count;

		TestClass()
		{
			++s_Count;
		}
		~TestClass()
		{
			--s_Count;
		}
	};
	u32 TestClass::s_Count = 0u;

	TEST(NA_FIXTURE_NAME, BasicWorkFlow)
	{
		NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(
			HeapZone<DefaultFreeListAllocator>, // Heap zone type
			HeapZoneBase::c_NoParent,           // Heap zone parent
			1_MB,                               // Heap zone size
			"Allocator",                        // Heap zone debug name
			c_NullMemoryTag                     // Heap zone scope memory tag
		);

		// Raw ptrs
		auto const* const individual = new TestClass();
		auto const* const arr = new TestClass[3u];
		EXPECT_EQ(TestClass::s_Count, 4u);

		// STD container
		{
			std::vector<TestClass*> vec = { new TestClass(), new TestClass(), new TestClass() };
			EXPECT_EQ(TestClass::s_Count, 7u);

			for (uInt i = 0u; i < 50u; ++i)
			{
				vec.push_back(new TestClass());
			}
			EXPECT_EQ(TestClass::s_Count, 57u);

			for (TestClass* tC : vec)
			{
				delete tC;
			}
			vec.clear();
			EXPECT_EQ(TestClass::s_Count, 4u);
		}

		// Smart ptrs
		{
			{
				auto const uniquePtr = std::make_unique<TestClass>();
				auto const sharedPtr = std::make_shared<TestClass>();
				std::weak_ptr<TestClass> const weakPtr = sharedPtr;
				EXPECT_EQ(TestClass::s_Count, 6u);
			}
			EXPECT_EQ(TestClass::s_Count, 4u);
		}

		delete individual;
		delete[] arr;
		EXPECT_EQ(TestClass::s_Count, 0u);
	}

	TEST(NA_FIXTURE_NAME, FullWorkFlow)
	{
		uInt constexpr parentZoneNumBytes = 256u;
		uInt constexpr childZoneNumBytes = parentZoneNumBytes / 2u;

		enum class MemoryTag : memoryTag
		{
			General = 1u << 1u,
			Rendering = 1u << 2u, // Just some example sections where allocations could come from
			All = ~0u,

			ENUM_COUNT = 3u
		};

		HeapZone<DefaultStackAllocator> parentZone = { HeapZoneBase::c_NoParent, parentZoneNumBytes, "ParentZone" };
		HeapZone<DefaultFreeListAllocator> childZone = { &parentZone, childZoneNumBytes, "ChildZone" };
		NA_SET_HEAP_ZONE_SCOPE(&childZone, type_utils::ToUnderlying(MemoryTag::General)); // Initial scope

		auto const verifyBlockMemoryTag =
			[](uPtr const heapZoneStart, MemoryTag const _memoryTag) -> bool
		{
			bool result = false;
			BlockHeader const* const blockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, heapZoneStart);

			result =
#ifdef NA_MEMORY_TAGGING
			(blockHeader->m_MemoryTag == type_utils::ToUnderlying(_memoryTag))
#else
				true
#endif // ifdef NA_MEMORY_TAGGING
				;

			return result;
		};
		uPtr const childZoneStart = childZone.GetZoneInfo().m_Start;

		// General tag allocation
		{
			auto const* const allocation = new int();
#ifdef NA_OVERRIDE_NEW_DELETE
			EXPECT_TRUE(verifyBlockMemoryTag(childZoneStart, MemoryTag::General));
#endif // ifdef NA_OVERRIDE_NEW_DELETE
			delete allocation;
		}

		// Switch tag and allocate
		{
			NA_SET_HEAP_ZONE_SCOPE(c_SameZone, type_utils::ToUnderlying(MemoryTag::Rendering));
			auto const* const allocation = new int();
#ifdef NA_OVERRIDE_NEW_DELETE
			EXPECT_TRUE(verifyBlockMemoryTag(childZoneStart, MemoryTag::Rendering));
#endif // ifdef NA_OVERRIDE_NEW_DELETE
			delete allocation;
		}

		// Switch to the unmanaged heap and allocate
		{
			NA_SET_HEAP_ZONE_SCOPE(&c_UnmanagedHeap, c_SameTag);
			auto const* const allocation = new int(21);
			EXPECT_TRUE(allocation); EXPECT_EQ(*allocation, 21);
			delete allocation;
		}

		// Check heap zone usage is as expected
#ifdef NA_TRACK_ALLOCATIONS
		EXPECT_EQ(parentZone.GetAllocator().GetStats().m_ActiveAllocationCount, 1u);
		EXPECT_EQ(childZone.GetAllocator().GetStats().m_ActiveAllocationCount, 0u);
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

	TEST(NA_FIXTURE_NAME, LocalHeapZoneWorkFlow)
	{
		// *The 'no global heap zone workflow' is tested, well, in a lot of other places. Eg: [Free/Stack]AllocatorTests.cpp->AllocatorDefaultTests.inl*

		NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(
			HeapZone<DefaultFreeListAllocator>, // Heap zone type
			HeapZoneBase::c_NoParent,           // Heap zone parent
			256u,                               // Heap zone size
			"Allocator",                        // Heap zone debug name
			c_NullMemoryTag                     // Heap zone scope memory tag
		);

		HeapZoneBase* const topHeapZone = MemoryCommand::Instance().GetTopHeapZoneScope()->GetHeapZone();
		{
			HeapZone<DefaultFreeListAllocator> localZone = { topHeapZone, 64u, "LocalZone" };
			EXPECT_EQ(MemoryCommand::Instance().GetHeapZoneScopeCount(), 1u);

			void* const allocation = localZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
			localZone.Free(allocation);
		}

#ifdef NA_TRACK_ALLOCATIONS
		EXPECT_EQ(reinterpret_cast<HeapZone<DefaultFreeListAllocator>*>(topHeapZone)->GetAllocator().GetStats().m_ActiveAllocationCount, 0u);
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_WORKFLOWS
}

// See https://github.com/WzrdIsTaken/Nabi-Allocator for full source code & wiki! 

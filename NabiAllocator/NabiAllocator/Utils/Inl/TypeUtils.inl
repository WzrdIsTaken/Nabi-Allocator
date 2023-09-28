// inl's Header
#include "TypeUtils.h"

namespace nabi_allocator::type_utils
{
	template<is_enum Enum>
	inline constexpr typename std::underlying_type<Enum>::type ToUnderlying(Enum const e) noexcept
	{
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}
} // namespace nabi_allocator::type_utils

// inl's Header
#include "TypeUtils.h"

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

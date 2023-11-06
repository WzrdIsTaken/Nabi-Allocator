#pragma once

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

#pragma once
#include <xxhash_cx.h>

namespace neko
{
using StringHash = xxhash::hash<64>::hash_type;
using StringHash32 = xxhash::hash<32>::hash_type;
constexpr StringHash kHashSeed = 0;
constexpr StringHash32 kHashSeed32 = kHashSeed;

// 64-bit versions
constexpr StringHash HashString(const std::string& s)
{
	// Using IM_ARRAYSIZE and data() because otherwise hashes does not generate the same way
	return xxhash::xxh64(s.data(), s.size(), kHashSeed);
}

constexpr StringHash HashString(const std::string_view& s)
{
	// Using IM_ARRAYSIZE and data() because otherwise hashes does not generate the same way
	return xxhash::xxh64(s.data(), s.size(), kHashSeed);
}

constexpr StringHash HashString(const char* s, std::size_t length)
{
	return xxhash::xxh64(s, length, kHashSeed);
}

// 32-bit versions
constexpr StringHash32 HashString32(const std::string& s)
{
	// Using IM_ARRAYSIZE and data() because otherwise hashes does not generate the same way
	return xxhash::xxh32(s.data(), s.size(), kHashSeed32);
}

constexpr StringHash32 HashString32(const std::string_view& s)
{
	// Using IM_ARRAYSIZE and data() because otherwise hashes does not generate the same way
	return xxhash::xxh32(s.data(), s.size(), kHashSeed32);
}

constexpr StringHash32 HashString32(const char* s, std::size_t length)
{
	return xxhash::xxh32(s, length, kHashSeed32);
}
}
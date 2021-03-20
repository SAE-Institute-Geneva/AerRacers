#pragma once
#include <functional>

#include <xxhash_cx.h>

inline void hash_combine(std::size_t&) {}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

#define MAKE_HASHABLE(type, ...)               \
	namespace std                              \
	{                                          \
	template<>                                 \
	struct hash<type>                          \
	{                                          \
		size_t operator()(const type& t) const \
		{                                      \
			size_t ret = 0;                    \
			hash_combine(ret, __VA_ARGS__);    \
			return ret;                        \
		}                                      \
	};                                         \
	}

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
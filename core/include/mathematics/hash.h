#pragma once
#include <functional>

#include <xxhash_cx.h>

/// Implementation of the HashCombine function from the boost library
inline void HashCombine(std::size_t&) {}

/// Implementation of the HashCombine function from the boost library
template<typename T, typename... Params>
inline void HashCombine(std::size_t& seed, const T& v, Params... params)
{
	std::hash<T> hash;
	seed ^= hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	HashCombine(seed, params...);
}

/// Specialization of std::hash for vectors using HashCombine <br>
/// Custom types must be registered with MAKE_HASHABLE in order to work
namespace std
{
template<typename T>
struct hash<vector<T>>
{
	typedef vector<T> ArgumentType;
	typedef size_t ResultType;
	ResultType operator()(ArgumentType const& in) const
	{
		size_t size = in.size();
		size_t seed = 0;
		for (size_t i = 0; i < size; i++) HashCombine(seed, in[i]);
		return seed;
	}
};
}    // namespace std

/// A macro that makes a custom type compatible with std::hash <br>
/// Useful for using unordered_map with a custom type as the key
#define MAKE_HASHABLE(type, ...)               \
	namespace std                              \
	{                                          \
	template<>                                 \
	struct hash<type>                          \
	{                                          \
		size_t operator()(const type& t) const \
		{                                      \
			size_t ret = 0;                    \
			HashCombine(ret, __VA_ARGS__);     \
			return ret;                        \
		}                                      \
	};                                         \
	}

namespace neko
{
/// A 64bit hash
using StringHash = xxhash::hash<64>::hash_type;

/// A 32bit hash
using StringHash32 = xxhash::hash<32>::hash_type;

/// The seed used for hashing
constexpr StringHash kHashSeed = 541959286;

/// The seed used for hashing
constexpr StringHash32 kHashSeed32 = kHashSeed;

/// Generates a 64bit long hash from a string
/// \param s: the string to hash
/// \return A 64bit hash
constexpr StringHash HashString(const std::string& s)
{
	return xxhash::xxh64(s.data(), s.size(), kHashSeed);
}

/// Generates a 64bit long hash from a string_view
/// \param s: the string_view to hash
/// \return A 64bit hash
constexpr StringHash HashString(const std::string_view& s)
{
	return xxhash::xxh64(s.data(), s.size(), kHashSeed);
}

/// Generates a 64bit long hash from a const char* <br>
/// \param s: the char array to hash
/// \param length: the length of the array <br>
/// Due to how pointers work, arrays shorter that 8 characters need to specify their size explicitly,
/// otherwise the hash will return a wrong value <br>
/// Prefer using a string_view whenever possible
/// \return A 64bit hash
constexpr StringHash HashString(const char* s, std::size_t length = 0)
{
	if (length == 0) length = sizeof(s) / sizeof(*s);
	return xxhash::xxh64(s, length, kHashSeed);
}

/// Generates a 32bit long hash from a string
/// \param s: the string to hash
/// \return A 32bit hash
constexpr StringHash32 HashString32(const std::string& s)
{
	return xxhash::xxh32(s.data(), s.size(), kHashSeed32);
}

/// Generates a 32bit long hash from a string_view
/// \param s: the string_view to hash
/// \return A 32bit hash
constexpr StringHash32 HashString32(const std::string_view& s)
{
	return xxhash::xxh32(s.data(), s.size(), kHashSeed32);
}

/// Generates a 32bit long hash from a const char* <br>
/// \param s: the char array to hash
/// \param length: the length of the array <br>
/// Due to how pointers work, arrays shorter that 8 characters need to specify their size explicitly,
/// otherwise the hash will return a wrong value <br>
/// Prefer using a string_view whenever possible
/// \return A 32bit hash
constexpr StringHash32 HashString32(const char* s, std::size_t length = 0)
{
	if (length == 0) length = sizeof(s) / sizeof(*s);
	return xxhash::xxh32(s, length, kHashSeed32);
}
}
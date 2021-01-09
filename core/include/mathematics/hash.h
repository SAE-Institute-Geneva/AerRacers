#pragma once
#include <xxhash.h>
#include <string>

namespace neko
{
using StringHash = XXH64_hash_t;
const static XXH64_hash_t kHashSeed = 0;

static XXH64_hash_t HashString(const std::string& s)
{
    return XXH64(s.c_str(), s.size(), kHashSeed);
}

static XXH64_hash_t HashString(const std::string_view& s)
{
    return XXH64(&s[0], s.size(), kHashSeed);
}

static XXH64_hash_t HashString(const char s[256])
{
    return XXH64(&s[0], strlen(s), kHashSeed);
}
}
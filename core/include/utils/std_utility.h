#pragma once
#include <optional>

namespace std
{
template<class T>
using optional_ref = optional<reference_wrapper<T>>;

template<class T>
using optional_const_ref = optional<reference_wrapper<const T>>;
}

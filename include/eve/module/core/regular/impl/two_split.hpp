//==================================================================================================
/*
  EVE - Expressive Vector Engine
  Copyright : EVE Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <eve/concept/value.hpp>
#include <eve/detail/abi.hpp>
#include <eve/detail/apply_over.hpp>
#include <eve/detail/implementation.hpp>
#include <eve/detail/kumi.hpp>
#include <eve/detail/meta.hpp>

namespace eve::detail
{
template<ordered_value T>
EVE_FORCEINLINE kumi::tuple<T, T>
                two_split_(EVE_SUPPORTS(cpu_), const T                &a) noexcept
{
  if constexpr( has_native_abi_v<T> )
  {
    T const c  = ieee_constant<0x1.0p+13f, 0x1.0p+27>(as<T>()) * a;
    T const c1 = c - a;
    T       r0 = c - c1;
    return {r0, a - r0};
  }
  else return apply_over2(two_split, a);
}
}
//==================================================================================================
/*
  EVE - Expressive Vector Engine
  Copyright : EVE Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <eve/module/core.hpp>
#include <eve/module/math/constant/pi.hpp>
#include <eve/module/math/regular/sinpi.hpp>

namespace eve::detail
{

template<value T>
EVE_FORCEINLINE constexpr auto
sinpic_(EVE_SUPPORTS(cpu_), T a0) noexcept
{
  using safe_t =  decltype(abs(a0));
  if constexpr( has_native_abi_v<T> )
  {
    if constexpr( scalar_value<T> )
    {
      if( is_eqz(a0) ) return one(eve::as(a0));
      if constexpr( eve::platform::supports_infinites )
        if( is_infinite(a0) ) return zero(eve::as<T>());
      if constexpr( eve::platform::supports_denormals )
        return eve::abs(a0) < eps(as<safe_t>()) ? one(eve::as<T>()) : sinpi(a0) / (pi(as<safe_t>()) * a0);
      else return sinpi(a0) / (pi(eve::as(a0)) * a0);
    }
    else
    {
      auto r1 = sinpi(a0) / (pi(eve::as(a0)) * a0);
      if constexpr( eve::platform::supports_denormals )
      {
        r1 = if_else(eve::abs(a0) < eps(as<safe_t>()), one(eve::as<T>()), r1);
      }
      else r1 = if_else(is_eqz(a0), one(eve::as<T>()), r1);
      if constexpr( eve::platform::supports_infinites )
        r1 = if_else(is_infinite(a0), eve::zero, r1);
      return r1;
    }
  }
  else { return apply_over(sinpic, a0); }
}

// -----------------------------------------------------------------------------------------------
// Masked cases
template<conditional_expr C, value U>
EVE_FORCEINLINE auto
sinpic_(EVE_SUPPORTS(cpu_), C const& cond, U const& t) noexcept
{
  return mask_op(cond, eve::sinpic, t);
}

template<conditional_expr C, decorator D, value U>
EVE_FORCEINLINE auto
sinpic_(EVE_SUPPORTS(cpu_), C const& cond, D const & d, U const& t) noexcept
{
  return mask_op(cond, d(eve::sinpic), t);
}
}
//==================================================================================================
/*
  EVE - Expressive Vector Engine
  Copyright : EVE Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <eve/module/core.hpp>
#include <eve/module/math/constant/pio_2.hpp>
#include <eve/module/math/constant/pio_4.hpp>
#include <eve/module/math/decorator/trigo_tags.hpp>
#include <eve/module/math/detail/constant/rempio2_limits.hpp>
#include <eve/module/math/detail/generic/trig_finalize.hpp>
#include <eve/module/math/regular/rempio2.hpp>

#include <type_traits>

namespace eve::detail
{
template<floating_ordered_value T>
EVE_FORCEINLINE constexpr auto
tan_(EVE_SUPPORTS(cpu_), quarter_circle_type const&, T a0) noexcept
{
  if constexpr( has_native_abi_v<T> )
  {
    if constexpr( scalar_value<T> )
    {
      if( is_not_less_equal(eve::abs(a0), pio_4(eve::as<T>())) ) return nan(eve::as<T>());
    }
    else
    {
      auto pi2_16 =  pi2o_16[upward](as<T>());
      a0 = if_else(is_not_less_equal(sqr(a0), pi2_16), eve::allbits, a0);
    }
    return tancot_eval(a0);
  }
  else return apply_over(quarter_circle(tan), a0);
}

template<floating_ordered_value T>
EVE_FORCEINLINE constexpr auto
tan_(EVE_SUPPORTS(cpu_), half_circle_type const&, T a0) noexcept
{
  if constexpr( has_native_abi_v<T> )
  {
    auto reduce = [](auto x)
    {
      auto pio2_1 = ieee_constant<0x1.921f000p+0f, 0x1.921fb54400000p+0>(eve::as<T>{});
      auto pio2_2 = ieee_constant<0x1.6a88000p-17f, 0x1.0b4611a600000p-34>(eve::as<T>{});
      auto pio2_3 = ieee_constant<0x1.0b46000p-34f, 0x1.3198a2e000000p-69>(eve::as<T>{});
      T    xr     = x - pio2_1;
      xr -= pio2_2;
      xr -= pio2_3;
      return xr;
    };
    T x = eve::abs(a0);
    if constexpr( scalar_value<T> )
    {
      if( x < eps(as<T>()) ) return a0;
      if( is_not_less_equal(x, pio_2(eve::as<T>())) ) return nan(eve::as<T>());
      int n = x > pio_4(eve::as<T>());
      if( n )
      {
        T xr = reduce(x);
        T y  = tancot_eval(xr);
        return bit_xor(bitofsign(a0), -rec(y));
      }
      else { return tancot_eval(a0); }
    }
    else if constexpr( simd_value<T> )
    {
      auto test = is_not_less_equal(x, pio_4(eve::as(a0)));
      T    xr   = if_else(test, reduce(x), x);
      auto y    = tancot_eval(xr);
      y         = if_else(is_not_finite(a0), eve::allbits, if_else(test, -rec(y), y));
      return if_else(x <= eps(as<T>()), a0, bit_xor(bitofsign(a0), y));
    }
  }
  else return apply_over(half_circle(tan), a0);
}

template<decorator D, floating_ordered_value T>
EVE_FORCEINLINE constexpr auto
tan_(EVE_SUPPORTS(cpu_), D const&, T a0) noexcept
    requires(is_one_of<D>(types<full_circle_type, medium_type, big_type> {}))
{
  if constexpr( has_native_abi_v<T> )
  {
    auto x       = abs(a0);
    auto xnlelim = is_not_less_equal(x, Rempio2_limit(D(), as(a0)));
    if constexpr( scalar_value<T> )
    {
      if( xnlelim ) return nan(eve::as<T>());
      if( is_eqz(a0) ) return a0;
    }
    else x = if_else(xnlelim, allbits, x);
    auto [fn, xr, dxr] = D()(rempio2)(x);
    return tan_finalize(a0, fn, xr, dxr);
  }
  else return apply_over(D()(tan), a0);
}

template<floating_ordered_value T>
EVE_FORCEINLINE constexpr auto
tan_(EVE_SUPPORTS(cpu_), T const& a0) noexcept
{
  if constexpr( has_native_abi_v<T> )
  {
    auto x = abs(a0);
    if( eve::all(x <= Rempio2_limit(quarter_circle_type(), as(a0))) )
      return quarter_circle(tan)(a0);
    else if( eve::all(x <= Rempio2_limit(half_circle_type(), as(a0))) ) return half_circle(tan)(a0);
    else if( eve::all(x <= Rempio2_limit(full_circle_type(), as(a0))) ) return full_circle(tan)(a0);
    else if( eve::all(x <= Rempio2_limit(medium_type(), as(a0))) ) return medium(tan)(a0);
    else return big(tan)(a0);
  }
  else return apply_over(tan, a0);
}


// -----------------------------------------------------------------------------------------------
// Masked cases
template<conditional_expr C, value U>
EVE_FORCEINLINE auto
tan_(EVE_SUPPORTS(cpu_), C const& cond, U const& t) noexcept
{
  return mask_op(cond, eve::tan, t);
}

template<conditional_expr C, decorator D, value U>
EVE_FORCEINLINE auto
tan_(EVE_SUPPORTS(cpu_), C const& cond, D const & d, U const& t) noexcept
{
  return mask_op(cond, d(eve::tan), t);
}
}

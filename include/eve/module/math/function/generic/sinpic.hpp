//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2020 Joel FALCOU
  Copyright 2020 Jean-Thierry LAPRESTE

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#pragma once

#include <eve/detail/implementation.hpp>
#include <eve/function/abs.hpp>
#include <eve/function/if_else.hpp>
#include <eve/function/is_eqz.hpp>
#include <eve/function/is_infinite.hpp>
#include <eve/function/sinpi.hpp>
#include <eve/constant/eps.hpp>
#include <eve/constant/one.hpp>
#include <eve/platform.hpp>
#include <eve/detail/apply_over.hpp>
#include <eve/concept/value.hpp>

namespace eve::detail
{

  template<floating_real_value T>
  EVE_FORCEINLINE constexpr auto sinpic_(EVE_SUPPORTS(cpu_)
                                     , T a0) noexcept
  {
    if constexpr(has_native_abi_v<T>)
    {
      if constexpr(scalar_value<T>)
      {
        if (is_eqz(a0)) return One(as(a0));
        if constexpr(eve::platform::supports_infinites) if(is_infinite(a0)) return Zero<T>();
        if constexpr(eve::platform::supports_denormals)
          return eve::abs(a0) < Eps<T>() ? One<T>() : sinpi(a0)/(Pi(as(a0))*a0);
        else
          return sinpi(a0)/(Pi(as(a0))*a0);
      }
      else
      {
        auto r1 =  sinpi(a0)/(Pi(as(a0))*a0);
        if constexpr(eve::platform::supports_denormals)
        {
          r1 = if_else (eve::abs(a0) < Eps<T>(), One<T>() , r1);
        }
        else
          r1 = if_else(is_eqz(a0), One<T>(), r1);
        if constexpr(eve::platform::supports_infinites)  r1 = if_else(is_infinite(a0), eve::zero_, r1);
        return r1;
      }
    }
    else
    {
      return apply_over(sinpic, a0);
    }
  }
}

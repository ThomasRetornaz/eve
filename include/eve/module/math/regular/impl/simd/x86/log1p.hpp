//==================================================================================================
/*
  EVE - Expressive Vector Engine
  Copyright : EVE Project Contributors
  SPDX-License-Identifier: BSL-1.0
*/
//==================================================================================================
#pragma once

#include <eve/module/core.hpp>
#include <eve/module/math/regular/horner.hpp>
#include <eve/module/core/detail/generic/horn.hpp>
#include <eve/module/math/constant/invsqrt_2.hpp>

namespace eve::detail
{
// this implementation is better only if we are in avx_ without avx2_

  template<floating_value T>
  EVE_FORCEINLINE auto
  log1p_(EVE_SUPPORTS(cpu_), plain_type const&, const T& a0) noexcept
  {
    if constexpr( has_native_abi_v<T> )
    {
      /* origin: FreeBSD /usr/src/lib/msun/src/e_log1p(f).c */
      /*
       * ====================================================
       * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
       *
       * Developed at SunPro, a Sun Microsystems, Inc. business.
       * Permission to use, copy, modify, and distribute this
       * software is freely granted, provided that this notice
       * is preserved.
       * ====================================================
       */
      T    Log_2hi     = ieee_constant<0x1.6300000p-1f, 0x1.62e42fee00000p-1>(eve::as<T>{});
      T    Log_2lo     = ieee_constant<-0x1.bd01060p-13f, 0x1.a39ef35793c76p-33>(eve::as<T>{});
      T    uf          = inc(a0);
      auto isnez       = is_nez(uf);
      auto [x, k]      = frexp(uf);
      auto x_lt_sqrthf = (invsqrt_2(eve::as<T>()) > x);
      /* reduce x into [sqrt(2)/2, sqrt(2)] */
      k   = dec[x_lt_sqrthf](k);
      T f = dec(x + if_else(x_lt_sqrthf, x, eve::zero));
      /* correction term ~ log(1+x)-log(u), avoid underflow in c/u */
      T c    = if_else(k >= 2, oneminus(uf - a0), a0 - dec(uf)) / uf;
      T hfsq = half(eve::as<T>()) * sqr(f);
      T s    = f / (2.0f + f);
      T z    = sqr(s);
      T w    = sqr(z);
      T t1, t2;
      if constexpr( std::is_same_v<element_type_t<T>, float> )
      {
        t1 = w *
          eve::reverse_horner(w, T(0x1.999c26p-2f), T(0x1.f13c4cp-3f))
          ;
        t2 = z *
          eve::reverse_horner(w, T(0x1.555554p-1f), T(0x1.23d3dcp-2f))
          ;
      }
      else if constexpr( std::is_same_v<element_type_t<T>, double> )
      {
        t1 = w *
      eve::reverse_horner(w, T(0x1.999999997fa04p-2), T(0x1.c71c51d8e78afp-3), T(0x1.39a09d078c69fp-3))
      ;
      t2 = z*eve::reverse_horner(w, T(0x1.5555555555593p-1), T(0x1.2492494229359p-2)
                                , T(0x1.7466496cb03dep-3), T(0x1.2f112df3e5244p-3));
      }
      T R = t2 + t1;
      T r = fma(k, Log_2hi, ((fma(s, (hfsq + R), k * Log_2lo + c) - hfsq) + f));
      T zz;
      if constexpr( eve::platform::supports_infinites )
      {
        zz = if_else(
          isnez, if_else(a0 == inf(eve::as<T>()), inf(eve::as<T>()), r), minf(eve::as<T>()));
      }
      else { zz = if_else(isnez, r, minf(eve::as<T>())); }
      return if_else(is_ngez(uf), eve::allbits, zz);
    }
    else return apply_over(plain(log1p), a0);
  }

// -----------------------------------------------------------------------------------------------
// 256 bits implementation for avx
  template<floating_scalar_value T, typename N>
  EVE_FORCEINLINE auto
  log1p_(EVE_SUPPORTS(avx_),
         wide<T, N> const& v) noexcept requires std::same_as<abi_t<T, N>, x86_256_>
  {
    if constexpr( current_api < avx2 ) return plain(log1p)(v);
    else return musl_(log1p)(v);
  }
}
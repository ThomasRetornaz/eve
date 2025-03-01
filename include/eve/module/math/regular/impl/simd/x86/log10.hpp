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
  template<floating_simd_value T>
  auto
  log10_(EVE_SUPPORTS(cpu_), plain_type const&, const T& a0) noexcept
  {
    T Invlog_10hi = ieee_constant<0x1.bcc0000p-2f, 0x1.bcb7b15200000p-2>(eve::as<T>{});
    T Invlog_10lo = ieee_constant<-0x1.09d5b20p-15f, 0x1.b9438ca9aadd5p-36>(eve::as<T>{});
    T Log10_2hi   = ieee_constant<0x1.3400000p-2f, 0x1.3440000000000p-2>(eve::as<T>{});
    T Log10_2lo   = ieee_constant<0x1.04d4280p-12f, 0x1.3509f79fef312p-18>(eve::as<T>{});
    if constexpr( has_native_abi_v<T> )
    {
      using elt_t = element_type_t<T>;
      if constexpr( std::is_same_v<elt_t, float> )
      {
        /* origin: FreeBSD /usr/src/lib/msun/src/e_log10f.c */
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
        T    xx    = a0;
        T    dk    = zero(eve::as<T>());
        auto isnez = is_nez(a0);
        //       logical<T> test;
        if constexpr( eve::platform::supports_denormals )
        {
          auto test = is_less(a0, smallestposval(eve::as<T>())) && isnez;
          if( eve::any(test) )
          {
            dk = sub[test](dk, T(25));
            xx = if_else(test, xx * T(33554432ul), xx);
          }
        }
        /* reduce x into [sqrt(2)/2, sqrt(2)] */

        auto [x, kk]     = eve::frexp(xx);
        auto x_lt_sqrthf = (invsqrt_2(eve::as<T>()) > x);
        dk += dec[x_lt_sqrthf](kk);
        T f    = dec(x + if_else(x_lt_sqrthf, x, eve::zero));
        T s    = f / (T(2) + f);
        T z    = sqr(s);
        T w    = sqr(z);
        T t1   = w *
          eve::reverse_horner(w, T(0x1.999c26p-2f), T(0x1.f13c4cp-3f))
          ;
        T t2   = z *
          eve::reverse_horner(w, T(0x1.555554p-1f), T(0x1.23d3dcp-2f))
          ;
        T R    = t2 + t1;
        T hfsq = half(eve::as<T>()) * sqr(f);
        T hi   = f - hfsq;
        hi     = bit_and(hi, 0xfffff000u);
        T lo   = fma(s, hfsq + R, f - hi - hfsq);
        T r    = fma(
          dk,
          Log10_2hi,
          fma(hi, Invlog_10hi, fma(lo, Invlog_10hi, fma(lo + hi, Invlog_10lo, dk * Log10_2lo))));
        T zz;
        if constexpr( eve::platform::supports_infinites )
        {
          zz = if_else(
            isnez, if_else(a0 == inf(eve::as<T>()), inf(eve::as<T>()), r), minf(eve::as<T>()));
        }
        else { zz = if_else(isnez, r, minf(eve::as<T>())); }
        return if_else(is_ngez(a0), eve::allbits, zz);
      }
      else if constexpr( std::is_same_v<elt_t, double> )
      {
        /* origin: FreeBSD /usr/src/lib/msun/src/e_log10.c */
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
        using uiT  = as_integer_t<T, unsigned>;
        T    xx    = a0;
        T    dk    = zero(eve::as<T>());
        auto isnez = is_nez(a0);

        logical<T> test = is_less(a0, smallestposval(eve::as<T>())) && isnez;
        if constexpr( eve::platform::supports_denormals )
        {
          if( eve::any(test) )
          {
            dk = sub[test](dk, T(54));
            xx = if_else(test, xx * T(18014398509481984ull), xx);
          }
        }
        /* reduce x into [sqrt(2)/2, sqrt(2)] */
        auto [x, kk]     = eve::frexp(xx);
        auto x_lt_sqrthf = (invsqrt_2(eve::as<T>()) > x);
        dk += dec[x_lt_sqrthf](kk);
        T f  = dec(x + if_else(x_lt_sqrthf, x, eve::zero));
        T s  = f / (T(2) + f);
        T z  = sqr(s);
        T w  = sqr(z);
        T t1 = w *
          eve::reverse_horner(w, T(0x1.999999997fa04p-2), T(0x1.c71c51d8e78afp-3), T(0x1.39a09d078c69fp-3));
        T t2 = z*eve::reverse_horner(w, T(0x1.5555555555593p-1), T(0x1.2492494229359p-2)
                                    , T(0x1.7466496cb03dep-3), T(0x1.2f112df3e5244p-3));
        T R    = t2 + t1;
        T hfsq = half(eve::as<T>()) * sqr(f);
        //      T r = -(hfsq-(s*(hfsq+R))-f)*Invlog_10<T>()+dk*Log_2olog_10<T>(); // fast ?
        T hi     = f - hfsq;
        hi       = bit_and(hi, (allbits(eve::as<uiT>()) << 32));
        T lo     = f - hi - hfsq + s * (hfsq + R);
        T val_hi = hi * Invlog_10hi;
        T y      = dk * Log10_2hi;
        T val_lo = dk * Log10_2lo + (lo + hi) * Invlog_10lo + lo * Invlog_10hi;
        //
        //  Extra precision in for adding y is not strictly needed
        //  since there is no very large cancellation near x = sqrt(2) or
        //  x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
        //  with some parallelism and it reduces the error for many args.
        //
        T w1 = y + val_hi;
        val_lo += (y - w1) + val_hi;
        val_hi = w1;
        T r    = val_lo + val_hi;
        T zz;
        if constexpr( eve::platform::supports_infinites )
        {
          zz = if_else(
            isnez, if_else(a0 == inf(eve::as<T>()), inf(eve::as<T>()), r), minf(eve::as<T>()));
        }

        else { zz = if_else(isnez, r, minf(eve::as<T>())); }
        return if_else(is_ngez(a0), eve::allbits, zz);
      }
    }
    else return apply_over(plain(log10), a0);
  }

// -----------------------------------------------------------------------------------------------
// 256 bits implementation for avx
  template<floating_scalar_value T, typename N>
  EVE_FORCEINLINE auto
  log10_(EVE_SUPPORTS(avx_),
         wide<T, N> const& v) noexcept requires std::same_as<abi_t<T, N>, x86_256_>
  {
    if constexpr( current_api < avx2 ) return plain(log10)(v);
    else return log10_(EVE_RETARGET(cpu_), v);
  }
}

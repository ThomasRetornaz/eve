//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2020 Joel FALCOU
  Copyright 2020 Jean-Thierry LAPRESTE

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <eve/function/ifrexp.hpp>
#include <tuple>

TTS_CASE_TPL("Check ifrexp return type", EVE_TYPE)
{
  TTS_EXPR_IS(eve::ifrexp(T()), (std::tuple<T,eve::detail::as_integer_t<T,signed>>));
}

TTS_CASE_TPL("Check (eve::ifrexp behavior", EVE_TYPE)
{
  using i_t = eve::detail::as_integer_t<T,signed>;
  {
    auto [p0, p1] = eve::ifrexp(T(1));
    TTS_EQUAL(p0, T(0.5));
    TTS_EQUAL(p1, i_t(1));
  }
  {
    auto [p0, p1] = eve::ifrexp(T(0));
    TTS_EQUAL (p0 , T(0));
    TTS_EQUAL (p1, i_t(0));
  }
}

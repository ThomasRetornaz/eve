//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2019 Joel FALCOU
  Copyright 2019 Jean-Thierry LAPRESTE

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef EVE_DETAIL_OVERLOAD_HPP_INCLUDED
#define EVE_DETAIL_OVERLOAD_HPP_INCLUDED

#include <eve/arch/spec.hpp>
#include <eve/detail/abi.hpp>
#include <utility>

#define EVE_DECLARE_CALLABLE(TAG)                                                                  \
  namespace tag                                                                                    \
  {                                                                                                \
    struct TAG                                                                                     \
    {                                                                                              \
    };                                                                                             \
  }                                                                                                \
  namespace detail                                                                                 \
  {                                                                                                \
    template<typename Dummy>                                                                       \
    struct  callable_object<tag::TAG, void, Dummy>                                                 \
          : conditionnal_support<tag::TAG, eve::supports_conditionnal<tag::TAG>::value>            \
    {                                                                                              \
      using tag_type = tag::TAG;                                                                   \
      template<typename... Args>                                                                   \
      EVE_FORCEINLINE constexpr auto operator()(Args &&... args) const noexcept                    \
          -> decltype(TAG(delay_t{}, EVE_CURRENT_API{}, std::forward<Args>(args)...))              \
      {                                                                                            \
        return TAG(delay_t{}, EVE_CURRENT_API{}, std::forward<Args>(args)...);                     \
      };                                                                                           \
    };                                                                                             \
                                                                                                   \
    template<typename Mode, typename Dummy>                                                        \
    struct callable_object<tag::TAG, Mode, Dummy>                                                  \
    {                                                                                              \
      Mode state_;                                                                                 \
      using tag_type = tag::TAG;                                                                   \
      template<typename... Args>                                                                   \
      EVE_FORCEINLINE constexpr auto operator()(Args &&... args) const noexcept                    \
          -> decltype(TAG(delay_t{}, EVE_CURRENT_API{}, Mode{}, std::forward<Args>(args)...))      \
      {                                                                                            \
        return TAG(delay_t{}, EVE_CURRENT_API{}, state_, std::forward<Args>(args)...);             \
      };                                                                                           \
    };                                                                                             \
  }                                                                                                \
  /**/

#define EVE_MAKE_CALLABLE(TAG, NAME)                                                               \
  EVE_DECLARE_CALLABLE(TAG)                                                                        \
  using callable_##TAG                                = detail::callable_object<tag::TAG>;         \
  inline detail::callable_object<tag::TAG> const NAME = {} /**/

// Flag a function to support delayed calls on given architecture
#define EVE_SUPPORTS(ARCH) delay_t const &, ARCH const &

// Flag a function to support delayed calls on given architecture
#define EVE_RETARGET(ARCH)                                                                         \
  delay_t{}, ARCH {}

// Create named object for consatnt
#define EVE_MAKE_NAMED_CONSTANT(TAG, FUNC)                                                         \
  namespace detail                                                                                 \
  {                                                                                                \
    template<typename T>                                                                           \
    EVE_FORCEINLINE constexpr auto TAG(EVE_SUPPORTS(cpu_), as_<T> const &) noexcept                \
    {                                                                                              \
      return FUNC<T>();                                                                            \
    }                                                                                              \
  }                                                                                                \
/**/

// basic type to support delayed calls
namespace eve
{
  template<typename Tag>
  struct supports_conditionnal : std::true_type
  {};

  namespace detail
  {
    struct delay_t
    {
    };

    template<typename T>
    struct not_t
    {
      T value;
    };

    template<typename Tag, typename Mode = void, typename Enabler = void>
    struct callable_object;

    template<typename Tag, bool isSUpported>
    struct conditionnal_support
    {
      template<typename Condition>
      EVE_FORCEINLINE constexpr auto operator[](Condition const &o) const noexcept
      {
        return callable_object<Tag, Condition>{o};
      }

      struct negator
      {
        template<typename Condition>
        EVE_FORCEINLINE constexpr auto operator[](Condition const &o) const noexcept
        {
          return callable_object<Tag, not_t<Condition>>{o};
        }
      };

      negator not_;
    };

    template<typename Tag> struct conditionnal_support<Tag,false> {};
  }
}

#endif

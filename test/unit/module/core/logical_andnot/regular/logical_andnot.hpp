//==================================================================================================
/**
  EVE - Expressive Vector Engine
  Copyright 2019 Joel FALCOU
  Copyright 2019 Jean-Thierry LAPRESTE

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <eve/function/logical_andnot.hpp>
#include <eve/constant/false.hpp>
#include <eve/constant/true.hpp>
#include <tts/tests/relation.hpp>
#include <tts/tests/types.hpp>

TTS_CASE("Check eve::logical_andnot return type")
{
  using eve::logical;

  TTS_EXPR_IS((eve::logical_andnot(Type()         , Type())         ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(logical<Type>(), Type())         ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(logical<Type>(), logical<Type>())), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(Type()         , logical<Type>())), (logical<Type>));

  TTS_EXPR_IS((eve::logical_andnot(Value()         , Type())           ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(Type()          , Value())          ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(logical<Type>() , Value())          ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(logical<Value>(), Type())           ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(logical<Value>(), logical<Type>())  ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(logical<Type>() , logical<Value>()) ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(Value()         , logical<Type>())  ), (logical<Type>));
  TTS_EXPR_IS((eve::logical_andnot(Type()          , logical<Value>()) ), (logical<Type>));
}

TTS_CASE("Check eve::logical_andnot behavior on scalars")
{
  TTS_EQUAL(eve::logical_andnot((Type(0)), (Type(0))), eve::False<Type>() );
  TTS_EQUAL(eve::logical_andnot((Type(0)), (Type(1))), eve::False<Type>() );
  TTS_EQUAL(eve::logical_andnot((Type(2)), (Type(1))), eve::False<Type>() );
  TTS_EQUAL(eve::logical_andnot((Type(1)), (Type(0))), eve::True<Type>()  );

  TTS_EQUAL(eve::logical_andnot(Value(0), (Type(0))), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(Value(0), (Type(1))), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(Value(2), (Type(1))), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(Value(1), (Type(0))), eve::True<Type>() );

  TTS_EQUAL(eve::logical_andnot((Type(0)), Value(0)), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot((Type(0)), Value(1)), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot((Type(2)), Value(1)), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot((Type(1)), Value(0)), eve::True<Type>() );
}

TTS_CASE("Check eve::logical_andnot behavior on logicals")
{
  TTS_EQUAL(eve::logical_andnot(eve::False<Type>() , eve::False<Type>()) , eve::False<Type>() );
  TTS_EQUAL(eve::logical_andnot(eve::False<Type>() , eve::True<Type>() ) , eve::False<Type>() );
  TTS_EQUAL(eve::logical_andnot(eve::True<Type>()  , eve::True<Type>() ) , eve::False<Type>() );
  TTS_EQUAL(eve::logical_andnot(eve::True<Type>()  , eve::False<Type>()) , eve::True<Type>()  );

  TTS_EQUAL(eve::logical_andnot(eve::False<Value>(), eve::False<Type>()), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(eve::False<Value>(), eve::True<Type>() ), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(eve::True<Value>() , eve::True<Type>() ), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(eve::True<Value>() , eve::False<Type>()), eve::True<Type>() );

  TTS_EQUAL(eve::logical_andnot(eve::False<Type>() , eve::False<Value>() ), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(eve::False<Type>() , eve::True<Value>()  ), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(eve::True<Type>()  , eve::True<Value>()  ), eve::False<Type>());
  TTS_EQUAL(eve::logical_andnot(eve::True<Type>()  , eve::False<Value>() ), eve::True<Type>() );
}

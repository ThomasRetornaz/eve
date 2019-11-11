#include <eve/function/none.hpp>
#include <eve/wide.hpp>

using wide_ft = eve::wide<float, eve::fixed<4>>;
using wide_it = eve::wide<std::int16_t, eve::fixed<4>>;

int main()
{
  wide_ft pf = {-1.0f, 0.0f, -3.0f, -32768.0f};
  wide_it qf = {-1.0f, 2.0f, -3.0f, -32768.0f};

  std::cout << "---- simd" << '\n'
            << "<- pf =                            " << pf << '\n'
            << "-> eve::none(pf) =                  " << eve::none(pf) << '\n'
            << "<- qf =                            " << qf << '\n'
            << "-> eve::none(qf) =                  " << eve::none(qf) << '\n';

  float        xf = -0.0f;
  float        yf = -3.0f;

  std::cout << "---- scalar" << '\n'
            << "<- xf =                            " << xf << '\n'
            << "-> eve::none(xf) =                  " << eve::none(xf) << '\n'
            << "<- yf =                            " << yf << '\n'
            << "-> eve::none(yf) =                  " << eve::none(yf) << '\n';
  return 0;
}

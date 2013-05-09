#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

// Source: http://www.boost.org/doc/libs/1_52_0/more/getting_started/unix-variants.html
// To compile: c++ -I path/to/boost_1_52_0 example.cpp -o example
// To run: echo 1 2 3 | ./example

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " " );
}

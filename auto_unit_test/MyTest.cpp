#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( free_test_function )
/* Compare with void free_test_function() */
{
 size_t test = 2u;
 BOOST_REQUIRE_EQUAL(test, 2u);
}

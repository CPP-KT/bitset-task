#include "bitset.h"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

namespace ct::test {

TEST_CASE("member types") {
  SECTION("bitset") {
    STATIC_CHECK(std::is_same_v<BitSet::Value, bool>);
    STATIC_CHECK_FALSE(std::is_same_v<BitSet::Reference, bool>);
    STATIC_CHECK(std::numeric_limits<BitSet::Word>::digits >= 32);
  }

  SECTION("iterators") {
    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::Iterator>::value_type, BitSet::Value>);
    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::ConstIterator>::value_type, BitSet::Value>);

    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::Iterator>::reference, BitSet::Reference>);
    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::ConstIterator>::reference, BitSet::ConstReference>);

    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::Iterator>::difference_type, std::ptrdiff_t>);
    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::ConstIterator>::difference_type, std::ptrdiff_t>);

    STATIC_CHECK(std::is_same_v<
                 std::iterator_traits<BitSet::Iterator>::iterator_category,
                 std::random_access_iterator_tag>);
    STATIC_CHECK(std::is_same_v<
                 std::iterator_traits<BitSet::ConstIterator>::iterator_category,
                 std::random_access_iterator_tag>);

    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::Iterator>::pointer, void>);
    STATIC_CHECK(std::is_same_v<std::iterator_traits<BitSet::ConstIterator>::pointer, void>);
  }

  SECTION("views") {
    STATIC_CHECK(std::is_same_v<BitSet::View::Value, BitSet::Value>);
    STATIC_CHECK(std::is_same_v<BitSet::ConstView::Value, BitSet::Value>);

    STATIC_CHECK(std::is_same_v<BitSet::View::Reference, BitSet::Reference>);
    STATIC_CHECK(std::is_same_v<BitSet::View::ConstReference, BitSet::ConstReference>);
    STATIC_CHECK(std::is_same_v<BitSet::ConstView::Reference, BitSet::ConstReference>);
    STATIC_CHECK(std::is_same_v<BitSet::ConstView::ConstReference, BitSet::ConstReference>);

    STATIC_CHECK(std::is_same_v<BitSet::ConstView::Iterator, BitSet::Iterator>);
    STATIC_CHECK(std::is_same_v<BitSet::ConstView::ConstIterator, BitSet::ConstIterator>);
  }
}

TEST_CASE("concepts") {
  STATIC_CHECK(std::random_access_iterator<BitSet::ConstIterator>);
  STATIC_CHECK(std::random_access_iterator<BitSet::Iterator>);
  STATIC_CHECK(std::output_iterator<BitSet::Iterator, bool>);
}

} // namespace ct::test

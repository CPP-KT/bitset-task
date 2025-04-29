#include "bitset.h"
#include "test-helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <sstream>
#include <utility>

namespace ct::test {

TEST_CASE("instantiations") {
  BitSet bs(1, false);

  SECTION("operations") {
    REQUIRE((bs & bs) == BitSet("0"));
    REQUIRE((bs | bs) == BitSet("0"));
    REQUIRE((bs ^ bs) == BitSet("0"));
  }

  SECTION("reference") {
    BitSet::Reference ref = bs[0];
    ref.flip();
    ref = false;
    BitSet::ConstReference const_ref = ref;
    const BitSet::Reference c_ref_copy = ref;
    BitSet::Reference ref_copy = c_ref_copy;
    REQUIRE(ref_copy == c_ref_copy);
    REQUIRE_FALSE(static_cast<bool>(ref));
    REQUIRE_FALSE(static_cast<bool>(const_ref));
  }

  SECTION("iterator") {
    BitSet::Iterator it;
    it = bs.begin();
    BitSet::ConstIterator cit;
    cit = it;

    *it = false;
    it++;
    it--;
    ++it;
    --it;
    cit++;
    cit--;
    ++cit;
    --cit;
    it += 0;
    it -= 0;
    cit += 0;
    cit -= 0;
    BitSet::Iterator it2 = 0 + it + 0 - 0;
    BitSet::ConstIterator cit2 = 0 + cit + 0 - 0;
    REQUIRE_FALSE(*it);
    REQUIRE_FALSE(*cit);
    REQUIRE_FALSE(it[0]);
    REQUIRE_FALSE(cit[0]);
    REQUIRE(it - it2 == 0);
    REQUIRE(cit - cit2 == 0);
    REQUIRE(it == it2);
    REQUIRE_FALSE(it != it2);
    REQUIRE_FALSE(it < it2);
    REQUIRE_FALSE(it > it2);
    REQUIRE(it <= it2);
    REQUIRE(it >= it2);
    REQUIRE(cit == cit2);
    REQUIRE_FALSE(cit != cit2);
    REQUIRE_FALSE(cit < cit2);
    REQUIRE_FALSE(cit > cit2);
    REQUIRE(cit <= cit2);
    REQUIRE(cit >= cit2);
  }

  SECTION("views") {
    BitSet::View view = bs;
    BitSet::View view_copy = view;
    BitSet::View view_it(view.begin(), view.end());
    BitSet::View view_sub = view.subview();
    BitSet::ConstView const_view_converted = view;
    BitSet::ConstView const_view = bs;
    BitSet::ConstView const_view_copy = const_view;
    BitSet::ConstView const_view_it(const_view.begin(), const_view.end());
    BitSet::ConstView const_view_sub = const_view.subview();

    view_copy.swap(view_it);
    const_view_copy.swap(const_view_it);
    std::swap(view_copy, view_it);
    std::swap(const_view_copy, const_view_it);

    REQUIRE(view.size() == 1);
    REQUIRE(view_copy.size() == 1);
    REQUIRE(view_it.size() == 1);
    REQUIRE(view_sub.size() == 1);
    REQUIRE(const_view_converted.size() == 1);
    REQUIRE(const_view.size() == 1);
    REQUIRE(const_view_copy.size() == 1);
    REQUIRE(const_view_it.size() == 1);
    REQUIRE(const_view_sub.size() == 1);

    REQUIRE_FALSE(view[0]);
    REQUIRE_FALSE(const_view[0]);
    REQUIRE_FALSE(view.any());
    REQUIRE_FALSE(const_view.any());
    REQUIRE_FALSE(view.all());
    REQUIRE_FALSE(const_view.all());
    REQUIRE(view.count() == 0);
    REQUIRE(const_view.count() == 0);
    REQUIRE_FALSE(view.empty());
    REQUIRE_FALSE(const_view.empty());

    REQUIRE(view == view);
    REQUIRE_FALSE(view != view);
    REQUIRE(const_view == const_view);
    REQUIRE_FALSE(const_view != const_view);

    view.set();
    view.reset();
    view.flip();
    view &= BitSet(1, false).subview(); // No memory intersection
    view |= BitSet(1, false).subview();
    view ^= BitSet(1, false).subview();

    const BitSet::View c_view = view;
    c_view.set();
    c_view.reset();
    c_view.flip();
    c_view.subview();

    c_view.reset();
    REQUIRE((view & view) == BitSet("0"));
    REQUIRE((view | view) == BitSet("0"));
    REQUIRE((view ^ view) == BitSet("0"));
    REQUIRE((~view) == BitSet("1"));
    REQUIRE((view << 0) == BitSet("0"));
    REQUIRE((view >> 0) == BitSet("0"));
    REQUIRE((const_view & const_view) == BitSet("0"));
    REQUIRE((const_view | const_view) == BitSet("0"));
    REQUIRE((const_view ^ const_view) == BitSet("0"));
    REQUIRE((~const_view) == BitSet("1"));
    REQUIRE((const_view << 0) == BitSet("0"));
    REQUIRE((const_view >> 0) == BitSet("0"));

    REQUIRE(view == view);
    REQUIRE_FALSE(view != view);
    REQUIRE(const_view == const_view);
    REQUIRE_FALSE(const_view != const_view);

    REQUIRE(to_string(view) == std::string("0"));
    REQUIRE(to_string(const_view) == std::string("0"));

    std::ostringstream istr_1;
    istr_1 << view;
    REQUIRE(istr_1.str() == std::string("0"));

    std::ostringstream istr_2;
    istr_2 << const_view;
    REQUIRE(istr_2.str() == std::string("0"));

    REQUIRE(bs == view);
    REQUIRE_FALSE(bs != view);
  }

  SECTION("assign overloads") {
    BitSet copy = bs;
    BitSet::View view = bs.subview();
    BitSet::ConstView const_view = bs.subview();

    copy = view;
    copy = const_view;
    const_view = view;
  }
}

} // namespace ct::test

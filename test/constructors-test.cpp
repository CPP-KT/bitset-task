#include "bitset.h"

#include "test-helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <string>

TEST_CASE("bitset default constructor") {
  bitset bs;

  CHECK(bs.empty());
  CHECK(bs.size() == 0);
  CHECK(bs.begin() == bs.end());
}

TEST_CASE("bitset constructor from size and vaue") {
  bool bit = GENERATE(false, true);
  CAPTURE(bit);

  SECTION("empty") {
    bitset bs(0, bit);

    CHECK(bs.empty());
    CHECK(bs.size() == 0);
    CHECK(bs.begin() == bs.end());
  }

  SECTION("single word") {
    bitset bs(7, bit);

    CHECK_FALSE(bs.empty());
    CHECK(bs.size() == 7);
    CHECK(bs.begin() + 7 == bs.end());

    for (std::size_t i = 0; i < 7; ++i) {
      CAPTURE(i);
      REQUIRE(bs[i] == bit);
    }
  }

  SECTION("multiple words") {
    bitset bs(80, bit);

    CHECK_FALSE(bs.empty());
    CHECK(bs.size() == 80);
    CHECK(bs.begin() + 80 == bs.end());

    for (std::size_t i = 0; i < 80; ++i) {
      CAPTURE(i);
      REQUIRE(bs[i] == bit);
    }
  }
}

TEST_CASE("bitset constructor from string") {
  SECTION("empty") {
    const bitset bs("");

    CHECK(bs.empty());
    CHECK(bs.size() == 0);
    CHECK(bs.begin() == bs.end());
  }

  SECTION("single word") {
    std::string_view str = "1101101";
    const bitset bs(str);

    CHECK_THAT(bs, bitset_equals_string(str));
  }

  SECTION("multiple words") {
    std::string_view str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    const bitset bs(str);

    CHECK_THAT(bs, bitset_equals_string(str));
  }
}

TEST_CASE("bitset copy constructor") {
  SECTION("empty") {
    const bitset bs;
    bitset copy = bs;

    CHECK(bs.empty());
    CHECK(bs.size() == 0);
    CHECK(bs.begin() == bs.end());
  }

  SECTION("single word") {
    std::string_view str = "1101101";
    const bitset bs(str);
    bitset copy = bs;

    CHECK_THAT(bs, bitset_equals_string(str));
  }

  SECTION("multiple words") {
    std::string_view str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    const bitset bs(str);
    bitset copy = bs;

    CHECK_THAT(bs, bitset_equals_string(str));
  }
}

TEST_CASE("bitset constructor from view or iterators") {
  SECTION("empty") {
    const bitset source("1101101");

    SECTION("view") {
      bitset bs(source.subview(3, 0));

      CHECK(bs.empty());
      CHECK(bs.size() == 0);
      CHECK(bs.begin() == bs.end());
    }

    SECTION("iterators") {
      bitset bs(source.begin() + 3, source.begin() + 3);

      CHECK(bs.empty());
      CHECK(bs.size() == 0);
      CHECK(bs.begin() == bs.end());
    }
  }

  SECTION("single word") {
    std::string_view str = "1101101";
    const bitset source(str);
    std::size_t offset = GENERATE(2, 3);
    CAPTURE(offset);

    SECTION("view") {
      bitset bs(source.subview(offset, 5));
      CHECK_THAT(bs, bitset_equals_string(str.substr(offset, 5)));
    }

    SECTION("iterators") {
      bitset bs(source.begin() + offset, source.begin() + std::min(offset + 5, str.size()));
      CHECK_THAT(bs, bitset_equals_string(str.substr(offset, 5)));
    }
  }

  SECTION("multiple words") {
    std::string_view str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    const bitset source(str);

    auto [offset, count] = GENERATE(table<std::size_t, std::size_t>({
        {0, 80},
        {1, 79},
        {60, 10},
        {10, 60},
    }));
    CAPTURE(offset, count);

    SECTION("view") {
      bitset bs(source.subview(offset, count));
      CHECK_THAT(bs, bitset_equals_string(str.substr(offset, count)));
    }

    SECTION("iterators") {
      bitset bs(source.begin() + offset, source.begin() + offset + count);
      CHECK_THAT(bs, bitset_equals_string(str.substr(offset, count)));
    }
  }
}

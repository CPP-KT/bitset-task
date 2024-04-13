#include "bitset.h"
#include "test-helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <array>
#include <utility>

TEST_CASE("left shift") {
  SECTION("empty") {
    bitset bs;
    std::string str;

    SECTION("without reallocation") {
      std::size_t shift_count = GENERATE(0, 1, 5);
      CAPTURE(shift_count);

      bs <<= shift_count;

      str.append(shift_count, '0');
      REQUIRE_THAT(bs, bitset_equals_string(str));
    }

    SECTION("with reallocation") {
      std::size_t shift_count = GENERATE(64, 190);
      CAPTURE(shift_count);

      bs <<= shift_count;

      str.append(shift_count, '0');
      REQUIRE_THAT(bs, bitset_equals_string(str));
    }
  }

  SECTION("single word") {
    std::string str = "1101101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;

    str.append(shift_count, '0');
    REQUIRE_THAT(bs, bitset_equals_string(str));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;

    str.append(shift_count, '0');
    REQUIRE_THAT(bs, bitset_equals_string(str));
  }
}

TEST_CASE("right shift") {
  SECTION("empty") {
    bitset bs;

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    REQUIRE(bs.empty());
    REQUIRE(bs.size() == 0);
    REQUIRE(bs.begin() == bs.end());
  }

  SECTION("single word") {
    std::string str = "1101101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    REQUIRE_THAT(bs, bitset_equals_string(str));
  }

  SECTION("single word") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    REQUIRE_THAT(bs, bitset_equals_string(str));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    REQUIRE_THAT(bs, bitset_equals_string(str));
  }
}

TEST_CASE("bitwise operations") {
  SECTION("empty") {
    bitset bs;

    bs &= bs;
    REQUIRE(bs.empty());

    bs |= bs;
    REQUIRE(bs.empty());

    bs ^= bs;
    REQUIRE(bs.empty());

    bs.flip();
    REQUIRE(bs.empty());

    bs.set();
    REQUIRE(bs.empty());

    bs.reset();
    REQUIRE(bs.empty());
  }

  SECTION("single bit") {
    bool lhs_bit = GENERATE(false, true);
    CAPTURE(lhs_bit);
    bitset lhs(1, lhs_bit);

    SECTION("flip") {
      lhs.flip();
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == !lhs_bit);
    }

    SECTION("set") {
      lhs.set();
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == true);
    }

    SECTION("reset") {
      lhs.reset();
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == false);
    }

    bool rhs_bit = GENERATE(false, true);
    CAPTURE(rhs_bit);
    bitset rhs(1, rhs_bit);

    SECTION("bitwise and") {
      lhs &= rhs;
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit && rhs_bit));
    }

    SECTION("bitwise or") {
      lhs |= rhs;
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit || rhs_bit));
    }

    SECTION("bitwise xor") {
      lhs ^= rhs;
      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit != rhs_bit));
    }
  }

  SECTION("single word") {
    std::string_view lhs_str = "1101101";
    bitset lhs(lhs_str);

    SECTION("flip") {
      lhs.flip();
      REQUIRE_THAT(lhs, bitset_equals_string("0010010"));
    }

    SECTION("set") {
      lhs.set();
      REQUIRE_THAT(lhs, bitset_equals_string("1111111"));
    }

    SECTION("reset") {
      lhs.reset();
      REQUIRE_THAT(lhs, bitset_equals_string("0000000"));
    }

    std::string_view rhs_str = "0111001";
    CAPTURE(rhs_str);
    bitset rhs(rhs_str);

    SECTION("bitwise and") {
      lhs &= rhs;
      REQUIRE_THAT(lhs, bitset_equals_string("0101001"));
    }

    SECTION("bitwise or") {
      lhs |= rhs;
      REQUIRE_THAT(lhs, bitset_equals_string("1111101"));
    }

    SECTION("bitwise xor") {
      lhs ^= rhs;
      REQUIRE_THAT(lhs, bitset_equals_string("1010100"));
    }
  }

  SECTION("multiple words") {
    std::string_view lhs_str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    bitset lhs(lhs_str);

    SECTION("flip") {
      lhs.flip();
      REQUIRE_THAT(
          lhs,
          bitset_equals_string("00001001000101111011010000010111100100000000111110011001101101110100011011001010")
      );
    }

    SECTION("set") {
      lhs.set();
      REQUIRE_THAT(
          lhs,
          bitset_equals_string("11111111111111111111111111111111111111111111111111111111111111111111111111111111")
      );
    }

    SECTION("reset") {
      lhs.reset();
      REQUIRE_THAT(
          lhs,
          bitset_equals_string("00000000000000000000000000000000000000000000000000000000000000000000000000000000")
      );
    }

    std::string_view rhs_str = "00011110011010000111001101110001000001000010001001011110010010110111011110111111";
    CAPTURE(rhs_str);
    bitset rhs(rhs_str);

    SECTION("bitwise and") {
      lhs &= rhs;
      REQUIRE_THAT(
          lhs,
          bitset_equals_string("00010110011010000100001101100000000001000010000001000110010010000011000100110101")
      );
    }

    SECTION("bitwise or") {
      lhs |= rhs;
      REQUIRE_THAT(
          lhs,
          bitset_equals_string("11111110111010000111101111111001011011111111001001111110010010111111111110111111")
      );
    }

    SECTION("bitwise xor") {
      lhs ^= rhs;
      REQUIRE_THAT(
          lhs,
          bitset_equals_string("11101000100000000011100010011001011010111101001000111000000000111100111010001010")
      );
    }
  }
}

TEST_CASE("bitset::all/any/count") {
  SECTION("empty") {
    bitset bs;
    REQUIRE(bs.all() == true);
    REQUIRE(bs.any() == false);
    REQUIRE(bs.count() == 0);
  }

  SECTION("single word") {
    std::string_view str = GENERATE("0000000", "1111111", "1101101");
    CAPTURE(str);

    std::size_t ones = std::ranges::count(str, '1');
    bitset bs(str);
    REQUIRE(bs.all() == (ones == str.size()));
    REQUIRE(bs.any() == (ones != 0));
    REQUIRE(bs.count() == ones);
  }

  SECTION("multiple words") {
    std::string_view str = GENERATE(
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "11111111111111111111111111111111111111111111111111111111111111111111111111111111",
        "11110110111010000100101111101000011011111111000001100110010010001011100100110101"
    );
    CAPTURE(str);

    std::size_t ones = std::ranges::count(str, '1');
    bitset bs(str);
    REQUIRE(bs.all() == (ones == str.size()));
    REQUIRE(bs.any() == (ones != 0));
    REQUIRE(bs.count() == ones);
  }
}

TEST_CASE("bitset comparison") {
  SECTION("empty") {
    bitset bs_1;
    bitset bs_2;

    REQUIRE(bs_1 == bs_2);
    REQUIRE_FALSE(bs_1 != bs_2);

    REQUIRE(bs_1 == bs_1);
    REQUIRE_FALSE(bs_1 != bs_1);
  }

  SECTION("single word") {
    std::string_view str_1 = GENERATE("10110", "10111", "101101", "101110");
    std::string_view str_2 = GENERATE("10110", "10111", "101101", "101110");
    CAPTURE(str_1, str_2);

    const bitset bs_1(str_1);
    const bitset bs_2(str_2);

    if (str_1 == str_2) {
      REQUIRE(bs_1 == bs_2);
      REQUIRE_FALSE(bs_1 != bs_2);
    } else {
      REQUIRE(bs_1 != bs_2);
      REQUIRE_FALSE(bs_1 == bs_2);
    }
  }

  SECTION("multiple words") {
    std::array strings = {
        "1111011011101000010010111110100001101111111100000110011001001",
        "11110110111010000100101111101000011011111111000001100110010010001011100100110101",
        "11110110111010000100101111101000011011111111000001100110010010000000000000000000",
    };
    std::string_view str_1 = GENERATE_REF(from_range(strings));
    std::string_view str_2 = GENERATE_REF(from_range(strings));
    CAPTURE(str_1, str_2);

    const bitset bs_1(str_1);
    const bitset bs_2(str_2);

    if (str_1 == str_2) {
      REQUIRE(bs_1 == bs_2);
      REQUIRE_FALSE(bs_1 != bs_2);
    } else {
      REQUIRE(bs_1 != bs_2);
      REQUIRE_FALSE(bs_1 == bs_2);
    }
  }
}
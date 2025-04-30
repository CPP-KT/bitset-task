#include "bitset.h"
#include "test-helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <algorithm>
#include <array>
#include <utility>

namespace ct::test {

TEST_CASE("left shift") {
  SECTION("empty") {
    BitSet bs;
    std::string str;

    SECTION("without reallocation") {
      std::size_t shift_count = GENERATE(0, 1, 5);
      CAPTURE(shift_count);

      bs <<= shift_count;

      str.append(shift_count, '0');
      CHECK_THAT(bs, BitSetEqualsString(str));
    }

    SECTION("with reallocation") {
      std::size_t shift_count = GENERATE(64, 190);
      CAPTURE(shift_count);

      bs <<= shift_count;

      str.append(shift_count, '0');
      CHECK_THAT(bs, BitSetEqualsString(str));
    }
  }

  SECTION("single word") {
    std::string str = "1101101";
    BitSet bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;

    str.append(shift_count, '0');
    CHECK_THAT(bs, BitSetEqualsString(str));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    BitSet bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;

    str.append(shift_count, '0');
    CHECK_THAT(bs, BitSetEqualsString(str));
  }
}

TEST_CASE("right shift") {
  SECTION("empty") {
    BitSet bs;

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    CHECK(bs.empty());
    CHECK(bs.size() == 0);
    CHECK(bs.begin() == bs.end());
  }

  SECTION("single word") {
    std::string str = "1101101";
    BitSet bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    CHECK_THAT(bs, BitSetEqualsString(str));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    BitSet bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs >>= shift_count;

    std::size_t erased = std::min(shift_count, str.size());
    str.erase(str.size() - erased);

    CHECK_THAT(bs, BitSetEqualsString(str));
  }
}

TEST_CASE("left-right shifts with flip") {
  SECTION("single word") {
    std::string str = "1101101";
    BitSet bs(str);
    bs <<= 1;
    bs.flip();
    (bs >>= 1) <<= 1;

    CHECK_THAT(bs, BitSetEqualsString("00100100"));
  }

  SECTION("multiple words") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    std::string inv = "00001001000101111011010000010111100100000000111110011001101101110100011011001010";
    BitSet bs(str);

    std::size_t shift_count = GENERATE(0, 1, 5, 64, 190);
    CAPTURE(shift_count);

    bs <<= shift_count;
    bs.flip();
    (bs >>= shift_count) <<= shift_count;

    CHECK_THAT(bs, BitSetEqualsString(inv + std::string(shift_count, '0')));
  }
}

TEST_CASE("bitwise operations") {
  SECTION("empty") {
    BitSet bs;

    bs &= bs;
    REQUIRE(bs.empty());
    REQUIRE((bs & bs).empty());

    bs |= bs;
    REQUIRE(bs.empty());
    REQUIRE((bs | bs).empty());

    bs ^= bs;
    REQUIRE(bs.empty());
    REQUIRE((bs ^ bs).empty());

    bs.flip();
    REQUIRE(bs.empty());
    REQUIRE((~bs).empty());

    bs.set();
    REQUIRE(bs.empty());

    bs.reset();
    REQUIRE(bs.empty());

    CHECK_THAT(bs << 2, BitSetEqualsString("00"));
  }

  SECTION("single bit") {
    bool lhs_bit = GENERATE(false, true);
    CAPTURE(lhs_bit);
    BitSet lhs(1, lhs_bit);

    SECTION("flip") {
      BitSet result = ~std::as_const(lhs);
      lhs.flip();

      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == !lhs_bit);

      REQUIRE(result.size() == 1);
      REQUIRE(result[0] == !lhs_bit);
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

    SECTION("bit shift left") {
      BitSet result = std::as_const(lhs) >> 1;
      lhs >>= 1;

      REQUIRE(lhs.empty());
      REQUIRE(result.empty());
    }

    SECTION("bit shift right") {
      BitSet result = std::as_const(lhs) << 2;
      lhs <<= 2;

      REQUIRE(lhs.size() == 3);
      REQUIRE(result.size() == 3);

      REQUIRE(lhs[0] == lhs_bit);
      REQUIRE(lhs[1] == 0);
      REQUIRE(lhs[2] == 0);
      REQUIRE(result[0] == lhs_bit);
      REQUIRE(result[1] == 0);
      REQUIRE(result[2] == 0);
    }

    bool rhs_bit = GENERATE(false, true);
    CAPTURE(rhs_bit);
    const BitSet rhs(1, rhs_bit);

    SECTION("bitwise and") {
      BitSet result = std::as_const(lhs) & rhs;
      lhs &= rhs;

      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit && rhs_bit));

      REQUIRE(result.size() == 1);
      REQUIRE(result[0] == (lhs_bit && rhs_bit));
    }

    SECTION("bitwise or") {
      BitSet result = std::as_const(lhs) | rhs;
      lhs |= rhs;

      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit || rhs_bit));

      REQUIRE(result.size() == 1);
      REQUIRE(result[0] == (lhs_bit || rhs_bit));
    }

    SECTION("bitwise xor") {
      BitSet result = std::as_const(lhs) ^ rhs;
      lhs ^= rhs;

      REQUIRE(lhs.size() == 1);
      REQUIRE(lhs[0] == (lhs_bit != rhs_bit));

      REQUIRE(result.size() == 1);
      REQUIRE(result[0] == (lhs_bit != rhs_bit));
    }
  }

  SECTION("single word") {
    std::string_view lhs_str = "1101101";
    BitSet lhs(lhs_str);

    SECTION("flip") {
      BitSet result = ~std::as_const(lhs);
      lhs.flip();
      REQUIRE_THAT(lhs, BitSetEqualsString("0010010"));
      REQUIRE_THAT(result, BitSetEqualsString("0010010"));
    }

    SECTION("set") {
      lhs.set();
      REQUIRE_THAT(lhs, BitSetEqualsString("1111111"));
    }

    SECTION("reset") {
      lhs.reset();
      REQUIRE_THAT(lhs, BitSetEqualsString("0000000"));
    }

    SECTION("bit shift left") {
      BitSet result = std::as_const(lhs) >> 2;
      lhs >>= 2;

      REQUIRE_THAT(lhs, BitSetEqualsString("11011"));
      REQUIRE_THAT(result, BitSetEqualsString("11011"));
    }

    SECTION("bit shift right") {
      BitSet result = std::as_const(lhs) << 3;
      lhs <<= 3;

      REQUIRE_THAT(lhs, BitSetEqualsString("1101101000"));
      REQUIRE_THAT(result, BitSetEqualsString("1101101000"));
    }

    std::string_view rhs_str = "0111001";
    CAPTURE(rhs_str);
    const BitSet rhs(rhs_str);

    SECTION("bitwise and") {
      BitSet result = std::as_const(lhs) & rhs;
      lhs &= rhs;
      REQUIRE_THAT(lhs, BitSetEqualsString("0101001"));
      REQUIRE_THAT(result, BitSetEqualsString("0101001"));
    }

    SECTION("bitwise or") {
      BitSet result = std::as_const(lhs) | rhs;
      lhs |= rhs;
      REQUIRE_THAT(lhs, BitSetEqualsString("1111101"));
      REQUIRE_THAT(result, BitSetEqualsString("1111101"));
    }

    SECTION("bitwise xor") {
      BitSet result = std::as_const(lhs) ^ rhs;
      lhs ^= rhs;
      REQUIRE_THAT(lhs, BitSetEqualsString("1010100"));
      REQUIRE_THAT(result, BitSetEqualsString("1010100"));
    }
  }

  SECTION("multiple words") {
    std::string_view lhs_str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    BitSet lhs(lhs_str);

    SECTION("flip") {
      BitSet result = ~std::as_const(lhs);
      lhs.flip();
      REQUIRE_THAT(
          lhs,
          BitSetEqualsString("00001001000101111011010000010111100100000000111110011001101101110100011011001010")
      );
      REQUIRE_THAT(
          result,
          BitSetEqualsString("00001001000101111011010000010111100100000000111110011001101101110100011011001010")
      );
    }

    SECTION("set") {
      lhs.set();
      REQUIRE_THAT(
          lhs,
          BitSetEqualsString("11111111111111111111111111111111111111111111111111111111111111111111111111111111")
      );
    }

    SECTION("reset") {
      lhs.reset();
      REQUIRE_THAT(
          lhs,
          BitSetEqualsString("00000000000000000000000000000000000000000000000000000000000000000000000000000000")
      );
    }

    SECTION("bit shift left") {
      BitSet result = std::as_const(lhs) >> 20;
      lhs >>= 20;

      REQUIRE_THAT(lhs, BitSetEqualsString("111101101110100001001011111010000110111111110000011001100100"));
      REQUIRE_THAT(result, BitSetEqualsString("111101101110100001001011111010000110111111110000011001100100"));
    }

    SECTION("bit shift right") {
      BitSet result = std::as_const(lhs) << 10;
      lhs <<= 10;

      REQUIRE_THAT(
          lhs,
          BitSetEqualsString(
              "111101101110100001001011111010000110111111110000011001100100100010111001001101010000000000"
          )
      );
      REQUIRE_THAT(
          result,
          BitSetEqualsString(
              "111101101110100001001011111010000110111111110000011001100100100010111001001101010000000000"
          )
      );
    }

    std::string_view rhs_str = "00011110011010000111001101110001000001000010001001011110010010110111011110111111";
    CAPTURE(rhs_str);
    const BitSet rhs(rhs_str);

    SECTION("bitwise and") {
      BitSet result = std::as_const(lhs) & rhs;
      lhs &= rhs;
      REQUIRE_THAT(
          lhs,
          BitSetEqualsString("00010110011010000100001101100000000001000010000001000110010010000011000100110101")
      );
      REQUIRE_THAT(
          result,
          BitSetEqualsString("00010110011010000100001101100000000001000010000001000110010010000011000100110101")
      );
    }

    SECTION("bitwise or") {
      BitSet result = std::as_const(lhs) | rhs;
      lhs |= rhs;
      REQUIRE_THAT(
          lhs,
          BitSetEqualsString("11111110111010000111101111111001011011111111001001111110010010111111111110111111")
      );
      REQUIRE_THAT(
          result,
          BitSetEqualsString("11111110111010000111101111111001011011111111001001111110010010111111111110111111")
      );
    }

    SECTION("bitwise xor") {
      BitSet result = std::as_const(lhs) ^ rhs;
      lhs ^= rhs;
      REQUIRE_THAT(
          lhs,
          BitSetEqualsString("11101000100000000011100010011001011010111101001000111000000000111100111010001010")
      );
      REQUIRE_THAT(
          result,
          BitSetEqualsString("11101000100000000011100010011001011010111101001000111000000000111100111010001010")
      );
    }
  }
}

TEST_CASE("bitset assignment") {
  SECTION("assignment from bitset") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    BitSet bs_1("11110110111010000100101111101000011011111111000001100110010010001011100100110101");
    const BitSet bs_2(str);

    bs_1 = bs_2;
    CHECK_THAT(bs_1, BitSetEqualsString(str));
  }

  SECTION("assignment from view") {
    std::string str = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
    BitSet bs_1("11110110111010000100101111101000011011111111000001100110010010001011100100110101");
    const BitSet bs_2("00000" + str + "11111");

    bs_1 = bs_2.subview(5, 80);
    CHECK_THAT(bs_1, BitSetEqualsString(str));
  }
}

TEST_CASE("bitset swap") {
  std::string str_1 = "11110110111010000100101111101000011011111111000001100110010010001011100100110101";
  std::string str_2 = "11010001001101000100110100010011010001001101000100110100010011010001001101000100";
  BitSet bs_1(str_1);
  BitSet bs_2(str_2);

  swap(bs_1, bs_2);

  CHECK_THAT(bs_1, BitSetEqualsString(str_2));
  CHECK_THAT(bs_2, BitSetEqualsString(str_1));
}

TEST_CASE("reference operations") {
  std::string str_1 = "00001100111101111101011000011001001010101001111011101010110010000010001111111100";
  std::string str_2 = "00011101001001000111010101110000110100010100010111110011001100011001010010100000";
  BitSet bs(str_1);

  SECTION("reference assignment") {
    for (std::size_t i = 0; i < bs.size(); i++) {
      CAPTURE(i);
      BitSet::Reference ref = bs[i];
      ref = (str_2[i] == '1');
    }
    CHECK_THAT(bs, BitSetEqualsString(str_2));
  }

  SECTION("reference flip") {
    for (std::size_t i = 0; i < bs.size(); i++) {
      bs[i].flip();
    }
    for (std::size_t i = 0; i < bs.size(); i++) {
      CAPTURE(i);
      CHECK(std::as_const(bs)[i] == (str_1[i] == '0'));
    }
  }
}

TEST_CASE("views swap") {
  std::string str_1 = "10101101001101100100101110111000001010101110101010100000100101001101010001101011";
  std::string str_2 = "10111011111000111100100101000100100100010101000000100000101111100111000011010111";
  const BitSet bs_1("0000" + str_1 + "1111");
  const BitSet bs_2("111" + str_2 + "000");

  BitSet::ConstView view1 = bs_1.subview(4, 80);
  BitSet::ConstView view2 = bs_2.subview(3, 80);

  swap(view1, view2);

  CHECK_THAT(BitSet(view1), BitSetEqualsString(str_2));
  CHECK_THAT(BitSet(view2), BitSetEqualsString(str_1));
}

TEST_CASE("bitset::all/any/count") {
  SECTION("empty") {
    const BitSet bs;
    CHECK(bs.all() == true);
    CHECK(bs.any() == false);
    CHECK(bs.count() == 0);
  }

  SECTION("single word") {
    std::string_view str = GENERATE("0000000", "1111111", "1101101");
    CAPTURE(str);

    std::size_t ones = std::ranges::count(str, '1');
    const BitSet bs(str);
    CHECK(bs.all() == (ones == str.size()));
    CHECK(bs.any() == (ones != 0));
    CHECK(bs.count() == ones);
  }

  SECTION("multiple words") {
    std::string_view str = GENERATE(
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "11111111111111111111111111111111111111111111111111111111111111111111111111111111",
        "11110110111010000100101111101000011011111111000001100110010010001011100100110101"
    );
    CAPTURE(str);

    std::size_t ones = std::ranges::count(str, '1');
    const BitSet bs(str);
    CHECK(bs.all() == (ones == str.size()));
    CHECK(bs.any() == (ones != 0));
    CHECK(bs.count() == ones);
  }
}

TEST_CASE("bitset comparison") {
  SECTION("empty") {
    BitSet bs_1;
    BitSet bs_2;

    CHECK(bs_1 == bs_2);
    CHECK_FALSE(bs_1 != bs_2);

    CHECK(bs_1 == bs_1);
    CHECK_FALSE(bs_1 != bs_1);
  }

  SECTION("single word") {
    std::string_view str_1 = GENERATE("10110", "10111", "101101", "101110");
    std::string_view str_2 = GENERATE("10110", "10111", "101101", "101110");
    CAPTURE(str_1, str_2);

    const BitSet bs_1(str_1);
    const BitSet bs_2(str_2);

    if (str_1 == str_2) {
      CHECK(bs_1 == bs_2);
      CHECK_FALSE(bs_1 != bs_2);
    } else {
      CHECK(bs_1 != bs_2);
      CHECK_FALSE(bs_1 == bs_2);
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

    const BitSet bs_1(str_1);
    const BitSet bs_2(str_2);

    if (str_1 == str_2) {
      CHECK(bs_1 == bs_2);
      CHECK_FALSE(bs_1 != bs_2);
    } else {
      CHECK(bs_1 != bs_2);
      CHECK_FALSE(bs_1 == bs_2);
    }
  }
}

TEST_CASE("view operations") {
  BitSet bs("1110010101");

  const BitSet::ConstView bs_const_view_1 = bs.subview(0, 5);
  const BitSet::ConstView bs_const_view_2 = bs.subview(5);
  const BitSet::View bs_view_1 = bs.subview(0, 5);
  const BitSet::View bs_view_2 = bs.subview(5);

  CHECK((bs_const_view_1.subview() & bs_const_view_2) == BitSet("10100"));
  CHECK((bs_const_view_1 | bs_const_view_2.subview()) == BitSet("11101"));
  CHECK((bs_const_view_1 ^ bs_const_view_2) == BitSet("01001"));
  CHECK((bs_const_view_2 << 2) == BitSet("1010100"));
  CHECK((bs_const_view_1 >> 3) == BitSet("11"));
  CHECK(~bs_const_view_1 == BitSet("00011"));
  CHECK(bs_const_view_1.count() == 3);

  CHECK((bs_view_1.subview() & bs_view_2) == BitSet("10100"));
  CHECK((bs_view_1 | bs_view_2.subview()) == BitSet("11101"));
  CHECK((bs_view_1 ^ bs_view_2) == BitSet("01001"));
  CHECK((bs_view_2 << 2) == BitSet("1010100"));
  CHECK((bs_view_1 >> 3) == BitSet("11"));
  CHECK(~bs_view_1 == BitSet("00011"));
  CHECK(bs_view_1.count() == 3);

  bs_view_1[0] = false;
  CHECK(bs == BitSet("0110010101"));

  bs_view_2.flip();
  CHECK(bs == BitSet("0110001010"));

  const BitSet::View bs_view_3 = bs_view_1.subview(2, 3);
  bs_view_3.set();
  CHECK(bs == BitSet("0111101010"));

  bs.subview(2, 3) ^= bs.subview(7, 3);
  CHECK(bs == BitSet("0110101010"));

  bs.subview() &= std::as_const(bs).subview();
  CHECK(bs == BitSet("0110101010"));
}

TEST_CASE("chained view operations") {
  BitSet bs_1("0011000011");
  BitSet bs_2("1110010101");

  const BitSet::View bs_view_1 = bs_1.subview();
  const BitSet::View bs_view_2 = bs_2.subview();

  bs_view_1 |= bs_view_2.flip();
  CHECK(bs_1 == BitSet("0011101011"));
  CHECK(bs_2 == BitSet("0001101010"));

  bs_view_1.flip() |= bs_2;
  CHECK(bs_1 == BitSet("1101111110"));
  CHECK(bs_2 == BitSet("0001101010"));

  bs_view_1 &= std::as_const(bs_view_2).flip();
  CHECK(bs_1 == BitSet("1100010100"));
  CHECK(bs_2 == BitSet("1110010101"));

  std::as_const(bs_view_1).flip() &= std::as_const(bs_2);
  CHECK(bs_1 == BitSet("0010000001"));
  CHECK(bs_2 == BitSet("1110010101"));

  std::as_const(bs_view_2) ^= bs_1;
  CHECK(bs_1 == BitSet("0010000001"));
  CHECK(bs_2 == BitSet("1100010100"));
}

TEST_CASE("multiple words view operations") {
  BitSet bs1("00110001011000001000100110001110011011010000100001100110101100010000110111011100");
  BitSet bs2("10111010001001111000100011000111011111011010100110101101110000010100110010101011");
  SECTION("bit and") {
    BitSet result("00110000001000001000100010000110011011010000100000100100100000010000110010001000");
    for (size_t l = 1; l < bs1.size(); l++) {
      BitSet temp = bs1;
      for (size_t i = 0; i < temp.size(); i += l) {
        temp.subview(i, l) &= bs2.subview(i, l);
      }
      REQUIRE(temp == result);
    }
  }
  SECTION("bit or") {
    BitSet result("10111011011001111000100111001111011111011010100111101111111100010100110111111111");
    for (size_t l = 1; l < bs1.size(); l++) {
      BitSet temp = bs1;
      for (size_t i = 0; i < temp.size(); i += l) {
        temp.subview(i, l) |= bs2.subview(i, l);
      }
      REQUIRE(temp == result);
    }
  }
  SECTION("bit xor") {
    BitSet result("10001011010001110000000101001001000100001010000111001011011100000100000101110111");
    for (size_t l = 1; l < bs1.size(); l++) {
      BitSet temp = bs1;
      for (size_t i = 0; i < temp.size(); i += l) {
        temp.subview(i, l) ^= bs2.subview(i, l);
      }
      REQUIRE(temp == result);
    }
  }
}

TEST_CASE("iterators with different constness operations") {
  BitSet bs("110101");
  std::ptrdiff_t i = GENERATE(0, 1, 6);
  BitSet::Iterator it = bs.begin() + i;
  BitSet::ConstIterator cit = bs.begin() + i;
  REQUIRE(cit == it);
  REQUIRE(it == cit);
  REQUIRE_FALSE(cit != it);
  REQUIRE_FALSE(it != cit);

  std::ptrdiff_t j = GENERATE(0, 1, 6);
  BitSet::Iterator it2 = bs.begin() + j;
  BitSet::ConstIterator cit2 = it2;
  REQUIRE(cit2 - it == j - i);
  REQUIRE(it2 - cit == j - i);
}

TEST_CASE("reference with different constness operations") {
  BitSet bs("110101");
  std::size_t i = GENERATE(0, 1, 5);
  const BitSet::Reference ref1 = bs[i];
  const BitSet::ConstReference ref2 = bs[i];
  REQUIRE(ref1 == ref2);
  REQUIRE(ref2 == ref1);
  REQUIRE_FALSE(ref1 != ref2);
  REQUIRE_FALSE(ref2 != ref1);
}

TEST_CASE("reference semantics") {
  BitSet bs("10");
  BitSet::Reference ref_1 = bs[0];
  BitSet::Reference ref_copy = ref_1;
  BitSet::Reference ref_2 = bs[1];

  REQUIRE(ref_1);
  REQUIRE(ref_copy);
  REQUIRE_FALSE(ref_2);

  ref_copy = ref_2;

  REQUIRE(ref_1);
  REQUIRE(bs[0]);
  REQUIRE_FALSE(ref_copy);
  REQUIRE_FALSE(ref_2);
}

TEST_CASE("view with different constness operations") {
  BitSet bs("110101");
  std::size_t i = GENERATE(0, 1, 4);
  std::size_t len = GENERATE(0, 1);
  const BitSet::View view1 = bs.subview(i, len);
  const BitSet::ConstView view2 = bs.subview(i, len);
  REQUIRE(view1 == view2);
  REQUIRE_FALSE(view1 != view2);
  REQUIRE(view2 == view1);
  REQUIRE_FALSE(view2 != view1);
}

TEST_CASE("bitset and view comparison") {
  BitSet bs("110101");
  const BitSet::View view = bs;
  const BitSet::ConstView const_view = bs;
  const BitSet& const_bs = bs;

  REQUIRE(const_bs == view);
  REQUIRE(bs == view);
  REQUIRE_FALSE(const_bs != view);
  REQUIRE_FALSE(bs != view);
  REQUIRE(view == const_bs);
  REQUIRE(view == bs);
  REQUIRE_FALSE(view != const_bs);
  REQUIRE_FALSE(view != bs);
  REQUIRE(const_bs == const_view);
  REQUIRE(bs == const_view);
  REQUIRE_FALSE(const_bs != const_view);
  REQUIRE_FALSE(bs != const_view);
  REQUIRE(const_view == const_bs);
  REQUIRE(const_view == bs);
  REQUIRE_FALSE(const_view != const_bs);
  REQUIRE_FALSE(const_view != bs);
  REQUIRE(BitSet::View(BitSet("10")) == BitSet("10"));
  REQUIRE(BitSet::ConstView(BitSet("10")) == BitSet("10"));
}

} // namespace ct::test

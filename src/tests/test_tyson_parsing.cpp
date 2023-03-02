//
// Created by felix on 01.02.23.
//
#include "gtest/gtest.h"
#include "../TySON.hpp"


TEST(tyson_parsing, number_bool_type)
{
    tyson::TySonObject object {"n|10.12345|"};
    ASSERT_EQ(object.type(), tyson::TySonType::Number);

    ASSERT_EQ(object.value<int>(), 10);
    ASSERT_EQ(object.value<short>(), 10);
    ASSERT_EQ(object.value<unsigned int>(), 10);
    ASSERT_EQ(object.value<int>(), 10);
    ASSERT_EQ(object.value<long long>(), 10);
    ASSERT_FLOAT_EQ(object.value<float>(), float(10.12345));
    ASSERT_EQ(object.value<double>(), 10.12345);
    ASSERT_EQ(object.value<bool>(), false);
}

TEST(tyson_parsing, string_type)
{
    tyson::TySonObject long_str_object {"s|Lorem ipsum dolor|"};
    ASSERT_EQ(long_str_object.type(), tyson::TySonType::String);

    tyson::TySonObject single_digit_str {"s|n|"};
    ASSERT_EQ(single_digit_str.type(), tyson::TySonType::String);

    ASSERT_THROW([&single_digit_str](){
        auto val = single_digit_str.value<int>();
        val *= 2;
        }(), std::invalid_argument);
}

TEST(tyson_parsing, null_type)
{
    tyson::TySonObject null_object {"null"};

    ASSERT_EQ(null_object.type(), tyson::TySonType::Null);
    ASSERT_TRUE(null_object.value<tyson::TySonType::Null>().empty());
}

TEST(tyson_parsing, uts_type)
{
    tyson::TySonObject object {"uts|123456789|"};

    ASSERT_EQ(object.type(), tyson::TySonType::Timestamp);
    ASSERT_EQ(object.value<long>(), 123456789);
}

TEST(tyson_parsing, link_type)
{
    tyson::TySonObject object {"users|e0bbcda2-0911-495e-9f0f-ce00db489f10|"};

    ASSERT_EQ(object.type(), tyson::TySonType::Link);

    auto link_obj = object.value<tyson::TySonType::Link>();

    ASSERT_EQ(std::get<0>(link_obj), "users");
    ASSERT_EQ(std::get<1>(link_obj), "e0bbcda2-0911-495e-9f0f-ce00db489f10");
}

TEST(tyson_parsing, vector_type)
{
    tyson::TySonObject object {"v[n|1|,n|2|,n|3|,]"};

    auto expected_value = std::vector<tyson::TySonObject> {};
    expected_value.reserve(3);

    expected_value.emplace_back("n|1|");
    expected_value.emplace_back("n|2|");
    expected_value.emplace_back("n|3|");

    ASSERT_EQ(object.type(), tyson::TySonType::Vector);
    ASSERT_EQ(object.value<tyson::TySonType::Vector>().size(), expected_value.size());

}

TEST(tyson_parsing, map_type)
{
    tyson::TySonObject object {"m{s|name|:s|test_2|,s|num|:n|2|,}"};
    auto first_key = tyson::TySonObject {"s|name|"};
    auto first_val = tyson::TySonObject {"s|test_2|"};

    auto sec_key = tyson::TySonObject {"s|num|"};
    auto sec_val = tyson::TySonObject {"n|2|"};

    std::map<tyson::TySonObject, tyson::TySonObject> tyson_map {};
    tyson_map.try_emplace(first_key, first_val);
    tyson_map.try_emplace(sec_key, sec_val);

    ASSERT_EQ(object.type(), tyson::TySonType::Map);
    ASSERT_EQ(object.value<tyson::TySonType::Map>().size(), 2);

    ASSERT_EQ(object.value<tyson::TySonType::Map>().at(first_key), first_val);
    ASSERT_EQ(object.value<tyson::TySonType::Map>().at(sec_key), sec_val);

}

TEST(tyson_parsing, create_tyson_number)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::Number<int>(10);

    ASSERT_EQ(val.type(), tyson::TySonType::Number);
    ASSERT_EQ(val.value<int>(), 10);

    sstream << val;

    ASSERT_EQ(sstream.str(), "n|10|");
}

TEST(tyson_parsing, create_tyson_string)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::String("foobar");

    ASSERT_EQ(val.type(), tyson::TySonType::String);
    ASSERT_EQ(val.value<tyson::TySonType::String>(), "foobar");

    sstream << val;

    ASSERT_EQ(sstream.str(), "s|foobar|");
}

TEST(tyson_parsing, create_tyson_boolean_true)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::Bool(true);

    ASSERT_EQ(val.type(), tyson::TySonType::Bool);
    ASSERT_TRUE(val.value<tyson::TySonType::Bool>());

    sstream << val;

    ASSERT_EQ(sstream.str(), "b|true|");
}

TEST(tyson_parsing, create_tyson_boolean_false)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::Bool(false);

    ASSERT_EQ(val.type(), tyson::TySonType::Bool);
    ASSERT_FALSE(val.value<tyson::TySonType::Bool>());

    sstream << val;

    ASSERT_EQ(sstream.str(), "b|false|");
}

TEST(tyson_parsing, create_tyson_null)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::Null();

    ASSERT_EQ(val.type(), tyson::TySonType::Null);
    ASSERT_TRUE(val.value<tyson::TySonType::Null>().empty());

    sstream << val;

    ASSERT_EQ(sstream.str(), "null");
}

TEST(tyson_parsing, create_tyson_utc)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::Timestamp(1676663261);

    ASSERT_EQ(val.type(), tyson::TySonType::Timestamp);
    ASSERT_EQ(val.value<tyson::TySonType::Timestamp>(), "1676663261");

    sstream << val;

    ASSERT_EQ(sstream.str(), "utc|1676663261|");
}

TEST(tyson_parsing, create_tyson_link)
{
    std::stringstream sstream;
    auto val = tyson::TySonObject::Link("users", "e0bbcda2-0911-495e-9f0f-ce00db489f10");

    ASSERT_EQ(val.type(), tyson::TySonType::Link);
    ASSERT_EQ(std::get<0>(val.value<tyson::TySonType::Link>()), "users");
    ASSERT_EQ(std::get<1>(val.value<tyson::TySonType::Link>()), "e0bbcda2-0911-495e-9f0f-ce00db489f10");

    sstream << val;

    ASSERT_EQ(sstream.str(), "users|e0bbcda2-0911-495e-9f0f-ce00db489f10|");
}

TEST(tyson_parsing, create_tyson_vector)
{
    std::stringstream sstream;

    auto new_num = tyson::TySonObject::Number(10);
    auto new_str = tyson::TySonObject::String("fizzbuzz");
    std::vector<tyson::TySonObject> new_values {new_num, new_str};

    auto val = tyson::TySonObject::Vector(new_values);

    // new_values will be moved into the TySon Vector object
    ASSERT_EQ(new_values.size(), 0);
    ASSERT_EQ(val.type(), tyson::TySonType::Vector);

    std::vector<tyson::TySonObject> expected {new_num, new_str};
    ASSERT_EQ(val.value<tyson::TySonType::Vector>(), expected);

    sstream << val;

    ASSERT_EQ(sstream.str(), "v[n|10|,s|fizzbuzz|,]");
}

TEST(tyson_parsing, create_tyson_map)
{
    std::stringstream sstream;

    auto val_1 = tyson::TySonObject::Number(10);
    auto val_2 = tyson::TySonObject::Bool(false);

    std::map<std::string, tyson::TySonObject> new_values {std::make_pair("num", val_1),
                                                          std::make_pair("active", val_2)};

    auto val = tyson::TySonObject::Map(new_values);
    ASSERT_EQ(val.type(), tyson::TySonType::Map);

    sstream << val;

    ASSERT_EQ(sstream.str(), "m{s|active|:b|false|,s|num|:n|10|,}");
}

TEST(tyson_parsing, create_tyson_value)
{
    std::stringstream sstream;

    auto val = tyson::TySonObject::Value("num", tyson::TySonObject::Number(10));
    ASSERT_EQ(val.type(), tyson::TySonType::Value);

    sstream << val;

    ASSERT_EQ(sstream.str(), "value|num|:n|10|");
}

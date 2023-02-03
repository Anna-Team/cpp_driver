//
// Created by felix on 01.02.23.
//
#include "gtest/gtest.h"
#include "../TySON.hpp"

std::string only_map_data = "s|data|:objects{"
                     "test|d08412fb-9d7e-4d8f-8905-c7355a67ff70|:m{"
                     "    s|num|:n|0|,"
                     "    s|name|:s|test_0|,"
                     "},"
                     "test|d261580c-1c7f-4cf0-a231-be4a25486146|:m{"
                     "    s|num|:n|1|,"
                     "    s|name|:s|test_1|,"
                     "},"
                     "test|2b908538-2ec5-4970-8a65-30f7e3f6302c|:m{"
                     "    s|name|:s|test_2|,"
                     "    s|num|:n|2|,"
                     "},"
                     "test|510eb502-a645-44f4-9ea1-537682fd4049|:m{"
                     "    s|name|:s|test_3|,"
                     "    s|num|:n|3|,"
                     "},"
                     "test|12b519bd-c7b8-4124-9ba2-8647ef851900|:m{"
                     "    s|num|:n|4|,"
                     "    s|name|:s|test_4|,"
                     "},"
                     "}";

std::string id_response = "s|data|:ids["
                          "test|4339ace2-9ab3-4c79-b557-f9b78d66b7f9|,"
                          "test|3677c916-ac4d-40ab-89f4-def1e565e7ab|,"
                          "test|5ff00377-34ac-43b9-8ebb-71bb5ff78ebf|,"
                          "test|7bdd7c8f-e9da-42f6-b473-5a6fd9a1c90f|,"
                          "]";

TEST(tyson_parsing, create_tyson_object)
{
    ASSERT_NO_THROW(tyson::TySonData tysonObject {only_map_data});
    ASSERT_NO_THROW(tyson::TySonData tysonObject {id_response});
}

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
    EXPECT_EQ(object.value<tyson::TySonType::Map>().at(first_key), first_val);

}

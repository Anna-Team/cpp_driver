//
// Created by felix on 05.02.23.
//
#include "gtest/gtest.h"
#include "../TySON.hpp"
#include "../connection.hpp"


std::string only_map_data = "s|data|:objects{"
                            "test|d08412fb-9d7e-4d8f-8905-c7355a67ff70|:m{"
                            "s|num|:n|0|,"
                            "s|name|:s|test_0|,"
                            "},"
                            "test|d261580c-1c7f-4cf0-a231-be4a25486146|:m{"
                            "s|num|:n|1|,"
                            "s|name|:s|test_1|,"
                            "},"
                            "test|2b908538-2ec5-4970-8a65-30f7e3f6302c|:m{"
                            "s|name|:s|test_2|,"
                            "s|num|:n|2|,"
                            "},"
                            "test|510eb502-a645-44f4-9ea1-537682fd4049|:m{"
                            "s|name|:s|test_3|,"
                            "s|num|:n|3|,"
                            "},"
                            "test|12b519bd-c7b8-4124-9ba2-8647ef851900|:m{"
                            "s|num|:n|4|,"
                            "s|name|:s|test_4|,"
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
    ASSERT_NO_THROW(annadb::Data data {only_map_data});
    ASSERT_NO_THROW(annadb::Data data {id_response});
}

TEST(tyson_parsing, get_object_by_id)
{
    annadb::Data data {only_map_data};
    std::string uuid = "2b908538-2ec5-4970-8a65-30f7e3f6302c";
    tyson::TySonObject expected_result {"m{s|name|:s|test_2|,s|num|:n|2|,}"};

    auto obj = data.get<tyson::TySonType::Objects>();
    ASSERT_TRUE(obj.has_value());

    auto tyson_collection = obj.value();
    auto obj_link = tyson_collection.get<tyson::TySonType::Object>(uuid);

    ASSERT_TRUE(obj_link.has_value());
    ASSERT_EQ(obj_link.value().first.value<tyson::TySonType::Link>().second, uuid);
    ASSERT_EQ(obj_link.value().second.value<tyson::TySonType::Map>(), expected_result.value<tyson::TySonType::Map>());
}

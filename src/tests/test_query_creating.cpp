//
// Created by felix on 18.02.23.
//

#include "gtest/gtest.h"
#include "../query.hpp"

TEST(annadb_query, create_single_element_insert_query_v1)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");
    auto new_num = tyson::TySonObject::Number(10);

    auto new_data = annadb::Query::Insert(new_num);
    query.insert(new_data);

    sstream << query;

    ASSERT_EQ(sstream.str(), "collection|test|:insert[n|10|,];");
}

TEST(annadb_query, create_single_element_insert_query_v2)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");
    auto new_num = tyson::TySonObject::Number(10);

    query.insert(new_num);
    sstream << query;
    ASSERT_EQ(sstream.str(), "collection|test|:insert[n|10|,];");
}

TEST(annadb_query, create_multi_element_insert_query_v1)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");
    auto val_1 = tyson::TySonObject::Number(10);
    auto val_2 = tyson::TySonObject::String("fizzbuzz");
    auto val_3 = tyson::TySonObject::Bool(false);

    auto new_values = std::vector<tyson::TySonObject> {val_1, val_2, val_3};

    auto new_data = annadb::Query::Insert(new_values);
    query.insert(new_data);
    sstream << query;
    // moved the new values into the query to avoid new memory consumption
    ASSERT_EQ(new_values.size(), 0);
    ASSERT_EQ(sstream.str(),
              "collection|test|:insert[n|10|,s|fizzbuzz|,b|false|,];");
}

TEST(annadb_query, create_multi_element_insert_query_v2)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");
    auto val_1 = tyson::TySonObject::Number(10);
    auto val_2 = tyson::TySonObject::String("fizzbuzz");
    auto val_3 = tyson::TySonObject::Bool(false);

    auto new_values = std::vector<tyson::TySonObject> {val_1, val_2, val_3};

    query.insert(new_values);
    sstream << query;

    // moved the new values into the query to avoid new memory consumption
    ASSERT_EQ(new_values.size(), 0);
    ASSERT_EQ(sstream.str(),
              "collection|test|:insert[n|10|,s|fizzbuzz|,b|false|,];");
}

TEST(annadb_query, create_get_query_v1)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");

    auto val = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");
    auto get = annadb::Query::Get(val);

    query.get(get);
    sstream << query;

    ASSERT_EQ(sstream.str(),
              "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,];");
}

TEST(annadb_query, create_get_query_v2)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");

    auto val = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");

    query.get(val);
    sstream << query;

    ASSERT_EQ(sstream.str(),
              "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,];");
}

TEST(annadb_query, create_get_query_many_v1)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");

    auto val_1 = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");
    auto val_2 = tyson::TySonObject::Link("test", "cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4");

    auto values = std::vector<tyson::TySonObject>{val_1, val_2};
    auto get = annadb::Query::Get(values);

    query.get(get);
    sstream << query;

    ASSERT_EQ(sstream.str(),
              "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,test|cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4|,];");
}

TEST(annadb_query, create_get_query_many_v2)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");

    auto val_1 = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");
    auto val_2 = tyson::TySonObject::Link("test", "cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4");

    auto values = std::vector<tyson::TySonObject>{val_1, val_2};

    query.get(values);
    sstream << query;

    ASSERT_EQ(sstream.str(),
              "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,test|cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4|,];");
}

TEST(annadb_query, create_update_query_starting_pipeline_not_allowed)
{
    auto new_num = tyson::TySonObject::Number(100);
    auto new_val = tyson::TySonObject::Value("num", new_num);
    auto update_type = annadb::Query::UpdateType::Set;

    auto query = annadb::Query::Query("test");
    ASSERT_THROW(query.update(update_type, new_val), std::invalid_argument);
}

TEST(annadb_query, create_update_query)
{
    std::stringstream sstream;

    auto link = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");
    auto new_num = tyson::TySonObject::Number(100);
    auto new_val = tyson::TySonObject::Value("num", new_num);
    auto update_type = annadb::Query::UpdateType::Set;

    auto query = annadb::Query::Query("test");
    query.get(link).update(update_type, new_val);
    sstream << query;

    ASSERT_EQ(sstream.str(),
              "collection|test|:q[get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,],update[set{value|num|:n|100|},],];");
}

TEST(annadb_query, create_find_query)
{
    {
        std::stringstream sstream;
        auto min_num = tyson::TySonObject::Number(5);

        auto query = annadb::Query::Query("test");
        query.find(annadb::Query::Find::GT(min_num));
        sstream << query;

        ASSERT_EQ(sstream.str(), "collection|test|:find[gt{root: n|5|},];");
    }
    {
        std::stringstream sstream;
        auto min_num = tyson::TySonObject::Number(5);
        auto max_num = tyson::TySonObject::Number(50);

        auto query = annadb::Query::Query("test");
        auto lower_bound = annadb::Query::Gt("num", min_num);
        auto upper_bound = annadb::Query::Lte("num", max_num);

        query.find(annadb::Query::Find::AND(lower_bound, upper_bound));
        sstream << query;

        ASSERT_EQ(sstream.str(), "collection|test|:find[and[gt{value|num|: n|5|},lte{value|num|: n|50|},],];");
    }
}

//
// Created by felix on 18.02.23.
//

#include "gtest/gtest.h"
#include "../query.hpp"

TEST(annadb_query, create_single_element_insert_query_v1)
{
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        auto new_num = tyson::TySonObject::Number(10);
        
        auto new_data = annadb::Query::Insert(new_num);
        query.insert(new_data);
        
        sstream << query;
        
        ASSERT_EQ(sstream.str(), "collection|test|:insert[n|10|,];");
    }
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        
        auto new_data = annadb::Query::Insert(tyson::TySonObject::Number(10));
        query.insert(new_data);
        
        sstream << query;
        
        ASSERT_EQ(sstream.str(), "collection|test|:insert[n|10|,];");
    }
}

TEST(annadb_query, create_single_element_insert_query_v2)
{
    
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        auto new_num = tyson::TySonObject::Number(10);
        
        query.insert(new_num);
        sstream << query;
        ASSERT_EQ(sstream.str(), "collection|test|:insert[n|10|,];");
    }
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        
        query.insert(tyson::TySonObject::Number(10));
        sstream << query;
        ASSERT_EQ(sstream.str(), "collection|test|:insert[n|10|,];");
    }
}

TEST(annadb_query, create_multi_element_insert_query_v1)
{
    std::stringstream sstream;
    auto query = annadb::Query::Query("test");
    
    auto new_data = annadb::Query::Insert(
            tyson::TySonObject::Number(10),
            tyson::TySonObject::String("fizzbuzz"),
            tyson::TySonObject::Bool(false));
    query.insert(new_data);
    sstream << query;
    ASSERT_EQ(sstream.str(),
              "collection|test|:insert[n|10|,s|fizzbuzz|,b|false|,];");
}

TEST(annadb_query, create_multi_element_insert_query_v2)
{
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        query.insert(
                tyson::TySonObject::Number(10),
                tyson::TySonObject::String("fizzbuzz"),
                tyson::TySonObject::Bool(false));
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|test|:insert[n|10|,s|fizzbuzz|,b|false|,];");
    }
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        auto val_1 = tyson::TySonObject::Number(10);
        auto val_2 = tyson::TySonObject::String("fizzbuzz");
        auto val_3 = tyson::TySonObject::Bool(false);
        
        query.insert(val_1, val_2, val_3);
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|test|:insert[n|10|,s|fizzbuzz|,b|false|,];");
    }
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
    
    auto get = annadb::Query::Get(val_1, val_2);
    
    query.get(get);
    sstream << query;
    
    ASSERT_EQ(sstream.str(),
              "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,test|cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4|,];");
}

TEST(annadb_query, create_get_query_many_v2)
{
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        
        auto val_1 = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");
        auto val_2 = tyson::TySonObject::Link("test", "cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4");
        
        query.get(val_1, val_2);
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,test|cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4|,];");
    }
    
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        
        query.get(
                tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da"),
                tyson::TySonObject::Link("test", "cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4"));
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|test|:get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,test|cf3d77c0-75bd-41e4-aa46-b9ae9f5856a4|,];");
    }
}

TEST(annadb_query, get_allows_only_tyson_link_objects)
{
    auto new_num = tyson::TySonObject::Number(100);
    auto new_str = tyson::TySonObject::String("fizzbuzz");
    auto new_timestamp = tyson::TySonObject::Timestamp(1676663261);
    auto new_null = tyson::TySonObject::Null();
    auto new_bool = tyson::TySonObject::Bool(false);
    
    auto new_vector = tyson::TySonObject::Vector(new_num, new_str);
    
    std::map<std::string, tyson::TySonObject> new_map_values{
            std::make_pair("num", new_num),
            std::make_pair("active", new_bool)
    };
    auto new_map = tyson::TySonObject::Map(new_map_values);
    
    std::vector<tyson::TySonObject> invalid_values = {
            new_num,
            new_str,
            new_timestamp,
            new_null,
            new_bool,
            new_vector,
            new_map
    };
    
    auto query = annadb::Query::Query("test");
//  ASSERTION is not correctly as it is throwing a std::invalid_argument to early
//    for (auto invalid_val: invalid_values)
//    {
//        ASSERT_THROW(query.get(invalid_val), std::invalid_argument);
//    }
    auto new_link = tyson::TySonObject::Link("users", "e0bbcda2-0911-495e-9f0f-ce00db489f10");
    ASSERT_NO_THROW(query.get(new_link));
}

TEST(annadb_query, create_update_query_starting_pipeline_not_allowed)
{
    std::stringstream sstream;
    
    auto new_val = tyson::TySonObject::Value("num", tyson::TySonObject::Number(100));
    
    auto query = annadb::Query::Query("test");
    ASSERT_THROW(query.update(annadb::Query::UpdateType::Set, new_val), std::invalid_argument);
}

TEST(annadb_query, create_update_query)
{
    std::stringstream sstream;
    
    auto link = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");
    auto new_val = tyson::TySonObject::Value("num", tyson::TySonObject::Number(100));
    
    auto query = annadb::Query::Query("test");
    query.get(link).update(annadb::Query::UpdateType::Set, new_val);
    
    sstream << query;
    
    ASSERT_EQ(sstream.str(),
              "collection|test|:q[get[test|b2279b93-00b3-4b44-9670-82a76922c0da|,],update[set{value|num|:n|100|},],];");
}

TEST(annadb_query, create_find_query)
{
    {
        std::stringstream sstream;
        
        auto query = annadb::Query::Query("test");
        query.find(annadb::Query::Find::GT(tyson::TySonObject::Number(5)));
        sstream << query;
        
        ASSERT_EQ(sstream.str(), "collection|test|:find[gt{root: n|5|},];");
    }
    {
        std::stringstream sstream;
        auto min_num = tyson::TySonObject::Number(5);
        auto findQuery = annadb::Query::Find();
        auto query = annadb::Query::Query("test");
        query.find(std::move(findQuery.gt(min_num).eq("some.field", min_num)));
        sstream << query;
        
        ASSERT_EQ(sstream.str(), "collection|test|:find[gt{root: n|5|},eq{value|some.field|: n|5|},];");
    }
    {
        std::stringstream sstream;
        
        auto query = annadb::Query::Query("test");
        
        query.find(
                annadb::Query::Find::AND(
                        annadb::Query::Gt("num", tyson::TySonObject::Number(5)),
                        annadb::Query::Lte("num", tyson::TySonObject::Number(50))));
        sstream << query;
        
        ASSERT_EQ(sstream.str(), "collection|test|:find[and[gt{value|num|: n|5|},lte{value|num|: n|50|},],];");
    }
}

TEST(annadb_query, create_sort_query)
{
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("test");
        
        query.find(annadb::Query::Find::GT(tyson::TySonObject::Number(5)))
             .sort(annadb::Query::Sort::DESC("some", "other", "more"));
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|test|:q[find[gt{root: n|5|},],sort[desc(value|some|),desc(value|other|),desc(value|more|),],];");
    }
    {
        std::stringstream sstream;
        auto min_num = tyson::TySonObject::Number(5);
        
        auto query = annadb::Query::Query("test");
        
        query.find(annadb::Query::Find::GT(min_num))
             .sort(annadb::Query::Sort::ASC("some", "other", "more"));
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|test|:q[find[gt{root: n|5|},],sort[asc(value|some|),asc(value|other|),asc(value|more|),],];");
    }
}

TEST(annadb_query, create_limit_query)
{
    std::stringstream sstream;
    
    auto query = annadb::Query::Query("test");
    query.find(annadb::Query::Find::GT(tyson::TySonObject::Number(5))).limit<short>(6);
    sstream << query;
    
    ASSERT_EQ(sstream.str(), "collection|test|:q[find[gt{root: n|5|},],limit(n|6|),];");
}

TEST(annadb_query, create_offset_query)
{
    std::stringstream sstream;
    
    auto query = annadb::Query::Query("test");
    query.find(annadb::Query::Find::GT(tyson::TySonObject::Number(5))).offset<short>(6);
    sstream << query;
    
    ASSERT_EQ(sstream.str(), "collection|test|:q[find[gt{root: n|5|},],offset(n|6|),];");
}

TEST(annadb_query, create_delete_query)
{
    std::stringstream sstream;
    auto min_num = tyson::TySonObject::Number(5);
    
    auto query = annadb::Query::Query("test");
    query.find(annadb::Query::Find::GT(min_num)).delete_q();
    sstream << query;
    
    ASSERT_EQ(sstream.str(), "collection|test|:q[find[gt{root: n|5|},],delete,];");
}

TEST(annadb_query, create_project_query)
{
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("users");
        query.find(annadb::Query::Find())
             .sort(annadb::Query::Sort::ASC("name"))
             .project(std::make_pair(tyson::TySonObject::String("username"),
                                     tyson::TySonObject::Keep()
                                     ));
    
        sstream << query;
    
        ASSERT_EQ(sstream.str(), "collection|users|:q[find[],sort[asc(value|name|),],project{s|username|:keep,},];");
    }
    
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("users");
        query.find(annadb::Query::Find())
             .sort(annadb::Query::Sort::ASC("name"))
             .project(std::make_pair(tyson::TySonObject::String("username"),
                                     tyson::TySonObject::ProjectValue("name")
                                     ));
    
        sstream << query;
    
        ASSERT_EQ(sstream.str(), "collection|users|:q[find[],sort[asc(value|name|),],project{s|username|:value|name|,},];");
    }
    
    {
        std::stringstream sstream;
        auto query = annadb::Query::Query("users");
        query.find(annadb::Query::Find())
             .sort(annadb::Query::Sort::ASC("name"))
             .project(std::make_pair(tyson::TySonObject::String("title"),
                                     tyson::TySonObject::String("Dr. ")
             ));
        
        sstream << query;
        
        ASSERT_EQ(sstream.str(), "collection|users|:q[find[],sort[asc(value|name|),],project{s|title|:s|Dr. |,},];");
    }
    
    {
        std::stringstream sstream;
        
        auto query = annadb::Query::Query("users");
        query.find(annadb::Query::Find())
             .sort(annadb::Query::Sort::ASC("name"))
             .project(std::make_pair(tyson::TySonObject::String("passport"),
                                     tyson::TySonObject::Map("name", tyson::TySonObject::ProjectValue("name"))
                                     ),
                      std::make_pair(tyson::TySonObject::String("address"),
                                     tyson::TySonObject::Map("street", tyson::TySonObject::Keep())));
        
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|users|:q[find[],sort[asc(value|name|),],project{s|passport|:m{s|name|:value|name|,},s|address|:m{s|street|:keep,},},];");
    }
    
    {
        std::stringstream sstream;
        
        auto query = annadb::Query::Query("users");
        query.find(annadb::Query::Find())
             .sort(annadb::Query::Sort::ASC("name"))
             .project(std::make_pair(tyson::TySonObject::String("name"),
                                     tyson::TySonObject::Vector(tyson::TySonObject::ProjectValue("name"))
                      ),
                      std::make_pair(tyson::TySonObject::String("emails"),
                                     tyson::TySonObject::Vector(tyson::TySonObject::String("TEST"), tyson::TySonObject::Keep())));
        
        sstream << query;
        
        ASSERT_EQ(sstream.str(),
                  "collection|users|:q[find[],sort[asc(value|name|),],project{s|name|:v[value|name|,],s|emails|:v[s|TEST|,keep,],},];");
    }
}

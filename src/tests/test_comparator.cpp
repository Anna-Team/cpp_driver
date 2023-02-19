//
// Created by felix on 19.02.23.
//

#include "gtest/gtest.h"
#include "../query_comparision.hpp"

TEST(annadb_query_find, find_eq_root)
{
    std::stringstream sstream;

    auto val = tyson::TySonObject::String("foo");
    auto eq_root = annadb::Query::Eq(val);

    sstream << eq_root;
    ASSERT_EQ(sstream.str(), "eq{root: s|foo|}");
}

TEST(annadb_query_find, find_eq_field)
{
    {
        std::stringstream sstream;

        auto val = tyson::TySonObject::String("foo");
        auto eq_root = annadb::Query::Eq("field.path", val);

        sstream << eq_root;
        ASSERT_EQ(sstream.str(), "eq{value|field.path|: s|foo|}");
    }
    {
        std::stringstream sstream;

        auto val = tyson::TySonObject::String("foo");
        auto eq_root = annadb::Query::Eq("path.to.nested.field", val);

        sstream << eq_root;
        ASSERT_EQ(sstream.str(), "eq{value|path.to.nested.field|: s|foo|}");
    }
}

TEST(annadb_query_find, find_and_field)
{
    {
        std::stringstream sstream;

        auto val = tyson::TySonObject::String("foo");
        auto eq_root = annadb::Query::Eq("field.path", val);
        auto and_val = annadb::Query::And(eq_root);

        sstream << and_val;
        ASSERT_EQ(sstream.str(), "and[eq{value|field.path|: s|foo|},]");
    }
    {
        std::stringstream sstream;

        auto val_1 = tyson::TySonObject::String("foo");
        auto val_2 = tyson::TySonObject::String("bar");
        auto eq_root = annadb::Query::Eq("field.path", val_1);
        auto neq_field = annadb::Query::Neq("field", val_2);
        auto ands = std::vector<annadb::Query::Comparison> {eq_root, neq_field};

        auto and_val = annadb::Query::And(ands);

        sstream << and_val;
        ASSERT_EQ(sstream.str(), "and[eq{value|field.path|: s|foo|},neq{value|field|: s|bar|},]");
    }
}

TEST(annadb_query_find, find_or_field)
{
    {
        std::stringstream sstream;

        auto val = tyson::TySonObject::String("foo");
        auto eq_root = annadb::Query::Eq("field.path", val);
        auto and_val = annadb::Query::Or(eq_root);

        sstream << and_val;
        ASSERT_EQ(sstream.str(), "or[eq{value|field.path|: s|foo|},]");
    }
    {
        std::stringstream sstream;

        auto val_1 = tyson::TySonObject::String("foo");
        auto val_2 = tyson::TySonObject::String("bar");
        auto eq_root = annadb::Query::Eq("field.path", val_1);
        auto neq_field = annadb::Query::Neq(val_2);
        auto ors = std::vector<annadb::Query::Comparison> {eq_root, neq_field};

        auto and_val = annadb::Query::Or(ors);

        sstream << and_val;
        ASSERT_EQ(sstream.str(), "or[eq{value|field.path|: s|foo|},neq{root: s|bar|},]");
    }
}

TEST(annadb_query_find, find_not_field)
{
    {
        std::stringstream sstream;

        auto eq_root = annadb::Query::Not("field.path");

        sstream << eq_root;
        ASSERT_EQ(sstream.str(), "not(value|field.path|)");
    }
    {
        std::stringstream sstream;

        auto eq_root = annadb::Query::Not("name");

        sstream << eq_root;
        ASSERT_EQ(sstream.str(), "not(value|name|)");
    }
}

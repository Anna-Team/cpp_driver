//
// Created by felix on 13.03.23.
//

#ifndef ANNADB_DRIVER_EXAMPLE_FIND_EXAMPLE_HPP
#define ANNADB_DRIVER_EXAMPLE_FIND_EXAMPLE_HPP

#include "../../src/connection.hpp"

using TYSON = tyson::TySonObject;
using tyson::TySonCollectionObject;
using tyson::TySonType;
using annadb::Query::Query;

void find_number(annadb::AnnaDB &connection, const std::string &collection_name, int search_number);

void find_number_limit(
        annadb::AnnaDB &connection,
        const std::string &collection_name,
        int search_number,
        int limit
);

void find_number_limit_offset(
        annadb::AnnaDB &connection,
        const std::string &collection_name,
        int search_number,
        int limit,
        int offset
);

#endif //ANNADB_DRIVER_EXAMPLE_FIND_EXAMPLE_HPP

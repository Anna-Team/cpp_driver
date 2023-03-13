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

void find_number(annadb::AnnaDB &connection, const std::string &collection_name, const int search_number);
#endif //ANNADB_DRIVER_EXAMPLE_FIND_EXAMPLE_HPP

//
// Created by felix on 11.03.23.
//

#ifndef ANNADB_DRIVER_EXAMPLE_INSERT_EXAMPLE_HPP
#define ANNADB_DRIVER_EXAMPLE_INSERT_EXAMPLE_HPP

#include "../../src/connection.hpp"
using TYSON = tyson::TySonObject;
using tyson::TySonCollectionObject;
using tyson::TySonType;
using annadb::Query::Query;

void insert_some_values(annadb::AnnaDB &connection, const std::string &collection_name);

#endif //ANNADB_DRIVER_EXAMPLE_INSERT_EXAMPLE_HPP

#include <iostream>
#include "../src/connection.hpp"
using TYSON = tyson::TySonObject;

void insert_some_values(annadb::AnnaDB &connection, const std::string &collection_name);


int main()
{
    annadb::AnnaDB con {"jondoe", "passwd1234", "0.0.0.0", 10001};
    insert_some_values(con, "my_collection");
    con.close();
    
    return 0;
}

void insert_some_values(annadb::AnnaDB &connection, const std::string &collection_name)
{
    // this is similar to the example written here https://annadb.dev/documentation/insert/
    connection.connect();
    auto query = annadb::Query::Query(collection_name);
    
    auto tyson_map = TYSON::Map();
    tyson_map.emplace("bar", TYSON::String("baz"));
    
    query.insert(
            TYSON::String("foo"),
            TYSON::Number(100),
            TYSON::Bool(true),
            TYSON::Vector(TYSON::Number(1),
                          TYSON::Number(2),
                          TYSON::Number(3)
                          ),
            tyson_map
            );
    
    auto result = connection.send(query);
    if (result)
    {
        std::cout << "Insert query was successful\n";
        
        auto journal = result.value();
        auto new_rows = journal.meta().rows<short>();
        
        std::cout << "Inserted: " << new_rows.value() << " new rows.\n";
        
        auto new_ids = journal.data().get<tyson::TySonType::IDs>();
        for (const auto &val: new_ids.value().get<tyson::TySonType::Objects>(collection_name))
        {
            std::cout << val.first << "\n";
            std::cout << val.second << "\n";
        };
    }
    else
    {
        std::cout << "Something went wrong during the insertion\n";
    }
}

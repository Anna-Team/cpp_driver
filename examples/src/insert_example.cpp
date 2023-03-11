#include "../includes/insert_example.hpp"


void insert_some_values(annadb::AnnaDB &connection, const std::string &collection_name)
{
    auto query = Query(collection_name);
    
    auto tyson_map = TYSON::Map();
    tyson_map.emplace("bar", TYSON::String("baz"));
    
    // this is similar to the example written here https://annadb.dev/documentation/insert/
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
    
    std::optional<annadb::Journal> result = connection.send(query);
    if (result)
    {
        std::cout << "Insert query was successful\n";
        
        auto journal = result.value();
        // I expect only a few new objects so that I would use `short` over `int`
        auto new_rows = journal.meta().rows<short>();
        
        std::cout << "Inserted: " << new_rows.value() << " new rows.\n";
        
        TySonCollectionObject new_ids = journal.data().get<TySonType::IDs>().value();
        for (auto &val: new_ids.get<TySonType::IDs>(collection_name))
        {
            std::cout << val << "\n";
        };
    }
    else
    {
        std::cout << "Something went wrong during the insertion\n";
    }
}

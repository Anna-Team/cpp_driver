#include "../includes/find_example.hpp"

void find_number(annadb::AnnaDB &connection, const std::string &collection_name, int search_number)
{
    auto query = Query(collection_name);
    
    query.find(annadb::Query::Find::GT(TYSON::Number(search_number)));
    
    std::optional<annadb::Journal> result = connection.send(query);
    if (result)
    {
        std::cout << "Find query was successful send\n";
        
        auto journal = result.value();
        // I expect only one object so that I would use `short` over `int`
        auto new_rows = journal.meta().rows<short>();
        
        std::cout << "Found: " << new_rows.value() << " rows.\n";
        
        TySonCollectionObject new_ids = journal.data().get<TySonType::Objects>().value();
        for (auto &val: new_ids.get<TySonType::Objects>(collection_name))
        {
            std::cout << val.first << ": " << val.second << "\n";
        };
    }
    else
    {
        std::cout << "Something went wrong during the find\n";
    }
}

void find_number_limit(
        annadb::AnnaDB &connection,
        const std::string &collection_name,
        int search_number,
        int limit
)
{
    auto query = Query(collection_name);
    
    query.find(annadb::Query::Find::GT(TYSON::Number(search_number))).limit<int>(std::move(limit));
    
    std::optional<annadb::Journal> result = connection.send(query);
    if (result)
    {
        std::cout << "Find query was successful send\n";
        
        auto journal = result.value();
        // I expect only one object so that I would use `short` over `int`
        auto new_rows = journal.meta().rows<short>();
        
        std::cout << "Found: " << new_rows.value() << " rows.\n";
        
        TySonCollectionObject new_ids = journal.data().get<TySonType::Objects>().value();
        for (auto &val: new_ids.get<TySonType::Objects>(collection_name))
        {
            std::cout << val.first << ": " << val.second << "\n";
        };
    }
    else
    {
        std::cout << "Something went wrong during the find\n";
    }
}

void find_number_limit_offset(
        annadb::AnnaDB &connection,
        const std::string &collection_name,
        int search_number,
        int limit,
        int offset
)
{
    auto query = Query(collection_name);
    
    query
        .find(annadb::Query::Find::GT(TYSON::Number(search_number)))
        .offset(std::move(offset))
        .limit<int>(std::move(limit));
    
    std::optional<annadb::Journal> result = connection.send(query);
    if (result)
    {
        std::cout << "Find query was successful send\n";
        
        auto journal = result.value();
        // I expect only one object so that I would use `short` over `int`
        auto new_rows = journal.meta().rows<short>();
        
        std::cout << "Found: " << new_rows.value() << " rows.\n";
        
        TySonCollectionObject new_ids = journal.data().get<TySonType::Objects>().value();
        for (auto &val: new_ids.get<TySonType::Objects>(collection_name))
        {
            std::cout << val.first << ": " << val.second << "\n";
        };
    }
    else
    {
        std::cout << "Something went wrong during the find\n";
    }
}
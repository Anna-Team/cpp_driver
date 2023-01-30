#include <iostream>
#include "connection.hpp"

int main()
{
    annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};
    con.connect();
//    auto answer = con.send("collection|test_journal|:"
//                           "insert[s|foo|,n|100|,b|true|,v[n|1|,n|2|,n|3|,],m{s|bar|:s|baz|,},];");
//
//    std::cout << "Received reply " << " [" << answer << "]" << std::endl;

    auto result = con.send("collection|test_journal|:find[];");
    con.close();
    if (result && result->ok())
    {
        std::cout << result->meta() << "\n";
        std::cout << "count is " << result->meta().count<int>() << "\n";
        std::cout << "type is " << result->meta().type() << "\n";
        std::cout << result->data() << std::endl;
    }
}

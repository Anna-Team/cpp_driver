#include "../src/connection.hpp"
#include "includes/insert_example.hpp"
#include "includes/find_example.hpp"

int main()
{
    {
        annadb::AnnaDB con {"jondoe", "passwd1234", "0.0.0.0", 10001};
        con.connect();
        // insert_some_values(con, "my_collection");
        con.close();
    }
    
    {
        annadb::AnnaDB con {"jondoe", "passwd1234", "0.0.0.0", 10001};
        con.connect();
        find_number(con, "my_collection", 99);
        con.close();
    }
    
    return 0;
}

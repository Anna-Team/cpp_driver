#include "../src/connection.hpp"
#include "includes/insert_example.hpp"

int main()
{
    annadb::AnnaDB con {"jondoe", "passwd1234", "0.0.0.0", 10001};
    con.connect();
    insert_some_values(con, "my_collection");
    con.close();
    
    return 0;
}

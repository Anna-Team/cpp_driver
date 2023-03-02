# Create a AnnaDB connection

### Create a new AnnaDB instance 
```c++
#include "connection.hpp"

annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};
```

### Establish a new connection
```c++
#include "connection.hpp"

annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};

con.connect();
```

### Send a raw query
```c++
auto answer = con.send("collection|test_journal|:"
                       "insert[s|foo|,n|100|,b|true|,v[n|1|,n|2|,n|3|,],m{s|bar|:s|baz|,},];");
```

### Send a query
- the Query class will be described in a different section in more detail
```c++

Query query {"test_journal"};
query.<Query Command>

auto answer = con.send(query);
```

### Close the connection
- do not forget to close the connection when your done
```c++
#include "connection.hpp"

...

con.close();
```


# AnnaDB CPP-Client

[![AnnaDB Logo](https://annadb.dev/assets/img/logo_colored.svg)](https://annadb.dev)![And](https://cdn.iconscout.com/icon/free/png-128/mobile-keyboard-key-program-ampersand-and-11559.png)![ISO_C++_Logo.svg.png](images%2FISO_C%2B%2B_Logo.svg.png)

This is a C++ driver built for type-safe query creation.

AnnaDB uses a custom query language dubbed [TySON](https://github.com/roman-right/tyson)


## Required
- [zeromq](https://zeromq.org/download/)
- cmake >= 3.24


## Test library

- googletest

## local development
- create a AnnaDB instance via `docker-compose up` access the DB via port `10001` on `localhost`

## How to use

### 1. Create a connection to AnnaDB
```c++
#include "connection.hpp"

annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};
con.connect();

// do something

con.close();
``` 
### 2. Make a query to AnnaDB
- [AnnaDB Query Documentation](https://annadb.dev/documentation/insert/)
- the result of a query request is a 
```c++
std::optional<annadb::Journal>
```
```c++
#include "connection.hpp"
        
// ...
auto answer = con.send("collection|test_journal|:insert[s|foo|,"
                       "n|100|,"
                       "b|true|,"
                       "v[n|1|,n|2|,n|3|,],"
                       "m{s|bar|:s|baz|,},"
                       "];");
// ...
```
### 3. Inspect the AnnaDB result
- the `annadb::Journal` class
```c++
// ...

// you need to check, cause Journal is an optional
if (answer) 
{
    // annadb::Journal contains two members to get information from the query
    // the data and the meta information
    const auto data_information = answer.value().data();
    const auto meta_information = answer.value().meta();
}
// ...
```


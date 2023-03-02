# Create a AnnaDB query

## The insert query
- Can only be used as a single statement
```c++
#include "query.hpp"

// open a connection
annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};
con.connect();

// create a query object with the name of the collection you want to modify
auto query = annadb::Query::Query(<collection_name>);

// create a TySON Object(s) you want to insert
auto new_num = tyson::TySonObject::Number(10);

// pass the TySON Object(s) to the `insert` command 
query.insert(new_num);

// send the query
auto answer = con.send(query);
```
- with multiple values
```c++
#include "query.hpp"
...

auto query = annadb::Query::Query(<collection_name>);

// create the TySON Objects
auto val_1 = tyson::TySonObject::Number(10);
auto val_2 = tyson::TySonObject::String("fizzbuzz");
auto val_3 = tyson::TySonObject::Bool(false);

// pass the TySON Objects to the insert statement
query.insert(val_1, val_2, val_3);

...
```

## The get query
- can be combined with `find`, `get`, `sort`, `limit`, `offset`, `update`, `delete`
- can __only__ be used with TySON Link Objects
- can also be used with a `std::vector` or as `...T` to send multiple links in one query
```c++
#include "query.hpp"
...

// create a TySON Link object
auto val_1 = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");

// use get command combined with limit
query.get(val_1).limit<short>(5);

// send the query
auto answer = con.send(query);

```

## The find query
- can be combined with `find`, `get`, `sort`, `limit`, `offset`
- must be used with the `Find` class
- needs to be called with any of

| Static Version | Alternative | Meaning      |
|----------------|-------------|--------------|
| `EQ`           | `eq`        | equals       |
| `NEQ`          | `neq`       | not equals   | 
| `GT`           | `gt`        | greater      |
| `GTE`          | `gte`       | greater than | 
| `LT`           | `le`        | less         |
| `LTE`          | `lte`       | less than    |

- multiple find statements can be combined with `AND`, `OR`, `NOT`
- if you need to compare with a specific field inside of the collection pass the name of it to the constructor
  - `annadb::Query::Gt("num", min_num);` will compare against the `num` field inside of the collection you could also 
  write `num.inside.a.nested.other.collection` without you compare against the `root` node `annadb::Query::Gt(min_num);`
  
Example __Static__ Version from `root`
```c++
#include "query.hpp"
...

// create a TySonObject object with which we want to compare
auto min_num = tyson::TySonObject::Number(5);

query.find(
        // this is the static version
        annadb::Query::Find::GT(min_num)
        // the other comparison statements can be used in the same way
        // annadb::Query::Find::EQ(min_num)
        // annadb::Query::Find::NEQ(min_num)
        // annadb::Query::Find::GTE(min_num)
        // annadb::Query::Find::LT(min_num)
        // annadb::Query::Find::LTE(min_num)
);

...

```

Example __Static__ Version with `field`
```c++
#include "query.hpp"
...

// create a TySonObject object with which we want to compare
auto min_num = tyson::TySonObject::Number(5);

query.find(
        annadb::Query::Find::GT("my.field.num", min_num)
        );

...

```

Example with instance of `Find` class
- this makes it easier to concat multiple finds which will __not__ result in an And statement
```c++
#include "query.hpp"
...

// create a TySonObject object with which we want to compare
auto min_num = tyson::TySonObject::Number(5);

// searching from the root
auto findQuery = annadb::Query::Find();

// requires moving
query.find(std::move(
        // can be concatenated
        findQuery.gt(min_num).eq(min_num))
        // if you want to compare with a specific field you can add this information
        // findQuery.lt("my.num", min_num)
        );

...

```

Example with `AND`
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);
auto max_num = tyson::TySonObject::Number(50);

auto lower_bound = annadb::Query::Gt("num", min_num);
auto upper_bound = annadb::Query::Lte("num", max_num);
        
query.find(
        annadb::Query::Find::AND(lower_bound, upper_bound)
);

...

```

Example with `OR`
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);
auto max_num = tyson::TySonObject::Number(50);

auto lower_bound = annadb::Query::Gt("num", min_num);
auto upper_bound = annadb::Query::Lte("num", max_num);
        
query.find(
        annadb::Query::Find::OR(lower_bound, upper_bound)
);

...

```

Example with `NOT`
```c++
#include "query.hpp"
...

query.find(
        annadb::Query::Find::NOT("exclude.me")
);

...

```

## The sort query
- can __not__ be used alone
- must be used with the `Sort` class
- possible choices are `ASC`, `DESC` both can be used static
- one or more field names are required

single field
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);

query.find(annadb::Query::Find::GT(min_num))
     .sort(annadb::Query::Sort::DESC("some.field"));

...

```
multiple fields
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);

query.find(annadb::Query::Find::GT(min_num))
     .sort(annadb::Query::Sort::ASC("some", "fields", "to", "sort"));

...

```

## The limit query
- can __not__ be used alone
- needs an integral
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);

query.find(
        annadb::Query::Find::GT("my.field.num", min_num).limit<short>(6)
);

...

```

## The offset query
- can __not__ be used alone
- needs an integral
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);

query.find(
        annadb::Query::Find::GT("my.field.num", min_num).offset<int>(260)
);

...

```

## The update query
- can __not__ be used alone and must be the last if you combine multiple statements
- you can either use `annadb::Query::UpdateType::Set` to set a field to a specific value or 
`annadb::Query::UpdateType::Inc` to increase a field by a specific value
- can only be used with an instance of `tyson::TySonObject::Value`
```c++
#include "query.hpp"
...

// create a TySON Link object
auto link = tyson::TySonObject::Link("test", "b2279b93-00b3-4b44-9670-82a76922c0da");

// create a TySON value object
auto new_val = tyson::TySonObject::Value("num",  // the name of the field 
                                         tyson::TySonObject::Number(100) // the new value
                                         );

query.get(link).update(annadb::Query::UpdateType::Set, // how you want to update
                       new_val // the value with which you want to update
                       ); 

// use get command combined with limit
query.get(val_1).limit<short>(5);

...

```

## The delete query
- can __not__ be used alone and must be the last if you combine multiple statements
```c++
#include "query.hpp"
...

auto min_num = tyson::TySonObject::Number(5);

auto query = annadb::Query::Query("test");

// this query will delete all findings
query.find(annadb::Query::Find::GT(min_num)).delete_q();

...

```

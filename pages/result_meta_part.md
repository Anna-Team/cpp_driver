# Journal Meta
- Each result of a transaction will be an instance of the `Journal` class this class
has two properties `Meta` and `Data`.
- We will now look at the `Meta` property
```c++
#include "query.hpp"

annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};
con.connect();

auto val_1 = tyson::TySonObject::Number(10);
auto val_2 = tyson::TySonObject::String("fizzbuzz");
auto val_3 = tyson::TySonObject::Bool(false);

// pass the TySON Objects to the insert statement
query.insert(val_1, val_2, val_3);

// create a transaction
std::optional<Journal> answer = con.send(query);

if (answer) 
{
    // extract the Meta information
    Meta meta_information = answer.value().meta();
}

con.close();
```

## Extract the type of the transaction from Meta
- the `MetaType` can be any of these 
  - `MetaType::insert_meta`
  - `MetaType::get_meta`
  - `MetaType::find_meta`
  - `MetaType::update_meta`
- this can be used to validate that your transaction/query did what you expected
```c++
#include "query.hpp"
...

Meta meta_information = answer.value().meta();

if (meta_information.type() == MetaType::insert_meta)
{
    ...
}
...
```

## Extract the number of rows which where affected
- you can either use the underlying TySON Map or directly the rows count
```c++
#include "query.hpp"
...

Meta meta_information = answer.value().meta();

// you can extract the rows count wall
// ewith an integral
short affected_rows = meta_information.rows<short>();

// or you can access the underlying TySON Map
tyson::TySonObject affected_rows = meta.data();

...
```

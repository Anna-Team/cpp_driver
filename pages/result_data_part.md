# Journal Data
- Each result of a transaction will be an instance of the `Journal` class this class
  has two properties `Meta` and `Data`.
- We will now look at the `Data` property
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
    // extract the Data information
    Data data_information = answer.value().data();
}

con.close();
```

## Extract the value from the Data
- you will either receive `TySonType::Objects` or `TySonType::IDs` wrapped in a `TySonCollectionObject` depending on your query
- following Queries will return `TySonType::Objects`
  - Get
  - Find
  - Sort
  - Limit
  - Offset
- following Query will return `TySonType::IDs`
  - Insert
  - Update
  - Delete
- Example for a query result where you get `TySonType::IDs`
```json
result:ok[
    response{
        s|data|:ids[
            test|9e1691d3-fbd6-4444-8175-1957f2871c45|,
            test|0cc130be-c19e-47a9-892e-2905d7427dbc|,
        ],
    ...
```
- Example for a query result where you get `TySonType::Objects`
```json
result:ok[
    response{
        s|data|:objects{
            test|ea63e06f-9d1c-442f-89fd-c5041d863f5f|:s|foo|,
            test|35db29f3-eba6-4589-bf81-2542b2f5db37|:s|bar|,
        },
    ...
```
- Example for `TySonType::IDs`
```c++
#include "TySON.hpp"

Data data_information = answer.value().data();

std::optional<TySonCollectionObject> ids = data_information.get<tyson::TySonType::IDs>();
```
- Example for `TySonType::Objects`
```c++
#include "TySON.hpp"

Data data_information = answer.value().data();

std::optional<TySonCollectionObject> objs = data_information.get<tyson::TySonType::Objects>();
```

## How to use TySonType::Objects
- How to extract the data from such an Object
- Here for an example for getting a specific value for following possible response
```text
result:ok[
    response{
        s|data|:objects{
            test|ea63e06f-9d1c-442f-89fd-c5041d863f5f|:s|foo|,
            test|35db29f3-eba6-4589-bf81-2542b2f5db37|:s|bar|,
        },
        s|meta|:get_meta{
            s|count|:n|2|,
        },
    },
];
```
```c++
#include "TySON.hpp"

std::string uuid = "ea63e06f-9d1c-442f-89fd-c5041d863f5f";
Data data_information = answer.value().data();

std::optional<TySonCollectionObject> objs = data_information.get<tyson::TySonType::Objects>();
auto tyson_collection = objs.value();

// the first will be a TySonType::Link
// the second can be any of 
// TySonType::Number, TySonType::String, TySonType::Bool, TySonType::Null, TySonType::Timestamp, TySonType::Vector, TySonType::Map,
std::optional<std::pair<TySonObject, TySonObject>> obj_link = tyson_collection.get<tyson::TySonType::Object>(uuid);

// will give `foo`
auto string_result = obj_link.value().second.value<tyson::TySonType::String>()
```
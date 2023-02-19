#include <iostream>
#include "connection.hpp"
#include "TySON.hpp"
#include "utils.hpp"
#include "query.hpp"

int main()
{
    std::cout << sizeof(annadb::Meta) << "\n";
//    auto num_asc = annadb::Query::Asc(std::string("num"));
//    auto name_desc {annadb::Query::Desc(std::string("name"))};
//
//    auto sort_cmds = std::vector<std::unique_ptr<annadb::Query::SortCmd>> {};
//    // std::make_unique<annadb::Query::Asc>(num_asc), std::make_unique<annadb::Query::Desc>(name_desc)
//    sort_cmds.emplace_back(std::make_unique<annadb::Query::Desc>(name_desc));
//    sort_cmds.emplace_back(std::make_unique<annadb::Query::Asc>(num_asc));
//
//    auto var = annadb::Query::Sort(std::move(sort_cmds));
//    auto ftmp = annadb::Query::Find();
//    auto gtmp = annadb::Query::Get();
//    var.find(ftmp).get(gtmp);
//    std::cout << var.query() << "\n";

//    annadb::AnnaDB con {"jondoe", "passwd1234", "localhost", 10001};
//    con.connect();
//    auto answer = con.send("collection|test_journal|:"
//                           "insert[s|foo|,n|100|,b|true|,v[n|1|,n|2|,n|3|,],m{s|bar|:s|baz|,},];");
//
//    if (answer)
//    {
//        std::cout << "Received reply " << " [" << answer.value() << "]" << std::endl;
//    }
//
//    auto result = con.send("collection|test_journal|:find[];");
//    con.close();
//    if (result && result->ok())
//    {
//        result->data();
//        std::cout << result->meta() << "\n";
//        std::cout << result->meta().data()["count"].value<tyson::TySonType::Number>();
//        std::cout << "count is " << result->meta().count<int>() << "\n";
//        std::cout << "type is " << result->meta().type() << "\n";
//        std::cout << result->data() << std::endl;
//    }

//    con.close();

//    tyson::TySonObject object {"v[n|1|,n|2|,n|3|,]"};
//    auto val = object.value<tyson::TySonType::Vector>();
//    std::for_each(val.begin(), val.end(), [](const tyson::TySonObject &elem){
//        auto data_type = elem.type();
//
//        std::string value;
//        switch (data_type)
//        {
//            case tyson::TySonType::Number:
//                value = elem.value<tyson::TySonType::Number>();
//                break;
//            case tyson::TySonType::String:
//                value = elem.value<tyson::TySonType::String>();
//                break;
//            case tyson::TySonType::Bool:
//                value = elem.value<tyson::TySonType::Bool>();
//                break;
//            case tyson::TySonType::Null:
//                value = elem.value<tyson::TySonType::Null>();
//                break;
//            case tyson::TySonType::Timestamp:
//                value = elem.value<tyson::TySonType::Timestamp>();
//                break;
//            case tyson::TySonType::Link:
//                value = elem.value<tyson::TySonType::Link>();
//                break;
//            case tyson::TySonType::Vector:
//                break;
//            case tyson::TySonType::Map:
//                break;
//        }
//        std::cout << value << "\n";
//    });
//
//    tyson::TySonObject object {"m{s|name|:s|test_2|,s|num|:n|2|,}"};

//    std::string_view data = "test_journal|c36de05f-5e0b-4064-874a-4fec5d97a89f|:n|100|,"
//                            "test_journal|14e4f62e-224c-4427-9c3e-4062f6119235|:m{s|bar|:s|baz|, s|foo|:s|baz|},"
//                            "test_journal|9a7ebdae-4be9-4456-987d-c3c7c0ef6c12|:s|foo|,"
//                            "test_journal|17fd4443-17b4-42a1-ab04-2c96f6cd058f|:v[n|1|,n|2|,n|3|],"
//                            "test_journal|42331da5-c711-4c88-98ec-ef19556c4fce|:v[n|1|,n|2|,n|3|],"
//                            "test_journal|4a1f81bd-eabe-40ac-865d-0ef1bf02c2ab|:b|true|,"
//                            "test_journal|7bcf6cb0-ba84-403d-b936-eb5453ecfe89|:b|true|,"
//                            "test_journal|d621f053-9164-4411-a490-599c5766e87a|:s|foo|,"
//                            "test_journal|f8f59afc-c8f3-477d-9c03-9f9c9a519fd3|:n|100|,"
//                            "test_journal|eb19ff2d-fb5a-4425-9a1c-3afe516bef2b|:m{s|bar|:s|baz|, s|firstname|:s|jon|, m{s|name|:s|doe|}}";
//


//    auto words_begin = std::sregex_iterator(data.begin(), data.end(), pattern);
//    auto words_end = std::sregex_iterator();
//
//    std::cout << "Found "
//              << std::distance(words_begin, words_end)
//              << " words\n";
//
//    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//        std::smatch match = *i;
//        std::string match_str = match.str();
//        std::cout << "  " << match_str << '\n';
//    }

//    const std::regex pattern ("(,)\\b[\\w-]{2,}+\\b\\|");
//    auto new_data = std::regex_replace(data.data(), pattern, "^$&");
//
//    for (auto &val: utils::split(new_data, '^'))
//    {
//        std::cout << val << "\n";
//    }
    return 0;
}

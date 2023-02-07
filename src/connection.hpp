//
// Created by felix on 28.01.23.
//

#ifndef ANNADB_DRIVER_CONNECTION_HPP
#define ANNADB_DRIVER_CONNECTION_HPP

#include <map>
#include <regex>
#include <valarray>
#include <zmq.hpp>
#include "TySON.hpp"


namespace annadb
{
    const std::regex pattern ("(,)\\b[\\w-]{2,}+\\b\\|");
    struct KeyVal
    {
        KeyVal(std::string data)
        {
            auto separation = data.find_first_of(':');
            auto start = data[0] == ',' ? 1 : 0;
            auto end = data[data.size() - 1] == ',' ? 1 : 0;
            link = data.substr(start, separation - start);
            value = data.substr(separation + 1, data.size() - 1 - separation - end);
        }

        std::string link;
        std::string value;
    };

    enum class MetaType: unsigned char
    {
        none = 'n',
        insert_meta = 'i',
        get_meta = 'g',
        find_meta = 'f',
        update_meta = 'u'
    };

    std::ostream& operator<< (std::ostream& os, MetaType metaType)
    {
        std::string meta_str;

        switch (metaType)
        {

            case MetaType::insert_meta:
                meta_str = "insert_meta";
                break;
            case MetaType::get_meta:
                meta_str = "get_meta";
                break;
            case MetaType::find_meta:
                meta_str = "find_meta";
                break;
            case MetaType::update_meta:
                meta_str = "update_meta";
                break;
            case MetaType::none:
                meta_str = "none";
                break;
        }

        return os << meta_str;
    }

    const auto metaTypes = std::map<std::string, MetaType>{std::make_pair(":insert_meta", MetaType::insert_meta),
                                                           std::make_pair(":get_meta", MetaType::get_meta),
                                                           std::make_pair(":find_meta", MetaType::find_meta),
                                                           std::make_pair(":update_meta", MetaType::update_meta),};

    class Data
    {
        std::string data_;

        std::vector<KeyVal> split_data(std::string_view str_data)
        {
            auto new_data = std::regex_replace(str_data.data(), pattern, "^$&");
            std::ranges::split_view split_view{ new_data, '^' };
            std::vector<KeyVal> parts {};

            parts.reserve(std::distance(split_view.begin(), split_view.end()));

            for (auto split: split_view)
            {
                std::string tmp;
                for (const auto &item: split)
                {
                    if (item)
                    {
                        tmp += item;
                    }
                }
                parts.emplace_back(tmp);
                tmp.clear();
            }

            return parts;
        }

        std::vector<std::string_view> split_data_ids(std::string_view object)
        {
            std::ranges::split_view split_view{ object, ',' };
            std::vector<std::string_view> parts {};

            parts.reserve(std::distance(split_view.begin(), split_view.end()));

            for (auto split: split_view)
            {
                std::string tmp;
                for (const auto &item: split)
                {
                    tmp += item;
                }
                parts.emplace_back(tmp);
                tmp.clear();
            }

            return parts;
        }

    public:
        explicit Data(std::string_view data) : data_(data) {}

        template<tyson::TySonType T>
        requires (T == tyson::TySonType::Objects || T == tyson::TySonType::IDs)
        std::optional<tyson::TySonCollectionObject> get()
        {
            if (data_.starts_with(std::string("s|data|:objects")) && T == tyson::TySonType::Objects)
            {
                tyson::TySonCollectionObject object {};
                auto start_val = data_.find_first_of('{') + 1;
                auto end_val = data_.find_last_of('}');

                auto tyson_str_data = split_data(data_.substr(start_val, end_val - start_val));
                for (auto &key_val: tyson_str_data)
                {
                    object.add(std::make_pair(key_val.link, key_val.value));
                }

                tyson_str_data.clear();
                tyson_str_data.shrink_to_fit();

                return object;
            }
            else if (data_.starts_with(std::string("s|data|:ids"))  && T == tyson::TySonType::IDs)
            {
                tyson::TySonCollectionObject object {};
                auto start_val = data_.find_first_of('[');
                auto end_val = data_.find_last_of(']');

                auto tyson_str_data = split_data_ids(data_.substr(start_val, end_val - start_val));
                for (auto &link_data: tyson_str_data)
                {
                    object.add(link_data);
                }

                tyson_str_data.clear();
                tyson_str_data.shrink_to_fit();

                return object;
            }
            return {};
        }
    };

    class Meta
    {
        /*
         * Always a TySON map object
         * s|meta|:find_meta{s|count|:n|5|}
         */
        std::string meta_txt_;

        std::string count_;
        MetaType metaType = MetaType::none;
        tyson::TySonObject data_;

        void parse_data()
        {
            auto pos_map_start = meta_txt_.find('{');
            auto map_str = "m" + meta_txt_.substr(pos_map_start, meta_txt_.size() - pos_map_start);
            data_ = tyson::TySonObject {map_str};
        }

        void parse_type()
        {
            auto pos_type_start = meta_txt_.find(':');
            auto pos_type_end = meta_txt_.find('{');
            auto meta_type_str = meta_txt_.substr(pos_type_start, pos_type_end - pos_type_start);
            metaType = metaTypes.at(meta_type_str);
        }

        friend std::ostream & operator<<(std::ostream &os, const Meta& meta)
        {
            std::string count_val = "0";

            auto count = meta.data_["count"];
            if (count)
            {
                count_val = count.value().value<tyson::TySonType::String>();
            }

            std::string repr = "{s|count|:n|" + count_val + "|";
            return os << "s|meta|:" << meta.metaType << repr << "}";
        }

    public:

        explicit Meta(std::string_view meta_txt)
        {
            meta_txt_ = meta_txt;
            parse_data();
            parse_type();
        }

        tyson::TySonObject data()
        {
            return data_;
        }

        MetaType type()
        {
            return metaType;
        }
    };

    class Journal
    {
        bool result_ = false;
        std::string data_;
        std::string meta_;

        void parse_response(std::string_view response)
        {
            /*
             * The response format from annadb is
             * `result:<ok|false>[response{s|data|<...>, s|meta|<...>}]`
             */
            auto pos_result_end = response.find('[');
            auto pos_data_begin = response.find("s|data|:");

            // the meta information is always at the end,
            // so that it is faster to use rfind instead of regular find
            auto pos_meta_begin = response.rfind("s|meta|:");

            // there is no usage for the closing tags, so we exclude them
            auto pos_response_end = response.rfind(",}]");

            auto res = response.substr(0, pos_result_end);
            if (res.find("ok") != std::string::npos)
            {
                result_ = true;
            }

            data_ = response.substr(pos_data_begin, pos_meta_begin - pos_data_begin);
            meta_ = response.substr(pos_meta_begin, pos_response_end - pos_meta_begin);
        }

        friend std::ostream & operator<<(std::ostream &os, const Journal& journal)
        {
            std::string result = journal.ok() ? "ok" : "err";
            auto meta = journal.meta();
            auto data = "journal.data()"; // journal.data();

            std::string repr = "result:" + result + "[response{";
            return os << repr << data << meta << ",},];";
        }

    public:
        explicit Journal(std::string_view response)
        {
            parse_response(response);
        }

        [[nodiscard]] bool ok() const
        {
            return result_;
        }

        [[nodiscard]] Meta meta() const
        {
            Meta meta{meta_};
            return meta;
        }

        [[nodiscard]] Data data() const
        {
            Data data{data_};
            return data;
        }
    };

    class AnnaDB
    {
        std::string username_;
        std::string password_;
        std::string host_;
        std::string port_;

        zmq::context_t context {1};
        zmq::socket_t requester {context, ZMQ_REQ};

        inline bool zmq_send(std::string_view query)
        {
            zmq::message_t message(query.size());
            std::memcpy(message.data(), query.data(), query.size());

            auto response = requester.send(message, zmq::send_flags::none);

            if (response)
            {
                return true;
            }

            return false;
        }

        inline std::optional<std::string> zmq_receive()
        {
            zmq::message_t message;
            auto response = requester.recv(message, zmq::recv_flags::none);

            if (response)
            {
                auto result = std::string(static_cast<char *>(message.data()), message.size());
                return result;
            }

            return {};
        }

    public:
        AnnaDB(std::string_view username,
               std::string_view password,
               std::string_view host,
               u_short port
        ) : username_(username),
            password_(password),
            host_(host),
            port_(std::to_string(port))
        {
        };

        ~AnnaDB() = default;

        void connect()
        {
            requester.connect("tcp://" + host_ + ":" + port_);
        }

        void close()
        {
            requester.close();
        }

        [[nodiscard]] std::optional<Journal> send(std::string_view query)
        {
            auto result = zmq_send(query);

            if (result)
            {
                auto response = zmq_receive();
                if (response)
                {
                    return Journal(*response);
                }
            }
            return {};
        }
    };
}

#endif //ANNADB_DRIVER_CONNECTION_HPP

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
#include "query.hpp"


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

    inline std::ostream& operator<< (std::ostream& os, MetaType metaType) noexcept
    {
        switch (metaType)
        {
            case MetaType::insert_meta:
                return os << "insert_meta";
            case MetaType::get_meta:
                return os << "get_meta";
            case MetaType::find_meta:
                return os << "find_meta";
            case MetaType::update_meta:
                return os << "update_meta";
            case MetaType::none:
                return os << "none";
        }

        return os << "";
    }

    const auto metaTypes = std::map<std::string, MetaType>{std::make_pair(":insert_meta", MetaType::insert_meta),
                                                           std::make_pair(":get_meta", MetaType::get_meta),
                                                           std::make_pair(":find_meta", MetaType::find_meta),
                                                           std::make_pair(":update_meta", MetaType::update_meta),};

    class Data
    {
        std::string data_;

        /**
         * Split the data into sections to create later TysonObjects
         *
         * @param str_data the raw string data inside of the AnnaDB response
         * @return the parts of the data response
         */
        std::vector<KeyVal> split_data(std::string_view str_data) noexcept
        {
            auto new_data = std::regex_replace(str_data.data(), pattern, "^$&");
            auto data = utils::split(new_data, '^');

            std::vector<KeyVal> parts {};
            parts.reserve(data.size());
            std::transform(data.begin(), data.end(), std::back_inserter(parts), [](auto &val){return KeyVal(val);});

            return parts;
        }

    public:

        /**
         * create a new Data object from the raw string
         * @param data
         */
        explicit Data(std::string_view data) noexcept : data_(data) {}
        ~Data() = default;

        /**
         * the AnnaDB response can contains Objects or IDs which will be handled differently
         *
         * @tparam T the TySonType Objects or IDs
         * @return a TySonCollection which holds either the IDs or the Objects
         */
        template<tyson::TySonType T>
        requires (T == tyson::TySonType::Objects || T == tyson::TySonType::IDs)
        std::optional<tyson::TySonCollectionObject> get() noexcept
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
                auto start_val = data_.find_first_of('[');
                auto end_val = data_.find_last_of(']');

                auto tyson_str_data = utils::split(data_.substr(start_val + 1, end_val - start_val - 1), ',');
                tyson::TySonCollectionObject object {tyson_str_data.size()};
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
        tyson::TySonObject data_;
        /*
         * Always a TySON map object
         * s|meta|:find_meta{s|count|:n|5|}
         */
        std::string meta_txt_;

        std::string count_;
        MetaType metaType = MetaType::none;


        /**
         * parsing the data part of meta into a TySonObject
         * the data part comes after `find_meta`
         *
         * exampl.: s|meta|:find_meta{s|count|:n|5|}
         * the data part here is: `{s|count|:n|5|}`
         */
        void parse_data() noexcept
        {
            auto pos_map_start = meta_txt_.find('{');
            auto pos_map_end = meta_txt_.rfind('}') + 1;
            auto map_str = "m" + meta_txt_.substr(pos_map_start, pos_map_end - pos_map_start);
            data_ = tyson::TySonObject {map_str};
        }

        /**
         * parsing the kind of meta into a MetaType
         * the kind/MetaType is the part between s|meta| and {s|count|:n|5|}
         *
         * exampl.: s|meta|:find_meta{s|count|:n|5|}
         * the MetaType part here is: `find_meta`
         */
        void parse_type() noexcept
        {
            auto pos_type_start = meta_txt_.find(':');
            auto pos_type_end = meta_txt_.find('{');
            auto meta_type_str = meta_txt_.substr(pos_type_start, pos_type_end - pos_type_start);
            metaType = metaTypes.at(meta_type_str);
        }

        friend std::ostream & operator<<(std::ostream &os, const Meta& meta) noexcept
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

        /**
         * Creating a new Meta object from the AnnaDB response
         *
         * @param meta_txt string
         */
        explicit Meta(std::string_view meta_txt) noexcept
        {
            meta_txt_ = meta_txt;
            parse_data();
            parse_type();
        }

        ~Meta() = default;

        /**
         *
         * @return the data part of the meta object
         */
        tyson::TySonObject data() noexcept
        {
            return data_;
        }
    
        template<typename T>
        requires std::is_integral_v<T>
        std::optional<T> rows() noexcept
        {
            auto count = data_["count"];
            if (count)
            {
                auto res = count.value().value<T>();
                return res;
            }
            return {};
        }

        /**
         *
         * @return the type part of the meta object
         */
        MetaType type() noexcept
        {
            return metaType;
        }
    };

    class Journal
    {
        std::string data_;
        std::string meta_;
        bool result_ = false;

        /**
         * Parse the AnnaDB query response into a meta and a data object
         * exampl.: `result:<ok|false>[response{s|data|<...>, s|meta|<...>}]`
         *
         * @param response string
         */
        void parse_response(std::string_view response) noexcept
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

        friend std::ostream & operator<<(std::ostream &os, const Journal& journal) noexcept
        {
            std::string result = journal.ok() ? "ok" : "err";
            auto meta = journal.meta();
            auto data = "journal.data()"; // journal.data();

            std::string repr = "result:" + result + "[response{";
            return os << repr << data << meta << ",},];";
        }

    public:

        /**
         * Creating a new Journal object from the AnnaDB response
         *
         * @param response string
         */
        explicit Journal(std::string_view response) noexcept
        {
            parse_response(response);
        }

        ~Journal() = default;

        /**
         * Indicates if the query request was successful
         *
         * @return the query result
         */
        [[nodiscard]] bool ok() const noexcept
        {
            return result_;
        }

        /**
         *
         * @return the meta part of the AnnaDB query response
         */
        [[nodiscard]] Meta meta() const noexcept
        {
            Meta meta{meta_};
            return meta;
        }

        /**
         *
         * @return the data part of the AnnaDB query response
         */
        [[nodiscard]] Data data() const noexcept
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

        bool zmq_send(std::string_view query) noexcept
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

        std::optional<std::string> zmq_receive() noexcept
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

        /**
         * Create a new AnnaDB object with settings to enable a connection.
         *
         * @param username string
         * @param password string
         * @param host string
         * @param port number
         */
        AnnaDB(std::string_view username,
               std::string_view password,
               std::string_view host,
               u_short port
        ) noexcept : username_(username),
            password_(password),
            host_(host),
            port_(std::to_string(port))
        {
        };

        ~AnnaDB() = default;

        /**
         * open a connection with the AnnaDB
         */
        void connect() noexcept
        {
            requester.connect("tcp://" + host_ + ":" + port_);
        }

        /**
         * close the connection to the AnnaDB
         */
        void close() noexcept
        {
            requester.close();
        }

        /**
         * Send a TySON formatted query to AnnaDB
         *
         * @param query string in TySON format
         * @return a Journal object representing the result of the query if successful
         */
        [[nodiscard]] std::optional<Journal> send(std::string_view query) noexcept
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

        /**
         * Send a TySON formatted query to AnnaDB
         *
         * @param query @see query.annadb::Query::Query
         * @return a Journal object representing the result of the query if successful
         */
        [[nodiscard]] std::optional<Journal> send(annadb::Query::Query &query) noexcept
        {
            std::stringstream sstream;
            sstream << query;
            
            auto query_str = sstream.str();
            auto result = zmq_send(query_str);

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

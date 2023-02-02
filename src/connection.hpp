//
// Created by felix on 28.01.23.
//

#ifndef ANNADB_DRIVER_CONNECTION_HPP
#define ANNADB_DRIVER_CONNECTION_HPP

#include <map>
#include <valarray>
#include <zmq.hpp>


namespace annadb
{
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

    class Meta
    {
        /*
         * Always a TySON map object
         * s|meta|:find_meta{s|count|:n|5|}
         */
        std::string meta_txt_;

        std::string count_;
        MetaType metaType = MetaType::none;

        void parse_count()
        {
            std::string count_str = "s|count|:n|";
            auto pos_count = meta_txt_.rfind(count_str) + count_str.size();
            auto pos_num_end = meta_txt_.rfind('|');
            count_ = meta_txt_.substr(pos_count, pos_num_end - pos_count);
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
            std::string repr = "{s|count|:n|" + meta.count_ + "|";
            return os << "s|meta|:" << meta.metaType << repr;
        }

    public:

        explicit Meta(std::string_view meta_txt)
        {
            meta_txt_ = meta_txt;
            parse_count();
            parse_type();
        }

        template<typename T>
        T count()
        {
            std::stringstream sstream;
            T outputval;

            sstream << count_;
            sstream >> outputval;

            return outputval;
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

            std::string repr = "result:" + result + "[response{" + journal.data();
            return os << repr << meta << ",},];";
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

        Meta meta() const
        {
            Meta meta{meta_};
            return meta;
        }

        [[nodiscard]] std::string data() const
        {
            return data_;
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

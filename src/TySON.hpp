//
// Created by felix on 29.01.23.
//

#ifndef ANNADB_DRIVER_TYSON_HPP
#define ANNADB_DRIVER_TYSON_HPP

#include <ranges>
#include <string_view>

namespace tyson
{
    enum class TySonType
    {
        Number = 'n',
        String = 's',
        Bool = 'b',
        Null = 'x',
        Timestamp = 'u',
        Link = 'l',
        Vector = 'v',
        Map = 'm',
    };

    class TySonObject
    {
        TySonType type_;
        std::string value_;
        std::vector<TySonObject> vector_ {};
        std::map<TySonObject, TySonObject> map_ {};
        std::pair<std::string, std::string> link_ {};

        void parse_vector_elements(std::string_view object)
        {
            auto end_type_sep = object.find_first_of('[') + 1;
            auto end_value_sep = (object.size() - 1) - end_type_sep;

            auto vector_data = object.substr(end_type_sep, end_value_sep);

            for (const auto elem : vector_data | std::views::split(','))
            {
                std::string tmp;

                for (auto &chr : elem)
                {
                    tmp += chr;
                }

                auto obj = TySonObject(tmp);
                vector_.emplace_back(obj);
            }
        }

        std::vector<TySonObject> parse_map_element(std::string_view object)
        {
            std::vector<TySonObject> result;
            result.reserve(2);

            for (const auto map_pair: object | std::ranges::views::split(':'))
            {
                std::string tmp;
                for (auto &chr : map_pair)
                {
                    tmp += chr;
                }
                result.emplace_back(tmp);
            }

            return result;
        }

        void parse_map_elements(std::string_view object)
        {
            auto end_type_sep = object.find_first_of('{') + 1;
            auto end_value_sep = (object.size() - 1) - end_type_sep;

            auto map_data = object.substr(end_type_sep, end_value_sep);
            for (const auto map_pair: map_data | std::ranges::views::split(','))
            {
                std::string tmp;
                for (auto &chr : map_pair)
                {
                    tmp += chr;
                }

                auto key_val = parse_map_element(tmp);
                map_.try_emplace(key_val[0], key_val[1]);
            }
        }

    public:
        explicit TySonObject(std::string_view object)
        {
            auto end_type_sep = object.find_first_of('|');
            auto end_value_sep = (object.size() - 1) - (end_type_sep + 1);
            auto type = object.substr(0, end_type_sep);

            if (object == "null")
            {
                type_ = TySonType::Null;
                value_ = {};
                return;
            }

            if (object.starts_with("uts"))
            {
                type_ = TySonType::Timestamp;
                value_ = object.substr(end_type_sep + 1, end_value_sep);
            }
            else if (object.starts_with('v'))
            {
                type_ = TySonType::Vector;

                parse_vector_elements(object);
            }
            else if (object.starts_with('m'))
            {
                type_ = TySonType::Map;
                parse_map_elements(object);
            }
            else if (type.size() > 1)
            {
                type_ = TySonType::Link;
                auto val = object.substr(end_type_sep + 1, end_value_sep);
                link_ = std::make_pair(type, val);
            }
            else
            {
                type_ = static_cast<TySonType>(type.at(0));
                value_ = object.substr(end_type_sep + 1, end_value_sep);
            }
        }

        TySonObject(const TySonObject& rhs) : type_(rhs.type_),
                                              value_(rhs.value_),
                                              vector_(rhs.vector_),
                                              map_(rhs.map_),
                                              link_(rhs.link_){}

        bool operator==(const TySonObject &rhs) const
        {
            return std::tie(this->type_, this->value_, this->vector_, this->map_, this->link_) ==
            std::tie(rhs.type_, rhs.value_, rhs.vector_, rhs.map_, rhs.link_);
        }

        bool operator<(const TySonObject &rhs) const
        {
            return std::tie(this->type_, this->value_, this->vector_) < std::tie(rhs.type_, rhs.value_, rhs.vector_);
        }

        [[nodiscard]] TySonType type() const
        {
            return type_;
        }

        template<TySonType T>
        [[nodiscard]] std::string value() const
        {
            return value_;
        }

        template<TySonType T>
        requires (T == TySonType::Bool)
        [[nodiscard]] bool value() const
        {
            return value_ == "true";
        }

        template<TySonType T>
        requires (T == TySonType::Link)
        [[nodiscard]] std::pair<std::string, std::string> value() const
        {
            return link_;
        }

        template<TySonType T>
        requires (T == TySonType::Vector)
        [[nodiscard]] std::vector<TySonObject> value() const
        {
            return vector_;
        }

        template<TySonType T>
        requires (T == TySonType::Map)
        [[nodiscard]] std::map<TySonObject, TySonObject> value() const
        {
            return map_;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        [[nodiscard]] T value() const
        {
            if (type_ == TySonType::Number || type_ == TySonType::Bool || type_ == TySonType::Timestamp)
            {
                std::size_t pos{};
                T result{};

                switch (*typeid(T).name())
                {
                    case 's':
                    case 'j':
                    case 'i':
                        result = std::stoi(value_, &pos);
                        break;
                    case 'l':
                        result = std::stol(value_, &pos);
                        break;
                    case 'f':
                        result = static_cast<T>(std::stof(value_, &pos));
                        break;
                    case 'd':
                        result = static_cast<T>(std::stod(value_, &pos));
                        break;
                    case 'x':
                        result = std::stoll(value_, &pos);
                        break;
                    case 'e':
                        result = static_cast<T>(std::stold(value_, &pos));
                        break;
                    case 'b':
                        result = value_ == "true";
                        break;
                    case 'c':
                        result = value_.c_str()[0];
                        break;
                }
                return result;
            }
            else
            {
                throw std::invalid_argument("Invalid TySonType");
            }
        }
    };


    class TySonCollectionObject
    {
        std::vector<TySonObject> collection_objects_ {};

    public:
        TySonCollectionObject() = default;

        void add(TySonObject &object)
        {
            collection_objects_.emplace_back(object);
        };
    };


    class TySonData
    {
        std::string data_;

    public:
        explicit TySonData(std::string_view data) : data_(data) {}

        template<const char* key>
        static std::optional<TySonCollectionObject> get(const TySonData &tysonData)
        {
            if (strcmp(key, "objects") == 0 || strcmp(key, "ids") == 0)
            {
                auto begin = std::string("s|data|:") + key;
                if (tysonData.data_.starts_with(begin))
                {
                    TySonCollectionObject object {};
                    // object.add();
                    return object;
                }
            }
            return {};
        }
    };

}

#endif //ANNADB_DRIVER_TYSON_HPP

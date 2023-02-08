//
// Created by felix on 29.01.23.
//

#ifndef ANNADB_DRIVER_TYSON_HPP
#define ANNADB_DRIVER_TYSON_HPP

#include <algorithm>
#include <ranges>
#include <string_view>
#include "utils.hpp"

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
        Object = 'o',
        ID = 'i',
        Objects = 'y',
        IDs = 'z'
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
            const auto to_tyson_object = [](std::string &val) -> TySonObject {
                return TySonObject{val};
            };

            auto end_type_sep = object.find_first_of('[') + 1;
            auto end_value_sep = (object.size() - 1) - end_type_sep;

            auto vector_data = object.substr(end_type_sep, end_value_sep);

            auto vec_data = utils::split(vector_data, ',');
            std::transform(vec_data.begin(), vec_data.end(), std::back_inserter(vector_), to_tyson_object);
        }

        std::vector<TySonObject> parse_map_element(std::string_view object)
        {
            const auto to_tyson_object = [](std::string &val) -> TySonObject {
                return TySonObject{val};
            };

            auto vec_data = utils::split(object, ':');

            std::vector<TySonObject> result;
            result.reserve(vec_data.size());

            std::transform(vec_data.begin(), vec_data.end(), std::back_inserter(result), to_tyson_object);
            return result;
        }

        void parse_map_elements(std::string_view object)
        {
            auto end_type_sep = object.find_first_of('{') + 1;
            auto end_value_sep = (object.size() - 1) - end_type_sep;
            auto map_data = object.substr(end_type_sep, end_value_sep);

            auto vec_data = utils::split(map_data, ',');

            auto to_map_element = [this](auto &val) -> std::vector<TySonObject> {
                return parse_map_element(val);
            };

            std::vector<std::vector<TySonObject>> result;
            std::transform(vec_data.begin(), vec_data.end(), std::back_inserter(result), to_map_element);
            std::for_each(result.begin(), result.end(), [this](auto key_val){map_.try_emplace(key_val[0], key_val[1]);});
        }

    public:

        TySonObject() = default;

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

        ~TySonObject()
        {
            value_.clear();
            vector_.clear();
            map_.clear();
            link_ = {};
        }

        TySonObject(const TySonObject& rhs) : type_(rhs.type_),
                                              value_(rhs.value_),
                                              vector_(rhs.vector_),
                                              map_(rhs.map_),
                                              link_(rhs.link_){}

        TySonObject& operator=(const TySonObject &rhs) = default;

        bool operator==(const TySonObject &rhs) const
        {
            return std::tie(this->type_, this->value_, this->vector_, this->map_, this->link_) ==
            std::tie(rhs.type_, rhs.value_, rhs.vector_, rhs.map_, rhs.link_);
        }

        bool operator<(const TySonObject &rhs) const
        {
            return std::tie(this->type_, this->value_, this->vector_) < std::tie(rhs.type_, rhs.value_, rhs.vector_);
        }

        std::optional<TySonObject> operator[](const std::string_view key) const
        {
            if (type_ == TySonType::Map)
            {
                TySonObject result;
                std::for_each(map_.begin(),
                              map_.end(),
                              [&result, &key](const std::pair<TySonObject, TySonObject> &obj)
                              {
                                    if (obj.first.value_ == key)
                                    {
                                        result = obj.second;
                                    }
                              });
                return result;
            }
            else
            {
                return {};
            }
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
        std::vector<TySonObject> collection_ids_ {};
        std::vector<std::pair<TySonObject, TySonObject>> collection_objects_ {};

    public:
        TySonCollectionObject() = default;

        void add(const std::string_view &object)
        {
            collection_ids_.emplace_back(object);
        };

        void add(const std::pair<std::string_view, std::string_view> &object)
        {
            collection_objects_.emplace_back(TySonObject(object.first), TySonObject(object.second));
        };

        template<TySonType T>
        requires (T == TySonType::Object)
        std::optional<std::pair<TySonObject, TySonObject>> get(std::string_view obj_id)
        {
            for (const auto &val: collection_objects_)
            {
                if (val.first.value<TySonType::Link>().second == obj_id)
                {
                    return val;
                }
            }
            return {};
        }

        template<TySonType T>
        requires (T == TySonType::Objects)
        std::vector<std::pair<TySonObject, TySonObject>> get(std::string_view collection)
        {
            std::vector<std::pair<TySonObject, TySonObject>> result {};
            std::for_each(collection_objects_.begin(), collection_objects_.end(),
                         [&collection, &result](const std::pair<TySonObject, TySonObject> &val)
                         {
                             if (val.first.value<TySonType::Link>().first == collection)
                             {
                                 result.emplace_back(val);
                             }
                         });
            return result;
        }

        template<TySonType T>
        requires (T == TySonType::Object)
        std::optional<std::pair<TySonObject, TySonObject>> get(std::string_view collection, std::string_view obj_id)
        {
            for (const std::pair<TySonObject, TySonObject> &val: collection_objects_)
            {
                auto tysonLink = val.first.value<TySonType::Link>();
                if (std::tie(tysonLink.first, tysonLink.second) == std::tie(collection, obj_id))
                {
                    return val;
                }
            }

            return {};
        }

        template<TySonType T>
        requires (T == TySonType::ID)
        std::optional<TySonObject> get(std::string_view obj_id)
        {
            for (const TySonObject &val: collection_ids_)
            {
                if (val.value<TySonType::Link>().second == obj_id)
                {
                    return val;
                }
            }
            return {};
        }

        template<TySonType T>
        requires (T == TySonType::ID)
        std::vector<TySonObject> get(std::string_view collection)
        {
            std::vector<TySonObject> result {};
            std::copy_if(collection_ids_.begin(), collection_ids_.end(),
                         std::back_inserter(result),
                         [&collection](const TySonObject &val)
                         {
                             if (val.value<TySonType::Link>().first == collection)
                             {
                                 return val;
                             }
                         });
            return result;
        }

        template<TySonType T>
        requires (T == TySonType::ID)
        std::optional<TySonObject> get(std::pair<std::string_view, std::string_view> collection_id)
        {
            for (const TySonObject &val: collection_ids_)
            {
                auto tysonLink = val.value<TySonType::Link>();
                if (std::tie(tysonLink.first, tysonLink.second) == std::tie(collection_id.first, collection_id.second))
                {
                    return val;
                }
            }

            return {};
        }
    };
}

#endif //ANNADB_DRIVER_TYSON_HPP

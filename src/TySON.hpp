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
    /**
     * AnnaDB data types
     */
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
        IDs = 'z',
        Value = 'e'
    };

    class TySonObject
    {
        std::vector<TySonObject> vector_{};
        std::map<TySonObject, TySonObject> map_{};
        std::pair<std::string, std::string> link_{};
        TySonType type_;
        std::string value_;

        /**
         * Parse a AnnaDB(TySON) Vector into a std::vector<TySonObject>
         * AnnaDB(TySON) Vector example:
         * v[n|1|,n|2|,n|3|,]
         *
         * @param object string_view the raw string of a AnnaDB(TySON) Vector
         * @see Tyson.tyson::TySonObject
         */
        void parse_vector_elements(std::string_view object)
        {
            const auto to_tyson_object = [](std::string &val) -> TySonObject
            {
                return TySonObject{val};
            };

            auto end_type_sep = object.find_first_of('[') + 1;
            auto end_value_sep = (object.size() - 1) - end_type_sep;

            auto vector_data = object.substr(end_type_sep, end_value_sep);

            auto vec_data = utils::split(vector_data, ',');
            std::transform(vec_data.begin(), vec_data.end(), std::back_inserter(vector_), to_tyson_object);
        }

        /**
         * Parse AnnaDB(TySON) Map values into a std::vector<TySonObject>
         * AnnaDB(TySON) Map example:
         * m{ s|bar|: s|baz|,}
         *
         * @param object string_view the raw string of a AnnaDB(TySON) Map
         * @return the inner elements of a TySON Map parsed into a TySonObject
         * @see Tyson.tyson::TySonObject
         */
        std::vector<TySonObject> parse_map_element(std::string_view object)
        {
            const auto to_tyson_object = [](std::string &val) -> TySonObject
            {
                return TySonObject{val};
            };

            auto vec_data = utils::split(object, ':');

            std::vector<TySonObject> result;
            result.reserve(vec_data.size());

            std::transform(vec_data.begin(), vec_data.end(), std::back_inserter(result), to_tyson_object);
            return result;
        }

        /**
         * Parse AnnaDB(TySON) Map into a std::vector<TySonObject>
         * AnnaDB(TySON) Map example:
         * m{ s|bar|: s|baz|,}
         *
         * @param object string_view the raw string of a AnnaDB(TySON) Map
         * @see Tyson.tyson::TySonObject
         */
        void parse_map_elements(std::string_view object)
        {
            auto end_type_sep = object.find_first_of('{') + 1;
            auto end_value_sep = (object.size() - 1) - end_type_sep;
            auto map_data = object.substr(end_type_sep, end_value_sep);

            auto vec_data = utils::split(map_data, ',');

            auto to_map_element = [this](auto &val) -> std::vector<TySonObject>
            {
                return parse_map_element(val);
            };

            std::vector<std::vector<TySonObject>> result;
            std::transform(vec_data.begin(), vec_data.end(), std::back_inserter(result), to_map_element);
            std::for_each(result.begin(), result.end(), [this](auto key_val)
            {
                map_.try_emplace(key_val[0], key_val[1]);
            });
        }

        friend std::ostream& operator<<(std::ostream &out, TySonObject const &obj)
        {
            switch (obj.type())
            {
                case TySonType::Number:
                    return out << "n|" << obj.value_ << "|";
                case TySonType::String:
                    return out << "s|" << obj.value_ << "|";
                case TySonType::Bool:
                    return out << "b|" << obj.value_ << "|";
                case TySonType::Null:
                    return out << "null";
                case TySonType::Timestamp:
                    return out << "utc|" << obj.value_ << "|";
                case TySonType::Link:
                    return out << std::get<0>(obj.link_) << "|" << std::get<1>(obj.link_) << "|";
                case TySonType::Value:
                    return out << "value|" << obj.map_.begin()->first.value_ << "|:" << obj.map_.begin()->second;
                case TySonType::Vector:
                {
                    std::stringstream sstream;
                    std::for_each(obj.vector_.begin(),
                                  obj.vector_.end(),
                                  [&sstream](const auto &val){ sstream << val << ","; });

                    return out << "v[" << sstream.str() << "]";
                }
                case TySonType::Map:
                {
                    std::stringstream sstream;
                    std::for_each(obj.map_.begin(),
                                  obj.map_.end(),
                                  [&sstream](const std::pair<TySonObject, TySonObject> &val)
                                  {
                                        sstream << val.first << ":" << val.second << ",";
                                  });

                    return out << "m{" << sstream.str() << "}";
                }
                default:
                    return out << "";
            }
        }

    public:

        TySonObject() : type_(tyson::TySonType::Null) {};
        ~TySonObject() = default;

        /**
         * Create a new TySonObject from a raw AnnaDB data type string
         *
         * @param object representing a AnnaDB data type
         * @see <a href="https://annadb.dev/documentation/data_types/">AnnaDB data types</a>
         */
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

        bool operator==(const TySonObject &rhs) const
        {
            return std::tie(this->type_, this->value_, this->vector_, this->map_, this->link_) ==
                   std::tie(rhs.type_, rhs.value_, rhs.vector_, rhs.map_, rhs.link_);
        }

        bool operator<(const TySonObject &rhs) const
        {
            return std::tie(this->type_, this->value_, this->vector_) < std::tie(rhs.type_, rhs.value_, rhs.vector_);
        }

        /**
         * Create a new TySonObject Number
         *
         * @tparam T an integral type
         * @param number the value of the new TySonObject
         * @return new TySonObject
         */
        template<typename T>
        requires std::is_integral_v<T>
        static TySonObject Number(T number)
        {
            TySonObject tySonObject {};
            tySonObject.value_ = std::to_string(number);
            tySonObject.type_ = TySonType::Number;
            return tySonObject;
        }


        /**
         * Create a new TySonObject String
         *
         * @param str the value of the new TySonObject
         * @return new TySonObject
         */
        static TySonObject String(const std::string &str)
        {
            TySonObject tySonObject {};
            tySonObject.value_ = str;
            tySonObject.type_ = TySonType::String;
            return tySonObject;
        }

        /**
         * Create a new TySonObject Bool
         *
         * @param bl the value of the new TySonObject
         * @return new TySonObject
         */
        static TySonObject Bool(bool bl)
        {
            TySonObject tySonObject {};
            tySonObject.value_ = bl ? "true" : "false";
            tySonObject.type_ = TySonType::Bool;
            return tySonObject;
        }

        /**
         * Create a new TySonObject Null
         *
         * @return new TySonObject
         */
        static TySonObject Null()
        {
            TySonObject tySonObject {};
            tySonObject.value_ = "null";
            tySonObject.type_ = TySonType::Null;
            return tySonObject;
        }

        /**
         * Create a new TySonObject Timestamp
         *
         * @param seconds that have elapsed since the Unix epoch
         * @return new TySonObject
         */
        static TySonObject Timestamp(unsigned long long seconds)
        {
            TySonObject tySonObject {};
            tySonObject.value_ = std::to_string(seconds);
            tySonObject.type_ = TySonType::Timestamp;
            return tySonObject;
        }

        /**
         * Create a new TySonObject Link
         *
         * @param collection the name of the reference collection
         * @param uuid the id of the element inside of the collection
         * @return new TySonObject
         */
        static TySonObject Link(const std::string &collection, const std::string &uuid)
        {
            TySonObject tySonObject {};
            tySonObject.link_ = {collection, uuid};
            tySonObject.type_ = TySonType::Link;
            return tySonObject;
        }

        /**
         * Create a new TySonObject Vector
         *
         * @param objs a std::vector of TySonObjects
         * @return new TySonObject
         */
        static TySonObject Vector(std::vector<TySonObject> &objs)
        {
            TySonObject tySonObject {};
            tySonObject.vector_ = {std::move(objs)};
            tySonObject.type_ = TySonType::Vector;
            return tySonObject;
        }

        static TySonObject Value(const std::string &field, TySonObject &val)
        {
            TySonObject tySonObject {};
            tySonObject.map_.try_emplace(TySonObject::String(field), val);
            tySonObject.type_ = TySonType::Value;
            return tySonObject;
        }

        /**
         * Create a new TySonObject Map
         *
         * @param objs a std::map of std::string and TySonObject
         * @return new TySonObject
         */
        static TySonObject Map(std::map<std::string, TySonObject> &objs)
        {
            TySonObject tySonObject {};
            std::for_each(objs.begin(), objs.end(),
                          [&tySonObject](std::pair<const std::string, TySonObject> &val)
                          {
                                tySonObject.map_.try_emplace(TySonObject::String(val.first), std::move(val.second));
                          });
            tySonObject.type_ = TySonType::Map;
            return tySonObject;
        }

        /**
         * Get the value of a AnnaDB Map as TySonObject
         *
         * @param key must be a string inside of AnnaDB Map
         * @return TySonObject value from a TySonObject Map if exists
         */
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

        /**
         *
         * @return the type name of the current TysonObject
         */
        [[nodiscard]] TySonType type() const
        {
            return type_;
        }

        /**
         * Get the current value of the TySonObject
         *
         * @tparam T must be of type TySonType
         * @return the current value representation
         */
        template<TySonType T>
        [[nodiscard]] std::string value() const
        {
            return value_;
        }

        /**
         * If the TySonObject represents a AnnaDB Bool Primitive
         *
         * @tparam T TySonType::Bool
         * @return true if the value representation is the string `true` false otherwise
         */
        template<TySonType T>
        requires (T == TySonType::Bool)
        [[nodiscard]] bool value() const
        {
            return value_ == "true";
        }

        /**
         * If the TySonObject represents a AnnaDB null Primitive
         *
         * @tparam T TySonType::Null
         * @return an empty string to indicate null
         */
        template<TySonType T>
        requires (T == TySonType::Null)
        [[nodiscard]] std::string value() const
        {
            return "";
        }

        /**
         * If the TySonObject represents a AnnaDB Link Primitive
         *
         * @tparam T TySonType::Link
         * @return collection name and object_id as uuid-string
         */
        template<TySonType T>
        requires (T == TySonType::Link)
        [[nodiscard]] std::pair<std::string, std::string> value() const
        {
            return link_;
        }

        /**
         * If the TySonObject represents a AnnaDB Vector
         *
         * @tparam T TySonType::Vector
         * @return vector of TySonObjects
         */
        template<TySonType T>
        requires (T == TySonType::Vector)
        [[nodiscard]] std::vector<TySonObject> value() const
        {
            return vector_;
        }

        /**
         * If the TySonObject represents a AnnaDB Map
         *
         * @tparam T TySonType::Map
         * @return Key-Value pairs of TySonObjects
         */
        template<TySonType T>
        requires (T == TySonType::Map)
        [[nodiscard]] std::map<TySonObject, TySonObject> value() const
        {
            return map_;
        }

        /**
         * If the TySonObject represents a AnnaDB Number Primitive
         * you can parse it into the type you want normal it will be returned as string
         *
         * @tparam T must be a arithmetic type
         * @return the casted value
         */
        template<typename T>
        requires std::is_arithmetic_v<T>
        [[nodiscard]] T value() const
        {
            if (type_ == TySonType::Number || type_ == TySonType::Bool || type_ == TySonType::Timestamp)
            {
                std::size_t pos{};

                switch (*typeid(T).name())
                {
                    case 's':
                    case 'j':
                    case 'i':
                        return std::stoi(value_, &pos);
                    case 'l':
                        return std::stol(value_, &pos);
                    case 'f':
                        return static_cast<T>(std::stof(value_, &pos));
                    case 'd':
                        return static_cast<T>(std::stod(value_, &pos));
                    case 'x':
                        return std::stoll(value_, &pos);
                    case 'e':
                        return static_cast<T>(std::stold(value_, &pos));
                    case 'b':
                        return value_ == "true";
                    case 'c':
                        return value_.c_str()[0];
                }
            }

            throw std::invalid_argument("Invalid Type");
        }
    };


    class TySonCollectionObject
    {
        std::vector<TySonObject> collection_ids_{};
        std::vector<std::pair<TySonObject, TySonObject>> collection_objects_{};

    public:
        TySonCollectionObject() = default;

        ~TySonCollectionObject() = default;

        /**
         * Add a new AnnaDB(TySon) string representation to the current collection.
         * It will be parsed automatically into a TySonObject
         *
         * @param object
         */
        void add(const std::string_view &object)
        {
            collection_ids_.emplace_back(object);
        };

        /**
         * Add a new AnnaDB(TySon) pair of string representations to the current collection.
         * It will be parsed automatically into a TySonObject
         *
         * @param object
         */
        void add(const std::pair<std::string_view, std::string_view> &object)
        {
            collection_objects_.emplace_back(TySonObject(object.first), TySonObject(object.second));
        };

        /**
         * Get the node value from the AnnaDB response data|:objects
         * Example:
         *      test|ea63e06f-9d1c-442f-89fd-c5041d863f5f|:s|foo|,
         *      `get("ea63e06f-9d1c-442f-89fd-c5041d863f5f")` => TySonObject with value `foo` and type `String`
         *
         * @tparam T TySonType::Object
         * @param obj_id uuid-string
         * @return TySonObject if found by the link_id
         */
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

        /**
         * Get all nodes from the AnnaDB response data|:objects which belongs to a specific collection
         *
         * @tparam T TySonType::Objects
         * @param collection name
         * @return vector of TySonObject pairs which belongs to the collection
         */
        template<TySonType T>
        requires (T == TySonType::Objects)
        std::vector<std::pair<TySonObject, TySonObject>> get(std::string_view collection)
        {
            std::vector<std::pair<TySonObject, TySonObject>> result{};
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

        /**
         * Get the node from the AnnaDB response data|:objects
         *
         * @tparam T TySonType::Object
         * @param collection name
         * @param obj_id uuid-string
         * @return TySonObject pair if found by collection name and link_id
         */
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

        /**
         * Get the node object from the AnnaDB response data|:ids
         *
         * @tparam T TySonType::ID
         * @param obj_id uuid-string
         * @return TySonObject if found by link_id
         */
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

        /**
         * Get all nodes from the AnnaDB response data|:ids
         *
         * @tparam T TySonType::ID
         * @param collection name
         * @return vector of TySonObjects which belongs to the collection found by collection name
         */
        template<TySonType T>
        requires (T == TySonType::ID)
        std::vector<TySonObject> get(std::string_view collection)
        {
            std::vector<TySonObject> result{};
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

        /**
         * Get the node from the AnnaDB response data|:ids
         *
         * @tparam T TySonType::ID
         * @param collection name
         * @param obj_id uuid-string
         * @return TySonObject if found by collection name and link_id
         */
        template<TySonType T>
        requires (T == TySonType::ID)
        std::optional<TySonObject> get(std::string_view collection, std::string_view obj_id)
        {
            for (const TySonObject &val: collection_ids_)
            {
                auto tysonLink = val.value<TySonType::Link>();
                if (std::tie(tysonLink.first, tysonLink.second) == std::tie(collection, obj_id))
                {
                    return val;
                }
            }

            return {};
        }
    };
}

#endif //ANNADB_DRIVER_TYSON_HPP

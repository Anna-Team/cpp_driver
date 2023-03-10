//
// Created by felix on 19.02.23.
//

/**
 * Comparison operators
Equal - eq{...}
Not equal - neq{...}
Greater than - gt{...}
Greater than or equal - gte{...}
Less than - lt{...}
Less than or equal - lte{...}


Logical operators
And - and[...]
Or - or[...]
Not - not(...)

In order to compare the value of the object root path notation could be used:
    eq{root: s|bar|}
In order to compare a field of the object path to value notation could be used:
    eq{value|path.to.field|: s|bar|}

 */

#ifndef ANNADB_DRIVER_QUERY_COMPARISION_HPP
#define ANNADB_DRIVER_QUERY_COMPARISION_HPP

#include <utility>

#include "TySON.hpp"

namespace annadb::Query
{
    /**
     * Inside of the compare statement you must decide how to compare
     *
     */
    enum class ComparisonType
    {
        EQ,
        NEQ,
        GT,
        GTE,
        LT,
        LTE,
        AND,
        OR,
        NOT
    };
    
    /**
     * The Base class for all comparison options
     */
    class Comparison
    {
        tyson::TySonObject value_;
        
        [[nodiscard]] virtual std::string to_string()
        {
            std::stringstream sstream;
            sstream << name_ << "{";
            
            if (field_ == "root")
            {
                sstream << "root: ";
            }
            else
            {
                sstream << "value|" << field_ << "|: ";
            }
            
            sstream << value_ << "}";
            return sstream.str();
        }
        
        friend std::ostream &operator<<(std::ostream &out, Comparison &obj)
        {
            return out << obj.to_string();
        }
    
    protected:
        std::string field_;
        std::string name_;
    
    public:
        explicit Comparison(std::string_view &field, std::string_view name) : value_(tyson::TySonObject::Null()),
                                                                              field_(field), name_(name)
        {}
        
        explicit Comparison(std::string field, std::string_view name) : value_(tyson::TySonObject::Null()),
                                                                        field_(std::move(field)), name_(name)
        {}
        
        template<std::convertible_to<tyson::TySonObject> T>
        explicit
        Comparison(T value, std::string_view name) : value_(std::forward<tyson::TySonObject>(value)), field_("root"),
                                                     name_(name)
        {}
        
        Comparison(tyson::TySonObject &value, std::string_view field, std::string_view name) : value_(value),
                                                                                               field_(field),
                                                                                               name_(name)
        {}
        
        virtual ~Comparison() = default;
        
        std::string str()
        {
            return to_string();
        }
        
        ComparisonType type()
        {
            if (name_ == "eq")
            {
                return ComparisonType::EQ;
            }
            else if (name_ == "neq")
            {
                return ComparisonType::NEQ;
            }
            else if (name_ == "gt")
            {
                return ComparisonType::GT;
            }
            else if (name_ == "gte")
            {
                return ComparisonType::GTE;
            }
            else if (name_ == "lt")
            {
                return ComparisonType::LT;
            }
            else if (name_ == "lte")
            {
                return ComparisonType::LTE;
            }
            else if (name_ == "and")
            {
                return ComparisonType::AND;
            }
            else if (name_ == "or")
            {
                return ComparisonType::OR;
            }
            else
            {
                return ComparisonType::NOT;
            }
        }
    };

    /**
     * The equal comparison class
     */
    class Eq : public Comparison
    {
    public:
        explicit Eq(tyson::TySonObject &value) : Comparison(value, "eq") {};
        Eq(std::string_view path_to_field, tyson::TySonObject &value) : Comparison(value, path_to_field, "eq") {};
        explicit Eq(tyson::TySonObject &&value) : Comparison(value, "eq") {};
        Eq(std::string_view path_to_field, tyson::TySonObject &&value) : Comparison(value, path_to_field, "eq") {};
    };

    /**
     * The not equal comparison class
     */
    class Neq : public Comparison
    {
    public:
        explicit Neq(tyson::TySonObject &value) : Comparison(value, "neq") {};
        Neq(std::string_view path_to_field, tyson::TySonObject &value) : Comparison(value, path_to_field, "neq") {};
        explicit Neq(tyson::TySonObject &&value) : Comparison(value, "neq") {};
        Neq(std::string_view path_to_field, tyson::TySonObject &&value) : Comparison(value, path_to_field, "neq") {};
    };
    
    /**
     * The greater comparison class
     */
    class Gt : public Comparison
    {
    public:
        explicit Gt(tyson::TySonObject &value) : Comparison(value, "gt") {};
        Gt(std::string_view path_to_field, tyson::TySonObject &value) : Comparison(value, path_to_field, "gt") {};
        explicit Gt(tyson::TySonObject &&value) : Comparison(value, "gt") {};
        Gt(std::string_view path_to_field, tyson::TySonObject &&value) : Comparison(value, path_to_field, "gt") {};
    };
    
    /**
     * The greater or equal comparison class
     */
    class Gte : public Comparison
    {
    public:
        explicit Gte(tyson::TySonObject &value) : Comparison(value, "gte") {};
        Gte(std::string_view path_to_field, tyson::TySonObject &value) : Comparison(value, path_to_field, "gte") {};
        explicit Gte(tyson::TySonObject &&value) : Comparison(value, "gte") {};
        Gte(std::string_view path_to_field, tyson::TySonObject &&value) : Comparison(value, path_to_field, "gte") {};
    };
    
    /**
     * The less comparison class
     */
    class Lt : public Comparison
    {
    public:
        explicit Lt(tyson::TySonObject &value) : Comparison(value, "lt") {};
        Lt(std::string_view path_to_field, tyson::TySonObject &value) : Comparison(value, path_to_field, "lt") {};
        explicit Lt(tyson::TySonObject &&value) : Comparison(value, "lt") {};
        Lt(std::string_view path_to_field, tyson::TySonObject &&value) : Comparison(value, path_to_field, "lt") {};
    };
    
    /**
     * The less or equal comparison class
     */
    class Lte : public Comparison
    {
    public:
        explicit Lte(const tyson::TySonObject &value) : Comparison(value, "lte") {};
        Lte(std::string_view path_to_field, tyson::TySonObject &value) : Comparison(value, path_to_field, "lte") {};
        explicit Lte(const tyson::TySonObject &&value) : Comparison(value, "lte") {};
        Lte(std::string_view path_to_field, tyson::TySonObject &&value) : Comparison(value, path_to_field, "lte") {};
    };
    
    /**
     * The and comparison class
     */
    class And : public Comparison
    {
        std::vector<Comparison> compares_{};
        
        [[nodiscard]] std::string to_string() override
        {
            std::stringstream sstream;
            sstream << "and[";
            
            std::for_each(compares_.begin(), compares_.end(), [&sstream](Comparison &val) { sstream << val << ","; });
            sstream << "]";
            return sstream.str();
        }
    
    public:
        /**
         * Initialise `And` comparison class
         * @param comps a variadic number of comparison objects which should be included in the `And` clause
         */
        template<std::convertible_to<Comparison> ...Comps>
        And(Comps ...comps) : Comparison("", "and")
        {
            compares_.reserve(sizeof ...(comps));
            (compares_.emplace_back(comps), ...);
        }
    };
    
    /**
     * The or comparison class
     */
    class Or : public Comparison
    {
        std::vector<Comparison> compares_{};
        
        [[nodiscard]] std::string to_string() override
        {
            std::stringstream sstream;
            sstream << "or[";
            
            std::for_each(compares_.begin(), compares_.end(), [&sstream](Comparison &val) { sstream << val << ","; });
            sstream << "]";
            return sstream.str();
        }
    
    public:
        /**
         * Initialise the `Or` comparison
         * @param comps a variadic number of comparison objects which should be included in the `Or` clause
         */
        template<std::convertible_to<Comparison> ...Comps>
        explicit Or(Comps ...comps) : Comparison("", "or")
        {
            compares_.reserve(sizeof ...(comps));
            (compares_.emplace_back(comps), ...);
        }
    };
    
    /**
     * The not comparison class
     */
    class Not : public Comparison
    {
        [[nodiscard]] std::string to_string() override
        {
            return name_ + "(value|" + field_ + "|)";
        }
    
    public:
        explicit Not(std::string_view field) : Comparison(field, "not")
        {};
    };
    
}

#endif //ANNADB_DRIVER_QUERY_COMPARISION_HPP

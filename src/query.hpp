//
// Created by felix on 16.02.23.
//

#ifndef ANNADB_DRIVER_QUERY_HPP
#define ANNADB_DRIVER_QUERY_HPP

#include <string>
#include <cstdarg>
#include <utility>
#include "TySON.hpp"
#include "query_comparision.hpp"

namespace annadb::Query
{

    /**
     * Inside of the update statement you can decide
     * if you either want to set the field to a particular value
     * or if you want to increase the field to a particular value
     *
     */
    enum class UpdateType
    {
        Inc = 0,
        Set = 1,
    };

    struct SortCmd
    {
        virtual ~SortCmd() = default;
        virtual std::string data() const = 0;
    };

    class Asc : public SortCmd
    {
        std::string field_;

    public:

        /**
         * Sort in ascending order by
         *
         * @param field
         */
        explicit Asc(std::string_view field) noexcept : field_(field) {}

        [[nodiscard]] std::string data() const override
        {
            return "asc(value|" + field_ + "|)";
        }
    };

    class Desc : public SortCmd
    {
        std::string field_;

    public:

        /**
         * Sort in descending order by
         *
         * @param field
         */
        explicit Desc(std::string_view field) noexcept : field_(field) {}

        [[nodiscard]] std::string data() const override
        {
            return "desc(value|" + field_ + "|)";
        }
    };

    class QueryCmd;

    class QueryCmd
    {
        std::string name_;
        bool start_cmd_ = false;
        [[nodiscard]] virtual std::string annadb_query() = 0;
        [[nodiscard]] virtual std::vector<std::string> previous_steps_() = 0;
        [[nodiscard]] virtual std::vector<std::string> next_steps_() = 0;

    public:
        QueryCmd() = default;
        explicit QueryCmd(std::string_view name, bool start_cmd) : name_(name), start_cmd_(start_cmd) {}
        virtual ~QueryCmd() = default;

        [[nodiscard]] std::string name() noexcept
        {
            return this->name_;
        }

        [[nodiscard]] bool can_start_pipeline() const noexcept
        {
            return start_cmd_;
        }

        [[nodiscard]] virtual std::string query() noexcept
        {
            return this->annadb_query();
        }

        [[nodiscard]] bool next_step_allowed(const std::string &cmdName) noexcept
        {
            if (next_steps_().empty())
            {
                return false;
            }

            auto res = std::find(next_steps_().begin(), next_steps_().end(), cmdName);
            return res != next_steps_().end();
        }

        [[nodiscard]] bool previous_step_allowed(const std::string &cmdName) noexcept
        {
            if (previous_steps_().empty())
            {
                return false;
            }

            auto res = std::find(previous_steps_().begin(), previous_steps_().end(), cmdName);
            return res != next_steps_().end();
        }
    };


    class Insert : public QueryCmd
    {
        std::vector<tyson::TySonObject> values_;

        std::string annadb_query() override
        {
            std::stringstream sstream;
            sstream << "insert[";
            std::for_each(values_.begin(), values_.end(),
                          [&sstream](auto &val){ sstream << val << ",";});
            sstream << "]";
            return sstream.str();
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return {};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {};
        }

    public:
        
        /**
         * Create a new insert statement from a single obj
         *
         * @param obj the TySonObject you want to insert into your DB collection
         * @see TySON::tyson::TySonObject
         */
        explicit Insert(tyson::TySonObject &obj) noexcept : QueryCmd("insert", true)
        {
            values_.emplace_back(std::move(obj));
        }
    
        template<typename ...T>
        explicit Insert(T&& ...values) noexcept : QueryCmd("insert", true)
        {
            std::vector<tyson::TySonObject> objs {};
            objs.reserve(sizeof...(values));
            (objs.emplace_back(values), ...);
            
            values_ = std::move(objs);
        }
    
        /**
         * Create a new insert statement from a vector objs
         *
         * @param objs the vector of TySonObjects you want to insert into your DB collection
         * @see TySON::tyson::TySonObject
         */
        explicit Insert(std::vector<tyson::TySonObject> &objs) noexcept : QueryCmd("insert", true), values_(std::move(objs)) {}

    };


    class Get : public QueryCmd
    {
        std::vector<tyson::TySonObject> values_;

        std::string annadb_query() override
        {
            std::stringstream sstream;
            sstream << "get[";
            std::for_each(values_.begin(), values_.end(),
                          [&sstream](auto &val){ sstream << val << ",";});
            sstream << "]";
            return sstream.str();
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        
        /**
         * Create a new get-Query statement from a single obj
         *
         * @param obj must be of TySonType::Link
         * @see TySON.tyson::TySonType
         *
         * @throw invalid_argument if obj is not TySonType::Link
         */
        Get(tyson::TySonObject &obj) : QueryCmd("get", true)
        {
            if (obj.type() != tyson::TySonType::Link)
            {
                throw std::invalid_argument(".get can only be used with a TySonObject of TySonType::Link.");
            }
            
            values_.emplace_back(obj);
        }
    
        /**
         * Create a new get-Query statement from a vector of objs
         *
         * @param objs must be a vector of TySonType::Link's
         * @see TySON.tyson::TySonType
         *
         * @throw invalid_argument if not all objs are TySonType::Link's
         */
        Get(std::vector<tyson::TySonObject> &objs) : QueryCmd("get", true)
        {
            auto all_Links = std::all_of(objs.cbegin(), objs.cend(), [](const tyson::TySonObject val)
            {
                return val.type() == tyson::TySonType::Link;
            });
            
            if (!all_Links)
            {
                throw std::invalid_argument(".get can only be used with a TySonObject of TySonType::Link.");
            }
    
            values_ = std::move(objs);
        }
    
        template<typename ...T>
        explicit Get(T &&...values) : QueryCmd("get", true)
        {
            std::vector<tyson::TySonObject> objs {};
            objs.reserve(sizeof...(values));
            (objs.emplace_back(values), ...);
            
            auto all_Links = std::all_of(objs.cbegin(), objs.cend(), [](const tyson::TySonObject val)
            {
                return val.type() == tyson::TySonType::Link;
            });
        
            if (!all_Links)
            {
                throw std::invalid_argument(".get can only be used with a TySonObject of TySonType::Link.");
            }
        
            values_ = std::move(objs);
        }
    };


    class Find : public QueryCmd
    {
        std::vector<std::unique_ptr<Comparison>> comparators_;
        std::string annadb_query() override
        {
            std::stringstream sstream;
            sstream << "find[";
            for (auto &val : comparators_)
            {
                sstream << val->str() << ",";
            }
            sstream << "]";
            return sstream.str();
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        Find() noexcept : QueryCmd("find", true) {};
        Find(Find &&rhs) = default;

        Find& eq(tyson::TySonObject &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Eq>(value));
            return *this;
        }
    
        Find& eq(std::string_view path_to_field, tyson::TySonObject &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Eq>(path_to_field, value));
            return *this;
        }

        /**
         * Find elements by equally comparison
         *
         * @param value @see TySON.tyson::TySonObject
         * @return the Find class to add additional filter
         */
        static Find EQ(tyson::TySonObject &value) noexcept
        {
            Find find {};
            find.eq(value);
            return find;
        }
        
        Find& neq(tyson::TySonObject &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Neq>(value));
            return *this;
        }
    
        Find& neq(std::string_view path_to_field, tyson::TySonObject &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Neq>(path_to_field, value));
            return *this;
        }

        /**
         * Find elements not equally comparison
         *
         * @param value @see TySON.tyson::TySonObject
         * @return the Find class to add additional filter
         */
        static Find NEQ(tyson::TySonObject &value) noexcept
        {
            Find find {};
            find.neq(value);
            return find;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& gt(T &&value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Gt>(std::forward<tyson::TySonObject>(value)));
            return *this;
        }
        
        Find& gt(tyson::TySonObject &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Gt>(value));
            return *this;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& gt(std::string_view path_to_field, T &&value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Gt>(path_to_field, std::forward<tyson::TySonObject>(value)));
            return *this;
        }
    
        Find& gt(std::string_view path_to_field, tyson::TySonObject &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Gt>(path_to_field, value));
            return *this;
        }

        /**
         * Find elements which are greater
         *
         * @param value @see TySON.tyson::TySonObject
         * @return the Find class to add additional filter
         */
        template<std::convertible_to<tyson::TySonObject> T>
        static Find GT(T &&value) noexcept
        {
            Find find {};
            find.gt(value);
            return find;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& gte(T &&value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Gte>(std::forward<tyson::TySonObject>(value)));
            return *this;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& gte(std::string_view path_to_field, T &&value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Gte>(path_to_field, std::forward<tyson::TySonObject>(value)));
            return *this;
        }

        /**
         * Find elements which are greater or equally
         *
         * @param value @see TySON.tyson::TySonObject
         * @return the Find class to add additional filter
         */
        template<std::convertible_to<tyson::TySonObject> T>
        static Find GTE(T value) noexcept
        {
            Find find {};
            find.gte(std::forward<tyson::TySonObject>(value));
            return find;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& lt(T value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Lt>(std::forward<tyson::TySonObject>(value)));
            return *this;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& lt(std::string_view path_to_field, T value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Lt>(path_to_field, std::forward<tyson::TySonObject>(value)));
            return *this;
        }

        /**
         * Find elements which are less
         *
         * @param value @see TySON.tyson::TySonObject
         * @return the Find class to add additional filter
         */
        template<std::convertible_to<tyson::TySonObject> T>
        static Find LT(T value) noexcept
        {
            Find find {};
            find.lt(std::forward<tyson::TySonObject>(value));
            return find;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& lte(T value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Lte>(std::forward<tyson::TySonObject>(value)));
            return *this;
        }
    
        template<std::convertible_to<tyson::TySonObject> T>
        Find& lte(std::string_view path_to_field, T value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Lte>(path_to_field, std::forward<tyson::TySonObject>(value)));
            return *this;
        }

        /**
         * Find elements which are less or equally
         *
         * @param value @see TySON.tyson::TySonObject
         * @return the Find class to add additional filter
         */
        static Find LTE(tyson::TySonObject &value) noexcept
        {
            Find find {};
            find.lte(value);
            return find;
        }

        Find& q(And &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<And>(value));
            return *this;
        }

        /**
         * Find elements which meet both statements
         *
         * @param comp_1 @see query_comparison.annadb::Query::Comparison
         * @param comp_2  @see query_comparison.annadb::Query::Comparison
         * @return the Find class to add additional filter
         */
        template<std::convertible_to<Comparison> ...Comps>
        static Find AND(Comps ...comps) noexcept
        {
            And and_ {comps ...};
            Find find {};
            find.q(and_);
            return find;
        }

        Find& q(Or &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Or>(value));
            return *this;
        }

        /**
         * Find elements by or comparison
         *
         * @param comp @see query_comparison.annadb::Query::Comparison
         * @return the Find class to add additional filter
         */
        template<std::convertible_to<Comparison> ...Comps>
        static Find OR(Comps ...comps) noexcept
        {
            Or or_ {comps ...};
            Find find {};
            find.q(or_);
            return find;
        }

        Find& q(Not &value) noexcept
        {
            comparators_.emplace_back(std::make_unique<Not>(value));
            return *this;
        }

        /**
         * Exclude a specific field
         *
         * @param field
         * @return the Find class to add additional filter
         */
        static Find NOT(std::string_view field) noexcept
        {
            Not not_ {field};
            Find find {};
            find.q(not_);
            return find;
        }

    };


    class Sort : public QueryCmd
    {
        std::vector<std::unique_ptr<annadb::Query::SortCmd>> cmds_ {};

        std::string annadb_query() override
        {
            std::string query = "sort[";
            std::for_each(cmds_.begin(), cmds_.end(), [&query](std::unique_ptr<annadb::Query::SortCmd> &val)
            {
                query += val->data() + ",";
            });
            query += "]";

            return query;
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        explicit Sort(std::vector<std::unique_ptr<annadb::Query::SortCmd>> &&cmds) noexcept : QueryCmd("sort", false), cmds_(std::move(cmds)) {}
    
        template<std::convertible_to<std::string> ...T>
        static Sort ASC(T&& ...fields) noexcept
        {
            std::vector<std::unique_ptr<annadb::Query::SortCmd>> cmds {};
            cmds.reserve(sizeof...(fields));
            
            (cmds.push_back(
                    std::make_unique<Asc>(annadb::Query::Asc(fields)
                    )), ...);
        
            return Sort(std::move(cmds));
        }
    
        template<std::convertible_to<std::string> ...T>
        static Sort DESC(T&& ...fields) noexcept
        {
            std::vector<std::unique_ptr<annadb::Query::SortCmd>> cmds = {};
            (
                cmds.push_back(
                        std::make_unique<Desc>(annadb::Query::Desc(fields))
                        ), ...
            );

            return Sort(std::move(cmds));
        }
    };


    class Limit : public QueryCmd
    {
        std::string data_;
        std::string annadb_query() override
        {
            return "limit(n|" + data_ + "|)";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        Limit(const Limit &rhs) noexcept : QueryCmd("delete", true), data_(rhs.data_) {};

        explicit Limit(short data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(unsigned short data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(int data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(unsigned int data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(long data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(unsigned long data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(long long data) : QueryCmd("limit", false), data_(std::to_string(data)) {}
        explicit Limit(unsigned long long data) : QueryCmd("limit", false), data_(std::to_string(data)) {}

    };


    class Offset : public QueryCmd
    {
        std::string data_;

        std::string annadb_query() override
        {
            return "offset(n|" + data_ + "|)";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        Offset(const Offset &rhs) : QueryCmd("delete", true), data_(rhs.data_) {};

        explicit Offset(short data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(unsigned short data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(int data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(unsigned int data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(long data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(unsigned long data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(long long data) : QueryCmd("offset", false), data_(std::to_string(data)) {}
        explicit Offset(unsigned long long data) : QueryCmd("offset", false), data_(std::to_string(data)) {}

    };


    class Update : public QueryCmd
    {
        std::vector<std::tuple<UpdateType, tyson::TySonObject>> values_ {};

        std::string annadb_query() override
        {
            std::stringstream sstream;
            sstream << "update[";
            for (auto &val : values_)
            {
                auto type = std::get<0>(val);
                auto obj = std::get<1>(val);

                if (type == UpdateType::Set)
                {
                    sstream << "set{" << obj << "},";
                }
                else
                {
                    sstream << "inc{" << obj << "},";
                }
            }

            sstream << "]";

            return sstream.str();
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {};
        }

    public:

        Update(tyson::TySonObject &val, UpdateType &type) noexcept : QueryCmd("update", false)
        {
            values_.emplace_back(type, val);
        }
        Update(std::vector<tyson::TySonObject> &values, UpdateType &type) noexcept : QueryCmd("update", false)
        {
            std::for_each(values.begin(), values.end(),
                          [this, &type](auto val)
                          {
                            this->values_.emplace_back(type, val);
                          });
        }
        Update(std::vector<std::tuple<UpdateType, tyson::TySonObject>> &&values) noexcept : QueryCmd("update", false), values_(values) {}
    };


    class Delete : public QueryCmd
    {
        std::string annadb_query() override
        {
            return "delete";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {};
        }

    public:
        Delete() = default;
        Delete(const Delete &) : QueryCmd("delete", false) {};
    };
    
    class Project : public QueryCmd
    {
        std::vector<std::pair<std::string, tyson::TySonObject>> values_;
    
        std::string annadb_query() override
        {
            std::stringstream sstream;
            sstream << "project{";
            std::for_each(values_.begin(), values_.end(),
                          [&sstream](auto &val)
                          {
                            sstream << "s|" <<std::get<0>(val) << "|:" << std::get<1>(val) << ",";
                          });
            sstream << "}";
            return sstream.str();
        }
        
        [[nodiscard]] std::vector<std::string> previous_steps_() noexcept override
        {
            return { "get", "find", "sort", "limit", "offset"};
        };
        
        [[nodiscard]] std::vector<std::string> next_steps_() noexcept override
        {
            return {};
        }
        
    public:

        template<std::convertible_to<std::pair<std::string, tyson::TySonObject>> ...T>
        explicit Project(T && ... objs)
        {
            values_.reserve(sizeof...(objs));
            (values_.emplace_back(objs), ...);
        }
    };


    class Query
    {
        std::string collection_name_;
        std::vector<std::unique_ptr<annadb::Query::QueryCmd>> cmds_ {};
        void add_to_cmds(std::unique_ptr<QueryCmd> queryCmd)
        {
            if (cmds_.empty() && !queryCmd->can_start_pipeline())
            {
                throw std::invalid_argument(queryCmd->name() + " can not be used to start a new query pipeline.");
            }

            if (cmds_.empty())
            {
                this->cmds_.emplace_back(std::move(queryCmd));
            }
            else
            {
                auto latest_cmd = cmds_.size() - 1;
                if (cmds_.at(latest_cmd)->next_step_allowed(queryCmd->name()) &&
                    queryCmd->previous_step_allowed(cmds_.at(latest_cmd)->name()))
                {
                    this->cmds_.emplace_back(std::move(queryCmd));
                }
                else
                {
                    throw std::invalid_argument(queryCmd->name() + " can not be used before/after " + cmds_.at(latest_cmd)->name());
                }
            }
        }

        friend std::ostream& operator<<(std::ostream &out, Query &query) noexcept
        {
            std::stringstream sstream;
            sstream << "collection|" << query.collection_name_ << "|";

            if (query.cmds_.size() == 1)
            {
                sstream << ":" << query.cmds_[0]->query() << ";";
            }
            else
            {
                sstream << ":q[";
                for (auto &cmd: query.cmds_)
                {
                    sstream << cmd->query() << ",";
                }
                sstream << "];";
            }
            return out << sstream.str();
        }

    public:
        /**
         * The class to create your queries
         *
         * @param collection_name
         */
        explicit Query(std::string collection_name) : collection_name_(std::move(collection_name)) {};
        ~Query() = default;

        /**
         * Create Insert statement
         *
         * @param insert @see query.annadb::Query::Insert
         */
        void insert(Insert &insert) noexcept
        {
            this->add_to_cmds(std::make_unique<Insert>(std::move(insert)));
        }

        /**
         * Create Insert statement
         *
         * @param insert @see TySON.tyson::TySonObject
         */
        void insert(tyson::TySonObject &insert) noexcept
        {
            this->add_to_cmds(std::make_unique<Insert>(insert));
        }

        /**
         * Create Insert statement
         *
         * @param insert
         */
        void insert(std::vector<tyson::TySonObject> &insert) noexcept
        {
            this->add_to_cmds(std::make_unique<Insert>(insert));
        }
    
        /**
         * Create Insert statement
         *
         * @param insert
         */
        template<typename ...T>
        void insert(T&& ...insert) noexcept
        {
            this->add_to_cmds(std::make_unique<Insert>(insert...));
        }

        /**
         * Create Get statement
         *
         * @param get @see query.annadb::Query::Get
         * @return the query class to add additional statements
         */
        Query& get(Get &get) noexcept
        {
            this->add_to_cmds(std::make_unique<Get>(std::move(get)));
            return *this;
        }

        /**
         * Create Get statement
         *
         * @param get @see TySON.tyson::TySonObject
         * @return the query class to add additional statements
         */
        Query& get(tyson::TySonObject &get) noexcept
        {
            this->add_to_cmds(std::make_unique<Get>(get));
            return *this;
        }

        /**
        * Create Get statement
        *
        * @param get
        * @return the query class to add additional statements
        */
        Query& get(std::vector<tyson::TySonObject> &get) noexcept
        {
            this->add_to_cmds(std::make_unique<Get>(get));
            return *this;
        }
    
        /**
         * Create Get statement
         *
         * @param values
         * @return the query class to add additional statements
         */
        template<std::convertible_to<tyson::TySonObject> ...T>
        Query& get(T &&...values) noexcept
        {
            this->add_to_cmds(std::make_unique<Get>(values...));
            return *this;
        }

        /**
         * Create Find statement
         *
         * @param find @see query.annadb::Query::Find
         * @return the query class to add additional statements
         */
        Query& find(Find &&find) noexcept
        {
            this->add_to_cmds(std::make_unique<Find>(std::move(find)));
            return *this;
        }
    
        /**
         * Create Find statement
         *
         * @param find @see query.annadb::Query::Find
         * @return the query class to add additional statements
         */
        Query& sort(Sort &&sort) noexcept
        {
            this->add_to_cmds(std::make_unique<Sort>(std::move(sort)));
            return *this;
        }

        /**
         * Create Limit statement
         *
         * @param limit @see query.annadb::Query::Limit
         * @return the query class to add additional statements
         */
        Query& limit(Limit &limit) noexcept
        {
            this->add_to_cmds(std::make_unique<Limit>(std::move(limit)));
            return *this;
        }

        /**
         * Create Limit statement
         *
         * @tparam T integral
         * @param limit the amount of values in the result
         * @return the query class to add additional statements
         */
        template<typename T>
        requires std::is_integral_v<T>
        Query& limit(T &&limit) noexcept
        {
            this->add_to_cmds(std::make_unique<Limit>(limit));
            return *this;
        }

        /**
         * Create Offset statement
         *
         * @param offset @see query.annadb::Query::Offset
         * @return the query class to add additional statements
         */
        Query& offset(Offset &offset) noexcept
        {
            this->add_to_cmds(std::make_unique<Offset>(offset));
            return *this;
        }

        /**
         * Create Offset statement
         *
         * @tparam T integral
         * @param offset the amount of values you skip in the result
         * @return the query class to add additional statements
         */
        template<typename T>
        requires std::is_integral_v<T>
        Query& offset(T &&offset) noexcept
        {
            this->add_to_cmds(std::make_unique<Offset>(offset));
            return *this;
        }

        /**
         * Create Update statement
         *
         * @param kind @see query.annadb::Query::UpdateType
         * @param value must be of type TySonType::Value @see TySON.tyson::TySonObject
         */
        void update(UpdateType &&kind, tyson::TySonObject &value)
        {
            if (value.type() == tyson::TySonType::Value)
            {
                this->add_to_cmds(std::make_unique<Update>(value, kind));
            }
            else
            {
                throw std::invalid_argument("Only `tyson::TySonType::Value` are allowed.");
            }
        }

        /**
         * Create Update statement
         *
         * @param kind @see query.annadb::Query::UpdateType
         * @param values must contain only types of TySonType::Value @see TySON.tyson::TySonObject
         */
        void update(UpdateType &kind, std::vector<tyson::TySonObject> &values)
        {
            if (std::all_of(values.begin(), values.end(),
                            [](const auto &val){ return val.type() == tyson::TySonType::Value;}))
            {
                this->add_to_cmds(std::make_unique<Update>(values, kind));
            }
            else
            {
                throw std::invalid_argument("Only `tyson::TySonType::Values` are allowed.");
            }
        }
    
        template<std::convertible_to<tyson::TySonObject> ...T>
        void update(UpdateType &kind, T&& ...values)
        {
            std::vector<tyson::TySonObject> objects {};
            objects.reserve(sizeof...(values));
            (objects.emplace_back(values), ...);
            
            if (std::all_of(objects.begin(), objects.end(),
                            [](const auto &val){ return val.type() == tyson::TySonType::Value;}))
            {
                this->add_to_cmds(std::make_unique<Update>(objects, kind));
            }
            else
            {
                throw std::invalid_argument("Only `tyson::TySonType::Values` are allowed.");
            }
        }

        /**
         * Create Update statement
         *
         * @param values can be a vector of mixed UpdateType with the corresponding value
         * @see query.annadb::Query::UpdateType
         * @see TySON.tyson::TySonObject
         */
        void update(std::vector<std::tuple<UpdateType, tyson::TySonObject>> &values)
        {
            if (std::all_of(values.begin(), values.end(),
                            [](const auto &val){ return std::get<1>(val).type() == tyson::TySonType::Value;}))
            {
                this->add_to_cmds(std::make_unique<Update>(std::move(values)));
            }
            else
            {
                throw std::invalid_argument("Only `tyson::TySonType::Values` are allowed.");
            }
        }

        /**
         * Create Delete statement
         *
         */
        void delete_q() noexcept
        {
            this->add_to_cmds(std::make_unique<Delete>(Delete()));
        }
    
        /**
         * Create Project statement
         *
         * @param values must be a pair of std::string, tyson::TySonObject
         * @see TySON.tyson::TySonObject
         * @return the query class to add additional statements
         */
        template<std::convertible_to<std::pair<std::string, tyson::TySonObject>> ...T>
        Query& project(T &&...values) noexcept
        {
            this->add_to_cmds(std::make_unique<Project>(values...));
            return *this;
        }
    };
}

#endif //ANNADB_DRIVER_QUERY_HPP

//
// Created by felix on 16.02.23.
//

#ifndef ANNADB_DRIVER_QUERY_HPP
#define ANNADB_DRIVER_QUERY_HPP

#include <string>
#include <stdarg.h>
#include "TySON.hpp"

namespace annadb::Query
{

    struct SortCmd
    {
        virtual ~SortCmd() = default;
        virtual std::string data() const = 0;
    };

    class Asc : public SortCmd
    {
        std::string field_;

    public:
        explicit Asc(std::string_view field) : field_(field) {}

        [[nodiscard]] std::string data() const override
        {
            return "asc(value|" + field_ + "|)";
        }
    };

    class Desc : public SortCmd
    {
        std::string field_;

    public:
        explicit Desc(std::string_view field) : field_(field) {}

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

        QueryCmd(QueryCmd &&rhs) = default;

        [[nodiscard]] std::string name()
        {
            return this->name_;
        }

        [[nodiscard]] bool can_start_pipeline() const
        {
            return start_cmd_;
        }

        [[nodiscard]] virtual std::string query()
        {
            return this->annadb_query();
        }

        [[nodiscard]] bool next_step_allowed(const std::string &cmdName)
        {
            if (next_steps_().empty())
            {
                return false;
            }

            auto res = std::find(next_steps_().begin(), next_steps_().end(), cmdName);
            return res != next_steps_().end();
        }

        [[nodiscard]] bool previous_step_allowed(const std::string &cmdName)
        {
            if (previous_steps_().empty())
            {
                return false;
            }

            auto res = std::find(next_steps_().begin(), next_steps_().end(), cmdName);
            return res != next_steps_().end();
        }


    };


    class Insert : public QueryCmd
    {
        std::vector<std::unique_ptr<tyson::TySonObject>> values;

        std::string annadb_query() override
        {
            return "";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return {};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {};
        }

    public:
        Insert() : QueryCmd("insert", true) {}
        Insert(const Insert &) : QueryCmd("insert", true) {}
    };


    class Get : public QueryCmd
    {
        std::string annadb_query() override
        {
            return "";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        Get() : QueryCmd("find", true) {}
        Get(const Get &) : QueryCmd("find", true) {}

    };


    class Find : public QueryCmd
    {
        std::string annadb_query() override
        {
            return "";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        Find(const Find &) : QueryCmd("find", true) {};

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

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        explicit Sort(std::vector<std::unique_ptr<annadb::Query::SortCmd>> &&cmds) : QueryCmd("sort", false), cmds_(std::move(cmds)) {}

    };


    class Limit : public QueryCmd
    {
        std::string data_;
        std::string annadb_query() override
        {
            return "limit(n|" + data_ + "|)";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {"find", "get", "sort", "limit", "offset", "update", "delete"};
        }

    public:
        Limit(const Limit &rhs) : QueryCmd("delete", true), data_(rhs.data_) {};

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

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
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
        std::string annadb_query() override
        {
            return "";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {};
        }

    public:
        Update(const Update &) : QueryCmd("update", true) {};
    };


    class Delete : public QueryCmd
    {
        std::string annadb_query() override
        {
            return "";
        }

        [[nodiscard]] std::vector<std::string> previous_steps_() override
        {
            return { "find", "get", "sort", "limit", "offset"};
        };
        [[nodiscard]] std::vector<std::string> next_steps_() override
        {
            return {};
        }

    public:
        Delete(const Delete &) : QueryCmd("delete", true) {};
    };


    class Query
    {
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

    public:
        Query() = default;
        ~Query() = default;

        Query& insert(Insert &insert)
        {
            this->add_to_cmds(std::make_unique<Insert>(insert));
            return *this;
        }
        Query& find(Find &find)
        {
            this->add_to_cmds(std::make_unique<Find>(find));
            return *this;
        }
        Query& get(Get &get)
        {
            this->add_to_cmds(std::make_unique<Get>(get));
            return *this;
        }
        Query& limit(Limit &limit)
        {
            this->add_to_cmds(std::make_unique<Limit>(limit));
            return *this;
        }
        Query& offset(Offset &offset)
        {
            this->add_to_cmds(std::make_unique<Offset>(offset));
            return *this;
        }
        Query& update(Update &update)
        {
            this->add_to_cmds(std::make_unique<Update>(update));
            return *this;
        }
        Query& remove(Delete &del)
        {
            this->add_to_cmds(std::make_unique<Delete>(del));
            return *this;
        }
    };
}

#endif //ANNADB_DRIVER_QUERY_HPP

#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <vector>
#include <optional>
#include "Poco/JSON/Object.h"
#include "Poco/DateTime.h"

namespace database
{
    class Service
    {
      private:
        long _id;
        std::string _name;
        std::string _type;
        long _implementer_id;
        std::string _description;
        Poco::DateTime _date;

      public:

        static Service fromJSON(const std::string &str);

        long get_id() const;
        const std::string &get_name() const;
        const std::string &get_type() const;
        long get_implementer_id() const;
        const std::string &get_description() const;
        Poco::DateTime get_date() const;

        long& id();
        std::string &name();
        std::string &type();
        long& implementer_id();
        std::string &description();
        Poco::DateTime &date();

        static void init();
        void save_to_mysql();
        static std::optional<Service>  read_by_id(long id);
        static std::vector<Service> read_all();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif // SERVICE_H
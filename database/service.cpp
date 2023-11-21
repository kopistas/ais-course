#include "service.h"
#include "database.h"
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/Statement.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    // ... Other method implementations ...

    void Service::init()
    {
        
    }

    void Service::save_to_mysql()
    {
        
    }

    std::optional<Service> Service::read_by_id(long id)
    {
        std::optional<Service> service;
        printf("%ld", id);
        return service;
        
    }

    std::vector<Service> Service::read_all()
    {
        std::vector<Service> services;
        return services;
    }

    Poco::JSON::Object::Ptr Service::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("name", _name);
        root->set("type", _type);
        root->set("implementer_id", _implementer_id);
        root->set("description", _description);
        root->set("date", Poco::DateTimeFormatter::format(_date, Poco::DateTimeFormat::ISO8601_FORMAT)); // Format the Poco::DateTime to a string

        return root;
    }

    Service Service::fromJSON(const std::string &str)
    {
        printf("%s", str.c_str());
        Service service;
        return service;
    }
}

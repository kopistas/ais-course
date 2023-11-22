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
    void Service::init()
    {
        // Does nothing on init
    }

    void Service::save_to_mysql()
    {
           try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Service (name,type,implementer_id,description,date) VALUES(?, ?, ?, ?, ?)",
                use(_name),
                use(_type),
                use(_implementer_id),
                use(_description),
                use(_date);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::optional<Service> Service::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Service service;

            select << "SELECT id, name, type, implementer_id, description, date FROM Service WHERE id=?",
                into(service._id),
                into(service._name),
                into(service._type),
                into(service._implementer_id),
                into(service._description),
                into(service._date),
                use(id),
                range(0, 1); 

            if (select.execute() > 0)
            {
                return service;
            }
        }
        catch (const Poco::Exception& ex)
        {
            std::cerr << "Service::read_by_id failed: " << ex.displayText() << std::endl;
        }
        return {};        
    }

    std::vector<Service> Service::read_all()
    {
        std::vector<Service> services;
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);

            Service service;
            select << "SELECT id, name, type, implementer_id, description, date FROM Service",
            into(service._id),
            into(service._name),
            into(service._type),
            into(service._implementer_id),
            into(service._description),
            into(service._date),
            range(0, 1); // iterate over result set one row at a time

            while (!select.done()) {
                select.execute();
                services.push_back(service);
            }
        }       
        catch (const Poco::Exception& ex)
        {
            std::cerr << "Service::read_all failed: " << ex.displayText() << std::endl;
        }
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

        // Getters
    long Service::get_id() const {
        return _id;
    }

    const std::string& Service::get_name() const {
        return _name;
    }

    const std::string& Service::get_type() const {
        return _type;
    }

    long Service::get_implementer_id() const {
        return _implementer_id;
    }

    const std::string& Service::get_description() const {
        return _description;
    }

    Poco::DateTime Service::get_date() const {
        return _date;
    }

    // Setters (non-const reference return type allows for direct modification)
    long& Service::id() {
        return _id;
    }

    std::string& Service::name() {
        return _name;
    }

    std::string& Service::type() {
        return _type;
    }

    long& Service::implementer_id() {
        return _implementer_id;
    }

    std::string& Service::description() {
        return _description;
    }

    Poco::DateTime& Service::date() {
        return _date;
    }

}

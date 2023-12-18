#include "user.h"
#include "database.h"
#include "../config/config.h"
#include "../database/cache.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void User::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`first_name` VARCHAR(256) NOT NULL,"
                        << "`last_name` VARCHAR(256) NOT NULL,"
                        << "`login` VARCHAR(256) NOT NULL,"
                        << "`password` VARCHAR(256) NOT NULL,"
                        << "`email` VARCHAR(256) NULL,"
                        << "`title` VARCHAR(1024) NULL,"
                        << "PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));",
                now;
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

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("title", _title);
        root->set("login", _login);
        root->set("password", _password);
        root->set("role", _role);

        return root;
    }

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.id() = object->getValue<long>("id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.title() = object->getValue<std::string>("title");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");
        user.role() = object->getValue<std::string>("role");

        return user;
    }

    std::optional<long> User::auth(std::string &login, std::string &password)
    {
        std::optional<long> id {};
        try
        {
            for (const auto& shard : database::Database::get().get_all_hints()) {
                Poco::Data::Session session = database::Database::get().create_session();
                Poco::Data::Statement select(session);

                select << "SELECT id FROM User where login=? and password=? " + shard,
                    into(id.value()),
                    use(login),
                    use(password),
                    range(0, 1); //  iterate over result set one row at a time

                select.execute();
                Poco::Data::RecordSet rs(select);
                if (rs.moveFirst()) return id;
            }
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::optional<User> User::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);

            std::string sharding_hint = database::Database::sharding_user(id);

            std::string query_string = "SELECT id, first_name, last_name, email, title,login,password,role FROM User where id=?";
            query_string += sharding_hint;
            std::cout << query_string << std::endl;

            User a;
            select << query_string,
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                into(a._login),
                into(a._password),
                into(a._role),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }


    std::vector<User> User::read_all()
    {
        std::vector<User> result;
        try
        {
            for(const auto& shard : database::Database::get().get_all_hints()) {
                Poco::Data::Session session = database::Database::get().create_session();
                Statement select(session);
                User a;

                select << "SELECT id, first_name, last_name, email, title, login, password, role FROM User " + shard,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    into(a._role),
                    range(0, 1); //  iterate over result set one row at a time

                while(!select.done())
                {
                    if(select.execute())
                        result.push_back(a);
                }
            }
            return result;

        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<User> User::search(std::string first_name, std::string last_name)
    {
        std::vector<User> result;
        try
        {
            for (const auto& shard : database::Database::get().get_all_hints()) {
                Poco::Data::Session session = database::Database::get().create_session();
                Statement select(session);
                User a;
                first_name += "%";
                last_name += "%";

                select << "SELECT id, first_name, last_name, email, title, login, password, role FROM User where first_name LIKE ? and last_name LIKE ? " + shard,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    into(a._role),
                    use(first_name),
                    use(last_name),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
            }
            return result;

        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        } catch (const std::exception& ex) {
            std::cout << "Generic error: " << ex.what() << std::endl;
            throw;
        }
    }

    void User::save_to_mysql()
    {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement count(session);

            long total_count = 0;
            for(const auto& shard : database::Database::get().get_all_hints()) {
                long count;
                Poco::Data::Statement countStatement(session);
                countStatement << "SELECT COUNT(*) FROM User " + shard,
                            into(count), now;
                countStatement.execute();
                total_count += count;
            }

            _id = total_count;

            std::string sharding_hint = database::Database::sharding_user(_id);

            std::cout << "For id " << _id << "Will use " << sharding_hint << std::endl;

            Poco::Data::Statement insert(session);
            std::string insert_str = "INSERT INTO User (id, first_name, last_name, email, title, login, password, role) VALUES(?, ?, ?, ?, ?, ?, ?, ?) " + sharding_hint;
            insert << insert_str,
                use(_id),
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_title),
                use(_login),
                use(_password),
                use(_role),
                now;
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
        catch (...) {
            std::cout << "Generic error" << std::endl;
        }
    }

    std::optional<User> User::read_from_cache_by_id(long id)
    {
        try
        {
            std::string result;
            if (database::Cache::get().get(id, result))
                return fromJSON(result);
            else
                return std::optional<User>();
        }
        catch (std::exception& err)
        {
           // std::cerr << "error:" << err.what() << std::endl;
            return std::optional<User>();
        }
    }

     void User::save_to_cache()
    {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(_id, message);
    }

    const std::string &User::get_login() const
    {
        return _login;
    }

    const std::string &User::get_password() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    const std::string &User::get_first_name() const
    {
        return _first_name;
    }

    const std::string &User::get_last_name() const
    {
        return _last_name;
    }

    const std::string &User::get_email() const
    {
        return _email;
    }

    const std::string &User::get_title() const
    {
        return _title;
    }

    const std::string &User::get_role() const
    {
        return _role;
    }

    long &User::id()
    {
        return _id;
    }

    std::string &User::first_name()
    {
        return _first_name;
    }

    std::string &User::last_name()
    {
        return _last_name;
    }

    std::string &User::email()
    {
        return _email;
    }

    std::string &User::title()
    {
        return _title;
    }

    std::string &User::role()
    {
        return _role;
    }
}
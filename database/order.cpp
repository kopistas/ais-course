#include "order.h"
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
    // Constructor and other necessary member functions can be added here

    // Order Order::create(long consumer_id)
    // {
    //     // Implementation of creating an order in the database
    //     // and returning its identifier.
    //     // ...
    // }

    // void Order::add_service(long service_id, long order_id)
    // {
    //     // Implementation of adding a service to the order
    //     // in the database.
    //     // ...
    // }

    std::vector<Order> Order::get_orders_for_consumer(long consumer_id)
    {
        // Implementation of getting all orders for a given consumer
        // from the database.
        std::vector<Order> orders;
        try {
            Session session = Database::get().create_session();
            Statement select(session);

            Order order;
            select << "SELECT id, consumer_id, date_of_creation, deadline_date FROM Orders WHERE consumer_id=?",
                into(order._id),
                into(order._consumer_id),
                into(order._date_of_creation),
                into(order._deadline_date),
                use(consumer_id),
                range(0, 1); // iterate over result set one row at a time

            while (!select.done()) {
                select.execute();
                orders.push_back(order);
            }
        }
        catch (const Poco::Exception& ex)
        {
            std::cerr << "Order::get_orders_for_consumer failed: " << ex.displayText() << std::endl;
        }
        return orders;
    }

    std::optional<Order> Order::read_by_id(long id) 
    {
        try {
            Session session = Database::get().create_session();
            Statement select(session);
            Order order;

            select << "SELECT id, consumer_id, date_of_creation, deadline_date FROM Orders WHERE id=?",
                into(order._id),
                into(order._consumer_id),
                into(order._date_of_creation),
                into(order._deadline_date),
                use(id),
                range(0, 1); // Limit to 1 row
            if (select.execute() > 0) {
                order.populate_services();
                return order;
            }
        } catch (const Poco::Exception& ex) {
            std::cerr << "Order::read_by_id failed: " << ex.displayText() << std::endl;
        }
        return std::nullopt;
    }

    void Order::populate_services()
    {
        // TODO: Implement
    }

    // static Order fromJSON(const std::string &str) {
    //     // TODO: Implement
    // }

    // Poco::JSON::Object::Ptr toJSON() const {
    //     // TODO: Implement
    // }

    // Getters
    long Order::get_id() const {
        return _id;
    }

    long Order::get_consumer_id() const {
        return _consumer_id;
    }

    const std::vector<long>& Order::get_list_of_services_ids() const {
        return _list_of_services_ids;
    }

    Poco::DateTime Order::get_date_of_creation() const {
        return _date_of_creation;
    }

    Poco::DateTime Order::get_deadline_date() const {
        return _deadline_date;
    }

    // Setters
    long& Order::id() {
        return _id;
    }

    long& Order::consumer_id() {
        return _consumer_id;
    }

    std::vector<long>& Order::list_of_services_ids() {
        return _list_of_services_ids;
    }

    Poco::DateTime &Order::date_of_creation() {
        return _date_of_creation;
    }

    Poco::DateTime &Order::deadline_date() {
        return _deadline_date;
    }
}

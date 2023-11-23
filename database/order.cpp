#include "order.h"
#include "database.h"
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/Statement.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/URI.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Parser.h>


using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    // Constructor and other necessary member functions can be added here
    Order Order::create(long consumer_id)
    {
        Order newOrder;
        Session session = Database::get().create_session();
        // Create a session to interact with the database

        try {
            // Begin a transaction to ensure data integrity
            session.begin();

            // Prepare a statement to insert a new record into the Order table
            Statement insert(session);
            insert << "INSERT INTO `Order` (consumer_id, date_of_creation, deadline_date) VALUES (?, NOW(), DATE_ADD(NOW(), INTERVAL 7 DAY))",
                use(consumer_id),  // Bind the consumer_id parameter
                now;               // Execute the statement now

            // Retrieve the generated id for the new order
            Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(newOrder._id),
                now;

            newOrder._consumer_id = consumer_id;
            newOrder._date_of_creation = Poco::DateTime(); 
            newOrder._deadline_date = Poco::DateTime(); 

            // Commit the transaction to finalize the changes
            session.commit();
        } catch (const Poco::Exception& ex) {
            // If an exception occurs, output the error and roll back the transaction
            std::cerr << "Order::create failed: " << ex.displayText() << std::endl;
            session.rollback();
            // Depending on your error handling strategy, you may throw an exception or handle it accordingly
        }
        return newOrder;

    }

    void Order::add_service(long service_id, long order_id) 
    {
        Session session = Database::get().create_session();
        try {
            // Create a session to interact with the database
            
            // Begin a transaction to ensure data integrity
            session.begin();

            // Prepare a statement to insert a new record into the Order_Service table
            Statement insert(session);
            insert << "INSERT INTO Order_Service (order_id, service_id) VALUES (?, ?)",
                use(order_id),    // Bind the order_id parameter
                use(service_id),  // Bind the service_id parameter
                now;              // Execute the statement now

            // Commit the transaction to finalize the changes
            session.commit();

            // Optionally, you could update the _list_of_services_ids vector, if needed
            _list_of_services_ids.push_back(service_id);

        } catch (const Poco::Exception& ex) {
            // If an exception occurs, output the error and roll back the transaction
            std::cerr << "Order::add_service failed: " << ex.displayText() << std::endl;
            session.rollback();
        }
    }

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

    void Order::populate_services() {
        try {
            Session session = Database::get().create_session();
            Statement select(session);

            // Temporary vector to hold associated service IDs
            std::vector<long> serviceIds;

            // Query the Order_Service table to find all service IDs related to this order
            select << "SELECT service_id FROM Order_Service WHERE order_id=?",
                into(serviceIds),
                use(_id); // Using the Order's _id field

            // Execute the query
            select.execute();

            // Clear existing service IDs and repopulate
            _list_of_services_ids.clear();
            _list_of_services_ids.assign(serviceIds.begin(), serviceIds.end());

        } catch (const Poco::Exception& ex) {
            std::cerr << "Order::populate_services failed: " << ex.displayText() << std::endl;
        }
    }

    Order Order::fromJSON(const std::string &str)
    {
        Order order;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        // Assigning parsed values to Order's properties using the accessors
        order.id() = object->getValue<long>("id");
        order.consumer_id() = object->getValue<long>("consumer_id");

        // Convert UNIX timestamp back to Poco::DateTime
        Poco::Timestamp dateOfCreationTimestamp(object->getValue<Poco::Int64>("date_of_creation") * Poco::Timestamp::resolution());
        order.date_of_creation() = Poco::DateTime(dateOfCreationTimestamp);

        Poco::Timestamp deadlineDateTimestamp(object->getValue<Poco::Int64>("deadline_date") * Poco::Timestamp::resolution());
        order.deadline_date() = Poco::DateTime(deadlineDateTimestamp);

        // Convert JSON array of strings back to std::vector<long>
        Poco::JSON::Array::Ptr servicesArray = object->getArray("list_of_services_ids");
        std::vector<long>& listOfServicesIds = order.list_of_services_ids();
        listOfServicesIds.clear(); // Clear existing data if necessary
        if (servicesArray) {
            for (size_t i = 0; i < servicesArray->size(); ++i) {
                listOfServicesIds.push_back(servicesArray->getElement<long>(i));
            }
        }

        return order;
    }



    Poco::JSON::Object::Ptr Order::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("consumer_id", _consumer_id);

        // Convert Poco::DateTime to UNIX timestamp (seconds since epoch)
        Poco::Timestamp dateOfCreationTimestamp = _date_of_creation.timestamp();
        root->set("date_of_creation", dateOfCreationTimestamp.epochTime());

        Poco::Timestamp deadlineDateTimestamp = _deadline_date.timestamp();
        root->set("deadline_date", deadlineDateTimestamp.epochTime());
        
        // Convert list of service IDs to a JSON array of strings
        Poco::JSON::Array::Ptr servicesArray = new Poco::JSON::Array();
        for (const auto& serviceId : _list_of_services_ids) {
            servicesArray->add(serviceId);
        }
        root->set("list_of_services_ids", servicesArray);

        return root;
    }

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

    // Properties
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

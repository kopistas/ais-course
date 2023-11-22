#ifndef DATABASE_ORDER_H
#define DATABASE_ORDER_H

#include <string>
#include <vector>
#include <optional>
#include <Poco/DateTime.h>
#include <Poco/JSON/Object.h>

namespace database {

    class Order {
    private:
        long _id; // Unique identifier for the order
        long _consumer_id; // Identifier for the consumer who placed the order
        std::vector<long> _list_of_services_ids; // List of service identifiers included in the order
        Poco::DateTime _date_of_creation; // Date and time when the order was created
        Poco::DateTime _deadline_date; // Deadline date for the order

    public:

        // API Methods
        static Order create(long consumer_id); // Create an order and return its identifier
        void add_service(long service_id, long order_id); // Add a service to the order
        static std::vector<Order> get_orders_for_consumer(long consumer_id); // Get all orders for a given consumer

        // Getters
        long get_id() const;
        long get_consumer_id() const;
        const std::vector<long>& get_list_of_services_ids() const;
        Poco::DateTime get_date_of_creation() const;
        Poco::DateTime get_deadline_date() const;

        // Setters
        long& id();
        long& consumer_id();
        std::vector<long>& list_of_services_ids();
        Poco::DateTime &date_of_creation();
        Poco::DateTime &deadline_date();

        // Serialization and Deserialization
        static Order fromJSON(const std::string &str);
        Poco::JSON::Object::Ptr toJSON() const;

        void populate_services();
        // Database Interaction
        void save_to_mysql(); // Method to save the order to MySQL database
        static std::optional<Order> read_by_id(long id); // Read an order by its ID from the database
        static std::vector<Order> read_all(); // Read all orders from the database
    };

} // namespace database

#endif // DATABASE_ORDER_H

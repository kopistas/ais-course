#ifndef ORDER_HANDLER_H
#define ORDER_HANDLER_H


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../helper.h"
#include "../../database/order.h"

class OrderHandler : public HTTPRequestHandler
{
private:
    std::string _format;

public:
    OrderHandler(const std::string& format) : _format(format) {}

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        HTMLForm form(request, request.stream());
        try
        {
             if (hasSubstr(request.getURI(), "/orders/create") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)) {

                long consumer_id = std::stol(form.get("consumer_id", "0"));
                database::Order newOrder = database::Order::create(consumer_id);

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                response.setContentType("application/json");
                std::ostream& ostr = response.send();
                Poco::JSON::Object::Ptr root = newOrder.toJSON();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;

             } else if (hasSubstr(request.getURI(), "/orders/add") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)) {
                
                long order_id = std::stol(form.get("order_id", "0"));
                long service_id = std::stol(form.get("service_id", "0"));

                std::optional<database::Order> order = database::Order::read_by_id(order_id);
                
                if (order.has_value()) {
                    auto orderValue = order.value();

                    orderValue.add_service(service_id, order_id);

                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                    response.setContentType("application/json");
                    std::ostream& ostr = response.send();
                    Poco::JSON::Object::Ptr root = orderValue.toJSON();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                } else {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setContentType("application/json");
                    std::ostream& ostr = response.send();
                    Poco::JSON::Object errorObj;
                    errorObj.set("error", "Order not found with the provided ID.");
                    Poco::JSON::Stringifier::stringify(errorObj, ostr);
                    return;
                }

             } else if (hasSubstr(request.getURI(), "/orders/search") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)) {
                
                long order_id = std::stol(form.get("order_id", "0"));

                std::optional<database::Order> order = database::Order::read_by_id(order_id);
                
                if (order.has_value()) {
                    auto orderValue = order.value();
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                    response.setContentType("application/json");
                    std::ostream& ostr = response.send();
                    Poco::JSON::Object::Ptr root = orderValue.toJSON();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                } else {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setContentType("application/json");
                    std::ostream& ostr = response.send();
                    Poco::JSON::Object errorObj;
                    errorObj.set("error", "Order not found with the provided ID.");
                    Poco::JSON::Stringifier::stringify(errorObj, ostr);
                    return;
                }
             }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request ot found");
        root->set("instance", "/user");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }
};

#endif // ORDER_HANDLER_H

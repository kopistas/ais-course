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
             if (hasSubstr(request.getURI(), "/orders/make") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)) {

                database::Service newService;
                newService.name() = form.get("name", "");
                newService.type() = form.get("type", "");
                newService.description() = form.get("description", "");

                std::string implementer_id_str = form.get("implementer_id", "");

                // Convert the string to a long                
                if (!implementer_id_str.empty()) {
                    newService.implementer_id() = std::stol(implementer_id_str);
                }

                std::string timestamp_str = form.get("date", "");
                if (!timestamp_str.empty()) {
                    
                        // Convert string to long to get the timestamp
                    long timestamp = std::stol(timestamp_str);

                        // Convert Unix timestamp to Poco::Timestamp
                    Poco::Timestamp pocoTimestamp;
                    pocoTimestamp = timestamp * Poco::Timestamp::resolution(); // Convert seconds to microseconds
                        
                        // Create a DateTime from the Timestamp
                    Poco::DateTime date(pocoTimestamp);
                        
                        // Set the service date
                    newService.date() = date;
                }

                // ... Populate other service fields and save to database

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                response.setContentType("application/json");
                std::ostream& ostr = response.send();
                Poco::JSON::Object::Ptr root = newService.toJSON();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;

             } else if (hasSubstr(request.getURI(), "/orders/add")) {
                std::vector<database::Service> servicesList = database::Service::read_all();
                Poco::JSON::Array jsonServices;
                for (const auto& service : servicesList) {
                    jsonServices.add(service.toJSON());
                }

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                response.setContentType("application/json");
                std::ostream& ostr = response.send();
                Poco::JSON::Stringifier::stringify(jsonServices, ostr);
                return;
             } else if (hasSubstr(request.getURI(), "/orders/search")) {

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

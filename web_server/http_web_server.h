#ifndef HTTPWEBSERVER_H
#define HTTPWEBSERVER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
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

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "service_request_factory.h"
#include "user_request_factory.h"
#include "order_request_factory.h"

#include "http_request_factory.h"
#include "../database/user.h"

class HTTPWebServer : public Poco::Util::ServerApplication
{
private:
    std::unique_ptr<HTTPRequestHandlerFactory> createFactory(const std::string& type)
    {
        std::string format = "SomeFormatString";

        if (type == "service")
        {
            return std::make_unique<ServiceRequestFactory>(format);
        }
        else if (type == "user")
        {
            return std::make_unique<UserRequestFactory>(format);
        }
        else if (type == "order")
        {
            return std::make_unique<OrderRequestFactory>(format);
        }
        else
        {
            throw Poco::InvalidArgumentException("Invalid argument for request factory type");
        }
    }

public:
    int main(const std::vector<std::string> &args)
    {
        if (args.empty())
        {
            std::cout << "Usage: <program> [service|user|order]" << std::endl;
            return Application::EXIT_USAGE;
        }

        try
        {
            database::User::init();
            std::unique_ptr<HTTPRequestHandlerFactory> requestFactory = createFactory(args[0]);
            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8080));
            HTTPServer srv(requestFactory.release(), svs, new HTTPServerParams);
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        }
        catch (const Poco::Exception& exc)
        {
            std::cerr << exc.displayText() << std::endl;
            return Application::EXIT_SOFTWARE;
        }

        return Application::EXIT_OK;
    }
};
#endif // !HTTPWEBSERVER

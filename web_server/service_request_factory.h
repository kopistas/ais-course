
#ifndef SERVICE_REQUEST_FACTORY_H
#define SERVICE_REQUEST_FACTORY_H

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "handlers/service_handler.h" 
#include <iostream>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;

class ServiceRequestFactory : public HTTPRequestHandlerFactory
{
public:
    ServiceRequestFactory(const std::string& format) : _format(format) {}

    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override
    {
        std::cout << "Service request: " << request.getURI() << std::endl;
        // You can add more URI checks here if needed
        if (hasSubstr(request.getURI(), "/services/list") || 
            hasSubstr(request.getURI(),"/services/make"))
            return new ServiceHandler(_format);
        return nullptr;
    }

private:
    std::string _format;
};

#endif // SERVICE_REQUEST_FACTORY_H

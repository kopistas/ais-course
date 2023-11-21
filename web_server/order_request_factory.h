// order_request_factory.h

#ifndef ORDER_REQUEST_FACTORY_H
#define ORDER_REQUEST_FACTORY_H

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "handlers/order_handler.h" 
#include <iostream>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;

class OrderRequestFactory : public HTTPRequestHandlerFactory
{
public:
    OrderRequestFactory(const std::string& format) : _format(format) {}

    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override
    {
        std::cout << "Order request: " << request.getURI() << std::endl;
        // You can add more URI checks here if needed
        if (hasSubstr(request.getURI(), "/order"))
            return new OrderHandler(_format);
        return nullptr;
    }

private:
    std::string _format;
};

#endif // ORDER_REQUEST_FACTORY_H

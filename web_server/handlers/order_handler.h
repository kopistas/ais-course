#ifndef ORDER_HANDLER_H
#define ORDER_HANDLER_H

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#include "../../helper.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class OrderHandler : public HTTPRequestHandler
{
private:
    std::string _format;

public:
    OrderHandler(const std::string& format) : _format(format) {}

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        (void)request; // Silencing unused variable warning

        response.setContentType("text/plain");
        response.setStatus(HTTPServerResponse::HTTP_OK);

        std::ostream& out = response.send();
        out << "OrderHandler: Received a request with format: " << _format;
        out.flush();
    }
};

#endif // ORDER_HANDLER_H

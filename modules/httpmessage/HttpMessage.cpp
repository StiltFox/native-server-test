#include <string.h>
#include <iostream>
#include "StringManip.hpp"
#include "HttpMessage.hpp"

using namespace std;

inline string getReasonCode(int statusCode)
{
    std::unordered_map<int,string> statusCodes = {{100, "Continue"},{101, "Switching Protocols"},{102,"Processing"},
        {103, "Early Hints"},{200,"OK"},{201,"Created"},{202,"Accepted"},{203,"Non-Authoritative Information"},
        {204,"No Content"},{205,"Reset Content"},{206,"Partial Content"},{207,"Multi-Status"},{208,"Already Reported"},
        {226,"IM Used"},{300,"Multiple Choices"},{301,"Moved Permanently"},{302,"Found"},{303,"See Other"},{304,"Not Modified"},
        {305,"use proxy"},{306,"unused"},{307,"Temporary Redirect"},{308,"Permanent Redirect"},{400,"Bad Request"},
        {401,"Unauthorized"},{402,"Payment Required"},{403,"Forbidden"},{404,"Not Found"},{405,"Method Not Allowed"},
        {406,"Not Acceptable"},{407,"Proxy Authentication Required"},{408,"Request Timeout"},{409,"Conflict"},{410,"Gone"},
        {411,"Length Required"},{412,"Precondition Failed"},{413,"Payload Too large"},{414,"URI Too Long"},
        {415,"Unsupported Media Type"},{416,"Range Not Satisfiable"},{417,"Expectation Failed"},{418,"I'm a teapot"},
        {421,"Misdirected Request"},{422,"Unprocessable Entity"},{423,"Locked"},{424,"Failed Dependency"},{425,"Too Early"},
        {426,"Upgrade Required"},{428,"Precondition Required"},{429,"Too Many Requests"},{431,"Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},{500,"Internal Server Error"},{501,"Not Implemented"},{502,"Bad Gateway"},
        {503,"Service Unavailable"},{504,"Gateway Timeout"},{505,"HTTP Version Not Supported"},{506,"Variant Also Negotiates"},
        {507, "Insufficient Storage"},{508,"Loop Detected"},{510,"Not Extended"},{511,"Network Authentication Required"}};

    return statusCodes.contains(statusCode) ? statusCodes[statusCode] : "";
}

inline string getStringMethod(HttpMessage::Method method)
{
    std::unordered_map<HttpMessage::Method,string> methodStrings = {{HttpMessage::Method::GET, "GET"},
        {HttpMessage::Method::HEAD, "HEAD"},{HttpMessage::Method::POST, "POST"},{HttpMessage::Method::PUT, "PUT"},{HttpMessage::Method::PATCH, "PATCH"},
        {HttpMessage::Method::DELETE, "DELETE"},{HttpMessage::Method::CONNECT, "CONNECT"},{HttpMessage::Method::OPTIONS, "OPTIONS"},
        {HttpMessage::Method::TRACE, "TRACE"}};

    return methodStrings.contains(method) ? methodStrings[method] : "";
}

inline HttpMessage::Method getMethodFromString(string method)
{
    std::unordered_map<string,HttpMessage::Method> stringMethods = {{"GET", HttpMessage::Method::GET},
        {"HEAD", HttpMessage::Method::HEAD},{"POST", HttpMessage::Method::POST},{"PUT", HttpMessage::Method::PUT},{"PATCH", HttpMessage::Method::PATCH},
        {"DELETE", HttpMessage::Method::DELETE},{"CONNECT", HttpMessage::Method::CONNECT},{"OPTIONS", HttpMessage::Method::OPTIONS},
        {"TRACE", HttpMessage::Method::TRACE}};

    return stringMethods.contains(method) ? stringMethods[method] : HttpMessage::Method::ERROR;
}

HttpMessage::HttpMessage(int status, std::unordered_map<std::string,std::string> head, std::string bod, string reason)
{
    statusCode = status;
    httpMethod = Method::NONE;
    statusReason = reason == "" ? getReasonCode(statusCode) : reason;
    headers = head;
    body = bod;
}

HttpMessage::HttpMessage(Method method, std::string uri, std::unordered_map<std::string,std::string> head, std::string bod)
{
    statusCode = 0;
    httpMethod = method;
    requestUri = uri;
    headers = head;
    body = bod;
}

HttpMessage::HttpMessage(int socketId, function<int(int,char*,int)> reader)
{
    int readBytes = 0;
    char buffer[1024];
    string requestString;

    do
    {
        memset(buffer, 0, sizeof(buffer));
        reader(socketId, buffer, 1024);
        requestString += buffer;
    } while (buffer[1023] != 0);

    parseString(requestString);
}

void HttpMessage::parseString(string requestString)
{
    statusCode = 0;
    string methodString = parseToDelim(requestString, " ");
    httpMethod = getMethodFromString(methodString);
    requestUri = parseToDelim(requestString.substr(methodString.length() + 1), " ");
    int lineBegin = requestString.find_first_of('\n') + 1;
    headers = parseMap(parseToDelim(requestString.substr(lineBegin), "\r\n\r\n"), ": ", "\r\n");
    body = requestString.substr(requestString.find("\r\n\r\n") + 4);

    if (httpMethod == Method::ERROR)
    {
        statusCode = 500;
        statusReason = "Internal Server Error";
    }
}

void HttpMessage::setHttpMethod(Method method)
{
    httpMethod = method;
}

void HttpMessage::setRequestUri(string uri)
{
    requestUri = uri;
}

void HttpMessage::setHeader(string header, string value)
{
    headers[header] = value;
}

void HttpMessage::removeHeader(string header)
{
    headers.erase(header);
}

void HttpMessage::setBody(string content)
{
    body = content;
}

void HttpMessage::setStatus(int status)
{
    statusCode = status;
    statusReason = getReasonCode(status);
}

string HttpMessage::printBodyAndHeaders()
{
    string output;

    for (const auto & [key, value] : headers) output += key + ": " + value + "\r\n";

    return output + "\r\n" + body;
}

string HttpMessage::printAsResponse()
{
    return "HTTP/1.1 " + to_string(statusCode) + " " + statusReason+"\r\n" + printBodyAndHeaders();
}

string HttpMessage::printAsRequest()
{
    return getStringMethod(httpMethod) + " " + requestUri +" HTTP/1.1\r\n" + printBodyAndHeaders();
}

string HttpMessage::getHttpMethodAsString()
{
    return getStringMethod(httpMethod);
}

bool HttpMessage::operator==(const HttpMessage& other) const
{
    return statusCode == other.statusCode && httpMethod == other.httpMethod && requestUri == other.requestUri 
        && statusReason == other.statusReason && headers == other.headers && body == other.body;
}

void HttpMessage::setStatusReason(string reason)
{
    statusReason = reason;
}

int HttpMessage::getStatusCode()
{
    return statusCode;
}

HttpMessage::Method HttpMessage::getHttpMethod()
{
    return httpMethod;
}

string HttpMessage::getRequestUri()
{
    return requestUri;
}

string HttpMessage::getStatusReason()
{
    return statusReason;
}

unordered_map<string,string> HttpMessage::getHeaders()
{
    return headers;
}

string HttpMessage::getBody()
{
    return body;
}
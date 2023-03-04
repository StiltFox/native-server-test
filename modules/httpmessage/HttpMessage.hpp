#ifndef StiltFox_UniversalLibrary_HttpMessage
#define StiltFox_UniversalLibrary_HttpMessage
#include <string>
#include <unordered_map>
#include <functional>
class HttpMessage {
    public:
    enum Method {GET,HEAD,POST,PUT,PATCH,DELETE,CONNECT,OPTIONS,TRACE,ERROR,NONE};

    protected:
    int statusCode;
    Method httpMethod;
    std::string requestUri;
    std::string statusReason;
    std::unordered_map<std::string,std::string> headers;
    std::string body;

    std::string printBodyAndHeaders() const;
    void parseString(std::string);

    public:
    HttpMessage(int statusCode, std::unordered_map<std::string,std::string> headers = {}, std::string body = "", std::string statusReason = "");
    HttpMessage(Method method, std::string uri = "*", std::unordered_map<std::string,std::string> headers = {}, std::string body = "");
    HttpMessage(int socketId, std::function<int(int,char*,int)> reader);
    
    void setStatus(int statusCode);
    void setHttpMethod(Method method);
    void setRequestUri(std::string uri);
    void setStatusReason(std::string statusReason);
    void setHeader(std::string header,std::string value);
    void setBody(std::string content);
    void removeHeader(std::string header);
    int getStatusCode() const;
    Method getHttpMethod() const;
    std::string getRequestUri() const;
    std::string getStatusReason() const;
    std::unordered_map<std::string,std::string> getHeaders() const;
    std::string getBody() const;
    std::string getHttpMethodAsString() const;
    std::string printAsResponse() const;
    std::string printAsRequest() const;

    bool operator==(const HttpMessage&) const;
};
#endif
#ifndef StiltFox_UniversalLibrary_HttpMessage
#define StiltFox_UniversalLibrary_HttpMessage
#include <string>
#include <unordered_map>
#include <functional>
class HttpMessage {
    public:
    enum Method {GET,HEAD,POST,PUT,PATCH,DELETE,CONNECT,OPTIONS,TRACE};

    protected:
    int statusCode;
    Method httpMethod;
    std::string requestUri;
    std::string statusReason;
    std::unordered_map<std::string,std::string> headers;
    std::string body;

    std::string printBodyAndHeaders();
    void parseString(std::string);

    public:
    HttpMessage(int statusCode, std::unordered_map<std::string,std::string> headers = {}, std::string body = "", std::string statusReason = "");
    HttpMessage(Method method, std::string uri = "*", std::unordered_map<std::string,std::string> headers = {}, std::string body = "");
    HttpMessage(int socketId, std::function<int(int,char*,int)> reader);
    
    void setHttpMethod(Method method);
    void setRequestUri(std::string uri);
    void setHeader(std::string header,std::string value);
    void setBody(std::string content);
    void setStatus(int statusCode);
    void removeHeader(std::string header);
    std::string getHttpMethodAsString();
    std::string printAsResponse();
    std::string printAsRequest();
};
#endif
/*
* Welcome to HttpMessage.cpp! We are glad to have you here. Just like java can have multiple java files, C++ can
* have multiple source files. This source file is compiled separately then linked into the main executable. When
* compiling this project the order looks kinda like this:
* compile StringManip as a library
* compile HttpMessage as a library
* compile Socket as a library
* compile main.cpp and link the stringmanip, httpmessage, and socket libraries.
* Because this project is broken up into libraries like this it is really easy to reuse this code in multiple projects.
* Just copy paste the binaries that are output into a central location, along with the header files and you can reference
* them from another project just like you did in this one without writing something again.
*/
#include <string.h>
#include <iostream>
#include "StringManip.hpp"
#include "HttpMessage.hpp"

using namespace std;

/*
* Let's take a moment to address this block of functions here. You'll notice they share a few things in common. They're inline,
* they are not mentioned in the header file, and they are not scoped to the HttpMethod struct. This has a few implications.
* These functions are effectively private, because there's no definition on the header, the person who consumes this library
* wont be able to access these.
*
* As for the inline keyword, this is a request to the compiler. Inline asks the compiler to copy and paste the code wherever this
* function is called instead of putting a call on the stack. This can save function over head, and on functions that are not used
* in a lot of places this can be very helpful. Be careful using inline in programs where file size matters, like micro controllers.
*/

/*
* This function will get a 'reason code' from the provided status code. If you want a default reason you use this function. If
* the status code is not found in the map we return empty string.
*/
inline string getReasonCode(int statusCode)
{
    //This is a map of all of the standard status codes that we accept.
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

    return statusCodes.contains(statusCode) ? statusCodes[statusCode] : ""; //return the reason code if found, if not empty string.
}

/*
* Because C++ only associates enums with integer values, we need a way to extract a string from one when we print out our http message.
* We check against all the accepted methods, if we dont find one we return empty string. Very similar to the above function just with 
* a smaller map.
*/
inline string getStringMethod(HttpMessage::Method method)
{
    std::unordered_map<HttpMessage::Method,string> methodStrings = {{HttpMessage::Method::GET, "GET"},
        {HttpMessage::Method::HEAD, "HEAD"},{HttpMessage::Method::POST, "POST"},{HttpMessage::Method::PUT, "PUT"},{HttpMessage::Method::PATCH, "PATCH"},
        {HttpMessage::Method::DELETE, "DELETE"},{HttpMessage::Method::CONNECT, "CONNECT"},{HttpMessage::Method::OPTIONS, "OPTIONS"},
        {HttpMessage::Method::TRACE, "TRACE"}};

    return methodStrings.contains(method) ? methodStrings[method] : "";
}

/*
* This is the same as the above method, except this time we go from a string to a method.
*/
inline HttpMessage::Method getMethodFromString(string method)
{
    std::unordered_map<string,HttpMessage::Method> stringMethods = {{"GET", HttpMessage::Method::GET},
        {"HEAD", HttpMessage::Method::HEAD},{"POST", HttpMessage::Method::POST},{"PUT", HttpMessage::Method::PUT},{"PATCH", HttpMessage::Method::PATCH},
        {"DELETE", HttpMessage::Method::DELETE},{"CONNECT", HttpMessage::Method::CONNECT},{"OPTIONS", HttpMessage::Method::OPTIONS},
        {"TRACE", HttpMessage::Method::TRACE}};

    return stringMethods.contains(method) ? stringMethods[method] : HttpMessage::Method::ERROR;
}

/*
* Here we start getting to the actual member functions of the struct. You can tell they're member functions because they're prefaced
* with the HttpMessage:: scope. Each one of these will correspond to one of the defined methods in the header file.
*/

/*
* This method is a constructor. They are unique in that they do not have a return type; not even void. You'll notice that
* the optional parameters from the header file are not here. That seems to be ok though as while writing this code, the
* compiler seems to read it fine. Be careful though, i dont know if that's undefined behaviour. For this lesson however
* we'll pretend this is ok as it does compile and work as intended.
*
* This constructor takes a status, some headers, a body and a reason code. These names wont be seen by the user, but the names
* on the header will. Because of this we use the full names on the header file, but abbreviate here so we don't have to use the
* this pointer. We prefer to avoid name collisions when we can and having a slightly abbreviated variable for just the constructor
* in our opinion is a worthwhile sacrifice. Use your own discretion in your own programs.
*
* Anyway this constructor doesn't do anything special other than set the values and the only thing slightly spicy it does
* is provide a standard reason code if you didn't provide one.
*/
HttpMessage::HttpMessage(int status, std::unordered_map<std::string,std::string> head, std::string bod, string reason)
{
    statusCode = status;
    httpMethod = Method::NONE;
    statusReason = reason == "" ? getReasonCode(statusCode) : reason;
    headers = head;
    body = bod;
}


// This constructor is also just a simple constructor that sets some values. Nothing fancy here.
HttpMessage::HttpMessage(Method method, std::string uri, std::unordered_map<std::string,std::string> head, std::string bod)
{
    statusCode = 0;
    httpMethod = method;
    requestUri = uri;
    headers = head;
    body = bod;
}

/*
* This constructor requires a lot more explanation and the internals will be commented on. This constructor will attempt
* to parse an Http Response from a socket and a reader function. You'll see that this accepts a function as an input.
* You can think of it like a lambda function being accepted as an input. It's a way to make it so this class isn't dependant
* on operating system specific code. The function it takes returns an integer, and takes an integer, character pointer and
* integer as it's arguments.
*/
HttpMessage::HttpMessage(int socketId, function<int(int,char*,int)> reader)
{
    char buffer[1024]; // Because we dont know how much data is being sent to us we will perform a buffered read. This byte
                       // array is the maximum number of bytes we can read per pass through the loop.
    string requestString; // This is the string where the data will be appended to from the buffer. We will parse this later.

    // This is the loop that will read the data from the socket.
    do
    {
        memset(buffer, 0, sizeof(buffer)); //set the buffer to all zeros. This will clear out any previous data and insure
                                           //that if partial data is set, that we will eventually end on a 0x00 or unicode
                                           // null.
        reader(socketId, buffer, 1024); // we call the reader function passed into here with the socket id and buffer.
        requestString += buffer; // append the buffer to the output.
    } while (buffer[1023] != 0); // if the buffer does not end in unicode null, then we loop again to get more data.

    parseString(requestString); //Now that we have the data we parse it.
}

/*
* This is the function that takes the string and converts it into an HTTP message.
*/
void HttpMessage::parseString(string requestString)
{
    statusCode = 0; // because C++ does not initialize variables by default it's best to make sure that there is no
                    // random garbage stored here.
    int currentPosition = 0; // This variable keeps track of where we are in the string.
    string methodString = parseToDelim(requestString, " "); // parseToDelim is defined in stringmanip. //This line grabs the
                                                            // http method.
    httpMethod = getMethodFromString(methodString); // This will convert the string to an enum. see function above.
    currentPosition += methodString.length() + 1;  // Because arrays start at index zero we need to subtract 1 from the length.

    if (currentPosition < requestString.length()) // Make sure we are not at the end of the request.
    {
        requestUri = parseToDelim(requestString.substr(currentPosition), " "); // We grab the request url next.
        if (requestUri.find("\n") != string::npos) requestUri = ""; // if we for some reason hit end of file this was improperly
                                                                    // parsed so we set it to empty.
        currentPosition += requestUri.length() + 1; // update our position again, then check if we're at end of string.
        if (currentPosition < requestString.length())
        {
            currentPosition = requestString.find('\n') + 1; // skip the http version, we don't care.
            if (0 < currentPosition && currentPosition < requestString.length()) //make sure there's data after the new line.
            {
                // If we hit a new line right away we know that there are no headers. So we parse the body.
                if (requestString.substr(currentPosition).starts_with("\r\n")) 
                {
                    body = requestString.substr(currentPosition + 2); // we add 2 to the current position to skip the white space.
                }
                else //if we get here that means that we have http headers.
                {
                    //we use parse map and parse to delim to get the headers
                    headers = parseMap(parseToDelim(requestString.substr(currentPosition), "\r\n\r\n"), ": ", "\r\n");
                    currentPosition = requestString.find("\r\n\r\n"); // then we check if we have a body and parse that.
                    if (0 < currentPosition && currentPosition+4 < requestString.length())
                    {
                        body = requestString.substr(currentPosition+4);
                    }
                }
            }
        }
    }
}

//This function outputs the body and headers of the Http Message to a string.
string HttpMessage::printBodyAndHeaders() const
{
    string output;

    for (const auto & [key, value] : headers) output += key + ": " + value + "\r\n";

    return output + "\r\n" + body;
}

// this method outputs a string formatting the Http message as a response.
string HttpMessage::printAsResponse() const
{
    return "HTTP/1.1 " + to_string(statusCode) + " " + statusReason+"\r\n" + printBodyAndHeaders();
}

// this method outputs a string formatting the Http message as a request.
string HttpMessage::printAsRequest() const
{
    return getStringMethod(httpMethod) + " " + requestUri +" HTTP/1.1\r\n" + printBodyAndHeaders();
}

// return the Method as a string instead of an Enum.
string HttpMessage::getHttpMethodAsString() const
{
    return getStringMethod(httpMethod);
}

// Overload the comparison operator to work on two Http Messages
bool HttpMessage::operator==(const HttpMessage& other) const
{
    return statusCode == other.statusCode && httpMethod == other.httpMethod && requestUri == other.requestUri
        && statusReason == other.statusReason && headers == other.headers && body == other.body;
}

// Overload the not equals operator to work on Http Messages
bool HttpMessage::operator!=(const HttpMessage& other) const
{
    return !(statusCode == other.statusCode && httpMethod == other.httpMethod && requestUri == other.requestUri
        && statusReason == other.statusReason && headers == other.headers && body == other.body);
}

//to continue this tutorial please continue to HttpMessageTest.cpp
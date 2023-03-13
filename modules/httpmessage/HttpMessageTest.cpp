/*
* This is a unit testing file. It's written like a normal executable, however CMake is configured to run it
* during testing. The return value of this program will determine weather the test passes or fails. Here we
* use GTest as our testing framework. This framework was developed by Google to be used by it's internal 
* teams, but lucky us it's open source so we can benefit as well. We dont know what sorcery they pull off
* under the hood but GTest breaks up the program into Testing Fixtures that allows multiple tests per
* executable along with some pretty nice console output.
*/
#include <gtest/gtest.h>
#include "HttpMessage.hpp"

/*
* Because Http Method is a custom struct, GTest does not know how to print it on the console. By default
* GTest will print a string of hex values that represent how the object is stored in memory. Unless you're
* a computer, I don't think this helps us very much. This function fixes that problem and allows us to have
* a nice readable output.
*/
void PrintTo(const HttpMessage& message, std::ostream* stream)
{
    //just dump everything out into the stream and be done with it.
    *stream << "statusCode: " << message.statusCode << std::endl
        << "httpMethod: " << message.httpMethod << std::endl
        << "requestUri: " << message.requestUri << std::endl
        << "statusReason: " << message.statusReason << std::endl
        << "body: " << message.body << std::endl << "headers: [";

        for (auto const&[key, value] : message.headers)
        {
            *stream << "\"" << key << "\": \"" << value << "\", ";
        }

        *stream << "]" << std::endl;
}

/*
* Because our http method requires that we read from sockets, we need a socket to read from. This function
* can be thought of as a mock of the function a real user would pass in to our library.
*/
int getDataChunk(std::string wholeData, char* chunkBuffer, int chunkSize, int currentChunk)
{
    //Here we break the data up into chunks, then copy that chunk into the provided char buffer. The test can
    //request whatever part of the data it wants by updating currentChunk.
    int copySize = 0;
    int currentPos = currentChunk * chunkSize;
    if (currentPos < wholeData.length())
    {
        copySize = currentPos + chunkSize > wholeData.length() ? wholeData.length() - currentPos : chunkSize;
        memcpy(chunkBuffer, wholeData.substr(currentPos, chunkSize).c_str(), copySize);
    }
    return copySize;
}

/*
* Now we get to our real tests!!! You will notice that these functions don't fallow the standard function declarations
* for C++, but that is ok. GTest has defined many macros for us to use. These macros are read by the GTest framework.
* Let's get familiar with some of the macros we'll be seeing below.
*
* TEST - This macro defines a test. It takes two parameters a Suite name and a Test name. These names will appear in the
*        console output.
* ASSERT_EQ - This macro asserts that two values are equal to each other. If they are not, the test fails. There are other
*             types of assert, but we dont use them in this set of tests.
*
* The tests are pretty well documented and follow a Gherkin format. The given, when and then are all commented aproproately.
* Some notes on the tests:
* It is good practice to write comments for your given when and then. Tests serve as your specification documentation.
* By not commenting our tests, we do not provide the business intent behind our actions leaving developers who come after
* us scratching their heads.
*
* Names are very important to tests. They allow us to make a statement about what we're testing for and can clear up
* ambiguous test results. Wading through heaps of code because of a test is not fun and kills a lot of time. When a test
* fails we want to know what happened without a doubt, and that's why a good test name follows a few rules:
* Your test should be a full sentence. All too often we see test names like "Should print a request.", Who is printing 
* what? what class? This gives us no information on what's happening. Often times the person running the test, isn't going
* to be the same person who wrote it. Try to make your name clear, even if it's a bit wordy. Take a look at the name below 
* "printAsRequest_will_print_a_well_formatted_http_request". This test name is great for many reasons. In the name alone it
* tells us, what method is running, what it's trying to do, and it tells us that the test will fail if the formatting is bad.
*/
TEST(HttpMessage, printAsRequest_will_print_a_well_formed_http_request)
{
    //given we have a http message with all data needed
    HttpMessage complete(HttpMessage::GET, "/an_endpoint", {{"header","some_value"},{"header2","some_val2"}}, "this is my body");

    //when we print the http message as a  request
    std::string actual = complete.printAsRequest();

    //then we get back a well formatted request
    ASSERT_EQ(actual, "GET /an_endpoint HTTP/1.1\r\nheader2: some_val2\r\nheader: some_value\r\n\r\nthis is my body");
}

TEST(HttpMessage, printAsResponse_will_print_a_well_formatted_http_response)
{
    //given we have a http response with all the data needed
    HttpMessage complete(200, {{"header","some_value"},{"header2","some_val2"}}, "This is a body");

    //when we print the http message as a response
    std::string actual = complete.printAsResponse();

    //then we get back a well formatted response
    ASSERT_EQ(actual,"HTTP/1.1 200 OK\r\nheader2: some_val2\r\nheader: some_value\r\n\r\nThis is a body");
}

TEST(HttpMessage, reading_from_a_well_formed_socket_request_will_produce_a_well_formed_HttpMessage)
{
    //given we have a valid data stream
    std::string dataToStream = "POST /an_endpoint HTTP/1.1\r\nheader2: some_val2\r\nheader: some_value\r\n\r\nthis is my body";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actual(9080,[dataToStream, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(dataToStream, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back the desired http response
    ASSERT_EQ(actual, HttpMessage(HttpMessage::POST,"/an_endpoint",{{"header2","some_val2"},{"header","some_value"}},"this is my body"));
}

TEST(HttpMessage, reading_from_a_socket_that_does_not_have_headers_or_a_body_will_parse_properly)
{
    //given we have an http request with just the meta data
    std::string dataToStream = "POST /an_endpoint HTTP/1.1";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actual(9080,[dataToStream, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(dataToStream, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back the desired http response
    ASSERT_EQ(actual, HttpMessage(HttpMessage::POST, "/an_endpoint"));
}

TEST(HttpMessage, reading_from_a_socket_that_does_not_have_a_Body_will_parse_properly)
{
    //given we have an http request without a body
    std::string dataToStream = "POST /an_endpoint HTTP/1.1\r\nheader2: some_val2\r\nheader: some_value";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actual(9080,[dataToStream, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(dataToStream, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back the desired http response
    ASSERT_EQ(actual, HttpMessage(HttpMessage::POST, "/an_endpoint", {{"header2","some_val2"},{"header","some_value"}}));
}

TEST(HttpMessage, reading_from_a_socket_that_does_not_have_headings_will_parse_properly)
{
    //given we have an http request without headers
    std::string dataToStream = "POST /an_endpoint HTTP/1.1\r\n\r\nThis is a body";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actual(9080,[dataToStream, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(dataToStream, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back the desired http response
    ASSERT_EQ(actual, HttpMessage(HttpMessage::POST, "/an_endpoint", {}, "This is a body"));
}

TEST(HttpMessage, reading_from_a_socket_request_with_an_unknown_method_will_set_httpMethod_to_error)
{
    //given we have an http request with an unknown method
    std::string dataToStream = "PICKLE /an_endpoint HTTP/1.1\r\nheader2: some_val2\r\nheader: some_value\r\n\r\nthis is my body";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actual(9080,[dataToStream, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(dataToStream, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back that the method is not allowed
    HttpMessage expectedMessage(HttpMessage::ERROR,"/an_endpoint",{{"header2","some_val2"},{"header","some_value"}},"this is my body");
    ASSERT_EQ(actual, expectedMessage);
}

TEST(HttpMessage, reading_from_a_socket_without_an_endpoint_will_finish_parsing)
{
    //given we have an http request without an endpoint
    std::string endpointMissing = "POST  HTTP/1.1\r\nheader2: some_val2\r\nheader: some_value\r\n\r\nthis is my body";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actual(9080,[endpointMissing, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(endpointMissing, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back that the message is malformed
    HttpMessage expectedMessage(HttpMessage::POST,"",{{"header2","some_val2"},{"header","some_value"}},"this is my body");
    ASSERT_EQ(actual, expectedMessage);
}

TEST(HttpMessage, reading_from_a_socket_without_an_endpoint_or_space_will_finish_parsing)
{
    //given we have an http request without an endpoint
    std::string endpointMissingWithoutSpace = "POST HTTP/1.1\r\nheader2: some_val2\r\nheader: some_value\r\n\r\nthis is my body";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 9080
    HttpMessage actualNoSpace(9080,[endpointMissingWithoutSpace, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(endpointMissingWithoutSpace, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });

    //then we get back that the message is malformed
    HttpMessage expectedMessageWithoutSpace(HttpMessage::POST,"",{{"header2","some_val2"},{"header","some_value"}},"this is my body");
    ASSERT_EQ(actualNoSpace, expectedMessageWithoutSpace);
}

/*
* This last test here is a little weird as it has no asserts. This should logically mean that it will always pass.
* However we were having issues with segfaults and reading from out of bound arrays when we would receive corrupted data.
* A crashing program isn't good, so this test will pass as long as the program doesn't crash.
*/
TEST(HttpMessage, reading_from_an_invalid_socket_request_will_not_cause_a_seg_fault)
{
    //given we have a garbage data stream
    std::string badData = "asdlkfjwoeihghodfal n sohdfiwekladf ng;lhfpoiea;kfhoe\r\n lkasdfhoiwea";
    int currentChunk = 0;

    //when we read the socket, let's pretend it's 104
    HttpMessage actual(104,[badData, &currentChunk](int socketId, char* buffer, int chunksize)
    {
        int readBytes = getDataChunk(badData, buffer, chunksize, currentChunk);
        currentChunk++;
        return readBytes;
    });
}

//to continue this tutorial please go to ../socket/socket.cpp. We will be skipping the header because
//there's not much else to say about them. go ahead and look at the header but there should be nothing
//surprising in it.
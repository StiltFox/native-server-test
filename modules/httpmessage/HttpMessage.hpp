/*
* This is a header file. 
*
* Header files are used to define functions and objects before their code is written. This is useful because
* in C++ the compiler only does a one time pass over your code. If a definition for a function is not found before
* it is used, this is a compile error. Header files are a way of making sure that all of your functions and objects
* are defined before use, regardless of when the function code is actually provided. This allows for functions to
* reference each other more easily and generally not worry about when they were declared as much.
*
* More importantly header files are used as a public interface for libraries you may write. Because C++ code is compiled
* and not interpreted, that means once the library is built there's nothing human readable to look at. The header file
* provides a human readable document that allows the person using your library to see what functions are available to them,
* enables gui code completion, and helps the compiler know how to hook your library into the new project.
*
* Now that that's out of the way, lets talk about our new preprocessor commands a bit!
* #ifndef - means to include whatever is between this and #endif if a particular value is defined.
*           This value can be defined by the compiler ahead of time, or by cmake, or even by another preprocessor command.
* #define - This defines a preprocessor variable. Often times no value is assigned but that does not mean there cant be!
*           If a define has a value.. ie: #define value 1 ..then everywhere value is written in your code, the compiler will
*           replace it with 1 before compiling. Because it's replaced before compile it is as fast as using a literal and
*           no variables are created.
* #endif - this signals the end of an if block, in this case it's #ifndef's if block.
*
* As mentioned before C++ compiler does a one time passthrough, so defining functions, ect.. is picky about order.
* Another problem arises when an object or function is defined more than once. If this happens the compiler does not know
* what function to call when and produces a compiler error. Header files are often used all over the place! We dont have control
* over when or where a programmer may use this. So to prevent this code from being defined more than once we use preprocessor 
* commands! If our preprocessor variable is not defined, we define it and include this code, if it is already defined though
* the compiler now knows to ignore this entire codeblock. Just like it never existed.
*/
#ifndef StiltFox_UniversalLibrary_HttpMessage
#define StiltFox_UniversalLibrary_HttpMessage
#include <string>
#include <unordered_map>
#include <functional>

/*
* A C++ struct is a lot like an object in other languages. It can contain methods, and fields and can exercise data
* encapsulation (If you believe in that kind of thing). Here we clearly throw the laws of encapsulation out the window
* because we're like that. The Philosophy lecture can come later, but for now let's dig into the code.
*
* Structs and Classes both exist in C++ and have an important difference. Structs default to public access, where as
* Classes default to private. Pick according to however you wish to organize your code. Want your private fields
* and methods at the top? pick a Class. Prefer defining that later? pick a struct. Here we start with the public 
* stuff first.
*/
struct HttpMessage {
    /*
    * This enum is a representation of the Http Methods that we will accept with two special ones. Error and None
    * are only used in particular instances and not meant to be sent to someone.
    */
    enum Method {GET,HEAD,POST,PUT,PATCH,DELETE,CONNECT,OPTIONS,TRACE,ERROR,NONE};
    int statusCode; //this is the actual status code. Ie: 200, 201, 404, 500
    Method httpMethod; //This is the method of this message
    std::string requestUri; //This is the request uri. This may or may not have host information
    std::string statusReason; //This is based off of the statusCode typically, but may be set to something other than the default
    std::unordered_map<std::string,std::string> headers; //this is a set of http headers and values
    std::string body; //The body of the http message

    /*
    * These are the constructors for the HttpMessage struct. We define 3 of them. 
    * The first one is intended to build an Http response object. It takes in a status code while everything else is optional.
    * The second one is intended to make an Http request object. It takes a method with everything else being optional;
    * The last one is the interesting one that reads data from a socket and creates an http request based on the streamed data.
    * 
    * Constructors and functions in C++ can have what is known as default parameters. Default parameters must come last in the
    * parameter list. When the parameter is ot provided it defaults to whatever it was set to here.
    */
    HttpMessage(int statusCode, std::unordered_map<std::string,std::string> headers = {}, std::string body = "", std::string statusReason = "");
    HttpMessage(Method method, std::string uri = "*", std::unordered_map<std::string,std::string> headers = {}, std::string body = "");
    HttpMessage(int socketId, std::function<int(int,char*,int)> reader);
    
    /*
    * Something here to note is the little const specifier at the end of these functions. This means that these functions
    * promise not to change or set any values in this struct. This means that this function can be called on a const object!
    * Functions not marked with const, cannot be called on const objects. Also const objects, cant get passed into non const
    * parameters.
    */
    std::string getHttpMethodAsString() const;
    std::string printAsResponse() const;
    std::string printAsRequest() const;

    /*
    * These are operator overloads.
    * In C++ you can actually change how operators like the + or - or even = works on your classes and structs.
    * Here we overload the == and != operators to allow proper comparisons of these classes.
    * 
    * A note on const inputs to functions: This little const marker means that the passed in value will not be
    * modified, and only const member functions can be called on it.
    */
    bool operator==(const HttpMessage&) const;
    bool operator!=(const HttpMessage&) const;

    /*
    * This is the end of the public block of this structure. Below this we enter the protected block. When a new
    * protection type is declared all statements under it are of that protection type. so from her on, until otherwise
    * specified everything is now protected. The main protection types used are public, protected and private. You
    * will notice that we dont use the public specifier above, but remember what we said before: structs are public
    * by default; Meaning we don't need it.
    */
    protected:
    std::string printBodyAndHeaders() const;
    void parseString(std::string);
};
#endif

//To continue this tutorial please move on to HttpMessage.cpp
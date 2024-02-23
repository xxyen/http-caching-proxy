#include <string>

class Request {
private:
    std::string request;
    std::string host;
    std::string method;
    std::string uri;
    std::string request_line;
    std::string header;
    std::string body;
    std::string content_length;
    std::string port;
    std::string cache_control;
    void parse(const std::string& raw_request);
public:
    Request(const std::string& raw_request) {
        parse(raw_request);
    }
    std::string getRequest() const;
    std::string getHost() const;
    std::string getMethod() const;
    std::string getUri() const;
    std::string getRequestLine() const;
    std::string getHeader() const;
    std::string getBody() const;
    std::string getContentLength() const;
    std::string getPort() const;
    std::string getCacheControl() const;
};

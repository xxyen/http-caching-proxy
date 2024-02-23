#include "request.hpp"
#include "response.hpp"
#include <iostream>

int main() {
    std::string http_request = "GET http://www.man7.org/linux/man-pages/man2/recv.2.html HTTP/1.1\r\n"
        "Host: www.man7.org\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.13; rv:58.0) Gecko/20100101 Firefox/58.0\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Connection: keep-alive\r\n"
        "Upgrade-Insecure-Requests: 1\r\n";

    std::string http_response = "HTTP/1.1 200 OK\r\n"
        "Date: Fri, 23 Feb 2024 05:49:38 GMT\r\n"
        "Server: Apache\r\n"
        "Last-Modified: Fri, 23 Feb 2024 05:45:36 GMT\r\n"
        "ETag: \"1709f-6120611b3fc9f-gzip\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Vary: Accept-Encoding\r\n"
        "Content-Encoding: gzip\r\n"
        "Content-Length: 22255\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html\r\n";

    Request request(http_request);
    std::cout << "Parsed HTTP Request:" << std::endl;
    std::cout << "Method: " << request.getMethod() << std::endl;
    std::cout << "URI: " << request.getUri() << std::endl;
    std::cout << "Host: " << request.getHost() << std::endl;
    std::cout << "Port: " << request.getPort() << std::endl;
    std::cout << "Cache-Control: " << request.getCacheControl() << std::endl;
    std::cout << "Content-Length: " << request.getContentLength() << std::endl;
    std::cout << "Body: " << request.getBody() << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    Response response(http_response);
    std::cout << "Parsed HTTP Response:" << std::endl;
    std::cout << "Status: " << response.getStatus() << std::endl;
    std::cout << "Content-Type: " << response.getContentType() << std::endl;
    std::cout << "Content-Length: " << response.getContentLength() << std::endl;
    std::cout << "Cache-Control: " << response.getCacheControl() << std::endl;
    std::cout << "Expires: " << response.getExpires() << std::endl;
    std::cout << "Last Modified: " << response.getLastModified() << std::endl;
    std::cout << "Body: " << response.getBody() << std::endl;

    return 0;
}

#include "request.hpp"
#include <sstream>
#include <iostream>

void Request::parse(const std::string& raw_request) {
    std::istringstream request_stream(raw_request);
    std::string line;
    bool request_line_parsed = false;

    while (std::getline(request_stream, line)) {
        if (!request_line_parsed) {
            request_line = line;
            std::istringstream line_stream(line);
            line_stream >> method >> uri;
            request_line_parsed = true;
        }
        else if (line.find("Host:") != std::string::npos) {
            host = line.substr(line.find(' ') + 1);
            size_t host_line_end;
            host_line_end = host.find_first_of("\r\n");
            if (host.find(':') != std::string::npos) {
                port = host.substr(host.find(':') + 1, host_line_end);
                host = host.substr(0, host.find(':'));
            }
            else {
                host = host.substr(0, host_line_end);
                port = "80";
            }
        }
        else if (line.find("Cache-Control:") != std::string::npos) {
            cache_control = line.substr(line.find(' ') + 1);
        }
        else if (line.find("Content-Length:") != std::string::npos) {
            content_length = line.substr(line.find(' ') + 1);
        }
        else if (line.empty()) {
            break;
        }
        else {
            header += line + "\r\n";
        }
    }

    if (!content_length.empty()) {
        int length = std::stoi(content_length);
        char* buffer = new char[length + 1];
        request_stream.read(buffer, length);
        buffer[length] = '\0';
        body = std::string(buffer, length);
        delete[] buffer;
    }

    request = raw_request;
}

std::string Request::getRequest() const { return request; }
std::string Request::getHost() const { return host; }
std::string Request::getMethod() const { return method; }
std::string Request::getUri() const { return uri; }
std::string Request::getRequestLine() const { return request_line; }
std::string Request::getHeader() const { return header; }
std::string Request::getBody() const { return body; }
std::string Request::getContentLength() const { return content_length; }
std::string Request::getPort() const { return port; }
std::string Request::getCacheControl() const { return cache_control; }

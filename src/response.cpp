#include "response.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string.h>

void Response::parse(const std::string& raw_response)
{
    std::istringstream response_stream(raw_response);
    std::string line;
    bool first_line_parsed = false;

    while (std::getline(response_stream, line)) {
        if (!first_line_parsed) {
            first_line = line;
            std::istringstream line_stream(line);
            line_stream >> status; // Assumes status code is the second token in the line
            first_line_parsed = true;
        }
        else if (line.find("Content-Length:") != std::string::npos) {
            content_length = std::stoi(line.substr(line.find(' ') + 1));
        }
        else if (line.find("Content-Type:") != std::string::npos) {
            content_type = line.substr(line.find(' ') + 1);
        }
        else if (line.find("Expires:") != std::string::npos) {
            expires = line.substr(line.find(' ') + 1);
        }
        else if (line.find("Cache-Control:") != std::string::npos) {
            cache_control = line.substr(line.find(' ') + 1);
        }
        else if (line.find("ETag:") != std::string::npos) {
            etag = line.substr(line.find(' ') + 1);
        }
        else if (line.find("Last-Modified:") != std::string::npos) {
            last_modified = line.substr(line.find(' ') + 1);
        }
        else if (line.find("Date:") != std::string::npos) {
            date = line.substr(line.find(' ') + 1);
        }
        else if (line.find("Transfer-Encoding:") != std::string::npos) {
            transfer_encoding = line.substr(line.find(' ') + 1);
        }
        else if (line.empty()) {
            break;
        }
        else {
            header += line + "\r\n";
        }
    }

    // Read the body if Transfer-Encoding is not chunked (simplified handling)
    if (transfer_encoding.find("chunked") == std::string::npos && content_length > 0) {
        char* buffer = new char[content_length + 1];
        response_stream.read(buffer, content_length);
        buffer[content_length] = '\0';
        body = std::string(buffer, content_length);
        delete[] buffer;
    }

    response = raw_response;
}

std::string Response::getResponse() const { return response; }
std::string Response::getStatus() const { return status; }
std::string Response::getDate() const { return date; }
std::string Response::getContentType() const { return content_type; }
std::string Response::getExpires() const { return expires; }
int Response::getMaxAge() const { return max_age; }
std::string Response::getTransferEncoding() const { return transfer_encoding; }
std::string Response::getHeader() const { return header; }
std::string Response::getBody() const { return body; }
int Response::getContentLength() const { return content_length; }
std::string Response::getCacheControl() const { return cache_control; }
std::string Response::getEtag() const { return etag; }
std::string Response::getLastModified() const { return last_modified; }
std::string Response::getFirstLine() const { return first_line; }

time_t convertStringToTimeT(const std::string& date_str)
{
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    const char* format = "%a, %d %b %Y %H:%M:%S %Z";
    if (strptime(date_str.c_str(), format, &tm) == nullptr) {
        return static_cast<time_t>(-1);
    }
    time_t time = mktime(&tm);
    return time;
}

time_t Response::getConvertedDate() const
{
    return convertStringToTimeT(date);
}

time_t Response::getConvertedExpires() const
{
    return convertStringToTimeT(expires);
}

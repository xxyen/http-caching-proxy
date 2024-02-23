#include <string>
#include <ctime>

class Response {
private:
    std::string response;
    std::string status;
    std::string date;
    std::string content_type;
    std::string expires;
    int max_age;
    std::string transfer_encoding;
    std::string header;
    std::string body;
    int content_length;
    std::string cache_control;
    std::string etag;
    std::string last_modified;
    std::string first_line;
    time_t converted_date;
    time_t converted_expires;
    void parse(const std::string& raw_response);

public:
    Response(const std::string& raw_response) {
        parse(raw_response);
    }
    std::string getResponse() const;
    std::string getStatus() const;
    std::string getDate() const;
    std::string getContentType() const;
    std::string getExpires() const;
    int getMaxAge() const;
    std::string getTransferEncoding() const;
    std::string getHeader() const;
    std::string getBody() const;
    int getContentLength() const;
    std::string getCacheControl() const;
    std::string getEtag() const;
    std::string getLastModified() const;
    std::string getFirstLine() const;
    time_t getConvertedDate() const;
    time_t getConvertedExpires() const;
};



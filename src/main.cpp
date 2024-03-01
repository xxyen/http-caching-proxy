#include "proxy.hpp"

int main() {
    const char* port = "12345";
    Proxy* proxy = new Proxy(port);
    proxy->start();
    return 0;
}
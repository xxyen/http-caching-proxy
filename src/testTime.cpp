#include<iostream>
#include<ctime>
std::string getCurrentTime() {
    std::time_t currentTime = std::time(NULL);
    std::tm* timeInfo = std::localtime(&currentTime);

    return asctime(timeInfo);
}

int main() {
    std::cout << getCurrentTime() << std::endl;
    return 0;
}
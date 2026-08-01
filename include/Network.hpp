#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <cstdint>

class Network
{
public:
    Network();
    ~Network();

    bool hostGame(int port);
    bool joinGame(const std::string& ip, int port);
    bool isConnected() const { return connected; }

    void sendInt(int32_t value);
    bool pollInt(int32_t& outValue);

    void close();

private:
    int listenFd = -1;
    int connFd = -1;
    bool connected = false;

    uint8_t recvBuf[4];
    int recvBufLen = 0;

    void setNonBlocking(int fd);
};

#endif

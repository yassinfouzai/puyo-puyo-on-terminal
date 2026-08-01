#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <vector>
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

    bool sendBytes(const uint8_t* data, size_t count);
    bool pollBytes(uint8_t* outData, size_t count);

    void close();

private:
    int listenFd = -1;
    int connFd = -1;
    bool connected = false;

    uint8_t recvBuf[4];
    int recvBufLen = 0;

    std::vector<uint8_t> byteRecvBuf;
    size_t byteRecvLen = 0;

    void setNonBlocking(int fd);
};

#endif

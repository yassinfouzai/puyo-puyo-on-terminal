#include "../include/Network.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

Network::Network() {}
Network::~Network() { close(); }

void Network::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool Network::hostGame(int port)
{
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        return false;
    }

    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listenFd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "bind() failed: %s\n", strerror(errno));
        return false;
    }
    if (listen(listenFd, 1) < 0)
    {
        fprintf(stderr, "listen() failed: %s\n", strerror(errno));
        return false;
    }

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    connFd = accept(listenFd, (sockaddr*)&clientAddr, &clientLen);
    if (connFd < 0)
    {
        fprintf(stderr, "accept() failed: %s\n", strerror(errno));
        return false;
    }

    setNonBlocking(connFd);
    connected = true;
    return true;
}

bool Network::joinGame(const std::string& ip, int port)
{
    connFd = socket(AF_INET, SOCK_STREAM, 0);
    if (connFd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) return false;

    if (connect(connFd, (sockaddr*)&addr, sizeof(addr)) < 0) return false;

    setNonBlocking(connFd);
    connected = true;
    return true;
}

void Network::sendInt(int32_t value)
{
    if (!connected) return;
    int32_t netValue = htonl(value);
    send(connFd, &netValue, sizeof(netValue), 0);
}

bool Network::pollInt(int32_t& outValue)
{
    if (!connected) return false;

    while (recvBufLen < 4)
    {
        ssize_t n = recv(connFd, recvBuf + recvBufLen, 4 - recvBufLen, 0);
        if (n > 0)
        {
            recvBufLen += n;
        }
        else if (n == 0)
        {
            connected = false;
            return false;
        }
        else
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN) return false;
            connected = false;
            return false;
        }
    }

    int32_t netValue;
    memcpy(&netValue, recvBuf, 4);
    outValue = ntohl(netValue);
    recvBufLen = 0;
    return true;
}

void Network::close()
{
    if (connFd >= 0)   { ::close(connFd);   connFd = -1; }
    if (listenFd >= 0) { ::close(listenFd); listenFd = -1; }
    connected = false;
}

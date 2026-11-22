#pragma once

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <memory>
#include "InetAddr.hpp"

namespace NS_SOCKET
{
    static const int gbacklog = 16;

    enum
    {
        SUCCESS = 0,
        CREATE_ERR,
        BIND_ERR,
        LISTEN_ERR,
    };

    class Socket
    {
    public:
        ~Socket()
        {
        }

    protected:
        virtual void CreateSocketOrDie() = 0;
        virtual void BindSocketOrDie(uint16_t port) = 0;
        virtual void ListenSocketOrDie() = 0;

    public:
        virtual std::shared_ptr<Socket> Accept(InetAddr &addr) = 0;
        virtual int Sockfd() = 0;
        virtual int Recv(std::string *out) = 0;
        virtual int Send(const std::string &in) = 0;
        virtual void Close() = 0;
        virtual bool Connect(InetAddr &addr) = 0;

    public:
        void BuildTcpSocketMethod(uint16_t port) // 模板方法
        {
            CreateSocketOrDie();
            BindSocketOrDie(port);
            ListenSocketOrDie();
        }

        void BulidTcpClientSocketMethod()
        {
            CreateSocketOrDie();
        }
        // void BulidDdpSocketMethod()
        // {
        //     CreateSocketOrDie();
        //     BindSocketOrDie();
        // }
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket() : _sockfd(-1)
        {
        }
        TcpSocket(int sockfd) : _sockfd(sockfd)
        {
        }

        void CreateSocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                std::cerr << "create tcp socket error!" << std::endl;
                exit(CREATE_ERR);
            }
        }

        void BindSocketOrDie(uint16_t port) override
        {
            InetAddr addr(port);
            if (bind(_sockfd, addr.NetAddress(), addr.Len()) != 0)
            {
                std::cerr << "bind tcp socket error!" << std::endl;
                exit(BIND_ERR);
            }
        }

        void ListenSocketOrDie() override
        {
            if (listen(_sockfd, gbacklog) < 0)
            {
                std::cerr << "listen tcp socket error!" << std::endl;
                exit(LISTEN_ERR);
            }
        }

        std::shared_ptr<Socket> Accept(InetAddr &clientaddr) override
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int sockfd = accept(_sockfd, (struct sockaddr *)&addr, &len);
            if (sockfd < 0)
            {
                std::cerr << "accept error" << std::endl;
                return nullptr;
            }
            clientaddr = addr;
            return std::make_shared<TcpSocket>(sockfd);
        }

        bool Connect(InetAddr &addr) override
        {
            int n = connect(_sockfd, addr.NetAddress(), addr.Len());
            if (n < 0)
                return false;
            return true;
        }

        int Sockfd() override
        {
            return _sockfd;
        }

        int Recv(std::string *out) override
        {
            char inbuffer[1024000];
            ssize_t n = recv(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
            if (n > 0)
            {
                inbuffer[n] = 0;
                *out += inbuffer;
            }
            return n;
        }

        int Send(const std::string &in) override
        {
            return send(_sockfd, in.c_str(), in.size(), 0);
        }

        void Close() override
        {
            if (_sockfd > 0)
            {
                close(_sockfd);
                _sockfd = -1;
            }
        }

        ~TcpSocket()
        {
        }

    private:
        int _sockfd;
    };

    class UdpSocket : public Socket
    {
    };

}

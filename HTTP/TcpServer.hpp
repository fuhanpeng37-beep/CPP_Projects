#include "InetAddr.hpp"
#include "Socket.hpp"
#include <memory>
#include <functional>

using namespace NS_SOCKET;

using Headler_t = std::function<std::string(std::string &)>;

class TcpServer
{
public:
    TcpServer(Headler_t headler, uint16_t port = 8080)
        : _listensockfd(std::make_unique<TcpSocket>()), _port(port), _headler(headler)
    {
        _listensockfd->BuildTcpSocketMethod(_port);
        std::cout << "create listen socket success : " << _listensockfd->Sockfd() << std::endl;
    }

    void Start()
    {
        while (true)
        {
            InetAddr clientaddr;
            auto sockfd = _listensockfd->Accept(clientaddr);
            if (!sockfd)
            {
                continue;
            }
            std::cout << "get a new link, socket address = " << clientaddr.ToString() << " sockfd = " << sockfd->Sockfd() << std::endl;

            if (fork() == 0)
            {
                // child

                service(sockfd, clientaddr);
                sockfd->Close();
                exit(0);
            }
        }
    }

    ~TcpServer()
    {
    }

private:
    void service(std::shared_ptr<Socket> sockfd, InetAddr &clientaddr)
    {
        std::string inbuffer, outbuffer;

        outbuffer.clear();

        if (sockfd->Recv(&inbuffer) <= 0)//认为第一次就读到了完整的http请求 
        {
            std::cerr << "recv : client quit," << clientaddr.ToString() << std::endl;
            return;
        }

        if (_headler)
            outbuffer = _headler(inbuffer);

        if (outbuffer.empty())
            return;

        if (sockfd->Send(outbuffer) <= 0)
        {
            std::cerr << "send : client quit," << clientaddr.ToString() << std::endl;
        }
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensockfd;
    Headler_t _headler;
};
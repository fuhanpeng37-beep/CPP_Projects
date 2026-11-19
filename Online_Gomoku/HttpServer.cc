#include "TcpServer.hpp"
#include "Http.hpp"

void Login(HttpRequest &req, HttpResponse &resp)
{
    std::cout << "Login Success !" << std::endl;
    req.DebugPrint();
    
    std::string data = req.GetText();
    std::cout << "data : " << data << std::endl;

    resp.SetCode(200);
    resp.AddHeader("Content-Type",MiniType::SuffixToMimeType(".txt"));
    resp.SetBody("Login Success!");
    resp.AddHeader("Content-Length",std::to_string(resp.GetBodySize()));
}

void Register(HttpRequest &req, HttpResponse &resp)
{
}

void Search(HttpRequest &req, HttpResponse &resp)
{
}

void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    // 0.定义计算器对象

    // 1.定义HTTP协议对象
    std::unique_ptr<HttpProtocol> protocol = std::make_unique<HttpProtocol>();
    protocol->RegisterService("/Login", Login);
    protocol->RegisterService("/Register", Register);
    protocol->RegisterService("/Search", Search);

    // 2.定义网络对象
    //  TcpServer tsvr;
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer) -> std::string
        {
            return protocol->HandlerHttpRequest(inbuffer);
        },
        port);

    // 3.启动
    tsvr->Start();
    return 0;
}
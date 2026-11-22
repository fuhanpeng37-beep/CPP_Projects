#include <iostream>
#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> wsserver_t;

void http_callback(wsserver_t *server, websocketpp::connection_hdl hdl)
{
    //先给客户端返回hello world页面
    wsserver_t::connection_ptr connect = server->get_con_from_hdl(hdl); //获取connect连接指针
    websocketpp::http::parser::request req = connect->get_request();    //获取客户端请求
    std::cout << "body = " << req.get_body() << std::endl;                 //获取请求正文
    std::cout << "method = " << req.get_method() << std::endl;             //获取请求类型
    std::cout << "uri = " << req.get_uri() << std::endl;                   //获取请求uri

    std::string body = "<html><body><h1>hello world</h1></body></html>";
    connect->set_body(body);                                            //设置应答正文
    connect->append_header("Content-Type","text/html");                 //设置应答数据类型
    connect->set_status(websocketpp::http::status_code::ok);            //设置应答状态码
}

void ws_open_callback(wsserver_t *server, websocketpp::connection_hdl hdl)
{
    std::cout << "websocket握手成功!!!" << std::endl;
}

void ws_close_callback(wsserver_t *server, websocketpp::connection_hdl hdl)
{
    std::cout << "websocket连接断开!!!" << std::endl;
}

void ws_msg_callback(wsserver_t *server, websocketpp::connection_hdl hdl, wsserver_t::message_ptr msg)
{
    wsserver_t::connection_ptr connect = server->get_con_from_hdl(hdl);
    std::cout << "wsmsg : " << msg->get_payload() << std::endl;         //获取消息内容
    std::string resp = "client say:" + msg->get_payload();
    connect->send(resp,websocketpp::frame::opcode::text);               //发送数据接口
}

int main()
{
    // 1.实例化websocket服务器对象
    wsserver_t wsserver;

    // 2.设置日志等级
    wsserver.set_access_channels(websocketpp::log::alevel::none);

    // 3.初始化asio调度器
    wsserver.init_asio();
    wsserver.set_reuse_addr(true);

    // 4.设置回调函数
    wsserver.set_http_handler(std::bind(http_callback,&wsserver,std::placeholders::_1));
    wsserver.set_open_handler(std::bind(ws_open_callback,&wsserver,std::placeholders::_1));
    wsserver.set_close_handler(std::bind(ws_close_callback,&wsserver,std::placeholders::_1));
    wsserver.set_message_handler(std::bind(ws_msg_callback,&wsserver,std::placeholders::_1,std::placeholders::_2));

    // 5.设置监听端口
    wsserver.listen(8080);

    // 6.开始获取新链接
    wsserver.start_accept();

    // 7.启动服务器
    wsserver.run();

    return 0;
}
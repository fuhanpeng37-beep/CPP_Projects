#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <functional>
#include <unordered_map>

const std::string linesep = "\r\n";
const std::string headersep = ": ";
const std::string spacesep = " ";
const std::string argsep = "?";
const std::string http_version = "HTTP/1.1";
const std::string first_page = "index.html";
const std::string www_root = "wwwroot";
const std::string page_404 = "./wwwroot/404.html";

class Util // 封装工具类
{
public:
    static std::string Readline(std::string &httpstr) // 读取一整行并删除这一行
    {
        auto pos = httpstr.find(linesep);
        if (pos == std::string::npos)
            return std::string();
        std::string line = httpstr.substr(0, pos);
        httpstr.erase(0, line.size() + linesep.size());
        if (line.empty()) // 当读到报头和正文之间的空行时
            return linesep;
        return line;
    }

    static std::string ReadFile(const std::string &filename)
    {
        // 采用二进制的方法读数据
        // std::ifstream in(filename, std::ios::binary);

        // if (!in.is_open())
        //     return std::string();

        // in.seekg(0,in.end);
        // int filesize = in.tellg();
        // in.seekg(0,in.beg);

        // std::string content;
        // content.resize(filesize);
        // in.read((char*)content.c_str(),filesize);
        // in.close();
        // return content;

        std::ifstream in(filename, std::ios::binary);

        if (!in.is_open())
            return "";

        std::ostringstream ss;
        ss << in.rdbuf();

        return ss.str();
    }
};

class HttpRequest
{
private:
    bool ParseReqLine(std::string &httpstr)
    {
        std::string req_line = Util::Readline(httpstr);
        if (req_line.empty() || req_line == linesep)
            return false;
        std::stringstream ss(req_line);
        ss >> _method >> _uri >> _version;

        if (_uri == "/")
            _uri += first_page;

        _uri = www_root + _uri; // /index.html -> wwwroot/index.html

        // 填写文件后缀
        auto pos = _uri.rfind("."); // 倒着找
        if (pos == std::string::npos)
            _suffix = ".html";
        else
            _suffix = _uri.substr(pos);

        return true;
    }

    bool ParseHeaderkv(std::string &httpstr)
    {
        std::string header_line;
        do
        {
            header_line = Util::Readline(httpstr);
            if (header_line.empty())
                return false;
            if (header_line != linesep) // 当没有读到报头和正文之间的空行时
            {
                auto pos = header_line.find(headersep);
                if (pos == std::string::npos)
                    return false;
                std::string k = header_line.substr(0, pos);
                std::string v = header_line.substr(pos + headersep.size());

                // _header[k] = v;
                // _header.insert(std::make_pair(k,v));
                _header.insert({k, v}); // 三种方法都可以
            }
        } while (header_line != linesep);

        return true;
    }

    bool ParseText(std::string &httpstr)
    {
        if (strcasecmp(_method.c_str(), "get") == 0) // 判断是不是get方法,忽略大小写
        {
            auto pos = _uri.find(argsep);
            if(pos == std::string::npos)
                _text = std::string();
            else
            {
                _text = _uri.substr(pos+argsep.size());
                _uri = _uri.substr(0,pos);
            }
        }
        else
        {
            // 判断有没有正文
            if (_header.find("Content-Length") == _header.end())
                _text = "";
            else
            {
                int content_len = std::stoi(_header["Content-Length"]);
                _text = httpstr.substr(0, content_len);
                httpstr.erase(0, content_len);
            }
        }

        return true;
    }

public:
    // 反序列化
    bool Deserialize(std::string &httpstr)
    {
        if (!ParseReqLine(httpstr)) // 处理请求首行
            return false;
        std::cout << "method# " << _method << std::endl;
        std::cout << "uri# " << _uri << std::endl;
        std::cout << "version# " << _version << std::endl;

        if (!ParseHeaderkv(httpstr)) // 处理报头部分
            return false;

        _blink_line = linesep;

        if (!ParseText(httpstr)) // 处理正文部分
            return false;

        return true;
    }

    std::string GetUri()
    {
        return _uri;
    }

    std::string GrtRequrstContent() // 获取网页的内核代码
    {
        return Util::ReadFile(_uri);
    }

    std::string GetSuffix()
    {
        return _suffix;
    }

    std::string GetText()
    {
        return _text;
    }

    void DebugPrint()
    {
        std::cout << "_method = " << _method << std::endl;
        std::cout << "_uri = " << _uri << std::endl;
        std::cout << "_version = " << _version << std::endl;

        for (auto &header : _header)
        {
            std::cout << "==>" << header.first << " # " << header.second << std::endl;
        }

        std::cout << _blink_line;
        std::cout << "_text = " << _text << std::endl;
    }

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _header;
    std::string _blink_line;
    std::string _text;

private:
    std::string _suffix; // 文件后缀
};

class HttpResponse
{
private:
    std::string CodeToDesc(int code)
    {
        switch (code)
        {
        // 1xx 信息响应
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocols";

        // 2xx 成功
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 204:
            return "No Content";
        case 206:
            return "Partial Content";

        // 3xx 重定向
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";

        // 4xx 客户端错误
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 413:
            return "Content Too Large";
        case 415:
            return "Unsupported Media Type";
        case 429:
            return "Too Many Requests";

        // 5xx 服务器错误
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";

        default:
            return "Unknown";
        }
    }

public:
    HttpResponse() : _version(http_version), _code(0), _code_describe(linesep), _blink_line(linesep)
    {
    }

    void SetCode(int code)
    {
        _code = code;
        _code_describe = CodeToDesc(code);
    }

    void AddHeader(const std::string &key, const std::string &value)
    {
        _header[key] = value;
    }

    void SetBody(const std::string &content)
    {
        _text = content;
    }

    int GetBodySize()
    {
        return _text.size();
    }

    // 序列化
    std::string Serialize()
    {
        std::string respstr;

        // 添加状态行
        respstr += _version;
        respstr += spacesep;
        respstr += std::to_string(_code);
        respstr += spacesep;
        respstr += _code_describe;
        respstr += linesep;

        // 添加报头
        for (auto &header : _header)
        {
            std::string line = header.first + headersep + header.second + linesep;
            respstr += line;
        }

        // 添加换行符
        respstr += _blink_line;

        // 添加正文
        respstr += _text;

        return respstr;
    }

private:
    std::string _version;
    int _code;
    std::string _code_describe;
    std::unordered_map<std::string, std::string> _header;
    std::string _blink_line;
    std::string _text;
};

using service_t = std::function<void(HttpRequest &req, HttpResponse &resp)>;

class MiniType
{
public:
    static std::string SuffixToMimeType(const std::string &suffix)
    {
        auto pos = _mime_types.find(suffix);
        if (pos == _mime_types.end())
            return "text/html";
        else
            return pos->second;
    }

private:
    static std::unordered_map<std::string, std::string> _mime_types;
};

std::unordered_map<std::string, std::string> MiniType::_mime_types = {
    {".html", "text/html; charset=utf-8"},
    {".css", "text/css"},
    {".js", "text/javascript"},
    {".txt", "text/plain"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".mp4", "video/mp4"},
    {".mp3", "audio/mpeg"},
    {".json", "application/json"},
    {".pdf", "application/pdf"}};

class HttpProtocol
{
public:
    HttpProtocol()
    {
    }

    void RegisterService(const std::string &uri, service_t service) // 注册服务类型
    {
        std::string key = www_root + uri;
        _http_services[key] = service;
    }

    bool IsService(const std::string &uri)
    {
        auto iter = _http_services.find(uri);
        if (iter == _http_services.end())
            return false;
        else
            return true;
    }

    std::string HandlerHttpRequest(std::string &req)
    {
        // 1.保证报文完整性，基本和之前的网络计算器一样，这里就不重复做了

        // 2.反序列化
        HttpRequest http_req;
        http_req.Deserialize(req);

        std::cout << "Suffix : " << http_req.GetSuffix() << std::endl;

        // 3.根据不同的请求方式和不同的uri，给用户返回不同的报文
        // 根据结构化的http_req 返回结构化的 http_resp
        HttpResponse http_resp;

        // 4.判断是否是服务链接
        if (IsService(http_req.GetUri()))
        {
            _http_services[http_req.GetUri()](http_req, http_resp);
        }
        else // 返回静态链接
        {
            std::string content = http_req.GrtRequrstContent();
            if (content.empty())
            {
                // http_resp.SetCode(404);
                // http_resp.SetBody(Util::ReadFile(page_404));
                // http_resp.AddHeader("Content-Length",std::to_string(http_resp.GetBodySize()));

                // 临时重定向
                http_resp.SetCode(302);
                http_resp.AddHeader("Location", "/404.html");
            }

            else
            {
                http_resp.SetCode(200);
                http_resp.SetBody(content);
                http_resp.AddHeader("Content-Length", std::to_string(http_resp.GetBodySize()));
                http_resp.AddHeader("Content-Type", MiniType::SuffixToMimeType(http_req.GetSuffix())); // 设置内容类型
                http_resp.AddHeader("Connection", "close");                                            // 设置成短连接
            }
        }

        return http_resp.Serialize();
    }

    ~HttpProtocol()
    {
    }

private:
    std::unordered_map<std::string, service_t> _http_services;
};
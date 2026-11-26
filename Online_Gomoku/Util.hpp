#pragma once

#include "Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <memory>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

class Mysql_Util
{
public:
    static MYSQL *Mysql_create(const std::string &host,
                               const std::string &user,
                               const std::string &password,
                               const std::string &database_name,
                               uint16_t port = 3306)
    {
        // 1.初始化mysql句柄
        MYSQL *mysql = mysql_init(nullptr);
        if (mysql == nullptr)
        {
            ELOG("mysql init failed!");
            return nullptr;
        }

        // 2.连接服务器
        // MTSQL* mysql_real_connect(mysql,host,user,password,database_name,port,unix_socket,flag);
        if (mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database_name.c_str(), 3306, nullptr, 0) == nullptr)
        {
            ELOG("mysql connect failed! , %s", mysql_error(mysql));
            mysql_close(mysql);
            return nullptr;
        }

        // 3.设置客户端字符集
        if (mysql_set_character_set(mysql, "utf8mb4") != 0)
        {
            ELOG("mysql set client character failed! , %s", mysql_error(mysql));
            mysql_close(mysql);
            return nullptr;
        }
        return mysql;
    }

    static bool Mysql_exec(MYSQL *mysql, const std::string &sql)
    {
        // 5.执行sql语句
        // int mysql_query(MYSQL* mysql,const char* sql);
        // const char *sql = "insert stu values(null,'xiaoming',18,80);";
        // const char *sql = "update stu set ch=ch+10 where id=1;";
        // const char *sql = "delete from stu where id=1;";
        // const char *sql = "select * from stu;";
        if (mysql_query(mysql, sql.c_str()) != 0)
        {
            ELOG("mysql query failed! , %s \n %s", mysql_error(mysql), sql.c_str());
            return false;
        }
        return true;
    }

    static void Mysql_close(MYSQL *mysql)
    {
        // 10.关闭连接，释放句柄
        if (mysql != nullptr)
            mysql_close(mysql);
    }
};

class Json_Util
{
public:
    static bool Serialize(const Json::Value &root, std::string &str)
    {
        // 实例化一个StreamWriterBuilder工厂类对象
        Json::StreamWriterBuilder swb;
        // 防止输出乱码
        swb["emitUTF8"] = true;
        // 利用StreamWriterBuilder工厂类对象生产一个StreamWriter对象
        std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
        // 使用StreamWriter对象进行序列化
        std::stringstream ss;
        int ret = sw->write(root, &ss);
        if (ret != 0)
        {
            ELOG("Json serialize failed!");
            return false;
        }
        str = ss.str();
        return true;
    }

    static bool Unserialize(const std::string &str, Json::Value &root)
    {
        // 实例化一个CharReaderBuilder工厂类对象
        Json::CharReaderBuilder crb;
        // 利用CharReaderBuilder工厂类对象生产一个CharReader对象
        std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
        // 使用CharReader对象进行反序列化
        std::string err;
        // parse(char* begin,char* end,Json::Value root,std::string err);
        bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), &root, &err);
        if (ret == false)
        {
            ELOG("Json unserialize failed: %s", err.c_str());
            return false;
        }
        return true;
    }
};

class String_Util
{
public:
    static int Split(const std::string &str, const std::string &sep, std::vector<std::string> &arr)
    {
        // 123,23,33,,,3
        size_t pos = 0;
        size_t i = 0;
        while (i < str.size())
        {
            pos = str.find(sep, i);
            if (pos == std::string::npos)
            {
                arr.push_back(str.substr(i));
                break;
            }
            if (pos == i)
            {
                i += sep.size();
                continue;
            }
            arr.push_back(str.substr(i, pos - i));
            i = pos + sep.size();
        }
        return arr.size();
    }
};

class File_Util
{
public:
    static bool Read(const std::string& filename,std::string& body)
    {
        //以二进制打开文件
        std::ifstream in(filename,std::ios::binary);
        if(!in.is_open())
        {
            ELOG("%s file open failed!",filename.c_str());
            return false;
        }
        //获取文件大小
        in.seekg(0,std::ios::end);//将读取位置移到文件末尾
        size_t fsize = in.tellg();
        in.seekg(0,std::ios::beg);//将读取位置移到文件开头
        //重置body大小
        body.resize(fsize);
        //写入数据
        in.read(&body[0],fsize);
        if(!in.good())
        {
            ELOG("read %s file content failed!",filename.c_str());
            in.close();
            return false;
        }
        //关闭文件
        in.close();
        return true;
    }
};
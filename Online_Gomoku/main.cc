#include "Logger.hpp"
#include "Util.hpp"
#include "db.hpp"
#include "Online.hpp"

#define HOST "127.0.0.1"
#define USER "root"
#define PASSWORD "fhp20051230@"
#define DBNAME "gomoku"
#define PORT 3306

void log_test()
{
    ILOG("hello1");
    DLOG("hello2");
    ELOG("hello3");
}

void mysql_test()
{
    MYSQL *mysql = Mysql_Util::Mysql_create(HOST, USER, PASSWORD, DBNAME, PORT);

    const std::string sql = "insert stu values(null,'小白',18,80);";
    if (!Mysql_Util::Mysql_exec(mysql, sql))
        return ;

    Mysql_Util::Mysql_close(mysql);
}

void json_test()
{
    Json::Value root;
    root["姓名"]="小马";
    root["年龄"]=17;
    std::string str;
    Json_Util::Serialize(root,str);
    DLOG("%s",str.c_str());

    Json::Value val;
    Json_Util::Unserialize(str,val);
    DLOG("%s",val["姓名"].asCString());
    DLOG("%d",val["年龄"].asInt());

}

void string_test()
{
    const std::string str = ",,,...,123,,,33,31,32,,,";
    std::vector<std::string> arr;
    const std::string sep=",";
    int n=String_Util::Split(str,sep,arr);
    for(auto s : arr)
        DLOG("%s",s.c_str());
    DLOG("%d",n);
}

void file_test()
{
    const std::string filename = "mysql.cpp";
    std::string body;
    File_Util::Read(filename,body);
    DLOG("%s",body.c_str());
}

void db_test()
{
    Table_user us(HOST, USER, PASSWORD, DBNAME, PORT);
    Json::Value user;
    //user["username"] = "小马";
    //user["password"] = "123223";
    us.Lose(3);
    // if(us.Select_by_name("小马",user))
    // {
    //     std::string body;
    //     Json_Util::Serialize(user,body);
    //     DLOG("%s",body.c_str());
    // }
}

void online_test()
{
    Online_manager om;
    wsserver_t::connection_ptr conn;
    uint64_t uid=2;
    om.Enter_game_room(uid,conn);
    if(om.Is_in_game_room(uid))
        DLOG("is in room");
    else
        DLOG("not in room");
    om.Exit_game_room(uid);
    if(om.Is_in_game_room(uid))
        DLOG("is in room");
    else
        DLOG("not in room");

}

int main()
{
    online_test();
    return 0;
}
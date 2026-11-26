#include <mutex>
#include <cassert>
#include "Util.hpp"

#define INSERT_USER_SQL "insert user values(null,'%s',sha2('%s',256),1000,0,0);"
#define LOGIN_USER_SQL "select id,score,total_count,win_count from user where username='%s' and password=sha2('%s',256);"
#define SELECT_BY_NAME_USER_SQL "select id,score,total_count,win_count from user where username='%s';"
#define SELECT_BY_ID_USER_SQL "select username,score,total_count,win_count from user where id=%ld;"
#define UPDATE_WIN_USER_SQL "update user set score=score+100,total_count=total_count+1,win_count=win_count+1 where id=%ld;"
#define UPDATE_LOSE_USER_SQL "update user set score=score-100,total_count=total_count+1 where id=%ld;"

// 封装user表的操作
class Table_user
{
public:
    Table_user(const std::string &host,
               const std::string &user,
               const std::string &password,
               const std::string &database_name,
               uint16_t port = 3306)
    {
        _mysql = Mysql_Util::Mysql_create(host, user, password, database_name, port);
        assert(_mysql != nullptr);
    }

    bool Insert(Json::Value &user) // 注册新用户时插入数据
    {
        char sql[4096];
        sprintf(sql, INSERT_USER_SQL, user["username"].asCString(), user["password"].asCString());
        bool ret = Mysql_Util::Mysql_exec(_mysql, sql);
        if (!ret)
        {
            DLOG("insert user info failed!");
            return false;
        }
        return true;
    }

    bool Login(Json::Value &user) // 登录验证，并返回用户信息
    {
        char sql[4096];
        sprintf(sql, LOGIN_USER_SQL, user["username"].asCString(), user["password"].asCString());
        MYSQL_RES *res = nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
            bool ret = Mysql_Util::Mysql_exec(_mysql, sql);
            if (!ret)
            {
                DLOG("login user failed!");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == nullptr)
            {
                DLOG("have no login user!");
                return false;
            }
        }
        int row_num = mysql_num_rows(res);
        if (row_num == 0)
        {
            DLOG("user info is not unique!");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["score"] = std::stoi(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"] = std::stoi(row[3]);
        mysql_free_result(res);
        return true;
    }

    bool Select_by_name(const std::string &name, Json::Value &user) // 通过用户名返回用户信息
    {
        char sql[4096];
        sprintf(sql, SELECT_BY_NAME_USER_SQL, name.c_str());
        MYSQL_RES *res = nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
            bool ret = Mysql_Util::Mysql_exec(_mysql, sql);
            if (!ret)
            {
                DLOG("get user by name failed!");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == nullptr)
            {
                DLOG("have no user!");
                return false;
            }
        }
        int row_num = mysql_num_rows(res);
        if (row_num == 0)
        {
            DLOG("user info is not unique!");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["username"] = name;
        user["score"] = std::stoi(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"] = std::stoi(row[3]);
        mysql_free_result(res);
        return true;
    }

    bool Select_by_id(uint64_t id, Json::Value &user) // 通过用户id返回用户信息
    {
        char sql[4096];
        sprintf(sql, SELECT_BY_ID_USER_SQL, id);
        MYSQL_RES *res = nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
            bool ret = Mysql_Util::Mysql_exec(_mysql, sql);
            if (!ret)
            {
                DLOG("get user by id failed!");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == nullptr)
            {
                DLOG("have no user!");
                return false;
            }
        }
        int row_num = mysql_num_rows(res);
        if (row_num == 0)
        {
            DLOG("user info is not unique!");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)id;
        user["username"] = row[0];
        user["score"] = std::stoi(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"] = std::stoi(row[3]);
        mysql_free_result(res);
        return true;
    }

    bool Win(uint64_t id) // 获胜时，天梯分数增加，总场次增加，胜场增加
    {
        char sql[4096];
        sprintf(sql, UPDATE_WIN_USER_SQL, id);
        bool ret = Mysql_Util::Mysql_exec(_mysql, sql);
        if (!ret)
        {
            DLOG("update win user info failed!");
            return false;
        }
        if (mysql_affected_rows(_mysql) == 0)//正确执行语句之后，表格没有发生更新，即找不到该id
        {
            DLOG("win user not exist!");
            return false;
        }
        return true;
    }

    bool Lose(uint64_t id) // 失败时，天梯分数减少，总场次增加，胜场不变
    {
        char sql[4096];
        sprintf(sql, UPDATE_LOSE_USER_SQL, id);
        bool ret = Mysql_Util::Mysql_exec(_mysql, sql);
        if (!ret)
        {
            DLOG("update lose user info failed!");
            return false;
        }
        if (mysql_affected_rows(_mysql) == 0)//正确执行语句之后，表格没有发生更新，即找不到该id
        {
            DLOG("lose user not exist!");
            return false;
        }
        return true;
    }

    ~Table_user()
    {
        Mysql_Util::Mysql_close(_mysql);
        _mysql = nullptr;
    }

private:
    MYSQL *_mysql;     // mysql操作句柄
    std::mutex _mutex; // 互斥锁保护数据库的操作安全
};
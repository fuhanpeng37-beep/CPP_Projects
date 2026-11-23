#include <iostream>
#include <string.h>
#include <mysql/mysql.h>

int main()
{
    // 1.初始化mysql句柄
    MYSQL *mysql = mysql_init(nullptr);
    if (mysql == nullptr)
    {
        std::cerr << "mysql init failed!" << std::endl;
        return -1;
    }

    // 2.连接服务器
    // MTSQL* mysql_real_connect(mysql,host,user,password,database_name,port,unix_socket,flag);
    if (mysql_real_connect(mysql, "127.0.0.1", "root", "fhp20051230@", "gomoku", 3306, nullptr, 0) == nullptr)
    {
        std::cerr << "mysql connect failed! , " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        return -1;
    }

    // 3.设置客户端字符集
    if (mysql_set_character_set(mysql, "utf8mb4") != 0)
    {
        std::cerr << "mysql set client character failed! , " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        return -1;
    }

    // 4.选择要操作的数据库
    // int mysql_select_db(MYSQL* mysql,const char* database_name);
    // 因为在第2步已经设置过了，所以这里就不用写了

    // 5.执行sql语句
    // int mysql_query(MYSQL* mysql,const char* sql);
    // const char *sql = "insert stu values(null,'xiaoming',18,80);";
    //const char *sql = "update stu set ch=ch+10 where id=1;";
    //const char *sql = "delete from stu where id=1;";
    const char *sql = "select * from stu;";
    if (mysql_query(mysql, sql) != 0)
    {
        std::cout << sql << std::endl;
        std::cerr << "mysql query failed! , " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        return -1;
    }

    // 6.如果sql语句为查询，则要把查询结果保存到本地
    //MYSQL_RES* mysql_store_result(MYSQL* mysql);
    MYSQL_RES* res = mysql_store_result(mysql);
    if(res == nullptr)
    {
        std::cerr << "mysql store result failed! , " << mysql_error(mysql) << std::endl;
        mysql_close(mysql);
        return -1;
    }

    // 7.获取结果集中的条数
    //int mysql_num_rows(MYSQL_RES* res);
    //int mysql_num_fields(MYSQL_RES* res);
    int num_row = mysql_num_rows(res);//行数
    int num_col = mysql_num_fields(res);//列数

    // 8.遍历保存到本地的结果集
    for(int i=0;i<num_row;i++)
    {
        MYSQL_ROW row = mysql_fetch_row(res); //获取一整行
        for(int j=0;j<num_col;j++)
        {
            std::cout << row[j] << "\t";
        }
        std::cout << std::endl;
    }

    // 9.释放结果集
    mysql_free_result(res);

    // 10.关闭连接，释放句柄
    mysql_close(mysql);

    return 0;
}

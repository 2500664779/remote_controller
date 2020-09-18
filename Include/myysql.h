/****************************************
 * 
 *           对mysql连接对象进行一个封装
 * 
 * ****************************************/
#ifndef __MYYSQL_H__
#define __MYYSQL_H__

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <cstdio>

#include "log.h"

#include <mysql/mysql.h>

// 故意多写一个y,也不知道取什么名字比较好...
class Myysql
{
public:
    Myysql();
    ~Myysql();
    bool Connect(std::string host, std::string user, std::string pwd, std::string dbname, unsigned int port = 3306);
    bool Query(std::string query);
    void Free();

private:
    // 初始化放到构造函数里,不需要暴露这个接口
    bool Init();

    MYSQL mysql;
    bool sql_init;

    // 暂时只需要update功能,所以不需要底下两个
    // MYSQL_RES *res;
    // MYSQL_ROW row;
};

#endif
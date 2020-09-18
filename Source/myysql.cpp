#include "myysql.h"

Myysql::Myysql()
    : sql_init(false)
{
    if (Init())
    {
        sql_init = true;
    }
    else
    {
        trace("mysql initialization failed\n");
    }
}

Myysql::~Myysql()
{
    Free();
}

bool Myysql::Init()
{
    // mysql_init成功时返回句柄,失败返回NULL
    return mysql_init(&mysql);
}

bool Myysql::Connect(std::string host, std::string user, std::string pwd, std::string dbname, unsigned int port)
{
    // mysql_real_connect成功返回连接句柄,和第一个参数的值一样,失败返回NULL

    // 只有初始化成功才有资格进行connect
    return sql_init && mysql_real_connect(&mysql, host.c_str(), user.c_str(), pwd.c_str(), dbname.c_str(), port, NULL, 0);
}

bool Myysql::Query(std::string query)
{
    // mysql_query返回值0表示成功
    // return !mysql_query(&mysql, query.c_str());
    return sql_init && (!mysql_query(&mysql, query.c_str()));
}

void Myysql::Free()
{
    mysql_close(&mysql);
}


// 数据库调用流程
// mysql_init
// mysql_real_connect
// mysql_query
// mysql_close
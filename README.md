# WebServer
用C++实现的高性能WEB服务器，经过webbenchh压力测试可以实现上万的QPS

## 环境要求
+ Linux
+ C++14
+ MySql

## 项目启动
需要先配置好对应的数据库
```sql
// 建立数据库
create database yourdb

// 创建user表
USE yourdb;
CREATE TABLE user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;

// 添加数据
INSERT INTO user(username, password) VALUES('name', 'password');
```

## 日志系统
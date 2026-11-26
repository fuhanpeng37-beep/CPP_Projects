/*可以使用mysql -uroot -p < db.sql 直接导入数据库*/
drop database if  exists gomoku;

create database if not exists gomoku;

use gomoku;

create table if not exists user(
    id int primary key auto_increment,
    username varchar(32) unique key not null,
    password varchar(64) not null,
    score int,
    total_count int,
    win_count int
);

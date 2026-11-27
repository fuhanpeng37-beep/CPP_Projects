#pragma once

#include "Util.hpp"
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> wsserver_t;

class Online_manager
{
public:
    Online_manager()
    {
    }

    void Enter_game_hall(uint64_t id, wsserver_t::connection_ptr &conn) // 玩家进入游戏大厅后建立websocket连接
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        //_hall_user[id] = conn;
        _hall_user.insert(std::make_pair(id,conn));
    }

    void Enter_game_room(uint64_t id, wsserver_t::connection_ptr &conn) // 玩家进入游戏房间后建立websocket连接
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        //_room_user[id] = conn;
        _room_user.insert(std::make_pair(id,conn));
    }

    void Exit_game_hall(uint64_t id) // 玩家退出游戏大厅后断开websocket连接
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        _hall_user.erase(id);
    }

    void Exit_game_room(uint64_t id) // 玩家退出游戏房间后断开websocket连接
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        _room_user.erase(id);
    }

    bool Is_in_game_hall(uint64_t id) // 判断该id玩家是否在游戏大厅
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        auto pos = _hall_user.find(id);
        if(pos == _hall_user.end())
            return false;
        return true;
    }

    bool Is_in_game_room(uint64_t id) // 判断该id玩家是否在游戏房间
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        auto pos = _room_user.find(id);
        if(pos == _room_user.end())
            return false;
        return true;
    }

    wsserver_t::connection_ptr Get_conn_from_game_hall(uint64_t id) // 从游戏大厅获取该id玩家的websocket连接指针
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        auto pos = _hall_user.find(id);
        if(pos == _hall_user.end())
            return wsserver_t::connection_ptr();
        return pos->second;
    }
    
    wsserver_t::connection_ptr Get_conn_from_game_room(uint64_t id) // 从游戏房间获取该id玩家的websocket连接指针
    {
        std::unique_lock<std::mutex> lock(_mutex); // 上互斥锁
        auto pos = _hall_user.find(id);
        if(pos == _hall_user.end())
            return wsserver_t::connection_ptr();
        return pos->second;
    }

    ~Online_manager()
    {
    }

private:
    std::mutex _mutex;
    std::unordered_map<uint64_t, wsserver_t::connection_ptr> _hall_user; // 游戏大厅里的玩家的id与websocket的映射
    std::unordered_map<uint64_t, wsserver_t::connection_ptr> _room_user; // 游戏房间里的玩家的id与websocket的映射
};

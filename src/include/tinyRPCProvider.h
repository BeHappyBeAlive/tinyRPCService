#pragma once
#include "google/protobuf/service.h"
#include <google/protobuf/descriptor.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include <functional>
#include <unordered_map>

//框架中用于对rpc服务进行网络发布的类
class TinyRPCProvider
{
public:
    //框架提供给外部使用，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);
    //启动rpc服务节点，开启提供rpc远程调用服务
    void Run();

private:
    //自定义socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr &conn);
    //自定义连接后用户操作读写的回调
    void OnMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buffer,
                   muduo::Timestamp timeStamp);
    //将响应信息发送给原来的连接上得函数
    void SendRPCResponseToSrcClientConn(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response);

private:
    muduo::net::EventLoop m_eventLoop;
    struct ServiceInfo
    {
        //保存服务对象
        google::protobuf::Service *m_service;
        //保存服务对象中的方法
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap;
    };
    //服务名称   服务信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
};
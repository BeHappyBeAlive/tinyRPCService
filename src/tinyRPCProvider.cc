#include "tinyRPCProvider.h"
#include "tinyRPCApplication.h"
#include "tinyRPCHeader.pb.h"

//框架提供给外部使用，可以发布rpc方法的函数接口
void TinyRPCProvider::NotifyService(google::protobuf::Service *service)
{
    const google::protobuf::ServiceDescriptor *desp = service->GetDescriptor();
    std::string serviceName = desp->name();
    std::cout << "service name:" << serviceName << std::endl;
    //存储服务信息
    ServiceInfo serviceInfo;
    for (int i = 0; i < desp->method_count(); i++)
    {
        const google::protobuf::MethodDescriptor *methodDes = desp->method(i);
        std::string methodName = methodDes->name();
        //存储方法信息
        serviceInfo.m_methodMap.insert({methodName, methodDes});
        std::cout << "\tmethod name:" << methodName << std::endl;
    }
    serviceInfo.m_service = service;
    m_serviceMap.insert({serviceName, serviceInfo});
}
//启动rpc服务节点，开启提供rpc远程调用服务
void TinyRPCProvider::Run()
{
    std::string rpcIP = TinyRPCApplication::getInstance().getTinyRPCConfig().LoadByKey("tinyRPCServer_ip");
    uint16_t rpcPort = std::atoi(TinyRPCApplication::getInstance().getTinyRPCConfig().LoadByKey("tinyRPCServer_port").c_str());
    muduo::net::InetAddress address(rpcIP, rpcPort);
    std::cout << "muduo server run on ip:" << rpcIP << ",port : " << rpcPort << std::endl;
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "tinyRPCProvider");
    //绑定连接回调和消息读写回调方法，分离了网络代码和业务代码  回调需要绑定参数
    server.setConnectionCallback(std::bind(&TinyRPCProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&TinyRPCProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置muduo库的现成数量
    server.setThreadNum(4);
    //启动网络服务
    server.start();
    m_eventLoop.loop();
}
//自定义socket连接回调
void TinyRPCProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // rpc和 client断开连接
        conn->shutdown();
    }
}

//将通过网络tcp传过来的请求字符串进行解析
//规定格式为header_size header_str args_str
// header为 service_name method_name args_size
// args_str 为方法的各个参数
//定义各个字符串的size是为了防止网络字符串黏包问题

void TinyRPCProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                                muduo::net::Buffer *buffer,
                                muduo::Timestamp timeStamp)
{
    std::string strBuffer = buffer->retrieveAllAsString();
    //读取header字符串的长度
    uint32_t headerSize;
    strBuffer.copy((char *)&headerSize, 4, 0);
    //截取protobuf字符串
    std::string rpcHeaderStr = strBuffer.substr(4, headerSize);
    // unpack 反序列化
    tinyRPC::RPCHeader rpcHeaderInfo;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    bool res = rpcHeaderInfo.ParseFromString(rpcHeaderStr);
    if (res)
    {
        service_name = rpcHeaderInfo.service_name();
        method_name = rpcHeaderInfo.method_name();
        args_size = rpcHeaderInfo.args_size();
    }
    else
    {
        // 数据头反序列化失败
        std::cout << "rpc_headerInfo_str:" << rpcHeaderStr << " parse error!" << std::endl;
        return;
    }
    //解析方法的参数
    std::string methodArgsStr = strBuffer.substr(4 + headerSize, args_size);

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << headerSize << std::endl;
    std::cout << "rpc_header_str: " << rpcHeaderStr << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << methodArgsStr << std::endl;
    std::cout << "============================================" << std::endl;
    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.m_service;
    //获取方法
    const google::protobuf::MethodDescriptor *method = mit->second;

    //生成rpc方法调用的请求request和响应response参数 通过获取的method获取其请求的proto封装message
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    //解析请求方法的参数
    // message LoginRequest
    // {
    //     bytes username = 1;
    //     bytes pwd=2;
    // }
    //解析参数失败
    if (!request->ParseFromString(methodArgsStr))
    {
        std::cout << "parse method " << method_name << " failed! args_str: " << methodArgsStr << std::endl;
        return;
    }
    //通过获取的method获取其函数响应的proto封装messagec
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //通过找到的服务调用其上面的对应方法，并且将返回结果通过原muduo::net::TcpConnectionPtr &conn发送回去
    //需要定义::google::protobuf::Closure* done的回调函数 即将response拿到 序列化后发送给原来的连接
    //首先定义一个函数用于发送业务
    //将此函数传入NewCallBack创建Closure回调
    // bug 识别第二个参数应该为Message*，但是却识别为Message*&类型，所以此处直接制定这个函数模板的参数来解决这一编译错误，
    // 原因为模板推演时候参数匹配错误
    google::protobuf::Closure *finishResponseCallBack = google::protobuf::NewCallback<TinyRPCProvider,
                                                                                      const muduo::net::TcpConnectionPtr &,
                                                                                      google::protobuf::Message *>(this,
                                                                                                                   &TinyRPCProvider::SendRPCResponseToSrcClientConn,
                                                                                                                   conn,
                                                                                                                   response);
    service->CallMethod(method, nullptr, request, response, finishResponseCallBack);
}
void TinyRPCProvider::SendRPCResponseToSrcClientConn(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string responseStr;
    bool res = response->SerializeToString(&responseStr);
    if (res)
    {
        conn->send(responseStr.c_str());
    }
    else
    {
        std::cout << "serialize method resposne failed，str: " << std::endl;
    }
    //模拟http短链接，关闭连接
    conn->shutdown();
}
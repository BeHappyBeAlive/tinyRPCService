#include "tinyRPCChannel.h"
#include "tinyRPCHeader.pb.h"
#include "tinyRPCApplication.h"
#include "tinyRPCController.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>

void TinyRPCChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                                google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                                google::protobuf::Message *response, google::protobuf::Closure *done)
{
    //将请求参数和网络请求服务端逻辑在此编写
    const google::protobuf::ServiceDescriptor *serviceDes = method->service();
    //服务名称
    std::string serviceName = serviceDes->name();
    //方法名称
    std::string methodName = method->name();

    //获取参数的序列化字符串 和 参数字符串长度 argStr argSize
    //参数字符串
    std::string argStr = "";
    //参数字符串长度
    uint32_t argSize = 0;
    if (request->SerializeToString(&argStr))
    {
        argSize = argStr.size();
    }
    else
    {
        // std::cout << "serialize request message error!" << std::endl;
        controller->SetFailed("serialize request message error!");
        return;
    }

    //拼接正确规则的请求字符串 format ： header_size header_str args_str
    //                       header_str : service_name method_name args_size
    // header_str
    tinyRPC::RPCHeader rpcHeader;
    rpcHeader.set_service_name(serviceName.c_str());
    rpcHeader.set_method_name(methodName.c_str());
    rpcHeader.set_args_size(argSize);

    // header_size
    uint32_t headerSize = 0;
    std::string rpcHeaderStr;
    if (rpcHeader.SerializeToString(&rpcHeaderStr))
    {
        headerSize = rpcHeaderStr.size();
    }
    else
    {
        // std::cout << "serialize rpcHeader message error!" << std::endl;
        controller->SetFailed("serialize rpcHeader message error!");
        return;
    }
    //开始进行组织字符串
    std::string sendRPCRequestStr;
    // headerSize
    sendRPCRequestStr.insert(0, std::string((char *)&headerSize, 4));
    sendRPCRequestStr += rpcHeaderStr;
    sendRPCRequestStr += argStr;

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << headerSize << std::endl;
    std::cout << "rpc_header_str: " << rpcHeaderStr << std::endl;
    std::cout << "service_name: " << serviceName << std::endl;
    std::cout << "method_name: " << methodName << std::endl;
    std::cout << "args_str: " << argStr << std::endl;
    std::cout << "============================================" << std::endl;

    //使用TCP编程完成rpc远程方法调用 客户端并不需要高并发 使用tcp连接发送即可
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientFd)
    {
        std::stringstream strStream;
        strStream << "create socket error! errno is:" << errno;
        controller->SetFailed(strStream.str());
        return;
    }
    // ip地址
    std::string rpcIP = TinyRPCApplication::getInstance().getTinyRPCConfig().LoadByKey("tinyRPCServer_ip");
    // port端口
    uint16_t rpcPort = std::atoi(TinyRPCApplication::getInstance().getTinyRPCConfig().LoadByKey("tinyRPCServer_port").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(rpcIP.c_str());
    server_addr.sin_port = htons(rpcPort);

    //连接rpc节点
    if (-1 == connect(clientFd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        close(clientFd);
        std::stringstream strStream;
        strStream << "connect to rpc Node failed! errno: " << errno;
        controller->SetFailed(strStream.str());
        return;
    }
    //连接成功之后，发送数据
    if (-1 == send(clientFd, sendRPCRequestStr.c_str(), sendRPCRequestStr.size(), 0))
    {
        close(clientFd);
        std::stringstream strStream;
        strStream << "send rpcRequestStr" << sendRPCRequestStr << " to rpc Node failed! errno: " << errno;
        controller->SetFailed(strStream.str());
        return;
    }

    //阻塞接受rpc的响应数据
    char resvBuffer[1024] = {0};
    int resvSize = 0;
    if (-1 == (resvSize = recv(clientFd, resvBuffer, 1024, 0)))
    {
        close(clientFd);
        std::stringstream strStream;
        strStream << "resv rpcResponseStr from rpc Node failed! errno: " << errno;
        controller->SetFailed(strStream.str());
        return;
    }
    //解析响应的字符串到Message
    // 反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size);
    // bug出现问题，char数组转为string中若遇到\0则截止，
    //后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str))
    std::string rpcResponseStr(resvBuffer, 0, resvSize);
    if (!response->ParseFromArray(resvBuffer, resvSize))
    {
        close(clientFd);
        std::stringstream strStream;
        strStream << "parse rpcResponseStr :" << rpcResponseStr << " from src responseStr failed! errno: " << errno;
        controller->SetFailed(strStream.str());
        return;
    }

    close(clientFd);
}
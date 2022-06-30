#include <iostream>
#include "tinyRPCApplication.h"
#include "user.pb.h"
#include "tinyRPCChannel.h"
#include "tinyRPCController.h"
int main(int argc, char **argv)
{
    //初始化RPC的框架，只需要初始化一次，单例，以便接下来的rpc服务的调用
    TinyRPCApplication::Init(argc, argv);
    //通过service_stub类去调用服务逻辑
    //需要一个RPCChannel的参数，后面执行服务方法的时候都会去channel中的CallMethod方法中去执行将
    // RPC的请求信息通过网络进行封装发送到服务端，所以网络call这块逻辑在RPCChannel的callMethod中进行编写
    zhurui::UserServiceRPC_Stub stub(new TinyRPCChannel());
    // service中rpc方法的请求
    zhurui::LoginRequest request;
    request.set_username("zhu");
    request.set_pwd("12345");
    // service中rpc方法的响应
    zhurui::LoginResponse response;
    // stub.serviceMethod()  RPCChannel的callMethod()中传入对应的函数进行逻辑编写 同步进行rpc服务中的方法调用
    // 重新实现控制器，对各种状态节点进行控制输出相应日志
    TinyRPCController rpcController;
    stub.Login(&rpcController, &request, &response, nullptr);
    // rpc调用失败
    if (rpcController.Failed())
    {
        std::cout << "rpc controller is failed , error message is :" << rpcController.ErrorText() << std::endl;
    }
    //调用成功，服务有响应
    else
    {
        //此次rpc调用完成
        if (0 == response.result().errcode())
        {
            //调用正确
            std::cout << "rpc login response:" << response.success() << std::endl;
        }
        else
        {
            //调用失败
            std::cout << "rpc login response:" << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}
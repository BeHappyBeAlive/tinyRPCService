#include <iostream>
#include "user.pb.h"
#include "tinyRPCApplication.h"
#include "tinyRPCProvider.h"
#include "tinyLogger.h"

/*
       服务发布方 即callee 提供服务供服务发起方caller进行调用 提供Login方法
*/
class UserService : public zhurui::UserServiceRPC
{
public:
    bool Login(std::string username, std::string pwd)
    {
        std::cout << "doing local service : Login on local desktop" << std::endl;
        std::cout << "username :" << username << "pwd : " << pwd << std::endl;
        return true;
    }
    // 重写基类方法  服务发起方  框架调用
    /*
        1. caller ==> Login(LoginRequest) ==> muduo ==> callee
        2. callee ==> 接受Login(LoginRequest)进行处理 ==> 调用下面的Login方法 ==> 结果通过muduo ==> caller
    */
    //相当于rpc service中的一个method 会有request和response
    void Login(::google::protobuf::RpcController *controller,
               const ::zhurui::LoginRequest *request,
               ::zhurui::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // rpc框架给Login方法提供了caller请求参数LoginRequest，然后在此处获取其中已经反序列化后的数据，
        //调用callee服务提供端的方法业务Login方法
        std::string username = request->username();
        std::string pwd = request->pwd();
        // callee端业务
        bool result = Login(username, pwd);

        //将响应给调用方返回
        zhurui::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("no error!");
        response->set_success(result);

        //执行回调操作  执行响应的数据进行序列化和网络发送
        done->Run();
    }
};

int main(int argc, char **argv)
{
    //连续的输出会导致日志类型识别错误
    LOG_INFO("first log message test!");
    LOG_ERR("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
    // //调用框架的初始化操作
    TinyRPCApplication::Init(argc, argv);
    TinyRPCProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();
    // //发布服务 UserService到rpc节点上
    //
    // provider.NotifyService(new UserService());

    // //启动一个rpc服务发布节点，Run后，进程进入阻塞状态，等待远程的rpc调用请求
    // provider.Run();

    return 0;
}
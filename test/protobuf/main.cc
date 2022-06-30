#include "test.pb.h"
#include <iostream>

using namespace zhurui;

int main()
{
#if 0
    LoginRequest req;
    req.set_name("zhurui");
    req.set_pwd("1234");
    std::string str;
    bool res=req.SerializeToString(&str);
    if (res)
    {
        /* code */
        std::cout<<str<<std::endl;
    }
    
    //反序列化
    LoginRequest resReq;
    if (resReq.ParseFromString(str))
    {
        /* code */
        std::cout<<resReq.name()<<std::endl;
        std::cout<<resReq.pwd()<<std::endl;
    }
#endif
#if 0
    LoginReponse rsp;
    ResponseCode *rc=rsp.mutable_result_code();
    rc->set_errcode(1);
    rc->set_errms("login failed");

    GetFriendListsResponse getRsp;
    ResponseCode* rc1=getRsp.mutable_result_code();
    rc1->set_errcode(0);

    user *user1=getRsp.add_frinedlist();
    user1->set_name("zhurui");
    user1->set_age(22);
    user1->set_sex(user::M);

    user *user2=getRsp.add_frinedlist();
    user2->set_name("zhurui");
    user2->set_age(23);
    user2->set_sex(user::M);

    std::cout<<getRsp.frinedlist_size()<<std::endl;
#endif
    return 0;
}
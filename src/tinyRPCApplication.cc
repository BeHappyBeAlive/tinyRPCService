#include "tinyRPCApplication.h"
#include <iostream>
#include <unistd.h>
#include <string>
//初始化静态成员变量
TinyRPCConfig TinyRPCApplication::m_config;

void RPCShowArgsHelp()
{
    std::cout << "format : commond -i <configuration>" << std::endl;
}

void TinyRPCApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        RPCShowArgsHelp();
        exit(EXIT_FAILURE);
        /* code */
    }
    int c = 0;
    std::string config_path;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        /* code */
        switch (c)
        {
        case 'i':
            //配置文件comfig.conf跟在了命令行后面
            config_path = optarg;
            break;
        //啥也没有
        case '?':
            std::cout << "invalid commond!" << std::endl;
            RPCShowArgsHelp();
            exit(EXIT_FAILURE);
            //有-i但是后面没有跟参数
        case ':':
            std::cout << "need <configuration>" << std::endl;
            RPCShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始加载配置文件
    m_config.LoadConfigFile(config_path.c_str());
}
TinyRPCApplication &TinyRPCApplication::getInstance()
{
    static TinyRPCApplication tinyApp;
    return tinyApp;
}

TinyRPCConfig& TinyRPCApplication::getTinyRPCConfig()
{
    return m_config;
}
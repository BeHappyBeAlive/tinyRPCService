#pragma once

#include "tinyRPCConfig.h"
// tinyRPC框架的初始化类  基础类
class TinyRPCApplication
{
public:
    static void Init(int argc, char **argv);
    static TinyRPCApplication &getInstance();
    static TinyRPCConfig& getTinyRPCConfig();

private:
    static TinyRPCConfig m_config;

private:
    TinyRPCApplication() {}
    TinyRPCApplication(const TinyRPCApplication &) = delete;
    //相当于const
    TinyRPCApplication(TinyRPCApplication &&) = delete;
};
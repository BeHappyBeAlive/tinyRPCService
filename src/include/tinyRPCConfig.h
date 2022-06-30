#pragma once

#include <unordered_map>
#include <string>

//框架读取配置文件类
class TinyRPCConfig
{
public:
    //从某个配置文件中读取配置信息
    void LoadConfigFile(const char *configFile_path);
    //根据键来获取其对应的值，从map中获取
    std::string LoadByKey(const std::string &key);

private:
    //存储rpc框架中需要配置的网络配置信息  tinyRPCServer_ip tinyRPCServer_port zooKeeper_ip zooKeeper_port
    std::unordered_map<std::string, std::string> m_ServerConfigMap;
    //去除字符串数据中多余的空格
    void trimStr(std::string &srcStr);
};
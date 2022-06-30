#include "tinyRPCConfig.h"
#include <iostream>
//从某个配置文件中读取配置信息
void TinyRPCConfig::LoadConfigFile(const char *configFile_path)
{
    FILE *file = fopen(configFile_path, "r");
    if (file == nullptr)
    {
        //文件打开失败
        std::cout << configFile_path << " is invalid path!" << std::endl;
        exit(EXIT_FAILURE);
        /* code */
    }
    //读取.conf文件中的内容，将其存储在map中
    //未到文件结尾
    while (!feof(file))
    {
        char buffStr[512] = {0};
        fgets(buffStr, 512, file);

        //去除读到的这个字符串的首尾空格并且跳过首字母为#的情况 如果含有=则表示为配置项
        std::string fileLineStr(buffStr);
        trimStr(fileLineStr);

        if (buffStr[0] == '#' || fileLineStr.empty())
        {
            continue;
        }
        //开始获取键值对
        int idx = fileLineStr.find('=');
        if (idx == -1)
        {
            continue;
        }

        std::string keyStr = fileLineStr.substr(0, idx);
        trimStr(keyStr);
        //排除末尾有换行符号的情况
        int endIdx = fileLineStr.find('\n', idx);
        std::string valueStr = fileLineStr.substr(idx + 1, endIdx - idx - 1);
        trimStr(valueStr);
        //直接插入二者的值会报错，make_pair后二者正确
        m_ServerConfigMap.insert(std::make_pair(keyStr, valueStr));
    }
}
//根据键来获取其对应的值，从map中获取
std::string TinyRPCConfig::LoadByKey(const std::string &key)
{
    auto iter = m_ServerConfigMap.find(key);
    if (iter == m_ServerConfigMap.end())
    {
        return "";
    }

    return iter->second;
}

void TinyRPCConfig::trimStr(std::string &srcStr)
{
    int idx = srcStr.find_first_not_of(' ');
    if (idx != -1)
    {
        //说明字符串前面有空格
        srcStr = srcStr.substr(idx, srcStr.size() - idx);
    }
    idx = srcStr.find_last_not_of(' ');
    if (idx != -1)
    {
        //说明字符串前面有空格
        srcStr = srcStr.substr(0, idx + 1);
    }
}
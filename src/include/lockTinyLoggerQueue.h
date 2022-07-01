#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <utility>

template <typename queueType>
class LockTinyLoggerQueue
{
public:
    //入队
    void Push(const queueType &str, int curLevel)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_LogQueue.push(str);
        m_logTypeQueue.push(curLevel);
        //有资源了通知写线程工作 注意此处如果写线程不唯一则notifyall
        m_conVariable.notify_one();
    }
    //出队返回队头元素
    std::pair<queueType, int> POP()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_LogQueue.empty())
        {
            //如果队列为空，则写线程不需要争夺锁资源 等待push线程往里面放入数据之后再执行
            m_conVariable.wait(lock);
        }
        //其中有数据，且拿到锁资源 开始往出拿数据
        queueType str = m_LogQueue.front();
        int curLogLevel = m_logTypeQueue.front();
        m_LogQueue.pop();
        m_logTypeQueue.pop();
        return std::make_pair(str, curLogLevel);
    }

private:
    //日志队列
    std::queue<queueType> m_LogQueue;
    //线程互斥锁
    std::mutex m_mutex;
    //线程间通信变量
    std::condition_variable m_conVariable;
    //日志类型的下标
    std::queue<int> m_logTypeQueue;
};
# mini_muduo: 一个多线程网络库

## 项目构成

* base库: 主要用于实现一个线程库和日志输出

* net库: 在使用base库的基础上上完成网络库

## 与muduo的差别
* 使用stl的mutex与condition_variable,而不是自己实现
* 对于线程池的任务队列,使用自己实现的线程安全队列而非deque

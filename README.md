# 概述

SGMessaging是一个UnrealEngine的消息系统插件，支持C++和蓝图双向发送和接收消息，基于UnrealEngine自带的MessageBus进行改造，采用消息总线设计，以及路由思想，拥有广播和点对点两种工作模式，提供了延时发送，跨线程/跨进程/跨端发送，消息转发，消息拦截器，消息调试器等功能。

# 文档

* [API](Docs/API.md)

# 技术

- TaskGraph
- Any参数
- 静态类型推导
- 自定义动态类型转换
- 泛型蓝图
- 模板偏特化

# TODO
- 支持UnLua
- 支持puerts

# 参考

[IMessageBus](https://docs.unrealengine.com/5.0/en-US/API/Runtime/Messaging/IMessageBus/)

[MessageBus](https://github.com/hejiajie1989/MessageBus)


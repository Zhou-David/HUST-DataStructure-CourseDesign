# 基于SAT的二进制数独游戏求解程序

#### 介绍
要求基于DPLL算法实现一个完备SAT求解器，对输入的CNF范式算例文件，解析并建立其内部表示；精心设计问题中变元、文字、子句、公式等有效的物理存储结构以及一定的分支变元处理策略，使求解器具有优化的执行性能；对一定规模的算例能有效求解，输出与文件保存求解结果，统计求解时间。

#### 软件架构
软件架构说明


#### 使用说明

## 1.  读取cnf文件
输入对应的处理文件（以.cnf结尾）绝对路径，或者将其放在与程序相同的文件夹下打开。
## 2.  遍历输出每个句子
输出“已经”读入的.cnf文件
## 3.  DPLL求解算例并保存
进行SAT求解，并将结果以同名+.res结尾的形式保存
## 4.  二进制数独游戏
在有求解器的条件下，可以创建一个二进制的数独游戏，并求其解。

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)

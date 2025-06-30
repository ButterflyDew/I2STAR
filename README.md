# Improved2star 项目说明

## 目录结构

```
.
├── include/           # 头文件目录，所有核心模块声明
│   ├── BinaryHeap.hpp     # 原始二叉堆（STL风格）
│   ├── StaticHeap.hpp     # 静态堆，支持下标定位和权值修改
│   ├── Graph.hpp          # 图结构与读入
│   ├── GlobalUtils.hpp    # 全局工具、计时、Tree等
│   ├── GSTCover.hpp       # 组斯坦纳树相关算法
│   └── Log.hpp            # 日志系统
├── src/               # 源文件目录
│   ├── GlobalUtils.cpp
│   └── Log.cpp
├── tests/             # 单元测试
│   ├── test_Heap_all.cpp      # 二叉堆与静态堆的功能与暴力测试
│   ├── test_GSTCover.cpp      # GSTCover算法测试
│   ├── test_graph_and_query.cpp # 图与询问读入测试
│   └── test_Log.cpp           # 日志系统测试
├── data/              # 数据集目录（如 LinkedMDB、Toronto 等）
├── main.cpp           # 主程序入口
├── Makefile           # 编译脚本
└── README.md          # 项目说明（本文件）
```

---

## 编译与测试

### 依赖

- C++20 编译器（如 g++ 10+）
- 推荐环境：Linux/macOS/WSL/MinGW

### 编译并运行所有单元测试

```sh
make test_heap_all
./test_heap_all

make test_graph_query
./test_graph_query <graphname>   # 例如 ./test_graph_query Toronto

make test_gstcover
./test_gstcover <graphname>  # 例如 ./test_graph_query Toronto

make test_log
./test_log
```

**其实只需要关注  test_gstcover即可**

### 清理编译产物

```sh
make clean
```

---

## 主要模块说明

**模块 5 是实际代码实现，其他都不是很重要，看下接口即可。**

### 1. BinaryHeap（原始二叉堆）

- 头文件：`include/BinaryHeap.hpp`
- 功能：支持 push、pop、top、empty、size，类似 STL `priority_queue`，不支持批量建堆和权值修改。

### 2. StaticHeap（静态堆）

- 头文件：`include/StaticHeap.hpp`
- 功能：支持用 1-indexed 静态序列批量建堆，支持下标定位和权值修改，适合静态场景。

### 3. Graph

- 头文件：`include/Graph.hpp`
- 功能：支持无向带权图的读入与存储，节点编号 1-index，支持邻接表和边权查询。

### 4. GlobalUtils

- 头文件：`include/GlobalUtils.hpp`
- 功能：
  - 全局参数与路径管理
  - 计时工具（Timer）：支持命名计时段，自动输出到日志
  - Tree 模板类：支持带权无向树的边管理与权值统计
  - 图与询问的读入

### 5. GSTCover

- 头文件：`include/GSTCover.hpp`
- 功能：组斯坦纳树相关算法与接口

### 6. Log

- 头文件：`include/Log.hpp`
- 功能：支持多级别日志（DEBUG/INFO/WARN/ERROR），可输出到控制台和文件，支持编译时开关
- 可以参考 test_GSTCover.cpp 第 14 行，设置为 LOG_DEBUG 后会在控制台中输出 Log::debug("") 的内容，否则不会。

---

## 数据格式说明

- `data/<dataset>/graph.txt`：第一行为 n m，后续每行 u v w 表示无向带权边
- `data/<dataset>/query.txt`：第一行为 q，后续每组询问格式见 GlobalUtils.hpp 注释

---

## 计时与日志用法

```cpp
Timer::start("mytask");
// ... 代码 ...
Timer::stop("mytask", LogLevel::LOG_INFO); // 结束计时并输出到日志
```

---

## 贡献与扩展

- 所有核心算法和数据结构均有单元测试，详见 `tests/` 目录。
- 欢迎扩展更多算法、数据结构和测试用例！

---

如需详细接口文档或有其他问题，欢迎提 issue 或联系作者。 
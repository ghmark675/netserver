# select缺点

- 采用轮询方式扫描bitmap，性能会随着socket数量增多而下降
- 每次调用select()，需要拷贝bitmap
- bitmap的大小（单个进程/线程打开的socket数量）由`FD_SETSIZE`宏设置，默认1024个，可以修改，但是效率将更低
- 将进程从socket等待队列中加入、删除也有较大时间开销

# 前言

select性能仍然很低，有很大优化空间，于是有了优化版本poll

但其实poll并没优化太多，所以只是简单介绍，不写代码

# poll主要优化点

- 使用动态数组 pollfd，没有固定大小限制，能够支持更多的文件描述符。
- 支持更细粒度的事件类型（如 POLLIN, POLLOUT, POLLERR, 等等），可以更好地处理各种 I/O 情况。
- poll：直接使用 pollfd 数组，每次调用前不需要重置，只需修改需要关注的部分即可。

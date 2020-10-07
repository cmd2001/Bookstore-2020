# 一点小小的提示
先不考虑文档的问题，首先对程序有一个通盘的设计。

假设你拿到手的是一道传统的 OI 大模拟题，数据也仅仅要求存储于内存，你会怎么设计程序？

然后再考虑文件的存储。首先所有信息都是以**定长**的形式存储在**二进制文件**中，那么显然，你需要提前设定好每个需要存储的元素的存储方式（其内部信息如何排列，总共占多少字节）。

其次，由于外存读写的时间开销远大于内存，内存的数据结构并不能有效地应用于外存上。这里推荐使用块状链表（每个文件是一个 block，程序运行时可以将文件索引直接存入内存来加速，在 block 内寻找时一次性读取整个文件入内存进行查找），有余力的同学可以提前学习下学期你们会用到的 B+ 树。

现在你可以开始思考如何使得自己的程序变得模块化了。首先将实现不同功能的代码拆分到不同的函数中。然后，再将函数按照其所处理的对象分类（例如图书、用户，或者是外存数据结构等等），同类的函数可以放入同一个 `class`（OI 中你可能更经常用 `struct`）中（例如，买书、进货、修改书目信息都可以放进一个“图书的管理类”中，类似地，用户的功能可以放进一个“用户的管理类中”）。

将这些划分一一写下来，你就拥有了一份能给自己看的初步文档。若想要将它转变为一份给别人看的设计文档，你需要完善其中的细节，例如类内具体成员，函数的参数，函数的功能和代码实现方式，前面所提到的文件相关的内容等等。

最后自顶向下 (DFS) 阅读一遍你的文档，看看是否能按着它实现出整个应用。
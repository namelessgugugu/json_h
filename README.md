## json_h
------

### 简介

这是我自己写的用于读写json文件的头文件，没有经过太多的测试，出现的问题可能有：
* 内存泄漏
* 迭代器越界
* 读入/输出错误

### 用法

包含两个类：`json_object`和`json_array`，分别是json的对象和数组。和一个枚举类型`type_of_value`，详细内容见代码。

其中，只有对象拥有迭代器，这是为了更好的遍历每一个值，而数组没有，因为遍历数组并不需要迭代器，而且我懒。
 #### `json_object`
* `void read(const char* file)` 用于读取符合json对象格式的字符串`file`并存储。在初始化时传入这个字符串有一样的效果。
* `char* to_str()` 将类转化成字符串并返回，注意，返回的字符串需要`delete`。键-值按照键的字典序升序排序。
* `void insert(const char* key, T value)` 插入/修改键为`key`的值。支持长整型，浮点数，字符串以及json的对象和数组，如果不填写`value`，则值为`null`，下文数组的`insert`函数同理。
* `std::pair<void*, type_of_value> at(const char* key)` 查询并返回键`key`对应的值。返回的`pair`的`first`与`second`表示值的地址和类型，如果出错返回`{NULL,Error}`，下文数组的`at`函数同理。
* 迭代器（`iterator`） 有`begin()`和`end()`两个函数，分别是指向第一个值的迭代器和指向最后一个值的迭代器的下一个迭代器。重载了`++,--,==,!=`，指向一个包含三个函数`const char* key(),void* value(),type_of_value typ()`的类，分别是键、值的地址和类型。

#### `json_array`
* `void read(const char* file)` 用于读取符合json数组格式的字符串`file`并存储。在初始化时传入这个字符串有一样的效果。
* `char* to_str()` 将类转化成字符串并返回，注意，返回的字符串需要`delete`。未初始化的数组部分输出`null`。
* `void insert(T value, int key)` 插入/修改下标为`key`的值。详细说明同上。
* `std::pair<void*, type_of_value> at(int key)` 查询并返回下标`key`对应的值。详细说明同上。

#### 其他
* 有一个bool型全局变量`bracket_with_enter`表示`{}[]`是否换行，默认为0，即不换行。

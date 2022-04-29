# C++11 相关知识点
## std::function
### std::function 简介
类模板std :: function是一个通用的多态函数包装器。 std :: function的实例可以存储，复制和调用任何可调用的目标 ：包括函数，lambda表达式，绑定表达式或其他函数对象，以及指向成员函数和指向数据成员的指针。当std::function对象未包裹任何实际的可调用元素，调用该std::function对象将抛出std::bad_function_call异常。
### function类模板
```C++
#include <functional>
template<class R, class... Args>
class function<R(Args...)>;
```
模板参数说明：
+ R：被调用函数的返回类型
+ Args：被调用函数的形参  

示例：
function类的实例 func 可以指向 返回值为 int 型，有两个形参都为 int 型的函数
```C++
#include <functional>
#include <iostream>
int f(int a, int b)
{
  return a + b;
}
int main()
{
	std::function<int(int, int)>func = f;
	cout << f(1, 2) << endl;      // 3
	system("pause");
	return 0;
}
```

## 智能指针

### std::unique_ptr
+ 任意时刻该指针智能指向某一个对象，指针销毁时，指向的对象也会被删除（通过内置删除器调用析构函数删除）
+ 禁止拷贝和赋值，可使用std::move()、unique_ptr.reset()转移指针控制权。

## 多线程

### lock_guard
互斥量包装程序，它提供了一种方便的RAII（Resource acquisition is initialization）风格的机制来在作用域块的持续时间内拥有一个互斥量。
+ 创建即加锁，作用域结束自动析构并解锁，无需手工解锁
+ 不能中途解锁，必须等作用域结束才解锁
+ 不能复制

## 条件变量(condition_variable)
条件变量是利用线程间共享的全局变量同步的一种机制，主要包括两个动作：一个线程等待条件变量的条件成立而挂起；另一个线程使条件成立

### notify_one()
只唤醒等待队列中的第一个线程；不存在锁竞争，能够立即获得锁。

### notify_all()
唤醒等待队列中所有阻塞的的线程，存在锁竞争，只有一个线程能获得锁。
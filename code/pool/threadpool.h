#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <assert.h>
#include <thread>
#include <functional>


/**
 * 线程池，Reactor方式
*/
class ThreadPool
{
private:
	struct Pool {
		std::mutex mtx; // 互斥锁变量
		std::condition_variable cond; // 条件变量， 实现多个线程间的同步操作
		bool isClosed;
		std::queue<std::function<void()>> tasks; // 多态函数包装器
	};
	std::shared_ptr<Pool> pool_; // shared_ptr 允许多个指针指向同一个对象
public: 
	ThreadPool() = default;

	ThreadPool(ThreadPool&&) = default;

	~ThreadPool() {
		// ？强转 Pool 类型指针为 bool 型
		if(static_cast<bool>(pool_)) {
			std::lock_guard<std::mutex> locker(pool_->mtx);
			pool_->isClosed = true;
		}
		pool_->cond.notify_all();
	}

	explicit ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()) {
		// 被 explicit 关键字修饰的类构造函数，不能进行自动地隐式类型转换，
		// 只能显式地进行类型转换
		assert(threadCount > 0);
		for(size_t i = 0; i < threadCount; i++) {
			std::thread([pool = pool_] {
				std::unique_lock<std::mutex> locker(pool->mtx);
				while (true) {
					if(!pool->tasks.empty()) {
						auto task = std::move(pool->tasks.front());
						pool->tasks.pop();
						locker.unlock();
						task();
						locker.lock();
					}
					else if(pool->isClosed) break;
					else pool->cond.wait(locker);
				}
				
			}).detach();
		}
	}

	template<class F>
	void AddTask(F&& task) {
		{
			std::lock_guard<std::mutex> locker(pool_->mtx);
			pool_->tasks.emplace(std::forward<F>(task));
		}
		pool_->cond.notify_one();
	}
};

#endif
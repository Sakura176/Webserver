#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/uio.h>
#include <vector>
#include <atomic> 
#include <assert.h>

class Buffer
{
private:
	char *BeginPtr_();
	const char *BeginPtr_() const;
	void MakeSpace_(size_t len);

	std::vector<char> buffer_;
	// std::atomic 对int, char, bool等数据结构进行原子性封装，
	// 在多线程环境中，对std::atomic对象的访问不会造成竞争-冒险。
	// 利用std::atomic可实现数据结构的无锁设计。
	std::atomic<std::size_t> readPos_; // 原子数据类型， 无需使用互斥锁处理
	std::atomic<std::size_t> writePos_;

public:
	Buffer(int initBuffSize = 1024);
	~Buffer() = default;

	size_t WritableBytes() const; // ? 可写字节
	size_t ReadableBytes() const; // ？ 可读字节
	size_t PrependableBytes() const; // ？

	const char *Peek() const;
	void EnsureWriteable(size_t len);
	void HashWritten(size_t len);

	void Retrieve(size_t len);
	void RetrieveUntil(const char *end);

	void RetrieveAll();
	std::string RetrieveAllToStr();

	const char *BeginWriteConst() const;
	char *BeginWrite();

	void Append(const std::string &str);
	void Append(const char *str, size_t len);
	void Append(const void *data, size_t len);
	void Append(const Buffer &buff);

	ssize_t ReadFd(int fd, int *Errno);
	ssize_t WriteFd(int fd, int *Errno);
};

#endif //BUFFER_H
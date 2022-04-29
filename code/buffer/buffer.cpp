#include "buffer.h"

/**
 * 构造函数， 初始化buffer大小， 设定读写位置为0
*/
Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0) {}

/**
 * 剩余可读的字节数
*/
size_t Buffer::ReadableBytes() const {
	return writePos_ - readPos_;
}

/**
 * 计算可写入的字节数
*/
size_t Buffer::WritableBytes() const {
	return buffer_.size() - writePos_;
}

size_t Buffer::PrependableBytes() const {
	return readPos_;
}

/**
 * 返回缓冲区到已读位置的字符串 ？
*/
const char* Buffer::Peek() const {
	return BeginPtr_() + readPos_;
}

void Buffer::Retrieve(size_t len) {
	assert(len <= ReadableBytes());
	readPos_ += len;
}

void Buffer::RetrieveUntil(const char* end) {
	assert(Peek() <= end );
	Retrieve(end - Peek());
}

/**
 * 恢复整个缓冲区， 将读写指针都置为 0
*/
void Buffer::RetrieveAll() {
	// 取地址缓冲区中首个字符的地址
	bzero(&buffer_[0], buffer_.size()); // 将字符串的前 n 个字节置为 0
	readPos_ = 0;
	writePos_ = 0;
}

std::string Buffer::RetrieveAllToStr() {
	std::string str(Peek(), ReadableBytes());
	RetrieveAll();
	return str;
}

const char* Buffer::BeginWriteConst() const {
	return BeginPtr_() + writePos_;
}

char *Buffer::BeginWrite() {
	return BeginPtr_() + writePos_;
}

void Buffer::HashWritten(size_t len) {
	writePos_ += len;
}

void Buffer::Append(const std::string& str) {
	Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len) {
	assert(data);
	Append(static_cast<const char *>(data), len);
}

/**
 * 增加缓冲区空间并写入
*/
void Buffer::Append(const char* str, size_t len) {
	assert(str);
	EnsureWriteable(len);
	std::copy(str, str + len, BeginWrite());
	HashWritten(len);
}

void Buffer::Append(const Buffer& buff) {
	Append(buff.Peek(), buff.ReadableBytes());
}

/**
 * 确定缓冲区空间是否足够，若不够则申请空间
*/
void Buffer::EnsureWriteable(size_t len) {
	if(WritableBytes() < len) {
		MakeSpace_(len);
	}
	assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
	char buff[65535];
	struct iovec iov[2]; // 定义一个向量元素， 用作一个多元素的数组
	const size_t writable = WritableBytes();
	/* 分散度， 保证数据全部读完 */
	iov[0].iov_base = BeginPtr_() + writePos_;
	iov[0].iov_len = writable;
	iov[1].iov_base = buff;
	iov[1].iov_len = sizeof(buff);

	// 分散读，读取多元素数组，返回成功读入的字节数
	const ssize_t len = readv(fd, iov, 2);
	// 读取字节数小于 2，错误
	if(len < 2) {
		*saveErrno = errno;
	}
	// 若读取字节数小于可写长度， 则移动写位置
	else if(static_cast<size_t>(len) <= writable) {
		writePos_ += len;
	}
	// 若字节数大于可写长度，则移动写位置至缓冲区末尾
	else {
		writePos_ = buffer_.size();
		Append(buff, len - writable);
	}
	return len;
}

/**
 * 写入句柄，
 * fd: 映射文件的描述符
*/
ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
	size_t readSize = ReadableBytes();
	ssize_t len = write(fd, Peek(), readSize);
	if(len < 0) {
		*saveErrno = errno;
		return len;
	}
	readPos_ += len;
	return len;
}

char* Buffer::BeginPtr_() {
	return &*buffer_.begin();
}

const char* Buffer::BeginPtr_() const {
	return &*buffer_.begin();
}

void Buffer::MakeSpace_(size_t len) {
	if(WritableBytes() + PrependableBytes() < len) {
		buffer_.resize(writePos_ + len + 1);
	}
	else {
		size_t readable = ReadableBytes();
		std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
		readPos_ = 0;
		writePos_ = readPos_ + readable;
		assert(readable == ReadableBytes());
	}
}
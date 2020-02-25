#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__
#include <string>

// 文件上传下载类
class FileTransfer {
public:
	void upload(const std::string& filename, int fd, int flag);
	void download(const std::string& filename, int fd, int flag);
};

#endif

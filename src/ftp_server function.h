#ifndef FTP_SERVER_MODULE_H
#define FTP_SERVER_MODULE_H

#include <string>

// برای استفاده از کتابخانه Winsock در برنامه
#include <winsock2.h>

// تعریف کلاس سرور FTP
class FTPServer {
public:
    FTPServer(int serverPort);  // سازنده برای راه‌اندازی سرور
    void initiate();  // شروع به کار سرور
    void processClient(int clientSock);  // پردازش درخواست‌های کاربر

    // توابع مدیریت فایل‌های FTP
    void upload(int clientSock, const std::string& fileName);
    void download(int clientSock, const std::string& fileName);
    void removeFile(int clientSock, const std::string& fileName);
    void findFile(int clientSock, const std::string& fileName);
    void showFiles(int clientSock);

private:
    int mainSocket;  // سوکت اصلی سرور
    int serverPort;  // پورت سرور
    void transmitMessage(int clientSock, const std::string& msg);  // ارسال پیام به کاربر
    std::string retrieveMessage(int clientSock);  // دریافت پیام از کاربر
};

#endif

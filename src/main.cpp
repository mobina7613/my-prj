#include "ftp_server.h"
#include <iostream>

int main() {
    // مقداردهی به پورت برای اتصال سرور (پورت پیش‌فرض: 54000)
    int serverPort = 54000;
    
    // ایجاد یک شیء از کلاس FTPServer
    FTPServer ftpServer(serverPort);
    
    // شروع به کار سرور
    ftpServer.start();

    return 0;
}

#include "ftp_server.h"
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")  // پیوند دادن کتابخانه winsock

FTPServer::FTPServer(int serverPort) : serverPort(serverPort) {
    // راه‌اندازی Winsock
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    // ساخت سوکت سرور
    mainSocket = socket(AF_INET, SOCK_STREAM, 0);

    // تنظیمات آدرس سرور
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(serverPort);

    // متصل کردن سوکت به آدرس
    bind(mainSocket, (sockaddr*)&address, sizeof(address));

    // قرار دادن سرور در حالت گوش دادن
    listen(mainSocket, SOMAXCONN);

    std::cout << "FTP Server is now running on port " << serverPort << std::endl;
}

void FTPServer::initiate() {
    while (true) {
        sockaddr_in clientAddress;
        int addrSize = sizeof(clientAddress);
        int clientSock = accept(mainSocket, (sockaddr*)&clientAddress, &addrSize);
        
        // پردازش درخواست‌های کاربر
        processClient(clientSock);
    }
}

void FTPServer::processClient(int clientSock) {
    std::string clientCommand = retrieveMessage(clientSock);
    if (clientCommand == "UPLOAD") {
        std::string fileName = retrieveMessage(clientSock);
        upload(clientSock, fileName);
    } else if (clientCommand == "DOWNLOAD") {
        std::string fileName = retrieveMessage(clientSock);
        download(clientSock, fileName);
    } else if (clientCommand == "DELETE") {
        std::string fileName = retrieveMessage(clientSock);
        removeFile(clientSock, fileName);
    } else if (clientCommand == "SEARCH") {
        std::string fileName = retrieveMessage(clientSock);
        findFile(clientSock, fileName);
    } else if (clientCommand == "LIST") {
        showFiles(clientSock);
    }
    closesocket(clientSock);
}

void FTPServer::upload(int clientSock, const std::string& fileName) {
    std::ofstream fileOutput(fileName, std::ios::binary);
    char buffer[1024];
    int bytesRead;
    while ((bytesRead = recv(clientSock, buffer, sizeof(buffer), 0)) > 0) {
        fileOutput.write(buffer, bytesRead);
    }
    fileOutput.close();
    transmitMessage(clientSock, "File uploaded successfully.");
}

void FTPServer::download(int clientSock, const std::string& fileName) {
    std::ifstream fileInput(fileName, std::ios::binary);
    if (!fileInput) {
        transmitMessage(clientSock, "File not found.");
        return;
    }

    char buffer[1024];
    while (fileInput.read(buffer, sizeof(buffer))) {
        send(clientSock, buffer, sizeof(buffer), 0);
    }
    fileInput.close();
    transmitMessage(clientSock, "File downloaded successfully.");
}

void FTPServer::removeFile(int clientSock, const std::string& fileName) {
    if (fs::remove(fileName)) {
        transmitMessage(clientSock, "File deleted successfully.");
    } else {
        transmitMessage(clientSock, "File not found.");
    }
}

void FTPServer::findFile(int clientSock, const std::string& fileName) {
    if (fs::exists(fileName)) {
        transmitMessage(clientSock, "File found.");
    } else {
        transmitMessage(clientSock, "File not found.");
    }
}

void FTPServer::showFiles(int clientSock) {
    std::string allFiles;
    for (const auto& entry : fs::directory_iterator(".")) {
        allFiles += entry.path().string() + "\n";
    }
    transmitMessage(clientSock, allFiles);
}

void FTPServer::transmitMessage(int clientSock, const std::string& msg) {
    send(clientSock, msg.c_str(), msg.size(), 0);
}

std::string FTPServer::retrieveMessage(int clientSock) {
    char buffer[1024];
    int receivedBytes = recv(clientSock, buffer, sizeof(buffer), 0);
    return std::string(buffer, receivedBytes);
}

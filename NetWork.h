//
// Created by ZZDirty on 2019/12/10.
//

#ifndef _NETWORK_H
#define _NETWORK_H



#pragma comment(lib, "ws2_32.lib")

#include <sstream>
#include <iostream>
#include <string>
#include <winsock.h>
#include <vector>
#include <regex>
#include "json.hpp"

typedef struct Danmu {
	std::string color;
	std::string size;
	std::string id;
	std::string text;
	std::string time;
} DANMU;

std::string getStringFromSocket() {
	WSADATA w;
	WORD ver = MAKEWORD(2, 2);
	int err;
	err = WSAStartup(ver, &w);
	SOCKET sock;
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("192.144.231.249");
	server_addr.sin_port = htons(8080);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sock, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		std::cout << "connect failed";
	}
	else {
		std::cout << "connect success\r\n";
	}
	//"GET /Danmu_war_exploded/get HTTP/1.1\r\n"
	char send_buf[1024] = "GET /Danmu_war_exploded/get HTTP/1.1\n"
		"Host: 192.144.231.249:8080\n"
		"Connection: keep-alive\n"
		"Cache-Control: max-age=0\n"
		"Upgrade-Insecure-Requests: 1\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36\n"
		"Accept: textml,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\n"
		"Referer: http://192.144.231.249:8080/Danmu_war_exploded/get\n"
		"Accept-Encoding: gzip, deflate\n"
		"Accept-Language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7\n\n";
	char* recv_buf = new char[1024 * 1024 * 5];
	memset(recv_buf, 0, 1024 * 1024 * 5);
	int send_flag = send(sock, send_buf, strlen(send_buf), 0);
	if (send_flag < 0) {
		std::cout << "sent failed\r\n";
	}
	else {
		std::cout << "sent success\r\n";
	}

	std::stringstream ss;

	recv(sock, recv_buf, 2000, 0);
	ss << recv_buf;

	/*do {
		int i = recv(sock, recv_buf, 1024 * 1024 * 5, 0);
		if (i < 1024 * 1024 * 5) {
			ss << recv_buf;
			break;
		}
		else {
			ss << recv_buf;
		}
	} while (1);*/


	delete recv_buf;
	closesocket(sock);
	WSACleanup();
	return ss.str();
}

void getDanmu(std::vector<DANMU>& ds) {
	std::string str = getStringFromSocket();
	std::cout << str;
	std::vector<std::string> vs;
	std::istringstream iss(str);
	std::string temp;
	while (getline(iss, temp)) {
		if (temp.length() != 0) {
			vs.push_back(temp);
		}
	}
	try {
		if (str.length() >= 90) {
			for (int i = 0; i < vs.size(); i++) {
				if (vs[i].length() > 0 && vs[i][0] == '{') {
					nlohmann::json j = nlohmann::json::parse(vs[i]);
					DANMU danmu = {
							j["color"].get<std::string>(),
							"",
							"",
							j["text"].get<std::string>(),
								""
					};
					ds.push_back(danmu);
				}
			}
		}
	}
	catch (...) {
		return;
	}

}

inline BYTE getHexNum(const char c) {
	if (c >= 'A' && c <= 'F')//十六进制还要判断字符是不是在A-F或者a-f之间
	{
		return c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	else {
		return c - '0';
	}
}


#endif
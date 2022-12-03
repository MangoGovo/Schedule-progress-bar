#include <iostream>
#include <ctime>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <graphics.h>
#include <fstream>
#include <json/json.h>
#include <strstream>

#include "tool.hpp"

#define BUFFER_SIZE 100
using namespace std;
void init();					//初始化方法
void getProfile();				//获取配置文件
bool isFileExists(string&);		//判断文件是否存在
void readJson();				//从Json中读取课表
int sWidth;						//屏幕宽度
int sHeight;					//屏幕高度
int wHeight = 3;				//窗口高度
int classLen = 0;				//课表长度
string configName = "Config.ini";	//配置文件目录

struct mClass {
	string name = "classname";
	string start_time;
	string end_time;
}classes[100];
void init() {
	HDC hdc = GetDC(NULL);
	sWidth = GetDeviceCaps(hdc, DESKTOPHORZRES);
	sHeight = GetDeviceCaps(hdc, DESKTOPVERTRES);
}


bool isFileExists(string& name) {
	ifstream f(name.c_str());
	return f.good();
}
void getProfile() {
	TCHAR buffer[BUFFER_SIZE] = { 0 };
	if (isFileExists(configName)) {
		//读
		wHeight = GetPrivateProfileInt(L"Settings", L"line_width", 3, L"Config.ini");
	}
	else {
		//写
		WritePrivateProfileString(L"Settings",  // 节名称
			L"line_width", // 配置项名称
			L"3", // 欲写入的值
			L".\\Config.ini"); // 配置文件名
	}

}

void readJson() {
	// 1. 将磁盘文件中的json字符串读到磁盘文件
	ifstream ifs("class.json");

	// 2. 反序列化 -> value对象
	Json::Value root;
	Json::Reader r;
	r.parse(ifs, root);

	classLen = root.size();
	// 3. 从value对象中将数据依次读出
	for (int i = 0; i < root.size(); i++)
	{
		Json::Value::Members keys = root[i].getMemberNames();

		classes[i].name = root[i][keys[1]].asString();
		classes[i].start_time = root[i][keys[2]].asString();
		classes[i].end_time = root[i][keys[0]].asString();
	}
	for (int i = 0; i < root.size(); i++)
	{
		time_t nowtime = time(NULL);
		struct tm tm_t;
		localtime_s(&tm_t, &nowtime);
		char buffer_[100];
		ostrstream strout(buffer_, sizeof(buffer_));
		strout << tm_t.tm_year + 1900 << '/' << tm_t.tm_mon + 1 << '/' << tm_t.tm_mday << ' ';
		classes[i].start_time = buffer_ + classes[i].start_time + ":00";
		classes[i].end_time = buffer_ + classes[i].end_time + ":00";
	}

	for (int i = 0; i < root.size(); i++)
	{
		std::cout << classes[i].name << '\t' << '\t';
		std::cout << classes[i].start_time << '\t';
		std::cout << classes[i].end_time << '\t';
		std::cout << endl;
	}
	cout << "一共" << classLen << "条数据" << endl;
}

int main() {
	readJson();
	getProfile();

	init();

	//窗口初始化
	initgraph(sWidth, wHeight);
	HWND hwnd = GetHWnd();



	// 去掉标题
	SetWindowLong(GetHWnd(), GWL_STYLE, GetWindowLong(GetHWnd(), GWL_STYLE) & ~WS_CAPTION);

	//移动窗口
	MoveWindow(hwnd, 0, sHeight - wHeight, sWidth, wHeight, false);

	//窗口透明
	setbkcolor(RGB(0, 0, 0));
	LONG lWindowLong = GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED;
	::SetWindowLong(hwnd, GWL_EXSTYLE, lWindowLong);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	////测试用
	//long long start = long long(getDate()) * 1000;
	//long long end = (long long(getDate()) + 5) * 1000;

	/*
	* 实际用
	long long start = long long(getDate("2022/12/3 20:32:00")) * 1000;
	long long end = long long(getDate("2022/12/3 20:33:00")) * 1000;
	*/

	while (true) {

		//置顶窗口
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		for (int i = 0; i < classLen; i++) {//遍历课表
			cout << "第" << i + 1 << "节课" << endl;

			if (int(getDate(classes[i].end_time.data())) < int(getDate())) {
				//如果已经结束那么就跳过
			/*	getchar();*/
				continue;
			}
			long long start = long long(getDate(classes[i].start_time.data())) * 1000;
			long long end = long long(getDate(classes[i].end_time.data())) * 1000;
			double t = 0;
			while (t < 1) {
				SYSTEMTIME currentTime;
				GetLocalTime(&currentTime);
				long long now = long long(getDate() * 1000 + currentTime.wMilliseconds);
				t = (now - start) * 1.0000 / (end - start);
				t = t > 1 ? 1 : t;	//控制t在[0,1]内
				BeginBatchDraw();
				cleardevice();
				COLORREF color;
				//#1e9600 (30,150,0)
				//#fff200 (255,242,0)
				//#ff0000 (255,0,0)

				if (t < 0.5) {
					double tt = t / 0.5;
					color = RGB(30 + (255 - 30) * tt,
						150 + (242 - 150) * tt,
						0);
				}
				else {
					double tt = (t - 0.5) / 0.5;
					color = RGB(255,
						242 - (242 - 0) * tt,
						0);
				}
				setfillcolor(color);
				setcolor(color);
				fillrectangle(0, 0, sWidth * 1.0000 * t, wHeight);
				EndBatchDraw();
			}
			Sleep(10);
		}
		cout << "放学" << endl;
		return 0;
	}
	return 0;
}
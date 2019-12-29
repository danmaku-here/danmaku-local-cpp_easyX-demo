// danmaku.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <graphics.h>
#include <vector>
#include <codecvt>
#include <locale>
#include <windows.h>
#include "NetWork.h"
#define ENABLE_FUGAI
HWND handle;
struct danmu {
	int left;
	std::wstring str;
	int used;
	int type;
	COLORREF color;
	int time;
	int track;
	int width;
}danmaku[1020];
struct pool {
	HWND handle;

	int screen_width;
	int screen_height;
	int char_height;
	int char_width;
	int n_track;

	int maxnum = 200;

	int n_gundong = 20;
	int n_top = 5;
	int n_bottom = 5;

	int track_gundong[50];
	int track_top[50];
	int track_bottom[50];
}pool;
int chongdie = 0;//是否允许弹幕重叠
int duration = 100;//刷新率是25fps,这里设定帧数（每个弹幕滚动时间）
int n_track = 20;//轨道数目

int procx(HWND hw, int tmd, int tms, int ct) {
	int temp1;
	int temp2;
	if (ct) {
		temp1 = GetWindowLong(hw, GWL_EXSTYLE);
		temp1 = temp1 ^ 32;
		temp1 = temp1 ^ 524288;
		SetWindowLong(hw, GWL_EXSTYLE, temp1);
	}
	else {
		SetWindowLong(hw, GWL_EXSTYLE, 524288);
	}
	if (tms == 0) {
		temp2 = 2;
	}
	else {
		if (tmd == 0) {
			temp2 = 1;
		}
		else {
			temp2 = 3;
		}
	}
	return(SetLayeredWindowAttributes(hw, tms, tmd, temp2));
}
void process() {
	BeginBatchDraw();
	setfillcolor(RGB(0, 0, 1));
	cleardevice();
	for (int i = 0; i < pool.n_track; i++) {
		pool.track_bottom[i] = 0;
		pool.track_gundong[i] = 0;
		pool.track_top[i] = 0;
	}
	for (int i = 0; i < pool.maxnum; i++) {
		if (danmaku[i].used == 1)
		{
			danmaku[i].left -= pool.screen_width / danmaku[i].time;
			//printf("[%d %d]\n", danmaku[i].left, pool.screen_width / danmaku[i].time);
			if (danmaku[i].left < -danmaku[i].width)
			{
				danmaku[i].used = 0;
				printf("弹幕[%d(在轨道%d上)]被销毁\n", i, danmaku[i].track);
			}
		}
		//printf("%d %d %d %d", danmaku[i].left, danmaku[i].track * pool.char_height - pool.char_height, danmaku[i].track, pool.char_height);
		settextcolor(danmaku[i].color);
		outtextxy(danmaku[i].left, danmaku[i].track * pool.char_height, danmaku[i].str.c_str());
		//rectangle(danmaku[i].left, danmaku[i].track * pool.char_height, danmaku[i].left + danmaku[i].width, danmaku[i].track * pool.char_height + pool.char_height);
		pool.track_gundong[danmaku[i].track] = max(pool.track_gundong[danmaku[i].track], danmaku[i].left + danmaku[i].width);
	}
	EndBatchDraw();
}

void add(std::wstring str, int position, COLORREF color)
{
	int id = -1;
	for (int i = 0; i < pool.maxnum; i++)
	{
		if (danmaku[i].used == 0) {
			id = i;
			break;
		}
	}
	if (id == -1)
	{
		//弹幕满了。扔掉
		printf("屏幕上弹幕已经达到最大数量\n");
		return;
	}
	int track_left = 99999;
	int track_id = 0;
	for (int i = 0; i < pool.n_gundong; i++) {
		if (pool.track_gundong[i] < track_left) {
			track_left = pool.track_gundong[i];
			track_id = i;
		}
	}
	if (track_left != 99999 && track_left > pool.screen_width&& chongdie == 0) {
		printf("弹幕轨道已占满\n");
		return;
	}
	danmaku[id].track = track_id;
	danmaku[id].left = pool.screen_width;
	danmaku[id].str = str;
	danmaku[id].time = duration;
	danmaku[id].used = 1;
	danmaku[id].width = textwidth(str.c_str());
	danmaku[id].color = color;
	danmaku[id].type = position;
	pool.track_gundong[track_id] = danmaku[id].left + danmaku[id].width;
	printf("弹幕[%d]在轨道%d创建\n", id, danmaku[id].track);
}
void init() {
	pool.n_track = n_track;
	LOGFONT font;
	pool.screen_width = GetSystemMetrics(SM_CXSCREEN);
	pool.screen_height = GetSystemMetrics(SM_CYSCREEN);
	pool.char_height = pool.screen_height / pool.n_track;
	pool.char_width = pool.char_height / 2;
	// GetClientRect(hd, &rect);
	// screen_width = (rect.right - rect.left);
	// screen_height = (rect.bottom - rect.top);
	printf("屏幕宽度 %d   屏幕高度 %d", pool.screen_width, pool.screen_height);
	initgraph(pool.screen_width, pool.screen_height, SHOWCONSOLE);
	handle = GetHWnd();
	setbkcolor(RGB(0, 0, 1));
	cleardevice();
	SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~WS_CAPTION);
	procx(handle, 150, RGB(0, 0, 1), 1);
	SetWindowPos(handle, HWND_TOPMOST, 0, 0, pool.screen_width, pool.screen_height, 0);
	gettextstyle(&font);
	font.lfHeight = pool.screen_height / pool.n_track;
	font.lfWidth = font.lfHeight / 2;
	font.lfWeight = 100;
	//font.lfQuality = DRAFT_QUALITY;
	wcscpy_s(font.lfFaceName, L"微软雅黑");
	settextstyle(&font);
	int speed = 1;
	//danmu temp;
	//for (int i = 0; i < 10; i++)
	//{
	//	temp.track = i + 1;
	//	temp.left = pool.screen_width;
	//	temp.str = L"test123文字😂";
	//	temp.str = L"123abc甲乙😂😶😫😭👨👩🐎🚌🚝";
	//	temp.time = 100;
	//	temp.used = 1;
	//	temp.width = textwidth(temp.str.c_str());
	//	danmaku[i] = temp;
	//}
}
int main()
{
	init();
	int count = 0;
	while (1) {
		count++;
		if (count == 50) {
			count = 0;
			std::vector<DANMU> ds;
			getDanmu(ds);
			for (int i = 0; i < ds.size(); i++)
			{

				COLORREF color = RGB((getHexNum(ds[i].color[1]) << 4) + (getHexNum(ds[i].color[2])), (getHexNum(ds[i].color[3]) << 4) + (getHexNum(ds[i].color[4])), (getHexNum(ds[i].color[5]) << 4) + (getHexNum(ds[i].color[6])));
				add(std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t >{}.from_bytes(ds[i].text), 0, color);
			}
			//add(L"da弹幕内容", 0, 0);
		}
		process();
		Sleep(40);
	}
}
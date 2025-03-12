#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//全局变量声明
HBITMAP bgStart,bg[20], r[20], q[20], d[20], ww[20], star[20], beauty[20], heart[20],scoreNum[10];
HDC		hdc, mdc, bufdc;
HWND	hWnd;
DWORD	tPre, tNow;

int flag = 0;
int w, h;
bool start = false;
int scoreNumLenX[10] = {50,24,44,42,44,42,46,43,46,46};
int scoreNumLenY[10] = {63,60,61,62,58,59,61,58,60,61};
int startx, starty;
int pos = 0;
int score = 0;
int number = 0; // 随机背景图序号
int bloodPackNumber = 0; // 判断血包是否出现 --> 随机概率，通过随机数
int bloodPosx = 0, bloodPosy = 0; //血包出现的位置
bool appearBloodPack = false;
bool eatBloodPack = false;
bool bgFirst = true;
bool DeadLook = false; //死亡回放
bool DeadFirst = true; //第一次死亡
bool DeadLooking = false; //判断是否在回放过程中
bool xuanfu = false; // 判断是否悬浮在开始按钮上
int countTime = 60; //倒计时时间
int countNum = 1; 
bool countEnd = false;


//敌人初始位置
int wnumRight = 5;
int wnumLeft = 5;
int wRun = 0, wRunLeft = 0, wRunRight = 0;
int wflag = 0;
int wx, wy;
int whurt = 0, wHurtRightNum = 0,wHurtLeftNum = 2;
int wAttack2 = 0, wAttack2RightNum = 0, wAttack2LeftNum = 3;
int wAttack3 = 0, wAttack3RightNum = 0, wAttack3LeftNum = 3;
bool wdead = false;
bool wover = false;
int wdir = 3;

//人物
bool speedup = false;
bool fight = false;
bool nShiftKeyDown = false;
bool gouquan = false;
bool walk = false;
bool normal = true;

int	num = 0, fightnum, dir = 2, x, y;       //x，y变量为人物贴图坐标，dir为人物移动方向，以0，1，2，3代表人物上，下，左，右方向上的移动：num为连续贴图中的小图编号
int blood = 5;
int hurt = 0;  //人物是否受伤
bool kouxue = false; // 人物是否应该扣血 --> 对应hurt的贴图

//查看死亡回放
struct Deadlook {
	int dir;
	int wdir;
	int x;
	int y;
	int wx;
	int wy;
	int wAttack2 = 0;
	int wAttack3 = 0;
	int wnumRight = 5;
	int wnumLeft = 5;
};

Deadlook deadlook[10];

//人物大小设置为高150px，宽80px 
//全局函数声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);
void                distance(int x, int y, int wx, int wy);
void                distance2(int x, int y, int wx, int wy);
void                distance3(int x, int y, int wx, int wy);
void                isGetBloodPack(int x,int y,int bloodPosx,int bloodPosy);
void                initial();
void                PeopleInitial();

void                MyTransparentBlt(HDC hdcDest,      // 目标DC
	int nXOriginDest,   // 目标X偏移
	int nYOriginDest,   // 目标Y偏移
	int nWidthDest,     // 目标宽度
	int nHeightDest,    // 目标高度
	HDC hdcSrc,         // 源DC
	int nXOriginSrc,    // 源X起点
	int nYOriginSrc,    // 源Y起点
	int nWidthSrc,      // 源宽度
	int nHeightSrc,     // 源高度
	UINT crTransparent  // 透明色,COLORREF类型
);


//****WinMain函数，程序入口点函数***********************
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	MyRegisterClass(hInstance);

	//初始化
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	GetMessage(&msg, NULL, NULL, NULL);//初始化msg              
	//游戏循环
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tNow = GetTickCount();

			if (DeadLooking == false) {
				if (tNow - tPre >= 100)
					MyPaint(hdc);
			}
			if (DeadLooking == true) {
				if (tNow - tPre >= 1000)
					MyPaint(hdc);
			}
		}
	}
	return msg.wParam;
}

//****设计一个窗口类****
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "canvas";
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

//****初始化函数****
// 加载位图并设定各种初始值
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hWnd = CreateWindow("canvas", "格斗游戏", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	MoveWindow(hWnd, 300, 100, 800, 740, true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	////////////////////////////////////////
	HBITMAP fullmap;
	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	//建立空的位图并置入mdc中
	fullmap = CreateCompatibleBitmap(hdc, 800, 700);
	SelectObject(mdc, fullmap);

	r[2] = (HBITMAP)LoadImage(NULL, "walkleft2.bmp", IMAGE_BITMAP, 840, 120, LR_LOADFROMFILE);//r2向左 
	r[3] = (HBITMAP)LoadImage(NULL, "milesright.bmp", IMAGE_BITMAP, 840, 120, LR_LOADFROMFILE);//r3向右 

	// 4 = dir + 2
	r[4] = (HBITMAP)LoadImage(NULL, "hurtleft.bmp", IMAGE_BITMAP, 124, 125, LR_LOADFROMFILE);//r4 向左走受伤 
	r[5] = (HBITMAP)LoadImage(NULL, "hurtright.bmp", IMAGE_BITMAP, 124, 125, LR_LOADFROMFILE);//r5 向右走受伤 
	r[6] = (HBITMAP)LoadImage(NULL, "zdeadleft.bmp", IMAGE_BITMAP, 125, 125, LR_LOADFROMFILE);//r6 左边死亡
	r[7] = (HBITMAP)LoadImage(NULL, "zdeadright.bmp", IMAGE_BITMAP, 125, 125, LR_LOADFROMFILE);//r7 右边死亡

	q[2] = (HBITMAP)LoadImage(NULL, "gouquanlefttst.bmp", IMAGE_BITMAP, 468, 125, LR_LOADFROMFILE);//q2向左 
	q[3] = (HBITMAP)LoadImage(NULL, "gouquanrighttst.bmp", IMAGE_BITMAP, 468, 125, LR_LOADFROMFILE);//q3向右 

	bgStart = (HBITMAP)LoadImage(NULL, "bgStart.bmp", IMAGE_BITMAP, 1600, 900, LR_LOADFROMFILE);
	bg[0] = (HBITMAP)LoadImage(NULL, "bg.bmp", IMAGE_BITMAP, 1300, 700, LR_LOADFROMFILE);
	bg[1] = (HBITMAP)LoadImage(NULL, "bg2.bmp", IMAGE_BITMAP, 1280, 700, LR_LOADFROMFILE);
	bg[2] = (HBITMAP)LoadImage(NULL, "bg3.bmp", IMAGE_BITMAP, 1920, 700, LR_LOADFROMFILE);

	ww[0] = (HBITMAP)LoadImage(NULL, "runleft.bmp", IMAGE_BITMAP, 1024, 128, LR_LOADFROMFILE); //敌人的Run
	ww[1] = (HBITMAP)LoadImage(NULL, "Run.bmp", IMAGE_BITMAP, 1024, 128, LR_LOADFROMFILE); //敌人的Run
	ww[4] = (HBITMAP)LoadImage(NULL, "Dead.bmp", IMAGE_BITMAP, 384, 128, LR_LOADFROMFILE); //敌人的hurt
	ww[8] = (HBITMAP)LoadImage(NULL, "wdeadleft.bmp", IMAGE_BITMAP, 384, 128, LR_LOADFROMFILE); //敌人的hurt

	ww[2] = (HBITMAP)LoadImage(NULL, "Attack_2.bmp", IMAGE_BITMAP, 512, 128, LR_LOADFROMFILE); //敌人的Attack_2
	ww[7] = (HBITMAP)LoadImage(NULL, "Attack2Left.bmp", IMAGE_BITMAP, 512, 128, LR_LOADFROMFILE); //敌人的Attack_2 left
	ww[3] = (HBITMAP)LoadImage(NULL, "Attack_3.bmp", IMAGE_BITMAP, 512, 128, LR_LOADFROMFILE); //敌人的Attack_3
	ww[5] = (HBITMAP)LoadImage(NULL, "Attack3Left.bmp", IMAGE_BITMAP, 512, 128, LR_LOADFROMFILE); //敌人的Attack_3 left

	star[1] = (HBITMAP)LoadImage(NULL, "star.bmp", IMAGE_BITMAP, 102, 104, LR_LOADFROMFILE);
	heart[1] = (HBITMAP)LoadImage(NULL, "heart.bmp", IMAGE_BITMAP, 184, 173, LR_LOADFROMFILE);

	beauty[1] = (HBITMAP)LoadImage(NULL, "face.bmp", IMAGE_BITMAP, 200, 200, LR_LOADFROMFILE);
	beauty[2] = (HBITMAP)LoadImage(NULL, "play.bmp", IMAGE_BITMAP, 214, 215, LR_LOADFROMFILE);
	beauty[3] = (HBITMAP)LoadImage(NULL, "score.bmp", IMAGE_BITMAP, 300, 300, LR_LOADFROMFILE);  //得分图标
	beauty[4] = (HBITMAP)LoadImage(NULL, "prew.bmp", IMAGE_BITMAP, 214, 215, LR_LOADFROMFILE);   //退出图标
	beauty[5] = (HBITMAP)LoadImage(NULL, "bloodPack2.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	beauty[6] = (HBITMAP)LoadImage(NULL, "GameOver.bmp", IMAGE_BITMAP, 650, 468, LR_LOADFROMFILE); //游戏结束
	beauty[7] = (HBITMAP)LoadImage(NULL, "table.bmp", IMAGE_BITMAP, 1161, 844, LR_LOADFROMFILE); //得分板
	beauty[8] = (HBITMAP)LoadImage(NULL, "star_3.bmp", IMAGE_BITMAP, 141, 85, LR_LOADFROMFILE);  
	beauty[9] = (HBITMAP)LoadImage(NULL, "DeadLook.bmp", IMAGE_BITMAP, 214, 215, LR_LOADFROMFILE);
	beauty[10] = (HBITMAP)LoadImage(NULL, "DeadLookIcon.bmp", IMAGE_BITMAP, 650, 400, LR_LOADFROMFILE); //死亡回放图标
	beauty[11] = (HBITMAP)LoadImage(NULL, "xuanfu.bmp", IMAGE_BITMAP, 502, 139, LR_LOADFROMFILE); //悬浮后PLAY图标
	beauty[12] = (HBITMAP)LoadImage(NULL, "countTime.bmp", IMAGE_BITMAP, 427, 483, LR_LOADFROMFILE);

	scoreNum[0] = (HBITMAP)LoadImage(NULL, "0.bmp", IMAGE_BITMAP, 50, 63, LR_LOADFROMFILE);
	scoreNum[1] = (HBITMAP)LoadImage(NULL, "1.bmp", IMAGE_BITMAP, 24, 60, LR_LOADFROMFILE);
	scoreNum[2] = (HBITMAP)LoadImage(NULL, "2.bmp", IMAGE_BITMAP, 44, 61, LR_LOADFROMFILE);
	scoreNum[3] = (HBITMAP)LoadImage(NULL, "3.bmp", IMAGE_BITMAP, 42, 62, LR_LOADFROMFILE);
	scoreNum[4] = (HBITMAP)LoadImage(NULL, "4.bmp", IMAGE_BITMAP, 44, 58, LR_LOADFROMFILE);
	scoreNum[5] = (HBITMAP)LoadImage(NULL, "5.bmp", IMAGE_BITMAP, 42, 59, LR_LOADFROMFILE);
	scoreNum[6] = (HBITMAP)LoadImage(NULL, "6.bmp", IMAGE_BITMAP, 46, 61, LR_LOADFROMFILE);
	scoreNum[7] = (HBITMAP)LoadImage(NULL, "7.bmp", IMAGE_BITMAP, 43, 58, LR_LOADFROMFILE);
	scoreNum[8] = (HBITMAP)LoadImage(NULL, "8.bmp", IMAGE_BITMAP, 46, 60, LR_LOADFROMFILE);
	scoreNum[9] = (HBITMAP)LoadImage(NULL, "9.bmp", IMAGE_BITMAP, 46, 61, LR_LOADFROMFILE);

	MyPaint(hdc);

	return TRUE;
}

//****自定义绘图函数****
// 人物贴图坐标修正及窗口贴图
void MyPaint(HDC hdc)
{
	if (start == false) {
		//先在mdc中贴上背景图
		SelectObject(bufdc, bgStart);
		BitBlt(mdc, 0, 0, 800, 700, bufdc, 410, 0, SRCCOPY);

		//play 按钮
		SelectObject(bufdc, beauty[2]);
		MyTransparentBlt(mdc, 350, 220, 60, 60, bufdc, 0, 0, 214, 215, RGB(255, 255, 255));

		if (xuanfu == true) {
			SelectObject(bufdc, beauty[11]);
			MyTransparentBlt(mdc, 250, 212, 250, 69, bufdc, 0, 0, 502, 139, RGB(255, 255, 255));
		}

		BitBlt(hdc, 0, 0, 800, 700, mdc, 0, 0, SRCCOPY);//把mdc贴到dc上 
	}

	if (start == true) {
		distance(x, y, wx, wy);
		distance2(x, y, wx, wy);
		distance3(x, y, wx, wy);
		isGetBloodPack(x, y, bloodPosx, bloodPosy);

		if (hurt == 1) {
			kouxue = true;
			hurt = 0;
		}
		
		// 使用时间作为随机数种子
		srand(time(0));

		// 生成0或2的随机数
		if (bgFirst == true) {
			number = rand() % 3;
			bgFirst = false;
		}
		SelectObject(bufdc, bg[number]);
		BitBlt(mdc, 0, 0, 800, 700, bufdc, pos, 0, SRCCOPY);

		//按照目前的移动方向取出对应人物的连续走动图，并确定截取人物图的宽度与高度
		if (fight == false && gouquan == false) SelectObject(bufdc, r[dir]);
		else if ((fight == true || gouquan == true)) SelectObject(bufdc, q[dir]);

		if (fight == false ) {
			w = 120;
			h = 120;
		}
		else if (fight == true || gouquan == true) {
			w = 117;
			h = 125;
		}

		//人物受伤
		if (kouxue == true && (wAttack3RightNum == 3 || wAttack2RightNum == 3 || wAttack3LeftNum == 1 || wAttack2LeftNum == 1)) {
			blood--;
			kouxue = false;
			SelectObject(bufdc, r[dir + 2]);
			MyTransparentBlt(mdc, x, y, 124, 125, bufdc, 0, 0, 124, 125, RGB(255, 255, 255));
		}
		else if (fight == false && blood > 0) MyTransparentBlt(mdc, x, y, w, h, bufdc, num * w, 0, w, h, RGB(255, 255, 255));
		else if (fight == true && blood > 0) MyTransparentBlt(mdc, x, y, w, h, bufdc, fightnum * w, 0, w, h, RGB(255, 255, 255));

		if (blood == 0) {
			SelectObject(bufdc, r[dir + 4]);
			MyTransparentBlt(mdc, x, y, 125, 125, bufdc, 0, 0, 125, 125, RGB(255, 255, 255));
		}

		//敌人静态、跑步动作
		if (whurt != 1 && wAttack3 == 0 && wAttack2 == 0 && wdir == 3) {
			SelectObject(bufdc, ww[1]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wnumRight, 0, 128, 128, RGB(255, 255, 255));
		}
		if (whurt != 1 && wAttack3 == 0 && wAttack2 == 0 && wdir == 2) {
			SelectObject(bufdc, ww[0]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wnumLeft, 0, 128, 128, RGB(255, 255, 255));
		}
		//敌人受伤
		if (whurt == 1 && wAttack3 == 0 && wAttack2 == 0 && dir == 3) {
			if (wHurtRightNum == 0) score++;
			SelectObject(bufdc, ww[4]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wHurtRightNum, 0, 128, 128, RGB(255, 255, 255));
		}
		if (whurt == 1 && wAttack3 == 0 && wAttack2 == 0 && dir == 2) {
			if (wHurtLeftNum == 1) score++;
			SelectObject(bufdc, ww[8]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wHurtLeftNum, 0, 128, 128, RGB(255, 255, 255));
		}
		//敌人按下E-->发起进攻
		if (wAttack3 == 1 && wAttack2 == 0 && wdir == 3) {
			//判断死亡回放
			if (blood == 1) {
				deadlook[1].wAttack3 = wAttack3;
				deadlook[1].wnumRight = wnumRight;
			}

			SelectObject(bufdc, ww[3]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wAttack3RightNum, 0, 128, 128, RGB(255, 255, 255));
		}
		if (wAttack3 == 1 && wAttack2 == 0 && wdir == 2) {
			//判断死亡回放
			if (blood == 1) {
				deadlook[1].wAttack3 = wAttack3;
				deadlook[1].wnumLeft = wnumLeft;
			}

			SelectObject(bufdc, ww[5]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wAttack3LeftNum, 0, 128, 128, RGB(255, 255, 255));
		}
		if (wAttack2 == 1 && wAttack3 == 0 && wdir == 3) {
			//判断死亡回放
			if (blood == 1) {
				deadlook[1].wAttack2 = wAttack2;
				deadlook[1].wnumRight = wnumRight;
			}

			SelectObject(bufdc, ww[2]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wAttack2RightNum, 0, 128, 128, RGB(255, 255, 255));
		}
		if (wAttack2 == 1 && wAttack3 == 0 && wdir == 2) {
			//判断死亡回放
			if (blood == 1) {
				deadlook[1].wAttack2 = wAttack2;
				deadlook[1].wnumLeft = wnumLeft;
			}

			SelectObject(bufdc, ww[7]);
			MyTransparentBlt(mdc, wx, wy, 170, 170, bufdc, 128 * wAttack2LeftNum, 0, 128, 128, RGB(255, 255, 255));
		}

		//star-->代表血量图
		int posx = 130, posy = 30;

		SelectObject(bufdc, heart[1]);
		for (int i = 0; i < blood; i++) {
			MyTransparentBlt(mdc, posx + 51 * i + i * 20, posy, 51, 52, bufdc, 0, 0, 184, 173, RGB(255, 255, 255));
		}

		//face.bmp --> 装饰
		if (blood != 0) {
			SelectObject(bufdc, beauty[1]);
			MyTransparentBlt(mdc, 20, 10, 100, 100, bufdc, 0, 0, 200, 200, RGB(255, 255, 255));
		}

		//score.bmp --> 得分图标
		if (blood != 0 && DeadLook == false) {
			SelectObject(bufdc, beauty[3]);
			MyTransparentBlt(mdc, 20, 120, 100, 100, bufdc, 0, 0, 300, 300, RGB(255, 255, 255));
			SelectObject(bufdc, star[1]);
			for (int i = 0; i < score; i++) {
				MyTransparentBlt(mdc, posx - 20 + 35 * (i % 10) + (i % 10) * 20, posy + 120 + i / 10 * 50, 35, 35.5, bufdc, 0, 0, 102, 104, RGB(255, 255, 255));
			}
		}

		//倒计时
		if (blood > 0 && DeadLooking == false) {
			SelectObject(bufdc, beauty[12]);
			MyTransparentBlt(mdc, 510, 20, 64, 72, bufdc, 0, 0, 427, 483, RGB(255, 255, 255));
			if (countTime < 10) {
				SelectObject(bufdc, scoreNum[countTime]);
				MyTransparentBlt(mdc, 590, 30, scoreNumLenX[countTime], scoreNumLenY[countTime], bufdc, 0, 0, scoreNumLenX[countTime], scoreNumLenY[countTime], RGB(255, 255, 255));
			}
			//默认countTime不可能大于100
			else {
				int tcountTime = countTime;
				int tcnum = 0;
				while (tcountTime) {
					int x = tcountTime % 10;
					tcountTime = tcountTime / 10;
					SelectObject(bufdc, scoreNum[x]);
					MyTransparentBlt(mdc, 630 - 50 * tcnum, 30, scoreNumLenX[x], scoreNumLenY[x], bufdc, 0, 0, scoreNumLenX[x], scoreNumLenY[x], RGB(255, 255, 255));
					tcnum++;
				}
			}
		}

		// 退出图标
		SelectObject(bufdc, beauty[4]);
		MyTransparentBlt(mdc, 700, 40, 50, 50, bufdc, 0, 0, 214, 215, RGB(255, 255, 255));

		// 血包
		if (blood != 0 && DeadLook == false) {
			if (blood < 5) {
				bloodPackNumber = rand() % 100;
				if (bloodPackNumber < 101 && eatBloodPack == false) {
					bloodPosx = 200;
					bloodPosy = 530;
					SelectObject(bufdc, beauty[5]);
					MyTransparentBlt(mdc, bloodPosx, bloodPosy, 80, 60, bufdc, 0, 0, 800, 600, RGB(255, 255, 255));
					appearBloodPack = true;
				}
			}
		}

		//血量为0 --> 游戏结束
		if (blood == 0 && DeadLook == false) {
			//死亡回放传数据-->之后进行死亡回放
			deadlook[1].dir = dir;
			deadlook[1].wdir = wdir;
			deadlook[1].x = x;
			deadlook[1].y = y;
			deadlook[1].wx = wx;
			deadlook[1].wy = wy;

			SelectObject(bufdc, beauty[6]);
			MyTransparentBlt(mdc, 230, 100, 325, 229, bufdc, 0, 0, 650, 458, RGB(255, 255, 255));

			//得分板
			SelectObject(bufdc, beauty[7]);
			MyTransparentBlt(mdc, 250, 380, 232, 169, bufdc, 0, 0, 1161, 844, RGB(255, 255, 255));

			//装饰
			SelectObject(bufdc, beauty[3]);
			MyTransparentBlt(mdc, 270, 380, 90, 90, bufdc, 0, 0, 300, 300, RGB(255, 255, 255));

			SelectObject(bufdc, beauty[8]);
			MyTransparentBlt(mdc, 360, 390, 113, 64, bufdc, 0, 0, 141, 85, RGB(255, 255, 255));

			//退出图标
			SelectObject(bufdc, beauty[9]);
			MyTransparentBlt(mdc, 700, 100, 50, 50, bufdc, 0, 0, 214, 215, RGB(255, 255, 255));

			//得分
			if (score < 10) {
				SelectObject(bufdc, scoreNum[score]);
				MyTransparentBlt(mdc, 340, 460, scoreNumLenX[score], scoreNumLenY[score], bufdc, 0, 0, scoreNumLenX[score], scoreNumLenY[score], RGB(255, 255, 255));
			}
			//默认score不可能大于100
			else {
				int tscore = score;
				int tnum = 0;
				while (tscore) {
					int x = tscore % 10;
					tscore = tscore / 10;
					SelectObject(bufdc, scoreNum[x]);
					MyTransparentBlt(mdc, 380 - 50 * tnum, 460, scoreNumLenX[x], scoreNumLenY[x], bufdc, 0, 0, scoreNumLenX[x], scoreNumLenY[x], RGB(255, 255, 255));
					tnum++;
				}
			}
		}

		if (DeadLook == true) {
			SelectObject(bufdc, beauty[10]);
			MyTransparentBlt(mdc, 60, 50, 650, 400, bufdc, 0, 0, 650, 400, RGB(255, 255, 255));
		}

		BitBlt(hdc, 0, 0, 800, 700, mdc, 0, 0, SRCCOPY);//把mdc贴到dc上 
	}

	//查看死亡回放
	if (DeadLook == true && DeadFirst == true) {
		//回放过程中
		DeadLooking = true;

		wRun = 0, wRunLeft = 0, wRunRight = 0;
		wflag = 0;
		whurt = 0, wHurtRightNum = 0, wHurtLeftNum = 2;
		wAttack2 = 0, wAttack2RightNum = 0, wAttack2LeftNum = 3;
		wAttack3 = 0, wAttack3RightNum = 0, wAttack3LeftNum = 3;
		wdead = false;
		wover = false;
		wdir = deadlook[1].wdir;
		dir = deadlook[1].dir;
		wnumRight = deadlook[1].wnumRight;
		wnumLeft = deadlook[1].wnumLeft;
		wAttack2 = deadlook[1].wAttack2;
		wAttack3 = deadlook[1].wAttack3;
		x = deadlook[1].x;
		y = deadlook[1].y;
		wx = deadlook[1].wx;
		wy = deadlook[1].wy;

		hurt = 1;
		blood = 1;
		
		DeadFirst = false;
	}

	tPre = GetTickCount();         //记录此次绘图时间
	if (flag) {
		if (fight == false) {

			if (gouquan == false) {//正常情况 
				num++;			//静止也在动
				if (num == 6) {//循环运动 
					num = 1;
				}
			}
		}
		else if (fight == true) {
			if (gouquan == true) {//勾拳情况 
				fightnum++;
				if (fightnum == 4) {
					gouquan = false;
					fightnum = 0;
				}
			}
		}

	}
	else if (flag == 0) {
		if (fight == false) num = 0;
		else if (fight == true) {
			if (gouquan == true) {//勾拳情况 
				fightnum++;
				if (fightnum == 4) {
					gouquan = false;
					fightnum = 0;
				}
			}
		}
	}

	if (wRun == 1 && wRunRight == 1) {
		wnumRight++;
		if (wnumRight == 8) {
			wnumRight = 0;
		}
	}
	if (wRun == 1 && wRunLeft == 1) {
		wnumLeft--;
		if (wnumLeft == 0) {
			wnumLeft = 7;
		}
	}
	if (whurt == 1 && dir == 3) {
		wHurtRightNum++;
		if (wHurtRightNum == 3) {
			wHurtRightNum = 0;
			whurt = 0;
		}
	}
	if (whurt == 1 && dir == 2) {
		if (wHurtLeftNum == 0) {
			wHurtLeftNum = 2;
			whurt = 0;
		}
		wHurtLeftNum--;
	}
	if (wAttack2 == 1 && wdir == 3) {
		wAttack2RightNum++;
		if (wAttack2RightNum == 4) {
			wAttack2RightNum = 0;
			wAttack2 = 0;
		}
	}
	if (wAttack2 == 1 && wdir == 2) {
		wAttack2LeftNum--;
		if (wAttack2LeftNum == 0) {
			wAttack2LeftNum = 3;
			wAttack2 = 0;
		}
	}
	if (wAttack3 == 1 && wdir == 3) {
		wAttack3RightNum++;
		if (wAttack3RightNum == 4) {
			wAttack3RightNum = 0;
			wAttack3 = 0;
		}
	}
	if (wAttack3 == 1 && wdir == 2) {
		wAttack3LeftNum--;
		if (wAttack3LeftNum == 0) {
			wAttack3LeftNum = 3;
			wAttack3 = 0;
		}
	}

	if (countNum <= 10) {
		countNum++;
	}
	else {
		countNum = 1;
		countTime--;
		if (countTime == 0) {
			blood = 0;
			countEnd = true;
		}
	}
}

//****消息处理函数***********************************
// 1.按下【Esc】键结束程序
// 2.按下方向键重设贴图坐标
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYUP://松开键位复原 
		flag = 0;
		wflag = 0;
		wRun = 0;
		wRunLeft = 0;
		wRunRight = 0;
		normal = true;
		speedup = false;
		nShiftKeyDown = false;
		walk = false;

		break;

	case WM_LBUTTONDOWN:
		// 获取鼠标点击的x和y坐标
		startx = LOWORD(lParam);
		starty = HIWORD(lParam);

		// 进入游戏
		if ((startx < 410 && startx > 350) && (starty < 280 && starty> 220) && start == false) {
			initial();
			start = true;
		}

		// 退出游戏，返回主界面
		if ((startx < 750 && startx > 700) && (starty < 90 && starty> 40) && start == true) {
			start = false;
			bgFirst = true;
		}

		//查看死亡回放
		if (blood == 0 && (startx < 750 && startx > 700) && (starty < 150 && starty> 100)) {
			DeadLook = true;
		}
		break;

	case WM_MOUSEMOVE:
		// 获取鼠标点击的x和y坐标
		startx = LOWORD(lParam);
		starty = HIWORD(lParam);
		// 悬浮在进入游戏图标上的时候-->出现play的图标
		if ((startx < 410 && startx > 350) && (starty < 280 && starty> 220)) {
			xuanfu = true;
		}
		else {
			xuanfu = false;
		}
		break;

	case WM_CHAR:

		switch (wParam)
		{
		case 'o':
			start = true;
			break;

		case 'd': //敌人右边跑
			if (blood > 0) {
				wx = wx + 10;
				wRun = 1;
				if (wx > 710) {
					wx = 710;
					if (pos < 500) {
						pos += 10;
						if (x > -50) {
							x -= 10;
						}
					}
				}
				wdir = 3;
				wRunRight = 1;
				break;
			}

		case 'a': //敌人左边跑
			if (blood > 0) {
				wx = wx - 10;
				wRun = 1;
				if (wx < -50)
				{
					wx = -50;
					if (pos > 0) {
						pos -= 10;
						if (x < 710) {
							x += 10;
						}
						else x = 710;
					}
				}
				wRunLeft = 1;
				wdir = 2;
				break;
			}

		case 'e':
			if(blood > 0) wAttack3 = 1;
			break;

		case 'q':
			if (blood > 0) wAttack2 = 1;
			break;

		case 'j': //勾拳键 
			if (blood > 0) {
				if (fight = true) gouquan = true;
				break;
			}

		case 'l':
			if (fight == true) fight = false;
			else fight = true;
			break;

			//闪现
		case 'k':
			if (blood > 0) {
				if (wdir == 3) {
					dir = 3;
					if (wx - 30 > 0) {
						x = wx - 30;
					}
				}
				if (wdir == 2) {
					dir = 2;
					if (wx + 30 < 710) {
						x = wx + 80;
					}
				}
				break;
			}
			}

		break;
	case WM_KEYDOWN:	     //按下键盘消息
		//判断按键的虚拟键码

		switch (wParam)
		{
		case VK_SHIFT:    
			nShiftKeyDown = true;
			break;

		case VK_ESCAPE:           //按下【Esc】键
			PostQuitMessage(0);  //结束程序
			break;
		case VK_UP:				  //按下【↑】键
			y -= 10;
			if (y < 465) //到顶了
				y = 465;
			PeopleInitial();
			break;

		case VK_DOWN:			  //按下【↓】键
			y += 6;
			if (y > 500)
				y = 500;
			PeopleInitial();
			break;


		case VK_LEFT:			  //按下【←】键
			switch (dir)
			{
			case 2:
				if (speedup == false) x -= 10;
				else {
					x += 15;
					if (x > 710) {
						x = 710;
						if (pos < 500) {
							pos += 10;
						}
					}
				}
				break;
			}

			if (x < -50)
			{
				x = -50;
				if (pos > 0) {
					pos -= 10;
					if (wx < 710) {
						wx += 10;
					}
					else wx = 710;
				}
			}

			dir = 2;
			PeopleInitial();
			break;

		case VK_RIGHT:			   //按下【→】键
			switch (dir)
			{
			case 3:
				if (speedup == false) x += 10;
				else {
					x -= 15;
					if (x < -50) {
						x = -50;
						if (pos > 0) {
							pos -= 10;
						}
					}
				}
				break;
			}
			if (x > 710) {
				x = 710;
				if (pos < 500) {
					pos += 10;
					if (wx > -50) {
						wx -= 10;
					}
					else wx = -50;
				}
			}

			dir = 3;
			PeopleInitial();
			break;

		}
		break;
	case WM_DESTROY:			    	//窗口结束消息
		DeleteDC(mdc);
		DeleteDC(bufdc);
		for (int i = 0; i < 20; i++) {
			DeleteObject(bg[i]);
			DeleteObject(r[i]);
			DeleteObject(q[i]);
			DeleteObject(d[i]);
			DeleteObject(ww[i]);
			DeleteObject(star[i]);
			DeleteObject(beauty[i]);
			DeleteObject(heart[i]);
		}
		for (int i = 0; i < 10; i++) {
			DeleteObject(scoreNum[i]);
		}
		DeleteObject(bgStart);
		ReleaseDC(hWnd, hdc);

		PostQuitMessage(0);
		break;
	default:							//其他消息
		flag = 0;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MyTransparentBlt(HDC hdcDest,      // 目标DC
	int nXOriginDest,   // 目标X偏移
	int nYOriginDest,   // 目标Y偏移
	int nWidthDest,     // 目标宽度
	int nHeightDest,    // 目标高度
	HDC hdcSrc,         // 源DC
	int nXOriginSrc,    // 源X起点
	int nYOriginSrc,    // 源Y起点
	int nWidthSrc,      // 源宽度
	int nHeightSrc,     // 源高度
	UINT crTransparent  // 透明色,COLORREF类型
)
{
	HBITMAP hOldImageBMP, hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);	// 创建兼容位图
	HBITMAP hOldMaskBMP, hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);			// 创建单色掩码位图
	HDC		hImageDC = CreateCompatibleDC(hdcDest);
	HDC		hMaskDC = CreateCompatibleDC(hdcDest);
	hOldImageBMP = (HBITMAP)SelectObject(hImageDC, hImageBMP);
	hOldMaskBMP = (HBITMAP)SelectObject(hMaskDC, hMaskBMP);

	// 将源DC中的位图拷贝到临时DC中
	if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc)
		BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	else
		StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
			hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);

	// 设置透明色
	SetBkColor(hImageDC, crTransparent);

	// 生成透明区域为白色，其它区域为黑色的掩码位图
	BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);

	// 生成透明区域为黑色，其它区域保持不变的位图
	SetBkColor(hImageDC, RGB(0, 0, 0));
	SetTextColor(hImageDC, RGB(255, 255, 255));
	BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);

	// 透明部分保持屏幕不变，其它部分变成黑色
	SetBkColor(hdcDest, RGB(0xff, 0xff, 0xff));
	SetTextColor(hdcDest, RGB(0, 0, 0));
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);

	// "或"运算,生成最终效果
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCPAINT);

	SelectObject(hImageDC, hOldImageBMP);
	DeleteDC(hImageDC);
	SelectObject(hMaskDC, hOldMaskBMP);
	DeleteDC(hMaskDC);
	DeleteObject(hImageBMP);
	DeleteObject(hMaskBMP);
}

//人物按下上下左右后的初始化封装
void PeopleInitial() {
	flag = 1;
	walk = true;
	gouquan = false;
	if (nShiftKeyDown == true) {
		speedup = true;
	}
	fight = false;
}
//判断人物是否可以攻击到敌人
void distance(int x, int y, int wx, int wy) {
	if ((x - wx < 100 && x > wx && gouquan == true && fightnum == 3 && dir == 2) || (wx - x < 60 && wx > x && gouquan == true && fightnum == 3 && dir == 3)) {
		whurt = 1;
	}
}

//判断敌人是否可以攻击到人物
void distance2(int x, int y, int wx, int wy) {
	if ((wx < x && x - wx < 80) && (wAttack2 == 1 || wAttack3 == 1) && wdir == 3) {
		hurt = 1;
	}
}

//判断敌人是否可以攻击到人物
void distance3(int x, int y, int wx, int wy) {
	if ((wx > x && wx - x < 60) && (wAttack2 == 1 || wAttack3 == 1) && wdir == 2) {
		hurt = 1;
	}
}

//判断人物是否捡到血包
void isGetBloodPack(int x, int y, int bloodPosx, int bloodPosy) {
	if ((x < bloodPosx + 50 && x > bloodPosx - 50)&&(appearBloodPack == true)) {
		blood++;
		eatBloodPack = true;
		appearBloodPack = false;
	}
}

//初始化
void initial() {
	pos = 0;
	score = 0;
	number = 0; // 随机背景图序号
	bloodPackNumber = 0; // 判断血包是否出现 --> 随机概率，通过随机数
	bloodPosx = 0, bloodPosy = 0; //血包出现的位置
	appearBloodPack = false;
	eatBloodPack = false;
	bgFirst = true;
	flag = 0;
	start = false;
	wnumRight = 5;
	wnumLeft = 5;
	wRun = 0, wRunLeft = 0, wRunRight = 0;
	wflag = 0;
	whurt = 0, wHurtRightNum = 0, wHurtLeftNum = 2;
	wAttack2 = 0, wAttack2RightNum = 0, wAttack2LeftNum = 3;
	wAttack3 = 0, wAttack3RightNum = 0, wAttack3LeftNum = 3;
	wdead = false;
	wover = false;
	wdir = 3;
	speedup = false;
	fight = false, nShiftKeyDown = false, gouquan = false, walk = false, normal = true;
	num = 0,dir = 2;   
	blood = 5;
	hurt = 0; 
	kouxue = false;
	x = 500,y = 480,wx = 100,wy = 420;
	DeadLook = false;
	DeadFirst = true;

	deadlook[1].x = 0;
	deadlook[1].y = 0;
	deadlook[1].wx = 0;
	deadlook[1].wy = 0;
	deadlook[1].dir = 0;
	deadlook[1].wdir = 0;
	deadlook[1].wAttack2 = 0;
	deadlook[1].wAttack3 = 0;
	//deadlook[0].wnumLeft = 5;
	//deadlook[0].wnumRight = 5;
	DeadLooking = false;
	xuanfu = false;
	countTime = 60;
	countNum = 1;
	countEnd = false;
}
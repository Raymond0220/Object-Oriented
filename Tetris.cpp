#include<iostream>
#include<windows.h>
#include<string>
#include<cstdlib>
#include<conio.h>
#include<ctime>

using namespace std;

/* 工具箱  */
	HANDLE hIn,hOut;
	//取得後臺處理I/O控制權，hIn=>鍵盤控制  hOut=>螢幕輸出控制
	void Control_HANDLE_Unit()
	{
		hIn=GetStdHandle(STD_INPUT_HANDLE);
		hOut=GetStdHandle(STD_OUTPUT_HANDLE);
	}
	//座標給定函式 
	void position(int x,int y)
	{
		static COORD c;
		c.X = x;
		c.Y = y;
		SetConsoleCursorPosition(hOut,c);
	}

/*遊戲主體*/ 
	/** 遊戲框架 **/ 
	int custom_x, custom_y;
	const int nWidth = 27, nHeight = 28;
	int *P = (int*)&nWidth;
//	*P = 15;
	int *Q = (int*)&nHeight;
//	*Q = 20;
	char Unit[3]={'|','H','@'};		//邊、底邊、方塊
	int frame[nHeight][nWidth]={0};	//後臺參數，框架中frame[y][x] x,y為x座標和y座標; 其值=>有0,1,2,3; 0空白,1為邊,2為底邊,3方塊
	int Delay_time;
	int score = 0;
	int color = 11;
	int mode = 0;

	//繪製函式
	void draw(int x,int y,int uniframe,char *unit,int Color=color)
	{
		SetConsoleTextAttribute(hOut,Color);
		if(uniframe==1)
		{
			position(x,y);
			cout<<unit[0];
		}
		else if(uniframe==2)
		{
			position(x,y);
			cout<<unit[1];
		}
		else if(uniframe==3)
		{
			position(x,y);
			cout<<unit[2];
		}
		else
		{
			position(x,y);
			cout<<' ';
		}
	}
	/**方塊架構 **/
		int i=5,j=0; //方塊初始座標 
		//7種方塊
		int Asset[7][4][4]={	{{0,0,3,0},{0,0,3,0},{0,0,3,0},{0,0,3,0}},  // 長條型 
								{{0,0,3,0},{0,3,3,0},{0,3,0,0},{0,0,0,0}},  // z型 (右上左下 
								{{0,3,0,0},{0,3,3,0},{0,0,3,0},{0,0,0,0}},  // z型 (左上右下 
								{{0,0,0,0},{0,0,3,0},{0,3,3,3},{0,0,0,0}},  // T型 
								{{0,0,0,0},{0,3,3,0},{0,3,3,0},{0,0,0,0}},  // 方形 
								{{0,3,3,0},{0,0,3,0},{0,0,3,0},{0,0,0,0}},  // L型 (右 
								{{0,3,3,0},{0,3,0,0},{0,3,0,0},{0,0,0,0}}	// L型 (左 
								};
		
		//方塊落下 =>方塊放置位置(x,y)、清除方塊、方塊放置位置(x,y+1)
		void AssetPos(int px,int py,int (&asset)[4][4],int color=0)//px,py為起始位置 
		{
			for(int y=py; y<(py+4); y++)
			{
				for(int x=px; x<(px+4); x++)
				{
					if(frame[y][x]==0 && asset[y-py][x-px]!=0)
					{
						frame[y][x]=asset[y-py][x-px];
						draw(x,y,frame[y][x],Unit,color);
					}
				}
			}
		}
		void cleanAsset(int px,int py,int (&asset)[4][4])
		{
			for(int y=py; y<(py+4); y++)
			{
				for(int x=px; x<(px+4); x++)
				{
					if(asset[y-py][x-px]==3)
					{
						frame[y][x]=0;
						draw(x,y,frame[y][x],Unit);
					}	
				}
			}
		}

		//判斷方塊是否碰撞
		bool collision(int px,int py,int (&asset)[4][4])
		{
			for(int y=py; y<(py+4); y++)
				for(int x=px; x<(px+4); x++)
					if(frame[y][x]!=0 && asset[y-py][x-px]!=0)	return 1;
							
			return 0;
		}

		//方塊旋轉
		void Rotation(int px,int py,int (&asset)[4][4],int color)
		{
			int temp[4][4]={0};
			for(int x=0; x<4; x++)
				for(int y=0; y<4; y++)
					temp[x][y]=asset[3-y][x]; //轉90度
			
			cleanAsset(px,py,asset);
			if(!collision(px, py, temp))
			{
				for(int x=0; x<4; x++)
					for(int y=0; y<4; y++)
						asset[x][y]=temp[x][y];
			}
			AssetPos(px,py,asset,color);
			
		}

	//鍵盤控制 往左、往右、旋轉
	static DWORD count;
	static INPUT_RECORD ir;
	void key_Control(int (&asset)[4][4])
	{
		ReadConsoleInput(hIn,&ir,1,&count);
		switch(ir.Event.KeyEvent.wVirtualKeyCode)
		{	
			case VK_LEFT:	//向左 
				cleanAsset(i,j,asset);
				if(collision(i-1,j,asset))//判斷落到下一格是否發生碰撞	
					AssetPos(i,j,asset,color);
				else
				{
					i--;
					AssetPos(i,j,asset,color);
				}
				break;
															
		  	case VK_RIGHT:	//向右 
				cleanAsset(i,j,asset);	
				if(collision(i+1,j,asset))//判斷落到下一格是否發生碰撞	
					AssetPos(i,j,asset,color);
				else
				{
					i++;
					AssetPos(i,j,asset,color);
				}
				break;
			
			case VK_UP:	//旋轉 
				Rotation(i,j,asset,color);
				break;
				
//			case VK_DOWN: //快速落下 
//				Delay_time=0;
//				break;
//			case VK_DOWN: //快速落下 
//				cleanAsset(i,j,asset);	
//				if(collision(i,j + 1,asset))//判斷落到下一格是否發生碰撞	
//					AssetPos(i,j,asset,color);
//				else
//				{
//					j++;
//					AssetPos(i,j,asset,color);
//				}
//				break;
//			case VK_CONTROL:
//				cleanAsset(i,j,asset);	
//				if(collision(i+4,j,asset))//判斷落到下一格是否發生碰撞	
//					AssetPos(i,j,asset,color);
//				else
//				{
//					i+=4;
//					AssetPos(i,j,asset,color);
//				}
//				break;
		}
		Sleep(30);
	}
	//遊戲判定
	bool Eliminate_Row_Asset(int y)	//判斷一排是否都為方塊
	{
		for(int x=1; x<nWidth-1; x++)
			if(frame[y][x]!=3)	return 0;
				return 1;
	}
	void Score(int count)
	{
		score+=count;
		position(nWidth+16,nHeight-1);
		cout<<score;
	}
	bool GameOver()
	{
		for(int x=1; x<nWidth-1; x++)
			if(frame[0][x]==3)	return 1;
		
		return 0;
	}
	
	void SpeedMode()
	{
		if(score>5000)
			Delay_time = 30 - mode * 5;
		else if(score>4000)
			Delay_time = 50 - mode * 5;
		else if(score>2000)
			Delay_time = 100 - mode * 10;
		else if (score>1000)
			Delay_time = 150 - mode * 15;
		else 
			Delay_time = 200 - mode * 20;
	}

int main()
{
	Control_HANDLE_Unit();
	
	position(0, 0);
	cout << "■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
	position(7, 4);
    cout << "  請輸入你要的方塊樣式：" << endl;
    position(7, 8);
    cout << "  請輸入方塊落下速度(1 ～ 3)：" << endl;
    position(7, 12);
    cout << "  是否有需要客製地圖大小(y / n)：" << endl;
    position(7, 14);
    cout << "                  --請輸入X(寬)：" << endl;
    position(7, 16);
    cout << "                  --請輸入Y(長)：" << endl;
    position(0, 20);
	cout << "■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
	SetConsoleTextAttribute(hOut ,243);
	position(71, 25);
	cout << "                                                 ";
	position(71, 26); 
	cout << "    文化大學 112物件導向程式設計B班第六組專題    ";
	position(71, 27);
	cout << "                                                 ";
	SetConsoleTextAttribute(hOut ,11);
    position(32, 4);
	char custom;
	cin >> custom;
	Unit[2] = custom;
	position(38, 8);
	cin >> mode;
	position(41, 12);
	string custom_size;
	cin >> custom_size;
//	system("cls");
	if(custom_size == "Y" || custom_size == "y")
	{
////		int custom_x, custom_y;
		cin >> custom_x >> custom_y;
		cout << "1";
		*P = custom_x;
		cout << "2";
		*Q = custom_y;
		frame[nHeight][nWidth] = {};
////		nWidth = custom_x;
////		nHeight = custom_y;
//	cout << "1";
	}
//	cout << "2";
	system("cls");
//	for(int n1 = 0;n1 < 100;n1++)
//	{
//		for(int n2 = 0;n2 < 100;n2++)
//		{
//			position(n1, n2);
//			cout << " ";
//		}
//	}
	
	
	//遊戲背景
	for(int x=0; x<nWidth; x++)		//底邊框 
		frame[nHeight-1][x]=2;
	for(int y=0; y<nHeight; y++)	//邊框 
	{	
		frame[y][0]=1;
		frame[y][nWidth-1]=1;
	}
	
	for(int y = 0; y<nHeight; y++)
		for(int x=0; x<nWidth; x++)
			draw(x,y,frame[y][x],Unit);
	
	for(int y=0; y<nHeight; y++)
	{
		position(nWidth + 1, y);
		cout << '>' << y;
		if(y == 8)
		{
			position(nWidth+10,y - 2);
			cout << "■■■■■■■■■■■■■■■■■■■■■" << endl;
			position(nWidth+15,y);
		    cout << "    操作方式：" << endl;
		    position(nWidth+15,y + 1);
		    cout << "    ↑  - 旋轉" << endl;
		    position(nWidth+15,y + 2);
		    cout << "    ↓  - 下移" << endl;
		    position(nWidth+15,y + 3);
		    cout << "    ←  - 左移" << endl;
		    position(nWidth+15,y + 4);
		    cout << "    →  - 右移" << endl;
		    position(nWidth+15,y + 5);
//		    cout << "  空白鍵 - 降落" << endl;
//		    position(nWidth+10,y + 6);
		    cout << "   空格 - 暫停" << endl;
		    position(nWidth+10,y + 7);
			cout << "■■■■■■■■■■■■■■■■■■■■■" << endl;
			SetConsoleTextAttribute(hOut ,243);
			position(71, 25);
			cout << "                                                 \n";
			position(71, 26); 
			cout << "    文化大學 112物件導向程式設計B班第六組專題    \n";
			position(71, 27);
			cout << "                                                 \n";
			SetConsoleTextAttribute(hOut ,11);
		}
	}
	//計分板
	position(nWidth+10,nHeight-1);
	cout<<"Score:";
	Score(0);
	//放入方塊 	
	int asset[4][4]={0};	//目前的方塊 
	
	while(1)
	{
		i=5,j=0;	//起始座標i,j
		SpeedMode();	//方塊落下速度 
		srand(time(NULL));
		int random = rand()%7;
		color = random+1;
		for(int i=0; i<4; i++)
		{
			for(int j=0; j<4; j++)
				asset[i][j]=Asset[random][i][j];	//選取隨機7種方塊之一 
		}
		AssetPos(i,j,asset,color);
		
		//方塊落下+控制 
		while(1)
		{
			while(!kbhit())
			{
				Sleep(Delay_time);
				cleanAsset(i,j,asset);
				if(collision(i,j+1,asset))	//有碰撞 
				{
					AssetPos(i,j,asset,color);
					break;
				}
				j++;
				AssetPos(i,j,asset,color);
				Sleep(Delay_time);
				if(kbhit())
				{
					ReadConsoleInput(hIn,&ir,1,&count);
					key_Control(asset);
				}
			}
			
			if(kbhit())
			{
				ReadConsoleInput(hIn,&ir,1,&count);
				key_Control(asset);
			}
			
			cleanAsset(i,j,asset);
			if(collision(i,j+1,asset))
			{
				AssetPos(i,j,asset,color);
				break;
			}
			AssetPos(i,j,asset,color);	
		}
		
		//從版面底下開始檢查檢查是否有一排都為方塊 
		for(int y=nHeight-2; y>0; y--)	
		{
			if(Eliminate_Row_Asset(y)) 	//若依牌都為方塊，削掉並計分 
			{
				for(int x=1; x<nWidth-1; x++)
				{
					frame[y][x]=0;
					draw(x,y,frame[y][x],Unit);
				}
				Sleep(100);
				Score(100);
				for(int j=y; j>0; j--)//上面方塊掉下來 
				{
					for(int x=1; x<nWidth-1; x++)
					{
						frame[j][x]=frame[j-1][x];
						draw(x,j,frame[j][x],Unit);
					}	
				}
				y++;
			}
		}
		
		if(GameOver())
		{
			position(nWidth+10,nHeight/2);
			cout<<"GAME	OVER!!";
			break;
		}
	}
	
	system("PAUSE");
	return 0;
} 

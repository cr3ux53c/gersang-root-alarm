#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>

#define TITLE "title �Ż� �峯 �˸� �ܼ� 1.0 Beta 3"
#define ALRAM_MIN1 2//¦����
#define ALRAM_MIN2 10

//�������� ����
time_t g_RealTimeSec;
time_t g_StartTimeSec;
tm g_GersangTime;//tm ����ü�� �ٸ� �Լ��� �������� ����ϹǷ� �ݵ�� ���.
tm *g_pRealTime;
tm g_ExpectLocalTime;
int g_nRemainHour;
int g_nRemainMin;
int g_arEachMonEndDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
bool g_bPlayBeep = true;

//�Լ� ����
void RefreshLocalTime();
int RequestGersangTime();
void PrintTime();
void InitUI();
void GetStartTime();
void CalcJangNalCountTime();
void Alram(int sec);
void PrintError(int i);
int GetTotalRemainMin(int ParamDay);
void ProceedKey();
bool IsKeyDown(int Key);
void GetExpectLocalTime(int nTotalRemainMin);
void PrintAlram();
void ChkIsAlramTime();
void RoundTime(tm *tmArg, bool UseMin, bool UseYear);
void gotoxy(int x, int y)
{
	COORD Cur;
	Cur.X=x;
	Cur.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);
}

void main()
{
	InitUI();
	//�Ż� �ð� �Է�
	for (int ret=0;1;)
	{
		ret = RequestGersangTime();
		if (!ret)
			break;
		else
			PrintError(ret);
	}
	//����
	g_StartTimeSec = time(NULL); // -115����� ��
	RefreshLocalTime();
	CalcJangNalCountTime();
	PrintTime();
	PrintAlram();
	for(;2;)
	{
		RefreshLocalTime();
		if (g_RealTimeSec - g_StartTimeSec >= 120)
		{
			g_StartTimeSec = time(NULL); //GetStartTime();
			g_GersangTime.tm_hour++;
			RoundTime(&g_GersangTime, false, false);
			CalcJangNalCountTime();
			PrintTime();
			ChkIsAlramTime();
		}
			ProceedKey();
			Sleep(100);
	}
}

//�Լ� ����

void ChkIsAlramTime()
{
	//���� �˶� �ð� ���
	if (g_GersangTime.tm_mday == 13 || g_GersangTime.tm_mday == 28)
	{
		if (ALRAM_MIN1%2 != 0) PrintError(1);
		if(g_GersangTime.tm_hour + ALRAM_MIN2/2 == 24)
		{
			Alram(ALRAM_MIN2);
		}
		if(g_GersangTime.tm_hour + ALRAM_MIN1/2 == 24)
		{
			Alram(ALRAM_MIN1);
		}
	}
}

void CalcJangNalCountTime()
{
	static int nTotalRemainMin;
	//���� �ð� ����Ʈ ���
	if (g_GersangTime.tm_mday < 14 || (g_GersangTime.tm_mon == 1 && g_GersangTime.tm_mday > 14))//15�� �̸�
	{
		nTotalRemainMin = GetTotalRemainMin(14);
	} else 	if (g_GersangTime.tm_mday < 29 && g_GersangTime.tm_mon != 1)//�� �ƴϸ� 29�� �̸� && 2�� �ƴ� ��
	{
		nTotalRemainMin = GetTotalRemainMin(29);
	} else if (g_GersangTime.tm_mon == 1 && g_GersangTime.tm_mday == 14)
	{
		nTotalRemainMin = GetTotalRemainMin(14);
	}
	if (nTotalRemainMin != 0)
	{
		g_nRemainHour = nTotalRemainMin / 60;
		g_nRemainMin = nTotalRemainMin % 60;
	}

	GetExpectLocalTime(nTotalRemainMin);
}

void GetExpectLocalTime(int nTotalRemainMin)
{
	//���� �ð� ����Ʈ ���
	g_ExpectLocalTime.tm_min = g_pRealTime->tm_min + nTotalRemainMin;
	g_ExpectLocalTime.tm_hour = g_pRealTime->tm_hour;
	g_ExpectLocalTime.tm_mday = g_pRealTime->tm_mday;
	g_ExpectLocalTime.tm_mon = g_pRealTime->tm_mon;
	g_ExpectLocalTime.tm_year = g_pRealTime->tm_year;
	
	RoundTime(&g_ExpectLocalTime, true, true);
}

void ProceedKey()
{
	int ch = 0;
	if (kbhit())
	{
		ch = getch();
		if (ch == 0xE0 || ch == 0)
		{
			ch=getch();
			switch(ch)
			{
			case 27://ESC
				exit(0);
			}
		} else {
			switch(ch)
			{
				case 109:
				case 77:
					if (g_bPlayBeep)
						g_bPlayBeep = false;
					else
						g_bPlayBeep = true;
					PrintAlram();
					break;
			}
		}
	}
}

bool IsKeyDown(int Key)
{
	return ((GetAsyncKeyState(Key) & 0x8000) != 0);
}

int GetTotalRemainMin(int ParamDay)
{
	if (g_GersangTime.tm_mon == 1 && g_GersangTime.tm_mday == 14)
	{
		return (ParamDay+28 - g_GersangTime.tm_mday)*48 - g_GersangTime.tm_hour*2;
	}
	return (ParamDay - g_GersangTime.tm_mday)*48 - g_GersangTime.tm_hour*2;
}

void Alram(int sec)
{
	char BufItoA[10] = {0,};
	itoa(sec, BufItoA, 10);
	char strTitle[100] = "title �峯���� �� ";
	strcat(strTitle, BufItoA);
	strcat(strTitle, "�� �̸� ���ҽ��ϴ�.");
	system(strTitle);
	system("cls");
	gotoxy(0, 0);
	printf("---------------------------------------\n");
	printf("�峯 �� �� %d�� �̸� ���ҽ��ϴ�.\n", sec);
	printf("---------------------------------------\n");
	PrintAlram();
	if (g_bPlayBeep)
	{
	printf("\a\a");
	Sleep(1000);
	printf("\a\a");
	}
}

void GetStartTime()
{
	g_StartTimeSec = time(NULL);
}

void RefreshLocalTime()
{
	g_RealTimeSec = time(NULL);//1970��1��1�� ���� ������ ��� �� ��ȯ
	g_pRealTime=localtime(&g_RealTimeSec);
}

void RoundTime(tm *tmArg, bool UseMin, bool UseYear)
{
	if (UseMin)
	{
		for (;tmArg->tm_min >= 60;)
		{
			tmArg->tm_min -= 60;
			tmArg->tm_hour++;
		}
	}
	if (tmArg->tm_hour >= 24)
	{
		tmArg->tm_hour -= 24;
		tmArg->tm_mday++;
	}
	if (tmArg->tm_mday+1 > g_arEachMonEndDay[tmArg->tm_mon])
	{
		tmArg->tm_mday = 0;
		tmArg->tm_mon++;
	}
	if (UseYear)
	{
		if (tmArg->tm_mon >= 12)
		{
			tmArg->tm_mon = 0;
			tmArg->tm_year++;
		}
	}
}

void PrintTime()
{
	system(TITLE);
	gotoxy(0, 0);
	puts("                                                    ");
	gotoxy(0, 0);
	printf("���� �ð�: %d�� %d�� %d�� %d�� %d��", \
		g_pRealTime->tm_year+1900,g_pRealTime->tm_mon+1,g_pRealTime->tm_mday, g_pRealTime->tm_hour,g_pRealTime->tm_min,g_pRealTime->tm_sec);
	gotoxy(0, 1);
	puts("                                                    ");
	gotoxy(0, 1);
	printf("�Ż� �ð�: %d�� %d�� %d��\n", \
		g_GersangTime.tm_mon+1, g_GersangTime.tm_mday+1, g_GersangTime.tm_hour);
	gotoxy(0, 2);
	puts("                                                    ");
	gotoxy(0, 2);
	printf("���� �峯���� ���� �ð�: %d�� %d��\n", g_nRemainHour, g_nRemainMin);
	gotoxy(0, 3);
	puts("                                                    ");
	gotoxy(0, 3);
	printf("���� �峯 ���� ���� �ð�: %d�� %d�� %d�� %d�� %d��\n", g_ExpectLocalTime.tm_year+1900, g_ExpectLocalTime.tm_mon+1, g_ExpectLocalTime.tm_mday, g_ExpectLocalTime.tm_hour, g_ExpectLocalTime.tm_min);
	gotoxy(0, 4);
	puts("                                                    ");
	gotoxy(0, 4);
	printf("----------------------------------------------------");
}

void PrintAlram()
{
	gotoxy(0, 5);
	puts("                          ");
	gotoxy(0, 5);
	printf("�˶� ������: ");
	if (g_bPlayBeep)
		printf("ON  (����: M)");
	else
		printf("OFF (����: M)");
}

void PrintError(int i)
{
	puts("");
	switch(i)
	{
		case 1:
			puts("ALRAM_MIN ��ũ�� ������� ¦���� �ƴմϴ�. �ٽ� �Է��Ϸ��� �ƹ�Ű�� �����ʽÿ�.");
			break;
		case 2:
			puts("������ ��� ���� �Է��߽��ϴ�. �ٽ� �Է��Ϸ��� �ƹ�Ű�� �����ʽÿ�.");
			break;
		case 3:
			puts("�ش� ���� �ִ� �ϼ��� �ʰ��Ͽ� �Է��߽��ϴ�. �ٽ� �Է��Ϸ��� �ƹ�Ű�� �����ʽÿ�.");
			break;
		default:
			exit(0);
			break;
	}
	getch();
}

int RequestGersangTime()
{
	int nTempBuf = -1;
	printf("�Ż� ���� ���� �Է��Ͻʽÿ�: ");
	scanf("%d", &nTempBuf);
	if (nTempBuf < 1 || nTempBuf > 12)
	{
		return 2;
	} else {
		g_GersangTime.tm_mon = nTempBuf-1;
	}
	printf("�Ż� ���� ���� �Է��Ͻʽÿ�: ");
	scanf("%d", &nTempBuf);
	if (nTempBuf < 1 || nTempBuf > 31)
	{
		return 2;
	} else {
		if (g_arEachMonEndDay[g_GersangTime.tm_mon] < nTempBuf)
		{
			return 3;
		}else{
			g_GersangTime.tm_mday = nTempBuf-1;
		}
	}
	printf("�Ż� ���� �ø� �Է��Ͻʽÿ�: ");
	scanf("%d", &nTempBuf);
	if (nTempBuf < 0 || nTempBuf > 23)
	{
		return 2;
	} else {
		g_GersangTime.tm_hour = nTempBuf;
		return 0;
	}
	return 1;
}

void InitUI()
{
	system(TITLE);
	system("color 0a");
	system("mode con:cols=56 lines=7");
}
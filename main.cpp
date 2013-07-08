#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include "EasyBMP.h" // http://easybmp.sourceforge.net/

using namespace std;
INPUT input[5][24];
int complete = 0;
int colno = 0;

#define BASE_X 334
#define BASE_Y 738

int getKeyCode(char* c){
	if ('A' <= c[0] && c[0] <= 'Z') return ((int) c[0] - 'A' + 0x41);
	if ('a' <= c[0] && c[0] <= 'z') return ((int) c[0] - 'a' + 0x41);
	if ('0' <= c[0] && c[0] <= '9') return ((int) c[0] - '0' + 0x30);
	return VK_SPACE;
}

void setClickInput(double x, double y, INPUT *input) {
	input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	input[0].mi.dx = x * 65535 / GetSystemMetrics(SM_CXSCREEN);
	input[0].mi.dy = y * 65535 / GetSystemMetrics(SM_CYSCREEN);
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
	input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
	return;
}

int main(){
	FILE *fp;

	::Sleep(5000);
	cout << "Start tracing" << endl;

	POINT p;
	GetCursorPos(&p);

	int base_x = p.x;
	int base_y = p.y;

	BMP Image;
	Image.ReadFromFile("ususugi.bmp");

	int height = Image.TellHeight();
	int width = Image.TellWidth();

	setClickInput(BASE_X - width / 2, BASE_Y - height / 2, input[0]);
	SendInput(3, input[0], sizeof(INPUT));

	bool **map = new bool*[width];
	for (int i = 0; i < width; i++) {
		map[i] = new bool[height];
	}

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			map[i][j] = false;
		}
	}

	for (int i = 0; i<5; i++) {
		for (int j = 0; j<24; j++){
			if (i == 0 || i == 1) {
				input[i][j].type = INPUT_MOUSE;
				input[i][j].mi.mouseData = 0;
				input[i][j].mi.dwExtraInfo = 0;
				input[i][j].mi.time = 0;
			}
			else if (i == 2) {
				input[i][j].type = INPUT_KEYBOARD;
				input[i][j].ki.dwExtraInfo = 0;
				input[i][j].ki.time = 0;
				input[i][j].ki.wScan = 0;
				if (j % 2 == 0)
					input[i][j].ki.dwFlags = 0;
				else
					input[i][j].ki.dwFlags = KEYEVENTF_KEYUP;
			}
		}
	}

	int x = 0, y = 0;

	for (y = 0; y<height; y++) {
		for (x = 0; x<width; x++) {
			if (map[x][y] == false) {

				char myChar[7];
				sprintf(myChar, "%02x%02x%02x", Image(x, y)->Red, Image(x, y)->Green, Image(x, y)->Blue);
				//sprintf( myChar, "000000" );

				cout << "ColNo." << colno + 1 << " #" << myChar << " x=" << x << ", y=" << y << " dot=" << complete << " " << (int) (complete * 100 / (height * width)) << "%" << endl;

				setClickInput(BASE_X - 235, BASE_Y + 65, input[0]);
				setClickInput(BASE_X + 35, BASE_Y + 45, input[1]);

				for (int i = 0; i<6; i++){
					input[2][i * 2].ki.wVk = VK_BACK;
					input[2][i * 2 + 1].ki.wVk = VK_BACK;
				}

				for (int i = 0; i<6; i++){
					input[2][i * 2 + 12].ki.wVk = getKeyCode(&myChar[i]);
					input[2][i * 2 + 13].ki.wVk = getKeyCode(&myChar[i]);
				}

				setClickInput(BASE_X + 35, BASE_Y + 85, input[3]);
				setClickInput(BASE_X - width / 2 + x, BASE_Y - height / 2 + y, input[4]);

				SendInput(3, input[0], sizeof(INPUT));
				::Sleep(300);
				SendInput(3, input[1], sizeof(INPUT));
				::Sleep(50);
				SendInput(24, input[2], sizeof(INPUT));
				::Sleep(50);
				SendInput(3, input[3], sizeof(INPUT));
				::Sleep(300);
				SendInput(3, input[4], sizeof(INPUT));
				::Sleep(500);

				map[x][y] = true;
				complete++;
				colno++;

				for (int l = 0; l<height; l++) {
					for (int k = 0; k<width; k++) {
						if (map[k][l] == false && Image(k, l)->Red == Image(x, y)->Red && Image(k, l)->Green == Image(x, y)->Green && Image(k, l)->Blue == Image(x, y)->Blue) {
							//if ((l+k)%2==0 && l+k>0) {
							setClickInput(BASE_X - width / 2 + k, BASE_Y - height / 2 + l, input[4]);
							SendInput(3, input[4], sizeof(INPUT));
							::Sleep(30);
							map[k][l] = true;
							complete++;
						}
						if (GetKeyState(VK_ESCAPE) & 0x8000 == 0x8000) break;
					}
					if (GetKeyState(VK_ESCAPE) & 0x8000 == 0x8000) break;
				}

			}
			if (GetKeyState(VK_ESCAPE) & 0x8000 == 0x8000) break;
		}
		if (GetKeyState(VK_ESCAPE) & 0x8000 == 0x8000) break;
	}

	cout << "Tracing finished" << endl;

	cout << "End program" << endl;
	::Sleep(2000);

	return 0;
}
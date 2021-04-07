#include <iostream>
#include <vector>
#include <windows.h>

struct Rect {
	int x, y;
	int w, h;
};

std::vector<Rect> getMonitorMetrics() {
	struct EnumInfo {
		int scrLeft;
		int scrTop;
		std::vector<Rect> metrics;
	};

	EnumInfo enumInfo;
	enumInfo.scrLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
	enumInfo.scrTop = GetSystemMetrics(SM_YVIRTUALSCREEN);

	auto enumProc = [](HMONITOR hMon, HDC hdc, LPRECT lpRect, LPARAM lParam) -> BOOL {
		auto& enumInfo = *(EnumInfo*)lParam;

		Rect rect;
		rect.x = lpRect->left - enumInfo.scrLeft;
		rect.y = lpRect->top - enumInfo.scrTop;
		rect.w = lpRect->right - lpRect->left;
		rect.h = lpRect->bottom - lpRect->top;

		enumInfo.metrics.push_back(rect);

		return true;
	};

	HDC hdc = GetDC(NULL);
	EnumDisplayMonitors(hdc, NULL, enumProc, (LPARAM)&enumInfo);
	ReleaseDC(NULL, hdc);

	return enumInfo.metrics;
}


HWND getWallpaperHandle()
{
	HWND progman = FindWindow(L"ProgMan", NULL);
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);

	HWND hWndWallpaper = NULL;

	auto enumProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
		HWND p = FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
		HWND* ret = (HWND*)lParam;

		if (p)
		{
			*ret = FindWindowEx(NULL, hWnd, L"WorkerW", NULL);
		}

		return true;
	};

	EnumWindows(enumProc, (LPARAM)&hWndWallpaper);
	return hWndWallpaper;
}

int main(int argc, const char* argv[], const char* env[])
{
	HWND wallpaper = getWallpaperHandle();
	auto metrics = getMonitorMetrics();

	HDC hdc = GetDC(wallpaper);

	for (size_t i = 0; i < metrics.size(); ++i) {
		auto& rect = metrics[i];

		COLORREF* arr = new COLORREF[rect.w * rect.h];

		for (int y = 0; y < rect.h; ++y) {
			for (int x = 0; x < rect.w; ++x) {
				int b = x * 255 / rect.w;
				int g = y * 255 / rect.h;
				int r = (i + 1) * 255 / metrics.size();
				r = 0;
				arr[x + y * rect.w] = RGB(r, g, b);
			}
		}

		HBITMAP map = CreateBitmap(rect.w, rect.h, 1, 8 * 4, arr);

		HDC src = CreateCompatibleDC(hdc);
		SelectObject(src, map);
		BitBlt(hdc, rect.x, rect.y, rect.w, rect.h, src, 0, 0, SRCCOPY);

		DeleteObject(map);
		DeleteDC(src);

		delete[] arr;
	}

	ReleaseDC(wallpaper, hdc);

	return 0;
}
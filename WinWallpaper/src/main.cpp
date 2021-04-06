#include <iostream>
#include <windows.h>

HWND getWallpaperHandle()
{
	HWND progman = FindWindow(L"ProgMan", NULL);
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);
	HWND hWndWallpaper = NULL;

	auto enumFunc = [](HWND hWnd, LPARAM lParam) -> BOOL {
		HWND p = FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
		HWND* ret = (HWND*)lParam;

		if (p)
		{
			*ret = FindWindowEx(NULL, hWnd, L"WorkerW", NULL);
		}

		return true;
	};

	EnumWindows(enumFunc, (LPARAM)&hWndWallpaper);

	return hWndWallpaper;
}

int main(int argc, const char* argv[], const char* env[])
{
	HWND wallpaper = getWallpaperHandle();
	RECT rectWallpaper;
	GetWindowRect(wallpaper, &rectWallpaper);

	int width = rectWallpaper.right - rectWallpaper.left;
	int height = rectWallpaper.bottom - rectWallpaper.top;
	std::cout << "Wallpaper dimensions: " << width << "x" << height << std::endl;

	HDC hdc = GetDC(wallpaper);

	COLORREF* arr = new COLORREF[1024 * 1024];

	for (int y = 0; y < 1024; ++y)
	{
		for (int x = 0; x < 1024; ++x)
		{
			int b = x % 256;
			int g = y % 256;
			int r = 0;
			arr[x + y * 1024] = RGB(r, g, b);
		}
	}

	HBITMAP map = CreateBitmap(1024, 1024, 1, 8 * 4, arr);

	HDC src = CreateCompatibleDC(hdc);
	SelectObject(src, map);
	BitBlt(hdc, -rectWallpaper.left, -rectWallpaper.top, 1024, 1024, src, 0, 0, SRCCOPY);

	DeleteObject(map);
	DeleteDC(src);

	delete[] arr;

	ReleaseDC(wallpaper, hdc);

	return 0;
}
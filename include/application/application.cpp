#include "application.h"
#include "math/timemath.h"
#include "interaction.h"
#include "math/graphics/graphics.h"
#include "optimization/stableTickLoop.h"

void application::changeKeyboardLayout()
{
	// Define the US English keyboard layout identifier
	LPCWSTR US_ENGLISH_KEYBOARD_LAYOUT_ID = L"00000409";

	// Load the US English keyboard layout
	HKL usEnglishLayout = LoadKeyboardLayout(US_ENGLISH_KEYBOARD_LAYOUT_ID, KLF_ACTIVATE);

	// Activate the US English keyboard layout
	ActivateKeyboardLayout(usEnglishLayout, KLF_ACTIVATE);

	// Example usage: Get the currently active keyboard layout
	HKL activeLayout = GetKeyboardLayout(0);
	std::wcout << L"Active Keyboard Layout: " << activeLayout << std::endl;
}

int application::run()
{
	changeKeyboardLayout();
	mainForm->focus();

	//cap at 60fps
	cmicroseconds& frameTime = (microseconds)(1000000 / cappedFps);
	stableLoop loop = stableLoop(frameTime);
	while (doEvents())//next frame
	{
		loop.waitTillNextLoopTime();
		processInput();//process events from user
		// Do stuff with graphics->colors
		render();
		// Draw graphics->colors to window

		if (!color::isByteColor)
		{
			std::copy(graphics.baseArray, graphics.baseArray + (graphics.size.volume()), windowColorPtr);
		}
		BitBlt(wndDC, 0, 0, (int)graphics.size.x(), (int)graphics.size.y(), hdcMem, 0, 0, SRCCOPY);
	}
	return 0;
}

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{

	application* app = application::getApplicationConnected(hwnd);
	switch (msg) {
	case WM_CREATE:
	{
		//https://social.msdn.microsoft.com/Forums/vstudio/en-US/b9ec34b5-827b-4357-8344-939baf284f46/win32-about-createwindowex-and-wndproc-pointer-relations?forum=vclanguage
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		app = (application*)lpcs->lpCreateParams;

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);//connect the window to this application
		app->hwnd = hwnd;//link it to the hwnd already, so the correct window is referenced to
		app->linkGraphics();
	}
	break;
	
	case WM_PASTE:
	{
		app->mainForm->paste();
	}
	break;
	
	case WM_MOUSEMOVE:
	{

	}
	break;
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		app->mainForm->scroll(app->MousePos, delta / WHEEL_DELTA);
	}
	break;
	case WM_KEYDOWN:
	{
		if (!app->lastKeyDown[wParam])
		{
			app->mainForm->keyDown((vk)wParam);
		}
		//if (pressed(VK_CONTROL))
		//{
		//	if (wParam == 'V')
		//	{
		//		app->mainForm->paste();
		//	}
		//}
		//else
		//{
		//
		//}
		app->lastKeyDown[wParam] = true;
	}
	break;
	case WM_KEYUP:
	{
		app->mainForm->keyUp((vk)wParam);
		app->lastKeyDown[wParam] = false;
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		cvk mouseButtonPressed = msg == WM_LBUTTONDOWN ? VK_LBUTTON : msg == WM_RBUTTONDOWN? VK_RBUTTON : VK_MBUTTON;
		if (!app->lastKeyDown[mouseButtonPressed])
		{
			app->mainForm->mouseDown(app->MousePos, mouseButtonPressed);
			app->lastKeyDown[mouseButtonPressed] = true;
		}
	}
	break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		cvk mouseButtonReleased = msg == WM_LBUTTONUP ? VK_LBUTTON : msg == WM_RBUTTONUP ? VK_RBUTTON : VK_MBUTTON;
		//f->onMouseUp(cveci2(MousePos.x(), MousePos.y()));
		app->lastKeyDown[mouseButtonReleased] = false;

		app->mainForm->mouseUp(cveci2(app->MousePos.x(), app->MousePos.y()), mouseButtonReleased);
	}
	break;
	case WM_SETFOCUS:
	{
		if (app->mainForm)
		{
			app->mainForm->focus();
		}
	}
	break;
	case WM_KILLFOCUS:
	{
		std::fill(app->lastKeyDown, app->lastKeyDown + 0x100, false);
		app->mainForm->lostFocus();
	}
	break;
	case WM_MDIMAXIMIZE:
	{
	}
	break;
	case WM_SIZE:
	case WM_SIZING:
	{
		app->linkGraphics();
	}
	break;
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_MAXIMIZE)
		{
			app->switchFullScreen();
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// Draw graphics->colors to window when window needs repainting
		BitBlt(hdc, 0, 0, (int)app->graphics.size.x(), (int)app->graphics.size.y(), app->hdcMem, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
	}
	break;	
	case WM_CLOSE:
	{
		if (app->mainForm->close())
		{
			DestroyWindow(hwnd);
		}
		else
		{
			//cancelled
			return 0;
		}
	}
	break;
	case WM_DESTROY:
	{
		SelectObject(app->hdcMem, app->hbmOld);
		DeleteDC(app->wndDC);
		PostQuitMessage(0);
	}
	break;
	default:
	{
	}
	break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void application::processInput()
{
	POINT p;
	if (GetCursorPos(&p))
	{
		//cursor position now in p.x() and p.y()
		if (ScreenToClient(hwnd, &p))
		{
			//p.x() and p.y() are now relative to hwnd's client area
			MousePos = vecl2(p.x, p.y);
			//swap y
			MousePos.y() = (int)graphics.size.y() - MousePos.y() - 1;
		}
	}
	if (mainForm->focused)
	{
		mainForm->hover(cveci2(MousePos.x(), MousePos.y()));
		for (vk keyCode = 0x0; keyCode < 0xff; keyCode++)
		{
			if (pressed(keyCode))
			{
				mainForm->keyPress(keyCode);
			}
		}
	}
}

void application::render()
{
	mainForm->render(cveci2(0, 0), graphics);
}

void application::linkGraphics()
{
	//adjust graphics size
	RECT graphicsrect, windowrect;
	GetClientRect(hwnd, &graphicsrect);
	GetWindowRect(hwnd, &windowrect);

	if (graphics.baseArray)
	{
		//else the colors will automatically be deleted
		if (!color::isByteColor)
		{
			delete[] graphics.baseArray;
			graphics.baseArray = nullptr;
		}
	}

	graphics = graphicsObject(cveci2(graphicsrect.right - graphicsrect.left, graphicsrect.bottom - graphicsrect.top), nullptr);

	/* Use CreateDIBSection to make a HBITMAP which can be quickly
	 * blitted to a surface while giving 100% fast access to graphics->colors
	 * before blit.
	 */
	 // Desired bitmap properties
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);//sizeof(BITMAPINFO);
	bmi.bmiHeader.biWidth = (LONG)graphics.size.x();
	bmi.bmiHeader.biHeight = (LONG)graphics.size.y(); //-graphics->size.y() to order graphics->colors from top to bottom
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // last byte not used, 32 bit for alignment
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;// graphics->width* graphics->size.y() * 4;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiColors[0].rgbBlue = 0;
	bmi.bmiColors[0].rgbGreen = 0;
	bmi.bmiColors[0].rgbRed = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	HDC hdc = GetDC(hwnd);
	// Create DIB section to always give direct access to colors
	hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&windowColorPtr, NULL, 0);
	if constexpr (color::isByteColor)
	{
		graphics.baseArray = (color*)windowColorPtr;
	}
	else
	{
		graphics.baseArray = new color[graphics.size.x() * graphics.size.y()];
	}

	DeleteDC(hdc);
	ShowWindow(hwnd, SW_SHOW);
	// Retrieve the window's DC
	wndDC = GetDC(hwnd);
	// Create DC with shared colors to variable 'graphics->colors'
	hdcMem = CreateCompatibleDC(wndDC);//HDC must be wndDC!! :)
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmp);

	mainForm->layout(crectanglei2(0, 0, (int)graphics.size.x(), (int)graphics.size.y()));
}
application::application(form* mainForm, HINSTANCE hInstance)
{
	this->mainForm = mainForm;
	hwnd = MakeWindow(GetMonitorRect(), hInstance, WndProc, this, false, windowCaption);
	std::fill(lastKeyDown, lastKeyDown + 0x100, false);
}
void application::switchFullScreen()
{
	//all styles will be kept except all overlappedwindow styles
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW)
	{
		//overlapped window: full screen is off
		//set it on
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(hwnd, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else
	{
		//turn full screen off
		SetWindowLong(hwnd, GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &g_wpPrev);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	isFullScreen = !isFullScreen;
	linkGraphics();
}
application* application::getApplicationConnected(HWND mainWindow)
{
	//getwindowlong is not going to cut it. long_ptr it is!
	LONG_PTR l = GetWindowLongPtr(mainWindow, GWLP_USERDATA);
	return (application*)l;
}
application::~application()
{
	delete mainForm;

	delete defaultTheme()->font->family->tex;
	delete defaultTheme()->font->family;
	delete defaultTheme()->font;
	delete defaultTheme();

	//we dont have to delete its colors because they are part of the DIBSection

	if (graphics.baseArray)
	{
		graphics.baseArray = nullptr;
	}
}


// Get the horizontal and vertical screen sizes in pixel
void GetMonitorResolution(int& horizontal, int& vertical)
{
	RECT desktop = GetMonitorRect();
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
//source:
//http://www.cplusplus.com/forum/windows/190177/
RECT GetDesktopRect()
{
	RECT desktop{};
	// size of screen (primary monitor) without just the taskbar
	BOOL fResult = SystemParametersInfo(SPI_GETWORKAREA, 0, &desktop, 0);
	return desktop;
}
RECT GetMonitorRect()
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	return desktop;
}

//get the graphics rect with getclientrect(HWND, &drawrect)
HWND MakeWindow(RECT windowrect,
	HINSTANCE hInstance,
	WNDPROC proc, void* userData, bool fullScreen, const std::wstring& windowCaption
)
{
	WNDCLASSEX wc{};
	//MSG msg;
	// Init wc
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = CreateSolidBrush(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = proc;
	wc.lpszClassName = L"animation_class";
	wc.lpszMenuName = NULL;
	wc.style = 0;
	// Register wc
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Failed to register window class.", L"Error", MB_OK);
		return 0;
	}
	// Make window:
	//https://docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window
	HWND hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		L"animation_class",
		windowCaption.c_str(),
		fullScreen ?
		WS_POPUP :
		WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_SYSMENU | WS_POPUP | WS_CAPTION,
		0, 0, windowrect.right - windowrect.left, windowrect.bottom - windowrect.top,
		NULL, NULL, hInstance,
		userData);//additional application data

	//POINT ptDiff;
	//RECT drawarea;
	// Get window and client sizes
	//GetClientRect(hwnd, &drawarea);
	//GetWindowRect(hwnd, &windowrect);
	// Find offset between window size and client size
	//ptDiff.x() = (windowrect.right - windowrect.left) - drawarea.right;
	//ptDiff.y() = (windowrect.bottom - windowrect.top) - drawarea.bottom;
	// Resize client
	//MoveWindow(hwnd, windowrect.left, windowrect.top, (windowrect.right - windowrect.left) + ptDiff.x(), (windowrect.bottom - windowrect.top) + ptDiff.y(), false);
	//UpdateWindow(hwnd);
	return hwnd;
}
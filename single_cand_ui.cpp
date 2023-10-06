#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "./single_cand_ui.h"
#include <comdef.h>
#include <ctime>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <fstream>
#include <iostream>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dwrite.lib")

ID2D1Factory *factory;
ID2D1HwndRenderTarget *target;
ID2D1SolidColorBrush *solid_brush;
IDWriteFactory *w_factory;
IDWriteTextFormat *w_format;
IDWriteTextLayout *w_layout;

HWND cand_ui_hwnd = NULL;

std::wstring fontname = L"Courier";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateSource();
void DrawString(std::string str, float fontSize, float x, float y, float r,
                float g, float b, float a);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASSEX wc = {};

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.lpszClassName = L"single_cand_ui";
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  RegisterClassEx(&wc);

  cand_ui_hwnd = CreateWindowEx(0, wc.lpszClassName, L"cand_ui", WS_POPUP,
                                100, // x
                                100, // y
                                640, // nWidth
                                480, // nHeight
                                NULL, NULL, hInstance, NULL);

  if (cand_ui_hwnd == NULL) {
    MessageBox(NULL, TEXT("NULL!"), TEXT("caption"),
               MB_OKCANCEL | MB_ICONINFORMATION);
  }
  CreateSource();

  ShowWindow(cand_ui_hwnd, nCmdShow);
  UpdateWindow(cand_ui_hwnd);

  // DrawString("1. hello\n2. what\n3. fany\n4. full\n5. world\n6 .window\n7. "
  //            "time\n8. hey",
  //            8, 0, 0, 0, 1, 1, 1);
  // UpdateWindow(cand_ui_hwnd);
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return msg.wParam;
}

void CreateSource() {
  MARGINS mar = {-1};
  DwmExtendFrameIntoClientArea(cand_ui_hwnd, &mar);
  D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);

  factory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(
          D2D1_RENDER_TARGET_TYPE_DEFAULT,
          D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
                            D2D1_ALPHA_MODE_PREMULTIPLIED)),
      D2D1::HwndRenderTargetProperties(cand_ui_hwnd, D2D1::SizeU(200, 200),
                                       D2D1_PRESENT_OPTIONS_IMMEDIATELY),
      &target);
  target->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &solid_brush);
  target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
  DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                      reinterpret_cast<IUnknown **>(&w_factory));
  w_factory->CreateTextFormat(fontname.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL,
                              DWRITE_FONT_STYLE_NORMAL,
                              DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-us",
                              &w_format);
}

void DrawString(std::string str, float fontSize, float x, float y, float r,
                float g, float b, float a) {

  target->BeginDraw();
  // 这可以把窗口的背景颜色给清除成透明色，主要在于最后一个指定 a 的参数
  // target->Clear(D2D1::ColorF(0, 0, 0, 0));
  target->Clear(D2D1::ColorF(r, g, b, a));
  RECT re;
  GetClientRect(cand_ui_hwnd, &re);
  FLOAT dpix, dpiy;
  dpix = static_cast<float>(re.right - re.left);
  dpiy = static_cast<float>(re.bottom - re.top);
  HRESULT res = w_factory->CreateTextLayout(
      std::wstring(str.begin(), str.end()).c_str(), str.length() + 1, w_format,
      dpix, dpiy, &w_layout);

  if (SUCCEEDED(res)) {
    DWRITE_TEXT_RANGE range;
    range.startPosition = 0;
    range.length = str.length();
    w_layout->SetFontSize(fontSize, range);
    solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
    target->DrawTextLayout(D2D1::Point2F(x, y), w_layout, solid_brush);
    w_layout->Release();
    w_layout = NULL;
  }
  target->EndDraw();
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uiMessage, WPARAM wParam,
                            LPARAM lParam) {
  switch (uiMessage) {
  case WM_CREATE:
    break;
  case WM_PAINT:
    DrawString("1. hello\n2. what\n3. fany\n4. full", 8, 0, 0, 0, 1, 1, 1);
    break;
  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  // case WM_LBUTTONDOWN:
    // 获取鼠标坐标
    // int xPos = GET_X_LPARAM(lParam);
    // int yPos = GET_Y_LPARAM(lParam);

    // 将鼠标事件传递给下层窗口
    // HWND hUnderlyingWindow = WindowFromPoint({xPos, yPos});
    // SendMessage(hUnderlyingWindow, uiMessage, wParam, lParam);
    // break;
  case WM_LBUTTONUP:
    break;
  // case WM_MOUSEMOVE:
  //   // 获取鼠标坐标
  //   POINT pt;
  //   GetCursorPos(&pt);
  //   ScreenToClient(hWnd, &pt);
  //
  //   // TODO: 检查鼠标位置是否在透明区域
  //   // 如果在透明区域，将消息传递给下层窗口
  //   // return DefWindowProc(hWnd, uiMessage, wParam, lParam);
  //
  //   break;
  default:
    return DefWindowProc(hWnd, uiMessage, wParam, lParam);
  }

  return 0;
}

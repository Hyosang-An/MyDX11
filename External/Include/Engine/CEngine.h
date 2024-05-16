#pragma once

class CEngine :
    public CSingleton<CEngine>
{
private:
    friend class CSingleton<CEngine>;
    CEngine();
    ~CEngine();

private:
    HWND			m_hWnd;
    POINT			m_ptResolution;

public:
    HWND GetMainWnd() { return m_hWnd; }

    int Init(HWND _wnd, POINT _ptResolution);
    void Progress();
    void ChangeWindowScale(UINT _width, UINT _height);
};


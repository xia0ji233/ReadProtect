
// XGameDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "XGame.h"
#include "XGameDlg.h"
#include "afxdialogex.h"
#include <winsvc.h>
#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DebugPrint(const char* format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	OutputDebugStringA(buffer);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CXGameDlg 对话框



CXGameDlg::CXGameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_XGAME_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXGameDlg::UnloadDriver(const char *ServeName)
{
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	DebugPrint("Open SCM handle=%p,GetLastError=%p\n", hServiceMgr, GetLastError());
	hServiceDDK = OpenServiceA(hServiceMgr, ServeName, SERVICE_ALL_ACCESS);
	if (hServiceDDK) {
		int bRet = 0;
		SERVICE_STATUS status;
		bRet = ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &status);
		if (bRet) {
			DebugPrint("Stop Service Success\n");
		}
		else {
			DebugPrint("Can't Stop Service\n");
			goto GETLASTERROR;
		}
		bRet = DeleteService(hServiceDDK);
		if (bRet) {
			DebugPrint("Unload Success\n");
		}
		else {
			DebugPrint("Unload Fail\n");
		}
	GETLASTERROR:
		DebugPrint("GetLastError=%p\n", GetLastError());
	}
	else {
		DebugPrint("OpenServe Failed\n");
		
	}

}

void CXGameDlg::LoadDriver(const char * ServeName, const char * DriverPath) {
	char FullPath[256] = { 0 };
	GetFullPathNameA(DriverPath, 256, FullPath, NULL);
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	printf("Open SCM handle=%p,GetLastError=%p\n", hServiceMgr, GetLastError());
	hServiceDDK = CreateServiceA(
		hServiceMgr,
		ServeName,
		ServeName,
		SERVICE_START,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		FullPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if (GetLastError() == ERROR_SERVICE_EXISTS) {
		DebugPrint("Service Already Exists\n");
		hServiceDDK = OpenServiceA(hServiceMgr, ServeName, SERVICE_START);
	}
	else if (GetLastError() != 0) {
		DebugPrint("GetLastError=%p\n", GetLastError());

		return;
	}
	DebugPrint("hServiceDDK=%p\n", hServiceDDK);

	int bRet = StartService(hServiceDDK, NULL, NULL);
	if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING) {
		DebugPrint("Service Already Running\n");
	}
	else {
		if (bRet == 0) {
			DebugPrint("Service Running Failed\n");
			DebugPrint("GetLastError=%p\n", GetLastError());
		}
		else {
			DebugPrint("Service Start Success\n");
		}
	}
}

void CXGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CXGameDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CXGameDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CXGameDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CXGameDlg 消息处理程序

BOOL CXGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->LoadDriver("xia0ji233", "Xprotect.sys");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CXGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CXGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CXGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int *ptr=NULL;
void CXGameDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ptr) {
		free(ptr);
	}
	ptr = (int *)malloc(sizeof(int));
	*ptr = rand();
	WCHAR s[32];
	wsprintf(s, L"%p", ptr);
	SetDlgItemText(IDC_EDIT1, s);
}


void CXGameDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s;
	GetDlgItemText(IDC_EDIT2, s);
	int ans = _ttoi(s);
	if (ans == *ptr) {
		MessageBox(L"你的猜测正确", L"Warning");
	}
	else {
		MessageBox(L"你的猜测错误", L"Warning");
	}
}

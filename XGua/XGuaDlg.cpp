
// XGuaDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "XGua.h"
#include "XGuaDlg.h"
#include "afxdialogex.h"
#include <winsvc.h>
#include <Windows.h>
#include <winioctl.h>

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

	OutputDebugStringA(buffer);//123123
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


// CXGuaDlg 对话框



CXGuaDlg::CXGuaDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_XGUA_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXGuaDlg::UnloadDriver(const char *ServeName)
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

void CXGuaDlg::LoadDriver(const char * ServeName, const char * DriverPath) {
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

void CXGuaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CXGuaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CXGuaDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CXGuaDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CXGuaDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CXGuaDlg 消息处理程序

BOOL CXGuaDlg::OnInitDialog()
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
	this->LoadDriver("XSafe", "Xsafe.sys");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CXGuaDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CXGuaDlg::OnPaint()
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
HCURSOR CXGuaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define SYMBOL L"\\??\\xia0ji2333"
#define ReadWriteCode CTL_CODE (FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED, FILE_ANY_ACCESS)

HANDLE hFile=NULL;

void CXGuaDlg::OnBnClickedButton1()
{
	
	// TODO: 在此添加控件通知处理程序代码
	if (!hFile) {
		hFile = CreateFile(SYMBOL, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		SetDlgItemText(IDC_EDIT4, L"打开设备成功");
	}
}




void CXGuaDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (hFile) {
		CloseHandle(hFile);
		hFile = NULL;
		SetDlgItemText(IDC_EDIT4, L"关闭设备成功");
	}
}


void CXGuaDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	typedef struct Mm {
		UINT32 PID;
		UINT32 len;
		PVOID Addr;
	}Mm;
	CString s;
	char buffer[32];
	WCHAR LOG[512],Mem[256];
	DWORD dwRetSize;
	UINT32 PID,len;
	PVOID Addr;
	Mm data;

	GetDlgItemText(IDC_EDIT1, s);
	PID = _ttoi(s);
	GetDlgItemText(IDC_EDIT2, s);
	Addr = (PVOID)_tcstoui64(s,nullptr,16);
	GetDlgItemText(IDC_EDIT3, s);
	len = _ttoi(s);
	
	data.PID = PID;
	data.len = len;
	data.Addr = Addr;
	memset(buffer, 0, sizeof(buffer));
	memset(LOG, 0, sizeof(LOG));
	memset(Mem, 0, sizeof(Mem));
	DeviceIoControl(
		hFile,//CreateFile打开驱动设备 返回的句柄
		ReadWriteCode,//控制码 CTL_CODE
		&data,//输入缓冲区指针
		sizeof(data),//输入缓冲区大小
		Mem,//返回缓冲区
		len,//返回缓冲区大小
		&dwRetSize,//返回字节数
		NULL
	);
	wsprintf(LOG, L"读取到的内存为：%d", *(UINT64*)Mem);
	SetDlgItemText(IDC_EDIT4, LOG);
}

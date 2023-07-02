
// XGuaDlg.h: 头文件
//

#pragma once


// CXGuaDlg 对话框
class CXGuaDlg : public CDialogEx
{
// 构造
public:
	CXGuaDlg(CWnd* pParent = nullptr);	// 标准构造函数
	void UnloadDriver(const char *);
	void LoadDriver(const char * ServeName, const char * DriverPath);
	~CXGuaDlg() {
		this->UnloadDriver("XSafe");
	}
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_XGUA_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};

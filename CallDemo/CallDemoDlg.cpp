
// CallDemoDlg.cpp : implementation file
//

#include "stdafx.h"

#include "afxdialogex.h"

#include "CallDemo.h"
#include "CallDemoDlg.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "../libPotraceWrapper/PotraceWrapper.h"

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/libPotraceWrapperd.lib")
#else
#pragma comment(lib, "../x64/Debug/libPotraceWrapper.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCallDemoDlg dialog

CCallDemoDlg::CCallDemoDlg(CWnd *pParent /*=nullptr*/)
    : CDialog(IDD_CALLDEMO_DIALOG, pParent) {
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_pPath = NULL;
  m_scaleW = m_scaleH = 1;
}

void CCallDemoDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCallDemoDlg, CDialog)
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE, &CCallDemoDlg::OnBnClickedButtonOpenFile)
END_MESSAGE_MAP()

// CCallDemoDlg message handlers

BOOL CCallDemoDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);  // Set big icon
  SetIcon(m_hIcon, FALSE); // Set small icon

  // TODO: Add extra initialization here

  return TRUE; // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCallDemoDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()),
                0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    if (m_pPath) {
      CPaintDC dc(this);
      Gdiplus::GraphicsPath *path = m_pPath->Clone();
      Gdiplus::Graphics *pGs = Gdiplus::Graphics::FromHDC(dc.m_hDC);
      Gdiplus::Pen pn(Gdiplus::Color::Red, 2.0);

      Gdiplus::Matrix mtx;
      mtx.Scale(m_scaleW, m_scaleH);
      path->Transform(&mtx);

      pGs->DrawPath(&pn, path);
      delete path;
    }
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user
// drags
//  the minimized window.
HCURSOR CCallDemoDlg::OnQueryDragIcon() {
  return static_cast<HCURSOR>(m_hIcon);
}

void CCallDemoDlg::OnBnClickedButtonOpenFile() {
  // TODO: Add your control notification handler code here
  CString filter = "Images (*.bmp;)|*.bmp;||";
  CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_READONLY, filter,
                      NULL);
  CString strFilePath;
  if (fileDlg.DoModal() == IDOK)
    strFilePath = fileDlg.GetPathName();
  else
    return;

  PotraceWrapper tracer;
  cv::Mat srcImage =
      cv::imread(strFilePath.GetBuffer(), cv::ImreadModes::IMREAD_GRAYSCALE);
  ASSERT(!srcImage.empty());
  cv::namedWindow("SrcImage");
  cv::imshow("SrcImage", srcImage);

  CRect rc;
  this->GetClientRect(&rc);
  m_scaleW = rc.Width() * 1.0 / srcImage.cols;
  m_scaleH = rc.Height() * 1.0 / srcImage.rows;

  bool ret = tracer.trace(&srcImage);
  if (!ret)
    MessageBox("Failed!");
  if (m_pPath)
    delete m_pPath;

  m_pPath = tracer.getpath();
  Invalidate();
}

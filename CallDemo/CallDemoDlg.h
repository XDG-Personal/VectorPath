
// CallDemoDlg.h : header file
//

#pragma once

// CCallDemoDlg dialog
class CCallDemoDlg : public CDialog {
  // Construction
public:
  CCallDemoDlg(CWnd *pParent = nullptr); // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_CALLDEMO_DIALOG };
#endif

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  private:
  Gdiplus::GraphicsPath *m_pPath;
    double m_scaleW;
  double m_scaleH;
  // Implementation
protected:
  HICON m_hIcon;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonOpenFile();
};

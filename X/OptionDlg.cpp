// OptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "X.h"
#include "OptionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG_
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// COptionDlg 对话框

IMPLEMENT_DYNAMIC(COptionDlg, CDialogEx)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Options_Dlg, pParent)
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &COptionDlg::OnNMClickTree1)
END_MESSAGE_MAP()


// COptionDlg 消息处理程序


BOOL COptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	/*
	m_optionTab.InsertItem(0, _T("第一页"));
	m_optionTab.InsertItem(1, _T("第二页"));
	m_optionTab.InsertItem(2, _T("第3页"));
	m_optionTab.InsertItem(3, _T("第4页"));
	*/
	

	//创建两个对话框
	CWnd* pSpinWnd = GetDlgItem(IDC_SPIN1);
	BOOL s;
	s = Page1.Create(IDD_Option_Page1, pSpinWnd);
	s = Page2.Create(IDD_Option_Page2, pSpinWnd);

	//设定在Tab内显示的范围
	CRect rc;
	pSpinWnd->GetClientRect(rc);

	rc.DeflateRect(10, 20);
	Page1.MoveWindow(&rc);
	Page2.MoveWindow(&rc);
	//显示初始页面
	//Page1.ShowWindow(SW_SHOW);
	//Page2.ShowWindow(SW_HIDE);

	//原文链接：https ://blog.csdn.net/mikasoi/article/details/81591778
	CTreeCtrl *ctreectrl = (CTreeCtrl *)GetDlgItem(IDC_TREE1);
	//调用ModifyStyle方法修改数控件Sytle
	ctreectrl->ModifyStyle(0, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);
	//设置树控件字体颜色
	ctreectrl->SetTextColor(RGB(0, 0, 255));
	//设置树控件背景颜色
	ctreectrl->SetBkColor(RGB(0, 255, 0));
	//添加根节点root
	HTREEITEM root = ctreectrl->InsertItem(_T("root"));
	ctreectrl->SetItemData(root, 1);
	//添加父节点parent1和parent2
	HTREEITEM parent1 = ctreectrl->InsertItem(_T("parent1"));
	ctreectrl->SetItemData(parent1, 2);
	HTREEITEM parent2 = ctreectrl->InsertItem(_T("parent2"));
	ctreectrl->SetItemData(parent2, 3);
	//添加根节点root的子节点rootchild
	HTREEITEM rootchild = ctreectrl->InsertItem(_T("rootchild"), root);
	ctreectrl->SetItemData(rootchild, 4);
	//添加父节点parent1的子节点
	HTREEITEM child1 = ctreectrl->InsertItem(_T("child1"), parent1);
	ctreectrl->SetItemData(child1, 5);
	HTREEITEM child2 = ctreectrl->InsertItem(_T("child2"), parent1, child1);
	ctreectrl->SetItemData(child2, 6);
	HTREEITEM child3 = ctreectrl->InsertItem(_T("child3"), parent1, child2);
	ctreectrl->SetItemData(child3, 7);
	//添加父节点parent2的子节点
	HTREEITEM child11 = ctreectrl->InsertItem(_T("child11"), parent2);
	ctreectrl->SetItemData(child11, 8);
	HTREEITEM child22 = ctreectrl->InsertItem(_T("child22"), parent2, child11);
	ctreectrl->SetItemData(child22, 9);
	HTREEITEM child33 = ctreectrl->InsertItem(_T("child33"), parent2, child22);
	ctreectrl->SetItemData(child33, 10);
	
		


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

/*
void COptionDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
// TODO: 在此添加控件通知处理程序代码
*pResult = 0;
int index = m_optionTab.GetCurSel();
switch (index)
{
case 0:
Page1.ShowWindow(SW_SHOW);
Page2.ShowWindow(SW_HIDE);
break;
case 1:
Page1.ShowWindow(SW_HIDE);
Page2.ShowWindow(SW_SHOW);
break;
default:
break;
}
}
*/

void COptionDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	/*————————————————
		原文链接：https ://blog.csdn.net/mikasoi/article/details/81591778
	/————————————————*/
	//先单击选中节点然后在单击节点才能响应
	HTREEITEM hTree = m_TreeCtrl.GetSelectedItem();
	int Sel = 0;
	if (hTree)
		Sel = m_TreeCtrl.GetItemData(hTree);
	switch (Sel)
	{
	default:
		break;
	}

	//获取当前鼠标点击消息的坐标点
	CPoint pt = GetCurrentMessage()->pt;

	//将鼠标的屏幕坐标，转换成树形控件的客户区坐标
	m_TreeCtrl.ScreenToClient(&pt);

	UINT uFlags = 0;
	HTREEITEM hItem = m_TreeCtrl.HitTest(pt, &uFlags);//然后做点击测试
													  //HitTest一般是指鼠标点击位置上是哪个树结点,从而得到鼠标位置的树结点
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))//如果点击的位置是在节点位置上
	{
		m_TreeCtrl.SelectItem(hItem);
		//获取开始我们设置的数据，注意这就是我为什么开始要每个节点设置不同的数据的原因。
		//当然，如果你的数据节点比较少，你也可以通过获取节点的文字，如上面设置的"root，parent1，parent2······",
		//然后通过字符串比较来判断点击了哪个节点
		//然后根据不同的节点，你完成不同的动作即可
		int nDat = m_TreeCtrl.GetItemData(hItem);
		CString itemText = m_TreeCtrl.GetItemText(hItem);
		//然后根据不同的节点，你完成不同的动作即可
		switch (nDat)
		{
		case 1:
			MessageBox(_T("1:root"));
			SetDlgItemText(IDC_SPIN1, itemText);
			Page1.ShowWindow(SW_SHOW);
			Page2.ShowWindow(SW_HIDE);
			break;
		case 2:
			MessageBox(_T("2:parent1"));
			SetDlgItemText(IDC_SPIN1, itemText);
			Page1.ShowWindow(SW_HIDE);
			Page2.ShowWindow(SW_SHOW);
			break;
		case 3:MessageBox(_T("3:parent2"));break;
		case 4:MessageBox(_T("4:rootchild"));break;
		case 5:MessageBox(_T("5:child1"));break;
		case 6:MessageBox(_T("6:child2"));break;
		case 7:MessageBox(_T("7:child3"));break;
		case 8:MessageBox(_T("8:child11"));break;
		case 9:MessageBox(_T("9:child22"));break;
		case 10:MessageBox(_T("10:child33"));break;
		default:MessageBox(_T("ERROR"));
		}
	}
}

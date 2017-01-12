#pragma once



// CResourceDirTreeCtrl

class CResourceDirTreeCtrl : public CTreeCtrl
{
	CImageList m_imgList;
	DECLARE_DYNAMIC(CResourceDirTreeCtrl)

public:
	CResourceDirTreeCtrl();
	virtual ~CResourceDirTreeCtrl();
	void SetResourceRootDir(LPCTSTR lpszRootDirectory);	// ������Դ·��
	void LoadImageList();
protected:
	HTREEITEM InsertDirectory(LPCTSTR lpszPathName, HTREEITEM hParent, HTREEITEM hInsertAfter);	// ���Ŀ¼���ڵ�

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	HTREEITEM ItemFromDirectory(LPCTSTR lpszDirectory, HTREEITEM hParent = TVI_ROOT);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
};
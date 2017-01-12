#pragma once

#include "BinStream.h"
#include "gldMatrix.h"
#include "gldCxform.h"

class CObjectAttachedInfo
{
public:
	enum {IF_DESC = 1, IF_TRANS = 2, IF_ALL = 3};

	CObjectAttachedInfo();
	HRESULT LoadFromStorage(IStorage *pIStorage);
	HRESULT SaveToStorage(IStorage *pIStorage);
	void SetDescription(const CString &strDesc);
	void SetTransform(const gldMatrix &matrix, const gldCxform &cxform);
	const CString &GetDescription() const { return m_strDesc; }
	const gldMatrix &GetMatrix() const { return m_matrix; }
	const gldCxform &GetCxform() const { return m_cxform; }
	UINT GetMask() const { return m_mask; }
	void SetMask(UINT mask) { m_mask = mask; }

private:
	UINT m_mask;
	CString m_strDesc;
	gldMatrix m_matrix;
	gldCxform m_cxform;
};

HRESULT WriteToStream(oBinStream &os, IStream *pStm);
HRESULT ReadFromStream(IStream *pStm, iBinStream &is);
HRESULT LoadObjectFromFile(const CString &strFileName, IGLD_Object **ppObject, CString &strDesc);
HRESULT SaveObjectToFile(IGLD_Object *pObject, const CString &strFileName, CString &strDesc);
HRESULT LoadObjectFromFile(const CString &strFileName, IGLD_Object **ppObject, CObjectAttachedInfo &info);
HRESULT SaveObjectToFile(IGLD_Object *pObject, const CString &strFileName, CObjectAttachedInfo &info);
HRESULT ChangeObjectFileDesc(const CString &strFileName, const CString &strDesc);
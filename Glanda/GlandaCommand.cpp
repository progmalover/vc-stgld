#include "StdAfx.h"
#include "GlandaCommand.h"

#include "gldInstance.h"
#include "gldMovieClip.h"
#include "GroupUpdate.h"
#include "SWFProxy.h"
#include "Selection.h"
#include "DrawHelper.h"
#include "my_app.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldSprite.h"

#include "gldDataKeeper.h"
#include "gldShapeRecord.h"
#include "gldShapeRecordChange.h"
#include "gldShapeRecordEdgeStraight.h"
#include "gldShapeRecordEdgeCurved.h"
#include "CmdAddShape.h"
#include "ObjectMap.h"
#include "GlandaDoc.h"
#include "gldLibrary.h"

CCmdChangeInstanceMask::CCmdChangeInstanceMask(gldInstance *pInstance, BOOL bMask)
{
	_M_Desc_ID = IDS_CMD_CHANGEINSTANCEMASK;

	m_pInstance = pInstance;
	m_mask = bMask;
}

CCmdChangeInstanceMask::~CCmdChangeInstanceMask(void)
{
}

bool CCmdChangeInstanceMask::Execute()
{
	m_pInstance->m_mask = m_mask;
	return TCommand::Execute();
}

bool CCmdChangeInstanceMask::Unexecute()
{
	m_pInstance->m_mask = !m_mask;
	return TCommand::Unexecute();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

CCmdImportVector::CCmdImportVector(gldLibrary *lib, gldMovieClip *mc, const string &name, bool create_inst)
: m_pMC(mc)
, m_bInst(create_inst)
, m_Name(name)
, m_x(0)
, m_y(0)
, m_bCenter(true)
{
	ASSERT(lib);
	ASSERT(mc);
	for (GOBJECT_LIST::const_iterator cit = lib->GetObjList().begin();
		cit != lib->GetObjList().end(); ++cit)	
		m_lstObj.push_back(*cit);	

	_M_Desc_ID = IDS_CMD_IMPORTVECTOR;
}

CCmdImportVector::CCmdImportVector(gldLibrary *lib, gldMovieClip *mc, const string &name, int x, int y)
: m_pMC(mc)
, m_bInst(true)
, m_Name(name)
, m_x(x)
, m_y(y)
, m_bCenter(false)
{
	ASSERT(lib);
	ASSERT(mc);
	for (GOBJECT_LIST::const_iterator cit = lib->GetObjList().begin();
		cit != lib->GetObjList().end(); ++cit)	
		m_lstObj.push_back(*cit);	

	_M_Desc_ID = IDS_CMD_IMPORTVECTOR;
}

CCmdImportVector::~CCmdImportVector(void)
{
}

gldShape *CCmdImportVector::CreateShapeFromMovie(gldMovieClip *pMovie)
{
	ASSERT(pMovie != NULL);
	gldShape *shape = new gldShape;
	GLAYER_LIST::reverse_iterator itl = pMovie->m_layerList.rbegin();
	for (; itl != pMovie->m_layerList.rend(); ++itl)
	{
		gldLayer *layer = *itl;
		if (layer->m_frameClipList.size() > 0)
		{
			gldFrameClip *frame = *layer->m_frameClipList.begin();
			GCHARACTERKEY_LIST_IT itk = frame->m_characterKeyList.begin();
			for (; itk != frame->m_characterKeyList.end(); ++itk)
			{
				gldCharacterKey *key = *itk;
				gldObj *obj = key->GetObj();
				ASSERT(obj != NULL);
				ASSERT(obj->IsGObjInstanceOf(gobjShape));
				gldShape *oshape = (gldShape *)obj;
				ASSERT(key->GetCharacterCount() > 0);
				gldMatrix &mx = key->GetCharacter(0)->m_matrix;
				MergeShape(shape, oshape, mx);
			}
		}
	}
	return shape;
}

void CCmdImportVector::MergeShape(gldShape *nshape, gldShape *oshape, gldMatrix &mx)
{
	ASSERT(nshape != NULL);
	ASSERT(oshape != NULL);

	gldShapeRecordChange *rc = new gldShapeRecordChange;
	rc->m_stateNewStyles = true;
	// copy styles
	vector<gldFillStyle *>::iterator itf = oshape->GetFillStyles()->begin();
	for (; itf != oshape->GetFillStyles()->end(); ++itf)
	{
		gldFillStyle *fill = (*itf)->Clone();
		FILLSTYLETYPE ftype = fill->GetFillStyleType();
		if (ftype == linear_gradient_fill || ftype == radial_gradient_fill)
		{
			gldMatrix mx1 = fill->GetGradientMatrix();
			fill->SetGradientMatrix(mx1 * mx);
		}
		else if (ftype == tiled_bitmap_fill || ftype == clipped_bitmap_fill)
		{
			gldMatrix mx1 = fill->GetBitmapMatrix();
			fill->SetBitmapMatrix(mx1 * mx);
		}
		rc->m_fsTable.push_back(fill);
	}
	vector<gldLineStyle *>::iterator itl = oshape->GetLineStyles()->begin();
	for (; itl != oshape->GetLineStyles()->end(); ++itl)
		rc->m_lsTable.push_back((*itl)->Clone());
	// add change style record to new shape
	nshape->GetShapeRecords()->push_back(rc);
	// copy shape records to new shape
	int nx = 0, ny = 0, ox = 0, oy = 0;
	vector<gldShapeRecord *>::iterator itr = oshape->GetShapeRecords()->begin();
	for (; itr != oshape->GetShapeRecords()->end(); ++itr)
	{
		gldShapeRecord *rec = (*itr)->Clone();
		GShapeRecType rtype = rec->GetGShapeRecType();
		if (rtype == gshapeRecChange)
		{
			gldShapeRecordChange *recc = (gldShapeRecordChange *)rec;
			if (recc->m_stateMoveTo)
			{
				ox = recc->m_moveDX;
				oy = recc->m_moveDY;
				nx = mx.GetX(ox, oy);
				ny = mx.GetY(ox, oy);
				recc->m_moveDX = nx;
				recc->m_moveDY = ny;				
			}
			if (recc->m_stateNewStyles)
			{
				itf = recc->m_fsTable.begin();
				for (; itf != recc->m_fsTable.end(); ++itf)
				{
					gldFillStyle *fill = *itf;
					FILLSTYLETYPE ftype = fill->GetFillStyleType();
					if (ftype == linear_gradient_fill || ftype == radial_gradient_fill)
					{
						gldMatrix mx1 = fill->GetGradientMatrix();
						fill->SetGradientMatrix(mx1 * mx);
					}
					else if (ftype == tiled_bitmap_fill || ftype == clipped_bitmap_fill)
					{
						gldMatrix mx1 = fill->GetBitmapMatrix();
						fill->SetBitmapMatrix(mx1 * mx);
					}
				}			
			}
		}
		else if (rtype == gshapeRecEdgeStraight)
		{
			gldShapeRecordEdgeStraight *recs = (gldShapeRecordEdgeStraight *)rec;
			ox += recs->m_dx;
			oy += recs->m_dy;
			int tx = mx.GetX(ox, oy);
			int ty = mx.GetY(ox, oy);
			recs->m_dx = tx - nx;
			recs->m_dy = ty - ny;
			nx = tx;
			ny = ty;
		}
		else if (rtype == gshapeRecEdgeCurved)
		{
			gldShapeRecordEdgeCurved *recc = (gldShapeRecordEdgeCurved *)rec;
			// control point
			ox += recc->m_cx;
			oy += recc->m_cy;
			int tx = mx.GetX(ox, oy);
			int ty = mx.GetY(ox, oy);
			recc->m_cx = tx - nx;
			recc->m_cy = ty - ny;
			nx = tx;
			ny = ty;
			// anchor point
			ox += recc->m_ax;
			oy += recc->m_ay;
			tx = mx.GetX(ox, oy);
			ty = mx.GetY(ox, oy);
			recc->m_ax = tx - nx;
			recc->m_ay = ty - ny;
			nx = tx;
			ny = ty;
		}
		nshape->GetShapeRecords()->push_back(rec);
	}
}

bool CCmdImportVector::Execute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	if (m_lstCommand.size() == 0)
	{
		// if the vector format only include shape then merge them to a shape
		// otherwise convert it to a sprite
		ASSERT(m_pMC != NULL);
		bool onlyShape = true;
		for (GLAYER_LIST_IT itl = m_pMC->m_layerList.begin()
			; itl != m_pMC->m_layerList.end() && onlyShape; ++itl)
		{
			gldLayer *layer = *itl;
			for (GFRAMECLIP_LIST_IT itf = layer->m_frameClipList.begin()
				; itf != layer->m_frameClipList.end() && onlyShape; ++itf)
			{
				gldFrameClip *frame = *itf;
				for (GCHARACTERKEY_LIST_IT itk = frame->m_characterKeyList.begin()
					; itk != frame->m_characterKeyList.end(); ++itk)
				{
					gldCharacterKey *key = *itk;
					if (!key->GetObj()->IsGObjInstanceOf(gobjShape))
					{
						onlyShape = false;
						break;
					}
				}
			}
		}
		gldObj *newObj = NULL;
		if (onlyShape)
		{
			gldShape *shape = CreateShapeFromMovie(m_pMC);
			delete m_pMC;
			m_pMC = NULL;
			ASSERT(shape != NULL);
			ASSERT(shape->m_ptr == NULL);
			CSWFProxy::PrepareTShapeForGObj(shape);
			gld_shape ts = CTraitShape(shape);
			ts.bound(ts.calc_bound());
			ts.normalize();
			shape->Clear();
			CTransAdaptor::TShape2GShape(ts, *shape);
			shape->m_name = m_Name;	// if name is empty CObjectMap::Use() will name it			
			newObj = shape;
		}
		else
		{
			gldSprite *sprite = new gldSprite;
			delete sprite->m_mc;
			sprite->m_mc = m_pMC;
			CSWFProxy::PrepareTShapeForGObj(sprite);
			int offsetX = -(sprite->m_bounds.left + sprite->m_bounds.right) / 2;
			int offsetY = -(sprite->m_bounds.top + sprite->m_bounds.bottom) / 2;
			// the center of sprite is zero
			for (GLAYER_LIST_IT itl = m_pMC->m_layerList.begin()
				; itl != m_pMC->m_layerList.end(); ++itl)
			{
				gldLayer *layer = *itl;
				for (GFRAMECLIP_LIST_IT itf = layer->m_frameClipList.begin()
					; itf != layer->m_frameClipList.end(); ++itf)
				{
					gldFrameClip *frame = *itf;
					for (GCHARACTERKEY_LIST_IT itk = frame->m_characterKeyList.begin()
						; itk != frame->m_characterKeyList.end(); ++itk)
					{
						gldCharacterKey *key = *itk;
						for (size_t i = 0; i < key->GetCharacterCount(); i++)
						{
							key->GetCharacter(i)->m_matrix.m_x += offsetX;
							key->GetCharacter(i)->m_matrix.m_y += offsetY;
						}
					}
				}
			}
			CTransAdaptor::UpdateTShape(sprite);
			newObj = sprite;
		}

		for (list<gldObj *>::iterator ito = m_lstObj.begin(); ito != m_lstObj.end(); ++ito)
		{
			gldObj *obj = *ito;
			if (onlyShape)
			{
				ASSERT(obj->IsGObjInstanceOf(gobjShape));
				CObjectMap::FreeSymbol(&obj);
			}
			else
			{
				Do(new CCmdAddObj(obj));
			}
		}

		Do(new CCmdAddObj(newObj));

		if (m_bInst)
		{			
			int x = m_x;
			int y = m_y;
			// place import graphics center of screen
			if (m_bCenter)
			{
				CSize sz = my_app.GetViewSize();
				x = sz.cx * 10;
				y = sz.cy * 10;
				my_app.ScreenToDataSpace(x, y);
			}
			Do(new CCmdAddInstance(_GetCurScene2(), newObj, 
				gldMatrix(1.0f, 1.0f, 0.0f, 0.0f, x, y), gldCxform()));
		}
	}

	return TCommandGroup::Execute();
}

bool CCmdImportVector::Unexecute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	return TCommandGroup::Unexecute();
}

/////////////////////////////////////////////////////////////////
// CCmdChangeInstanceName

CCmdChangeInstanceName::CCmdChangeInstanceName(gldInstance *pInstance, LPCTSTR lpszNewName)
{
	_M_Desc_ID = IDS_CMD_CHANGEINSTANCENAME;

	m_pInstance = pInstance;
	m_sNameOld = pInstance->m_name;
	m_sNameNew = lpszNewName;
}

CCmdChangeInstanceName::~CCmdChangeInstanceName()
{
}

bool CCmdChangeInstanceName::Execute()
{
	m_pInstance->m_name = m_sNameNew;

	// Some UI elements, such as timeline, need to update
	CSubjectManager::Instance()->GetSubject("RenameInstance")->Notify((void *)m_pInstance);

	return TCommand::Execute();
}

bool CCmdChangeInstanceName::Unexecute()
{
	m_pInstance->m_name = m_sNameOld;

	// Some UI elements, such as timeline, need to update
	CSubjectManager::Instance()->GetSubject("RenameInstance")->Notify((void *)m_pInstance);

	return TCommand::Unexecute();
}

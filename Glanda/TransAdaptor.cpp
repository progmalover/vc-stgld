#include "StdAfx.h"
#include "transadaptor.h"

#include "GGObject.h"
#include "Image.h"
#include "JPEG.h"
#include "Bitmap.h"

#include "gldGradient.h"
#include "gldGradientRecord.h"
#include "gldFillStyle.h"
#include "gldShapeRecord.h"
#include "gldShapeRecordChange.h"
#include "gldShapeRecordEdgeStraight.h"
#include "gldShapeRecordEdgeCurved.h"

#include "gldImage.h"
#include "gldText.h"
#include "gldFont.h"
#include "gldTextGlyphRecord.h"
#include "gldTextGlyph.h"

#include "gldMorphShape.h"
#include "gldButton.h"
#include "gldSprite.h"
#include "gldMovieClip.h"
#include "gldLibrary.h"

#include "TShapeParser.h"
#include "my_app.h"
#include "gldDataKeeper.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "SWFProxy.h"
#include "DrawHelper.h"
#include "TextToolEx.h"
#include "gld_shape_builder.h"

#include "gldScene2.h"
#include "GlandaDoc.h"

#include "Observer.h"
#include "Background.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SINGLETON(CTransAdaptor)
CTransAdaptor::CTransAdaptor(void)
{
}

CTransAdaptor::~CTransAdaptor(void)
{
}

//////////////////////////
// TMatrix structure    //
// | m_e11  m_e12 |     //
// | m_e21  m_e22 |     //
// | m_dx   m_dy  |     //
// gldMatrix structure  //
// | m_e11  m_e12 |     //
// | m_e21  m_e22 |     //
// | m_x    m_y   |     //
//////////////////////////
// Transform the TMatrix to gldMatrix
void CTransAdaptor::TM2GM(TMatrix &tm, gldMatrix &gm)
{
    gm.m_e11 = tm.m_e11;
    gm.m_e12 = tm.m_e12;
    gm.m_e21 = tm.m_e21;
    gm.m_e22 = tm.m_e22;
    gm.m_x = tm.m_dx;
    gm.m_y = tm.m_dy;
}

// Transfor the gldMatrix to TMatrix
void CTransAdaptor::GM2TM(gldMatrix &gm, TMatrix &tm)
{
    tm.m_e11 = gm.m_e11;
    tm.m_e12 = gm.m_e12;
    tm.m_e21 = gm.m_e21;
    tm.m_e22 = gm.m_e22;
    tm.m_dx = gm.m_x;
    tm.m_dy = gm.m_y;
}

// Transform the TCxform matrix to gldCxform matrix
void CTransAdaptor::TCX2GCX(TCxform &tcx, gldCxform &gcx)
{
    gcx.aa = FIXEDTOFLOAT(tcx.aa);
    gcx.ra = FIXEDTOFLOAT(tcx.ra);
    gcx.ga = FIXEDTOFLOAT(tcx.ga);
    gcx.ba = FIXEDTOFLOAT(tcx.ba);
    gcx.ab = FIXEDTOINT(tcx.ab);
    gcx.rb = FIXEDTOINT(tcx.rb);
    gcx.gb = FIXEDTOINT(tcx.gb);
    gcx.bb = FIXEDTOINT(tcx.bb);
}

// Transform the gldCxform matrix to TCxform matrix
void CTransAdaptor::GCX2TCX(gldCxform &gcx, TCxform &tcx)
{
    tcx.aa = FLOATTOFIXED(gcx.aa);
    tcx.ra = FLOATTOFIXED(gcx.ra);
    tcx.ga = FLOATTOFIXED(gcx.ga);
    tcx.ba = FLOATTOFIXED(gcx.ba);
    tcx.ab = INTTOFIXED(gcx.ab);
    tcx.rb = INTTOFIXED(gcx.rb);
    tcx.gb = INTTOFIXED(gcx.gb);
    tcx.bb = INTTOFIXED(gcx.bb);
}

// Transform the TColor to gldColor
void CTransAdaptor::TC2GC(TColor &tc, gldColor &gc)
{
    gc.red = tc.m_r;
    gc.green = tc.m_g;
    gc.blue = tc.m_b;
    gc.alpha = tc.m_a;
}

// Transform the gldColor to TColor
void CTransAdaptor::GC2TC(gldColor &gc, TColor &tc)
{
    tc.m_r = gc.red;
    tc.m_g = gc.green;
    tc.m_b = gc.blue;
    tc.m_a = gc.alpha;
}

// Transform the TFillStyle to gldFillStyle
void CTransAdaptor::TFS2GFS(TFillStyle &tfs, gldFillStyle &gfs)
{
    int _ftype = tfs.GetType();
    switch (_ftype)
    {
        case TFillStyle::FillStyleType::solid_fill:
        {
            // Set the fill type of gldFillStyle
            gfs.SetFillStyleType(::FILLSTYLETYPE::solid_fill);

            // Get TSolidFillStyle
            TSolidFillStyle *_fs = (TSolidFillStyle *)&tfs;

            // Transform the solid fill color
            TColor _tcol = _fs->GetColor();
            gldColor _gcol;
            TC2GC(_tcol, _gcol);

            // Set the solid fill color to gldFillStyle
            gfs.SetSolidFillColor(_gcol);
            break;
        }
        case TFillStyle::FillStyleType::linear_gradient_fill:
        {
            // Set the fill type of gldFillStyle
            gfs.SetFillStyleType(::FILLSTYLETYPE::linear_gradient_fill);

            // Create a new gradient fill (gldGradient)
            gldGradient *_ggradient = new gldGradient();

            // Get TLinearGradientFillStyle
            TLinearGradientFillStyle *_fs = (TLinearGradientFillStyle *)&tfs;

            // Transform each gradient record
            GGRADIENTRECORD *_trec = _fs->GetFirstRecord();
            while (_trec != NULL)
            {
                gldGradientRecord *_grec = new gldGradientRecord();
                TColor _tcol(_trec->color);
                TC2GC(_tcol, _grec->m_color);
                _grec->m_ratio = _trec->ratio;

                _ggradient->AddGradRecord(_grec);
                
                _trec = _trec->next;
            }

            // Set the gradient
            gfs.SetGradient(_ggradient);

            // Transform the gradient matrix
            TMatrix _tmx = _fs->GetMatrix();
            gldMatrix _gmx;
            TM2GM(_tmx, _gmx);
            gfs.SetGradientMatrix(_gmx);

            break;
        }
        case TFillStyle::FillStyleType::radial_gradient_fill:
        {
            // Set the fill type of gldFillStyle
            gfs.SetFillStyleType(::FILLSTYLETYPE::radial_gradient_fill);

            // Create a new gradient fill (gldGradient)
            gldGradient *_ggradient = new gldGradient();

            // Get TRadialGradientFillStyle
            TRadialGradientFillStyle *_fs = (TRadialGradientFillStyle *)&tfs;

            // Transform each gradient record
            GGRADIENTRECORD *_trec = _fs->GetFirstRecord();
            while (_trec != NULL)
            {
                gldGradientRecord *_grec = new gldGradientRecord();
                TColor _tcol(_trec->color);
                TC2GC(_tcol, _grec->m_color);
                _grec->m_ratio = _trec->ratio;

                _ggradient->AddGradRecord(_grec);
                
                _trec = _trec->next;
            }

            // Set the gradient
            gfs.SetGradient(_ggradient);

            // Transform the gradient matrix
            TMatrix _tmx = _fs->GetMatrix();
            gldMatrix _gmx;
            TM2GM(_tmx, _gmx);
            gfs.SetGradientMatrix(_gmx);

            break;
        }
        case TFillStyle::FillStyleType::clipped_bitmap_fill:
        {
            // Set the fill type of gldFillStyle
            gfs.SetFillStyleType(::FILLSTYLETYPE::clipped_bitmap_fill);

            // Get TClippedBitmapFillStyle
            TClippedBitmapFillStyle *_fs = (TClippedBitmapFillStyle*)&tfs;

            // Transform the bitmap
            TImage *_timg = _fs->GetImage();
            gldImage *_gimg = (gldImage*)_timg->Ptr();
            gfs.SetBitmap(_gimg);

            // Transform the bitmap matrix
            TMatrix _tmx = _fs->GetMatrix();
            gldMatrix _gmx;
            TM2GM(_tmx, _gmx);
            gfs.SetBitmapMatrix(_gmx);

            break;
        }
        case TFillStyle::FillStyleType::tiled_bitmap_fill:
        {
            // Set the fill type of gldFillStyle
            gfs.SetFillStyleType(::FILLSTYLETYPE::tiled_bitmap_fill);

            // Get TTiledBitmapFillStyle
            TTiledBitmapFillStyle *_fs = (TTiledBitmapFillStyle*)&tfs;

            // Transform the bitmap
            TImage *_timg = _fs->GetImage();
            gldImage *_gimg = (gldImage*)_timg->Ptr();
            gfs.SetBitmap(_gimg);

            // Transform the bitmap matrix
            TMatrix _tmx = _fs->GetMatrix();
            gldMatrix _gmx;
            TM2GM(_tmx, _gmx);
            gfs.SetBitmapMatrix(_gmx);

            break;
        }
        case TFillStyle::FillStyleType::no_fill:
        default:
        {
            // Set the fill type of gldFillStyle
            gfs.SetFillStyleType(::FILLSTYLETYPE::no_fill);
            break;
        }
    }
}

// Transform the gldFillStyle to TFillStyle
void CTransAdaptor::GFS2TFS(gldFillStyle &gfs, TFillStyle **tfs)
{
	FILLSTYLETYPE _ftype = gfs.GetFillStyleType();
	switch (_ftype)
	{
        case ::FILLSTYLETYPE::solid_fill:
		{
            // Create the TSolidFillStyle
			TSolidFillStyle *_tfs = new TSolidFillStyle();

            // Transform the solid color
			gldColor _gcol = gfs.GetSolidFillColor();
			TColor _tcol;
			GC2TC(_gcol, _tcol);
			_tfs->SetColor(_tcol);

			*tfs = _tfs;
			break;
		}
        case ::FILLSTYLETYPE::linear_gradient_fill:
		{
            // Create the TLinearGradientFillStyle
			TLinearGradientFillStyle *_tfs = new TLinearGradientFillStyle();

            // Get the gradient fill records from gfs
            gldGradient *_ggradient = gfs.GetGradient();

            // Transform each gradient record
            for (vector<gldGradientRecord *>::iterator it = _ggradient->m_gradRecords.begin(); it != _ggradient->m_gradRecords.end(); it++)
            {
                gldGradientRecord *_ggrec = (*it);

                TColor _tc;
                GC2TC(_ggrec->m_color, _tc);
                _tfs->AddRecord(_tc, _ggrec->m_ratio);
            }

            // Transform the gradient matrix
            TMatrix _tmx;
            gldMatrix _gmx = gfs.GetGradientMatrix();
            GM2TM(_gmx, _tmx);
            _tfs->SetMatrix(_tmx);

            *tfs = _tfs;
			break;
		}
        case ::FILLSTYLETYPE::radial_gradient_fill:
		{
            // Create the TRadialGradientFillStyle
			TRadialGradientFillStyle *_tfs = new TRadialGradientFillStyle();

            // Get the gradient fill records from gfs
            gldGradient *_ggradient = gfs.GetGradient();

            // Transform each gradient record
            for (vector<gldGradientRecord *>::iterator it = _ggradient->m_gradRecords.begin(); it != _ggradient->m_gradRecords.end(); it++)
            {
                gldGradientRecord *_ggrec = (*it);

                TColor _tc;
                GC2TC(_ggrec->m_color, _tc);
                _tfs->AddRecord(_tc, _ggrec->m_ratio);
            }

            // Transform the gradient matrix
            TMatrix _tmx;
            gldMatrix _gmx = gfs.GetGradientMatrix();
            GM2TM(_gmx, _tmx);
            _tfs->SetMatrix(_tmx);

            *tfs = _tfs;
			break;
		}
        case ::FILLSTYLETYPE::clipped_bitmap_fill:
		{
            // Create the TClippedBitmapFillStyle
			TClippedBitmapFillStyle *_tfs = new TClippedBitmapFillStyle();

            // Get the fill image
            gldImage *_gimg = gfs.GetBitmap();
            ASSERT(_gimg != NULL);
			ASSERT(CTraitImage::Binded(_gimg));            

            // Transform the bitmap matrix
            TMatrix _tmx;
            gldMatrix _gmx = gfs.GetBitmapMatrix();
            GM2TM(_gmx, _tmx);
            _tfs->SetMatrix(_tmx);
			_tfs->SetImage(CTraitImage(_gimg));

			*tfs = _tfs;
			break;
		}
        case ::FILLSTYLETYPE::tiled_bitmap_fill:
		{
            // Create the TTiledBitmapFillStyle
			TTiledBitmapFillStyle *_tfs = new TTiledBitmapFillStyle();

            // Get the fill image
            gldImage *_gimg = gfs.GetBitmap();
            ASSERT(_gimg != NULL);            
			ASSERT(CTraitImage::Binded(_gimg));            

            // Transform the bitmap matrix
            TMatrix _tmx;
            gldMatrix _gmx = gfs.GetBitmapMatrix();
            GM2TM(_gmx, _tmx);
            _tfs->SetMatrix(_tmx);
			_tfs->SetImage(CTraitImage(_gimg));

			*tfs = _tfs;
			break;
		}
        case ::FILLSTYLETYPE::no_fill:
		default:
		{
			ASSERT(false);
			break;
		}
	}
}

// Transform the TFillStyleTable to gldFillStyleTable
void CTransAdaptor::TFST2GFST(TFillStyleTable &tfst, std::vector<gldFillStyle *> &gfst)
{
    for (int i = 0; i < tfst.Count(); i++)
    {
        TFillStyle *_tfs = tfst.Get(i+1);
        gldFillStyle *_gfs = new gldFillStyle();
        TFS2GFS(*_tfs, *_gfs);
        // Add the fillstyle to the gfillstyle table
        gfst.push_back(_gfs);
    }
}

// Transform the gldFillStyleTable to TFillStyleTable
void CTransAdaptor::GFST2TFST(std::vector<gldFillStyle *> &gfst, TFillStyleTable &tfst)
{
	for (unsigned int i = 0; i < gfst.size(); i++)
	{
		gldFillStyle *_gfs = gfst[i];
		TFillStyle *_tfs = NULL;
		GFS2TFS(*_gfs, &_tfs);
		// Add the fillstyle to the tfillstyle table
		tfst.Add(_tfs);
	}
}

// Transform the TLineStyle to gldLineStyle
void CTransAdaptor::TLS2GLS(TLineStyle &tls, gldLineStyle &gls)
{
    TColor _tcol = tls.GetColor();
    gldColor _gcol;
    TC2GC(_tcol, _gcol);
    int _w = tls.GetWidth();
    gls.SetColor(_gcol);
    gls.SetWidth(_w);
}

// Transform the gldLineStyle to TLineStyle
void CTransAdaptor::GLS2TLS(gldLineStyle &gls, TLineStyle &tls)
{
	gldColor _gcol = gls.GetColor();
	TColor _tcol;
	GC2TC(_gcol, _tcol);
	int _w = gls.GetWidth();
	tls.SetColor(_tcol);
	tls.SetWidth(_w);
}

// Transform the TLineStyleTable to gldLineStyleTable
void CTransAdaptor::TLST2GLST(TLineStyleTable &tlst, std::vector<gldLineStyle *> &glst)
{
    for (int i = 0; i < tlst.Count(); i++)
    {
        TLineStyle *_tls = tlst.Get(i+1);
        gldLineStyle *_gls = new gldLineStyle();
        TLS2GLS(*_tls, *_gls);
        // Add the linestyle to the glinestyle table
        glst.push_back(_gls);
    }
}

// Transform the gldLineStyleTable to TLineStyleTable
void CTransAdaptor::GLST2TLST(std::vector<gldLineStyle *> &glst, TLineStyleTable &tlst)
{
	for (unsigned int i = 0; i < glst.size(); i++)
	{
		gldLineStyle *_gls = glst[i];
		TLineStyle *_tls = new TLineStyle();
		GLS2TLS(*_gls, *_tls);
		// Add the linestyle to the tlinestyle table
		tlst.Add(_tls);
	}
}

// Transform the gld_shape to gldShape
void CTransAdaptor::TShape2GShape(gld_shape &tshape, gldShape &gshape)
{
    // Clear the gshape content
    gshape.Clear();

    // Transform the bound box
    gld_rect _trc = tshape.bound();
    gshape.m_bounds.top = _trc.top;
    gshape.m_bounds.left = _trc.left;
    gshape.m_bounds.bottom = _trc.bottom;
    gshape.m_bounds.right = _trc.right;

    // Transform the shape
    bool _firstStyles = true;
    for (gld_draw_obj_iter dit = tshape.begin_draw_obj(); dit != tshape.end_draw_obj(); dit++)
    {
        // Get the draw_obj
        gld_draw_obj _dobj = (*dit);

        // Transform the fill/line styles
        if (_firstStyles)
        {
            // Transform the fill styles table
            TFST2GFST(_dobj.fill_styles(), *(gshape.GetFillStyles()));
            TLST2GLST(_dobj.line_styles(), *(gshape.GetLineStyles()));
            _firstStyles = false;
        }
        else
        {
            // Add a shape change record to the gshape
            gldShapeRecordChange *_grecChange = new gldShapeRecordChange();
            // Clear all states
            _grecChange->m_stateMoveTo = true;
            _grecChange->m_moveDX = 0;
            _grecChange->m_moveDY = 0;
            _grecChange->m_stateFS0 = true;
            _grecChange->m_fs0 = 0;
            _grecChange->m_stateFS1 = true;
            _grecChange->m_fs1 = 0;
            _grecChange->m_stateLS = true;
            _grecChange->m_ls = 0;
            // Set the new fill styles and line styles
            _grecChange->m_stateNewStyles = true;
            TFST2GFST(_dobj.fill_styles(), _grecChange->m_fsTable);
            TLST2GLST(_dobj.line_styles(), _grecChange->m_lsTable);
            gshape.AddShapeRecord(_grecChange);
        }

        // Transform the path
        for (gld_path_iter pit = _dobj.begin_path(); pit != _dobj.end_path(); pit++)
        {
            // Get the path
            gld_path _path = (*pit);

            // Get the start point
            int _sx = _path.start_x();
            int _sy = _path.start_y();
            // Add a shape change record to the gshape
            gldShapeRecordChange *_grecChange = new gldShapeRecordChange();
            _grecChange->m_stateMoveTo = true;
            _grecChange->m_moveDX = _sx;//0;
            _grecChange->m_moveDY = _sy;//0;
            gshape.AddShapeRecord(_grecChange);

            // Get the current fillstyle0, fillstyle1 and the linestyle
            int _cur_fs0 = -1;
            int _cur_fs1 = -1;
            int _cur_ls = -1;
            for (gld_edge_iter eit = _path.begin_edge(); eit != _path.end_edge(); eit.next())
            {
                gld_edge _edge = (*eit);

                gldShapeRecordChange *_grecChange = new gldShapeRecordChange();
                int _fs0 = _edge.fill_style0();
                int _fs1 = _edge.fill_style1();
                int _ls = _edge.line_style();
                if (_fs0 != _cur_fs0)
                {
                    _grecChange->m_stateFS0 = true;
                    _grecChange->m_fs0 = _fs0;
                    _cur_fs0 = _fs0;
                }
                else
                    _grecChange->m_stateFS0 = false;

                if (_fs1 != _cur_fs1)
                {
                    _grecChange->m_stateFS1 = true;
                    _grecChange->m_fs1 = _fs1;
                    _cur_fs1 = _fs1;
                }
                else
                    _grecChange->m_stateFS1 = false;

                if (_ls != _cur_ls)
                {
                    _grecChange->m_stateLS = true;
                    _grecChange->m_ls = _ls;
                    _cur_ls = _ls;
                }
                else
                    _grecChange->m_stateLS = false;

                if (_grecChange->m_stateFS0 || _grecChange->m_stateFS1 || _grecChange->m_stateLS)
                {
                    gshape.AddShapeRecord(_grecChange);
                }
                else
                {
                    delete _grecChange;
                }

                int _etype = _edge.edge_type();
                switch (_etype)
                {
                    case S_straight_edge:
                    {
                        gld_s_edge *_sedge = (gld_s_edge *)&_edge;
                        gldShapeRecordEdgeStraight *_grecS = new gldShapeRecordEdgeStraight();
                        long _dx = _sedge->e_point().x() - _sedge->s_point().x();
                        long _dy = _sedge->e_point().y() - _sedge->s_point().y();

                        _grecS->m_dx = _dx;
                        _grecS->m_dy = _dy;

                        gshape.AddShapeRecord(_grecS);
                        break;
                    }
                    case S_qbezier_edge:
                    {
                        gld_q_edge *_qedge = (gld_q_edge *)&_edge;
                        gldShapeRecordEdgeCurved *_grecC = new gldShapeRecordEdgeCurved();
                        _grecC->m_cx = _qedge->c_point().x() - _qedge->s_point().x();
                        _grecC->m_cy = _qedge->c_point().y() - _qedge->s_point().y();
                        _grecC->m_ax = _qedge->e_point().x() - _qedge->c_point().x();
                        _grecC->m_ay = _qedge->e_point().y() - _qedge->c_point().y();
                        gshape.AddShapeRecord(_grecC);
                        break;
                    }
                    case S_unknow_edge:
                    default:
                    {
                        break;
                    }
                }
            }
        }
    }
}

// Transform the gldShape to gls_shape
void CTransAdaptor::GShape2TShape(gldShape &gshape, gld_shape **tshape)
{
    // Clear the tshape content
    if (*tshape != NULL)
        (*tshape)->abandon();

	// Parse the gldShape data to TShape
	CTShapeParser _tparser;
	_tparser.Parse(&gshape);
	*tshape = _tparser._M_shape;
}

// Update the gobj's TShape ptr
void CTransAdaptor::UpdateTShape(gldObj *obj)
{
    ASSERT(obj != NULL);    

    // Recalculate the sub objects' bounds of the sprite and add the first frame's character of the sprite to the TShape
    if (obj->IsGObjInstanceOf(gobjSprite))
    {
		// Get the TShape ptr
		gld_shape *_tshape = (gld_shape*)obj->m_ptr;
		ASSERT(_tshape!= NULL);

        // Abandon the old data
	    _tshape->abandon();

        // Get the MC of the Sprite
		gldMovieClip *_mc = ((gldSprite*)obj)->m_mc;

        // Get the show list of the first frame
        GCHARACTER_LIST _list;
        _mc->GetShowList(_list, 0);

        // Recalculate all items' bounds in the show list
        for (GCHARACTER_LIST_IT it = _list.begin(); it != _list.end(); it++)
        {
            // Get the sub object
            gldCharacter *_character = (*it);
            gldObj *_subobj = _character->m_key->GetObj();
            gldMatrix _gmx = _character->m_matrix;
            gldCxform _gcx = _character->m_cxform;
            TMatrix _tmx;
            TCxform _tcx;
            CTransAdaptor::GM2TM(_gmx, _tmx);
            CTransAdaptor::GCX2TCX(_gcx, _tcx);

            // Update the sub object's bounds
            UpdateTShape(_subobj);

            // Add the sub object to the parent shape
            gld_shape *_subshape = (gld_shape*)_subobj->m_ptr;
            gld_shape _newtshape;
            _newtshape.create(*_subshape, _tmx, _tcx, _character);
            _tshape->push_back(_newtshape);
        }

		_tshape->bound(_tshape->calc_bound());
		gld_rect _trc = _tshape->bound();
		obj->m_bounds.top = _trc.top;
		obj->m_bounds.left = _trc.left;
		obj->m_bounds.bottom = _trc.bottom;
		obj->m_bounds.right = _trc.right;
    }
    // Recalculate the sub objects' bounds of the button and add the first frame's character of the button to the TShape
    if (obj->IsGObjInstanceOf(gobjButton))
    {
		// Get the TShape ptr
		gld_shape *_tshape = (gld_shape*)obj->m_ptr;
		ASSERT(_tshape!= NULL);

        // Abandon the old data
	    _tshape->abandon();

        // Get the MC of the Sprite
		gldMovieClip *_mc = ((gldButton*)obj)->m_bmc;

        // Get the show list of the first frame
        GCHARACTER_LIST _list;        
        _mc->GetShowList(_list, 0);
        bool    _firstempty = (_list.size() == 0);
        int _index = 3;
        while (_list.size() == 0 && _index >= 1)
        {
            _mc->GetShowList(_list, _index);
            _index--;
        }        

        // Recalculate all items' bounds in the show list
        for (GCHARACTER_LIST_IT it = _list.begin(); it != _list.end(); it++)
        {
            // Get the sub object
            gldCharacter *_character = (*it);
            gldObj *_subobj = _character->m_key->GetObj();
            gldMatrix _gmx = _character->m_matrix;
            gldCxform _gcx = _character->m_cxform;
            TMatrix _tmx;
            TCxform _tcx;
            CTransAdaptor::GM2TM(_gmx, _tmx);
            if (_firstempty)
            {
                _tcx.aa = INTTOFIXED(0);
                _tcx.ab = INTTOFIXED(50);
                _tcx.ra = 0;
                _tcx.rb = 0;
                _tcx.ga = 0;
                _tcx.gb = INTTOFIXED(150);
                _tcx.ba = 0;
                _tcx.bb = INTTOFIXED(50);
            }
            else
            {
                CTransAdaptor::GCX2TCX(_gcx, _tcx);
            }

            // Update the sub object's bounds
            UpdateTShape(_subobj);

            // Add the sub object to the parent shape
            gld_shape *_subshape = (gld_shape*)_subobj->m_ptr;
            gld_shape _newtshape;
            _newtshape.create(*_subshape, _tmx, _tcx, _character);
            _tshape->push_back(_newtshape);
        }

		_tshape->bound(_tshape->calc_bound());
		gld_rect _trc = _tshape->bound();
		obj->m_bounds.top = _trc.top;
		obj->m_bounds.left = _trc.left;
		obj->m_bounds.bottom = _trc.bottom;
		obj->m_bounds.right = _trc.right;
    }
    // Recalculate the sub objects' bounds of the text and add all of the letters of the text to the TShape ptr
    if (obj->IsGObjInstanceOf(gobjText))
    {
		// Get the TShape ptr
		gld_shape *_tshape = (gld_shape*)obj->m_ptr;
		ASSERT(_tshape!= NULL);

        // Abandon the old data	    
		_tshape->clear(true);

        // Get the gldText
        gldText2 *_gtext = (gldText2 *)obj;		

		int x1, x2, y1, y2;
		x1 = _gtext->rtRegion.left;
		y1 = _gtext->rtRegion.top;
		x2 = _gtext->rtRegion.right;
		y2 = _gtext->rtRegion.bottom;

		TLineStyle *pls = NULL;
		TSolidFillStyle *pfs = NULL;
		// create border for text that has border property
		if (_gtext->m_textType == gtextDynamic
			&& _gtext->m_dynaInfo.HasBorder())
		{			
			pls = new TLineStyle(20, TColor(0, 0, 0, 255));
			pfs = new TSolidFillStyle(TColor(255, 255, 255, 255));			
		}
		else
		{
			// create place object
		}
		gld_shape border = TShapeBuilder::BuildRect(x1 + 20,
				y1 + 20, x2 - 20, y2 - 20, pls, pfs);
		ASSERT(border.validate());
		_tshape->push_back(border);

        // Process the text
	    long _fontHeight;
	    gldMatrix _fmat;
	    gldColor _color;

	    // Get each letter of the text
	    for(list<gldTextGlyphRecord*>::iterator it = _gtext->m_glyphRecords.begin(); it != _gtext->m_glyphRecords.end(); it++)
        {
		    gldTextGlyphRecord *_rec = (*it);
		    if (_rec != NULL)
            {  
				_fontHeight = _rec->m_fontHeight;

			    // Update font matrix
                _fmat.m_e11 = _fontHeight / (float)1000;
                _fmat.m_e22 = _fontHeight / (float)1000;

                // Get the font shape
                gldShape *_gfShape = _rec->m_font->GetCharShape(_rec->m_code);

                // Update font matrix
                _fmat.m_x = _rec->m_xOffset;
                _fmat.m_y = _rec->m_yOffset;

                gldMatrix _bkmat = _gfShape->m_matrix;
                // Compute Character matrix
                _gfShape->m_matrix = _fmat; // * _gtext->m_matrix;

                gldFillStyle *_fs = new gldFillStyle();
                _fs->SetFillStyleType(solid_fill);
                _fs->SetSolidFillColor(_rec->m_color);

                _gfShape->ClearFillStyles();
                _gfShape->AddFillStyle(_fs);

                // Create the associated TShape
                TMatrix _tmx;
                TCxform _tcx;
                CTransAdaptor::GM2TM(_gfShape->m_matrix, _tmx);
                CTransAdaptor::GCX2TCX(_gfShape->m_cxform, _tcx);
                gld_shape *_subshape =  NULL;//new gld_shape();
                //_subshape->create();
                GShape2TShape(*_gfShape, &_subshape);
                _subshape->bound(_subshape->calc_bound());
                gld_rect _trc = _subshape->bound();
                _gfShape->m_bounds.top = _trc.top;
                _gfShape->m_bounds.left = _trc.left;
                _gfShape->m_bounds.bottom = _trc.bottom;
                _gfShape->m_bounds.right = _trc.right;
                _gfShape->m_ptr = _subshape;
                gld_shape _newtshape;
                _newtshape.create(*_subshape, _tmx, _tcx, NULL);
				_subshape->release(true);
				delete _subshape;
                _tshape->push_back(_newtshape);               
		    }			
	    }
		
		_tshape->bound(gld_rect(x1, y1, x2, y2));
		obj->m_bounds = _gtext->rtRegion;
		
		return;
    }   
}

// Create a dummy gld_shape(TShape) with given color
void CTransAdaptor::CreateDummyShape(gld_shape *tshape, TColor &tcolor)
{
    int _x1 = 0;
    int _y1 = 0;
    int _x2 = 2000;
    int _y2 = 2000;

	gld_draw_obj _d;
	gld_a_point _p1, _p2, _p3, _p4;
	gld_s_edge _e1, _e2, _e3, _e4, _e5, _e6;	
	
	tshape->create();	
	_d.create();	
	_p1.create(_x1, _y1);
	_p2.create(_x2, _y1);
	_p3.create(_x2, _y2);
	_p4.create(_x1, _y2);
	tshape->push_back(_d);
	_d.insert(_p1);
	_d.insert(_p2);
	_d.insert(_p3);
	_d.insert(_p4);

    TLineStyle *_ls = new TLineStyle(4, tcolor);
    TSolidFillStyle *_fs = new TSolidFillStyle(TColor(0, 0, 255, 64));
	int _lsIndex = _d.line_styles().Add(_ls);
    int _fsIndex = 0;//_d.fill_styles().Add(_fs);
	_e1.create(_p1, _p2, _lsIndex, _fsIndex, 0);
	_d.insert(_e1, gld_edge());
	_e2.create(_p2, _p3, _lsIndex, _fsIndex, 0);
	_d.insert(_e2, gld_edge());
	_e3.create(_p3, _p4, _lsIndex, _fsIndex, 0);
	_d.insert(_e3, gld_edge());
	_e4.create(_p4, _p1, _lsIndex, _fsIndex, 0);
	_d.insert(_e4, gld_edge());
	_e5.create(_p1, _p3, _lsIndex, _fsIndex, _fsIndex);
	_d.insert(_e5, gld_edge());
	_e6.create(_p2, _p4, _lsIndex, _fsIndex, _fsIndex);
	_d.insert(_e6, gld_edge());

    tshape->bound(gld_rect(_x1, _y1, _x2, _y2));
	tshape->normalize();
}

// Parse the given gobj's raw data and create the gld_shape(TShape) ptr(m_ptr)
void CTransAdaptor::CreateTShapePtr(CSWFParse *parser, gldObj *obj)
{
	GObjType _type = obj->GetGObjType();
    if (_type == gobjShape)
    {
	    // Parse the shape data to tshape
	    CTShapeParser _tparser;
        if (obj->m_dataLen > 0)
        {
            ASSERT(obj->m_data != NULL);
	        _tparser.Parse(obj->m_dataLen, (unsigned char *)obj->m_data);

            // Clear the original data to avoid the wrong parse when parse the shape again
            delete[] obj->m_data;
            obj->m_data = NULL;
            obj->m_dataLen = 0;
        }
        else
            _tparser.Parse((gldShape*)obj);
	    gld_shape *_tshape = _tparser._M_shape;

        // Set the gldShape tshape ptr
        gldShape *_gshape = (gldShape*)obj;
        _gshape->m_ptr = _tshape;

	    // Transform tshape to gldshape
//        CTransAdaptor::TShape2GShape(*_tshape, *_gshape);
    }
    else if (_type == gobjSprite || _type == gobjESprite)
    {
        gld_shape *_tshape = new gld_shape();
        _tshape->create_composite_shape();
        obj->m_ptr = _tshape;

        ((gldSprite*)obj)->m_mc->ParseMotionTween();
    }
    else if (_type == gobjButton)
    {
        gld_shape *_tshape = new gld_shape();
        _tshape->create_composite_shape();
        obj->m_ptr = _tshape;
    }
    else if (_type == gobjText)
    {
        gld_shape *_tshape = new gld_shape();
        _tshape->create_composite_shape();
        obj->m_ptr = _tshape;
    }
    else if (_type == gobjMShape)
    {
        gldMorphShape *_gmshape = (gldMorphShape*)obj;

		// create start shape ptr(composite shape)
	    gldShape *_gshape = _gmshape->m_startShape;
		gld_shape *_tcshape = new gld_shape();
		_tcshape->create_composite_shape();
		gld_shape *_tsshape = NULL;
	    CTransAdaptor::GShape2TShape(*_gshape, &_tsshape);
        _tsshape->normalize();
		_tcshape->push_back(*_tsshape);
        delete _tsshape;
        _gmshape->m_startShape->m_ptr = _tcshape;
		_tcshape->bound(_tcshape->calc_bound());
        _gmshape->m_ptr = _tcshape;		

		// create end shape ptr(composite shape)
	    _gshape = _gmshape->m_endShape;
	    _tcshape = new gld_shape();
		_tcshape->create_composite_shape();
		_tsshape = NULL;
	    CTransAdaptor::GShape2TShape(*_gshape, &_tsshape);
        _tsshape->normalize();
		_tcshape->push_back(*_tsshape);
        delete _tsshape;
		_tcshape->bound(_tcshape->calc_bound());
        _gmshape->m_endShape->m_ptr = _tcshape;
    }
	else if (_type == gobjImage)
	{		
		gldImage *_gimg = (gldImage *)obj;
		if (!CTraitImage::Binded(_gimg))
		{
			TImage *_timg = new TImage();        
			if (_gimg->m_gimgType == gimageJPEG)
			{
				TJPEGAdaptor _adaptor;
				U8 *imgData,*alphaData;
				U32 imgDataLen,alphaDataLen;

				_gimg->GetImageJPEGData(&imgData,&imgDataLen,&alphaData,&alphaDataLen);
				_adaptor.SetJPEGData(imgData,imgDataLen,alphaData,alphaDataLen);
				_timg->Load(&_adaptor);
			}
			else if (_gimg->m_gimgType == gimageBMP)
			{
				TWinBitmapAdaptor _adaptor;
				_adaptor.SetBitmapData(_gimg->GetRawData() + sizeof(BITMAPFILEHEADER), _gimg->GetRawDataLen() - sizeof(BITMAPFILEHEADER));
				_timg->Load(&_adaptor);
			}
			CTraitImage::Bind(_gimg, _timg);
			REFRENCE(_timg);
		}
	}
    else
    {
        //gld_shape *_tshape = new gld_shape();
        //CreateDummyShape(_tshape, TColor(255, 0, 255, 128));
        //obj->m_ptr = _tshape;
        obj->m_ptr = NULL;
    }
}

// Destroy the TShape(gld_shape) ptr(m_ptr) of given gobj
void CTransAdaptor::DestroyTShapePtr(gldObj *obj)
{
	GObjType _type = obj->GetGObjType();
	if (_type == gobjShape || _type == gobjSprite || _type == gobjESprite || _type == gobjButton) 
	{
		gld_shape *_tshape = (gld_shape*)obj->m_ptr;
		if (_tshape != NULL)
		{
			_tshape->destroy(true, false);
			delete _tshape;
			obj->m_ptr = NULL;
		}
	}
	else if (_type == gobjText)
	{
		gld_shape *_tshape = (gld_shape*)obj->m_ptr;
		if (_tshape != NULL)
		{
			_tshape->destroy(true, true);
			delete _tshape;
			obj->m_ptr = NULL;
		}
	}
	else if (_type == gobjFont)
	{
		gld_shape *_tshape = (gld_shape*)obj->m_ptr;
		if (_tshape != NULL)
		{
			_tshape->destroy(true, true);
			delete _tshape;
			obj->m_ptr = NULL;
		}
	}
	else if (_type == gobjMShape)
	{
		gldMorphShape *_gmshape = (gldMorphShape*)obj;
		// start shape
		gld_shape *_tshape = (gld_shape*)_gmshape->m_startShape->m_ptr;
		if (_tshape != NULL)
		{
			_tshape->destroy(true, true);
			delete _tshape;
			_gmshape->m_startShape->m_ptr = NULL;
		}
		// end shape
		_tshape = (gld_shape*)_gmshape->m_endShape->m_ptr;
		if (_tshape != NULL)
		{
			_tshape->destroy(true, true);
			delete _tshape;
			_gmshape->m_endShape->m_ptr = NULL;
		}

		_gmshape->m_ptr = NULL;
	}
	else if (_type == gobjImage)
	{
		TImage *pImage = CTraitImage((gldImage *)obj);
		RELEASE(pImage);
	}

    // Release the object's raw data
    if (obj->m_data != NULL)
    {
        ASSERT(obj->m_dataLen != 0);
        delete obj->m_data;
    }
    obj->m_dataLen = 0;
    obj->m_data = NULL;
}

// Convert the TShape to gldShape
void CTransAdaptor::ConverTShapePtr(gldObj *obj)
{
    if (obj->IsGObjInstanceOf(gobjShape))
    {
	    gldShape *_gshape = (gldShape*)obj;
	    gld_shape *_tshape = (gld_shape*)obj->m_ptr;

	    CTransAdaptor::TShape2GShape(*_tshape, *_gshape);
    }
//    else
//    {
//    }
}

// Return a new gldText object
gldText *CTransAdaptor::CreateText(void)
{
    gldText *_gtext = new gldText2();
    return _gtext;
}

// Return a new gldEffectSprite object
gldSprite *CTransAdaptor::CreateEffectSprite(void)
{
	ASSERT(FALSE);
	return NULL;

    //gldEffectSprite *_geffect = new gldEffectSprite();
    //return _geffect;
}

// Set the adaptor function ptr to given object library
void CTransAdaptor::SetAdaptor(gldLibrary *lib)
{
	lib->CreateObjPtr = CTransAdaptor::CreateTShapePtr;
	lib->DestroyObjPtr = CTransAdaptor::DestroyTShapePtr;
	lib->ConvertObjPtr = CTransAdaptor::ConverTShapePtr;
	lib->UpdateObjPtr = CTransAdaptor::UpdateTShape;
    lib->CreateTextPtr = CTransAdaptor::CreateText;
    lib->CreateEffectSpritePtr = CTransAdaptor::CreateEffectSprite;
}

// Return a new gldLibrary object and set the adaptor function ptr
gldLibrary *CTransAdaptor::CreateObjLibrary(bool setAdaptor)
{
    gldLibrary *_lib = new gldLibrary();
    if (setAdaptor)
    {
        SetAdaptor(_lib);
    }

    return _lib;
}

int CTransAdaptor::GLayerProp2TLayerProp(gldLayer *pLayer)
{
	ASSERT(pLayer);

	if (pLayer->GetFrameClip(pLayer->m_parentMC->m_curTime) == NULL)
	{
		return LA_NULL;
	}

	int nProp = LA_NORMAL;

    if (pLayer->m_locked)
	{
        nProp &= ~LA_UNLOCKED;
	}
    if (!pLayer->m_visible)
	{
        nProp &= ~LA_SHOW;
	}
    
    return nProp;
}

void CTransAdaptor::GMC2TFrame(gldMovieClip *pMC, gld_frame &frame)
{
    for (GLAYER_LIST::reverse_iterator iLayer = pMC->m_layerList.rbegin(); iLayer != pMC->m_layerList.rend(); iLayer++)
    {	
        frame.push_back(GLayer2TLayer(*iLayer));
    }

	if (pMC->m_curLayer)
	{
		frame.cur_layer(CTraitLayer(pMC->GetCurLayerPtr()));
	}
}

gld_layer CTransAdaptor::GLayer2TLayer(gldLayer *pLayer)
{
	ASSERT(pLayer);
	
	gld_layer	layer;

	layer.create();
	CTraitLayer::Attach(pLayer, layer);
	layer.attributes(GLayerProp2TLayerProp(pLayer));

	gldFrameClip *pFC = pLayer->GetFrameClip(pLayer->m_parentMC->m_curTime);

	if (pFC == NULL || !pLayer->m_visible)
	{
		return layer;
	}

	// Copy current time layer's character to shape editor
    GCHARACTER_LIST lstChar;
    pFC->GetShowList(lstChar, pLayer->m_parentMC->m_curTime);	
   
    for (GCHARACTER_LIST_IT iChar = lstChar.begin(); iChar != lstChar.end(); iChar++)
    {		
		layer.push_back(GCharacter2TShape(*iChar));
    }

    // Add the guidline
    for (GUIDLINE_LIST_IT iGL = pLayer->m_guidlineList.begin(); iGL != pLayer->m_guidlineList.end(); iGL++)
	{
        layer.guid_lines()->push_back(*iGL);
	}

	return layer;
}

gld_shape CTransAdaptor::GCharacter2TShape(gldCharacter *pChar)
{
	ASSERT(pChar);

	UpdateTShape(pChar->m_key->GetObj());

	TMatrix mx;
    TCxform cx;
    CTransAdaptor::GM2TM(pChar->m_matrix, mx);
    CTransAdaptor::GCX2TCX(pChar->m_cxform, cx);
	
	gld_shape	shape;
    shape.create((gld_shape)CTraitShape(pChar->m_key->GetObj()), mx, cx, NULL);
	shape.trans_origin(pChar->m_key->m_tx, pChar->m_key->m_ty);
	CTraitCharacter::Attach(pChar, shape);
	
	return shape;
}

void CTransAdaptor::ClearCurrentScene()
{
	CGuardDrawOnce	xGuad;

	if (my_app.Tools().Current() != -1)
	{
		my_app.CurTool()->Leave();
	}

	if (!my_app.CurSel().empty())
	{
		my_app.DoSelect(gld_shape_sel());
	}		
	
	my_app.SelectGuidLine(NULL);	

	// Get the shape list from the current frame of the shape editor
    gld_frame _frame = my_app.CurFrame();

    // Clear the frame container
    if (_frame.validate())
	{
		gld_layer_iter	ilayer = _frame.begin_layer();

		for(; ilayer != _frame.end_layer(); ++ilayer)
		{
			gld_layer	layer = *ilayer;
			ASSERT(layer.validate());

			gld_shape_iter	ishape = layer.begin_shape();
			for (; ishape != layer.end_shape(); ++ishape)
			{
				if ((*ishape).ptr() != NULL)
					CTraitInstance::Detach(*ishape);				
			}

			CTraitLayer::Detach(layer);
		}

        _frame.abandon();
	}

	// Clear auxliary line
	TShapeEditorAuxLine &al = my_app.GetAuxLine();
	al.HLines.clear();
	al.VLines.clear();	
}

void CTransAdaptor::BuildCurrentScene(bool notify)
{	
	gldScene2 *pCurScene = _GetCurScene2();
	if (pCurScene == NULL)
		return;

	// Create Layer
	gld_layer	layer;
	gld_frame	frame;

	frame = my_app.CurFrame();
	layer.create();
	frame.push_back(layer);

	my_app.SetCurLayer(layer);
	
	if (pCurScene->m_backgrnd != NULL)
		layer.push_back(pCurScene->m_backgrnd->GetEditShape());

	GINSTANCE_LIST &insts = pCurScene->m_instanceList;
	GINSTANCE_LIST::iterator iInst = insts.begin();
	for (; iInst != insts.end(); ++iInst)
	{
		gldInstance *pInst = *iInst;

		UpdateTShape(pInst->m_obj);

		TMatrix mx;
		TCxform cx;
		CTransAdaptor::GM2TM(pInst->m_matrix, mx);
		CTransAdaptor::GCX2TCX(pInst->m_cxform, cx);
		
		gld_shape	shape;
		shape.create((gld_shape)CTraitShape(pInst->m_obj), mx, cx, NULL);
		shape.trans_origin(pInst->m_tx, pInst->m_ty);
		shape.ptr(pInst);
		pInst->m_ptr = shape.get_row_data();
		//CTraitCharacter::Attach(pChar, shape);
		layer.push_back(shape);
	}
	
	
	TShapeEditorAuxLine &al = my_app.GetAuxLine();
	vector<int>::iterator y = pCurScene->m_hLines.begin();
	for (; y != pCurScene->m_hLines.end(); ++y)
	{
		al.HLines.push_back(*y);
	}

	vector<int>::iterator x = pCurScene->m_vLines.begin();
	for (; x != pCurScene->m_vLines.end(); ++x)
	{
		al.VLines.push_back(*x);
	}
	/*
	ASSERT(my_app.CurFrame().begin_layer() == my_app.CurFrame().end_layer());

	if (CSWFProxy::GetCurMovieClip())
	{
		// ¸¨ÖúÏß
		gldMovieClip *mc = CSWFProxy::GetCurMovieClip();
		TShapeEditorAuxLine &al = my_app.GetAuxLine();

		vector<int>::iterator y = mc->m_hLines.begin();
		for (; y != mc->m_hLines.end(); ++y)
		{
			al.HLines.push_back(*y);
		}

		vector<int>::iterator x = mc->m_vLines.begin();
		for (; x != mc->m_vLines.end(); ++x)
		{
			al.VLines.push_back(*x);
		}

		GMC2TFrame(CSWFProxy::GetCurMovieClip(), my_app.CurFrame());
	}
	*/

	if (notify)
		CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);
}

void CTransAdaptor::RebuildCurrentScene(bool notify)
{
	ClearCurrentScene();
	BuildCurrentScene(notify);
}

void CTransAdaptor::TSel2GSel(const gld_shape_sel &sel, gld_list<gldCharacterKey *> &gsel)
{
	gld_shape_sel::iterator isel = sel.begin();

	for (; isel != sel.end(); ++isel)
	{
		gld_shape	shape = *isel;

		ASSERT(shape.ptr());

		gldCharacter	*gchar = (gldCharacter *)shape.ptr();
		gldCharacterKey *gkey = gchar->m_key;

		ASSERT(gkey->m_ptr != NULL);
		ASSERT(gld_shape(gkey->m_ptr) == shape);

		gsel.push_back(gkey);
	}
}

void CTransAdaptor::GSel2TSel(const gld_list<gldCharacterKey *> &gsel, gld_shape_sel &sel)
{
	gld_list<gldCharacterKey *>::iterator isel = gsel.begin();

	for (; isel != gsel.end(); ++isel)
	{
		gldCharacterKey	*gkey = *isel;

		ASSERT(gkey->m_ptr);

		sel.select(gld_shape(gkey->m_ptr));
	}
}

void CTransAdaptor::OnUpdateObj(gldObj *pObj)
{
	gld_frame	   frame = my_app.CurFrame();
	gld_layer_iter iLayer = frame.begin_layer();

	my_app.SetRedraw(false);

	for (; iLayer != frame.end_layer(); ++iLayer)
	{
		gld_shape_iter iShape = (*iLayer).begin_shape();
		
		for (; iShape != (*iLayer).end_shape(); ++iShape)
		{
			gld_shape	shape = *iShape;

			ASSERT(shape.ptr());

			gldCharacter *pChar = (gldCharacter *)shape.ptr();

			if (pChar->m_key->GetObj() == pObj)
			{
				my_app.Redraw(_transform_rect(shape.bound(), shape.matrix()));
			}
		}		
	}

	my_app.SetRedraw(true);
	my_app.Repaint();
}

void CTransAdaptor::RecalcMotion(gldCharacterKey *pKey)
{
	gldFrameClip	*pFC = pKey->m_parentFC;
	gldLayer		*pLayer = pFC->m_parentLayer;
	gldFrameClip	*pPrevFC = pLayer->GetFrameClip(pFC->m_startTime - 1);

	if (pFC->IsMotion() && pFC->IsValid())
	{
		pFC->UpdateCharacters();
	}

	if (pPrevFC && pPrevFC->IsMotion() && pPrevFC->IsValid())
	{
		pPrevFC->UpdateCharacters();
	}
}
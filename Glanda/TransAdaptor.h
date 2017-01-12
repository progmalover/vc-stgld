#pragma once
#include "Matrix.h"
#include "Cxform.h"
#include "gld_graph.h"
#include "gldMatrix.h"
#include "gldCxform.h"
#include "gldShape.h"
#include "SWFParse.h"
#include "gld_selection.h"
#include "selection.h"

class gldSprite;
class gldButton;
class gldText;
class gldLibrary;

class CTransAdaptor
{
public:
    CTransAdaptor(void);
    virtual ~CTransAdaptor(void);

	DECLARE_SINGLETON(CTransAdaptor)

    // Transform the TColor to gldColor
    static void TC2GC(TColor &tc, gldColor &gc);
    // Transform the gldColor to TColor
    static void GC2TC(gldColor &gc, TColor &tc);

    // Transform the TMatrix to gldMatrix
    static void TM2GM(TMatrix &tm, gldMatrix &gm);
    // Transfor the gldMatrix to TMatrix
    static void GM2TM(gldMatrix &gm, TMatrix &tm);

    // Transform the TCxform matrix to gldCxform matrix
    static void TCX2GCX(TCxform &tcx, gldCxform &gcx);
    // Transform the gldCxform matrix to TCxform matrix
    static void GCX2TCX(gldCxform &gcx, TCxform &tcx);

    // Transform the TFillStyle to gldFillStyle
    static void TFS2GFS(TFillStyle &tfs, gldFillStyle &gfs);
    // Transform the gldFillStyle to TFillStyle
    static void GFS2TFS(gldFillStyle &gfs, TFillStyle **tfs);

    // Transform the TFillStyleTable to gldFillStyleTable
    static void TFST2GFST(TFillStyleTable &tfst, std::vector<gldFillStyle *> &gfst);
    // Transform the gldFillStyleTable to TFillStyleTable
    static void GFST2TFST(std::vector<gldFillStyle *> &gfst, TFillStyleTable &tfst);

    // Transform the TLineStyle to gldLineStyle
    static void TLS2GLS(TLineStyle &tls, gldLineStyle &gls);
    // Transform the gldLineStyle to TLineStyle
    static void GLS2TLS(gldLineStyle &gls, TLineStyle &tls);

    // Transform the TLineStyleTable to gldLineStyleTable
    static void TLST2GLST(TLineStyleTable &tlst, std::vector<gldLineStyle *> &glst);
    // Transform the gldLineStyleTable to TLineStyleTable
    static void GLST2TLST(std::vector<gldLineStyle *> &glst, TLineStyleTable &tlst);

    // Transform the gld_shape to gldShape
    static void TShape2GShape(gld_shape &tshape, gldShape &gshape);
    // Transform the gldShape to gls_shape
    static void GShape2TShape(gldShape &gshape, gld_shape **tshape);

    // Update the gobj's TShape ptr
    static void UpdateTShape(gldObj *obj);	
    // Convert the TShape to gldShape
    static void ConverTShapePtr(gldObj *obj);
    // Create a dummy gld_shape(TShape) with given color
    static void CreateDummyShape(gld_shape *tshape, TColor &tcolor);

    // Parse the given gobj's raw data and create the gld_shape(TShape) ptr(m_ptr)
    static void CreateTShapePtr(CSWFParse *parser, gldObj *obj);
    // Destroy the TShape(gld_shape) ptr(m_ptr) of given gobj
    static void DestroyTShapePtr(gldObj *obj);
	// Build current time scene 
	static void BuildCurrentScene(bool notify = true);
	static void ClearCurrentScene();
	static void RebuildCurrentScene(bool notify = true);

    // Return a new gldText object
    static gldText *CreateText(void);
    // Return a new gldEffectSprite object
    static gldSprite *CreateEffectSprite(void);
    // Set the adaptor function ptr to given object library
    static void SetAdaptor(gldLibrary *lib);
    // Return a new gldLibrary object and set the adaptor function ptr
    static gldLibrary *CreateObjLibrary(bool setAdaptor = true);

	static void TSel2GSel(const gld_shape_sel &sel, gld_list<gldCharacterKey *> &gsel);
	static void GSel2TSel(const gld_list<gldCharacterKey *> &gsel, gld_shape_sel &sel);

	static void OnUpdateObj(gldObj *pObj);	
	static void RecalcMotion(gldCharacterKey *pKey);

	// Send cswf library to shape editor	
	static void GMC2TFrame(gldMovieClip *pMC, gld_frame &frame);
	static gld_layer GLayer2TLayer(gldLayer *pLayer);
	static gld_shape GCharacter2TShape(gldCharacter *pChar);
	static int GLayerProp2TLayerProp(gldLayer *pLayer);
};

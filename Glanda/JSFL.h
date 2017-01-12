#pragma once

#include <vector>
#include <list>
#include "gldObj.h"

using namespace std;

struct tGuidLine;
class gldMovieClip;
class gldLayer;
class gldMorphShape;
class gldText2;
struct gldMotionTweenParam;

class CJSFLWriter
{
public:
	CJSFLWriter();
	virtual ~CJSFLWriter();

	void SetDocumentProperties(int width, int height, int rate, U32 backgroundColor);
	
	void AddNewSymbol(const char *type, const char *namePath);
	void RenameSymbol(const char *newName, const char *oldName);
	void EditSymbol(const char *namePath);
	void ExitEditMode();
	void AddSymbolToDocument(int x, int y, const char *namePath);

	void SelectLayer(int layer);
	void AddNewLayer(const char *type, const char *name, bool above = true);
	void AddMotionGuide();
	void SetLayerParent(int parentLayer);
	void SetLayerParent(int layer, int parentLayer);
	void SetLayerProperties(int layer, const char *type, const char *name);
	void SetLayerProperties(const char *type, const char *name);

	void SelectFrames(int startTime, int endTime);
	void InsertBlankKeyframe(int time);
	void ConvertToKeyframes();
	void ConvertToKeyframes(int startTime, int endTime);
	void ConvertToBlankKeyframes();
	void ConvertToBlankKeyframes(int startTime, int endTime);
	void InsertFrames(int numFrames, bool bAllLayers = false);
	void InsertFrames(int frame, int numFrames, bool bAllLayers = false);
	void SetFrameName(int layer, int frame, const char *name);
	void SetFrameName(const char *name);
	void SetFrameMotionTween(int layer, int frame, const char *type, bool snap, bool orient, const char *rotate, int rotateTimes, bool scale, int easing);
	void SetFrameMotionTween(const char *type, bool snap, bool orient, const char *rotate, int rotateTimes, bool scale, int easing);	
	void SetFrameActionScript(int layer, int frame, const char *actionScript);
	void SetFrameActionScript(const char *actionScript);
	void SetFrameSound(int layer, int frame, const char *libItemName, const char *sync, const char *loopMode, int loop, const char *effect);
	void SetFrameSound(const char *libItemName, const char *sync, const char *loopMode, int loop, const char *effect);

	void SelectInstance(int layer, int frame, int elem);
	void SelectInstance(int elem);
	void Transform(double e11, double e12, double e21, double e22, int dx, int dy);
	void Transform(int redPercent, int redAmount, int greenPercent, int greenAmount, int bluePercent, int blueAmount,int alphaPercent, int alphaAmount);
	void SetTransformationPoint(int x, int y);
	void SetInstanceActionScript(int layer, int frame, int elem, const char *actionScript);
	void SetInstanceActionScript(const char *actionScript);
	void BreakApart();

	bool Open(const char *fileName);	
	void Close();

	void BeginPath(int x, int y);
	void SetFillColor(const char *color);
	void SetStroke(const char *color, int size, const char *type);
	void CurveTo(int cx, int cy, int ax, int ay);
	void LineTo(int x, int y);
	void EndPath();

	void SetElementProperty(const CString &attr, const CString &value);

	void AddText(const CString &text, const CRect &rcText);
	void SetTextAttr(const CString &attr, const CString &value, int start, int end);
	void SetTextAttr(const CString &attr, const CString &value, int start);

	void Crlf();
private:
	int m_selLayer;
	int m_selStartFrame, m_selEndFrame;
	int m_selInstance;	
	int m_x, m_y;
	list<CString> m_editSymbols;

	char m_fileName[MAX_PATH + 1];
	CStdioFile m_jsflFile;
};

class CJSFLBuilder
{
public:
	CJSFLBuilder();
	virtual ~CJSFLBuilder();

	bool Build(const char *jsflFile);
	void BuildObjList();
	bool BuildDependentFiles(const char *path);
	void BuildJSFLFile();

private:
	static void SortObjList(GOBJECT_LIST &sorted, const GOBJECT_LIST &objs);
	static void SortObjListProc(GOBJECT_LIST &sorted, gldObj *pObj);
	static void ResetObjListIDs(GOBJECT_LIST &objs);
	void RenameSymbols(GOBJECT_LIST &objs);
	void BuildMovieSymbols(GOBJECT_LIST &objs);
	void BuildMovie(gldMovieClip *movie);	

	int BuildMaskedLayer(gldLayer *pLayer, int parentLayer, int layerPos, bool replace = false);
	int BuildMaskLayer(gldLayer *pLayer, int layerPos, bool replace = false);
	int BuildNormalLayer(gldLayer *pLayer, int layerPos, bool replace = false);
	void BuildGuidLine(tGuidLine *guidLine, int offsetX = 0, int offsetY = 0);
	void BuildText(gldText2 *text);
	void BuildStaticText(gldText2 *text);
	void BuildDynamicText(gldText2 *text);
	void BuildInputText(gldText2 *text);	
	void BuildMorphShape(gldMorphShape *mShape, int length, int id);

	void GetTextCharacters(gldText2 *text, CString &chars);
	void ESCReservedCharacter(CString &outText, const char *inText);
	bool IsReservedCharacter(char c);

	tGuidLine *GetSnapGuidLine(gldCharacterKey *key1, gldCharacterKey *key2, gldLayer *layer);
	CPoint GetLayerInstancesCenter(gldLayer *pLayer);
	CPoint GetSymbolCenter(gldObj *pObj);
	CRect GetSymbolRect(gldObj *pObj);

	void RemoveFiles(const char *path);

	static bool FlaCanImplMotion(const gldMotionTweenParam *motion);

public:
	GOBJECT_LIST m_SortedObjs;

	CJSFLWriter m_Writer;
	int m_nextObjID;
};
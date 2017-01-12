#pragma once

#include "GGraphics.h"
#include "Matrix.h"

#include <vector>
using namespace std;

class gldFillStyle;
class gldLineStyle;
class gldShapeRecord;
class gldShape;
class gldText;

class TShapeDraw
{
	enum GlsErrorCode
	{
		notEnoughData = 0xffff
	};

	enum GlsTag
	{ 
		stagEnd 				= 0,
		stagShowFrame 			= 1,
		stagDefineShape		 	= 2,
		stagFreeCharacter 		= 3,
		stagPlaceObject 		= 4,
		stagRemoveObject 		= 5,
		stagDefineBits 			= 6,
		stagDefineButton 		= 7,
		stagJPEGTables 			= 8,
		stagSetBackgroundColor	= 9,
		stagDefineFont			= 10,
		stagDefineText			= 11,
		stagDoAction			= 12,
		stagDefineFontInfo		= 13,
		stagDefineSound			= 14,	// Event sound tags.
		stagStartSound			= 15,
		stagDefineButtonSound	= 17,
		stagSoundStreamHead		= 18,
		stagSoundStreamBlock	= 19,
		stagDefineBitsLossless	= 20,	// A bitmap using lossless zlib compression.
		stagDefineBitsJPEG2		= 21,	// A bitmap using an internal JPEG compression table.
		stagDefineShape2		= 22,
		stagDefineButtonCxform	= 23,
		stagProtect				= 24,	// This file should not be importable for editing.

		stagPathsArePostScript	= 25,	// assume shapes are filled as PostScript style paths

		// These are the new tags for Flash 3.
		stagPlaceObject2		= 26,	// The new style place w/ alpha color transform and name.
		stagRemoveObject2		= 28,	// A more compact remove object that omits the character tag (just depth).

		// This tag is used for RealMedia only
		stagSyncFrame			= 29, // Handle a synchronization of the display list

		stagFreeAll				= 31, // Free all of the characters


		stagDefineShape3		=	32,	// A shape V3 includes alpha values.
		stagDefineText2			=	33,	// A text V2 includes alpha values.
		stagDefineButton2		=	34,	// A button V2 includes color transform, alpha and multiple actions
		stagDefineBitsJPEG3		=	35,	// A JPEG bitmap with alpha info.
		stagDefineBitsLossless2 =	36,	// A lossless bitmap with alpha info.
		stagDefineSprite		=	39,	// Define a sequence of tags that describe the behavior of a sprite.
		stagNameCharacter		=	40,	// Name a character definition, character id and a string, (used for buttons, bitmaps, sprites and sounds).
	    
		stagSerialNumber		=	41,	// a tag command for the Flash Generator customer serial id and cpu information
		stagDefineTextFormat	=	42,	// define the contents of a text block with formating information
		
		stagFrameLabel			=	43,	// A string label for the current frame.
		stagSoundStreamHead2	=	45,	// For lossless streaming sound, should not have needed this...
		stagDefineMorphShape	=	46,	// A morph shape definition
	    
		stagFrameTag			=	47,	// a tag command for the Flash Generator (WORD duration, STRING label)
		stagDefineFont2			=	48,	// a tag command for the Flash Generator Font information
		stagGenCommand			=	49,	// a tag command for the Flash Generator intrinsic
		stagDefineCommandObj	=	50,	// a tag command for the Flash Generator intrinsic Command
		stagCharacterSet		=	51,	// defines the character set used to store strings
		stagFontRef				=	52,   // defines a reference to an external font source

		// Flash 4 tags
		stagDefineEditText		=	37,	// an edit text object (bounds, width, font, variable name)
		stagDefineVideo			=	38,	// a reference to an external video stream

		// NOTE: If tag values exceed 255 we need to expand SCharacter::tagCode from a BYTE to a WORD
		stagDefineBitsPtr		=	1023  // a special tag used only in the editor
	};

	enum GlsFlag
	{
			// These flag codes are used for state changes - and as return values from ShapeParser::GetEdge()
			eflagsMoveTo	   =	0x01,
			eflagsFill0	   	   =	0x02,
			eflagsFill1		   =	0x04,
			eflagsLine		   =	0x08,
			eflagsNewStyles	   =	0x10,
			eflagsEnd 	   	   =	0x80  // a state change with no change marks the end
	};

	enum GlsFillStyle
	{
		Solid_Fill				=	0x00,
		Linear_Gradient_Fill	=   0x10,
		Radial_Gradient_Fill	=	0x12,
		Tiled_Bitmap_Fill		=	0x40,
		Clipped_Bitmap_Fill		=	0x41
	};

protected:
	virtual bool OnNewFillStyle(TFillStyle *fillstyle);
	virtual bool OnNewLineStyle(TLineStyle *linestyle);
	virtual void OnNewStyleTable();
	virtual void OnMoveTo(long x, long y);
	virtual void OnLineTo(long dx, long dy);
	virtual void OnBezierTo(long dcx, long dcy, long dax, long day);	
	virtual void OnSetFillStyle0(int num);
	virtual void OnSetFillStyle1(int num);
	virtual void OnSetLineStyle(int num);
private:	
	unsigned char	*m_pData;
	UINT			m_dataLen;

    unsigned long	m_tagStart;
	unsigned long	m_tagEnd;
	unsigned long	m_tagLen;
	long			m_bitPos;
	unsigned long	m_bitBuf;
	int				m_level;
	long			m_nFillBits;
	long			m_nLineBits;
	UINT			m_dataPos;

	bool ParseShapeRecord(bool bHasAlpha);
	bool ParseLineStyle(bool bHasAlpha);
	bool ParseFillStyle(bool bHasAlpha);
	bool ParseDefineShape(int level);

    bool ParseShapeRecord(vector<gldShapeRecord*> *gshapeRecords, bool bHasAlpha);
	bool ParseLineStyle(vector<gldLineStyle*> *glst, bool bHasAlpha);
	bool ParseFillStyle(vector<gldFillStyle*> *gfst, bool bHasAlpha);
	bool ParseDefineShape(gldShape *gshape, int level);
	bool ParseText(gldText *gtext);

    void Init();
    void backup();
	unsigned short GetTag();
	void InitBits();
	unsigned long GetDWord();
	unsigned char GetByte();
	long GetSBits (long n);
	void GetMatrix(TMatrix & matrix );
	void GetRect(GGRECT & rect );
	unsigned long GetBits(long n);
	unsigned short GetWord();		

    U32 GenImageData(U8 **data, U32 rawLength, U8* rawData, U32 jtableLength, U8* jtableData);
    U32 GenBMPData(U8 **data, U32 zipDataLen, U8* zipData, U32 width, U32 height, U32 bpp, U32 colorTabSize, BOOL withAlpha = false);

public:
	TShapeDraw();
	virtual ~TShapeDraw(void);

	virtual bool Draw(int dataLen, unsigned char *data, TGraphicsDevice *device);
    virtual bool Draw(gldShape *gshape, TGraphicsDevice *device);
	virtual bool Draw(gldText *gtext, TGraphicsDevice *device);
private:

	TGraphicsDevice	*m_pDevice;
	TLineStyleTable	m_LineStyleTab;
	TFillStyleTable m_FillStyleTab;
};

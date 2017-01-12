#include "StdAfx.h"
#include "tshapeparser.h"

#include "gldDataKeeper.h"
#include "gldLibrary.h"
#include "gldObj.h"
#include "gldImage.h"
#include "gldShape.h"
#include "gldGradient.h"
#include "gldGradientRecord.h"
#include "gldFillStyle.h"
#include "gldShapeRecord.h"
#include "gldShapeRecordChange.h"
#include "gldShapeRecordEdgeStraight.h"
#include "gldShapeRecordEdgeCurved.h"
#include "SWFProxy.h"
#include "TransAdaptor.h"

#include "Image.h"
#include "JPEG.h"
#include "Bitmap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern "C"
{
    #include "zlib.h"
}

extern void GetJPEGRect(const char* fileName, U32* width, U32* height);
extern void TransTableStream(U8* rawData, U32 rawSize, U8** dstData, U32* dstSize);
extern void UniteTableStream(U8* table, U32 tableSize, U8* data, U32 dataSize, U8** dstData, U32* dstSize);

void ValidFillStyle(gldFillStyle *_gfs)
{	
	FILLSTYLETYPE type = _gfs->GetFillStyleType();

	if (type == no_fill)
		goto err;
	else if (type == linear_gradient_fill || type == radial_gradient_fill)
	{
		if (_gfs->GetGradient() == NULL)
			goto err;
	}
	else if (type == tiled_bitmap_fill || type == clipped_bitmap_fill)
	{
		gldImage *img = _gfs->GetBitmap();
		if (img == NULL || img->GetGObjType() != gobjImage 
			|| img->GetWidth() == 0 || img->GetHeight() == 0)
			goto err;
	}

	return;

err:
	_gfs->SetSolidFillColor(gldColor(0, 0, 0, 0));
	_gfs->SetFillStyleType(solid_fill);
}

CTShapeParser::CTShapeParser()
{	
	m_dataLen = 0;
	m_pData = NULL;
}

CTShapeParser::~CTShapeParser(void)
{
}

unsigned long CTShapeParser::GetBits(long n)
{
    unsigned long v = 0;

    for (;;)
    {
        long s = n - m_bitPos;
        if (s > 0)
        {
            // Consume the entire buffer
            v |= m_bitBuf << s;
            n -= m_bitPos;

            // Get the next buffer
            m_bitBuf = GetByte();
            m_bitPos = 8;
        }
        else
        {
         	// Consume a portion of the buffer
            v |= m_bitBuf >> -s;
            m_bitPos -= n;
            m_bitBuf &= 0xff >> (8 - m_bitPos);	// mask off the consumed bits
            return v;
        }
    }
}

void CTShapeParser::GetRect(GGRECT &rect)
{
    InitBits();

    int nBits = (int) GetBits(5);

	rect.xmin = GetSBits(nBits);    
    rect.xmax = GetSBits(nBits);
    rect.ymin = GetSBits(nBits);
    rect.ymax = GetSBits(nBits);
}

void CTShapeParser::GetMatrix(TMatrix &matrix)
{
	float	e11, e12, e21, e22;
	long	dx, dy;

    InitBits();

    // Scale terms
    if (GetBits(1))
    {
        int nBits = (int) GetBits(5);
        e11 = (float)(GetSBits(nBits))/(float)0x10000;
        e22 = (float)(GetSBits(nBits))/(float)0x10000;
    }
    else
    {
     	e11 = e22 = 1.0;
    }

    // Rotate/skew terms
    if (GetBits(1))
    {
        int nBits = (int)GetBits(5);
        e12 = (float)(GetSBits(nBits))/(float)0x10000;
        e21 = (float)(GetSBits(nBits))/(float)0x10000;
    }
    else
    {
		e12 = e21 = 0.0;     	
    }

    // Translate terms
    int nBits = (int) GetBits(5);
    dx = GetSBits(nBits);
    dy = GetSBits(nBits);
	matrix.SetMatrix(e11, e12, e21, e22, dx, dy);
}

long CTShapeParser::GetSBits(long n)
{
    long v = (long) GetBits(n);

    // Is the number negative?
    if (v & (1L << (n - 1)))
    {
        // Yes. Extend the sign.
        v |= -1L << n;
    }

    return v;
}

unsigned char CTShapeParser::GetByte()
{
    InitBits();
	return m_pData[m_dataPos++];
}

unsigned short CTShapeParser::GetWord()
{
    unsigned char * s = m_pData + m_dataPos;
    m_dataPos += 2;
    InitBits();
    return (unsigned short) s[0] | ((unsigned short) s[1] << 8);
}

unsigned long CTShapeParser::GetDWord()
{
    unsigned char * s = m_pData + m_dataPos;
    m_dataPos += 4;
    InitBits();
    return (unsigned long) s[0] | ((unsigned long) s[1] << 8) | ((unsigned long) s[2] << 16) | ((unsigned long) s [3] << 24);
}

bool CTShapeParser::Parse(int dataLen, unsigned char *data)
{
    m_dataLen = dataLen;
    m_pData = data;

    ASSERT(m_pData);

	if ( m_pData == NULL )
	{		
		return false;
	}

	Init();

	unsigned short code = GetTag();
	if ( code == notEnoughData ) {
		//数据长度不够
		return false;
	}
	if (m_tagEnd > m_dataLen ) {
		//数据长度不够
	    return false;
	}

	// Create the TShape
	_M_shape = new gld_shape();
	_M_shape->create();

	_M_cur_do = gld_draw_obj();
	_M_cur_pos = gld_a_point();
	_M_prev_edge = gld_edge();
	_M_x = 0;
	_M_y = 0;
	_M_cur_ls = 0;
	_M_cur_fs0 = 0;
	_M_cur_fs1 = 0;

	// Parse the data
	m_FillType = 0;
	bool retCode;
	switch ( code ) {
		case stagDefineShape: 
			retCode = ParseDefineShape(1);
			break;

		case stagDefineShape2:
			retCode = ParseDefineShape(2);
			break;

		case stagDefineShape3:
			retCode = ParseDefineShape(3);
			break;

		default:
			retCode = false;
	}	

	if (!retCode)
	{
		_M_shape->destroy(true, true);
	}
	
	return retCode;
}

void CTShapeParser::InitBits()
{
    m_bitPos = 0;
    m_bitBuf = 0;
}

unsigned short CTShapeParser::GetTag()
{
    m_tagStart = m_dataPos;

    if (m_dataLen-m_dataPos < 2) return notEnoughData;

    // Get the combined code and length of the tag.
    unsigned short code = GetWord();

    // The length is encoded in the tag.
    unsigned long len = code & 0x3f;

    // Remove the length from the code.
    code = code >> 6;

    // Determine if another long word must be read to get the length.
    if (len == 0x3f) {
        if (m_dataLen-m_dataPos < 4) return notEnoughData;
    	len = (unsigned long) GetDWord();
    }

    // Determine the end position of the tag.
    m_tagEnd = m_dataPos + (unsigned long) len;
    m_tagLen = (unsigned long) len;

    return code;

}

void CTShapeParser::Init()
{
    m_dataPos = 0;
	m_tagStart = 0;
	m_tagEnd = 0;
	m_tagLen = 0;
	m_level = 0;
	m_nFillBits = 0;
	m_nLineBits	= 0;

	m_bound.xmin = 0;
	m_bound.xmax = 0;
	m_bound.ymin = 0;
	m_bound.ymax = 0;

	InitBits();
}

bool CTShapeParser::ParseDefineShape( int level )
{
	unsigned short tagID = GetWord();

	m_level = level;
    
	GetRect(m_bound);    
	OnGetBound(m_bound);

	bool bHasAlpha = (level == 3);

	bool retCode;
	retCode = ParseFillStyle( bHasAlpha );

	if ( !retCode ) {
		// error
		return retCode;
	}

	retCode = ParseLineStyle( bHasAlpha );
	if ( !retCode ) {
		// error
		return retCode;
	}

    InitBits();
    m_nFillBits = (unsigned short) GetBits(4);
    m_nLineBits = (unsigned short) GetBits(4);

	retCode = ParseShapeRecord( bHasAlpha );
	if ( !retCode ) {
		// error
		return retCode;
	}

	return retCode;
}

bool CTShapeParser::ParseFillStyle(bool bHasAlpha)
{
	// Get the number of fills.
	unsigned short nFills = GetByte();

	// Do we have a larger number?
	if (nFills == 255)
	{
		// Get the larger number.
		nFills = GetWord();
	}

	int i;	
	// Get each of the fill style.
	for (i = 0; i < nFills; i++)
	{			
		unsigned short fillStyle = GetByte();
		if (fillStyle & fillGradient)
        {
			TGradientFillStyle *pfs;
            if (fillStyle == Linear_Gradient_Fill)
            {
                pfs = new TLinearGradientFillStyle;
            }
            else
            {
                pfs = new TRadialGradientFillStyle;
            }
			TMatrix matrix;
			// Get the gradient matrix.
			GetMatrix(matrix);
			pfs->SetMatrix(matrix);
			// Get the number of colors.
			unsigned short nbGradients = GetByte();
			// Get each of the colors.
			for (unsigned short j = 0; j < nbGradients; j++)
			{				
				U8	ratio = GetByte();
				U8	red = GetByte();
				U8	green = GetByte();
				U8	blue = GetByte();
				U8	alpha = 255;
				if ( bHasAlpha ) {
					alpha = GetByte();
				}
				pfs->AddRecord(TColor(red, green, blue, alpha), ratio);
			}			
			if (!OnNewFillStyle(pfs))
			{
				delete pfs;
			}
		} 		
		else if (fillStyle & fillTiledBits)
        {
			// Get the image ID
            unsigned short imgID = GetWord();

			// Get the bitmap matrix.
			TMatrix matrix;
			GetMatrix(matrix);

            // Try to find the image object from the library
            gldLibrary *_lib = gldDataKeeper::Instance()->m_objLib;
			gldObj *_gobj = _lib->FindObjByID(imgID);
			if (_gobj == NULL || !_gobj->IsGObjInstanceOf(gobjImage))
			{
				TSolidFillStyle *pfs = new TSolidFillStyle;

                pfs->SetColor(TColor(0, 0, 0, 0));
			    if (!OnNewFillStyle(pfs))
			    {
				    delete pfs;
			    }
			}
			else
			{
				gldImage *_gimage = (gldImage *)_gobj;				
            
                TBitmapFillStyle *pfs;
                if (fillStyle == Tiled_Bitmap_Fill)
                    pfs = new TTiledBitmapFillStyle();
                else
                    pfs = new TClippedBitmapFillStyle();

			    ASSERT(CTraitImage::Binded(_gimage));

                // Set the bitmap
                pfs->SetImage(CTraitImage(_gimage));

                // Set the matrix to image fill style
                pfs->SetMatrix(matrix);

			    if (!OnNewFillStyle(pfs))
			    {
				    delete pfs;
			    }
            }            
		}
		else {
			TSolidFillStyle *pfs = new TSolidFillStyle;
			// A solid color
			U8	red = GetByte();
			U8	green = GetByte();
			U8	blue = GetByte();
			U8	alpha = 255;
			if ( bHasAlpha) {
				alpha = GetByte();
			}
			pfs->SetColor(TColor(red, green, blue, alpha));
			if (!OnNewFillStyle(pfs))
			{
				delete pfs;
			}
		}		
	}

	return true;
}

bool CTShapeParser::ParseLineStyle(bool bHasAlpha)
{

    // Get the number of lines.
    unsigned short nLines = GetByte();
    // Do we have a larger number?
    if (nLines == 255)
    {
		// Get the larger number.
		nLines = GetWord();
    }
    // Get each of the line styles.
    for (long i = 0; i < nLines; i++)
    {
		TLineStyle *pls = new TLineStyle;
		pls->SetWidth( GetWord() );		
		TColor	color;
		color.m_r = GetByte();
		color.m_g = GetByte();
		color.m_b = GetByte();
		if ( bHasAlpha ) 
		{
			color.m_a = GetByte();
		}
		pls->SetColor(color);
		if (!OnNewLineStyle(pls))
		{
			delete pls;
		}		
    }
	return true;
}

bool CTShapeParser::ParseShapeRecord(bool bHasAlpha)
{		
	long	dx, dy;
	long	ax, ay, cx, cy; 

	while( 1 ) {
		// Determine if this is an edge.
		BOOL isEdge = (BOOL) GetBits(1);

		if (!isEdge)
		{
			// Handle a state change
			unsigned short flags = (unsigned short) GetBits(5);

			// Are we at the end?
			if (flags == 0) 
			{
				// End of shape
				return true;
			}

			// Are we at the end?
			if ( flags & eflagsEnd ) {
				// End of shape
				return true;
			}
			
			// Process a move to.
			if (flags & eflagsMoveTo)
			{
				unsigned short nBits = (unsigned short) GetBits(5);
				dx = GetSBits(nBits);
				dy = GetSBits(nBits);
				OnMoveTo(dx, dy);
			}

			// Get new fill info.
			if (flags & eflagsFill0)
			{
				OnSetFillStyle0(GetBits(m_nFillBits));				
			}

			if (flags & eflagsFill1)
			{
				int	fillid = GetBits(m_nFillBits);
				if (fillid != 0)
				{
					m_FillType = 1;
				}
				OnSetFillStyle1(fillid);				
			}

			// Get new line info
			if (flags & eflagsLine)
			{
				OnSetLineStyle(GetBits(m_nLineBits));				
			}

			// Check to get a new set of styles for a new shape layer.
			if (flags & eflagsNewStyles)
			{
				OnNewStyleTable();

				// Parse the style.
				ParseFillStyle( bHasAlpha );
				ParseLineStyle( bHasAlpha );

				InitBits();	// Bug !

				// Reset.
				m_nFillBits = (unsigned short) GetBits(4);
				m_nLineBits = (unsigned short) GetBits(4);
			}			
		}
		else { // is Edge
			if (GetBits(1)) { // Straight Edge				
				// Handle a line
				unsigned short nBits = (unsigned short) GetBits(4) + 2;	// nBits is biased by 2
				
				if (GetBits(1)) {
					// Handle a general line.
					dx = GetSBits(nBits);
					dy = GetSBits(nBits);
					OnLineTo(dx, dy);		            
				}
				else {
					// Handle a vert or horiz line.
					if ( GetBits(1) ) {
						OnLineTo(0, GetSBits(nBits));						
					}else{
						OnLineTo(GetSBits(nBits), 0);						
					}
				}
			}
			else {
				// Handle a curve
				unsigned short nBits = (unsigned short) GetBits(4) + 2;	// nBits is biased by 2
				cx = GetSBits(nBits);
				cy = GetSBits(nBits);
				ax = GetSBits(nBits);
				ay = GetSBits(nBits);
				OnBezierTo(cx, cy, ax, ay);				
			}
		}
	}
	return true;
}

GGRECT CTShapeParser::GetBound()
{
	return m_bound;
}

int CTShapeParser::GetFillType()
{
	return m_FillType;
}


void CTShapeParser::OnNewStyleTable()
{
	_M_cur_do.create();
	_M_shape->push_back(_M_cur_do);
	_M_cur_pos = gld_a_point();
	_M_prev_edge = gld_edge();
	_M_cur_ls = 0;
	_M_cur_fs0 = 0;
	_M_cur_fs1 = 0;	
}

bool CTShapeParser::OnNewFillStyle(TFillStyle *fillstyle)
{	
	if (!_M_cur_do.validate()) {
		OnNewStyleTable();
	}
	_M_cur_do.fill_styles().Add(fillstyle);
	return true;
}

bool CTShapeParser::OnNewLineStyle(TLineStyle *linestyle)
{	
	if (!_M_cur_do.validate()) {
		OnNewStyleTable();
	}
	_M_cur_do.line_styles().Add(linestyle);
	return true;
}

void CTShapeParser::OnSetFillStyle0(int num)
{
	_M_cur_fs0 = num;
}

void CTShapeParser::OnSetFillStyle1(int num)
{	
	_M_cur_fs1 = num;
}

void CTShapeParser::OnSetLineStyle(int num)
{
	_M_cur_ls = num;
}

void CTShapeParser::OnMoveTo(long x, long y)
{	                
	if (!_M_cur_do.validate()) {
		OnNewStyleTable();
	}
	_M_prev_edge = gld_edge();
	_M_cur_do.insert(_M_cur_pos.create(x, y));
	_M_x = x;
	_M_y = y;	
}

void CTShapeParser::OnLineTo(long dx, long dy)
{
	if (!_M_cur_pos.validate()) {
		OnMoveTo(_M_x, _M_y);
	}	

	_M_x += dx;
	_M_y += dy;

	gld_a_point _p;	
	_M_cur_do.insert(_p.create(_M_x, _M_y));
	gld_s_edge _e;
	_e.create(_M_cur_pos, _p, _M_cur_ls, _M_cur_fs0, _M_cur_fs1);
	_M_prev_edge = _M_cur_do.insert(_e, _M_prev_edge);
	_M_cur_pos = _p;
}

void CTShapeParser::OnBezierTo(long dcx, long dcy, long dax, long day)
{
	if (!_M_cur_pos.validate()) {
		OnMoveTo(_M_x, _M_y);
	}
	_M_x += dcx;
	_M_y += dcy;
	gld_c_point _c;
	_M_cur_do.insert(_c.create(_M_x, _M_y));
	_M_x += dax;
	_M_y += day;
	gld_a_point _a;
	_M_cur_do.insert(_a.create(_M_x, _M_y));
	gld_q_edge _e;
	_e.create(_M_cur_pos, _c, _a, _M_cur_ls, _M_cur_fs0, _M_cur_fs1);
	_M_prev_edge = _M_cur_do.insert(_e, _M_prev_edge);
	_M_cur_pos = _a;
}

void CTShapeParser::OnGetBound(const GGRECT &bound)
{		
	gld_rect _b(bound.xmin, bound.ymin, bound.xmax, bound.ymax);
	_M_shape->bound(_b);
}

U32 CTShapeParser::GenImageData(U8 **data, U32 rawLength, U8* rawData, U32 jtableLength, U8* jtableData)
{
    // Backup the shape data
    UINT _oldLen = m_dataLen;
    unsigned char *_oldData = m_pData;
    UINT _oldPos = m_dataPos;

    // Set the image data to buffer
    m_dataLen = rawLength;
    m_pData = rawData;
    Init();

    unsigned short type = GetTag();
    U32 rtnLength = 0;
    switch (type)
    {
        case stagDefineBitsJPEG2:
        case stagDefineBitsJPEG3:
        {
			ASSERT(FALSE);
            // Jump to the jpeg data part
			//GetWord();  // skip id
   //         U32 alphaOffset = 0;
   //         // skip the alpha offset in JPEG3
   //         if (type == stagDefineBitsJPEG3)
   //             alphaOffset = GetDWord();

   //         U8* rawData = (U8*)m_pData + m_dataPos;
   //         U32 rawSize = m_tagEnd - m_dataPos;
   //         if (type == stagDefineBitsJPEG3)
   //             rawSize = alphaOffset;

   //         // Transform the SWF format jpeg to official format
   //         U8* dstData = NULL;
   //         U32 dstSize = 0;
   //         TransTableStream(rawData, rawSize, &dstData, &dstSize);

   //         // Set the result data and return the data length
   //         *data = dstData;
   //         rtnLength = dstSize;
            break;
        }
        case stagDefineBits:
        {
			ASSERT(FALSE);
				//// Get data and dataSize
				//GetWord();  // skip image id
				//U8* rawData = (U8*)m_pData + m_dataPos;
				//U32 rawSize = m_tagEnd - m_dataPos;

				//// Get table and tableSize
				//int offset = 2;
				//U8 b1 = jtableData[0];
				//U8 b2 = jtableData[1];
				//unsigned short jtableTagCode = ((b1 << 8) || b2);
				//unsigned short jtableTagLen = jtableTagCode & 0x3f;
				//jtableTagCode = jtableTagCode >> 6;
				//ASSERT(jtableTagCode == stagJPEGTables);
				//if (jtableTagLen == 0x3f)
    //				offset += 4;
				//U8* table = (U8*)jtableData + offset;
				//U32 tableSize = jtableLength - offset;

				//// Unite the encoding table and the image data
				//U8* dstData = NULL;
				//U32 dstSize = 0;
				//UniteTableStream(table, tableSize, rawData, rawSize, &dstData, &dstSize);

				//// Set the result data and return the data length
				//*data = dstData;
				//rtnLength = dstSize;
            break;
        }
        case stagDefineBitsLossless:
        case stagDefineBitsLossless2:
        {
            UINT bmpID = GetWord();
            U8 format = GetByte();
            UINT width = GetWord();
            UINT height = GetWord();
            U32 colorTabSize = 0;
            U8 bpp = 1;
            if (format == 3)
            {
                colorTabSize = GetByte();
                bpp = 1;
            }
            else if (format == 4)
            {
//                colorTabSize = GetWord();
                colorTabSize = 0;
                bpp = 2;
            }
            else if (format == 5)
            {
//                colorTabSize = GetDWord();
                colorTabSize = 0;
                bpp = 4;
            }
            else
                assert(0);
            U32 zipDataLen = m_tagEnd - m_dataPos;
            U8* zipData = new U8[zipDataLen];

            U32 _datalen = 0;
            if (type == stagDefineBitsLossless)
                _datalen = GenBMPData(data, zipDataLen, zipData, width, height, bpp, colorTabSize);
            else
                _datalen = GenBMPData(data, zipDataLen, zipData, width, height, bpp, colorTabSize, true);

            delete[] zipData;
            rtnLength = _datalen;
            break;
        }
        default:
            *data = NULL;
            rtnLength = 0;
            break;
    }

    // Restore the shape data
    m_dataLen = _oldLen;
    m_pData = _oldData;
    m_dataPos = _oldPos;
    this->InitBits();

    return rtnLength;
}

U32 CTShapeParser::GenBMPData(U8 **data, U32 zipDataLen, U8* zipData, U32 width, U32 height, U32 bpp, U32 colorTabSize, BOOL withAlpha)
{
    // Unzip the color table
    U32 tableSize = colorTabSize;
    
    if (bpp == 1)
        tableSize++;

    z_stream stream;
    U8* buffer = zipData;
    U8 colorNum = 3;
    if (withAlpha)
        colorNum = 4;
    U8* colorTable = new U8[tableSize*colorNum];

    stream.next_in = buffer;
    stream.avail_in = 1;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;

    stream.next_out = colorTable;
    stream.avail_out = tableSize*colorNum;

    inflateInit(&stream);

    int status;
    while (1)
    {
        status = inflate(&stream, Z_SYNC_FLUSH);
        if (status == Z_STREAM_END)
            break;
        if (status != Z_OK)
        {
            inflateEnd(&stream);
            delete[] colorTable;
            colorTable = NULL;

            *data = NULL;
            return 0;
        }
        stream.avail_in = 1;
        // Colormap if full
        if (stream.avail_out == 0)
            break;
    }

    // Unzip the rawdata
    U32 padWidth = width * bpp;
    if (padWidth % 4 != 0)
        padWidth = (padWidth / 4 + 1) * 4;

    U32 dataLen = padWidth * height;
    U8* rawdata = new U8[dataLen];

    stream.next_out = rawdata;
    stream.avail_out = dataLen;

    while (1)
    {
        status = inflate(&stream, Z_SYNC_FLUSH) ;
        if (status == Z_STREAM_END)
            break;

        if (status != Z_OK)
        {
            inflateEnd(&stream);
            delete[] colorTable;
            delete[] rawdata;
            colorTable = NULL;
            rawdata = NULL;

            *data = NULL;
            return 0;
        }
        stream.avail_in = 1;
    }

    inflateEnd(&stream);

    // Adjust the raw data to BMP data
    U8* vData = new U8[dataLen];
    U8* ptrDst = vData;
    U8* ptrSrc = rawdata + padWidth * (height - 1);
    for (int i = 0; i < (int)(height); i++)
    {
        // 32 bits color, exchange the rgba
        if (bpp == 4)
        {
            U8* curDst = ptrDst;
            U8* curSrc = ptrSrc;
            for (int j = 0; j < (int)(padWidth/4); j++)
            {
                curDst[j*4]   = curSrc[j*4+3];
                curDst[j*4+1] = curSrc[j*4+2];
                curDst[j*4+2] = curSrc[j*4+1];
                curDst[j*4+3] = curSrc[j*4];
            }
        }
        // 16 bits color, 565 format, exchange the rgb
        else if (bpp == 2)
        {
            U8* curDst = ptrDst;
            U8* curSrc = ptrSrc;
            for (int j = 0; j < (int)(padWidth/2); j++)
            {
                curDst[j*4]   = curSrc[j*4]   & 0x07;   // 0000 0111
                curDst[j*4+1] = curSrc[j*4+1] & 0xe0;   // 1110 0000
                curDst[j*4]   |= ((curSrc[j*4+1]&0x1f)<<3);
                curDst[j*4+1] |= ((curSrc[j*4]&0xf8)>>3);
            }
        }
        // 8 bits color
        else
            memcpy(ptrDst, ptrSrc, padWidth);

        ptrDst += padWidth;
        ptrSrc -= padWidth;
    }

    delete[] colorTable;
    delete[] rawdata;

    *data = vData;
    return dataLen;
}

bool CTShapeParser::ParseShapeRecord(vector<gldShapeRecord*> *gshapeRecords, bool bHasAlpha)
{		
	long dx, dy;
	long ax, ay, cx, cy;

    for (vector<gldShapeRecord*>::iterator it = gshapeRecords->begin(); it != gshapeRecords->end(); it++)
    {
        // Get the record
        gldShapeRecord *_rec = (*it);

        // End record
        if (_rec->GetGShapeRecType() == gshapeRecEnd)
        {
            return true;
        }
        // Change record
        else if (_rec->GetGShapeRecType() == gshapeRecChange)
		{
            gldShapeRecordChange *_recChange = (gldShapeRecordChange*)_rec;

			// Handle a state change
			// Process a move to
            if (_recChange->m_stateMoveTo)
			{
                dx = _recChange->m_moveDX;
				dy = _recChange->m_moveDY;
				OnMoveTo(dx, dy);
			}

			// Get new fill info
            if (_recChange->m_stateFS0)
			{
                OnSetFillStyle0(_recChange->m_fs0);				
			}

			if (_recChange->m_stateFS1)
			{
				int	fillid = _recChange->m_fs1;
				if (fillid != 0)
				{
					m_FillType = 1;
				}
				OnSetFillStyle1(fillid);				
			}

			// Get new line info
            if (_recChange->m_stateLS)
			{
                OnSetLineStyle(_recChange->m_ls);				
			}

			// Check to get a new set of styles for a new shape layer
            if (_recChange->m_stateNewStyles)
			{
				OnNewStyleTable();

				// Parse the styles
                ParseFillStyle(&(_recChange->m_fsTable), bHasAlpha);
                ParseLineStyle(&(_recChange->m_lsTable), bHasAlpha);
			}			
		}
        // Straignt edge record
        else if (_rec->GetGShapeRecType() == gshapeRecEdgeStraight)
        {
            gldShapeRecordEdgeStraight *_recEdgeS = (gldShapeRecordEdgeStraight *)_rec;

            dx = _recEdgeS->m_dx;
			dy = _recEdgeS->m_dy;
			OnLineTo(dx, dy);
		}
        // Curved edge record
        else if (_rec->GetGShapeRecType() == gshapeRecEdgeCurved)
		{
            gldShapeRecordEdgeCurved *_recEdgeC = (gldShapeRecordEdgeCurved *)_rec;
            cx = _recEdgeC->m_cx;
			cy = _recEdgeC->m_cy;
            ax = _recEdgeC->m_ax;
			ay = _recEdgeC->m_ay;
			OnBezierTo(cx, cy, ax, ay);				
		}
        // Wrong record
        else
        {
            ASSERT(false);
        }
	}
	return true;
}

bool CTShapeParser::ParseLineStyle(vector<gldLineStyle*> *glst, bool bHasAlpha)
{
    // Pare each of the line styles.
    for (vector<gldLineStyle*>::iterator it = glst->begin(); it != glst->end(); it++)
    {
        gldLineStyle *_gls = (*it);
		TLineStyle *_tls = new TLineStyle;
        CTransAdaptor::GLS2TLS(*_gls, *_tls);

        if (!OnNewLineStyle(_tls))
		{
			delete _tls;
		}		
    }
	return true;
}

bool CTShapeParser::ParseFillStyle(vector<gldFillStyle*> *gfst, bool bHasAlpha)
{
    // Pare each of the fill styles.
    for (vector<gldFillStyle*>::iterator it = gfst->begin(); it != gfst->end(); it++)
    {
        gldFillStyle *_gfs = (*it);
		
		ValidFillStyle(_gfs);

        TFillStyle *_tfs = NULL;
        CTransAdaptor::GFS2TFS(*_gfs, &_tfs);
        ASSERT(_tfs!= NULL);
		if (!OnNewFillStyle(_tfs))
		{
			delete _tfs;
		}
    }

	return true;
}

bool CTShapeParser::ParseDefineShape(gldShape *gshape, int level)
{
	m_level = level;
    
    CRect _rc = gshape->m_bounds;
    m_bound.xmin = _rc.left;
    m_bound.xmax = _rc.right;
    m_bound.ymin = _rc.top;
    m_bound.ymax = _rc.bottom;
    OnGetBound(m_bound);

    bool bHasAlpha = (gshape->GetShapeType() == 3);

	bool retCode;
    retCode = ParseFillStyle(gshape->GetFillStyles(), bHasAlpha);

	if (!retCode)
    {
		// error
		return retCode;
	}

    retCode = ParseLineStyle(gshape->GetLineStyles(), bHasAlpha);
	if (!retCode)
    {
		// error
		return retCode;
	}

    retCode = ParseShapeRecord(gshape->GetShapeRecords(), bHasAlpha);
	if (!retCode)
    {
		// error
		return retCode;
	}

	return retCode;
}

bool CTShapeParser::Parse(gldShape *gshape)
{
    m_dataLen = 0;
    m_pData = NULL;

	Init();

	// Create the TShape
	_M_shape = new gld_shape();
	_M_shape->create();

	_M_cur_do = gld_draw_obj();
	_M_cur_pos = gld_a_point();
	_M_prev_edge = gld_edge();
	_M_x = 0;
	_M_y = 0;
	_M_cur_ls = 0;
	_M_cur_fs0 = 0;
	_M_cur_fs1 = 0;

    bool rtnFlag = ParseDefineShape(gshape, gshape->GetShapeType());
    return rtnFlag;
}

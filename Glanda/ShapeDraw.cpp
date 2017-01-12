#include "StdAfx.h"
#include "ShapeDraw.h"
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
#include "gldText.h"
#include "gldTextGlyphRecord.h"
#include "gldFont.h"
#include "TransAdaptor.h"

#include "Image.h"
#include "JPEG.h"
#include "Bitmap.h"

extern "C"
{
    #include "zlib.h"
}

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern void GetJPEGRect(const char* fileName, U32* width, U32* height);
extern void TransTableStream(U8* rawData, U32 rawSize, U8** dstData, U32* dstSize);
extern void UniteTableStream(U8* table, U32 tableSize, U8* data, U32 dataSize, U8** dstData, U32* dstSize);

TShapeDraw::TShapeDraw()
{	
	m_dataLen = 0;
	m_pData = NULL;
	m_pDevice = NULL;
}

TShapeDraw::~TShapeDraw(void)
{
}

unsigned long TShapeDraw::GetBits(long n)
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

void TShapeDraw::GetRect(GGRECT &rect)
{
    InitBits();

    int nBits = (int) GetBits(5);

	rect.xmin = GetSBits(nBits);    
    rect.xmax = GetSBits(nBits);
    rect.ymin = GetSBits(nBits);
    rect.ymax = GetSBits(nBits);
}

void TShapeDraw::GetMatrix(TMatrix &matrix)
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

long TShapeDraw::GetSBits(long n)
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

unsigned char TShapeDraw::GetByte()
{
    InitBits();
	return m_pData[m_dataPos++];
}

unsigned short TShapeDraw::GetWord()
{
    unsigned char * s = m_pData + m_dataPos;
    m_dataPos += 2;
    InitBits();
    return (unsigned short) s[0] | ((unsigned short) s[1] << 8);
}

unsigned long TShapeDraw::GetDWord()
{
    unsigned char * s = m_pData + m_dataPos;
    m_dataPos += 4;
    InitBits();
    return (unsigned long) s[0] | ((unsigned long) s[1] << 8) | ((unsigned long) s[2] << 16) | ((unsigned long) s [3] << 24);
}

bool TShapeDraw::Draw(int dataLen, unsigned char *data, TGraphicsDevice *device)
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

	assert(device);
	m_pDevice = device;
	m_pDevice->Render();
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();

	// Parse the data	
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
	
	m_pDevice->SetFillStyleTable(&m_FillStyleTab);
	m_pDevice->SetLineStyleTable(&m_LineStyleTab);
	m_pDevice->Render();
	m_pDevice->SetFillStyleTable(0);
	m_pDevice->SetLineStyleTable(0);
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();
	m_pDevice = NULL;

	return retCode;
}

void TShapeDraw::InitBits()
{
    m_bitPos = 0;
    m_bitBuf = 0;
}

unsigned short TShapeDraw::GetTag()
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

void TShapeDraw::Init()
{
    m_dataPos = 0;
	m_tagStart = 0;
	m_tagEnd = 0;
	m_tagLen = 0;
	m_level = 0;
	m_nFillBits = 0;
	m_nLineBits	= 0;

	InitBits();
}

bool TShapeDraw::ParseDefineShape( int level )
{
	unsigned short tagID = GetWord();

	m_level = level;
    
	GGRECT bound;
	GetRect(bound);

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

bool TShapeDraw::ParseFillStyle(bool bHasAlpha)
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
		if (fillStyle == Linear_Gradient_Fill ) {
			//pFillStyle->SetFillStyleType( linear_gradient_fill );
			TLinearGradientFillStyle	*pfs;
			TMatrix matrix;
			pfs = new TLinearGradientFillStyle;
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
		else if ( fillStyle == Radial_Gradient_Fill) {
			TRadialGradientFillStyle *pfs = new TRadialGradientFillStyle;
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
		else if ((fillStyle == Tiled_Bitmap_Fill) || (fillStyle == Clipped_Bitmap_Fill))
        {
			// Get the image ID
            unsigned short imgID = GetWord();
            // Try to find the image object from the library
            gldLibrary *_lib = gldDataKeeper::Instance()->m_objLib;
            gldImage *_gimage = (gldImage*)(_lib->FindObjByID(imgID));

            // Get the bitmap matrix.
			TMatrix matrix;
			GetMatrix(matrix);

            // Create the image
            TImage *_timg = new TImage();
            // Set the ptr of gldImage to the TImage
            if (_gimage != NULL)
                _gimage->m_ptr = _timg;
            // Set the ptr of TImage to the gldImage
            _timg->Ptr(_gimage);
            // Set TImage data
            if (_gimage != NULL)
            {
                if (_gimage->m_gimgType == gimageJPEG)
                {
                    TJPEGAdaptor _adaptor;
					U8 *imageData,*alphaData;
					U32 imageLen,alphaLen;
					_gimage->GetImageJPEGData(&imageData,&imageLen,&alphaData,&alphaLen);
                    _adaptor.SetJPEGData(imageData,imageLen,alphaData,alphaLen);
                    _timg->Load(&_adaptor);
                }
                else if (_gimage->m_gimgType == gimageBMP)
                {
                    TWinBitmapAdaptor _adaptor;
                    _adaptor.SetBitmapData(_gimage->GetRawData() + sizeof(BITMAPFILEHEADER), _gimage->GetRawDataLen() - sizeof(BITMAPFILEHEADER));
                    _timg->Load(&_adaptor);
                }
            }

            // Create the filll style
            TBitmapFillStyle *pfs;
            if (fillStyle == Tiled_Bitmap_Fill)
                pfs = new TTiledBitmapFillStyle();
            else
                pfs = new TClippedBitmapFillStyle();

            // Set the bitmap
            pfs->SetImage(_timg);

            // Set the matrix to image fill style
            pfs->SetMatrix(matrix);

			if (!OnNewFillStyle(pfs))
			{
				delete pfs;
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

bool TShapeDraw::ParseLineStyle(bool bHasAlpha)
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

bool TShapeDraw::ParseShapeRecord(bool bHasAlpha)
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

void TShapeDraw::OnNewStyleTable()
{
	m_pDevice->SetFillStyleTable(&m_FillStyleTab);
	m_pDevice->SetLineStyleTable(&m_LineStyleTab);
	m_pDevice->Render();
	m_pDevice->SetFillStyleTable(0);
	m_pDevice->SetLineStyleTable(0);
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();	
	m_pDevice->SetLineStyle(0);
	m_pDevice->SetFillStyle0(0);
	m_pDevice->SetFillStyle1(0);
	m_pDevice->MoveTo(0, 0);
}

bool TShapeDraw::OnNewFillStyle(TFillStyle *fillstyle)
{	
	m_FillStyleTab.Add(fillstyle);

	return true;
}

bool TShapeDraw::OnNewLineStyle(TLineStyle *linestyle)
{	
	m_LineStyleTab.Add(linestyle);

	return true;
}

void TShapeDraw::OnSetFillStyle0(int num)
{
	m_pDevice->SetFillStyle0(num);
}

void TShapeDraw::OnSetFillStyle1(int num)
{	
	m_pDevice->SetFillStyle1(num);
}

void TShapeDraw::OnSetLineStyle(int num)
{
	m_pDevice->SetLineStyle(num);
}

void TShapeDraw::OnMoveTo(long x, long y)
{	                
	m_pDevice->MoveTo(x, y);
}

void TShapeDraw::OnLineTo(long dx, long dy)
{
	int x, y;
	
	m_pDevice->GetCurrentPosition(x, y);
	m_pDevice->LineTo(x + dx, y + dy);	
}

void TShapeDraw::OnBezierTo(long dcx, long dcy, long dax, long day)
{
	int cx, cy, a2x, a2y;
	
	m_pDevice->GetCurrentPosition(cx, cy);
	cx += dcx;
	cy += dcy;
	a2x = cx + dax;
	a2y = cy + day;
	m_pDevice->BezierTo(cx, cy, a2x, a2y);	
}

U32 TShapeDraw::GenImageData(U8 **data, U32 rawLength, U8* rawData, U32 jtableLength, U8* jtableData)
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
            // Get data and dataSize
			//GetWord();  // skip image id
   //         U8* rawData = (U8*)m_pData + m_dataPos;
   //         U32 rawSize = m_tagEnd - m_dataPos;

   //         // Get table and tableSize
   //         int offset = 2;
   //         U8 b1 = jtableData[0];
   //         U8 b2 = jtableData[1];
   //         unsigned short jtableTagCode = ((b1 << 8) || b2);
   //         unsigned short jtableTagLen = jtableTagCode & 0x3f;
   //         jtableTagCode = jtableTagCode >> 6;
   //         ASSERT(jtableTagCode == stagJPEGTables);
   //         if (jtableTagLen == 0x3f)
   // 	        offset += 4;
			//U8* table = (U8*)jtableData + offset;
   //         U32 tableSize = jtableLength - offset;

   //         // Unite the encoding table and the image data
   //         U8* dstData = NULL;
   //         U32 dstSize = 0;
   //         UniteTableStream(table, tableSize, rawData, rawSize, &dstData, &dstSize);

   //         // Set the result data and return the data length
   //         *data = dstData;
   //         rtnLength = dstSize;
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

U32 TShapeDraw::GenBMPData(U8 **data, U32 zipDataLen, U8* zipData, U32 width, U32 height, U32 bpp, U32 colorTabSize, BOOL withAlpha)
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

bool TShapeDraw::ParseShapeRecord(vector<gldShapeRecord*> *gshapeRecords, bool bHasAlpha)
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

bool TShapeDraw::ParseLineStyle(vector<gldLineStyle*> *glst, bool bHasAlpha)
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

bool TShapeDraw::ParseFillStyle(vector<gldFillStyle*> *gfst, bool bHasAlpha)
{
    // Pare each of the fill styles.
    for (vector<gldFillStyle*>::iterator it = gfst->begin(); it != gfst->end(); it++)
    {
        gldFillStyle *_gfs = (*it);
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

bool TShapeDraw::ParseDefineShape(gldShape *gshape, int level)
{
	m_level = level;    
    
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

bool TShapeDraw::ParseText(gldText *_gtext)
{
	// Process the text
    long _x, _y;
	long _fontHeight;
	gldMatrix _fmat;
	gldColor _color;
    // Set the initial value
	gldMatrix _bktmat = _gtext->m_matrix;
	_x = _y = 0;
	_fontHeight = 0;
    // Get each letter of the text
	for(list<gldTextGlyphRecord*>::iterator it = _gtext->m_glyphRecords.begin(); it != _gtext->m_glyphRecords.end(); it++)
    {
		gldTextGlyphRecord *_rec = (*it);
		if (_rec != NULL)
        {
			if (_rec->m_flags & isTextControl)
            {
				if (_rec->m_flags & textHasXOffset)
                {
					_x = _rec->m_xOffset;
				}
				if (_rec->m_flags & textHasYOffset)
                {
					_y = _rec->m_yOffset;
				}
				if (_rec->m_flags & textHasColor)
                {
					_color = _gtext->m_cxform.getColor(_rec->m_color);
				}
			}

			_fontHeight = _rec->m_fontHeight;

			// Update font matrix
            _fmat.m_e11 = _fontHeight / 1000.0f;
            _fmat.m_e22 = _fontHeight / 1000.0f;

            // Get the font shape
            gldShape *_gfShape = _rec->m_font->GetCharShape(_rec->m_code);

            // Update font matrix
            _fmat.m_x = _rec->m_xOffset;
            _fmat.m_y = _rec->m_yOffset;

            gldMatrix _bkmat = _gfShape->m_matrix;
            // Compute Character matrix
            _gfShape->m_matrix = _fmat * _gtext->m_matrix;
			
	        gldFillStyle *_fs = new gldFillStyle();
            _fs->SetFillStyleType(solid_fill);
            _fs->SetSolidFillColor(_rec->m_color);

            _gfShape->ClearFillStyles();
            _gfShape->AddFillStyle(_fs);
			
			TMatrix	_bkdevmat = m_pDevice->GetMatrix();
			TMatrix	_tmx;
			CTransAdaptor::GM2TM(_gfShape->m_matrix, _tmx);
			m_pDevice->SetMatrix(_tmx * _bkdevmat);
            if (!ParseDefineShape(_gfShape, _gfShape->GetShapeType()))
			{
				m_pDevice->SetMatrix(_bkdevmat);
				return false;
			}
			m_pDevice->SetFillStyleTable(&m_FillStyleTab);
			m_pDevice->SetLineStyleTable(&m_LineStyleTab);
			m_pDevice->Render();
			m_LineStyleTab.Clear();
			m_FillStyleTab.Clear();
			m_pDevice->SetMatrix(_bkdevmat);
		}	    
	    _gtext->m_matrix = _bktmat;
    }

	return true;
}

bool TShapeDraw::Draw(gldShape *gshape, TGraphicsDevice *device)
{
    m_dataLen = 0;
    m_pData = NULL;

	Init();

	assert(device);
	m_pDevice = device;
	m_pDevice->Render();
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();

    bool rtnFlag = ParseDefineShape(gshape, gshape->GetShapeType());
	m_pDevice->SetFillStyleTable(&m_FillStyleTab);
	m_pDevice->SetLineStyleTable(&m_LineStyleTab);
	m_pDevice->Render();
	m_pDevice->SetFillStyleTable(0);
	m_pDevice->SetLineStyleTable(0);
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();
	m_pDevice = NULL;
    return rtnFlag;
}

bool TShapeDraw::Draw(gldText *gtext, TGraphicsDevice *device)
{
	m_dataLen = 0;
    m_pData = NULL;

	Init();

	assert(device);
	m_pDevice = device;
	m_pDevice->Render();
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();

	bool ret = ParseText(gtext);
	m_pDevice->SetFillStyleTable(&m_FillStyleTab);
	m_pDevice->SetLineStyleTable(&m_LineStyleTab);
	m_pDevice->Render();
	m_pDevice->SetFillStyleTable(0);
	m_pDevice->SetLineStyleTable(0);
	m_LineStyleTab.Clear();
	m_FillStyleTab.Clear();
	m_pDevice = NULL;
	return ret;
}


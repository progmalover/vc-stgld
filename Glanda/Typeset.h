#pragma once

#include "gld_selection.h"

class CTypeset
{
public:
	enum TYPESETSTYLE
	{
		ALIGN_LEFT			=	1,
		ALIGN_TOP			=	2,
		ALIGN_RIGHT			=	3,
		ALIGN_BOTTOM		=	4,
		ALIGN_CENTER		=	5,
		ALIGN_MIDDLE		=	6,
		MAKE_SAME_WIDTH		=	7,
		MAKE_SAME_HEIGHT	=	8,
		MAKE_SAME_SIZE		=	9,
		MAKE_HSPACING_EQUAL	=	10,
		MAKE_VSPACING_EQUAL	=	11		
	};

public:
	CTypeset(void);
	~CTypeset(void);

	static void Format(const gld_shape_sel &sel, TYPESETSTYLE nFormat);

protected:
	static gld_shape_sel::iterator GetBase(const gld_shape_sel &sel);

	static void AlighLeft(const gld_shape_sel &sel);
	static void AlighTop(const gld_shape_sel &sel);
	static void AlighRight(const gld_shape_sel &sel);
	static void AlighBottom(const gld_shape_sel &sel);
	static void AlighCenter(const gld_shape_sel &sel);
	static void AlighMiddle(const gld_shape_sel &sel);
	static void MakeSameWidth(const gld_shape_sel &sel);
	static void MakeSameHeight(const gld_shape_sel &sel);
	static void MakeSameSize(const gld_shape_sel &sel);
	static void MakeHSpacingEqual(const gld_shape_sel &sel);
	static void MakeVSpacingEqual(const gld_shape_sel &sel);

};

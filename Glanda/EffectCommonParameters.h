#pragma once

#include "EffectCommonUtils.h"

#define EP_BREAK_APART				"Common.BreakApart"
#define EP_TEXT_BORDER				"Common.TextBorder"
#define EP_BORDER_WIDTH				"Common.BorderWidth"
#define EP_BORDER_COLOR				"Common.BorderColor"
#define EP_CUSTOM_FILL				"Common.CustomFill"
#define EP_FILL_STYLE				"Common.FillStyle"
#define EP_FILL_INDIVIDUALLY		"Common.FillIndividually"
#define EP_FILL_ANGLE				"Common.FillAngle"
#define EP_ADD_SHADOW				"Common.AddShadow"
#define EP_SHADOW_SIZE				"Common.ShadowSize"
#define EP_SHADOW_COLOR				"Common.ShadowColor"

#define EP_AUTO_PLAY				"Common.AutoPlay"
#define EP_LOOP						"Common.Loop"

const GUID CLSID_CommonPage = {0x4427a761, 0x1e28, 0x47e5, 0xa8, 0x41, 0xfe, 0xeb, 0x55, 0x6a, 0xaf, 0x7f};
const GUID CLSID_EffectPreviewOptionsPage = {0xe288eee8, 0x16d9, 0x4e56, 0xb9, 0xc2, 0x3a, 0x8c, 0x4c, 0xcf, 0xfb, 0xf};

enum ParameterType
{
	PT_CUSTOM,
	PT_DEFINED,
	PT_COMMON,	
};

int GetParameterType(LPCTSTR name, LPCTSTR *paras = NULL, int nParas = 0);
void RemoveParameters(IGLD_Parameters *paras, ParameterType type, LPCTSTR *defined_paras = NULL, int n_defined_paras = 0);
void CopyParameters(IGLD_Parameters *src_paras, ParameterType type, IGLD_Parameters *dst_paras, LPCTSTR *defined_paras = NULL, int n_defined_paras = 0);
void CopyParameters(IGLD_Parameters *src_paras, IGLD_Parameters *dst_paras);

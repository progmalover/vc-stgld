

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Nov 04 20:14:08 2011
 */
/* Compiler settings for Glanda.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __Glanda_i_h__
#define __Glanda_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGLD_Application_FWD_DEFINED__
#define __IGLD_Application_FWD_DEFINED__
typedef interface IGLD_Application IGLD_Application;
#endif 	/* __IGLD_Application_FWD_DEFINED__ */


#ifndef __GLD_Application_FWD_DEFINED__
#define __GLD_Application_FWD_DEFINED__

#ifdef __cplusplus
typedef class GLD_Application GLD_Application;
#else
typedef struct GLD_Application GLD_Application;
#endif /* __cplusplus */

#endif 	/* __GLD_Application_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_Glanda_0000_0000 */
/* [local] */ 

#include "..\GlandaCOM\GlandaCOM.h" 


extern RPC_IF_HANDLE __MIDL_itf_Glanda_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Glanda_0000_0000_v0_0_s_ifspec;


#ifndef __GlandaLib_LIBRARY_DEFINED__
#define __GlandaLib_LIBRARY_DEFINED__

/* library GlandaLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_GlandaLib;

#ifndef __IGLD_Application_INTERFACE_DEFINED__
#define __IGLD_Application_INTERFACE_DEFINED__

/* interface IGLD_Application */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IGLD_Application;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("87F96757-781C-4A12-A93F-09651218B592")
    IGLD_Application : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddScene( 
            /* [in] */ long nPos,
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteScene( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetCurScene( 
            /* [in] */ long nPos) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ /* external definition not present */ IGLD_Object *pObj,
            /* [in] */ /* external definition not present */ IGLD_Matrix *pMatrix,
            /* [in] */ /* external definition not present */ IGLD_Cxform *pCxform,
            /* [retval][out] */ long *pPos) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [in] */ long nPos,
            /* [in] */ VARIANT_BOOL bAppend) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMovieSize( 
            /* [out] */ long *nWidth,
            /* [out] */ long *nHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMovieSize( 
            /* [in] */ long nWidth,
            /* [in] */ long nHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetViewSize( 
            /* [out] */ long *nWidth,
            /* [out] */ long *nHeight) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ViewToCanvas( 
            /* [out] */ long *x,
            /* [out] */ long *y) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CenterMovie( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddEffect( 
            /* [in] */ long nPos,
            /* [in] */ BSTR bstrProgID,
            /* [in] */ long nStartTime,
            /* [in] */ long nLength,
            /* [in] */ /* external definition not present */ IGLD_Parameters *pParameters) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadObjectFromFile( 
            /* [in] */ BSTR bstrFileName,
            /* [retval][out] */ /* external definition not present */ IGLD_Object **ppObject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Draw( 
            /* [in] */ /* external definition not present */ IGLD_Object *pIObject,
            /* [in] */ HDC hDC,
            /* [in] */ long x,
            /* [in] */ long y,
            /* [in] */ long cx,
            /* [in] */ long cy,
            /* [in] */ long nTime) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ExportMovie( 
            /* [in] */ BSTR PathName,
            /* [in] */ VARIANT_BOOL bCompressed) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Group( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Ungroup( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE BringForward( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE BringToFront( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SendBackward( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SendToBack( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddSceneSound( 
            /* [in] */ /* external definition not present */ IGLD_Sound *pSound,
            /* [in] */ long nStartTime,
            /* [retval][out] */ long *pPos) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DeleteSceneSound( 
            /* [in] */ long nPos) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMovieSound( 
            /* [in] */ /* external definition not present */ IGLD_Sound *pSound,
            /* [in] */ BSTR bstrStartScene,
            /* [in] */ VARIANT_BOOL bLoopFlag,
            /* [in] */ long nLoopTimes) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_hWnd( 
            /* [retval][out] */ long *pWnd) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BgColor( 
            /* [retval][out] */ long *Color) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_BgColor( 
            /* [in] */ long Color) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FrameRate( 
            /* [retval][out] */ double *fFrameRate) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FrameRate( 
            /* [in] */ double fFrameRate) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TotalScenes( 
            /* [retval][out] */ long *nCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EnterSceneActionScript( 
            /* [retval][out] */ BSTR *bstrAction) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EnterSceneActionScript( 
            /* [in] */ BSTR bstrAction) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ExitSceneActionScript( 
            /* [retval][out] */ BSTR *bstrAction) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ExitSceneActionScript( 
            /* [in] */ BSTR bstrAction) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_InstanceActionScript( 
            /* [retval][out] */ BSTR *bstrAction) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_InstanceActionScript( 
            /* [in] */ BSTR bstrAction) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_InstanceName( 
            /* [retval][out] */ BSTR *bstrName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_InstanceName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SceneName( 
            /* [retval][out] */ BSTR *bstrName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SceneName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EffectCommonParameters( 
            /* [retval][out] */ /* external definition not present */ IGLD_Parameters **ppParameters) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EffectCommonParameters( 
            /* [in] */ /* external definition not present */ IGLD_Parameters *pParameters) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_InstanceMask( 
            /* [retval][out] */ VARIANT_BOOL *bMask) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_InstanceMask( 
            /* [in] */ VARIANT_BOOL bMask) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGLD_ApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGLD_Application * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGLD_Application * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGLD_Application * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGLD_Application * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGLD_Application * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGLD_Application * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGLD_Application * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddScene )( 
            IGLD_Application * This,
            /* [in] */ long nPos,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DeleteScene )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetCurScene )( 
            IGLD_Application * This,
            /* [in] */ long nPos);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
            IGLD_Application * This,
            /* [in] */ /* external definition not present */ IGLD_Object *pObj,
            /* [in] */ /* external definition not present */ IGLD_Matrix *pMatrix,
            /* [in] */ /* external definition not present */ IGLD_Cxform *pCxform,
            /* [retval][out] */ long *pPos);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IGLD_Application * This,
            /* [in] */ long nPos,
            /* [in] */ VARIANT_BOOL bAppend);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetMovieSize )( 
            IGLD_Application * This,
            /* [out] */ long *nWidth,
            /* [out] */ long *nHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetMovieSize )( 
            IGLD_Application * This,
            /* [in] */ long nWidth,
            /* [in] */ long nHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetViewSize )( 
            IGLD_Application * This,
            /* [out] */ long *nWidth,
            /* [out] */ long *nHeight);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ViewToCanvas )( 
            IGLD_Application * This,
            /* [out] */ long *x,
            /* [out] */ long *y);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CenterMovie )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddEffect )( 
            IGLD_Application * This,
            /* [in] */ long nPos,
            /* [in] */ BSTR bstrProgID,
            /* [in] */ long nStartTime,
            /* [in] */ long nLength,
            /* [in] */ /* external definition not present */ IGLD_Parameters *pParameters);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadObjectFromFile )( 
            IGLD_Application * This,
            /* [in] */ BSTR bstrFileName,
            /* [retval][out] */ /* external definition not present */ IGLD_Object **ppObject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Draw )( 
            IGLD_Application * This,
            /* [in] */ /* external definition not present */ IGLD_Object *pIObject,
            /* [in] */ HDC hDC,
            /* [in] */ long x,
            /* [in] */ long y,
            /* [in] */ long cx,
            /* [in] */ long cy,
            /* [in] */ long nTime);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ExportMovie )( 
            IGLD_Application * This,
            /* [in] */ BSTR PathName,
            /* [in] */ VARIANT_BOOL bCompressed);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Group )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Ungroup )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *BringForward )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *BringToFront )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SendBackward )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SendToBack )( 
            IGLD_Application * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddSceneSound )( 
            IGLD_Application * This,
            /* [in] */ /* external definition not present */ IGLD_Sound *pSound,
            /* [in] */ long nStartTime,
            /* [retval][out] */ long *pPos);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DeleteSceneSound )( 
            IGLD_Application * This,
            /* [in] */ long nPos);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetMovieSound )( 
            IGLD_Application * This,
            /* [in] */ /* external definition not present */ IGLD_Sound *pSound,
            /* [in] */ BSTR bstrStartScene,
            /* [in] */ VARIANT_BOOL bLoopFlag,
            /* [in] */ long nLoopTimes);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_hWnd )( 
            IGLD_Application * This,
            /* [retval][out] */ long *pWnd);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BgColor )( 
            IGLD_Application * This,
            /* [retval][out] */ long *Color);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BgColor )( 
            IGLD_Application * This,
            /* [in] */ long Color);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FrameRate )( 
            IGLD_Application * This,
            /* [retval][out] */ double *fFrameRate);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FrameRate )( 
            IGLD_Application * This,
            /* [in] */ double fFrameRate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TotalScenes )( 
            IGLD_Application * This,
            /* [retval][out] */ long *nCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EnterSceneActionScript )( 
            IGLD_Application * This,
            /* [retval][out] */ BSTR *bstrAction);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EnterSceneActionScript )( 
            IGLD_Application * This,
            /* [in] */ BSTR bstrAction);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExitSceneActionScript )( 
            IGLD_Application * This,
            /* [retval][out] */ BSTR *bstrAction);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ExitSceneActionScript )( 
            IGLD_Application * This,
            /* [in] */ BSTR bstrAction);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InstanceActionScript )( 
            IGLD_Application * This,
            /* [retval][out] */ BSTR *bstrAction);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InstanceActionScript )( 
            IGLD_Application * This,
            /* [in] */ BSTR bstrAction);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InstanceName )( 
            IGLD_Application * This,
            /* [retval][out] */ BSTR *bstrName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InstanceName )( 
            IGLD_Application * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SceneName )( 
            IGLD_Application * This,
            /* [retval][out] */ BSTR *bstrName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SceneName )( 
            IGLD_Application * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EffectCommonParameters )( 
            IGLD_Application * This,
            /* [retval][out] */ /* external definition not present */ IGLD_Parameters **ppParameters);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EffectCommonParameters )( 
            IGLD_Application * This,
            /* [in] */ /* external definition not present */ IGLD_Parameters *pParameters);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InstanceMask )( 
            IGLD_Application * This,
            /* [retval][out] */ VARIANT_BOOL *bMask);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InstanceMask )( 
            IGLD_Application * This,
            /* [in] */ VARIANT_BOOL bMask);
        
        END_INTERFACE
    } IGLD_ApplicationVtbl;

    interface IGLD_Application
    {
        CONST_VTBL struct IGLD_ApplicationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGLD_Application_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGLD_Application_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGLD_Application_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGLD_Application_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGLD_Application_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGLD_Application_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGLD_Application_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IGLD_Application_AddScene(This,nPos,bstrName)	\
    ( (This)->lpVtbl -> AddScene(This,nPos,bstrName) ) 

#define IGLD_Application_DeleteScene(This)	\
    ( (This)->lpVtbl -> DeleteScene(This) ) 

#define IGLD_Application_SetCurScene(This,nPos)	\
    ( (This)->lpVtbl -> SetCurScene(This,nPos) ) 

#define IGLD_Application_CreateInstance(This,pObj,pMatrix,pCxform,pPos)	\
    ( (This)->lpVtbl -> CreateInstance(This,pObj,pMatrix,pCxform,pPos) ) 

#define IGLD_Application_Select(This,nPos,bAppend)	\
    ( (This)->lpVtbl -> Select(This,nPos,bAppend) ) 

#define IGLD_Application_GetMovieSize(This,nWidth,nHeight)	\
    ( (This)->lpVtbl -> GetMovieSize(This,nWidth,nHeight) ) 

#define IGLD_Application_SetMovieSize(This,nWidth,nHeight)	\
    ( (This)->lpVtbl -> SetMovieSize(This,nWidth,nHeight) ) 

#define IGLD_Application_GetViewSize(This,nWidth,nHeight)	\
    ( (This)->lpVtbl -> GetViewSize(This,nWidth,nHeight) ) 

#define IGLD_Application_ViewToCanvas(This,x,y)	\
    ( (This)->lpVtbl -> ViewToCanvas(This,x,y) ) 

#define IGLD_Application_CenterMovie(This)	\
    ( (This)->lpVtbl -> CenterMovie(This) ) 

#define IGLD_Application_AddEffect(This,nPos,bstrProgID,nStartTime,nLength,pParameters)	\
    ( (This)->lpVtbl -> AddEffect(This,nPos,bstrProgID,nStartTime,nLength,pParameters) ) 

#define IGLD_Application_LoadObjectFromFile(This,bstrFileName,ppObject)	\
    ( (This)->lpVtbl -> LoadObjectFromFile(This,bstrFileName,ppObject) ) 

#define IGLD_Application_Draw(This,pIObject,hDC,x,y,cx,cy,nTime)	\
    ( (This)->lpVtbl -> Draw(This,pIObject,hDC,x,y,cx,cy,nTime) ) 

#define IGLD_Application_ExportMovie(This,PathName,bCompressed)	\
    ( (This)->lpVtbl -> ExportMovie(This,PathName,bCompressed) ) 

#define IGLD_Application_Group(This)	\
    ( (This)->lpVtbl -> Group(This) ) 

#define IGLD_Application_Ungroup(This)	\
    ( (This)->lpVtbl -> Ungroup(This) ) 

#define IGLD_Application_BringForward(This)	\
    ( (This)->lpVtbl -> BringForward(This) ) 

#define IGLD_Application_BringToFront(This)	\
    ( (This)->lpVtbl -> BringToFront(This) ) 

#define IGLD_Application_SendBackward(This)	\
    ( (This)->lpVtbl -> SendBackward(This) ) 

#define IGLD_Application_SendToBack(This)	\
    ( (This)->lpVtbl -> SendToBack(This) ) 

#define IGLD_Application_AddSceneSound(This,pSound,nStartTime,pPos)	\
    ( (This)->lpVtbl -> AddSceneSound(This,pSound,nStartTime,pPos) ) 

#define IGLD_Application_DeleteSceneSound(This,nPos)	\
    ( (This)->lpVtbl -> DeleteSceneSound(This,nPos) ) 

#define IGLD_Application_SetMovieSound(This,pSound,bstrStartScene,bLoopFlag,nLoopTimes)	\
    ( (This)->lpVtbl -> SetMovieSound(This,pSound,bstrStartScene,bLoopFlag,nLoopTimes) ) 

#define IGLD_Application_get_hWnd(This,pWnd)	\
    ( (This)->lpVtbl -> get_hWnd(This,pWnd) ) 

#define IGLD_Application_get_BgColor(This,Color)	\
    ( (This)->lpVtbl -> get_BgColor(This,Color) ) 

#define IGLD_Application_put_BgColor(This,Color)	\
    ( (This)->lpVtbl -> put_BgColor(This,Color) ) 

#define IGLD_Application_get_FrameRate(This,fFrameRate)	\
    ( (This)->lpVtbl -> get_FrameRate(This,fFrameRate) ) 

#define IGLD_Application_put_FrameRate(This,fFrameRate)	\
    ( (This)->lpVtbl -> put_FrameRate(This,fFrameRate) ) 

#define IGLD_Application_get_TotalScenes(This,nCount)	\
    ( (This)->lpVtbl -> get_TotalScenes(This,nCount) ) 

#define IGLD_Application_get_EnterSceneActionScript(This,bstrAction)	\
    ( (This)->lpVtbl -> get_EnterSceneActionScript(This,bstrAction) ) 

#define IGLD_Application_put_EnterSceneActionScript(This,bstrAction)	\
    ( (This)->lpVtbl -> put_EnterSceneActionScript(This,bstrAction) ) 

#define IGLD_Application_get_ExitSceneActionScript(This,bstrAction)	\
    ( (This)->lpVtbl -> get_ExitSceneActionScript(This,bstrAction) ) 

#define IGLD_Application_put_ExitSceneActionScript(This,bstrAction)	\
    ( (This)->lpVtbl -> put_ExitSceneActionScript(This,bstrAction) ) 

#define IGLD_Application_get_InstanceActionScript(This,bstrAction)	\
    ( (This)->lpVtbl -> get_InstanceActionScript(This,bstrAction) ) 

#define IGLD_Application_put_InstanceActionScript(This,bstrAction)	\
    ( (This)->lpVtbl -> put_InstanceActionScript(This,bstrAction) ) 

#define IGLD_Application_get_InstanceName(This,bstrName)	\
    ( (This)->lpVtbl -> get_InstanceName(This,bstrName) ) 

#define IGLD_Application_put_InstanceName(This,bstrName)	\
    ( (This)->lpVtbl -> put_InstanceName(This,bstrName) ) 

#define IGLD_Application_get_SceneName(This,bstrName)	\
    ( (This)->lpVtbl -> get_SceneName(This,bstrName) ) 

#define IGLD_Application_put_SceneName(This,bstrName)	\
    ( (This)->lpVtbl -> put_SceneName(This,bstrName) ) 

#define IGLD_Application_get_EffectCommonParameters(This,ppParameters)	\
    ( (This)->lpVtbl -> get_EffectCommonParameters(This,ppParameters) ) 

#define IGLD_Application_put_EffectCommonParameters(This,pParameters)	\
    ( (This)->lpVtbl -> put_EffectCommonParameters(This,pParameters) ) 

#define IGLD_Application_get_InstanceMask(This,bMask)	\
    ( (This)->lpVtbl -> get_InstanceMask(This,bMask) ) 

#define IGLD_Application_put_InstanceMask(This,bMask)	\
    ( (This)->lpVtbl -> put_InstanceMask(This,bMask) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGLD_Application_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GLD_Application;

#ifdef __cplusplus

class DECLSPEC_UUID("77A5CE55-910C-400D-B632-5F2BC6FC1F1D")
GLD_Application;
#endif
#endif /* __GlandaLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



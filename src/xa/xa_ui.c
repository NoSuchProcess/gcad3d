//  xa_ui.c
/*
 *
 * Copyright (C) 2015 CADCAM-Services Franz Reiter (franz.reiter@cadcam.co.at)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
-----------------------------------------------------
TODO:
  ..

-----------------------------------------------------
Modifications:
2016-03-31 UI_WinPrint1: rename print.dat -> print.ps for evince. RF.
2001-06-06 Trennen in xa_app und xa_ui.

-----------------------------------------------------
*/
#ifdef globTag
void UI(){}
#endif
/*!
\file  ../xa/xa_ui.c
\brief Main GUI 
\code
=====================================================
List_functions_start:

UI_win_main           Das gesamte Window-Layout
UI_open_last          display file <tmpDir>MdlLst.txt as subMenu

UI_block__            activate / disactivate functions, input, cursor
UI_block_get          query if functions, input, cursor is blocked
UI_block_input        activate / disactivate keystrokes & grafic_selections
UI_func_stat_set__    activate / disactivate Functions.
UI_func_stat_set_tab  activate / disactivate Functions.
UI_mBars_off           entspr. SM Menuebar sensitiv machen oder abdunkeln
UI_wait_Esc           Wait for pressing the Esc-Key ...
UI_askEscape          alle pending events abarbeiten;
UI_askExit            ?

UI_VW_upd             enable/disable active view-button
UI_VW_set             enable/disable active view-button

UI_CAD_ON
UI_CAD_reload
UI_CAD_OFF

UI_winTX_prt          print into message-window
UI_winTX_prf          write file to message-window
UI_winTX_rmLast       delete last textoutputline

UI_wireCB             set GL-shaden
UI_sur_act_CB2
UI_sur_act_CB1
UI_Set_ConstPl_Z      write out the name of the Constr.Plane
UI_Set_actPrg         display label actoive program-name
UI_view_Z_CB
UI_suract_keyIn       ViewZ|ConstZ
UI_Enter              reset KeyStatEscape to normal if cursor enters Window ..
UI_creObjHdr          .
UI_EdKeyCtrlM3        Ctrl M3 pressed
UI_EdKeyCR            CR|M3
AP_UserKeyIn_reset
AP_UserKeyIn_get
AP_User_reset         alle reset-funcs, die bei MS-Win u Linux gleich sind
PLU_appNamTab_set   provide names for application-objects
UI_CAD_activate
UI_save__
UI_PRI__              export / print
UI_vwz__
UI_vwzCB
UI_loadImg_CB
UI_loadMock_CB
UI_loadBmp_CB
UI_del_CB
UI_delMdl
UI_ren_CB
UI_cpyMdl3
UI_cpyMdl2
UI_cpyMdl1
UI_renMdl
UI_opePtab
UI_open_add      UNUSED
UI_open_symCB
UI_openCB
UI_disp_modsiz         display modelsize in gtk-label

UI_disp_dbo            display temporary db-obj
UI_disp_var1           temporary display lenght-variable
UI_disp_Pos            temporary display position (with little red circle)
UI_disp_vec1           create vector as a temporary outputObject. True length.
UI_disp_pln_oid        hilite plane from objID
UI_disp_tra

UI_butSM               Search/Measure
UI_CB_view
UI_src_edi             start modus with datasource = editor
UI_src_mem             start modus with datasource = memory
UI_butCB               Callback fuer Buttons.
UI_reset_hide          disactivate 0=Hide 1=View 2=beide
UI_reset__             back to VWR
UI_ToolBars            Toolbars ein/ausschalten.
UI_grp__               Goupmode ON|OFF
UI_grpAdd
UI_dump_oid            dump DB-object  into file & display with browser
UI_dump_obj            dump DB-object  into file & display with browser
UI_dump__
UI_menCB               Mainentry Menufunktions.
UI_file_sensi          TRUE od FALSE
UI_WinInfo1            InfoWindow
UI_RelAbs_act
UI_RelAbsCB
UI_WinDefTx            Textsizepanel
UI_WinAppr             DXF-Import Panel ..
UI_WinDxfImp           DXF-Import Panel ..
UI_colSel              see UI_WinDefTx TSU_exp_wrlCol
UI_WinLtypMod          Linetypes
UI_WinToler            Toleranzen ...
UI_Ed_del              LEER
UI_Ed_hili             Hintergrund Editfenster hilite ON / OFF
UI_Ed_ins              insert into editor ?
UI_Ed_sel_ln
UI_Ed_sel              auch zum CurPos setzen
UI_Ed_scroll           auch zum CurPos setzen
UI_Ed_sel1             text selektiert darstellen
// UI_reset               reset "add to Group"
UI_AP                  Hauptinterface zur App (APP ruft UI);

List_functions_end:
=====================================================
obsolet:
// UI_impAux1        UNUSED
// UI_impLwoCB       UNUSED
// UI_imp3dsCB       UNUSED
// UI_expMockup      UNUSED
// UI_expDxfCB       UNUSED
// UI_expObjCB       UNUSED
// UI_expIgeCB       UNUSED
// UI_saveMockCB     UNUSED
// UI_expIsoCB       UNUSED
// UI_saveCB         UNUSED
// UI_expNat         UNUSED
UI_EdKeyPress         allback keyPress im editwin UND CAD-Entryfelder
UI_EdKeyRelease       callback KeyRelease im editwin
UI_EdButtonPress      klick into Editor-window
UI_EdButtonRelease    Click into EditWindow.  Kommt nur mehr in MAN.

\endcode *//*----------------------------------------





-------------------------------------------------------
Compile:
cc -c `gtk-config --cflags` xa_ui.c
cl -c /I ..\include xa_ui.c

*/



#ifdef _MSC_VER
#include "MS_Def1.h"
#else
#include <dlfcn.h>           // Unix: dlopen
#endif




#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>               // va_list
#include <ctype.h>                // toupper


#include "../ut/ut_geo.h"         // OFF, ON ..
#include "../ut/ut_ox_base.h"             // OGX_SET_INDEX
#include "../ut/ut_TX.h"
#include "../ut/ut_os.h"
#include "../ut/ut_txt.h"         // fnam_del
#include "../ut/ut_txfil.h"       // UTF_FilBuf0Siz
#include "../ut/ut_cast.h"             // INT_PTR
#include "../ut/ut_gtypes.h"           // AP_src_typ__
#include "../ut/ut_col.h"              // COL_INT32

#include "../gui/gui__.h"              // Gtk3

#include "../ci/NC_Main.h"
#include "../ci/NC_apt.h"

#include "../ut/func_types.h"          // FUNC_DispWire
#include "../gr/ut_gr.h"          // FUNC_ViewTop
#include "../gr/ut_GL.h"          // GL_Redraw
#include "../gr/ut_DL.h"          // GL_Redraw

#include "../db/ut_DB.h"          // DB_GetVector

#include "../xa/xa_ui_gr.h"
#include "../xa/xa_uid.h"         //  DLI_POS_TMP
#include "../xa/xa_ui.h"               // UID_ckb_nam u. ex func_types.h: UI_FuncGet
#include "../xa/xa_ed.h"            // EDI_CB__
#include "../xa/xa_edi__.h"            // EDI_CB__
#include "../xa/xa_undo.h"
#include "../xa/xa_aux.h"
#include "../xa/xa_mem.h"              // memspc55
#include "../xa/xa_sele.h"             // Typ_go*
#include "../xa/xa_msg.h"              // MSG_open
#include "../xa/xa.h"                  // AP_modact_nam PRC_IS_ACTIVE
#include "../xa/gcad_version.h"        // INIT_TXT
#include "../xa/xa_ui_cad.h"
#include "../xa/xa_app.h"              // PRC_IS_ACTIVE

#include "../xa/xa_brw.h"              // Brw_CB_sel


#define   TRUE 1
#define   FALSE 0




/* ============ Externe Var: ======================================= */

//  Imports von ut_txfil: der Memorybereich fuer die Datei.
extern char    *UTF_FilBuf0;
extern long    UTF_FilBuf0Len;  // die aktuelle Filesize
extern long    UTF_FilBuf0Siz;  // die allocierte Filesize



// from ../ci/NC_Main.c
extern int     APT_2d_to_3d_Ind;       // f. Refsys-Anzeige
extern double  APT_ModSiz;
extern int     APT_dispPT, APT_dispPL, APT_dispNam, APT_dispDir;


// aus ut_cvApp.c:
// extern double cvApp_tol;           // Approximationstoleranz (Import IGES)
// extern double cvApp_lmin;          // Approximation: MindestLineLen.


// aus xa.c:
extern AP_STAT   AP_stat;               // sysStat,errStat..
extern int       AP_ED_cPos;            // die aktuelle CharPos im Edi.
extern int       WC_modact_ind;         // -1=primary Model is active;
                                        // else subModel is being created
extern double    WC_sur_Z;
extern int       WC_sur_ind;            // Index auf die ActiveConstrPlane
extern Plane     WC_sur_act;
extern char      WC_ConstPl_Z[16];      // diplayed name of the Constr.Plane
extern int       AP_src;                // AP_SRC_MEM od AP_SRC_EDI
extern double     AP_txsiz;             // Notes-Defaultsize
extern double     AP_txdimsiz;          // Dimensions-Text-size
extern int        AP_txNkNr;            // Nachkommastellen
extern ColRGB     AP_actcol;
extern ColRGB     AP_defcol;


// ex ../xa/xa_ui_gr.c
extern int        GR_Func_Act;


// aus ../gr/ut_gtx.c:
extern double GR_tx_scale;


// static int UI_Ed_CurPos1;   // CursorPos vor einem Insert
extern long   UI_Ed_fsiz;      // AptTextsize







//============ Lokale Var: =======================================
  MemObj ckb_hide, ckb_view, ckb_man, ckb_cad, ckb_vwr, ckb_3D,
         ckb_ptDisp, ckb_plDisp, ckb_such,
         bt_top, bt_front, bt_side, bt_iso,    // view-buttons
         ckb_meas,
         UIw_Box_TB,  // box for the Toolbars; under Grafic, over textWindow.
         UI_box_ntb;  // Notebook-box (browser,editor);

  static MemObj ckb_nam, ckb_dir, ckb_shd,
         ckb_bound, ckb_light, ckb_compl, ckb_Iact, ckb_Bar1, ckb_Bar2,
         ckb_backW, // background white
         ckb_Tut,   // ScreenCast ON/OFF
         ckb_Brw,   // BrowserWindow OFF
         ckb_rctl,  // RemoteControl OFF
         ckb_impnat, ckb_compr, // ckb_grpAdd,ckb_mod,ckb_del,
         ckb_vwz;

  static MemObj box00, box1, w_test1, ToolBar1, ToolBar2,
         box1C, box1C1, boxRelAbs,
         box1R, box1S,
         box1C1v, box1C2v, box1C3v, box1C7v,
         box1C1v1h, box1C1v2h,
         box1C2,
         box1V, box1V1, box1V2,
         box1A, box1X, box1Y,
         box2B1, box2B2,
         but_go, but_step, but_end,
         menu_bar, men_fil, men_mod, men_opt, men_mdf, men_ins, men_sel,
         men_cat, men_exp1, men_exp2, men_hlp, men_app, men_prc, men_plu, men_rpc,
         UIw_file_new, UIw_file_ope, UIw_file_opD, UIw_file_ins,
         UIw_file_imp, UIw_file_exp, UIw_infoSel,
         UIw_file_sav1, UIw_file_sav2, UIw_file_sav3;


  MemObj   winMain;      // see also UI_MainWin
  MemObj   winTX;        // das Textausgabefenster
  MemObj   wMsg;         // das Messagewidget (Textausgabefenster)

  MemObj   winGR;        // das rechte Grafikfenster

  MemObj      win_paned;    // window editor & browser
  MemObj      win_edi;      // notebookpage editor
  MemObj      win_brw;      // notebookpage browser

  MemObj      winED;        // Editorfenster - fuer pack
  MemObj      winBrw;       // TreeBrowser
  int         win_panStat=1;   // 1=paned activ; 0=off
  int         winBrStat=1;     // 1=Brw activ; 0=off
  int         win_edStat=0;    // 1=Editor activ; 0=off
  // GIO_WinTree winBrw;       // TreeBrowser
  // GtkWidget EdView;    // Editorfenster - Focus
  // GtkWidget wText;     // Editorfenster - Edit


  MemObj      tbCad=GUI_OBJ_NEW;   // der CAD-Toolbar
  MemObj      tbNc=GUI_OBJ_NEW;    // der NC-Toolbar
  MemObj      tbWcGeo=GUI_OBJ_NEW; // der WC-Toolbar
  MemObj      tbWcTec=GUI_OBJ_NEW; // der WC-Toolbar

  MemObj      UI_curPos;
  MemObj      UI_ouf_scl;
  MemObj      UI_sur_act;
  MemObj      UI_sur_Z;
  MemObj      UI_view_Z;
  // GtkWidget UI_Refsys,
  MemObj      UI_grpNr, UI_ConstP, UI_GrAttr, UI_lNr, UIw_prg, UIw_typPrg;

// static GtkStyle  *UI_styl_0, *UI_styl_1;


int    UI_Focus      = 0;      // wer Focus hat; 0=GL, 1=Edit, 2=ViewZ-Entryfeld
                               // 3 =ConstZ

int    (*UI_UserKeyFunc) ();        // ob KeyIn an eine UserFunction geht

// // table of user-defined names of application-objects
// APP_OBJ_NAM *UI_User_appNamTab = NULL;     // appObjNamTab

int    KeyStatEscape = OFF;
int    KeyStatShift  = OFF;
int    KeyStatCtrl   = OFF;            // 1=OFF; 0=ON
int    KeyStatAlt    = OFF;

int    UI_InpMode    = UI_MODE_START;  // MAN od CAD od NC; see also AP_SRC
// int    UI_InpSM      = OFF;            // S/M
int    UI_InpVWZ     = OFF;            // S/M

char   UI_stat_view  = 1,              // view active: 0=yes, 1=not
       UI_stat_hide  = 1,              // hide active: 0=yes, 1=not
       UI_stat_3D    = 1,                  // 0=not sel, 1=selected.
       UI_stat_shade = 1,               // 0=not sel, 1=selected.
       UI_block_func = 0,
       UI_block_inp  = 0,               // 0=open, 1=blocked
       UI_block_cur  = 0;

int UI_EditMode = UI_EdMode_Add;

int    UI_RelAbs = 0;                 // 0=Rel, 1=Abs

  // char xa_auxbuf1[256];

//Ein Flag fuer die Synchronisation Edittext - memory;
//  0 = synchron (nach dem ReadText -> memory;
//  1 = needUpdate, nach textChanged.
int xa_fl_TxMem;


char    UI_fnamFilt[80] = "*";               // filenamefilter


// static char txbuf[10000];





//============ Prototypen: =======================================
  // int UI_sur_act_CB2 (GtkWidget *parent, void *data);
  // gint UI_ButtonPress (GtkWidget *widget, GdkEventMotion *event);
  int UI_RelAbsCB (MemObj *mo, void **data);
  int UI_lang_chg (MemObj *mo, void **data);
  int UI_def_browser ();
  int UI_def_editor ();
  int UI_open_last ();

  // int UI_EdKeyPress (GtkWidget *parent, GdkEventKey *event);

  // gint UI_GR_ButtonPress (GtkWidget *widget, GdkEventButton *event);
  // gint UI_GR_ButtonRelease (GtkWidget *widget, GdkEventButton *event);
  // int UI_GR_ButtonPress (void *widget, void *event);
  // int UI_GR_ButtonRelease (void *widget, void *event);

// ex xa_grp.c:
  int Grp_exp (char *fnam,char *dirNam);
  int UI_paned_CB (void *parent, int pgNr);



//================================================================
  void* WIN_ED () {
//================================================================
// returns winED as void*
// use with:
// extern void*     WIN_ED();         // returns winED as void*
// l1 = GUI_Ed_getEof (WIN_ED());

  return &winED;

}



//================================================================
  int UI_winTX_rmLast () {
//================================================================
/// UI_winTX_rmLast       delete last textoutputline

  return GUI_msgwin_rmLast (&winTX);

}


//================================================================
  int UI_winTX_prf (char *fn) {
//================================================================
/// UI_winTX_prf          write file to message-window

  return GUI_msgwin_prf (&winTX, fn);

}



//================================================================
  int UI_winTX_prt (char *txt) {
//================================================================
/// UI_winTX_prt          print into message-window

  return GUI_msgwin_prt (&winTX, txt);

}


/*
//================================================================
  int UI_block_glEvents (int mode) {
//================================================================
// 1=block, 0=unblock
// block OpenGL-events Reshape, Enter, Draw.

  if(mode != 0) goto L_1;
    
  //----------------------------------------------------------------
  g_signal_handlers_unblock_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_Reshape), NULL);

  g_signal_handlers_unblock_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_Enter), NULL);

  g_signal_handlers_unblock_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_Draw), NULL);

  return 0;



  //----------------------------------------------------------------
  // 1 = block 
  L_1:

  g_signal_handlers_block_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_Reshape), NULL);

  g_signal_handlers_block_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_Enter), NULL);

  g_signal_handlers_block_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_Draw), NULL);

  return 0;

}
*/


//================================================================
  int UI_block_group (int mode) {
//================================================================
/// \code
/// block / unblock groupSelections
/// mode    0 = ON  = reset - groupSelect can be used
///         1 = OFF = block (disactivate) groupSelections
///        -1 =       query; returns active state; 
/// \endcode

  static int selGrpStat=0;


  // printf("UI_block_group %d\n",mode);
  // printf("   selGrpStat=%d\n",selGrpStat);


  if(mode < 0) return selGrpStat;

  selGrpStat = mode;
 
  return 0;

}

/* 2012-07-30 replaced by GUI_gl_block
//================================================================
  int UI_block_input (int mode) {
//================================================================
/// \code
/// block / unblock keystrokes & grafic_selections
/// mode    0 = reset keystrokes & grafic_selections to normal
///         1 = block keystrokes & grafic_selections
/// see also UI_block__ UI_func_stat_set__
/// \endcode


  // printf("UI_block_input %d\n",mode);

  if(AP_stat.sysStat < 2) return 0;  // only for MS-Win !

  if(mode != 0) goto L_1;


  //----------------------------------------------------------------
  // 0 = reset to normal
  // unblock keystrokes
  // GUI_update__ ();   // löscht die gespeicherten signals !!!
  g_signal_handlers_unblock_by_func (GTK_OBJECT(UI_MainWin),
                                     GTK_SIGNAL_FUNC(UI_EdKeyPress), NULL);

  // unblock grafic_selections
  g_signal_handlers_unblock_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_ButtonPress), NULL);

  // unblock mouse-movements
  g_signal_handlers_unblock_by_func (GTK_OBJECT(GL_widget),
                                     GTK_SIGNAL_FUNC(UI_GR_MotionNotify), NULL);
  return 0;



  //----------------------------------------------------------------
  // 1 = block keystrokes & grafic_selections
  L_1:
  // block mouse-movements
  g_signal_handlers_block_by_func (GTK_OBJECT(GL_widget),
                                   GTK_SIGNAL_FUNC(UI_GR_MotionNotify), NULL);

  // block grafic selections
  g_signal_handlers_block_by_func (GTK_OBJECT(GL_widget),
                                   GTK_SIGNAL_FUNC(UI_GR_ButtonPress), NULL);

  // block keystrokes
  g_signal_handlers_block_by_func (GTK_OBJECT(UI_MainWin),
                                   GTK_SIGNAL_FUNC(UI_EdKeyPress), NULL);
  // GUI_update__ ();

  return 0;

}
*/


//================================================================
  int UI_block_get (int *iFunc, int *iInp, int *iCur) {
//================================================================
// UI_block_get    query if functions, input, cursor is blocked

  if(iFunc) *iFunc = UI_block_func;
  if(iInp)  *iInp  = UI_block_inp;
  if(iCur)  *iCur  = UI_block_cur;

    // printf("ex UI_block_get %d %d %d\n",*iFunc,*iInp,*iCur);

  return 0;

}


//================================================================
  int UI_block__ (int iFunctions, int iInput, int iCursor) {
//================================================================
/// \code
/// Input:
///   iFunctions    0=reset;  1=blockFunctions;   -1=do not modify
///   iInput        0=reset;  1=blockInput;       -1=do not modify
///   iCursor       0=reset;  1=waitCursor;       -1=do not modify
///
/// see also UI_block_input UI_func_stat_set__
/// \endcode

  int   ii;
  // static char actFun=0, actCur=0;


  // printf("UI_block__ mode=%d actMode=%d\n",mode,actMode);



  //----------------------------------------------------------------
  L_cur:
  if((iCursor < 0) ||
     (UI_block_cur == iCursor)) goto L_func;

  if(iCursor == 0)
  UI_CursorWait (1);    // reset cursor

  if(iCursor == 1) {
    ii = UI_CursorWait (0);    // wait-cursor
  }

  UI_block_cur = iCursor;


  //----------------------------------------------------------------
  L_func:
  // 0 = reset to normal
  if((iFunctions < 0) ||
     (UI_block_func == iFunctions)) goto L_inp;

  // unlock some application-functions...
  if(iFunctions == 0) {
    UI_func_stat_set__ (
                      // APF_TB_CAD,
                      APF_MEN0,
                      APF_WIN_B_E,   // Browser-window & Editor-window
                      APF_HIDE,
                      APF_VWCADMAN,
                      APF_BUT_GSE,
                      APF_PLUGINS,
                      APF_UNDO,
                      0);
  }

  if(iFunctions == 1) {
    UI_func_stat_set__ (
      // -APF_TB_CAD,
      -APF_MEN0,
      -APF_WIN_B_E,    // Browser-window & Editor-window
      -APF_HIDE,
      -APF_VWCADMAN,
      -APF_BUT_GSE,
      -APF_PLUGINS,
      -APF_UNDO,
      0);
  }

  UI_block_func = iFunctions;


  //----------------------------------------------------------------
  L_inp:
  if((iInput < 0) ||
     (UI_block_inp == iInput)) goto L_exit;

  if(iInput == 0)
  GUI_gl_block (&winMain, 0);   // unblock keystrokes & grafic_selections

  if(iInput == 1)
  GUI_gl_block (&winMain, 1);   // block keystrokes & grafic_selections

  UI_block_inp = iInput;




  //----------------------------------------------------------------
  L_exit:
  return 0;

}


//================================================================
  int UI_upd_ptDisp () {
//================================================================

  if(APT_dispPT == OFF) {
    // hide pts
    GUI_menu_checkbox_set (&ckb_ptDisp, TRUE);

  } else {
    // view pts (std)
    GUI_menu_checkbox_set (&ckb_ptDisp, FALSE);
  }

  return 0;
}
 

//================================================================
  int UI_upd_plDisp () {
//================================================================
// change checkbox "PlanesDisplay OFF"


  if(APT_dispPL == OFF) {
    // hide pts
    GUI_menu_checkbox_set (&ckb_plDisp, TRUE);

  } else {
    // view pts (std)
    GUI_menu_checkbox_set (&ckb_plDisp, FALSE);
  }

  return 0;
}



//================================================================
  int UI_func_stat_set__ (int i0, ...) {
//================================================================
/// \code
/// activate / disactivate Functions.
/// values: see ../xa/xa_ui.h
/// last argument must be 0 !
/// negative values: disable; positive values: enable.
/// see also UI_func_stat_set_tab UI_block_input UI_block__
/// \endcode

  int     i1;
  short   sTab[2]= {0, 0};
  va_list va;

  // printf("AP_func_stat_set__ %d\n",i0);

  va_start (va, i0);
  sTab[0] = i0;
  UI_func_stat_set_tab (sTab);    // WORK

  L_nxt_arg:
    i1 = va_arg (va, int);
      // printf("       _stat_set__ %d\n",i1);
    if(i1 == 0) {
      va_end (va);
      return 0;
    }
    sTab[0] = i1;
    UI_func_stat_set_tab (sTab);    // WORK
    goto L_nxt_arg;
}


//================================================================
  int UI_func_stat_set_tab (short *sTab) { 
//================================================================
/// \code
/// activate / disactivate Functions.  View/Hide toolbars !
/// values: see ../xa/xa_ui.h
/// last argument must be 0 !
/// negative values: disable; positive values: enable.
/// see also UI_func_stat_set__
/// \endcode

// see also UI_mBars_off 

  int   ii, iDat, mode;


  // printf("UI_func_stat_set_tab \n");

  // loop tru table
  ii = -1;


  L_nxt_record:
  ++ii;
  iDat = sTab[ii];
  if(iDat == APF_EndTab) return 0;
    // printf(" sTab[%d] = %d\n",ii,sTab[ii]);

  if(iDat > 0) {
    mode = TRUE;
  } else {
    mode = FALSE;
    iDat = -iDat;
  }

    // printf("UI_func_stat_set_tab iDat=%d mode=%d\n",iDat,mode);


  switch (iDat) {

    case APF_TB_CAD:
      if(UI_InpMode == UI_MODE_CAD) {
        // GUI_set_enable (&tbCad, mode);
        GUI_set_show (&tbCad, mode);
        // if(mode == TRUE) GUI_obj_view (tbCad);     // restore Toolbar
        // else             GUI_obj_hide (tbCad);
      }
      break;

/*
    case APF_TB_PED:
      if(mode == TRUE) GUI_obj_view (PEDtb);
      else             GUI_obj_hide (PEDtb);
      break;


    case APF_TB_TED:
      if(mode == TRUE) GUI_obj_view (TEDtb);
      else             GUI_obj_hide (TEDtb);
      break;
*/

  
    case APF_MEN0:     // gesamten oberen Menubar, von File bis Help !
      GUI_set_enable (&menu_bar, mode);
      break;

    case APF_MEN_FIL:  // Menu-File
      GUI_set_enable (&men_fil, mode);
      break;

    case APF_MEN_SM:   // Menu-subModels
      GUI_set_enable (&men_mod, mode);
      break;

    case APF_MEN_MDF:  // Menu-Modify
      GUI_set_enable (&men_mdf, mode);
      break;

    case APF_MEN_INS:  // Insert im oberen Menubar
      GUI_set_enable (&men_ins, mode);
      break;

    case APF_MEN_SEL:  // Select im oberen Menubar
      GUI_set_enable (&men_sel, mode);
      break;

    case APF_MEN_CAT:  // Catalog im oberen Menubar
      GUI_set_enable (&men_cat, mode);
      break;

    case APF_MEN_APP:  // Menu-Applications
      GUI_set_enable (&men_app, mode);
      break;

    case APF_MEN_PRC:  // Menu-process
      GUI_set_enable (&men_prc, mode);
      break;

    case APF_MEN_RPC:  // Menu-remote
      GUI_set_enable (&men_rpc, mode);
      break;

    case APF_WIN_B_E:  // Browser-window & Editor-window
        // printf("****** brwStat=%d edStat=%d\n",winBrStat,win_edStat);
      if(winBrStat > 0)
        GUI_set_enable (&winBrw, mode); // Browser-window
      if(win_edStat > 0)
        GUI_set_enable (&winED, mode); // Editor-window
      break;

    case APF_HIDE:     // HIDE & VIEW - Box
      GUI_set_enable (&box1C2v, mode);
      break;

    case APF_VWCADMAN:  // VWR CAD MAN
      GUI_set_enable (&box1A, mode);
      break;

    case APF_VWR:
      GUI_set_enable (&ckb_vwr, mode);
      break;

    case APF_CAD:
      GUI_set_enable (&ckb_cad, mode);
      break;

    case APF_MAN:
      GUI_set_enable (&ckb_man, mode);
      break;

    case APF_MEASEA:   // Search/Name
      GUI_set_enable (&box1X, mode);
      // VWR/CAD/MAN sind nicht in gemeinsamer Box ! ckb_vwr ckb_cad ckb_man
      break;

    case APF_BUT_GSE:  // Buttons GO STEP END
      GUI_set_enable (&box1C1v, mode);
      break;

    case APF_BUT_GO:   // Button GO
      GUI_set_enable (&but_go, mode);
      break;

    case APF_BUT_STEP: // Button STEP
      GUI_set_enable (&but_step, mode);
      break;

    case APF_BUT_END:  // Button END
      GUI_set_enable (&but_end, mode);
      break;

    case APF_PLUGINS:  // Plugins (userPrograms)
      // GUI_set_enable (&box1Y, mode);
      GUI_set_enable (&men_plu, mode);
      break;

    case APF_UNDO:     // Undo/Redo; see also UNDO_lock()
      if(mode == TRUE) {
        UNDO_lock (1);     // unlock Undo/Redo
      } else {
        UNDO_lock (0);     // lock UNDO/REDO
      }
      break;



    // case APF_:
      // break;


    default:
      printf("*** ERROR UI_func_stat_set_tab %d\n",iDat);
  }

  goto L_nxt_record;

  //----------------------------------------------------------------
/*
box1C1v, box1X, box1Y, wTx->view, ckb_mdel, boxRelAbs, ckb_Iact  

  GUI_set_enable (box1X, mode);   // VWR/CAD/MAN/Wcut/Search/Modify
  GUI_set_enable (box1Y, mode);   // userPrograms
  GUI_set_enable (UIw_file_new,  mode);
  GUI_set_enable (UIw_file_ope,  mode);
  GUI_set_enable (UIw_file_opD,  mode);
  GUI_set_enable (UIw_file_ins,  mode);
  GUI_set_enable (UIw_file_sav1, mode);
  GUI_set_enable (UIw_file_sav2, mode);
  GUI_set_enable (UIw_file_sav3, mode);
  // GUI_set_enable (men_imp, mode);
  // GUI_set_enable (men_exp, mode);
        GUI_set_enable (ckb_mdel, FALSE);
  DL_pick__ (1);                  // reset selectionfilter
*/

  return 0;

}



//================================================================
  int UI_brw__ (int mode) {
//================================================================
/// \code
/// switch browser/editor-window ON|OFF.
/// mode      UI_FuncGet - returns state (ON|OFF)
/// mode     -1         - switch browser/editor-window off
/// mode      0         - update window (acc. UI_InpMode, win_panStat)
/// mode      1         - switch browser/editor-window on
/// \endcode

// win_panStat   1=paned (browser|editor) activ; 0=off
// state: win_edStat, winBrStat; 1=activ; 0=hidden

// if EDITOR is active: always activate editor-window; return.
// else hide EDITOR; activate Browser, if Browser is not diabled.


  // printf("UI_brw__ mode=%d Brw=%d ED=%d\n",mode,winBrStat,win_edStat);
  // printf("  UI_InpMode=%d win_panStat=%d\n",UI_InpMode,win_panStat); 



  //-----------------------------------------------------------
  // switch browser-window off
  if(mode == -1) { 
    // do not activated in MAN-mode
    if(UI_InpMode == UI_MODE_MAN) {
      printf(" skip - MAN is active ..\n");
      goto L_exit;
    }
    GUI_box_paned_siz (&win_paned, 0);  // remove paned-box
    win_panStat = 0;
    winBrStat = 0;
    win_edStat = 0;
    goto L_exit;
  }


  //-----------------------------------------------------------
  // switch browser-window on
  if(mode == 1) {
    GUI_box_paned_siz (&win_paned, 180);   // restore paned-box
    win_panStat = 1;
    // Brw_Mdl_upd ();   // 2013-10-09
  }



  //-----------------------------------------------------------
  // 0 = UPDATE
  if(!win_panStat) return 0;

  if(UI_InpMode == UI_MODE_MAN) {
      // printf(" activate MAN\n");
    // hide browser
    if(winBrStat == 1) {            // browser = active

      GUI_set_show (&winED, 1);
      GUI_set_enable (&winED, 1);
      GUI_set_show (&win_edi, 1);           // activate editor
      GUI_notebook_set (&UI_box_ntb, 1);

      GUI_set_enable (&winBrw, 0);
      GUI_set_enable (&win_brw, 0);         // 1=ON, 0=OFF
      GUI_set_show (&win_brw, 0);           // disactivate browser
    // } else {
    }
    winBrStat = 0;
    win_edStat = 1;
    goto L_exit;
  }


  // VWR od CAD
    // printf(" activate VWR/MAN\n");
  // hide editor
  if(win_edStat == 1) {        // editor = active   

    GUI_set_enable (&winBrw, 1);
    GUI_set_enable (&win_brw, 1);
    GUI_set_show (&win_brw, 1);           // activate browser
    GUI_notebook_set (&UI_box_ntb, 0);

    GUI_set_show (&win_edi, 0);           // disactivate editor
    GUI_set_show (&winED, 0);
    GUI_set_enable (&winED, 0);

    // } else {
  }
  win_edStat = 0;
  winBrStat = 1;

  Brw_Mdl_upd ();      // update browser 2013-10-11


  L_exit:
  // printf("ex UI_brw__ Brw=%d ED=%d\n",winBrStat,win_edStat);

  return 0;

}


//================================================================
  int UI_wait_Esc () {
//================================================================

// Wait for pressing the Esc-Key ...
// Problem: man muesste den Key resetten, damit er nicht nochmal durchgeht,
// oder fuer ca 0.5 sec als nicht gedrueckt setzen ...
 // (auf off setzen und von einem Timer wieder einschalten lassen ...
 // Oder einfach ausschalten ?

  int     i1;


  printf("key Esc to continue .......\n");

  i1 = 1;

  while (i1 == 1) {
    i1 = UI_askEscape();
    // Display zwischendurch updaten
    // for(i1=0; i2<10; ++i2) {
      // usleep(10);
      DL_Redraw ();
    // }
  }

  return 0;

}


//=====================================================================
  int UI_askEscape () {
//=====================================================================
// alle pending events abarbeiten;
// RC =  1 = OFF  Esc nicht gedrueckt
// RC = -1        Escape-taste ist gerade gedrueckt
//


  // printf("UI_askEscape\n");

  // GUI_edi_Focus (&winED);

  GUI_update__ ();

  if(KeyStatEscape != 1) {
    KeyStatEscape = 1;         // reset
    return -1;
  }

  return 1;

}

/*
//=====================================================================
  int UI_ask_mode () {
//=====================================================================
// returns UI_MODE_VWR/UI_MODE_CAD/UI_MODE_MAN.


  int mode;

  if(UI_InpMode == UI_MODE_MAN)        mode = UI_MODE_MAN;

  else if(UI_InpMode == UI_MODE_CAD) {
    if(tbCad == NULL)                       mode = UI_MODE_VWR;
    else if(GTK_WIDGET_VISIBLE(tbCad) == 0) mode = UI_MODE_VWR;
    else                                    mode = UI_MODE_CAD;
  }

  return mode;

}
*/


//=====================================================================
  int UI_askExit   (MemObj *mo, void **data) {
//=====================================================================

  int   i1;

  i1 = 0;

  // printf("UI_askExit\n");

/*
  // exit NC-Tech
  if(UI_InpMode == UI_MODE_WCTEC) {
    // i1 = UI_wcTec_askExit(parent,(char*)data);
    i1 = UI_wcTec_askExit(NULL, "");
    ED_work_END (0);



  } else if(UI_InpMode == UI_MODE_WCGEO) {
    i1 = UI_wcg_askExit(parent,(char*)data);
*/


  if       (UI_InpMode == UI_MODE_CAD) {
      // stop CAD (exit Insert)
      IE_cad_exit0 ();
  }


  return i1;

}



/*
//=====================================================================
  int UI_upd_Refs () {
//=====================================================================
// Update Refsys-display

  char txbuf[64];

  sprintf(txbuf, "%03d", APT_2d_to_3d_Ind);
  GUI_label_mod ((GtkLabel*) (UI_Refsys), txbuf);

  return 0;

}
*/


//=====================================================================
  int UI_wireCB (MemObj *mo, void **data) {
//=====================================================================
// set GL-shaden

  int  i2;


  // printf("UI_wireCB\n");


  i2 = GUI_menu_checkbox_get (&ckb_backW);
  if(i2) GL_backgrnd_1 (0);  // ON
  else   GL_backgrnd_1 (1);  // OFF


  // Status der Checkbox abfragen
  UI_stat_3D = GUI_ckbutt_get (&ckb_3D);
    // printf(" UI_stat_3D=%d\n",UI_stat_3D);


  // Status der Checkbox "Shade" abfragen
  UI_stat_shade = GUI_ckbutt_get (&ckb_shd);

  // Status der Checkbox "Extra Light" abfragen
  i2 = GUI_menu_checkbox_get (&ckb_light);


  UI_GR_DrawInit ();

  if(UI_stat_shade == 0) {
    GL_DefineDisp (FUNC_DispWire, 0);    // Default = Wireframe
  } else {
    GL_DefineDisp (FUNC_DispRend, i2);
  }

  UI_GR_DrawExit ();

  return 0;

}



/*
//===============================================================
  int UI_sur_act_CB2 (GtkWidget *widget, GdkEventAny *event, gpointer data) {
//===============================================================
// see also UI_GR_CB_Sel2


  static long isel=-1;

  // printf("UI_sur_act_CB2 \n");


  // Unix-GTK2:
  //   GDK_BUTTON_RELEASE  GDK_ENTER_NOTIFY GDK_UNMAP bei select
  //   GDK_BUTTON_RELEASE  GDK_UNMAP                  bei cancel
  //   Bei MS-Win kommt kein UNMAP !!!
  if(event->type == GDK_BUTTON_RELEASE) {
    if(isel >= 0) UI_sur_act_CB1 (NULL, isel);

  } else if (event->type == GDK_LEAVE_NOTIFY) {
    isel = -1;

  } else if (event->type == GDK_ENTER_NOTIFY) {
    isel = LONG_PTR(data);

  } else if (event->type == GDK_UNMAP) {
    UI_sur_act_CB1 (NULL, isel);

  }

  return FALSE;  // GTK2 ! was TRUE

}


//=======================================================================
  int UI_sur_act_CB1 (GtkWidget *parent, void *data) {
//=======================================================================
// data = -1: build List of ConstrPlanes
// data >= 0: listSelection; activate new ConstrPlane.
// sel 0=XY, 1=XZ, 2=YZ

  int      i1, i2, i3;
  long     l1;
  Vector   vc1, vc2;
  // Mat_4x3  m1;


#define LstSiz  200   // Anzahl Listenelemente
static char *LstPtr[LstSiz];
static char LstBuf[LstSiz][32];



  i1 = INT_PTR(data);

  // printf("UI_wcg_CBprog %d\n",(int)data);
  printf("UI_sur_act_CB1 %d\n",i1);


  if(i1 >= 0) goto L_activ1;

  strcpy(LstBuf[0], "RZ (XY)");
  strcpy(LstBuf[1], "RY (XZ)");
  strcpy(LstBuf[2], "RX (YZ)");
  LstPtr[0]=LstBuf[0];
  LstPtr[1]=LstBuf[1];
  LstPtr[2]=LstBuf[2];
  LstPtr[3]=NULL;


  // DB_GetRef
  i1 = 3;
  for(l1=1; l1<1000000; ++l1) {
    i2 = DB_CkRef (l1);
    if(i2 == -2) break;
    if(i2 >= 0) {
      sprintf(LstBuf[i1],"R%ld",l1);
      LstPtr[i1]=LstBuf[i1];
      printf(" +Ref[%d] |%s| %ld\n",i1,LstBuf[i1],l1);
      ++i1;
      LstPtr[i1]=NULL;
    }
  }

  // UI_sur_act = GUI_OptMen (parent, surLst, UI_sur_act_CB1, 0);
  GUI_Popup (LstPtr, UI_sur_act_CB1, UI_sur_act_CB2);

  return 0;



  //=================================================================
  L_activ1:

  // display ID of new ConstPln
  printf(" activate ConstPln |%s|\n",LstBuf[i1]);


  WC_sur_Z = 0.;

  if(i1 == 0)  { // 0=XY
      vc1 = UT3D_VECTOR_X;
      vc2 = UT3D_VECTOR_Y;
      strcpy(WC_ConstPl_Z, "RZ");


  } else if(i1 == 1)  { // 1=XZ
      vc1 = UT3D_VECTOR_Z;
      vc2 = UT3D_VECTOR_X;
      strcpy(WC_ConstPl_Z, "RY");


  } else if(i1 == 2)  { // 2=YZ
      vc1 = UT3D_VECTOR_Y;
      vc2 = UT3D_VECTOR_Z;
      strcpy(WC_ConstPl_Z, "RX");


  } else {

    printf("  sel: |%s|\n",LstPtr[i1]);
    strcpy(WC_ConstPl_Z, LstPtr[i1]);

    APED_dbo_oid (&i2, &i3, LstPtr[i1]);
    DB_GetRef (&WC_sur_act, i3);
    goto L_activ2;
  }

  // WC_sur_act setzen
  // UI_AP (UI_FuncSet, UID_ouf_coz, (void*)&WC_sur_Z);


  UT3D_pl_pt2vc (&WC_sur_act, (Point*)&UT3D_PT_NUL, &vc1, &vc2);



  L_activ2:
  GL_SetConstrPln ();

  UI_Set_ConstPl_Z (); // write Label name_of_the_Constr.Plane


  return 0;

}
*/


//================================================================
  int UI_Set_actPrg (char *prgNam, int iCol) {
//================================================================
// display label active program-name
// Input:
//   prgNam    NULL=do_not_modify
//   iCol      0=black; 1=red, 2=blue, -1=do_not_modify_color


  // printf("UI_Set_actPrg |%s| %d\n",prgNam,iCol);


  if(iCol >= 0)
  // GUI_mod_styl (UIw_prg, iCol);
  GUI_label_styl (&UIw_prg, iCol);

  if(prgNam)
  GUI_label_mod (&UIw_prg, prgNam);


  return 0;
}


//================================================================
  int UI_Set_infoSel (int mode) {
//================================================================
// eg "<span fgcolor=\"#ff0000\" weight=\"bold\">xy</span>"

  static char *sAct;
  static char *s0 = "-";
  static char *s1 =
  "<span fgcolor=\"#ff0000\" weight=\"bold\">Point on Constr.plane</span>";
  static char *s18 =
  "<span fgcolor=\"#ff0000\" weight=\"bold\">Point on Obj</span>";
  static char *s19 =
  "<span fgcolor=\"#ff0000\" weight=\"bold\">Line from poylgon/contour</span>";

  char   *p1;


  // printf("UI_Set_infoSel %d\n",mode);

  p1 =  s0;
  if(mode ==  1) p1 =  s1;
  if(mode == 18) p1 = s18;
  if(mode == 19) p1 = s19;

  if(p1 != sAct) {
    GUI_label_htm_mod (&UIw_infoSel, p1);
    sAct = p1;
  }

  return 0;

}

 
//================================================================
  int UI_Set_typPrg () {
//================================================================
// display type active program-name
// Input:
//   APP_act_typ  1=applic. 2=process 3=plugin 4=remote;
//             // APP       PRC       PLU      RPC

  static char *spTyp[] = {"-","Application","Process","Plugin","RemoteProcess"};


  // printf("UI_Set_typPrg\n");

  if(APP_act_typ > 0) {
    GUI_label_mod (&UIw_typPrg, spTyp[APP_act_typ]);
  }

  return 0;
}




//=====================================================================
  int UI_Set_ConstPl_Z () {
//=====================================================================
// display active Constr.Plane and en/dis-able the rel|abs checkboxes
// WC_ConstPl_Z  name_of_the_Constr.Plane   String; zB "RZ" od R20"
// diplay "ConstrPln   R20"  (upper right)


  // do nothing in submodels
  // do nothing during run;
  // only in edit-mode (MAN or CAD)

  // printf("ZZZZZZZZZZZZZZZ UI_Set_ConstPl_Z ZZZZZZZZZZZZZZZZZZ\n");
  // printf(" ED_mode_go=%d\n",ED_query_mode());


  if(ED_query_mode() == ED_mode_go) return 0;


  GUI_label_mod (&UI_ConstP, WC_ConstPl_Z);

  // activate the rel - abs checkBoxes
  if(WC_sur_ind == 0) UI_RelAbs_act (0); // 1=activ, 0=inaktiv.
  else                UI_RelAbs_act (1);

  return 0;
}




//================================================================
  int UI_view_Z_CB (MemObj *mo, void **data) {
//================================================================

  // printf("UI_sur_Z_CB\n");


  // first focus-in
  UI_Focus=2;      // wer Focus hat; 0=GL, 1=Edit, 2=ViewZ-Entryfeld

  return 0;

}

/*
//================================================================
  gint UI_sur_Z_CB (GtkWidget *widget, GdkEventKey *event) {
//================================================================

  // printf("UI_sur_Z_CB\n");


  // first focus-in
  UI_Focus=3;      // wer Focus hat; 0=GL, 1=Edit, 2=ViewZ-Entryfeld

  return 0;

}
*/


//=======================================================================
  int UI_suract_keyIn (int mode) {
//=======================================================================
// mode=1 = new ViewZ
// mode=2 = new ConstZ


  char   *txt;
  double  sur_act;


  // printf("UI_suract_keyIn %d\n",mode);


  switch (mode) {


    //------------------------------------------------------------
    // mode=1 = new ViewZ
    case 1:

      txt = GUI_entry_get (&UI_view_Z);
      sur_act = atof(txt);
        // printf("got %f\n",sur_act);

      // den neuen ViewZ-Wert aktivieren
      UI_GR_view_set_Z1 (sur_act);


      /* Reset focus to glarea widget */
      // UI_GR_setKeyFocus ();
      break;


/*
    //------------------------------------------------------------
    // mode=2 = new ConstZ
    case 2:

      txt = gtk_entry_get_text ((GtkEntry*) (UI_sur_Z));
      sur_act = atof(txt);
        printf("got %f\n",sur_act);

      // den neuen Z-Wert an WC_sur_Z uebergeben
      WC_sur_Z = sur_act;

      // UI_GR_view_set_Z1 (sur_act);
      GL_SetConstrPln ();
      DL_Redraw ();


      // Reset focus to glarea widget
      // UI_GR_setKeyFocus ();
      break;
*/


  }


  // set focus to glarea widget
  // UI_GR_setKeyFocus ();
  GUI_obj_focus (&winGR);


  return 0;

}


/* UNUSED
//===============================================================
  gint UI_Enter (GtkWidget *widget, GdkEventCrossing *event) {
//===============================================================
// reset KeyStatEscape to normal if cursor enters Window ..

  // int    x, y;
  // GdkModifierType state;


  // printf(">>>>>>> UI_Enter %d sysStat=%d\n",UI_Focus,AP_stat.sysStat);
  // printf(" event.typ=%d\n");



  if(AP_stat.sysStat < 2) return TRUE;   // wait until window is up ..


  // nach visibility_notify_event notwendig !
  DL_Redraw ();



  if(UI_Focus == 1)
    GUI_edi_Focus (&winED);
  // else
    // UI_GR_setKeyFocus();   // set Focus to OpenGL-window


  // wird nicht OFF gesetzt, wenn Cursor ausserhalb Window !!
  KeyStatEscape = OFF;


  return FALSE;
  // return TRUE; damit kein Cursor im textWin moeglich !

}
*/
/*
//=====================================================================
  int UI_creObjHdr (GtkWidget *parent, void *data) {
//=====================================================================
// ObjHeader zB "P20=" generieren und gleich ins File schreiben

  long      ind;
  int       typ, istart;
  char      buf[32];
  gchar     *gtxt;
  GtkWidget *w_nam;

  typ = INT_PTR(data);


  // printf("UI_creObjHdr %d\n",typ);


/
  // if(typ == Typ_PT) {
    // w_nam = UI_inf_sip;

  // } else if(typ == Typ_LN) {
    // w_nam = UI_inf_sil;

  // } else if(typ == Typ_CI) {
    // w_nam = UI_inf_sic;

  // } else if(typ == Typ_PLN) {
    // w_nam = UI_inf_sir;

  // } else if(typ == Typ_VC) {
    // w_nam = UI_inf_sid;

  // } else if(typ == Typ_CV) {
    // w_nam = UI_inf_sis;
  // }

  // // aus dem CAD-entry einlesen ?
  // gtxt = gtk_entry_get_text ((GtkEntry*) (w_nam));
  // // gtxt = gtk_editable_get_chars ((GtkEntry*) (w_nam), 1, 5);
  // printf("StartInd=|%s|\n",gtxt);
  // istart = atoi(gtxt);
  // // printf("StartInd=|%s| %d\n",gtxt,istart);
/
  istart = 20;


  ind = AP_cre_defHdr (typ, istart);

/
  // // den StartIndex im CAD-Panel setzen
  // // UI_AP (UI_FuncSet, UID_inf_sip, (void*)ind);
  // // sprintf(buf, "%6ld", ind);    // 999999 rechtsbuendig mit blanks
  // sprintf(buf, "%d", ind);    // 999999 rechtsbuendig mit blanks
  // GUI_entry_set ((GtkEntry*) (w_nam), buf);

  // // g_free (gtxt);  // damit Absturz !!
/

  return ind;

}
*/

/* UNUSED
//=====================================================================
  gint UI_EdKeyRelease (GtkWidget *parent, GdkEventKey *event) {
//=====================================================================
// callback KeyRelease im editwin
//

  int  lNr;
  long ipos;


  // printf("UI_EdKeyRelease %x /%c/\n",event->keyval,event->keyval);
  // printf(" Ctrl=%d Alt=%d Shift=%d\n",KeyStatCtrl,KeyStatAlt,KeyStatShift);


  switch (event->keyval) {

  case GDK_Escape:
    KeyStatEscape = OFF;
    // KeyStatEscape += 1;            // wenn Release vor press ..
    // 205-05-02: weg; in einem Plugin (flat) nach Abbruch m. Esc kein END
    // mehr moeglich !!
    // printf("UI_EdKeyRelease Esc EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
    return TRUE;   



  case GDK_Return:
  case GDK_Left:
  case GDK_Right:
  case GDK_Up:
  case GDK_Down:
      // printf("UI_EdKeyRelease-new curPos\n");
    if(KeyStatShift == ON) return FALSE;   // nix tun
    // ACHTUNG: kommt auch in CAD !
    if(UI_InpMode == UI_MODE_MAN) {
      ED_newPos ();
      goto Finish;
    }
    return TRUE;  // do no defOps



  // GDK_Alt_L GDK_Alt_R GDK_Meta_L GDK_Meta_R
  case GDK_Alt_L:
  case GDK_Alt_R:
  case GDK_Meta_L:
  case GDK_Meta_R:
    // printf("Alt OFF\n");
    KeyStatAlt = OFF;
    // goto Finish;  ??
    return TRUE;



  case GDK_Shift_L:
  case GDK_Shift_R:
    // printf("GR_Func_Act=%d\n",GR_Func_Act);
    if(GR_Func_Act != 0) {
      GR_Func_Act = 0;
      // UI_ChangeCursor (GDK_LEFT_PTR); // default
      UI_ResetCursor ();
    }
    KeyStatShift = OFF;
      // printf("Shift OFF\n");
    return TRUE;



  case GDK_Control_L:
  case GDK_Control_R:
    if(GR_Func_Act != 0) {
      GR_Func_Act = 0;
      // UI_ChangeCursor (GDK_LEFT_PTR); // default
      UI_ResetCursor ();
    }
    KeyStatCtrl = OFF;
      // printf("Ctrl OFF\n");
    return TRUE;



  }

  return FALSE;




  Finish:
  // prevent the default handler from being run
  // WARUM ?
  gtk_signal_emit_stop_by_name (GTK_OBJECT(parent),"key_release_event");
  return TRUE;


}
*/
/* UNUSED
//========================================================================
  gint UI_EdButtonPress (GtkWidget *widget, GdkEventButton *event) {
//========================================================================
// klick into Editor-window.
// liefert immer die alte Curpos


  long          l1, ipos;

  // printf("==============\nUI_EdButtonPress %d\n",event->button);


  // CurPos holen
  // UI_AP (UI_FuncGet, UID_Edit_Cpos, (void*)&ipos);
  // UI_Ed_CurPos1 = ipos;   // CursorPos vor einem Insert
  // printf("curp=%d\n",ipos);


  // if(event->button == 2) {         // insert



  // MAN: M3 soll "advance 1 Line" nicht set Cursor.
  if(event->button == 3) {
    if(KeyStatCtrl == ON) {
      UI_EdKeyCtrlM3 ();
    } else {
      UI_EdKeyCR (2);
    }
    goto AllDone;
  }


  // printf("ex UI_EdButtonPress %d\n",UI_Ed_fsiz);
  return FALSE;


  AllDone:
  // prevent the default handler from being run
  gtk_signal_emit_stop_by_name (GTK_OBJECT(widget),"button_press_event");
  return TRUE;

}
*/
/* UNUSED
//================================================================
  gint UI_EdButtonRelease (GtkWidget *widget, GdkEventButton *event) {
// Click into EditWindow.  Kommt nur mehr in MAN.
// Mode manuell: nur Focus auf EditWin.
// Wenn CAD aktiv: aktuelle Zeile auslesen, analysieren, eintragen.



  int             lNr, i1, x, y;
  long            ipos, fSiz, l1, l2;

  // GdkModifierType state;
  // GdkEventType    type;


  // if (event->is_hint) {
    // gdk_window_get_pointer (event->window, &x, &y, &state);
  // }

  // event->type = 7 = GDK_BUTTON_RELEASE


  // printf("==============\nUI_EdButtonRelease %d\n",event->button);
  // i1 = event->button;

  ED_newPos ();
  UI_Focus = 1;
  GUI_edi_Focus (&winED);
  return FALSE;


  //----------------------------------------------------------------
  // CAD:
// / gibts nicht mehr; 2004-12-15
  // if(UI_InpMode == UI_MODE_CAD) {


    // //.............. CAD ohne SM ...............................
    // // wenn (mode=CAD) und (SM = OFF)
    // if(UI_InpSM != ON) {

      // //und nicht EOF: SM einschalten.
      // if(ipos < fSiz) {

        // // Search-Mode einschalten
        // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_mdel), TRUE);


      // // sind am EOF: activate
      // } else {
        // IE_activate();
      // }

      // goto Fertig;



    // } else {
    // //.............. CAD und SM ...............................
      // // wenn (mode=CAD) und (SM = ON)

      // // EOF ?
      // if(fSiz <= ipos) {
        // // sind am EOF: SM ausschalten, Mode Add.
        // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_mdel), FALSE);
        // goto Fertig;


      // } else {
        // // CAD u SM: wenn schon aktive Zeile selektiert wurde,
        // if(lNr == ED_get_lnr_act()) {
          // // dann Search beenden und in den Modify-Mode gehen.
          // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_mdel), FALSE);
// 
        // } else {
        // // normales CAD+SM: das Obj in der aktuellen Zeile hiliten
        // UI_hili_actLn ();
        // }
        // goto Fertig;
      // }
    // }
  // }
// /


  //----------------------------------------------------------------
/
  // int             x, y;
  // GdkModifierType state;
// 
  // if (event->is_hint) {
    // gdk_window_get_pointer (event->window, &x, &y, &state);
  // }
// 
  // // printf("UI_EdButtonPress %d %d %d\n",x, y, state);
// 
// 
  // // if (state & GDK_BUTTON1_MASK) {
    // // printf("+B1\n");
// 
// 
  // // Mode manuell: nur Focus auf EditWin.
  // if(UI_InpMode == UI_MODE_CAD) goto L_CAD;
    // GUI_edi_Focus (&winED);
// 
    // // Ctrl M1 = Return
    // // if(KeyStatCtrl == ON) {
      // // IE_cad_InpIn_CR ();
    // // }
// 
    // goto Fertig;
// 
// 
  // //============== CAD aktiv: ========================================
  // L_CAD:
// 
// 
  // // das entspr. Menue aktivieren
  // IE_activate ();
// 
// 
  // Fertig:
  // AP_lNr_set (lNr);
// 
  // // printf("ex UI_EdButtonRelease %d\n",lNr);
  // return FALSE;
// /
}
*/


//=====================================================================
  int UI_EdKeyCtrlM3 () {
//=====================================================================
// Ctrl M3 pressed
//

  int   i1, iFilt, iFunc, iGrp;

  printf("UI_EdKeyCtrlM3 \n");


  // if(UI_InpSM == ON) {   // wenn Search: Done (Exit)
    // UI_WinSM (NULL, (void*)UI_FuncExit);   // exit SM
    // TX_Print("- modify => add");
    // return 0;
  // }



  iFilt = UI_GR_Sel_Filter(-1);
  if(iFilt != 0) {
    // printf(" GR_Sel_Filter=%d\n",iFilt);
    if(iFilt == 17) {      // Form active
      PRG_key_CB ("CMBR");
      return 0;
    }
  }


  iGrp  = Grp_get_nr ();


  //----------------------------------------------------------------
  if(UI_InpMode == UI_MODE_CAD) {
    // CtrlM3 in CAD:
    // test if cad-function is active:
    iFunc = IE_get_Func ();
    if(iFunc < 0) {
      // no cad-function is active:
      if(iGrp > 0) {
        // yes, group exists; display M3-menu
        OMN_popup_Brw (Typ_TmpPT, 0, 0, 0);
        return 0;
      }
      // // no group, no func: exit CAD.
      // UI_reset__ (); // back to VWR

    } else {
      // cad-function is active:
      if(IE_get_modify() == 1) {           // 1=modify
        IE_cad_INS ("Exit");               // exit mode Modify; enter mode Add
  
      } else {
        // mode=0=add;2=Insert: OK
        UI_EdKeyCR (2);
      }
    }



  //----------------------------------------------------------------
  } else if(UI_InpMode == UI_MODE_MAN) {
    // CtrlM3 in MAN;
    if(iGrp > 0) goto L_M3_menu;
    // OK in MAN
    UI_EdKeyCR (2);



  //----------------------------------------------------------------
  } else if(UI_InpMode == UI_MODE_VWR) {
    // CtrlM3 in VWR:
    goto L_M3_menu;
  }

  return 0;
  

  //----------------------------------------------------------------
  L_M3_menu:
    // test if group exists
    if(iGrp > 0) {
      // yes, group exists; display M3-menu
      OMN_popup_Brw (Typ_TmpPT, 0, 0, 0);
    }
    return 0;

}


//=====================================================================
  int UI_EdKeyCR (int iCall) {
//=====================================================================
// iCall = 1: CR pressed (do not add CR; done by CR-key) 
// iCall = 2: M3 pressed (add CR to source)
//

  int   imode, iFilt;
  long  l1, l2;


  // printf("UI_EdKeyCR %d\n",iCall);


  // if(UI_InpSM == ON) {   // wenn Search: Modify ..
    // UI_WinSM (NULL, (void*)UI_FuncModify);
    // TX_Print("modify Line / Save: right mouseb / Exit Modify: Ctl right mouseb.");
    // goto AllDone;
  // }


  iFilt = UI_GR_Sel_Filter(-1);
  if(iFilt != 0) {
    // printf(" GR_Sel_Filter=%d\n",iFilt);
    if(iFilt == 17) {      // Form active
      PRG_key_CB ("MBR");
      return 0;
    }
    // clear filter:  // 2013-04-08
    UI_GR_Sel_Filter (0);
  } 




  // imode = UI_ask_mode();
  imode = UI_InpMode;


  //----------------------------------------------------------------
  if(imode == UI_MODE_VWR) {
    DL_hili_off (-1L); DL_Redraw (); // unhilite alle Objekte
  }


  //----------------------------------------------------------------
  if(imode == UI_MODE_CAD) {
    // printf(" CAD!!\n");
    IE_cad_InpIn_CR ();
    goto AllDone;
  }


  //----------------------------------------------------------------
  if(imode == UI_MODE_MAN) {

/*
    // test if cursor is at end of line ..
    if(ED_ck_lnEnd() != 0) {
      // cursor is inside a line; execute lien ..
      ED_work_STEP();   // advance 1 line
      return 0;
    }
*/

    // cursur is at the end of the line; add (insert) CR ..

    // DL_hili_off (-1L);               // unhilite alle Objekte
    GL_temp_del_all ();                   // remove indicate-circ   2010-06-03

    // switch to perm
    WC_set_obj_stat (0);  // 0=perm


    // get act. line

    // add (insert) CRLF & work line
    // if(iCall == 2) {                               // 2013-04-08
      UI_AP (UI_FuncSet, UID_Edit_Line, term_buf); // insert CRLF at curPos
    // } else {
        // ED_update (0L);
        // l1 = GUI_edi_getLnr (&winED);
        // // if(ED_lnr_act >= l1) ED_lnr_act = l1 - 1;
        // ED_set_lnr_act (l1);
      // UI_AP (UI_FuncSet, UID_ouf_lNr, (void*)l1);
    // }


    l1 = ED_get_lnr_act() - 1;  // get lNr AP_ED_lNr
    // printf(" lastLn %d |%s|\n",l1,mem_cbuf1); // filled by ED_Run
    UNDO_grp_add (l1, 0); // add neu generierte Zeile to Undo-List
    UNDO_upd__ ();

    goto AllDone;
  }


/*
  //----------------------------------------------------------------
  if(UI_InpMode == UI_MODE_WCTEC) {
    // imply OK-Butt
    UI_wcTec_CBtx (NULL, (void*)"OK");
    goto AllDone;
  }
*/


  AllDone:
    // printf("ex UI_EdKeyCR\n");
  return 0;

}


//=====================================================================
  int AP_UserKeyIn_reset () {
//=====================================================================

  // printf(" AP_UserKeyIn_reset\n");

  UI_UserKeyFunc = NULL;
  // AP_stat.APP_stat = 0;    // no plugin is active ..
  

  return 0;

}


//=====================================================================
  int AP_UserKeyIn_get (int uFunc ()) {
//=====================================================================
/// \code
/// request key-press-callback into function uFunc
///
/// prototyp uFunc:
/// int uFunc (int key);
///
/// reset with AP_UserKeyIn_reset
/// \endcode


  // printf(" AP_UserKeyIn_get\n");

  UI_UserKeyFunc = uFunc;

  return 0;

}


//===================================================================
  int AP_User_reset () {
//===================================================================
// alle reset-funcs, die bei MS-Win u Linux gleich sind

  // printf("AP_User_reset \n");

  AP_UserKeyIn_reset ();      // reset  KeyIn
  AP_UserSelection_reset ();  // reset  select
  AP_UserMousemove_reset ();  // reset user-mouseMovementCallback
  UI_GR_Sel_Filter (0);       // reset selectFilter.

  // reset red color of application-program in label UIw_prg
  UI_Set_actPrg (NULL, 0);

  APP_MSG_close ();        // close application-messages-file

  AP_stat.APP_stat = 0;    // no plugin is active ..

  // if(UI_User_appNamTab) UI_User_appNamTab = NULL;   // reset appObjNamTab
  PLU_appNamTab_set (NULL);

    // printf("ex AP_User_reset AP_stat.APP_stat=%d\n",AP_stat.APP_stat);


  return 0;

}


//=====================================================================
  int UI_save__ (int mode) {
//=====================================================================
/// \code
/// save Model AP_mod_fnam; AP_mod_fnam ist ohne Path, mit Filetyp.
/// Path = AP_mod_dir (mit "/" am Ende)
/// see also AP_Mod_load__
/// ACHTUNG: AP_mod_fnam = Input muss Filetyp haben; wird hier entfernt.
/// Input:
///   mode = 0:  check for overwrite
///   mode = 1:  overwrite
///   AP_mod_dir AP_mod_sym AP_mod_fnam AP_mod_ftyp AP_mod_iftyp
/// see also AP_save__
/// \endcode

  int   i1, ift;
  char  cbuf[256], mnam[256], s1[256];


  printf("UI_save__ %d\n",mode);
  printf("  |%s|%s|%s|%s| %d\n",AP_mod_sym,AP_mod_dir,
         AP_mod_fnam,AP_mod_ftyp,AP_mod_iftyp);


  // check if outDir AP_mod_dir exists
  if(OS_checkFilExist(AP_mod_dir,1) ==  0) {
    // TX_Error("Directory %s does not exist ..",AP_mod_dir);
    MSG_err_1 ("NOEX_dir", "%s", AP_mod_dir);
    return 0;
  }


  UI_CursorWait (0); // wait-cursor

  // clear Undo-List (sonst wilde gtk-Fehlermeldungen beim exit)
  UNDO_clear ();

  // get cbuf = absolute-filename
  Mod_fNam_set (cbuf, 0);


  // check if File exists
  if(mode == 0) {
    if(OS_checkFilExist(cbuf,1) == 1) {
      // sprintf(mnam,"overwrite file %s ?\n\n (Path %s)",AP_mod_fnam,AP_mod_dir);
      MSG_get_1 (mnam, 256, "OVER_fil", "%s", cbuf);
      // GUI_DialogYN (mnam, UI_save_over_CB);
      i1 = GUI_Dialog_2b (mnam, "OK", "Cancel");
      if(i1) return 0;
    }
  }

  // new deletes AP_mod_fnam
  strcpy(mnam, AP_mod_fnam);

    printf(" mnam=|%s|\n",mnam);
    printf(" cbuf=|%s|\n",cbuf);


    if(AP_mod_iftyp == Mtyp_Gcad) {
      // save native
      // Mainmodel und Submodels -> Datei AP_mod_fnam sichern
      Mod_sav__ (GUI_menu_checkbox_get(&ckb_compr), cbuf, 0);
      goto L_fertig;


    } else if(AP_mod_iftyp == Mtyp_DXF) {
      // printf(" Export DXF |%s|\n",AP_mod_fnam);
      OS_dll_do ("xa_dxf_w", "DXFW__", cbuf);
      goto L_fertig;


    } else if(AP_mod_iftyp == Mtyp_SVG) {
      // printf(" Export SVG |%s|\n",AP_mod_fnam);
      OS_dll_do ("xa_svg_w", "SVG_w__", cbuf);
      goto L_fertig;


    } else if(AP_mod_iftyp == Mtyp_Iges) {
      // printf(" Export IGS |%s|\n",AP_mod_fnam);
      AP_ExportIges__ (cbuf);
      goto L_fertig;


    } else if(AP_mod_iftyp == Mtyp_Step) {
      OS_dll_do ("xa_stp_w", "STP_w__", cbuf);
      goto L_fertig;


    } else if(AP_mod_iftyp == Mtyp_JPG) {
      sprintf(s1, "%swin.bmp",OS_get_tmp_dir());
      // create BMP-file of active OpenGL-window
      bmp_save__ (s1);
      // create JPG-file from BMP-file
      OS_jpg_bmp (cbuf, s1);
      TX_Print ("- %s exported ..",cbuf);
      goto L_fertig;


    } else if((AP_mod_iftyp == Mtyp_WRL)  ||
              (AP_mod_iftyp == Mtyp_STL)  ||
              (AP_mod_iftyp == Mtyp_OBJ)  ||
              (AP_mod_iftyp == Mtyp_TESS)) {
      goto L_mock;

    } else {
      TX_Print("**** Error filetyp %s %d",AP_mod_ftyp,AP_mod_iftyp);
      return -1;
    }




  //================================================================
  // save Mockup         // see UI_saveMockCB
  L_mock:
  // tess. exportieren ..
  // sprintf(memspc011, "%s%c%s",dirNam,fnam_del,fnam);
  i1 = TSU_exp__ (AP_mod_ftyp, cbuf);
  if(i1 != 0) DB_del_Mod__ ();  // del all subModels nach error

  ED_Reset ();       // ED_lnr_act resetten
  ED_work_END (0);   // restore display ..
  // TX_Print("save tesselated as %s in %s",AP_mod_fnam,AP_mod_dir);




  //================================================================
  // Defaults rausschreiben
  L_fertig:

  // restore AP_mod_fnam  - ohne Filetyp
  strcpy(AP_mod_fnam, mnam);

  AP_defaults_write ();

  // Title oben auf den Mainwinrahmen
  UI_AP (UI_FuncSet, UID_Main_title, NULL);

  // MAN: reset focus                     // 2013-04-12
  // sonst kein Cursor im Editor mehr ..
  if(UI_InpMode == UI_MODE_MAN) {
    GUI_edi_Focus (&winED);
  }


  // model unmodified ..
  // AP_mdl_modified_reset ();

  // make a copy of Model for ck-modified
  Mod_sav_ck (0);


  UI_CursorWait (1);    // reset cursor

/*
  // vom Window ins Memory kopieren
  ED_unload__ ();

  // // DYNAMIC_AREA (im Memory) zufuegen
  // DL_save_DYNAMIC_AREA ();

  // Memory in Datei rausschreiben
  UTF_wri_file (AP_mod_fnam);

  // // Title oben auf den Mainwinrahmen
  // UI_AP (UI_FuncSet, UID_Main_title, NULL);

  // DYNAMIC_AREA (im Memory) wieder wegloeschen
  UTF_del_rest(":DYNAMIC_AREA");
*/

  return 0;

}


//================================================================
  int UI_PRI__ (int mode) {
//================================================================
//
/// Input:
///   mode       FUNC_EXEC   = load & start dll
///              FUNC_UNLOAD = unload dll


  static void  *dll1 = NULL; // pointer to loaded dll "xa_print__.so"


  printf("UI_PRI__ %d\n",mode);


  //----------------------------------------------------------------
  if(mode == FUNC_EXEC) {   // load & start
    // TESTBLOCK
    if(dll1 != NULL) OS_dll__ (&dll1,  FUNC_RECOMPILE, "xa_print__");  // rebuild
    // END TESTBLOCK

    // load dll
    if(dll1 == NULL) OS_dll__ (&dll1,  FUNC_LOAD_only, "xa_print__");

    // connect func
    OS_dll__ (&dll1,  FUNC_CONNECT, "PRI__");

    // start func
    OS_dll__ (&dll1,  FUNC_EXEC, NULL);


  //----------------------------------------------------------------
  } else if(mode == FUNC_UNLOAD) {   // unload
    if(dll1 != NULL) {
      // test if window is active
      if(GUI_Win_exist ("Export/Print")) {
        printf("**** UI_PRI__ E001\n");
        return -1;
      }
      OS_dll__ (&dll1,  FUNC_UNLOAD, NULL); // unload
    }
  }


  return 0;

}


//=====================================================================
  double UI_vwz__ (int mode, long ind) {
//=====================================================================
// mode = 0: Punktekette erzeugen ..
// mode = 1: Z-Wert der Selektion retournieren


#define VWZ_PTNR_ 1000
#define VWZ_PTNR2 500         // die halbe Punkteanzahl; VWZ_PTNR_ / 2


  int    i1;
  double d1, scl, zVal, incrZ;
  Point  pt1, pt2, pt3;
  // Vector vc1;


  static long   startInd;
  static double startZ;
  static Point  ptStart;
  static Vector vcInc;




  // printf("UI_vwz__ mode=%d ind=%ld\n",mode,ind);




  //===========================================================
  // mode = 0: Genau in Bildmitte eine Kette temporaerer Punkte hinmalen
  if(mode != 0) goto L_1;


  // DL-startindex (fuers delete)
  startInd = GL_Get_DLind();
  // printf(" startInd=%ld\n",startInd);



  // aktueller scale ?
  scl = GL_get_Scale();
  // incrZ = 30. / scl;
  // incrZ = 50. / scl;
  // hier auch APT_ModSiz beruecksichten !
  // incrZ = 10.; // in diesem Fall Punktabst immer 10 mm !!
  incrZ = 10. / scl;   //
  // printf(" scl=%f incrZ=%f\n",scl,incrZ);



  // den Bildschirmmittelpunkt in Userkoordinaten feststellen
  pt1 = GL_GetCen ();


  pt2 = pt1;

  pt2.z += incrZ;
  pt3 = GL_GetViewPt (&pt2);

  // den Differenzvektor pt3 -> pt2
  UT3D_vc_2pt (&vcInc, &pt3, &pt2);

  ptStart = pt1;

  pt2 = pt1;


  for (i1=1; i1<VWZ_PTNR2; ++i1) {
    d1 = pow(i1, 2.);
    pt1.x = ptStart.x + (vcInc.dx * d1);
    pt1.y = ptStart.y + (vcInc.dy * d1);
    pt1.z = ptStart.z + (vcInc.dz * d1);
    // printf(" oben[%d]=%f %f %f\n",i1,pt1.x,pt1.y,pt1.z);
    APT_disp_SymB (SYM_STAR_S, 2, &pt1);
  }

  for (i1=1; i1<VWZ_PTNR2; ++i1) {
    d1 = pow(i1, 2.);
    pt1.x = ptStart.x - (vcInc.dx * d1);
    pt1.y = ptStart.y - (vcInc.dy * d1);
    pt1.z = ptStart.z - (vcInc.dz * d1);
    // printf(" unte[%d]=%f %f %f\n",i1,pt1.x,pt1.y,pt1.z);
    APT_disp_SymB (SYM_STAR_S, 2, &pt1);
  }

  DL_Redraw ();



  goto L_fertig;





  //===========================================================
  // mode = 1: Z-Wert der Selektion retournieren
  L_1:
  if(mode != 1) goto L_2;


  // der wievielte Punkt wurde selektiert ?
  i1 = ind - startInd + 1;

  // printf(" sel %d\n",i1);

  // zVal = ptStart.z + (i1 * vcInc.dz);
  // printf(" startZ=%f ptNr=%d\n",ptStart.z,i1);

  if(i1 < VWZ_PTNR2) {
    d1 = pow(i1, 2.);
    pt1.x = ptStart.x + (vcInc.dx * d1);
    pt1.y = ptStart.y + (vcInc.dy * d1);
    pt1.z = ptStart.z + (vcInc.dz * d1);

  } else {
    i1 = i1 - VWZ_PTNR2 + 1;
    d1 = pow(i1, 2.);
    pt1.x = ptStart.x - (vcInc.dx * d1);
    pt1.y = ptStart.y - (vcInc.dy * d1);
    pt1.z = ptStart.z - (vcInc.dz * d1);
  }

  zVal = pt1.z;

  // printf(" newCen=%f,%f,%f\n",pt1.x,pt1.y,pt1.z);
  GL_Set_Cen (&pt1);

  // Punkte noch loeschen
  mode = 2;


  //===========================================================
  // mode = 2: alle Bitmap-symbole wegloeschen
  L_2:
  if(mode != 2) goto L_3;
  if(startInd >= 0) {
    // printf(" ... delete von %ld\n",startInd);
    GL_Delete (startInd);
    startInd = -1;
  }
  goto L_fertig;



  //===========================================================
  // mode = 3: den DL-StartIndex retournieren
  L_3:
  // if(mode != 3) goto L_3;
  return startInd;




  //===========================================================
  L_fertig:

  // UI_InpVWZ = OFF;

  return zVal;

}


/* ===================================================================== */
  int UI_vwzCB (MemObj *mo, void **data) {
//=====================================================================


  // ausschalten
  if(!GUI_ckbutt_get(&ckb_vwz)) {

    UI_InpVWZ = OFF;

    // remove symbols
    UI_vwz__ (2, 0L);
    DL_Redraw ();
    return 0;
  }


  UI_InpVWZ = ON;


  // Genau in Bildmitte eine Kette temporaerer Punkte hinmalen
  UI_vwz__ (0, 0L);


  MSG_pri_0 ("RotCen");
  // TX_Print("neue Drehebene definieren:");
  // TX_Print(" - Vertex (Endpunkt einer Linie selektieren oder");
  // TX_Print(" - Ansicht drehen bis Punktekette sichtbar - dann Punkt waehlen");

  // TX_Print("define new rotation-center:");
  // TX_Print("- select vertex or -");
  // TX_Print("- rotate display (Ctrl-drag_mouse) and select red star");


  return 0;

}


//=====================================================================
  int UI_loadImg_CB (char *fnam,char *dirNam) {
//=====================================================================
//   fnam   <filename>.<filetyp>


  long ind;
  char cbuf[256]; //, dirSym[64];


  if(fnam == NULL) return 0;

  // printf("UI_loadImg_CB |%s|%s|\n",fnam,dirNam);


  // set AP_mod_dir & AP_mod_sym from dirNam
  AP_set_dir_open (dirNam);
  // AP_set_dir_open (AP_mod_dir);

  // create line N1=IMG P(0 0 0) "BMP/Andi.bmp"
  ind = DB_dbo_get_free (Typ_GTXT); // N
  ++ind;
  sprintf(cbuf,"N%ld=IMG P(0 0 0) \"%s/%s\"",ind,AP_mod_sym,fnam),

  ED_add_Line (cbuf);

  return 0;

}


//=====================================================================
  int UI_loadMock_CB (char *fnam, char *dirNam) {
//=====================================================================
// fnam ist FileName + Filetyp.
// create source-line "M# = "symDir/fnam" P(0 0 0)"

  long ind;
  char cbuf[256]; //, dirSym[64];


  if(fnam == NULL) return 0;

  // printf("UI_loadMock_CB |%s|%s|\n",fnam,dirNam);


  // set AP_mod_dir & AP_mod_sym from dirNam
  AP_set_dir_open (dirNam);
  // AP_set_dir_open (AP_mod_dir);

  // get next free Model-index
  ind = DB_dbo_get_free (Typ_Model); // M
  ++ind;

  // create line M# = "symDir/fnam" P(0 0 0)
  // sprintf(cbuf,"M%ld=MOCK \"%s/%s\" P(0 0 0)",ind,AP_mod_sym,fnam),
  sprintf(cbuf,"M%ld=\"%s/%s\" P(0 0 0)",ind,AP_mod_sym,fnam),
    // printf(" Mock_CB |%s|\n",cbuf);

  // add to model and work
  ED_add_Line (cbuf);

  return 0;

}


//=====================================================================
  int UI_loadBmp_CB (char *fnam,char *dirNam) {
//=====================================================================

  long ind;
  char cbuf[256], dirSym[64];


  if(fnam == NULL) return 0;

  printf("UI_loadBmp_CB |%s|%s|\n",fnam,dirNam);


  strcpy(AP_mod_dir, dirNam);
  // strcpy(AP_mod_fnam, fnam);

  // get symbol from path
  Mod_sym_get2 (dirSym, dirNam, 0);

  // create line M#=IMG "fnam" P(0 0 0)
  ind = DB_dbo_get_free (Typ_GTXT); // M
  ++ind;
  sprintf(cbuf,"N%ld=IMG P(0 0 0) \"%s/%s\"",ind,dirSym,fnam),

  ED_add_Line (cbuf);

  return 0;

}

/*
//=====================================================================
  int UI_del_CB (GtkWidget *parent, void *data) {
//=====================================================================
//
  int idat;

  printf("UI_del_CB %d\n",INT_PTR(data));


  if(INT_PTR(data)== UI_FuncOK) {
    // del. file ...
    printf(" delete |%s|\n",memspc011);
    TX_Print("delete %s",memspc011);
    OS_file_delete (memspc011);
  }

  return 0;

}
*/

/*
//=====================================================================
  int UI_delMdl (char *fnam,char *dirNam) {
//=====================================================================

  char cbuf[256];

  if(fnam == NULL) return 0;

  printf("UI_delMdl |%s|%s|\n",fnam,dirNam);

  sprintf(memspc011, "%s%s", dirNam,fnam);          // %c fnam_del

  sprintf(cbuf, "delete file   %s   in Dir. %s ?",fnam,dirNam);

  GUI_DialogYN (cbuf, UI_del_CB);

  return 0;

}
*/

/*
//================================================================
  int UI_ren_CB (GtkWidget *parent, void *data) {
//================================================================

  char cbuf[256];

  if(data == NULL) return 0;           // Cancel ?

  // copy symbol
  strcpy(cbuf, (char*)data);

  printf("UI_ren_CB |%s|\n",(char*)data);

  sprintf(cbuf, "%s%s", memspc011,(char*)data);
  printf("     old |%s|\n",&memspc011[512]);
  printf("     new |%s|\n",cbuf);

  TX_Print("rename %s %s",&memspc011[512],cbuf);

  rename (&memspc011[512], cbuf);   // (old, new)

  // kill window
  GUI_GetText_CB (NULL, (void*)UI_FuncKill);

  return 0;

}
*/
/*
//=====================================================================
  int UI_cpyMdl3 (GtkWidget *parent, void *data) {
//=====================================================================
// oldName = memspc011
// newName = memspc012

  int    idat;
  char   cbuf[256];

  printf("UI_cpyMdl3 %d\n",INT_PTR(data));

  idat = INT_PTR(data);

  if(INT_PTR(data )== UI_FuncOK) {
    // extract filname from old
    UTX_fnam_s (cbuf, memspc011);
    TX_Print ("copy file  %s --> %s",cbuf, memspc012);  // memspc011
    printf(" oldName=|%s|\n",memspc011);
    printf(" newName=|%s|\n",memspc012);
    OS_file_copy (memspc011, memspc012);   // (old, new)
  }

  return 0;

}
*/
/*
//=====================================================================
  int UI_cpyMdl2 (char *fnam,char *dirNam) {
//=====================================================================
// oldName = memspc011
// newName = memspc012

  char  ftyp[16], cbuf[256];


  printf("UI_cpyMdl2\n");

  if(fnam == NULL) return 0;

  printf("UI_cpyMdl2 |%s|%s|\n",fnam,dirNam);

  // extract Filetyp from oldName and add to newName
  UTX_ftyp_s (ftyp, memspc011, 0);

  sprintf(memspc012,"%s%s.%s",dirNam,fnam,ftyp);
  printf(" newName=|%s|\n",memspc012);

  if(OS_checkFilExist(memspc012, 1) == 1) {
    // sprintf(cbuf," overwrite file %s.%s ?  ",fnam,ftyp);
    MSG_get_1 (cbuf, 256, "OVER_fil", "%s", memspc012);
    GUI_DialogYN (cbuf, UI_cpyMdl3);
    return 0;
  }

  UI_cpyMdl3 (NULL, (void*)UI_FuncOK);

  return 0;

}
*/
/*
//=====================================================================
  // int UI_cpyMdl1 (char *fnam,char *dirNam) {
  int UI_cpyMdl1 (GtkWidget *parent, void *data) {
//=====================================================================

  char  cbuf1[256], cbuf2[256], cbuf3[64], ftyp[16];


  printf("UI_cpyMdl1\n");

  // if(fnam == NULL) {
    // return 0;
  // }

  // printf("UI_cpyMdl1 |%s|%s|\n",fnam,dirNam);

  GUI_File_selGet (memspc011, parent);

  // extract filname
  UTX_fnam_s (cbuf2, memspc011);

  // extract Filetyp from oldName and add to newName
  UTX_ftyp_s (ftyp, cbuf2, 1);



  // GUI_List3_CBl (NULL, (void*)UI_FuncKill);
  // GUI_update__();
  // sleep (2);

  AP_get_fnam_symDir (cbuf1);   // get filename of dir.lst
  // sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
  sprintf(cbuf3,"copy %s File to",ftyp);
  GUI_save__ (cbuf3,            // titletext
          AP_mod_dir,            // path
          cbuf1,                  // directoryList
          cbuf2,                  // defaultModelname
          (void*)UI_cpyMdl2);

  return 0;

}
*/
/*
//=====================================================================
  int UI_renMdl (char *fnam,char *dirNam) {
//=====================================================================

  if(fnam == NULL) return 0;

  printf("UI_renMdl |%s|%s|\n",fnam,dirNam);

  strcpy(memspc011, dirNam);
  sprintf(&memspc011[512], "%s%s", dirNam,fnam);

  GUI_GetText("new name for file: ",
              fnam, -300, UI_ren_CB);    // defaultname = old name

  return 0;

}
*/

//================================================================
  int UI_opePtab (char *fnam,char *dirNam) {
//================================================================

  if(fnam == NULL) return 0;

  printf("UI_opePtab |%s|%s|\n",fnam,dirNam);

  return 0;

}


//=====================================================================
  int UI_open_add (char *fnam,char *dirNam) {
//=====================================================================
// UNUSED

  int  irc;
  char cbuf[256], actNam[256];


  if(fnam == NULL) return 0;

  printf("UI_open_add |%s|%s|\n",fnam,dirNam);


  // Open Model setzt AP_mod_dir und AP_mod_dir;
  // strcpy(AP_mod_dir, dirNam);
  AP_set_dir_open (dirNam);
  // AP_set_dir_save (dirNam);

  strcpy(actNam, AP_mod_fnam);  // save act Modelnam
  strcpy(AP_mod_fnam, fnam);

  // load
  irc = AP_Mod_load__ (1);

  strcpy(AP_mod_fnam, actNam);  // restore act Modelnam

  return irc;

}


//================================================================
  int UI_disp_var1 (double *val1) {
//================================================================
/// temporary display  a value

  int    att;
  long   dli;
  Point  pt1;
  Vector *vc1;


  // printf("UI_disp_var1 %lf\n",*val1);

  dli = DLI_TMP;  // als temp. obj anlegen ..

  att = 9;        // 12=red dashed  7 = sw;

  // if(pos) {
    // pt1 = *pos;
  // } else {
    pt1 = GL_GetCen();               // Mittelpunkt Bildschirm
  // }

  vc1 = (Vector*)&UT3D_VECTOR_X;

  // GL_DrawSymV3 (&dli, SYM_LENGTH, att, &pt1, vc1, 20.);
  GL_DrawSymVTL (&dli, SYM_LENGTH, att, &pt1, val1);


  return 0;

}


//================================================================
  int UI_disp_dbo (int typ, long dbi, int att) {
//================================================================
/// \code
/// UI_disp_dbo           display temporary db-obj
///   att       see GR_Draw_obj
/// \endcode

  long    dli;


  // printf("UI_disp_dbo %d %ld\n",typ,dbi);


  dli = DLI_TMP;    // dli for temp.Objects

  return GR_Draw_dbo (&dli, typ, dbi, att, 0);

}


//================================================================
  int UI_disp_Pos (int typ, void *data) {
//================================================================
/// \code
/// temporary display position (with little red circle)
/// typ:  Typ_Index||Typ_PT|Typ_Txt
///   data   if(typ==Typ_PT) *Point
///          if(typ==Typ_Index) *long   (DB-Index)
///          if(typ==Typ_Txt) ..
/// DL_Redraw()  necessary.
/// see also IE_cad_Inp_disp 
/// \endcode


  long   dli;
  Point  pt1;

  // printf("UI_disp_Pos %d\n",typ);


  if(typ == Typ_Index) {
    pt1 = DB_GetPoint(LONG_PTR(data));


  } else if(typ == Typ_Txt) {
    APT_obj_expr (&pt1, Typ_PT, (char*)data);


  } else if(typ == Typ_PT) {
    pt1 = *((Point*)data);


  } else {
    TX_Error("UI_disp_Pos E001 %d",typ);
    return -1;
  }

  // Kreiserl an der VertexPosition
  dli = DLI_POS_TMP;  // overwrite
  GL_DrawSymB (&dli, Typ_Att_hili1, SYM_CIR_S, &pt1);  // rot


  L_fertig:
  // DL_Redraw ();

  return 0;

}


//================================================================
  int UI_disp_activ (int mode, long dbi, Activity *ac1) {
//================================================================
/// display Activity as Text "I#"
// Input:
//   mode     0=permanent, 1=temporary;
     
  int     irc, iCol;
  long    dli;
  char    s1[16];
  Point   pt1; 
  ObjGX   ox1;
    
    
  // printf("UI_disp_activ mode=%d dbi=%ld\n",mode,dbi);
  // printf(" typ=%d dbi=%ld\n",ac1->typ,ac1->ind);
  // printf(" WC_modact_ind=%d\n",WC_modact_ind);

    
  // skip display in subModels
  if(WC_modact_ind >= 0) {           // 0-n = sind Submodel; -1=main
    return 0;
  }



  // get typical point for activity-object -> pt1
  OGX_SET_INDEX (&ox1, ac1->typ, ac1->ind);
  // irc = UTO_pt_ox (&pt1, &ox1);
  irc = UT3D_ptvcpar1_std_obj (&pt1, NULL, NULL, Ptyp_0, Typ_ObjGX, &ox1);
  if(irc < 0) {
    TX_Print ("UI_disp_activ E001 %d %d",ac1->typ,ac1->ind);
    return -1;
  }
    // UT3D_stru_dump (Typ_PT, &pt1, " activ-pt1:");


  if(mode) {
    strcpy(s1, "I");
    dli = DLI_TMP; //-2L;
    iCol = Typ_Att_hili1;

  } else {
    sprintf(s1, "I%ld",dbi);
    dli = DL_StoreObj (Typ_Activ, dbi, 0);
    iCol = Typ_Att_PT;
  }



  if(UI_InpMode == UI_MODE_VWR) {
    DL_hide__ (dli, 1);  //unvis
  }

  GR_DrawTxtA (&dli, iCol, &pt1, s1);


  return 0;

}


//================================================================
  int UI_disp_joint (int mode, long indJnt, Memspc *spcObj) {
//================================================================
// Input:
//   mode     0=permanent, 1=temporary;

  int     iCol;
  long    dli;
  char    s1[16];
  ObjGX   *jnt;
  Point   *pt1;


  jnt = UME_get_start (spcObj);

  // printf("UI_disp_joint mode=%d indJnt=%d\n",mode,indJnt);
  // UTO_dump__ (jnt, "JNT_exp__");
  // UTO_dump_s_ (jnt, "APT_decode_Joint");
  // printf(" WC_modact_ind=%d\n",WC_modact_ind);


  // skip display in subModels
  if(WC_modact_ind >= 0) {           // 0-n = sind Submodel; -1=main
    return 0;
  }


  // get point out of jnt
    // printf(" _disp_joint typ=%d form=%d\n",jnt->typ,jnt->form);

  if(jnt->typ == Typ_PT) {
    pt1 = ((Point*)jnt->data);


  } else if(jnt->form == Typ_GTXT) {
    pt1 = (&((GText*)jnt->data)->pt);


  } else {
    TX_Print ("UI_disp_joint E001 %d %d",jnt->typ,jnt->form);
    return -1;
  }
    // UT3D_stru_dump (Typ_PT, pt1, " joint-pt1:");


  if(mode) {
    strcpy(s1, "J");
    dli = -2L;
    iCol = Typ_Att_hili1;

  } else {
    sprintf(s1, "J%ld",indJnt);
    dli = DL_StoreObj (Typ_Joint, indJnt, 0);
    iCol = Typ_Att_PT;
  }


  if(UI_InpMode == UI_MODE_VWR) {
    DL_hide__ (dli, 1);      // unvis
  } 

  GR_DrawTxtA (&dli, iCol, pt1, s1);

  return 0;

}


//=====================================================================
  int UI_disp_vec1 (int typ, void *data, Point *pos) {
//=====================================================================
/// \code
/// UI_disp_vec1        create vector as a temporary outputObject. True length.
/// Input:
///   typ    Typ_Index|Typ_VC|Typ_Txt
///   data   if(typ==Typ_VC)    Vector*
///          if(typ==Typ_Index) long*   (DB-Index)
///          if(typ==Typ_Txt)   char*   (eg "0 0 1")
///   pos    position of vector or NULL
/// True length:   see also IE_cad_Inp_disp_vc GL_DrawVec GR_Disp_vc
/// unified lengt: see GL_DrawSymV3 GR_Disp_vc GL_Disp_vc GR_Disp_vc
/// \endcode


  int    att, mode;
  long   dli;
  double vl;
  Point  pt1;
  Vector vc1;


  // printf("UI_disp_vec1 %d\n",typ);
  // if(pos) UT3D_stru_dump (Typ_PT, &pt1, "  pos:");
  // else    printf("  pos=NULL\n");


  // if(vecID >= 0) {
    // GL_Del0 (vecID);
    // vecID = -1;
  // }

  // if(vi < 1) goto L_fertig;          // nur loeschen


  if(pos) {
    pt1 = *pos;
  } else {
    pt1 = GL_GetCen();               // Mittelpunkt Bildschirm
  }
    // UT3D_stru_dump (Typ_PT, &pt1, "cen in UI_disp_vec1");


  if(typ == Typ_Index) {
    // vc1 = DB_GetVector(*((long*)data));
    vc1 = DB_GetVector(LONG_PTR(data));
      // UT3D_stru_dump (Typ_VC, &vc1, " vc1 UI_disp_vec1");
    if(&vc1 == &UT3D_VECTOR_Z) return -1;


  } else if(typ == Typ_Txt) {
    // printf(" dispVC |%s|\n",(char*)data);
    APT_obj_expr (&vc1, Typ_VC, (char*)data);


  } else if(typ == Typ_VC) {
    vc1 = *((Vector*)data);


  } else {
    TX_Error("UI_disp_vec1 E001 %d",typ);
    return -1;
  }


  //----------------------------------------------------------------
  // display vc1 at position pt1
    // UT3D_stru_dump (Typ_VC, &vc1, "vc1 in UI_disp_vec1");

  dli = DLI_TMP;  // als temp. obj anlegen ..

  // GL_temp_del_all (); // alle temp. obj loeschen ..
  GL_temp_del_1 (dli);


  vl = UT3D_len_vc (&vc1);
    // printf("  vl=%f\n",vl);


  att = 9;   // 12=red dashed  7 = sw;

  mode = UTP_comp2db(vl, 1., 0.8);
  mode = ICHG01(mode);   // 0=normalized
    // printf(" mode=%d\n",mode);


  // mode: 0=exact; 1=normalized
  GR_Draw_vc (&dli, &vc1, &pt1, att, mode);

/*
  if(UTP_comp2db(vl, 1., 0.1)) {    // check if vl has length 1
    // att 7 = sw; Laenge 1
    // APT_disp_SymV3 (SYM_ARROW, 7, &pt1, &vc1, 10.);
    // APT_disp_Vec (7, (long)vi, &pt1, &vc1);
    // GR_Disp_vc (&vc1, &pt1, 7, 0);
    // GL_DrawSymV3 (&dli, SYM_ARROW, 7, &pt1, &vc1, 10.);
    GL_DrawSymV3 (&dli, SYM_ARROW, att, &pt1, &vc1, 20.);

  } else {
    // APT_disp_Vec (2, (long)vi, &pt1, &vc1);
    // GR_Disp_vc (&vc1, &pt1, 2, 1);
    GL_DrawVec (&dli, att, &pt1, &vc1);
  }
*/

  // vecID = GL_GetActInd();


  L_fertig:
  DL_Redraw ();

  return 0;

}

/* UNUSED
//=====================================================================
  int UI_disp_vec2 (long ind) {
//=====================================================================
// temp disp of vector # 0 (used for display TRA)

  ObjGX   *ox1;


  printf("UI_disp_vec2 %ld\n",ind);

  ox1 = DB_GetTra (ind);
    // UTO_dump__ (ox1, "T[%d]:",ind);


  return UI_disp_tra (ox1);

}
*/

//================================================================
  int UI_disp_pln_oid (char *plID) {
//================================================================
// hilite plane from objID
// plID   NULL = clear hilite
 
  static long   dli = -1;

  int    typ;
  long   dbi;
  Point  pt1;
  Plane  *pln1;


  printf("UI_disp_pln_oid |%s| %ld\n",plID,dli);


  if(dli >= 0)  DL_hili_off (dli);


  // plID   NULL = clear hilite
  if(!plID) {
    if(dli < -1L) GL_temp_del_1 (dli);
    dli = -1;
    return 0;
  }


  APED_dbo_oid (&typ, &dbi, plID);
    printf("  typ=%d dbi=%ld\n",typ,dbi);


  if(dbi < 0) {   // RX RY RZ
    dli = GL_GetInd_last_temp ();
    pt1 = GL_GetCen();   // ScreenCenter in UserCoords
    goto L_disp;
  }


  if(dli < -1L) GL_temp_del_1 (dli);

  dli = DL_find_obj (Typ_PLN, dbi, -1L);
  if(dli >= 0) {
    DL_hili_on (dli);
    goto L_exit;
  } 


  L_disp:
    pln1 = DB_get_PLN (dbi);
    if(dbi > 0) pt1 = pln1->po;
    GL_DrawSymV3 (&dli, SYM_SQUARE, 9, &pt1, &pln1->vz, 1.);


  L_exit:
  DL_Redraw ();

  return 0;

}


//================================================================
  int UI_disp_tra (ObjGX *tra) {
//================================================================

  long    dli;
  double  a1;
  Point   pt1, pt2, pt3;
  Vector  vx, vz;


  // UTO_dump__ (tra, "UI_disp_tra ");


  // GL_temp_del_all (); // alle temp. obj loeschen ..

  if(tra->form == Typ_VC) {

    pt1 = GL_GetCen();
    UI_disp_vec1 (Typ_VC, tra->data, &pt1);
    return 0;


  } else if(tra->form != Typ_TraRot) {
    TX_Error("UI_disp_tra E001 %d",tra->form);
    return -1;
  }


  //----------------------------------------------------------------
  // disp TraRot
  dli = DLI_TMP;  // temp obj
  GL_Draw_tra (&dli, 12, (TraRot*)tra->data);

/*
  // where to display:  get screenCenter
  pt1 = GL_GetCen();
  // prj pt1 -> Achse pt-vc1
  UT3D_pt_projptptvc (&pt3, &d2, &pt1, &pt2, &vc1);
  // dli=DL_StoreObj (Typ_LN, 0L, 2);
  //  wenn V0=leer: nur Vektor; else Angle in V0. see GR_DrawCirc
  GL_DrawCirSc (&dli, 12, &pt2, &vc1, d1);
*/
  return 0;

}

/*
//=====================================================================
  int UI_hili_actLn () {
//=====================================================================
// das Obj in der aktuellen Zeile hiliten


  int    i1, i2, lNr, ilen;
  long   l1, l2, dlInd;
  double d1;
  char   *cpos;


  printf("hhhhhhh UI_hili_actLn\n");


  // den Zeilentext der aktuellen Zeile holen ..
  lNr = ED_Get_LineNr ();
  cpos = UTF_GetLinNr (txbuf, &ilen, lNr);
  if(ilen < 0) {
    DL_hili_on (-1L);        // loeschen der Hili-Tabelle
    DL_Redraw  ();
    return -1; // am EOF
  }
  // printf(" lNr=%d, ln=|%s| len=%d\n",lNr,txbuf,ilen);

  // lNr global merken (erfordel f Esc - AP_obj_del1)
  // AP_set_lNr (lNr);


  // die ZeilenNr korrigieren
  UI_AP (UI_FuncSet, UID_ouf_lNr, (void*)lNr);


  // aktuelle Zeile selektiert darstellen
  i1 = UTF_offset_ (cpos);
  i2 = i1 + ilen;
  UI_Ed_sel (i1,i2);



  // das gefundene Obj hiliten
  AP_hili_obj (txbuf);


  // // typ und DB-index aus Header ableiten
  // APED_dbo_oid (&i1, &i2, txbuf);
  // // printf("typ=%d ind=%ld\n",i1,i2);
  // if(i1 == Typ_VC) {
    // l2 = i2;
    // UI_disp_vec1 (Typ_VC, &l2);
    // return 0;
  // } else if(i1 == Typ_VAR) {
    // d1 = DB_GetVar ((long)i2);
    // TX_Print ("VAR. Nr. %d = %f",i2,d1);
    // return 0;
  // }
  // // find obj in DL
  // l2 = i2; // apt-ind meist long !
  // dlInd = DL_find_obj (i1, l2);
  // if(dlInd < 0) {
    // TX_Print("Obj. aus Zeile nicht %d nicht gefunden",lNr);
  // } else {
    // DL_hili_on (dlInd);      // hilite obj
  // }
  // DL_Redraw  ();


  printf("ex UI_hili_actLn\n");
  return 0;
}
*/


//================================================================
  int UI_ckb_meas (int mode) {
//================================================================
// checkbox "Measure"

  GUI_ckbutt_set (&ckb_meas, mode);

  return 0;

}


//=====================================================================
  int UI_cb_search (int mode) {
//=====================================================================
// switch checkbox "Search/Name" on | off
// mode  0 = switch OFF & disactivate checkbox
//       1 = switch ON.
//       2 = deselect checkbox
//       3 = clear list
//       4 = query; 0=off, 1=on
// ckb_such

  int    i1;


  // printf("UI_cb_search %d\n",mode);


  //----------------------------------------------------------------
  if(mode == 1) {    // switch on
    GUI_set_enable (&ckb_such, TRUE);


  //----------------------------------------------------------------
  } else if(mode == 0){  // FALSE, switch off
    // check if active
    i1 = GUI_ckbutt_get (&ckb_such); // 0=not sel, 1=selected.
    if(i1 != 0) {
      // // UI_mcl__ (NULL, PTR_INT(-1));        // shutdown List
      // UI_mcl__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncExit)); // shutdownList
      GUI_set_enable (&ckb_such, FALSE);
    }


  //----------------------------------------------------------------
  } else if(mode == 3){  // clear list
    // check if active
    i1 = GUI_ckbutt_get (&ckb_such); // 0=not sel, 1=selected.
    if(i1 != 0) {
      // UI_mcl__ (NULL, PTR_INT(-1));        // shutdown List
      // UI_mcl__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncExit));
      UI_mcl_CB1 (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncUCB12));
    }


  //----------------------------------------------------------------
  } else if(mode == 4){  // query
    return GUI_ckbutt_get (&ckb_such); // 0=not sel, 1=selected.



  //----------------------------------------------------------------
  } else if(mode == 2){  // deselect checkbox (implicit by UI_mcl__)
    GUI_ckbutt_set (&ckb_such, FALSE);  // deselect checkbox
    // UI_mcl__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncExit));

  }

  return 0;

}


/*
//=====================================================================
  int UI_butSM (GtkWidget *parent, void *data) {
//=====================================================================
// Search/Measure
// aktivieren mit
// UI_AP (UI_FuncSet, UID_ckb_search, (void*)TRUE);


  int i1;


  printf("UI_butSM XXXXXXXXXXXXXXXXX \n");
  printf("   UI_EditMode=%d\n",UI_EditMode);



  //============== activate ================================
  if(GTK_TOGGLE_BUTTON (parent)->active) {

      // MSG_pri_0 ("MD_On");
      // TX_Print("select object / key ObjID Enter / Exit: Ctl right mouseb.");

      printf("UI_butSM: UI_InpSM => ON\n");

      UI_InpSM = ON;

      // akt. Menuebar komplett abblenden
      UI_mBars_off ();


      // display alle elemente, cursor auf letzte zeile:
      // ED_work_aux (99999);
      if(UI_InpMode == UI_MODE_CAD) { // mode CAD:
        // ??
        IE_cad_INS (NULL, (void*)"Exit");

      } else {
        if(UI_InpMode == UI_MODE_MAN) {
          // set EditWindow noEdit
          GUI_Ed_editable (&winED, 0);
        }
        ED_work_CurSet (999999);
      }


      if(AP_src == AP_SRC_EDI) {        // mode MAN:
        // vom Window ins Memory kopieren
        ED_unload__ ();
      }

      if(UI_InpMode == UI_MODE_CAD) { // mode CAD:
         // kill GruppenEditorWindow, if activ
         IE_ed1__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncKill));
      }


      // display Window Modify/Delete
      UI_WinSM (NULL, (void*)UI_FuncInit);



      // das Obj in der aktuellen Zeile hiliten
      // UI_hili_actLn ();

      DL_hili_off (-1L); DL_Redraw (); // unhilite alle Objekte


      UI_EditMode = UI_EdMode_Add;

      return 0;




  //============== disactivate =============================
  } else {

      // printf("UI_butSM: UI_InpSM => OFF; EditMode=%d\n",UI_EditMode);

      UI_InpSM = OFF;

      // kill S/M-Panel; Aufruf UI_WinSM/Kill waere ein Loop !
      // UI_WinSM (NULL, (void*)UI_FuncCancel);  makes  Gtk-CRITICAL **: gtk_style_detach: assertion .. 
      UI_WinSM (NULL, (void*)UI_FuncKill);


      // akt. Menubar ausblenden
      UI_mBars_off ();

      // remove vectors
      // UI_disp_vec1 (-1L);


      // wenn CAD aktiv:
        // printf(" UI_InpMode=%d\n",UI_InpMode);

      if(UI_InpMode == UI_MODE_CAD) {

        if(UI_EditMode == UI_EdMode_Add) {
          // // Zeilennummer neu holen
          // IE_cad_init_add ();

        } else { // UI_EdMode_Modify
          GUI_set_enable (ckb_mdel, FALSE);  // make MOD/DEL unselectable
          // weiter wie wenn aktive Zeile vom user selektiert worden waere.
          IE_activate();
          return 0;
        }



      } else if(UI_InpMode == UI_MODE_MAN) {

        // set EditWindow Editable
        GUI_Ed_editable (&winED, 1);

        // // vom Memory -> Editor
        // ED_load__ ();
          printf(" UI_EditMode=%d\n",UI_EditMode);

        if(UI_EditMode == UI_EdMode_Add) {
          // Manuell - SM aus: den Textcursor ans Dateiende
          UI_AP (UI_FuncSet, UID_Edit_Cpos, NULL);

        } else { // UI_EdMode_Modify MAN
          // // unselect, set Cursor to end of Line (nicht ans Fileende !)
          // GUI_Ed_setCpos (&winED, AP_ED_cPos); // UI_Ed_sel

          // die ZeilenNr korrigieren
          i1 = ED_get_lnr_act ();    // get AP_ED_lNr
          UI_AP (UI_FuncSet, UID_ouf_lNr, PTR_INT(i1));
        }
        // den Focus wieder ans Edit-Window
        // UI_AP (UI_FuncFocus, UID_WinEdit, NULL);
        GUI_edi_Focus (&winED);


      }

      // unhilite all
      DL_hili_off (-1L);
      DL_Redraw ();

  }


  return 0;
}
*/


//================================================================
  int UI_objView__ (int mode) {
//================================================================

  if(mode == 0) {
      printf("UI_objView__ - disactivate view\n");
    // UI_reset_hide (1);
    GUI_ckbutt_set_noCB (&ckb_view, FALSE);
    GA_hide__ (6, 0L, 0); // reverseMode off
    UI_stat_view = 1;    // 0=OFF
    DL_hide_chg ();
    DL_Redraw ();
  }

  return 0;

}


//===================================================
  int UI_CB_view  (MemObj *mo, void **data) {
//===================================================
// restore hidden objects
// see also UI_CB_hide


  printf("UI_CB_view %d\n",GUI_DATA_EVENT);
  // printf("  UI_stat_hide=%d UI_stat_view=%d\n",
           // UI_stat_hide,UI_stat_view);  // 0=ON; 1=OFF



  //----------------------------------------------------------
 // disactivate view
  if(GUI_DATA_EVENT == TYP_EventPress) goto L_activate;
    printf("disactivate view\n");

  UI_objView__ (0);  // disactivate view\

  // reactivate functions ..
  UI_func_stat_set__ (APF_TB_CAD,
                      APF_MEN_FIL,
                      APF_MEN_SM,
                      APF_MEN_MDF,
                      APF_MEN_APP,
                      // APF_HIDE,
                      APF_WIN_B_E,
                      APF_VWCADMAN,
                      APF_MEASEA,
                      APF_BUT_GSE,
                      APF_PLUGINS,
                      APF_UNDO,
                      0);

  sele_setNoParents (0);         // enable selection of parents (unvisible)
  sele_restore ();
  
  return 0;



  //----------------------------------------------------------
  L_activate:
    printf("activate view\n");

  // if hide is active: disactivate hide.
  if (UI_stat_hide == 0) {
    // uncheck checkbox Hide
    GUI_ckbutt_set_noCB (&ckb_hide, FALSE);
    sele_restore ();
    UI_stat_hide = 1;    // 0=OFF
  }




  //----------------------------------------------------------
  // printf("activate view\n");
  UI_stat_view = 0;    // 0=active

  Grp_Clear (0);       // clear group


  // block some functions ..
  // GUI_set_enable (&box1C1v, FALSE); // disact. GO, RUN
  // UI_brw__ (0);                               // disact.. EditWin
  // GUI_set_enable (&box1X, FALSE);   // disact. S/M und CAD ...
  // GUI_set_enable (&menu_bar, FALSE); // disact. alle menus
  UI_func_stat_set__ (-APF_TB_CAD,
                      -APF_MEN_FIL,
                      -APF_MEN_SM,
                      -APF_MEN_MDF,
                      -APF_MEN_APP,
                      // -APF_HIDE,
                      -APF_WIN_B_E,
                      -APF_VWCADMAN,
                      -APF_MEASEA,
                      -APF_BUT_GSE,
                      -APF_PLUGINS,
                      -APF_UNDO,
                      0);


  MSG_pri_0 ("ViewOn");
  // TX_Print("display hidden objects; select objects to display");

  // clear group (sonst wird auch group angezeigt)
  Grp_Clear (0);

  // loeschen der Hili-Tabelle
  // (sonst werden alle gehliteten als hidden angezeigt !)
  DL_hili_on (-1L);

  GA_hide__ (5, 0L, 0);   // reverseMode on

  sele_save ();
  sele_set__ (Typ_goGeom);       // enable selection of all types
  sele_setNoConstrPln ();        // disable selection of point on ConstrPln



  L_fertig:

  // change VIEW-Bit of complete DL
  DL_hide_chg ();
  DL_Redraw ();

    // printf("ex UI_CB_view UI_stat_hide=%d UI_stat_view=%d\n",
           // UI_stat_hide,UI_stat_view);  // 0=ON; 1=OFF

  return 0;

}


//================================================================
  int UI_src_edi () {
//================================================================
// start modus with datasource = editor, rework ..
// see UI_src_mem

  char        s1[256];
  long        l1;
  void        *fBuf;


  // printf("UI_src_edi %d\n",AP_src);
    // printf(" APP_stat=%d APP_act_typ=%d\n",AP_stat.APP_stat,APP_act_typ);
  // UTF_dump__ ();


  // addOn-prog active ?
  if(PRC_IS_ACTIVE) {

    // copy active process into editor
    // get file -> tempSpc
    sprintf(s1, "%s%s",OS_get_tmp_dir(),APP_act_nam);
      // printf(" get file |%s|\n",s1);
    // UME_TMP_FILE (&fBuf, &UI_Ed_fsiz, s1);
    UI_Ed_fsiz = OS_FilSiz (s1);   
    l1 = UI_Ed_fsiz + 100 + UI_Ed_fsiz/4;
    fBuf = MEM_alloc_tmp ((int)l1);
    MEM_get_file (fBuf, &UI_Ed_fsiz, s1);
    if(UI_Ed_fsiz < 1) {
      TX_Print("UI_src_edi E001 |%s|",s1);
      return -1;
    }
    // copy active process into editor
    GUI_edi_Write (&winED, fBuf, &UI_Ed_fsiz, l1);
      // printf(" UI_Ed_fsiz=%ld l1=%ld\n",UI_Ed_fsiz,l1);
    ED_Reset ();    // ED_lnr_act = 0;
    goto L_1;
  }




  // normal
  if(AP_src == AP_SRC_MEM) {
    // die Hauptdatei raus ins Editfenster (mem->Window)
    ED_load__ ();
  }


  L_1:
  AP_src = AP_SRC_EDI;

  // rework all ..
  UI_but_END ();

  // display help-info
  TX_Print ("- key F1 - display help ..");

    // printf("ex UI_src_edi\n");

  return 0;

}


//================================================================
  int UI_src_mem (int mode) {
//================================================================
// start modus with datasource = memory


  // printf("UI_src_mem %d\n",AP_stat.sysStat);

  if(mode > 0) {

    // vom Window ins Memory kopieren
    if(AP_src == AP_SRC_EDI) {

      // test if size of editor == size of memSpc
      if(GUI_edi_getModif(&winED)) {
        xa_fl_TxMem = 1;
        ED_unload__ ();
      }
    }

    AP_src = AP_SRC_MEM;
    UI_but_END ();
  }

  AP_src = AP_SRC_MEM;


  return 0;

}


//================================================================
  int UI_but_END () {
//================================================================
// reprocess whole model

  int    i2, opMod;
  long   ll;


  // printf(" butEND: InpMode=%d\n",UI_InpMode); // 2=MAN,3=CAD

  GUI_set_enable (&but_end, FALSE);

  // opMod = UI_ask_mode ();
  opMod = UI_InpMode;

  ll = ED_work_END (0);

  // nach einem Error: select Line.
  i2 = AP_errStat_get();
    // printf(" n.work_END: lNr=%d iErr=%d\n",ll,i2);


  if(opMod == UI_MODE_CAD) {

    // CAD: give Line -> LineEditor; reRun.
    if(i2 != 0) {
      printf(" CAD-Error: edit Line %ld\n",ll);
      // ask LineNr
      ll = ED_get_lnr_act();
      // modify zeile im mem mit dem SystemEditor.
      // AP_src_mod_ed (ll+1);
      AP_src_mod_ed (ll);              // 2010-09-11
    }



  } else if(opMod == UI_MODE_MAN) {

    if(i2 != 0) {  // Ablauffehler in MAN; set CurPos, select act. Line.
      GUI_edi_sel_ln (&winED, ll); // select Line, set Curpos to Line.
    }
  }

  GUI_set_enable (&but_end, TRUE);

    // printf("ex UI_but_END\n");

  return 0;

}
 

//================================================================
  int UI_VWR_ON () {
//================================================================
// activate VWR

  int     i1,  opMod;


  // printf("UI_VWR_ON \n");

    opMod = UI_InpMode;    // old mode

      if(opMod == UI_MODE_VWR) return 0;


      // printf("VWR - activate\n");
      UI_InpMode = UI_MODE_VWR;

      MSG_pri_0 ("VWR_On");

        GL_temp_del_1 (DLI_TMP);               // remove tempObj
        GUI_gl_block (&winMain, 1);
        // UI_cb_search (0);  // switch off "Search/Name"        2011-03-03

        // disactivate menu Insert Select CATALOG,
        // enable selection of all types
        UI_set_Ins_Sel_Cat (FALSE);

        // reactiv. Interact (MAN->VWR u CAD->VWR)
        // GUI_menu_checkbox_set (&ckb_Iact, TRUE);
        GUI_set_enable (&ckb_Iact, TRUE);

        // GUI_update__ ();
        GUI_gl_block (&winMain, 0);

        // MAN,CAD -> VWR: hide all activities, joints.
        DL_hide_unvisTypes (1);


      //----------------------------------------------------------------
      // MAN -> VWR:
      if(opMod == UI_MODE_MAN) {
        UI_brw__ (0);         // das Edit-Fenster deaktivieren

        // act. GO,STEP;   MAN:On; CAD,VWR:Off.
        GUI_set_enable (&but_go, FALSE);
        GUI_set_enable (&but_step, FALSE);

      }


      //----------------------------------------------------------------
      // // CAD -> VWR:
      // if(opMod == UI_MODE_CAD) {
        // IE_cad_exitFunc ();                // remove GroupEdit-win
        // // remove CAD-toolbar tbCad
        // GUI_set_show (&tbCad, 0);
      // }


  GUI_update__ ();  // f Gtk3
  DL_Redraw ();


  return 0;

}

 
//================================================================
  int UI_VWR_OFF () {
//================================================================
// disactivate VWR

  // printf("UI_VWR_OFF \n");

  return 0;

}


//================================================================
  int UI_CAD_reload () {
//================================================================
// reload CAD-toolbox (language changed)

  int    i1;



  if(GUI_OBJ_IS_VALID(&tbCad)) {
      printf("UI_CAD_reload-1\n");
    // delete CAD-menu's (was already loaded)
    tbCad = GUI_toolbox_del (&tbCad);
    // reload all 
    tbCad = UI_cad (&UIw_Box_TB, NULL);
  }


  // hide if not active
  i1 = GUI_radiobutt_get (&ckb_cad);     // 1=active, 0=not
  if(!i1)  GUI_set_show (&tbCad, 0);



  return 0;

}

 
//================================================================
  int UI_CAD_ON () {
//================================================================
// activate CAD

  int     i1,  opMod;


  // printf("UI_CAD_ON \n");

    opMod = UI_InpMode;    // old mode


      if(opMod == UI_MODE_CAD) return 0;


      // printf("CAD - activate\n");
      UI_InpMode = UI_MODE_CAD;
         UI_brw__ (0);         // das Edit-Fenster deaktivieren

        MSG_pri_0 ("CAD_On");

        GUI_gl_block (&winMain, 1);
        // UI_cb_search (1);  // switch on "Search/Name"          2011-03-03

        // activate menu Insert Select CATALOG,
        // enable selection of all types
        UI_set_Ins_Sel_Cat (TRUE);

        if(!GUI_OBJ_IS_VALID(&tbCad)) {
            // GUI_obj_dump_mo (&UIw_Box_TB);
          tbCad = UI_cad (&UIw_Box_TB, NULL);

        } else {
          GUI_set_show (&tbCad, 1);
        }

        // UI_Focus = -1;
        IE_cad_init0 ();
        GUI_gl_block (&winMain, 0);

        //----------------------------------------------------------------
        // MAN -> CAD:
        if(opMod == UI_MODE_MAN) {
          printf(" MAN -> CAD\n");
          // act. GO,STEP;   MAN:On; CAD,VWR:Off.
          GUI_set_enable (&but_go, FALSE);
          GUI_set_enable (&but_step, FALSE);
        }

        //----------------------------------------------------------------
        // VWR -> CAD: view all activities, joints.
        if(opMod == UI_MODE_VWR) {
          printf(" VWR -> CAD\n");

          // disactiv. Interact.
          // GUI_menu_checkbox_set (&ckb_Iact, FALSE);
          GUI_set_enable (&ckb_Iact, FALSE);

          DL_hide_unvisTypes (0);
          DL_Redraw ();  // first time only necessary ..
        }

  GUI_obj_focus (&winGR);

  return 0;

}


//================================================================
  int UI_CAD_OFF () {
//================================================================
// disactivate CAD


  // printf("UI_CAD_OFF \n");
      // printf("CAD - disactivate\n");
        // remove GroupEdit-win
        IE_cad_exitFunc ();
        // remove CAD-toolbar tbCad
        GUI_set_show (&tbCad, 0);



  return 0;

}


//================================================================
  int UI_MAN_ON () {
//================================================================
// activate MAN
 
  int     i1,  opMod;


  // printf("UI_MAN_ON \n");


    opMod = UI_InpMode;    // old mode

    if(opMod == UI_MODE_MAN) return 0;

        // printf("MAN - activate\n");
      MSG_pri_0 ("MAN_On");

      // activate menu Insert Select CATALOG,
      // enable selection of all types
      UI_set_Ins_Sel_Cat (TRUE);

      // disactiv. Interactons. (MAN,CAD: Off; VWR:On).
      // GUI_menu_checkbox_set (&ckb_Iact, FALSE);
      GUI_set_enable (&ckb_Iact, FALSE);

      UI_InpMode = UI_MODE_MAN;
        // printf(" MAN UI_InpMode=%d\n",UI_InpMode);

      // act. GO,STEP,END;   MAN:On; CAD,VWR:Off.
      GUI_set_enable (&but_go, TRUE);
      GUI_set_enable (&but_step, TRUE);

      // das Edit-Fenster reaktivieren
      UI_brw__ (0);

      // start modus with datasource = editor, rework ..
      UI_src_edi ();             // mem -> edi

#ifdef _MSC_VER
// Im Editor in MS-Win sonst kein Cursor; nur aktivieren anderes Window hilft ..
IE_ed1__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));
IE_ed1__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncKill));
#endif



      //----------------------------------------------------------------
      // CAD -> MAN:    nothing to do ..


      //----------------------------------------------------------------
      // VWR -> MAN
      if(opMod == UI_MODE_VWR) {
          // printf(" VWR -> MAN\n");
        DL_hide_unvisTypes (0);      // view joints,activities.
        DL_Redraw ();                // first time only necessary ..

      }


  L_exit:
    // printf("ex UI_MAN_ON\n");
  return 0;

}


//================================================================
  int UI_MAN_OFF () {
//================================================================
// disactivate MAN


  // printf("UI_MAN_OFF \n");


  if(PRC_IS_ACTIVE) {
    PRC_disactivate__ ();

  } else {
      // editor -> memory
      UI_src_mem (1);
  }

  // primary source memory not editor
  AP_src = AP_SRC_MEM;

  // reset hidden-state after hilite
  DL_hili_off (-2);

  return 0;

}


//=====================================================================
  int UI_butCB (MemObj *mo, void **data) {
//=====================================================================
/*
Callback fuer Buttons.

See UI_but__ (txt);
*/

  int    i1, i2, opMod;
  long   ll;
  double d1;
  char   *cp1, *p1, *p2;


  // printf("\n=================================================\n");
  cp1 = GUI_DATA_S1;
  printf("UI_butCB |%s|\n",cp1);
  // printf("  UI_InpMode=%d\n",UI_InpMode);




  if(UI_InpMode  == UI_MODE_START) return 0;   // startup


  // manchmal bleibt Esc-Taste gedrueckt; dann gar keine Ausgabe mehr !
  KeyStatEscape = OFF;



  //=============================================================
  if(!strcmp(cp1, "butGO")) {
    // work lines 0 to <active-line>
    i1 = ED_work_GO ();
    TX_Print("Go [0 - %d]",i1);
    goto Fertig;


  //=============================================================
  } else if(!strcmp(cp1, "butSTEP")) {

    i1 = ED_work_STEP ();

    // // disp LineNr
    // UI_AP (UI_FuncSet, UID_ouf_lNr, (void*)i1);

    TX_Print("Step [%d]",i1);
    goto Fertig;


  //=============================================================
  } else if(!strcmp(cp1, "butEND")) {
    UI_but_END ();     // reprocess whole model



/*
  //=============================================================
  } else if(!strcmp(cp1, "butUndo")) {
    if(UI_InpMode == UI_MODE_WCGEO) {
      UI_wcGeo_Undo ();
    } else {
      AP_Undo ();
    }

  //=============================================================
  } else if(!strcmp(cp1, "butRedo")) {
    AP_Redo ();
*/


  //=============================================================
  } else if(!strcmp(cp1, "butVwpt")) {
    vwpt_UI ();


  //=============================================================
  } else if(!strcmp(cp1, "butClear")) {
    UI_view__ ("ClrView");


  //=============================================================
  } else if(!strcmp(cp1, "butHili")) {
    // DL_hili_off (-1L); DL_Redraw (); // unhilite alle Objekte
    UI_unHili ();  // clear group, unhilite all objs


  //=============================================================
  } else if(!strcmp(cp1, "butTop")) {
    UI_view__ ("TopView");  // UI_view__ -> UI_viewCB


  //=============================================================
  } else if(!strcmp(cp1, "butFront")) {
    UI_view__ ("FrontView");


  //=============================================================
  } else if(!strcmp(cp1, "butSide")) {
    UI_view__ ("SideView");


  //=============================================================
  } else if(!strcmp(cp1, "butAxo")) {
    UI_view__ ("IsoView");


  //=============================================================
  } else if(!strcmp(cp1, "butAll")) {              // ScaleFix
    UI_GR_ScalAuto (1);


  //=============================================================
  } else if(!strcmp(cp1, "butRes")) {              // ScaleAll
    UI_GR_ScalAuto (0);


  //=============================================================
  } else if(!strcmp(cp1, "butScG")) {              // Scale group
    DL_scale_grp ();


  //=============================================================
  } else if(!strcmp(cp1, "oNam")) {              // Search/Name
/* 2011-03-03
    if(UI_InpMode == UI_MODE_VWR) {
      TX_Print("***** active in CAD only ..");
      goto L_exit;
    }
*/
    // check if Search is active
    i1 = GUI_ckbutt_get (&ckb_such); // 0=not sel, 1=selected.
      // printf(" ckb_such-state=%d\n",i1);
    if(i1) {
      // activate
      // UI_mcl__ (NULL, PTR_INT(UI_FuncInit));
      UI_mcl__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));
    } else {
      // disactivate
      // UI_mcl__ (NULL, PTR_INT(-1));
      UI_mcl__ (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncExit));
    }




  //=============================================================
  } else if(!strcmp(cp1, "MAN")) {
    i1 = GUI_radiobutt_get (&ckb_man);     // 1=active, 0=not
      // printf(" ckb_man=%d\n",i1);
    if(i1) {
      UI_MAN_ON ();
    } else {
      UI_MAN_OFF ();
    }



  //=============================================================
  } else if(!strcmp(cp1, "VWR")) {
    i1 = GUI_radiobutt_get (&ckb_vwr);     // 1=active, 0=not
      // printf(" ckb_vwr=%d\n",i1);
    if(i1) {
      UI_VWR_ON ();
    } else { 
      UI_VWR_OFF ();
    }
        

  //=============================================================
  } else if(!strcmp(cp1, "CAD")) {
    i1 = GUI_radiobutt_get (&ckb_cad);     // 1=active, 0=not
      // printf(" ckb_cad=%d\n",i1);
    if(i1) {
      UI_CAD_ON ();
    }  else {
      UI_CAD_OFF ();
    } 
        

  //-------------------------------------------------
  } else if(!strcmp(cp1, "intAct")) {
    // i1 = GUI_menu_checkbox_get (&ckb_Iact);
    i1 = GUI_ckbutt_get (&ckb_Iact);
      // printf(" Iact=%d\n",i1);  //0=OFF, 1=ON
    if(i1 == 1) {
      // ON
      AP_stat.iActStat = 1;
      // set filter = object_with_activity_only
        // printf(" actFilt=%d\n",UI_GR_Sel_Filter(-1));
      UI_GR_Sel_Filter (21);
    } else {
      // OFF
      AP_stat.iActStat = 0;
      // set filter = standard
      UI_GR_Sel_Filter (0);
    }


/*
  //=============================================================
  } else if((!strcmp(cp1, "CAD"))||(!strcmp(cp1, "VWR"))) {
    // i1 = GTK_TOGGLE_BUTTON (parent)->active;
    i1 = GUI_radiobutt_get (&ckb_cad);
    i2 = GUI_radiobutt_get (&ckb_vwr);
    if((i1 != 0)||(i2 != 0)) {     // activate
      printf("VWR/CAD - activate\n");

      if(UI_askExit(mo, data) != 0) goto Fertig;



      // // disactiv. S/M
      // if(UI_InpSM == ON) GUI_ckbutt_set (ckb_mdel, FALSE);

      // disact. GO, STEP END
      GUI_set_enable (&but_go, FALSE);
      GUI_set_enable (&but_step, FALSE);
      // GUI_set_enable (but_end, FALSE);

      opMod = UI_InpMode;    // old mode
      if(!strcmp(cp1, "CAD")) UI_InpMode = UI_MODE_CAD;
      else                    UI_InpMode = UI_MODE_VWR;

      // das Edit-Fenster deaktivieren
      UI_brw__ (0);

      AP_src = AP_SRC_MEM;  // 2011-05-29  after MAN necessary !

      // if startup is done ..
      // if(AP_stat.sysStat > 1) UI_src_mem(1);  // edi -> mem RAUS 2011-01-23


      if(UI_InpMode == UI_MODE_CAD) {   // activate CAD
        GUI_gl_block (&winMain, 1);
        // UI_cb_search (1);  // switch on "Search/Name"          2011-03-03

        // activate menu Insert Select CATALOG,
        // enable selection of all types
        UI_set_Ins_Sel_Cat (TRUE);

        // disactiv. Interact.
        GUI_menu_checkbox_set (&ckb_Iact, FALSE);

        if(!GUI_OBJ_IS_VALID(&tbCad)) {
          tbCad = UI_cad (&UIw_Box_TB, NULL);
        } else {
          // GUI_update__ ();
          GUI_set_show (&tbCad, 1);
          // GUI_update__ ();
          // UI_GR_RECONFIG (NULL, NULL);
          // GUI_Idle (UI_GR_RECONFIG, NULL);
        }

        // UI_Focus = -1;
        IE_cad_init0 ();
        // GUI_update__ ();
        GUI_gl_block (&winMain, 0);

        // VWR -> CAD: view all activities, joints.
        if(opMod == UI_MODE_VWR) {
          printf(" VWR -> CAD\n");
          DL_hide_unvisTypes (0);
          DL_Redraw ();  // first time only necessary ..
        }


      } else {                         //  activate VWR
          printf(" activate VWR\n");
        // UI_block__ (-1, 1, -1);
        GL_temp_del_1 (DLI_TMP);               // remove tempObj
        GUI_gl_block (&winMain, 1);
        // UI_cb_search (0);  // switch off "Search/Name"        2011-03-03

        // disactivate menu Insert Select CATALOG,
        // enable selection of all types
        UI_set_Ins_Sel_Cat (FALSE);

        // reactiv. Interact.
        GUI_menu_checkbox_set (&ckb_Iact, TRUE);

        // GUI_set_enable (ckb_mdel, FALSE);           2011-03-03

        // GUI_update__ ();
        GUI_gl_block (&winMain, 0);
       
        // CAD -> VWR: hide all activities, joints.
        // if(opMod == UI_MODE_CAD) {
          DL_hide_unvisTypes (1);
          if(opMod == UI_MODE_MAN) DL_Redraw ();
        // }

        // remove CAD-toolbar tbCad
        GUI_set_show (&tbCad, 0);
      }

      return 0;


    //-----------------------------------------
    } else {
      printf("VWR/CAD - disactivate\n");

      GUI_gl_block (&winMain, 1);

      // reactivete SM (aus in VWR)
      // GUI_set_enable (ckb_mdel, TRUE);           2011-03-03

      // CAD-Mode- Modify -> Add
      IE_cad_reset_modif ();

      // reset to perm, kill GroupWindow ..
      IE_cad_exit0 ();


      // del temp. Objects
      GL_temp_del_all ();
      // unhilite alle Objekte (last obj zB Kontur sonst ganz rot)
      DL_hili_off (-1L);

      // AP_src = AP_SRC_EDI;
      // // die Hauptdatei raus ins Editfenster (mem->Window)
      // ED_load__ ();

      if(GUI_OBJ_IS_VALID(&tbCad)) GUI_set_show (&tbCad, 1);
          // UI_GR_RECONFIG (NULL, NULL);
          // GUI_Idle (UI_GR_RECONFIG, NULL);
      // GUI_update__ ();

      GUI_gl_block (&winMain, 0);

      goto Fertig;
    }
*/
/*
  //=============================================================
  } else if(!strcmp(cp1, "NC")) {
    i1 = GTK_TOGGLE_BUTTON (parent)->active;
    if(i1 != 0) {     // activate
      printf("NC - activate\n");

      if(UI_askExit(mo, data) != 0) goto Fertig;
      // act. GO, STEP
      GUI_set_enable (&but_go, TRUE);
      GUI_set_enable (&but_step, TRUE);

      // if(tbNc == NULL) tbNc = UI_wcg__ (UIw_Box_TB, NULL);
      // gtk_widget_show (tbNc);
      UI_InpMode = UI_MODE_NC;

    } else {
      printf("NC - disactivate\n");
      // gtk_widget_hide (tbNc);

      goto Fertig;
    }

    // UI_mBars_off (); // Menuebar ein- od ausblenden
*/


  //=============================================================
  } else if(!strcmp(cp1, "Meas")) {
    if(GUI_ckbutt_get (&ckb_meas) == 0) {
      // disactivation
      Meas_exit__ ();
    } else {
      // activation
      Meas_init ();
    }


/*
  //=============================================================
  } else if(!strcmp(cp1, "DE_GEO")) {

    i1 = GTK_TOGGLE_BUTTON (parent)->active;
    if(i1 != 0) {     // activate
      printf("DE_GEO - activate\n");

      // GEO darf nur aktiviert werden, wenn kein TEC aktiv ist.
      // if(UI_wcTec_askExit(parent, "DE_GEO") != 0) goto Fertig;

      // disact. GO, STEP, END
      GUI_set_enable (but_go, FALSE);
      GUI_set_enable (but_step, FALSE);
      GUI_set_enable (but_end, FALSE);

			// rufen UI_wcg_askExit
      if(UI_askExit(parent, data) != 0) goto Fertig;

      // vom Window ins Memory kopieren
      ED_unload__ ();

      // das Edit-Fenster deaktivieren
      // gtk_widget_hide (winED);
      gtk_widget_set_usize (winED.win, 1, 500);
      GUI_set_enable (winED.win, FALSE);

      // // die Filefunctions deaktivieren
      // UI_file_sensi (FALSE);

      if(tbWcGeo == NULL) tbWcGeo = UI_wcg__ (UIw_Box_TB, NULL);
      gtk_widget_show (tbWcGeo);
      UI_InpMode = UI_MODE_WCGEO;

      UI_wcg_init_ ();

    } else {
      printf("NC_Wcut - disactivate\n");
      gtk_widget_hide (tbWcGeo);

      // // die Filefunctions reaktivieren
      // UI_file_sensi (TRUE);


      goto Fertig;
    }

    // UI_mBars_off (); // Menuebar ein- od ausblenden




  //=============================================================
  } else if(!strcmp(cp1, "DE_TEC")) {

    // TEC darf nur aktiviert werden, wenn kein GEO aktiv ist.
    // if(UI_wcg_askExit(parent, "DE_TEC") != 0) goto Fertig;

    i1 = GTK_TOGGLE_BUTTON (parent)->active;
    if(i1 != 0) {     // activate
      printf("DE_TEC - activate\n");

      if(UI_askExit(parent, data) != 0) goto Fertig;

      // disact. GO, STEP END
      GUI_set_enable (but_go, FALSE);
      GUI_set_enable (but_step, FALSE);
      GUI_set_enable (but_end, FALSE);

      // vom Window ins Memory kopieren
      ED_unload__ ();

      // das Edit-Fenster deaktivieren
      // gtk_widget_hide (winED);
      gtk_widget_set_usize (winED.win, 1, 500);
      GUI_set_enable (winED.win, FALSE);

      if(tbWcTec == NULL) tbWcTec = UI_wcTec__ (UIw_Box_TB, NULL);
      gtk_widget_show (tbWcTec);

      UI_InpMode = UI_MODE_WCTEC;

      UI_wcTec_Init1 ();


    } else {
      printf("NC-Tech - disactivate\n");
      gtk_widget_hide (tbWcTec);

      // User moechte NC-Tech verlassen;
      i1 = UI_wcTec_askExit(NULL, "");


      goto Fertig;
    }

    // UI_mBars_off (); // Menuebar sensitiv machen oder abdunkeln
*/





  //=============================================================
  }



  Fertig:
    // printf("UI_butCB Fertig\n");
  /* Reset focus to glarea widget */
  /* UI_GR_setKeyFocus (); */


  // MAN: den Focus wieder ans Edit-Window
  if(AP_src == AP_SRC_EDI) {
    // UI_AP (UI_FuncFocus, UID_WinEdit, NULL);
    GUI_edi_Focus (&winED);
  }

  // DL_Redraw ();
  L_exit:
  return 0;
}


/*
//=====================================================================
  int UI_fix_lang () {
//=====================================================================
// de od en -> AP_lang


  strcpy(AP_lang, "de");

  if(((GtkCheckMenuItem*)UI_lang[1])->active == 1) {
    strcpy(AP_lang, "en");

  } else if(((GtkCheckMenuItem*)UI_lang[2])->active == 1) {
    strcpy(AP_lang, "fr");

  } else if(((GtkCheckMenuItem*)UI_lang[3])->active == 1) {
    strcpy(AP_lang, "es");

  } else if(((GtkCheckMenuItem*)UI_lang[4])->active == 1) {
    strcpy(AP_lang, "it");

  }

    printf("UI_fix_lang %s\n",AP_lang);

  return 0;

}
*/

//=====================================================================
  int UI_reset_hide (int mode) {
//=====================================================================
// disactivate 0=Hide 1=View 2=beide

  if(mode != 1)
    GUI_ckbutt_set (&ckb_hide, FALSE);

  if(mode != 0)
    GUI_ckbutt_set (&ckb_view, FALSE);

  return 0;

}


//=====================================================================
  int UI_main_set__ (int iMode) {
//=====================================================================
// back to VWR

  if(iMode == UI_MODE_VWR) GUI_radiobutt_set (&ckb_vwr);
  if(iMode == UI_MODE_CAD) GUI_radiobutt_set (&ckb_cad);
  if(iMode == UI_MODE_MAN) GUI_radiobutt_set (&ckb_man);

  // // disactivate Hide & View
  // UI_reset_hide (2);

  return 0;

}


//=====================================================================
  int UI_ToolBars (int inr, int mode) {
//=====================================================================
// Toolbars ein/ausschalten.
// 0 = OFF; 1=ON.

  if(inr == 1) {
    GUI_menu_checkbox_set (&ckb_Bar1, mode);
    GUI_set_show (&ToolBar1, mode);
    
  } else {
    GUI_menu_checkbox_set (&ckb_Bar2, mode);
    GUI_set_show (&ToolBar2, mode);
  } 

  return 0;

}


//================================================================
  int UI_grp__ (int mode) {
//================================================================


  if(mode == ON) {
      // printf("UI_grp__ set grpAdd ON\n");
    Grp_Start ();
    // DL_grp1_set (-1L, OFF);           // clear all groupBits
    // if(UI_GR_Sel_Filter(-1) != 5)     // nochmal ware Reset !

// 2 raus 2011-07-30
    // if(UI_GR_Sel_Filter(-1) != 5)        // nochmal waere Reset !
    // UI_GR_Sel_Filter (5);                // activate add to group

    // DL_hili_off (-1L); DL_Redraw (); // unhilite alle Objekte

  } else {
      // printf("UI_grp__ set grpAdd OFF\n");
    Grp_Clear (1);
    // DL_grp1_set (-1L, OFF);           // clear all groupBits
    // UI_GR_Sel_Filter (0);             // reset add to group
    // DL_hili_off (-1L); DL_Redraw (); // unhilite alle Objekte

  }

  return 0;

}


/*
//================================================================
  int UI_grpAdd (GtkWidget *parent, void *data) {
//================================================================


  if(((GtkCheckMenuItem*)parent)->active == 1) {
    UI_grp__ (ON);     // activate add to group

  } else {
    UI_grp__ (OFF);    // reset add to group

  }

  return 0;

}
*/


//================================================================
 int UI_dump_oid (char *sOid) {
//================================================================
/// UI_dump_oid    dump DB-object  into file & display with browser

  int   typ;
  long  dbi;


  APED_dbo_oid (&typ, &dbi, sOid);

  return UI_dump_obj (typ, dbi);

}


//================================================================
 int UI_dump_obj (int typ, long ind) {
//================================================================
/// \code
/// UI_dump_obj   dump DB-object  into file & display with browser
/// see also UTO_dump__
/// \endcode

  int    i1, i2, oTyp, oNr;
  long   l1, iga;
  void   *vp;
  char   cbuf1[256], cbuf2[64], cbuf3[64];

  ObjAtt    *ga1;
  ColRGB    *col1;

  TexBas  *tb;
  TexRef  *tr;



  // printf("================UI_dump_obj==================================\n");
  // printf("UI_dump_obj %d %ld\n",typ,ind);

  oNr = 1;

  APED_oid_dbo__(cbuf2, typ, ind);
  TX_Print ("dump obj %s to textWindow ..",cbuf2);

  oTyp = typ;

  if(typ == Typ_Ditto) {
    sprintf(cbuf3, " - temporary object - Typ Ditto");

  } else if(typ == Typ_SymB) {
    sprintf(cbuf3, " - temporary object - bitmapSymbol");

  } else if(typ == Typ_SymV) {
    sprintf(cbuf3, " - temporary object - vectorSymbol");

  } else {
    i1 = UTO_get_DB (&vp, &oNr, &oTyp, ind);   // typ wird auf ObjGX gesetzt !
    if(oNr >= 0) goto L_disp;
    sprintf(cbuf3, " - temporary object");
  }




  L_err:
    vp = cbuf3;
    typ = Typ_Error;




  L_disp:

    // TESTBLOCK
    // if(oNr > 1) UT3D_nstru_dump (oTyp, oNr, vp, cbuf2);
    // else UT3D_stru_dump (oTyp, vp, cbuf2);                  // dump obj ..
    // UTO_dump__ (vp, "UI_dump__"); // return 0;
    // UTO_dump_s_ (vp, "UI_dump_obj"); return 0; // nur ObjGX !
    // END TESTBLOCK


  // open file
  sprintf(cbuf1, "%stmp.html",OS_get_tmp_dir());
  UT3D_stru_dump (TYP_FuncInit, (void*)"htm", cbuf1);


  // write objID  & typ
  UT3D_stru_dump (Typ_Txt, AP_src_typ__(oTyp), "%s        ",cbuf2);


  // dump obj ..
  if(oNr > 1) UT3D_nstru_dump (oTyp, oNr, vp, cbuf2);
  else        UT3D_stru_dump (oTyp, vp, cbuf2);                  // dump obj ..


  //----------------------------------------------------------------
  // Attributes: find GA-Rec
  iga = GA_find__ (typ, ind);
    printf(" %ld ex GA_find__ %d %ld\n",iga,typ,ind);
  if(iga < 0) goto L_out;

  GA_getRec (&ga1, iga);  // get the ga-record
  sprintf(cbuf2, "\n\nAttributes:");
  UT3D_stru_dump(Typ_Txt, cbuf2, "");


  // surf's : Color, Transparenz, Style, Style;
  if(typ == Typ_SUR) {
    col1 = COL_INT32(&ga1->iatt); // col1 = (ColRGB*)&ga1->iatt;
/*
    if(col1->color == 0) {
      sprintf(cbuf2, " Color = Default");
      UT3D_stru_dump(Typ_Txt, cbuf2, "");
    } else {
*/
    if(col1->color > 0) {
      UT3D_stru_dump (Typ_Color, col1, " ");         // write color
    }
    if(col1->vtra > 0) {
      sprintf(cbuf2, " Transparency = %d",col1->vtra);
      UT3D_stru_dump(Typ_Txt, cbuf2, "");
    }
    if(col1->vtex > 0) {
      i1 = GA_tex_ga2tr (iga);
      Tex_getRef (&tr, i1);
      UT3D_stru_dump (Typ_TEXR, tr, "Texture-Reference[%d]:\n",i1);
      i1 = tr->ibas;
      Tex_getBas__ (&tb, i1);
      UT3D_stru_dump (Typ_TEXB, tb, "Basic-Texture[%d]:\n",i1);
    }



  } else if((typ == Typ_LN)   ||
            (typ == Typ_CI)   ||
            (typ == Typ_CV))     {
    sprintf(cbuf2, "  Linetyp = %d\n",ga1->iatt);
    UT3D_stru_dump(Typ_Txt, cbuf2, "");
  }



  L_out:
  UT3D_stru_dump (TYP_FuncEnd, (void*)"htm", "");
  OS_browse_htm (cbuf1, NULL);

  return 0;

}


//================================================================
  int UI_dump__ (int typ) {
//================================================================
// dump textures

  int  i1;
  char cbuf1[256];
  FILE *fpo;


  // open temp. html-File
  sprintf(cbuf1, "%stmp.html",OS_get_tmp_dir());
  printf("UI_dump__ %d |%s|\n",typ,cbuf1);




  i1 = UTX_htm_fop (&fpo, cbuf1);    // open
  DB_dump_f (fpo, typ);              // dump
  UTX_htm_fcl (&fpo);                // close
  // disp file
  OS_browse_htm (cbuf1, NULL);


  return 0;

}


//==================================================================
  int UI_menCB (MemObj *mo, void **data) {
//==================================================================
/// Mainentry Menufunktions.
   
  return UI_men__ (GUI_DATA_S1);

}


//==================================================================
  int UI_men__ (char *cmd) {
//==================================================================
/// Mainentry Menufunktions.
/// Retcod:  0=OK; -1=error; cancel.


  int    irc, i1, i2, i3, imode, iCompr;
  long   il1, il2;
  char   *cp1, *p1, cbuf1[256], cbuf2[256], s1[256], sTit[80];
  FILE   *fpo;
  ColRGB col1;


  // printf("UI_menCB |%s|\n",GUI_DATA_S1);
  printf("UI_men__ |%s|\n",cmd);


  // printf("  AP_mod_fnam=|%s|\n",AP_mod_fnam);
  // printf("  AP_mod_dir=|%s|\n",AP_mod_dir);


  // // wenn AP_mod_dir kein closing "/" hat, eins zufuegen.
  // UTX_add_slash (AP_mod_dir);


/*
  // printf("  Widget-type =%d\n",gtk_type_is_a (gtk_menu_item_get_type ());
  // printf("  Widget-type =%d\n",GTK_IS_MENU_ITEM (parent));

  // wenn NICHT Menu-Item: skip disactivate-call
  if (GTK_IS_MENU_ITEM (parent) == 0) {
    if ((GTK_TOGGLE_BUTTON (parent)->active) == 0) goto Fertig;
  }
*/


  // cp1 = (char*)data;
  // cp1 = GUI_DATA_S1;
  cp1 = cmd;


  // imode = UI_ask_mode();
  imode = UI_InpMode;


  //-------------------------------------------------
  if(!strcmp(cp1, "new")) {

    // save Model+Submodels into tempDir as "Model" native
    Mod_sav_i (0);

    // compare Model - Mod_in
    i1 = Mod_sav_ck (1);
    if(i1) {
      // model has changed -
      irc = AP_save_ex (1);
      if(irc < 0) return 0;     // cancel
    } 

    // clear src-Memory, reset Undo, Hide, View-Plane, ConstrPlane.
    AP_src_new(1);

    // save Model+Submodels into tempDir as "Model" native
    Mod_sav_i (0);

    // make a copy (copy Model -> Mod_in)
    Mod_sav_ck (0);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "open")) {

    // // reset GUI  (back to VWR)
    // UI_reset__ ();


    // // save Model+Submodels into tempDir as "Model" native
    // Mod_sav_i (0);
    // compare Model - Mod_in
    i1 = Mod_sav_ck (1);
    if(i1) {
      // model has changed -
      i1 = AP_save_ex (1);
      if(i1 < 0) return 0;     // cancel
    }


    // get cbuf1 = filename, cbuf2=directory from user
    strcpy(sTit, MSG_const__(MSG_open));  // "Model open"


                        // fnam      dirnam     filter-out 
    AP_get_fnam_symDir (s1);   // get filename of cfg/dir.lst
    // let user select (symbolic-directory,) modelfile
    // get cbuf1=fnam cbuf2=dnam
    strcpy(cbuf2, AP_mod_dir);
    irc = GUI_file_open__ (cbuf1,128, cbuf2,128, UI_fnamFilt,
                           s1, sTit, NULL);
      printf("ex-GUI_file_open__ %d|%s|%s|\n",irc,cbuf2,cbuf1);
    if(irc) return irc;

    // clear src-Memory, reset Undo, Hide, View-Plane, ConstrPlane. 
    AP_src_new (1);
    // UI_menCB (NULL, "new");   // NEW

    // get cbuf2 = full-filename
    UTX_add_fnam_del (cbuf2);  // add following '/' to dnam
    strcat(cbuf2, cbuf1);

    // get AP_mod_sym, AP_mod_dir, AP_mod_fnam, AP_mod_ftyp, AP_mod_iftyp
    irc = AP_Mod_load_fn (cbuf2, 0);
    if(irc) return irc;

    if(!AP_mod_sym[0]) {
      // get symbol-name for new directory from user
      AP_mod_sym_get (AP_mod_sym, AP_mod_dir);
    }

    return 0;


/* UNUSED
  //-------------------------------------------------
  } else if(!strcmp(cp1, "opeDir")) {

    strcpy(cbuf1, AP_mod_dir);
    GUI_File_selext ("open", cbuf1, UI_openCB, (void*)"open");
*/

/*
  //-------------------------------------------------
  } else if(!strcmp(cp1, "opePtab")) {

    // Liste mit Dir-Auswahl
    sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_List2 ("open Pointfile",    // titletext
            AP_mod_dir,            // Pfadname des activeDirectory
            cbuf1,                  // Liste der directories
            (void*)APP_opePtab);    // CallBack der Liste
*/




  //-------------------------------------------------
/* UNUSED
  } else if(!strcmp(cp1, "insert")) {
    // GUI_File_selext ("Include File","./", UI_openCB,(void*)"insert");
    AP_get_fnam_symDir (cbuf1);   // get filename of dir.lst
    // sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_List2 ("Model add",    // titletext
            AP_mod_dir,       // Pfadname des activeDirectory
            cbuf1,              // Liste der directories
            (void*)UI_open_add); // CallBack der Liste
*/




  //----------------------------------------------------------------
  } else if(!strcmp(cp1, "save")) {

    // save, 1=overwrite.
    UI_save__ (1);

    // add filename to list "last-used"
    AP_Mod_lstAdd ();


/*
  } else if(!strcmp(cp1, "saveAs")) {
    // strcpy(txbuf, AP_mod_dir);
    // strcat(txbuf, fnam_del_s);
    // strcat(txbuf, "Unknown.dat");
    // GUI_File_selext ("save as",txbuf,UI_saveCB,NULL);
    sprintf(txbuf,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_save__ ("save model as",   // titletext
            AP_mod_dir,           // path
            txbuf,                 // directoryList
            AP_mod_fnam,             // defaultModelname
            (void*)UI_saveCB);




  //-------------------------------------------------
  } else if(!strcmp(cp1, "loadMock")) {
    sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_List2 ("Load Mockup-Model (VRML/OBJ/TESS)",    // titletext
            AP_mod_dir,       // Pfadname des activeDirectory
            cbuf1,              // Liste der directories
            (void*)UI_loadMock_CB);



  } else if(!strcmp(cp1, "loadBmp")) {
    sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_List2 ("Load Bitmap-Image (BMP)",    // titletext
            AP_mod_dir,       // Pfadname des activeDirectory
            cbuf1,              // Liste der directories
            (void*)UI_loadBmp_CB);




  } else if(!strcmp(cp1, "impDxf")) {
    strcpy(txbuf, AP_mod_dir);
    strcat(txbuf, fnam_del_s);
    strcat(txbuf, "*.dxf");
    GUI_File_selext ("import DXF",txbuf,UI_impDxfCB,NULL);



  } else if(!strcmp(cp1, "impIge")) {
    strcpy(txbuf, AP_mod_dir);
    strcat(txbuf, fnam_del_s);
    strcat(txbuf, "*.igs");
    GUI_File_selext ("import IGES",txbuf,UI_impIgeCB,NULL);



  } else if(!strcmp(cp1, "implwo")) {
    strcpy(txbuf, AP_mod_dir);
    strcat(txbuf, fnam_del_s);
    strcat(txbuf, "*.lwo");
    GUI_File_selext ("import LWO",txbuf,UI_impLwoCB,NULL);



  } else if(!strcmp(cp1, "imp3ds")) {
    strcpy(txbuf, AP_mod_dir);
    strcat(txbuf, fnam_del_s);
    strcat(txbuf, "*.3ds");
    GUI_File_selext ("import 3DS",txbuf,UI_imp3dsCB,NULL);


  } else if(!strcmp(cp1, "impobj")) {
    strcpy(txbuf, AP_mod_dir);
    strcat(txbuf, fnam_del_s);
    strcat(txbuf, "*.obj");
    GUI_File_selext ("import WaveFront - OBJ",txbuf,UI_impobjCB,NULL);
*/



/*
  //-------------------------------------------------
  } else if(!strcmp(cp1, "saveMock")) {
    sprintf(txbuf,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_save__ ("save tesselated as VRML / OBJ / TESS",   // titletext
            AP_mod_dir,           // path
            txbuf,                 // directoryList
            AP_mod_fnam,             // defaultModelname
            (void*)UI_saveMockCB);
*/





  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1nat")) {


    iCompr = GUI_menu_checkbox_get(&ckb_compr);  // 0=not checked; 1=checked
    if(iCompr == 0) strcpy(cbuf1, "gcad");
    else            strcpy(cbuf1, "gcaz");

    AP_save__ (0, cbuf1);



/*
  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp2nat")) {

    strcpy(memspc011, AP_mod_fnam);
    strcat(memspc011, ".gcad");

    // strcat(memspc011, "fn.dxf");
    GUI_File_selext ("save native", memspc011, UI_expNat, NULL);
*/



  //-------------------------------------------------
  } else if(!strcmp(cp1, "expDxf__")) {
    AP_stat.subtyp = 99;  // headerless
    AP_save__ (0, "dxf");

  //-------------------------------------------------
  } else if(!strcmp(cp1, "expDxfR10")) {
    AP_stat.subtyp = 0;
    AP_save__ (0, "dxf");

  //-------------------------------------------------
  } else if(!strcmp(cp1, "expDxf2000")) {
    AP_stat.subtyp = 3;
    AP_save__ (0, "dxf");



  //-------------------------------------------------
  } else if(!strcmp(cp1, "expStp")) {
    AP_save__ (0, "stp");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Ige")) {
/*
    strcpy(memspc011, AP_mod_fnam);
    strcat(memspc011, ".igs");
    AP_get_fnam_symDir (cbuf1);   // get filename of dir.lst
    // sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_save__ ("save model as IGES",   // titletext
            AP_mod_dir,           // path
            cbuf1,                 // directoryList
            memspc011,            // defaultModelname
            (void*)UI_saveCB);
*/
    AP_save__ (0, "igs");

/*
  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp2Ige")) {

    strcpy(memspc011, AP_mod_fnam);
    strcat(memspc011, ".igs");

    // strcat(memspc011, "fn.dxf");
    GUI_File_selext ("export IGES", memspc011, UI_expIgeCB, NULL);
*/




  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Wrl1")) {
    AP_stat.subtyp = 0;  // VRML1
    AP_save__ (0, "wrl");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Wrl2")) {
    AP_stat.subtyp = 1;  // VRML2
    AP_save__ (0, "wrl");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "expSVG")) {
    // see also Export/print svg; different versions !!!
    AP_save__ (0, "svg");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Obj")) {
    AP_save__ (0, "obj");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Stl")) {
    AP_save__ (0, "stl");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Tess")) {
    AP_save__ (0, "tess");


/*
  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp1Jpg")) {
    AP_save__ (0, "jpg");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "exp2Tess")) {
    strcpy(memspc011, AP_mod_fnam);
    strcat(memspc011, ".tess");
    // strcat(memspc011, "fn.dxf");
    GUI_File_selext ("save Mockup as TESS", memspc011, UI_expMockup, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "expISO")) {
    // strcpy(memspc011, AP_mod_fnam);
    // cp1 = strrchr (memspc011, '.');     // den letzten Punkt suchen
    // *cp1 = '\0';
    // strcat(memspc011, ".iso");
    // GUI_File_selext ("export ISO",memspc011,UI_expIsoCB,NULL);
    // AP_ExportISO (memspc011);
    ED_work_PP ();
*/


  //-------------------------------------------------
  } else if(!strcmp(cp1, "selHlp")) {
    APP_Help ("Select", "");

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpSav")) {        // save Group -> modelFile
    Grp_Mdl ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpSM")) {        // move group -> subModel
    Grp_SM ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpMov")) {        // move all objs of group
    GMVO__ (0);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpHid")) {        // hide all objs of group
    Grp_hide (0);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpAll")) {        // make  grp of all objs
    DL_grp1__ (0, NULL, 2, 0);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpHlp")) {        // HELP group
    APP_Help ("GROUP", "");

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpClr")) {        // clear group
    Grp_Clear (1);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpDel")) {        // delete all objs of group
    Del_grp__ ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "grpInv")) {
    Grp_Inv (1);


  //==========================================================
  } else if(!strcmp(cp1, "AppLoa")) {
    PRG_Loa ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "AppCre")) {
    PRG_Cre__ ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "AppDel")) {
    PRG_Del__ ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "AppEd")) {
    PRG_Ed ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "AppUnl")) {
    PLU_unl ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "AppHlp")) {        // HELP Application
    APP_Help ("Appli", "");


  //==========================================================
  } else if(!strcmp(cp1, "PrcLoa")) {
    PRC_Loa ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "PrcCre")) {
    PRC_Cre__ ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "PrcDel")) {
    PRC_Del__ ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "PrcHlp")) {        // HELP Application
    APP_Help ("Process", "");




  //==========================================================
  } else if(!strcmp(cp1, "PluLoa")) {
    PLU_Loa ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "PluHlp")) {        // HELP Application
    APP_Help ("Plugin", "");



  //==========================================================
  } else if(!strcmp(cp1, "RpcLoa")) {
    RPC_Loa ();

  //-------------------------------------------------
  } else if(!strcmp(cp1, "RpcHlp")) {        // HELP Remote
    APP_Help ("RemoteControl", "");
      


  //==========================================================
  } else if(!strcmp(cp1, "Clean")) {
    AP_APT_clean ();

	
  //-------------------------------------------------
  } else if(!strcmp(cp1, "ModSiz")) {
    UI_WinToler (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //-------------------------------------------------
  } else if(!strcmp(cp1, "ModPos")) {
    // modify submodel-Position
    UI_Tra__ (-1L, 0, 0L);                   // prim. Init
    UI_GR_Sel_Filter(16);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Toler")) {
    UI_WinToler (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Appr")) {
    UI_WinAppr (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Text")) {
    UI_WinDefTx (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //-------------------------------------------------
  } else if(!strcmp(cp1, "print")) {
    UI_PRI__ (FUNC_EXEC);
    // OS_dll_do ("xa_print__", "PRI__", "abc");
    // UI_WinPrint1 (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //-------------------------------------------------
  } else if(!strcmp(cp1, "doc1")) {
    // Infowindow
    // strcpy(AP_lang, "de");
    // UI_WinInfo1 (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));
    UI_WinInfo2 ();


  //-------------------------------------------------
  } else if(!strcmp(cp1, "doc2")) {
    // disp doku
    // strcpy(AP_lang, "de");
    // sprintf(cbuf1, "%sdoc/gCAD3D_%s.htm", OS_get_bas_dir(), AP_lang);
    sprintf(cbuf1, "%shtml%cindex_%s.htm", OS_get_doc_dir(), fnam_del, AP_lang);
    OS_browse_htm (cbuf1, NULL);

/*
  //-------------------------------------------------
  } else if(!strcmp(cp1, "doc2_old")) {
    // disp doku
    // strcpy(AP_lang, "de");
    // sprintf(cbuf1, "%sdoc/gCAD3D_%s.htm", OS_get_bas_dir(), AP_lang);
    sprintf(cbuf1, "%shtml%cgCAD3D_en.htm", OS_get_doc_dir(), fnam_del);
    OS_browse_htm (cbuf1, NULL);
  

  //-------------------------------------------------
  } else if(!strcmp(cp1, "doc_tra")) { 
    // disp doku
    // strcpy(AP_lang, "de");
    // sprintf(cbuf1, "%sdoc/gCAD3D_%s.htm", OS_get_bas_dir(), AP_lang);
    sprintf(cbuf1, "%shtml%ctransl_en.htm", OS_get_doc_dir(), fnam_del);
    OS_browse_htm (cbuf1, NULL);
*/

  //-------------------------------------------------
  } else if(!strcmp(cp1, "staVWR")) {
    sprintf(cbuf1, "%shtml%cVWR_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    // sprintf(cbuf1, "%sgCAD3D_startVWR_%s.htm",OS_get_doc_dir(),AP_lang);
    OS_browse_htm (cbuf1, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "staMAN")) {
    sprintf(cbuf1, "%shtml%cgCAD3D_startMAN_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "staCAD")) {
    // sprintf(cbuf1, "%shtml%cCAD_examples_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    sprintf(cbuf1, "%shtml%cCAD_using_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "docSearch")) {
    sprintf(cbuf1, "%shtml%cSearch_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "docIact")) {
    sprintf(cbuf1, "%shtml%cCAD_Activ_%s.htm",OS_get_doc_dir(),fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "docTex")) {
    sprintf(cbuf1, "%shtml%cTextures_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "docCatalog")) {
    sprintf(cbuf1, "%shtml%cCatalog_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);

  //-------------------------------------------------
  // } else if(!strcmp(cp1, "docAppli")) {
    // sprintf(cbuf1, "%sAppli_%s.htm",OS_get_doc_dir(),AP_lang);
    // OS_browse_htm (cbuf1);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "docPlugin")) {
    sprintf(cbuf1, "%shtml%cPlugin_%s.htm",OS_get_doc_dir(),
            fnam_del, AP_lang);
    OS_browse_htm (cbuf1, NULL);

  //-------------------------------------------------
  } else if(!strcmp(cp1, "docCTRL")) {
    sprintf(cbuf1, "%shtml%cRemoteControl_%s.htm",OS_get_doc_dir(),
            fnam_del, AP_lang);
    OS_browse_htm (cbuf1, NULL);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "docWC_g")) {
    sprintf(cbuf1, "%shtml%cwcut_de.htm",OS_get_doc_dir(), fnam_del);
    OS_browse_htm (cbuf1, NULL);

  //-------------------------------------------------

  } else if(!strcmp(cp1, "docTransl")) {
    sprintf(cbuf1, "%shtml%ctransl_%s.htm",OS_get_doc_dir(), fnam_del,AP_lang);
    OS_browse_htm (cbuf1, NULL);



  //-------------------------------------------------
  } else if(!strcmp(cp1, "LtypEd")) {
    sprintf(cbuf1, "%sltyp.rc",OS_get_cfg_dir());
    APP_edit (cbuf1, 0);
    DL_InitAttTab ();             // neu einlesen
    ED_Reset (); ED_work_END (0); // redraw


  //-------------------------------------------------
  } else if(!strcmp(cp1, "DirEd")) {
    AP_get_fnam_symDir (cbuf1);   // get filename of dir.lst
    // sprintf(cbuf1, "%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    APP_edit (cbuf1, 0);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "ckb_nam")) {

    APT_dispNam = GUI_menu_checkbox_get (&ckb_nam); //1=checked; 0=not
    // imply END-Button (Redraw)
    // UI_AP (UI_FuncSet, UID_but_END, NULL);  // Stuerzt ab !! ??
    ED_Reset ();
    ED_work_END (0);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "ckb_dir1")) {

    APT_dispDir = GUI_menu_checkbox_get (&ckb_dir); // 1=checked; 0=not
    ED_Reset (); // imply END-Button (Redraw)
    ED_work_END (0);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Redraw")) {

      // imply END-Button (Redraw)
      // UI_AP (UI_FuncSet, UID_but_END, NULL);  // Stuerzt ab !! ??
      ED_Reset ();
      ED_work_END (0);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "mod_cre")) {
    Mod_cre__ ();

  } else if(!strcmp(cp1, "mod_m2s")) {
    Mod_m2s__ ();

  } else if(!strcmp(cp1, "mod_chg")) {
    Mod_chg__ (0);

  } else if(!strcmp(cp1, "mod_sav")) {
    Mod_sav2file__ ();

  } else if(!strcmp(cp1, "mod_loa")) {
    Mod_LoadFile__ ();

  } else if(!strcmp(cp1, "mod_ren")) {
    Mod_ren__ ();

  } else if(!strcmp(cp1, "mod_del")) {
    Mod_del__ ();




  //-------------------------------------------------
  } else if(!strcmp(cp1, "colStd")) {
    irc = GUI_Color_select (&i1, &i2, &i3);
    if(irc < 0) return -1;
    Col_set__ (&col1, i1>>8, i2>>8, i3>>8);
    AP_colSel (&col1);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "colAct")) {
    irc = GUI_Color_select (&i1, &i2, &i3);
    if(irc < 0) return -1;
    Col_set__ (&col1, i1>>8, i2>>8, i3>>8);
    UI_colSel (&col1);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Men_off")) {
    GUI_set_show (&menu_bar, 0);

  } else if(!strcmp(cp1, "Men_on")) {
    GUI_set_show (&menu_bar, 1);



  //-------------------------------------------------
  } else if(!strcmp(cp1, "Bar1")) {
    // hide geht seit GTK2 erst wenn voll initalisiert (nach show ?..)
    // printf(" AP_sysStat=%d\n",AP_sysStat);
    if(AP_stat.sysStat < 2) goto L_exit;
    i1 = GUI_menu_checkbox_get (&ckb_Bar1);
    printf(" Bar1=%d\n",i1);
    if(i1 == 0) GUI_set_show (&ToolBar1, 0);
    else        GUI_set_show (&ToolBar1, 1);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Bar1_off")) {
    GUI_menu_checkbox_set (&ckb_Bar1, 0);
    UI_men__ ("Bar1");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Bar1_on")) {
    GUI_menu_checkbox_set (&ckb_Bar1, 1);
    UI_men__ ("Bar1");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Bar2")) {
      // printf(" AP_sysStat=%d\n",AP_stat.sysStat);
    if(AP_stat.sysStat < 2) goto L_exit;
    i1 = GUI_menu_checkbox_get (&ckb_Bar2);
      // printf(" Bar2=%d\n",i1);
    if(i1 == 0) GUI_set_show (&ToolBar2, ON);   // 0);
    else        GUI_set_show (&ToolBar2, OFF);  // 1);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Comp")) {
    AP_stat.comp = GUI_menu_checkbox_get (&ckb_compl);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Bar2_off")) {
    GUI_menu_checkbox_set (&ckb_Bar2, 0);
    UI_men__ ("Bar2");
  

  //-------------------------------------------------
  } else if(!strcmp(cp1, "Bar2_on")) {
    GUI_menu_checkbox_set (&ckb_Bar2, 1);
    UI_men__ ("Bar2");


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Brw_off")) {  // do not view BrowserWindow
    UI_brw__ (-1);           // off


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Brw_on")) {  // view BrowserWindow
    UI_brw__ (1);            // on


  //-------------------------------------------------
  } else if(!strcmp(cp1, "TUT_ON")) {          // ScreenCast-mode on/off
    i1 = GUI_menu_checkbox_get (&ckb_Tut);
    AP_stat.TUT_stat = i1;           // on


  //-------------------------------------------------
  } else if(!strcmp(cp1, "BRW_OFF")) {  // do not view BrowserWindow
    i1 = GUI_menu_checkbox_get (&ckb_Brw);
    if(i1 == 0) UI_brw__ (1);            // on
    else        UI_brw__ (-1);           // off
    

  //-------------------------------------------------
  } else if(!strcmp(cp1, "RCTL_OFF")) {  // RemoteControl
    i1 = GUI_menu_checkbox_get (&ckb_rctl);
    if(i1 == 0) AP_tmr_CB__ ("RCTL_ON");          // activate
    else        AP_tmr_CB__ ("RCTL_OFF");           // do never activate
  


  //-------------------------------------------------
  } else if(!strcmp(cp1, "ViewP")) {   // Insert ViewPort
    printf(" add ViePortDef\n");
    if(imode == UI_MODE_VWR) {
      TX_Print("nur MAN/CAD");

    } else if(imode == UI_MODE_MAN) {
      strcpy(cbuf1, " \"");
      AP_view_2_txt (&cbuf1[2]);
      strcat(cbuf1, "\"");
      UI_AP (UI_FuncSet, UID_Edit_Line, (void*)cbuf1);

    } else if(imode == UI_MODE_CAD) {
      AP_view_2_txt (cbuf1);
      IE_cad_InpIn_String (cbuf1);
    }



  //-------------------------------------------------
  } else if(!strcmp(cp1, "Edit")) {   // Edit
      printf(" Edit %d err=%d errLn=%d\n",imode,AP_stat.errStat,AP_stat.errLn);


    if(AP_stat.errStat != 0) {
      // handle error
      AP_APT_sysed ();
 
    } else {
      // user-call, normal operation

      if(imode == UI_MODE_VWR) {
        TX_Print("only for MAN or CAD");
  
      } else if(imode == UI_MODE_MAN) {
        ED_sysed__ (1);
  
      } else if(imode == UI_MODE_CAD) {
        // TODO: test if a cad-function is active; yes: edit inpFiled,
        //  else edit sourceline (AP_APT_sysed)
        // AP_APT_sysed ();
        IE_cad_Inp_edit_field ();
      }
    }



  //-------------------------------------------------
  } else if(!strcmp(cp1, "delMdl")) {   // delete Model
/*
    // Liste mit Dir-Auswahl
    AP_get_fnam_symDir (cbuf1);   // get filename of dir.lst
    // sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_List2 ("delete File",    // titletext
            AP_mod_dir,       // Pfadname des activeDirectory
            cbuf1,              // Liste der directories
            (void*)UI_delMdl); // CallBack der Liste
*/
    strcpy(sTit, APP_MSG_get_0("MMfDel"));
    // Liste mit Dir-Auswahl
    i1 = AP_Mod_open (1, cbuf1, cbuf2, sTit, "*");
    if(i1 < 0) return -1;
    AP_open__ (cbuf1, cbuf2, sTit, 1);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "renMdl")) {   // rename Model
/*
    // Liste mit Dir-Auswahl
    AP_get_fnam_symDir (cbuf1);   // get filename of dir.lst
    // sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    GUI_List2 ("rename File",    // titletext
            AP_mod_dir,         // Pfadname des activeDirectory
            cbuf1,               // Liste der directories
            (void*)UI_renMdl);   // CallBack der Liste
*/
    strcpy(sTit, APP_MSG_get_0("MMfRen"));
    // Liste mit Dir-Auswahl
    i1 = AP_Mod_open (1, cbuf1, cbuf2, sTit, "*");
    if(i1 < 0) return -1;
    AP_open__ (cbuf1, cbuf2, sTit, 2);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "cpyMdl")) {   // copy Model
/*
    // Liste mit Dir-Auswahl
    // sprintf(cbuf1,"%sxa%cdir.lst",OS_get_bas_dir(),fnam_del);
    // GUI_List2 ("copy File - select from",      // titletext
            // AP_mod_dir,         // Pfadname des activeDirectory
            // cbuf1,               // Liste der directories
            // (void*)UI_cpyMdl1);  // CallBack der Liste
    strcpy(cbuf1, AP_mod_dir);
    GUI_File_selext ("copy File - select from", cbuf1, UI_cpyMdl1, NULL);
*/
    strcpy(sTit, APP_MSG_get_0("MMfCpy"));
    // Liste mit Dir-Auswahl
    i1 = AP_Mod_open (1, cbuf1, cbuf2, sTit, "*");
    if(i1 < 0) return -1;
    AP_open__ (cbuf1, cbuf2, sTit, 3);


  //======================================================
  } else if(!strcmp(cp1, "ObjDelete")) {
    // if(UI_InpMode != UI_MODE_VWR)
      UNDO_grp_undo ();


  //======================================================
  } else if(!strcmp(cp1, "ObjRestore")) {
    // if(UI_InpMode != UI_MODE_VWR)
      UNDO_grp_redo ();



  //======================================================
  } else if(!strcmp(cp1, "LtypMod")) {   // modify Linetyp
    // printf(" LtypMod\n");
    UI_WinLtypMod (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //======================================================
  } else if(!strcmp(cp1, "LayMod")) {   // modify Layer
    // printf(" LtypMod\n");
    // UI_WinLayMod (NULL, (void*)UI_FuncInit);


  //======================================================
  } else if(!strcmp(cp1, "SstylMod")) {   // apply/modify Linetyp
    // printf(" LtypMod\n");
    UI_WinSurfStyl (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //======================================================
  } else if(!strcmp(cp1, "SCol")) {   // apply/modify Color
    // UI_WinSurfCol (NULL, (void*)UI_FuncInit);
    UI_WinSurfCol (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //======================================================
  } else if(!strcmp(cp1, "STra")) {   // apply/modify transparenz
    UI_WinSurfTra (NULL, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));


  //======================================================
  } else if(!strcmp(cp1, "STex")) {   // apply/modify texture
    UI_Tex__ (NULL, GUI_SETDAT_EI (TYP_EventPress,UI_FuncInit));



  //======================================================
  } else if(!strcmp(cp1, "paste_tx")) {   // paste textBuffer (ctrl del)
    if(UI_InpMode == UI_MODE_MAN) {
      sprintf(cbuf1,"%sselection.txt",OS_get_tmp_dir());
      GUI_edi_InsFile (&winED, cbuf1);
      xa_fl_TxMem = 1;
    }




    //======================================================
    } else if(!strcmp(cp1, "oNam")) {   // objLst
      // if MAN is active, display a List of objecttypes; else
      // if CAD ist active, zeige Liste zum aktuellen Inputfeld passend
      if(UI_InpMode == UI_MODE_VWR) {
        TX_Print("***** active in CAD only ..");
        goto L_exit;
      }
      // check if Search is active
      i1 = GUI_ckbutt_get (&ckb_such); // 0=not sel, 1=selected.
        // printf(" ckb_such-state=%d\n",i1);
      i1 = abs(i1 -1);  // invert
      GUI_ckbutt_set (&ckb_such, i1);

/*
    //======================================================
    } else if(!strcmp(cp1, "oDel")) {   // Delete
      // nur MAN&CAD
      // if CAD ist active, zeige Liste zum aktuellen Inputfeld passend
      if(UI_InpMode == UI_MODE_VWR) {
        TX_Print("***** active in CAD only ..");
        goto L_exit;
      }
      // check if Delete is active
      i1 = GUI_ckbutt_get (&ckb_mdel); // 0=not sel, 1=selected.
          // printf(" ckb_such-state=%d\n",i1);
      i1 = abs(i1 -1);  // invert
      GUI_ckbutt_set (&ckb_mdel, i1);
*/

  //======================================================
  } else if(!strcmp(cp1, "Posi2P")) {   // PT <-- Position
    UI_GR_Sel_Filter (1);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Vert2P")) {   // PT <-- Vertex      UNUSED
    UI_GR_Sel_Filter (2);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Obj2P")) {   // PT <-- Obj
    UI_GR_Sel_Filter (3);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Obj2PP")) {   // PT <-- Obj
    UI_GR_Sel_Filter (18);


  //-------------------------------------------------
  } else if(!strcmp(cp1, "Obj2LN")) {   // PT <-- Obj
    UI_GR_Sel_Filter (19);


  //-------------------------------------------------
  // } else if(!strcmp(cp1, "ObjParent")) {   // parentObj < obj
    // UI_GR_Sel_Filter (19);



  //======================================================
  } else if(!strcmp(cp1, "dump")) {   // dump objData
    // exit active CAD-function (else no selection possible)
    IE_cad_exitFunc ();
    UI_GR_Sel_Filter (4); // "select or keyIn obj to dump .."


  //======================================================
  } else if(!strcmp(cp1, "vars")) {
    UI_dump__ (Typ_VAR);


  //======================================================
  } else if(!strcmp(cp1, "points")) {
    UI_dump__ (Typ_PT);


  //======================================================
  } else if(!strcmp(cp1, "vecs")) {
    UI_dump__ (Typ_VC);


  //======================================================
  } else if(!strcmp(cp1, "tras")) {
    UI_dump__ (Typ_Tra);


  //======================================================
  } else if(!strcmp(cp1, "ints")) {
    UI_dump__ (Typ_Activ);


  //======================================================
  } else if(!strcmp(cp1, "texs")) {
    UI_dump__ (Typ_TEXR);


  //======================================================
  } else if(!strcmp(cp1, "dump_ga")) {
    sprintf(cbuf1, "%stmp.html",OS_get_tmp_dir());
    UTX_htm_fop (&fpo, cbuf1);    // open
    GA_dump__ (fpo);
    UTX_htm_fcl (&fpo);                // close
    OS_browse_htm (cbuf1, NULL);


  //======================================================
  } else if(!strcmp(cp1, "mods")) {
    // UI_dump__ (Typ_Model);

    sprintf(cbuf1, "%stmp.html",OS_get_tmp_dir());
    UT3D_stru_dump (TYP_FuncInit, (void*)"htm", cbuf1);

    // il1 = DB_dbo_get_free (Typ_Model);
    sprintf(cbuf2, "\nReference models:\n"); // APT_MR_IND
    UT3D_stru_dump(Typ_Txt, cbuf2, "");
    DB_dump_ModRef ();

    // i1 = DB_get_ModBasNr ();
    sprintf(cbuf2, "\nBasic models:\n");  // DYN_MB_IND
    UT3D_stru_dump(Typ_Txt, cbuf2, "");
    DB_dump_ModBas ();

    UT3D_stru_dump (TYP_FuncEnd, (void*)"htm", "");
    OS_browse_htm (cbuf1, NULL);



  //======================================================
  } else if(!strcmp(cp1, "dumpGrp")) {

    sprintf(cbuf1, "%stmp.html",OS_get_tmp_dir());
    UT3D_stru_dump (TYP_FuncInit, (void*)"htm", cbuf1);

    Grp_dump ();

    UT3D_stru_dump (TYP_FuncEnd, (void*)"htm", "");
    OS_browse_htm (cbuf1, NULL);



  //======================================================
  } else if(!strcmp(cp1, "dumpStd")) {     // dump standards

    sprintf(cbuf1, "%stmp.html",OS_get_tmp_dir());
    UT3D_stru_dump (TYP_FuncInit, (void*)"htm", cbuf1);
    
    UT3D_dump_txt("Date:          %s",OS_date1());
    UT3D_dump_txt("User:          %s",OS_get_user());
    UT3D_dump_txt("Language:      %s",AP_lang);
    UT3D_dump_txt("Basedirectory: %s",OS_get_bas_dir());
    UT3D_dump_txt("Tempdirectory: %s",OS_get_tmp_dir());
    UT3D_dump_txt("Bin.directory: %s",OS_get_bin_dir());
    UT3D_dump_txt("Model:         %s",AP_mod_fnam);
    UT3D_dump_txt("Modelsize:     %f",APT_ModSiz);
    UT3D_dump_txt("Tol.Points:    %f",UT_TOL_pt);
    UT3D_dump_txt("Tol.Curves:    %f",UT_TOL_cv);
    UT3D_dump_txt("Tol.Display:   %f",UT_DISP_cv);
    UT3D_dump_txt("Size-Text:     %f",AP_txsiz);
    UT3D_dump_txt("Size-Dim.:     %f",AP_txdimsiz);
    UT3D_dump_txt("Scale-Text:    %f",GR_tx_scale);
    UT3D_dump_txt("Browser:       %s",OS_get_browse_htm());
    UT3D_dump_txt("Printer:       %s",OS_get_printer());
    UT3D_dump_txt("Editor:        %s",OS_get_edi());
    UT3D_dump_txt("Terminal:      %s",OS_get_term());


    // dump stdCol AP_defcol
    UT3D_stru_dump (Typ_Color, &AP_defcol, "DefaultColor:  ");
    UT3D_stru_dump (Typ_Color, &AP_actcol, "Active-Color:  ");

    AP_get_fnam_symDir (cbuf2);
    UT3D_dump_txt ("\nSymbolicDirectories (%s):\n",cbuf2);
    UT3D_stru_dump (TYP_FuncAdd, cbuf2, "");    // put out the file 

    UT3D_stru_dump (TYP_FuncEnd, (void*)"htm", "");
    OS_browse_htm (cbuf1, NULL);



  //======================================================
  } else if(!strcmp(cp1, "defBrowser")) {
    UI_def_browser ();
    // GUI_GetText1 ("define Browser", "Html-Browser:",
                  // AP_browser, -180, UI_def_browser);


  //======================================================
  } else if(!strcmp(cp1, "defEditor")) {
    UI_def_editor ();
    // GUI_GetText1 ("define Editor", "Texteditor:",
                  // AP_editor, -180, UI_def_editor);


  //======================================================
  } else if(!strcmp(cp1, "dumpSrc")) {
    SRC_dump__ (0);


  //======================================================
  } else if(!strcmp(cp1, "CtlgMod")) {
    CTLG_Mod__();

  } else if(!strcmp(cp1, "CtlgCre")) {
    CTLG_Cre__();

  } else if(!strcmp(cp1, "CtlgDel")) {
    CTLG_Del__();

  } else if(!strcmp(cp1, "CtlgHlp")) {
    APP_Help ("Catalog", "");


  //======================================================
  } else if(!strcmp(cp1, "logfile")) {
    LOG_A_disp ();   // view logfile


  //======================================================
  } else if(!strcmp(cp1, "abort")) {
    AP_work__ ("crashEx", NULL);


  //======================================================
  } else if(!strcmp(cp1, "uninst")) {
    TX_Print("exit gCAD3D; in Filebrowser start %sUninstall",
             OS_get_bas_dir());


  //======================================================
  } else if(!strcmp(cp1, "DispList")) {
    DL_DumpObjTab ();


  //-------------------------------------------------
  } else {
    TX_Error("UI_menCB E001 %s",cp1);
  }




  // Fertig:
  /* Reset focus to glarea widget */
  /* UI_GR_setKeyFocus (); */

  L_exit:
  return 0;
}


/*
//================================================================
  int UI_DllLst_read () {
//================================================================
// fill list of userprograms (DLL's) -> memspc55

  int   i1, i2, ftyplen;
  char  *cBuf, **pBuf, cbuf1[256];
  FILE  *fpi;


  pBuf = (char**)memspc55;
  cBuf = memspc54;

  cBuf[0] = '\0';

  // Liste aller userprograms
#ifdef _MSC_VER
  // sprintf(cbuf1, "dir/b \"%sxa\\*.dll\" > \"%stmp\\Dll.lst\"",
                  // OS_get_bas_dir(),OS_get_bas_dir());
  ftyplen = 4; // ".dll"
#else
  // sprintf(cbuf1, "cd %sxa;ls -1 *.so > %stmp/Dll.lst",
                  // OS_get_bas_dir(),OS_get_bas_dir());
  ftyplen = 3; // ".so"
#endif

  // printf("%s\n",cbuf1);
  // OS_system(cbuf1);


  sprintf(cbuf1, "%sDll.lst",OS_get_tmp_dir());
    printf(" read Dll.lst |%s|\n",cbuf1);
  if ((fpi = fopen (cbuf1, "r")) == NULL) { return -1; }


  // einlesen ..
  i2 = 0;
  while(UTX_fgets(cbuf1, 80, fpi)) {
    printf("%s\n",cbuf1);
    cbuf1[strlen(cbuf1)-ftyplen] = '\0'; // remove .so /.dll

    i1 = strlen(cbuf1);

    // if(!strncmp(cbuf1, "xa_", 3)) continue;

    strncpy(cBuf, cbuf1, i1);
    cBuf[i1] = '\0';
    pBuf[i2] = cBuf;
    ++i2;
    cBuf += i1+1;
  }

  fclose(fpi);

  pBuf[i2] = NULL;

  return 0;

}
*/
/*
//===================================================================
  int UI_DllLst_CB (MemObj *mo, void **data) {
//===================================================================
// userselection of DLL ..


  int  i1, lNr;
  char cbuf1[256], prgNam[256];


  // // check if WC_GEO is activ
  // UI_AP (UI_FuncGet, UID_WinWCGEO, (void*)&i1);
  // if(i1 == 0) return 0;


  // lNr = INT_PTR(data )+ 1;
  lNr = GUI_DATA_I1;
  strcpy (prgNam, GUI_DATA_S2);

  printf("UI_DllLst_CB |%d|\n",lNr);

  // // Line #  lNr aus Datei tmp/Dll.lst einlesen
  // sprintf(cbuf1, "%sDll.lst",OS_get_tmp_dir());
  // UTX_fgetLine (memspc011, 256, cbuf1, lNr);


  sprintf(cbuf1, "Execute Userprog. %s",prgNam);
  TX_Print(cbuf1);

  UTX_ftyp_cut  (prgNam);     // remove the filetyp (.so|.dll)
  AP_exec_dll (prgNam);
  
  return 0;

}
*/

//================================================================
  int UI_paned_CB (void *parent, int pgnr) {
//================================================================
// pgnr   0=browse; 1=edit
 
  // printf("UI_paned_CB %d\n",pgnr);


  return 0;

}


/*
//================================================================
  int UI_cbKey_win_main (MemObj *o_par, void **data) {
//================================================================

  printf("UI_cbKey_win_main \n");

  return 0;

}
*/

 
//================================================================
  int UI_win_main (MemObj *mo, void **data) {
//================================================================
/*
Das gesamte Window-Layout

box0
(ueber alles)

menu_bar

box1h

winED                         winGR
(Editorfenster)               (Grafikfenster)

Box3
(Textausgabefenster)


Toolbar1
box1

00
  |-----------------------------------------------------------------|
  |1                                                                |
  |                                                                 |
  |-----|---------|-------------------------------------------------|
  |2    |         |                                                 |
  |2A   |2B       |                                                 |
  |     |2B1      |                                                 |
  |     |2B1A     |2B1B                                             |
  |     |         |                                                 |
  |     |         |                                                 |
  |     |         |                                                 |
  |     |         |                                                 |
  |     |         |                                                 |
  |     |         |                                                 |
  |     |---------|-------------------------------------------------|
  |     |2B2                                                        |
  |     |                                                           |
  |     |                                                           |
  |-----------------------------------------------------------------|




*/

  MemObj    actbox, actwi,
            wtmp1, wtmp2, wtmp3, wtmp4, wtmp7, wtmp8;

  // static GIO_OptMen wom1;
  static MemObj hpaned;
  static MemObj frm_act;

  int    irc, i1, i2, idat;
  long   l1;
  char   cbuf1[256], *cp1;
  // char   *surLst[]={"XY","XZ","YZ",NULL};


// #define LNG_MAX_NR 16
//   typedef  char lngCode[4];
//   typedef  char lngName[40];
//   static lngCode    TabLngCode[LNG_MAX_NR];
//   lngName    *TabLngName;
//   int        lngNr;






  /* -------------------------------------------------- */
  idat = GUI_DATA_I1;
    printf("UI_win_main %d\n",idat);


  switch (idat) {


    /* Init Panel -------------------------- */
    case UI_FuncInit:

      // // Read Colours und Fonts
      // // GUI_Init ("xa/xa.rc");
      // GUI_Init ("");


/*  dzt unused ..
      // define colors fonts ....
      colbg.red   =0xc000; // c zu dunkel was 0xfe00
      colbg.green =0xc000;
      colbg.blue  =0xc000;

      // f. Hintergrund Edit-Fenster
      // IE_styl_mod = gtk_style_copy (GTK_WIDGET(IE_but_OK)->style);
      UI_styl_0 = gtk_widget_get_default_style ();
      UI_styl_1 = gtk_style_copy (UI_styl_0);
      UI_styl_1->base[0] = colbg; // normal
*/



      // Create Mainwindow
      winMain = GUI_Win__ (NULL, UI_win_main, "-1000,-690");
      // GUI_obj_init (); // TEMP_ONLY - remove with GUI_Win__


      // GUI_Win_ev_key (&winMain, UI_cbKey_win_main);



      // vbox as primary box
      box00 = GUI_box_v (&winMain, "");


      // Create a menu-bar to hold the menus
      // menu_bar = GUI_MenuBar (box00);
      menu_bar = GUI_menu__ (&box00, NULL, 0);               // create Menubar



      // ----------- Menubar ---------------------------------------
      men_fil = GUI_menu__ (&menu_bar, "File", 0);
      men_mod = GUI_menu__ (&menu_bar, "subModels", 0);
      men_opt = GUI_menu__ (&menu_bar, "Options", 0);
      wtmp1   = GUI_menu__ (&menu_bar, "Display", 0);
      men_mdf = GUI_menu__ (&menu_bar, "Modify", 0);
      men_ins = GUI_menu__ (&menu_bar, "Insert", 0);
      men_sel = GUI_menu__ (&menu_bar, "Select", 0);
      wtmp8   = GUI_menu__ (&menu_bar, "Dump", 0);
      wtmp7   = GUI_menu__ (&menu_bar, "Group", 0);
      wtmp4   = GUI_menu__ (&menu_bar, "Standards", 0);
      men_cat = GUI_menu__ (&menu_bar, "Catalog", 0);
      men_app = GUI_menu__ (&menu_bar, "Application", 0);
      men_prc = GUI_menu__ (&menu_bar, "Process", 0);
      men_plu = GUI_menu__ (&menu_bar, "Plugin", 0);
      men_rpc = GUI_menu__ (&menu_bar, "Remote", 0);
      men_hlp = GUI_menu__ (&menu_bar, "Help", 'r');   //'r' problem on Gtk3



      //----------------------------------------------------------------
      // Entries von "File"
      UIw_file_new=GUI_menu_entry(&men_fil,"New",   UI_menCB,   (void*)"new");
      MSG_Tip ("MMclear"); // (" clear modelspace");

      UIw_file_ope=GUI_menu_entry(&men_fil,"Open Model",UI_menCB,(void*)"open");
      cp1 = MSG_const__(MSG_open);
      if(cp1) strcpy(cbuf1, cp1);
      strcat(cbuf1, " - GCAD, STP/STEP, IGS/IGE/IG2, DXF, 3DS, STL, LWO, WRL(VRML1), OBJ(WaveFront), TESS, BMP, JPG");
      GUI_Tip (cbuf1);

      GUI_menu_entry (&men_fil,"Open last-used",UI_open_last,NULL);
      // wtmp2 = GUI_menu__ (&men_fil, " last-used", 's');
      // UI_addLast (&wtmp2);


      // UIw_file_opD=GUI_menu_entry(men_fil,"Open Model new Dir",
                                 // UI_menCB,(void*)"opeDir");
      // GUI_Tip  ("Load from new system-directory.");


      // 2010-01-02
      // UIw_file_ins=GUI_menu_entry(men_fil,"Insert",UI_menCB,(void*)"insert");
      // GUI_Tip  (" add Model ");


      GUI_menu_entry   (&men_fil, "---",     NULL,       NULL);
      ckb_impnat = GUI_menu_checkbox__ (&men_fil,
                                    "import as Model or Mockup",
                                        0, NULL, NULL);
        // GUI_menu_checkbox_set (GTK_CHECK_MENU_ITEM(ckb_impnat),TRUE);
      MSG_Tip ("MMmock"); //ON: imports Mockup-Models native, else Mockup (OBJ)

      // GUI_menu_entry   (men_fil, "Import PointGroup from File",UI_menCB,(void*)"opePtab");
      // GUI_Tip  ("imports GIS-points from Ascii-File");

/*
      men_imp=GUI_Menu(men_fil, "Import Model", 's');
      GUI_menu_entry   (men_fil, "Import Mockup",UI_menCB,(void*)"loadMock");
        GUI_Tip  (" load (add) tesselated Model Typ VRML/OBJ/TESS");
      GUI_menu_entry   (men_fil, "Import Bitmap",UI_menCB,(void*)"loadBmp");
        GUI_Tip  (" load (add) Bitmap Typ BMP(uncompressed)");
*/

      GUI_menu_entry   (&men_fil, "---",     NULL,       NULL);
      UIw_file_sav1=GUI_menu_entry(&men_fil,"save Model        (Ctrl s)",
                                  UI_menCB,(void*)"save");
      MSG_Tip ("MMsave"); //" save native"

      // UIw_file_sav2=GUI_Menu(men_fil,"save Model as",UI_menCB,(void*)"saveAs");
      men_exp1 = GUI_menu__ (&men_fil,"save Model as", 's');
        // GUI_Tip  (" save native, DXF, IGS, WRL(VRML1), OBJ, TESS");

      GUI_menu_entry (&men_fil, "save active Group",
                                 UI_menCB, (void*)"grpSav");
      MSG_Tip ("MMgrSav");
      // men_exp2=GUI_Menu(men_fil,"save Model new Dir", 's');

/*
      men_exp=GUI_Menu(men_fil, "save Model as", 's');
        GUI_Tip  (" save Model as DXF IGES OBJ NC-ISO");

      GUI_menu_entry   (men_fil, "save Mockup as",UI_menCB,(void*)"saveMock");
        GUI_Tip  (" save tesselated as VRML / OBJ / TESS");
*/


      GUI_menu_entry   (&men_fil, "---",     NULL,       NULL);
      ckb_compr = GUI_menu_checkbox__ (&men_fil, "compress gcad-Models",
                                       0, NULL, NULL);
        // GUI_menu_checkbox_set (GTK_CHECK_MENU_ITEM(ckb_impnat),TRUE);
      MSG_Tip ("MMcomp");
      // GUI_Tip  ("compress native Models (.gcad) with gzip (.gcaz)");



      GUI_menu_entry   (&men_fil, "---",     NULL,       NULL);
      GUI_menu_entry   (&men_fil, "delete File",UI_menCB, (void*)"delMdl");
      MSG_Tip ("MMfDel");
      GUI_menu_entry   (&men_fil, "rename File",UI_menCB, (void*)"renMdl");
      MSG_Tip ("MMfRen");
      GUI_menu_entry   (&men_fil, "copy File",UI_menCB, (void*)"cpyMdl");
      MSG_Tip ("MMfCpy");

      GUI_menu_entry   (&men_fil, "Export / Print",   UI_menCB,   (void*)"print");
      MSG_Tip ("MMfPrt");

      GUI_menu_entry   (&men_fil, "Abort (do not save  Alt-X)",
                        UI_menCB,(void*)"abort");

      GUI_menu_entry   (&men_fil, "Exit",    UI_win_main,(void*)&GUI_FuncKill);
      MSG_Tip ("MMexit");

/*
      // Submenu File/LoadMock
      GUI_menu_entry   (men_imp, "OBJ",     UI_menCB,   (void*)"impDxf");
      GUI_menu_entry   (men_imp, "TESS",    UI_menCB,   (void*)"impIge");


      // Submenu File/Import
      GUI_menu_entry   (men_imp, "DXF",     UI_menCB,   (void*)"impDxf");
      GUI_menu_entry   (men_imp, "IGES",    UI_menCB,   (void*)"impIge");
      GUI_menu_entry   (men_imp, "LWO",     UI_menCB,   (void*)"implwo");
      GUI_menu_entry   (men_imp, "3DS",     UI_menCB,   (void*)"imp3ds");
      GUI_menu_entry   (men_imp, "OBJ",     UI_menCB,   (void*)"impobj");
*/


      // Submenu File/Export
      GUI_menu_entry (&men_exp1, "gcad",    UI_menCB,   (void*)"exp1nat");
      GUI_menu_entry (&men_exp1, "DXF",     UI_menCB,   (void*)"expDxf__");
      GUI_menu_entry (&men_exp1, "DXF-R10", UI_menCB,   (void*)"expDxfR10");
      GUI_menu_entry (&men_exp1, "DXF-2000",UI_menCB,   (void*)"expDxf2000");
      GUI_menu_entry (&men_exp1, "IGES",    UI_menCB,   (void*)"exp1Ige");
      GUI_menu_entry (&men_exp1, "STEP",    UI_menCB,   (void*)"expStp");
      GUI_menu_entry (&men_exp1, "VRML1",   UI_menCB,   (void*)"exp1Wrl1");
      GUI_menu_entry (&men_exp1, "VRML2",   UI_menCB,   (void*)"exp1Wrl2");
      GUI_menu_entry (&men_exp1, "SVG",     UI_menCB,   (void*)"expSVG");
      GUI_menu_entry (&men_exp1, "STL",     UI_menCB,   (void*)"exp1Stl");
      GUI_menu_entry (&men_exp1, "OBJ",     UI_menCB,   (void*)"exp1Obj");
      GUI_menu_entry (&men_exp1, "TESS",    UI_menCB,   (void*)"exp1Tess");
      // GUI_menu_entry (&men_exp1, "JPG",     UI_menCB,   (void*)"exp1Jpg");
      // GUI_menu_entry (&men_exp1, "NC-ISO",  UI_menCB,   (void*)"expISO");
      // GUI_menu_entry (men_exp, "Charmilles Robofil",UI_menCB,(void*)"ppCha1");
      // GUI_menu_entry (men_exp, "Fanuc",   UI_menCB,   (void*)"ppFan1");
      // GUI_menu_entry (men_exp, "Maho",    UI_menCB,   (void*)"ppMah1");



      // Entries von "Models"
      GUI_menu_entry(&men_mod,"activate submodel", UI_menCB, (void*)"mod_chg");
        MSG_Tip ("MMsma"); //("anderes Submodel aktivieren");

      GUI_menu_entry(&men_mod,"move main --> new submodel",
                     UI_menCB,(void*)"mod_m2s");
        MSG_Tip ("MMsmcp"); //("Mainmodel in ein neues Submodel kopieren");

      GUI_menu_entry(&men_mod,"move Group --> new submodel",
                     UI_menCB,(void*)"grpSM");
        // MSG_Tip ("MMsmcp"); //("Mainmodel in ein neues Submodel kopieren");

      GUI_menu_entry(&men_mod,"create new submodel", UI_menCB, (void*)"mod_cre");
        MSG_Tip ("MMsmcr"); //("ein neues Submodel generieren");

      GUI_menu_entry(&men_mod,"save Submodel to File",UI_menCB, (void*)"mod_sav");
        MSG_Tip ("MMsmsf"); //("das aktuelle Submodel in Datei speichern");

      GUI_menu_entry(&men_mod,"load Submodel from File",
                     UI_menCB, (void*)"mod_loa");
        MSG_Tip ("MMsmlf"); //("Model aus Datei als Submodel laden");

      GUI_menu_entry(&men_mod,"rename Submodel", UI_menCB, (void*)"mod_ren");
        MSG_Tip ("MMsmre"); //("das aktuelle Submodel umbenennen");

      GUI_menu_entry(&men_mod,"delete Submodel", UI_menCB, (void*)"mod_del");
        MSG_Tip ("MMsmde"); //("das aktuelle Submodel loeschen");

      GUI_menu_entry(&men_mod,"modify Position of subModel",
                     UI_menCB, (void*)"ModPos");
        MSG_Tip ("MMsmmp");
        // GUI_Tip  ("modify the Position of a subModel");




      //----------------------------------------------------------------
      // Entries von "Options"
      ckb_compl = GUI_menu_checkbox__ (&men_opt, "compile DLLs", 0,
                                 UI_menCB, (void*)"Comp");
        MSG_Tip ("MMopDll");

      // ckb_Iact = GUI_menu_checkbox__ (&men_opt, "Interaction", 0, NULL, NULL);
        // MSG_Tip ("MMopInt");
      // GUI_menu_checkbox_set (&ckb_Iact, TRUE);

      ckb_Bar1 = GUI_menu_checkbox__ (&men_opt, "MainToolBar", 1,
                                UI_menCB, (void*)"Bar1");
        MSG_Tip ("MMopTb1");
      // GUI_menu_checkbox_set (&ckb_Bar1, TRUE);

      ckb_Bar2 = GUI_menu_checkbox__ (&men_opt,"MessageToolBar", 1,
                                UI_menCB, (void*)"Bar2");
        MSG_Tip ("MMopTb2");
      // GUI_menu_checkbox_set (&ckb_Bar2, TRUE);

      ckb_Tut =  GUI_menu_checkbox__ (&men_opt,"ScreenCast ON", 0,
                                UI_menCB, (void*)"TUT_ON");
        // MSG_Tip ("MMopBrw");

      ckb_Brw =  GUI_menu_checkbox__ (&men_opt,"BrowserWindow OFF", 0,
                                UI_menCB, (void*)"BRW_OFF");
        MSG_Tip ("MMopBrw");

      ckb_rctl =  GUI_menu_checkbox__ (&men_opt,"RemoteControl OFF", 0,
                                UI_menCB, (void*)"RCTL_OFF");

      GUI_menu_entry (&men_opt, "Uninstall",UI_menCB, (void*)"uninst");


      //----------------------------------------------------------------
      // Entries von "Display"
      GUI_menu_entry   (&wtmp1, "Scale All",UI_butCB,    (void*)"butRes");
      GUI_menu_entry   (&wtmp1, "Top",      UI_butCB,    (void*)"butTop");
      GUI_menu_entry   (&wtmp1, "---",     NULL,       NULL);


      // GUI_menu_entry   (&wtmp1, "Save",     UI_viewCB,  (void*)"Save");
      // MSG_Tip ("MMdsps"); //

      // GUI_menu_entry   (&wtmp1, "Save&Restore",UI_viewCB,  (void*)"SavRes");
      // MSG_Tip ("MMdspsr"); //

      // GUI_menu_entry   (&wtmp1, "Restore",  UI_viewCB,  (void*)"Restore");
      // MSG_Tip ("MMdspr"); //


      ckb_nam = GUI_menu_checkbox__ (&wtmp1, "ObjNames ON", 0,
                                     UI_menCB, (void*)"ckb_nam");
      MSG_Tip ("MMdspon"); //
      // GUI_Tip  ("Darstellung aller Objektbezeichnungen und "
                // "der Bearbeitungsrichtungen");

      ckb_dir = GUI_menu_checkbox__ (&wtmp1, "ObjDirection ON", 0,
                                     UI_menCB, (void*)"ckb_dir1");
      MSG_Tip ("MMdspdir"); // display direction of curves


      // ckb_txt = GUI_menu_checkbox__ (&wtmp1, "NC-Text", 0,
                                     // UI_menCB, (void*)"Redraw");
      // MSG_Tip ("MMdspnc"); //
      // GUI_Tip  ("Darstellung aller NC-Hilfsfunktionen (als Text) an "
                // "den jeweiligen Positionen.");

      ckb_ptDisp = GUI_menu_checkbox__ (&wtmp1, "PointDisplay OFF", 0,
                                    UI_viewCB, (void*)"PT_OFF");
      MSG_Tip ("MMdsppt"); //
      // GUI_Tip  ("Darstellung Punkte EIN / AUS");

      ckb_plDisp = GUI_menu_checkbox__ (&wtmp1, "PlanesDisplay OFF", 0,
                                    UI_viewCB, (void*)"PL_OFF");
      MSG_Tip ("MMdsppl"); //
      // GUI_Tip  ("Darstellung Ebenen (R) EIN / AUS");

      GUI_menu_checkbox__ (&wtmp1, "SolidDisplay OFF", 0,
                           UI_viewCB, (void*)"SOL_OFF");
      MSG_Tip ("MMdspso"); //
      // GUI_Tip  ("Darstellung Solids (Sphere/Cone/Torus/Cube) als Solid "
                // "oder Drahtmodell.");

      ckb_bound = GUI_menu_checkbox__ (&wtmp1, "Boundary", 0, NULL, NULL);
        MSG_Tip ("MMopBnd");
      // GUI_Tip  ("Checkbox fuer mit od. ohne Kantenlinien");

      ckb_light = GUI_menu_checkbox__ (&wtmp1,"Extra Light",0,UI_wireCB,NULL);

      ckb_backW = GUI_menu_checkbox__ (&wtmp1,"BackGrd white",0,UI_wireCB,NULL);


      //----------------------------------------------------------------
      // Entries von "Standards"
      GUI_menu_entry   (&wtmp4, "Tolerances", UI_menCB,  (void*)"Toler");
      MSG_Tip ("MMstTol"); //
        // GUI_Tip  ("define Modelsize and Tolerances");

      GUI_menu_entry   (&wtmp4, "Text", UI_menCB,  (void*)"Text");
      MSG_Tip ("MMstTxl"); //
        // GUI_Tip  ("define Textsize for Notes and Dimensions");

      // GUI_menu_entry   (wtmp4, "Color", GUI_ColSel,  AP_colSel);
      GUI_menu_entry   (&wtmp4, "Color", UI_menCB,  (void*)"colStd");
      MSG_Tip ("MMstCol"); //
        // GUI_Tip  ("define Default-Modelcolor");

      GUI_menu_entry   (&wtmp4, "Linetypes", UI_menCB,  (void*)"LtypEd");
      MSG_Tip ("MMstLty"); //
        // GUI_Tip  ( "define Linetypes (Color-Lintyp-LineThick) "
                   // "for Lines/Circs/Curves");

      GUI_menu_entry   (&wtmp4, "Directories", UI_menCB,  (void*)"DirEd");
      MSG_Tip ("MMstDir"); //
        // GUI_Tip  ("define Modeldirectories");

      GUI_menu_entry   (&wtmp4, "Browser", UI_menCB,  (void*)"defBrowser");
      MSG_Tip ("MMstBrw"); //
        // GUI_Tip  ("define Browser for documentation");

      GUI_menu_entry   (&wtmp4, "Editor", UI_menCB,  (void*)"defEditor");
      MSG_Tip ("MMstEdi"); //
        // GUI_Tip  ("define Editor");


      //----------------------------------------------------------------
      // Entries von "Modify"
      GUI_menu_entry   (&men_mdf, "modelSize",  UI_menCB,  (void*)"ModSiz");
      MSG_Tip ("MMmdMsi"); //

      GUI_menu_entry   (&men_mdf, "submodelPosition",  UI_menCB,  (void*)"ModPos");
      MSG_Tip ("MMmdMdp"); //
        // GUI_Tip  ("modify the Position of a subModel");

      GUI_menu_entry   (&men_mdf, "Curve->Circ", UI_menCB,  (void*)"Appr");
      MSG_Tip ("MMmdCvC"); //
        // GUI_Tip  ("Kurven in Kreise umwandeln");

      // GUI_menu_entry   (men_mdf, "act.Line <-> Editor",UI_menCB,(void*)"EdLn");


      GUI_menu_entry   (&men_mdf, "---", NULL, NULL); //------------------
 
      GUI_menu_entry   (&men_mdf, "Linetyp",  UI_menCB,  (void*)"LtypMod");
      MSG_Tip ("MMmdLty"); //
        // GUI_Tip  ("change Linetyp (Colour, Linetyp, LineThick) "
                  // "of Lines/Circs/Curves");

      // GUI_menu_entry   (men_mdf, "Layer",  UI_menCB,  (void*)"LayMod");
        // GUI_Tip  ("change LayerNumber of object ..");
      GUI_menu_entry   (&men_mdf, "SurfaceStyle",UI_menCB,(void*)"SstylMod");
      MSG_Tip ("MMmdSst"); //
        // GUI_Tip  ("change SurfaceStyle to shaded or symbolic (wireframe)");

      GUI_menu_entry   (&men_mdf, "SurfaceColor",UI_menCB,(void*)"SCol");
      MSG_Tip ("MMmdSco"); //
        // GUI_Tip  ("change Color of Surface to active Color) ");

      GUI_menu_entry   (&men_mdf, "SurfaceTranspar",UI_menCB,(void*)"STra");
      MSG_Tip ("MMmdpsr"); //
        // GUI_Tip  ("make Surface transparent");

      GUI_menu_entry   (&men_mdf, "SurfaceTexture",UI_menCB,(void*)"STex");
      MSG_Tip ("MMmdStx"); //
        // GUI_Tip  ("make textured surface");

        GUI_menu_entry   (&men_mdf, "ObjectNames",UI_menCB,(void*)"oNam");
      MSG_Tip ("MMmdOnm"); //
          // GUI_Tip  ("display a list of objects");

        // GUI_menu_entry   (&men_mdf, "Delete",UI_menCB,(void*)"oDel");
      // MSG_Tip ("MMmdDel"); // GUI_Tip  ("delete objects");


      GUI_menu_entry (&men_mdf, "---", NULL, NULL); //------------------------

      GUI_menu_entry (&men_mdf, "Edit  (F3)",UI_menCB,
                 (void*)"Edit");
      MSG_Tip ("MMmdEdi"); //
        // GUI_Tip  ("modify active line/field with systemEditor / F3");

      GUI_menu_entry(&men_mdf,"APT-Clean  (F4)",UI_menCB,
                  (void*)"Clean");
      MSG_Tip ("MMmdCln"); //
        // GUI_Tip  ("remove all deleted Lines permanent / F4");

      GUI_menu_entry(&men_mdf,"Delete last Obj  (Ctrl-Z)",UI_menCB,
                  (void*)"ObjDelete");
      MSG_Tip ("MMundo"); //
        // GUI_Tip  ("zuletzt erzeugtes Objekt loeschen");

      GUI_menu_entry(&men_mdf,"Restore last deleted  (Ctrl-Y)",UI_menCB,
                  (void*)"ObjRestore");
      MSG_Tip ("MMredo"); //
        // GUI_Tip  ("zuletzt geloeschtes Objekt wiederherstellen");




      //----------------------------------------------------------------
      // Entries von "Insert"
      // GUI_menu_entry   (men_ins, "Position",  UI_menCB,  (void*)"Posi");
      GUI_menu_entry   (&men_ins, "ViewPort",  UI_menCB,  (void*)"ViewP");
      MSG_Tip ("MMinVpt"); //
        // GUI_Tip  ("give ViewportParameters (into open Interaction)");

      // GUI_menu_entry   (men_ins, "VertexPos", UI_menCB,  (void*)"VxPos");
      GUI_menu_entry   (&men_ins, "paste textBuffer (ctrl del)",
                       UI_menCB,(void*)"paste_tx");
      MSG_Tip ("MMinTxt"); //
        // GUI_Tip  ("MAN: paste textBuffer after delete or ctrl X");



      //----------------------------------------------------------------
      // Entries von "Select"
      GUI_menu_entry   (&men_sel, "get point = position of cursor on construction-plane",
                       UI_menCB,(void*)"Posi2P");
      MSG_Tip ("MMsePcp"); //
        // GUI_Tip  ("give Coordinates of cursor on construction-plane at next selection");

      // GUI_menu_entry   (&men_sel, "get position of cursor on selected object",
                       // UI_menCB,  (void*)"Obj2P");
      // MSG_Tip ("MMsePso"); //
        // GUI_Tip  ("give Coordinates of cursor on selected object at next selection");

      GUI_menu_entry   (&men_sel, "get point = (parametric) position of cursor on object",
                       UI_menCB,  (void*)"Obj2PP");
      MSG_Tip ("MMsePop"); //
        // GUI_Tip  ("give parametric point of cursor on selected object at next selection");

      GUI_menu_entry   (&men_sel, "get line = segment of polygon / contour",
                       UI_menCB,  (void*)"Obj2LN");
      MSG_Tip ("MMseLoO"); //
        // GUI_Tip  ("give parametric point of cursor on selected object at next selection");



      // GUI_menu_entry(&men_sel,"get parentObject",UI_menCB,(void*)"ObjParent");
      // MSG_Tip ("MMseOop"); //
        // GUI_Tip  ("give parent-Object of selected object at next selection");

      // GUI_menu_entry   (men_sel, "get position of nearest vertex",
                       // UI_menCB,  (void*)"Vert2P");
        // GUI_Tip  ("give Coordinates of vertex nearest to cursor at next selection");

/*
      GUI_menu_entry   (men_sel, "PT <-- ObjectPoint",UI_menCB,(void*)"Obj2P");
        GUI_Tip  ("give nearest definitionPoint of object at next selection");
      GUI_menu_entry   (men_sel, "PT <-- Vertex",   UI_menCB,  (void*)"Vert2P");
        GUI_Tip  ("give VertexCoordinates at next selection");
      GUI_menu_entry   (men_sel, "PT <-- Position", UI_menCB,  (void*)"Posi2P");
        GUI_Tip  ("give CursorCoordinates at next selection");
*/
      // GUI_menu_entry   (&men_sel, "---",     NULL,       NULL);
      // GUI_menu_entry   (&men_sel, "save selected obj --> file",
                       // UI_menCB, (void*)"savSel");
      // MSG_Tip ("MMseSof"); //
        // GUI_Tip  ("save native data of the selected object --> file");

      GUI_menu_entry (&men_sel, "HELP Select",UI_menCB,(void*)"selHlp");


      //----------------------------------------------------------------
      // Entries von "Dump"
      // GUI_menu_entry   (wtmp6, "dump obj",   UI_menCB,  (void*)"dump");
      GUI_menu_entry   (&wtmp8, "Object", UI_menCB,  (void*)"dump");
        // GUI_Tip  ("display objData in textWindow");
      GUI_menu_entry   (&wtmp8, "Variables", UI_menCB,        (void*)"vars");
      GUI_menu_entry   (&wtmp8, "Points", UI_menCB,           (void*)"points");
      GUI_menu_entry   (&wtmp8, "Vectors", UI_menCB,          (void*)"vecs");
      GUI_menu_entry   (&wtmp8, "Transformations", UI_menCB,  (void*)"tras");
      GUI_menu_entry   (&wtmp8, "Interactions", UI_menCB,     (void*)"ints");
      GUI_menu_entry   (&wtmp8, "Textures", UI_menCB,  (void*)"texs");
      GUI_menu_entry   (&wtmp8, "Graf.Attrib.s", UI_menCB,  (void*)"dump_ga");
      GUI_menu_entry   (&wtmp8, "Models", UI_menCB,  (void*)"mods");
      GUI_menu_entry   (&wtmp8, "Group", UI_menCB,  (void*)"dumpGrp");
      GUI_menu_entry   (&wtmp8, "Source", UI_menCB,  (void*)"dumpSrc");
      GUI_menu_entry   (&wtmp8, "Standards", UI_menCB,  (void*)"dumpStd");
      GUI_menu_entry   (&wtmp8, "---",     NULL,       NULL);
      GUI_menu_entry   (&wtmp8, "view logfile", UI_menCB,  (void*)"logfile");
      GUI_menu_entry   (&wtmp8, "DispList", UI_menCB,  (void*)"DispList");
      // GUI_menu_entry   (&wtmp8, "Parents", UI_menCB,  (void*)"Parents");



      //----------------------------------------------------------------
      // Entries von "Group"
      // ckb_grpAdd = GUI_menu_checkbox__   (wtmp7, "add Obj (remove)",UI_grpAdd, NULL);
      // MSG_Tip ("MMgrAdd"); //
        // GUI_Tip  ("add obj to group; remove from Group, if already selected");

      GUI_menu_entry (&wtmp7, "save Group --> File",
                      UI_menCB,(void*)"grpSav");
      MSG_Tip ("MMgrSav"); //
        // GUI_Tip  ("save all objects of Group into File (native)");

      GUI_menu_entry (&wtmp7, "move Group --> new subModel",
                      UI_menCB,(void*)"grpSM");


      GUI_menu_entry   (&wtmp7, "---",     NULL,       NULL);

      GUI_menu_entry (&wtmp7, "move all objects of group",
                     UI_menCB,(void*)"grpMov");

      GUI_menu_entry (&wtmp7, "hide all objects of group",
                     UI_menCB,(void*)"grpHid");

      GUI_menu_entry (&wtmp7, "add all objects to group  (Ctrl-A)",
                     UI_menCB,(void*)"grpAll");

      GUI_menu_entry (&wtmp7, "invert Group ",
                     UI_menCB,(void*)"grpInv");

      GUI_menu_entry (&wtmp7, "delete Group             (Dele)",
                     UI_menCB,(void*)"grpDel");

      GUI_menu_entry (&wtmp7, "clear Group              (Esc)",
                     UI_menCB,(void*)"grpClr");

      GUI_menu_entry (&wtmp7, "HELP group",UI_menCB,(void*)"grpHlp");



      //----------------------------------------------------------------
      // Entries von "Catalog"
      GUI_menu_entry(&men_cat,"Modify Catalog", UI_menCB, (void*)"CtlgMod");
      MSG_Tip ("MMcaMod"); //

      GUI_menu_entry(&men_cat,"Create Catalog", UI_menCB, (void*)"CtlgCre");
      MSG_Tip ("MMcaCre"); //

      GUI_menu_entry(&men_cat,"Delete Catalog", UI_menCB, (void*)"CtlgDel");
      MSG_Tip ("MMcaDel"); //

      GUI_menu_entry (&men_cat, "HELP catalog", UI_menCB,(void*)"CtlgHlp");



      //----------------------------------------------------------------
      // Entries von "Applications"
      GUI_menu_entry(&men_app,"Start Application  (Ctrl-P)",
                     UI_menCB, (void*)"AppLoa");
      MSG_Tip ("MMapSta"); //
        // GUI_Tip  ("Load and start application");

      GUI_menu_entry(&men_app,"Edit Application   (Ctrl-E)",
                     UI_menCB, (void*)"AppEd");
      MSG_Tip ("MMapEdi"); //
        // GUI_Tip  ("edit the active  application");

      GUI_menu_entry(&men_app,"Create Application", UI_menCB, (void*)"AppCre");
      MSG_Tip ("MMapCre"); //

      GUI_menu_entry(&men_app,"Delete Application", UI_menCB, (void*)"AppDel");
      MSG_Tip ("MMapDel"); //

      GUI_menu_entry(&men_app,"Unload Application", UI_menCB, (void*)"AppUnl");

      GUI_menu_entry(&men_app,"HELP Application",  UI_menCB, (void*)"AppHlp");



      //----------------------------------------------------------------
      // Entries von "Process"
      GUI_menu_entry(&men_prc,"Select / Start / Edit Process  (Ctrl-P)",
                     UI_menCB, (void*)"PrcLoa");
      // MSG_Tip ("MMprcSta"); //
        // GUI_Tip  ("Load and start application");

      GUI_menu_entry(&men_prc,"Create Process", UI_menCB, (void*)"PrcCre");
        // select Prozessor

      GUI_menu_entry(&men_prc,"Delete Process", UI_menCB, (void*)"PrcDel");

      GUI_menu_entry(&men_prc,"HELP Process",  UI_menCB, (void*)"PrcHlp");



      //----------------------------------------------------------------
      // Entries von "Plugin"
      GUI_menu_entry(&men_plu,"Select / Start Plugin  (Ctrl-P)",
                     UI_menCB, (void*)"PluLoa");
      // MSG_Tip ("MMpluSta"); //
        // GUI_Tip  ("Load and start application");

      GUI_menu_entry(&men_plu,"HELP Plugin",  UI_menCB, (void*)"PluHlp");



      //----------------------------------------------------------------
      // Entries von "Remote"
      GUI_menu_entry(&men_rpc,"Select / Start Remote-control-program  (Ctrl-P)",
                     UI_menCB, (void*)"RpcLoa");
      // MSG_Tip ("MMpluSta"); //
        // GUI_Tip  ("Load and start application");

      GUI_menu_entry(&men_rpc,"HELP ",  UI_menCB, (void*)"RpcHlp");



      //----------------------------------------------------------------
      // Entries von "Help"
      GUI_menu_entry(&men_hlp,"Info", UI_menCB,   (void*)"doc1");
      // GUI_menu_entry(men_hlp,"Info English", UI_menCB,   (void*)"doc1_e");

      GUI_menu_entry(&men_hlp,"getting-started VWR",  UI_menCB,(void*)"staVWR");
      GUI_menu_entry(&men_hlp,"getting-started CAD",  UI_menCB,(void*)"staCAD");
      GUI_menu_entry(&men_hlp,"getting-started MAN",  UI_menCB,(void*)"staMAN");

      GUI_menu_entry(&men_hlp, "---",     NULL,       NULL);
      GUI_menu_entry(&men_hlp,"Documentation",UI_menCB,(void*)"doc2");
      // GUI_menu_entry(&men_hlp,"english-Docu (old)",UI_menCB,(void*)"doc2_old");
      // GUI_menu_entry(&men_hlp,"Translations (es/fr/it)",UI_menCB,(void*)"doc_tra");


      GUI_menu_entry(&men_hlp, "---",     NULL,       NULL);
      // GUI_menu_entry(men_hlp,"Dokumentation WCUT", UI_menCB,(void*)"docWC_g");
      GUI_menu_entry(&men_hlp,"Search/Names ", UI_menCB,(void*)"docSearch");
      GUI_menu_entry(&men_hlp,"Interaction ", UI_menCB,(void*)"docIact");
      GUI_menu_entry(&men_hlp,"Textures ", UI_menCB,(void*)"docTex");
      GUI_menu_entry(&men_hlp,"Catalog ", UI_menCB,(void*)"docCatalog");
      // GUI_menu_entry(men_hlp,"Application", UI_menCB,(void*)"docAppli");
      GUI_menu_entry(&men_hlp,"Plugin", UI_menCB,(void*)"docPlugin");
      GUI_menu_entry(&men_hlp,"RemoteControl", UI_menCB,(void*)"docCTRL");
      // GUI_menu_entry(men_hlp,"getting-started Assy", UI_menCB,(void*)"sta_g");
      // GUI_menu_entry(men_hlp,"getting-started Assy", UI_menCB,(void*)"sta_g");
      // GUI_menu_entry(men_hlp,"getting-started english",UI_menCB,(void*)"sta_e");


      // GUI_menu_checkbox__   (men_hlp, " Sprache deutsch");
      // GUI_menu_checkbox__   (men_hlp, " Language english");
      actwi = GUI_menu_entry   (&men_hlp, "---",     NULL,       NULL);


      // add List of installed languages
      UI_lang_men (&men_hlp);


      GUI_menu_entry(&men_hlp,"other language", UI_menCB,(void*)"docTransl");



      //================================================================
      // oberer Toolbar in eine Toolbox
    ToolBar1 = GUI_toolbox__ (&box00);
      printf(" men_hlp-L31\n");

    box1 = GUI_box_h (&ToolBar1, "");

    // GUI_spc_h (&box1, 4);
    box1A   = GUI_box_v (&box1, ",e");   // VWR/CAD/MAN

    // GUI_spc_v (&box1, 4);
    GUI_sep_v (&box1, 0);                  // vertical separatorline
    box1X   = GUI_box_v (&box1, ",e");     // Measure  Search/Name

    box1C   = GUI_box_v (&box1, ""); 
      box1C1  = GUI_box_h (&box1C,  "");
        box1C1v = GUI_box_v (&box1C1, ""); // GO STEP END
        box1C7v = GUI_box_v (&box1C1, ""); // suchen, ViewZ RotCen
        box1C2v = GUI_box_v (&box1C1, ""); // Names, Text
        box1C3v = GUI_box_v (&box1C1, ""); // 3D, Shade
        box1V   = GUI_box_v (&box1C1, ""); // Top, Front, Axo, Alles, Reset

                                      //   & Buttons Top,Front..UnHili


    box1R   = GUI_box_v (&box1, "");   // Labels ConstPln - Group

    box1S   = GUI_box_v (&box1, "");   // Values ConstPln - Group

    GUI_spc_h (&box1, 4);           // space 
    box1Y   = GUI_box_v (&box1, "");   // Box plugins-List
                                      //   & Buttons Undo / Redo



      //----------------------------------------------------------------





      //--------------------------------------------------------------
      // wtmp1 = GUI_box_h (&box1A, "");
      ckb_vwr=GUI_radiobutt__ (&box1A, "VWR ", 0, UI_butCB, (void*)"VWR", ",e");
      MSG_Tip ("MMrbVwr"); //
      // GUI_Tip  ("Viewer");
      
      // wtmp1 = GUI_box_h (&box1A, "");
      ckb_cad=GUI_radiobutt__ (&box1A, "CAD ", 1, UI_butCB, (void*)"CAD", ",e");
      MSG_Tip ("MMrbCad"); //
      // GUI_Tip  ("interactive CAD-Object-Editor");

      // wtmp1 = GUI_box_h (&box1A, "");
      ckb_man=GUI_radiobutt__ (&box1A, "MAN ", 1, UI_butCB, (void*)"MAN", ",e");
      MSG_Tip ("MMrbMan"); //
      // GUI_Tip  ("manual mode");






      //--------------------------------------------------------------
      GUI_label__ (&box1R, " ConstrPln", "l");
      // GUI_label__(box1R, "Refsys", 0);
      // GUI_label__(box1R, "Layer ", 0);
      // GUI_label__(box1R, "GrAttr", 0);
      GUI_label__ (&box1R, " Modelsize", "l");
      GUI_label__ (&box1R, " LineNr",    "l");
      GUI_label__ (&box1R, " Undo  ",    "l");
      GUI_label__ (&box1R, " Group ",    "l");

      UI_ConstP  = GUI_label__ (&box1S, "  0", "r,6");
      UI_GrAttr  = GUI_label__ (&box1S, "  0", "r");
      UI_lNr     = GUI_label__ (&box1S, "  0", "r");
      UNDO_init1 (&box1S);    // Undo-Listbox
      UI_grpNr   = GUI_label__ (&box1S, "  0", "r");




      //--------------------------------------------------------------
      // in box1C1v 2 boxen untereinender
      box1C1v1h = GUI_box_h (&box1C1v, "");
      box1C1v2h = GUI_box_h (&box1C1v, "");


      but_go = GUI_button__ (&box1C1v1h, "  GO  ", UI_butCB,(void*)"butGO","");
      MSG_Tip ("MMbtgo"); //
      // GUI_Tip  ("rework to act. line / bis zur aktuellen Zeile abarbeiten.");

      but_step = GUI_button__ (&box1C1v1h," STEP ",UI_butCB,(void*)"butSTEP","");
      MSG_Tip ("MMbtstp"); //
      // GUI_Tip  ("rework line / aktuelle Zeile ausfuehren");

      but_end = GUI_button__ (&box1C1v2h, " END  ", UI_butCB, (void*)"butEND", "e");
      MSG_Tip ("MMbtend"); //
      // GUI_Tip  ("regenerate all / das gesamte Prog. abarbeiten");

/*
      GUI_button__ (box1C1v2h, "UNDO", UI_butCB, (void*)"butUndo", 0);
      GUI_Tip  ("letzten Befehl loeschen");
      GUI_button__ (box1C1v2h, "REDO", UI_butCB, (void*)"butRedo", 0);
      GUI_Tip  ("letzten Befehl wiederherstellen");
*/


      // box1C7v:
/*
      wtmp1=GUI_ckbutt__ (box1C7v, " Zoom", FALSE, UI_butCB, (void*)"butZoom", 0);
      GUI_Tip  ("wenn aktiv: mit der linken Maustaste zwei gegenueberliegende "
                "Eckpunkte des gewuenschten Ausschnittes waehlen");
      GUI_set_enable (wtmp1, FALSE);
*/
      UI_view_Z = GUI_entry__ (&box1C7v, NULL, "0.0", UI_view_Z_CB, NULL, "8");
      MSG_Tip ("MMenZva"); //
      // GUI_Tip  ("Z-value of viewplane / "
                // "Z-Wert der aktuellen Ansichtsebene");


      // was ViewPln
      ckb_vwz=GUI_ckbutt__ (&box1C7v, "RotCen", FALSE, UI_vwzCB, NULL, "");
      MSG_Tip ("MMcbRot"); //
      // GUI_Tip  ("define rotation-center / "
                // "den Mittelpunkt fuer das interaktive Drehen definieren. ");
                // "Drehzentrum genau in die Mitte schieben, ViewZ.");




      // box1C2v:
      ckb_hide = GUI_ckbutt__ (&box1C2v,"Hide",FALSE, UI_CB_hide, NULL, "");
      MSG_Tip ("MMcbHid"); //
      // GUI_Tip  ("Objeke ausblenden.");

      ckb_view = GUI_ckbutt__ (&box1C2v,"View",FALSE, UI_CB_view, NULL, "");
      MSG_Tip ("MMcbVie"); //
      // GUI_Tip  ("Objekte (nach \"Hide\") wieder einblenden");


      // box1C3v:
      ckb_3D = GUI_ckbutt__ (&box1C3v, " 3D  ", TRUE, UI_wireCB, NULL, "");
      MSG_Tip ("MMcb3dv"); //
      // GUI_Tip  ("Darstellung der aktiven Flaeche oder volles 3D");

      ckb_shd = GUI_ckbutt__ (&box1C3v, "Shade", TRUE, UI_wireCB, NULL, "");
      MSG_Tip ("MMcbSha"); //
      // GUI_Tip  ("Darstellung schattiert oder als Drahtmodell.");






      //--------------------------
      box1V1 = GUI_box_h (&box1V, "");
      box1V2 = GUI_box_h (&box1V, "");


      bt_top=GUI_button__ (&box1V1, "View Top", UI_butCB, (void*)"butTop", "e");
      MSG_Tip ("MMbtTop"); //
      // GUI_Tip  ("Draufsicht (X-Y)");

      bt_front=GUI_button__ (&box1V1, "Front", UI_butCB, (void*)"butFront", "e");
      MSG_Tip ("MMbtFro"); //
      // GUI_Tip  ("Vorderansicht (X-Z)");

      bt_side=GUI_button__ (&box1V1, "Side", UI_butCB, (void*)"butSide", "e");
      MSG_Tip ("MMbtSid"); //
      // GUI_Tip  ("Seitenansicht (Y-Z)");

      bt_iso=GUI_button__ (&box1V1, "Axo", UI_butCB, (void*)"butAxo", "e");
      MSG_Tip ("MMbtAxo"); //
      // GUI_Tip  ("Axonometrische Ansicht");




      // box1C4v:
      GUI_button__ (&box1V2, "Scale All", UI_butCB, (void*)"butRes", "");
      MSG_Tip ("MMbtSca"); //
      // GUI_Tip  ("Vergroesserung und Darstellungsmittelpunkt automatisch "
                // "errechnen");

      GUI_button__ (&box1V2, "Mdl", UI_butCB, (void*)"butAll", "");
      MSG_Tip ("MMbtScf"); //
      // GUI_Tip  ("eine Raumgroesse von ca. 6000 x 6000 x 6000 mm darstellen");

      GUI_button__ (&box1V2, "Grp", UI_butCB, (void*)"butScG", "");
      MSG_Tip ("MMbtScg"); //
      // GUI_Tip  ("scale group");

      GUI_button__ (&box1V2, "View", UI_butCB, (void*)"butVwpt", "");
      MSG_Tip ("MMbtVwpt"); // store (Ctrl-V) / restore view (Ctrl-<curKey>)




      //--------------------------
      // CheckBox "ViewZ"
      // actbox = GUI_box_h (box1C6v, 0);
      // ckb_vwz=GUI_ckbutt__ (actbox, "ViewZ", FALSE, UI_vwzCB, NULL, 0);

      // ein Entryfeld Format 999999.9
      // UI_sur_Z = GUI_Entry (actbox, NULL, "     0.0",UI_sur_Z_CB, 0);
      // GUI_Tip  ("der Z-Wert der aktuellen Ansichtsebene");



      //----------------------------------------------------------------
      // CursorPosition
      frm_act = GUI_frame__ (&box1C, NULL, 0);
      // box1C2: CursorPosition, rel/abs, Scale.
      box1C2 = GUI_box_h (&frm_act, ",e"); //
      UI_curPos = GUI_label__(&box1C2,"","");   // l30


/*
      // Konstruktionsebene - Liste der Refsys
      // UI_sur_act = GUI_OptMen (box1C2, surLst, UI_sur_act_CB1, 0);
      l1 = -1;
      GUI_button__ (box1C2, "ConstPln", UI_sur_act_CB1, (void*)l1, 0);
      GUI_Tip  ("select constr.plane / "
                "Konstruktionsebene auswaehlen");
*/

      // rel abs der MouseCoords
      frm_act = GUI_frame__ (&box1C2, NULL, 0);
      boxRelAbs = GUI_box_h (&frm_act, ""); //
      GUI_radiobutt__ (&boxRelAbs, "rel", 0, UI_RelAbsCB, &GUI_FuncUCB1, "");
      MSG_Tip ("MMrbRel"); //
      GUI_radiobutt__ (&boxRelAbs, "abs", 1, UI_RelAbsCB, &GUI_FuncUCB2, "");
      MSG_Tip ("MMrbAbs"); //


/*
      //  Z-Wert Konstruktionsebene
      UI_sur_Z = GUI_Entry (box1C2, " ConstZ=", "0", UI_sur_Z_CB, -60);
      GUI_Tip  ("Z-value of constr.plane / "
                "Z-Wert der aktuellen Konstruktionsebene");
*/


      // actbox = gtk_fixed_new();
      // gtk_fixed_put(GTK_FIXED (actbox), UI_curPos, 0, 0);


      // ein Ausgabefeld (Scale) mit rahmen
      frm_act = GUI_frame__ (&box1C2, NULL, 0);
      actbox = GUI_box_h (&frm_act, ""); //
      GUI_label__ (&actbox, " Scale ", "");
      UI_ouf_scl = GUI_label__ (&actbox, "", "");   // 12e
      // GUI_Tip  ("der aktuelle Vergroesserungsfaktor (Scale)");


      // ZeilenNr.
      // frm_act = GUI_frame__ (box1C2, NULL, 0);
      // actbox = GUI_box_h (frm_act, 0); //
      // GUI_label__(actbox," Zeile ",0);
      // UI_lNr = GUI_label__(actbox,"999999",0);
      // // gtk_misc_set_alignment (GTK_MISC (UI_lNr), 0.01, 0.01);






      //--------------------------------------------------------------

      ckb_meas = GUI_ckbutt__ (&box1X,"Measure",FALSE,
                               UI_butCB, (void*)"Meas", ",e");
      MSG_Tip ("MMcbMea"); //


      ckb_such = GUI_ckbutt__ (&box1X,"Search/Name",FALSE,
                               UI_butCB, (void*)"oNam", ",e");
      MSG_Tip ("MMcbSN"); //


      ckb_Iact = GUI_ckbutt__ (&box1X,"Interaction",FALSE,
                               UI_butCB, (void*)"intAct", ",e");
      MSG_Tip ("MMopInt"); //

      // ckb_mdel=GUI_ckbutt__ (box1X, "Modify/Delete", FALSE, UI_butSM, NULL, 0);
      // MSG_Tip ("MMcbMD"); //

/*

      // ckb_wcGeo=GUI_RadButtAdd (wtmp1,"NC-Wcut",UI_butCB,(void*)"DE_GEO",-80);
      // MSG_Tip ("MMrbNCw"); //
      // GUI_Tip  ("Drahterodieren - Schneidwege erzeugen."
                // "nicht aktiv / not active");

      // ckb_nc =GUI_RadButtAdd  (wtmp1, "NC",  UI_butCB, (void*)"NC", -80);
      // ckb_wcTec=GUI_RadButtAdd(wtmp1,"NC-Tech",UI_butCB,(void*)"DE_TEC",-80);
      // GUI_Tip  ("NC - Bearbeitung definieren");

      // ckb_nc =GUI_RadButtAdd  (wtmp1, "NC",  UI_butCB, (void*)"NC", -80);
      // ckb_wcTec=GUI_RadButtAdd(wtmp1,"NC-Tech",UI_butCB,(void*)"DE_TEC",-80);
      // GUI_Tip  ("NC - Bearbeitung definieren");


      // mit der folgenden Zeile ist Editor beim Startup NICHT sichtbar !?
      // 2011-01-22 raus; impliziert UI_butCB "VWR"  -> CRASH!!
      // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_man), TRUE);

*/



      //--------------------------------------------------------------
      // box1Y = Undo-Listbox active addOnProgram, type-of-program
      // UI_undo_Win (box1Y);

      // GUI_spc_v (&box1Y, 16);
      // GUI_spc__ (&box1Y, 1, 6);   // 0=h, 1=v

      // Label active programtyp
      UIw_typPrg = GUI_label__(&box1Y, "---------", "l16");
      if(APP_act_typ > 0) UI_Set_typPrg ();

      // Label active program
      UIw_prg = GUI_label__(&box1Y, "---------", "l16");
      if(strlen(APP_act_nam) > 0)  UI_Set_actPrg (APP_act_nam, -1);
      // if(strlen(APP_act_nam) > 0)  GUI_OptMenSet (APP_act_nam, -1);

      // infolabel; eg for "PTonObj"
      UIw_infoSel = GUI_label_htm__ (&box1Y, "-", "l");

      // Buttons Undo / Redo
      wtmp2 = GUI_box_h (&box1Y, "");
      UNDO_init2 (&wtmp2);    // Undo-Listbox



      //================================================================
      // CAD und NC-Toolbar; anfangs hidden.
      // tbCad = UI_cad (UIw_Box_TB, NULL);
      // gtk_widget_hide (tbCad);
      // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_cad), TRUE);
      // UI_InpMode = UI_MODE_CAD;


      // tbWcGeo  = UI_wcg__  (UIw_Box_TB, NULL);
      // gtk_widget_hide (tbWcGeo);
      // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_wcGeo), TRUE);
      // UI_InpMode = UI_MODE_WCGEO;


      // tbWcTec  = UI_wcTec__ (UIw_Box_TB, NULL);
      // gtk_widget_hide (tbWcTec);
      // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_wcTec), TRUE);
      // UI_InpMode = UI_MODE_WCTEC;


      // tbNc  = UI_nc  (UIw_Box_TB, NULL);
      // gtk_widget_hide (tbNc);
      // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (ckb_nc), TRUE);
      // UI_InpMode = UI_MODE_NC;







      //================================================================
      // create box for Browser & GraficWindow
      //das Edit-Fenster und das Grafik-fenster nebeneinender = 2B1A / 2B1B
      // hpaned = gtk_hpaned_new ();      // horizontal paned
      // gtk_container_add (GTK_CONTAINER(box00), hpaned);
      // gtk_widget_show (hpaned);
      win_paned = GUI_box_paned__ (&box2B1, &wtmp1, &box00,
                                 0, 0, 200);  // 0,0,200


      //----------------------------------------------------------------
      // links das Browser/Editorfenster; into box; hide one of it

      // create a notebook in left paned (box2B1)
      UI_box_ntb = GUI_notebook__ (&box2B1, UI_paned_CB);
      win_brw = GUI_notebook_add (&UI_box_ntb, "Browse");
      win_edi = GUI_notebook_add (&UI_box_ntb, "Edit");

      // TreeBrowser
      winBrw = GUI_tree1__ (&win_brw, Brw_CB_sel, "e,e");
      winBrStat = OFF;

      // Editor
      winED = GUI_edi__ (&win_edi, EDI_CB__, 0, "e,e");




      //----------------------------------------------------------------
      // setUp OpenGL-window
      // winGR = UI_GR_Init (600, 0);
      // gtk_container_add (GTK_CONTAINER (wtmp1), winGR);
      // gtk_widget_show (winGR);
      // winGR = GUI_gl__ (&wtmp1, UI_GL_draw__, "-600e,-400e");
      winGR = GUI_gl__ (&wtmp1, UI_GL_draw__, "-600e,e");

      // connect the mouse-move events
      GUI_gl_ev_move (&winGR, UI_GL_move__);

      // connect the mouse-button events
      GUI_gl_ev_button (&winGR, UI_GL_mouse__);

      // connect the keyboard-events
      GUI_gl_ev_key (&winGR, UI_GL_keys__);




      //----------------------------------------------------------------
      // box for the application-toolbars
      UIw_Box_TB = GUI_box_v (&box00, "");
        // GUI_obj_dump_mo (&UIw_Box_TB);


      //================================================================
      // auch das TextausgabeWin in eine Toolbox
      // ToolBar2 = GUI_toolbar__ (&box00);
      ToolBar2 = GUI_toolbox__ (&box00);

      GUI_get_version (cbuf1, &i1, &i2);
      if(i1 == 2) {
        // Gtk2
        box2B2 = GUI_box_h (&ToolBar2, "e,e");  //"e,6");
        // winTX = GUI_Tx_Win (&box2B2, -400, -80);
        // gtk2-problem: image on the left takes full box !
        winTX = GUI_msgwin__ (&box2B2, "e,e");   // 96,6
        // disp. Logo rechts neben dem Textwindow.
        wtmp1 = GUI_box_v (&box2B2, "a,a");   // -150
        // ../icons/xa_logo.xpm 127x46 pixels
        sprintf(cbuf1, "%sxa_logo.xpm", OS_get_ico_dir ());
        GUI_img__ (&wtmp1, cbuf1, "-127,-64");   // min = 46; 2013-10-05
  
      } else {
        // Gtk3
        box2B2 = GUI_box_h (&ToolBar2, "e,4");  //"e,6");
        // winTX = GUI_Tx_Win (&box2B2, -400, -80);
        // gtk2-problem: image on the left takes full box !
        winTX = GUI_msgwin__ (&box2B2, "e,e");   // 96,6
        // disp. Logo rechts neben dem Textwindow.
        wtmp1 = GUI_box_v (&box2B2, "a,a");   // -150
        // ../icons/xa_logo.xpm 127x46 pixels
        sprintf(cbuf1, "%sxa_logo.xpm", OS_get_ico_dir ());
        GUI_img__ (&wtmp1, cbuf1, "a,a");
      }




      //================================================================
      // connect KeyPress und -Release am Textwin
/*
      gtk_widget_set_events (GTK_WIDGET(winED.view),
                             GDK_BUTTON_PRESS_MASK|
                             GDK_BUTTON_RELEASE_MASK|
                             GDK_KEY_PRESS_MASK|
                             GDK_KEY_RELEASE_MASK
                             );
*/


/*
      // die Mausbuttons
      gtk_signal_connect (GTK_OBJECT(winED.view),"button_press_event",  // GTK2
                          GTK_SIGNAL_FUNC(UI_EdButtonPress), NULL);

      // weil nach dem Press kann die Pos nicht richtig gelesen werden ..
      // vor dem Press ist die Curpos falsch !!
      gtk_signal_connect (GTK_OBJECT(winED.view),"button_release_event",  // GTK2
                          GTK_SIGNAL_FUNC(UI_EdButtonRelease), NULL);


      gtk_signal_connect (GTK_OBJECT(winED.view),"key_press_event",  // GTK2
                           GTK_SIGNAL_FUNC(UI_EdKeyPress), NULL);


      gtk_signal_connect (GTK_OBJECT(winED.view),"key_release_event",  // GTK2
                          GTK_SIGNAL_FUNC(UI_EdKeyRelease), NULL);
*/

    // g_signal_connect (G_OBJECT(winED.view), "focus-in-event",
                        // G_CALLBACK (UI_winEd_enter), NULL);




      // size_allocate u realize u show u expose_event geht; aber zu frueh.
      // draw geht; aber viel zu oft !
      // gtk_signal_connect (GTK_OBJECT (tbCad),
                    // "draw",
                    // (GtkSignalFunc) GUI_ToolboxCB, NULL);
      // unmap kommt zu frueh


      // gtk_signal_connect (GTK_OBJECT (tbCad),
                    // "draw",
                    // (GtkSignalFunc) GUI_ToolboxCB, NULL);


// hpaned winBrw.win






      //================================================================
      /* -------- Display Win ---------------- */
        // printf(" vor _show\n");
      // gtk_widget_show (GTK_WIDGET(UI_MainWin));
      // GUI_Win_go (&winMain);

      // wait until win is up !
      // GUI_update__ ();

      // Focus aufs EditWindow
      // UI_focEdCB (NULL, NULL);

      // UI_butCB (ckb_wcTec, (void*)"DE_TEC");
      // UI_butCB (NULL, (void*)"MAN");

      // ED_work_END get no ned !

      // Init Textfensterparameter; into GUI_Ed_Init genommen.
      // GUI_Ed_Init1 (&winED);

      // im Modus VWR starten
      // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON(ckb_vwr), TRUE);
      // 2011-01-23
      UI_InpMode = UI_MODE_VWR;


      // primary display browser, not editor
      win_edStat = 1;  // disactivate ..
      UI_brw__ (0);


/*
      //----------------------------------------------------------------
      // activate language; crash in MS if before show mainWinmainWin  !
      // gtk_signal_emit_by_name (GTK_OBJECT(UI_lang[i1]),"activate");
      GUI_menu_radiobutt_set ("en");
*/

      // act. GO,STEP;   MAN:On; CAD,VWR:Off.
      GUI_set_enable (&but_go, FALSE);
      GUI_set_enable (&but_step, FALSE);


      // copy out
      if(mo) *mo = winMain;

      // GUI_obj_focus (&winGR);

      break;






    //================================================================
    case UI_FuncKill:
    case UI_FuncExit:
      // printf("UI_win_main UI_FuncKill\n");

      // if(UI_InpMode == UI_MODE_WCTEC) {
        // if(UI_wcTec_askExit(NULL, "") != 0) return 0;

      // } else if(UI_InpMode == UI_MODE_WCGEO) {
        // if(UI_wcg_askExit(NULL, "") != 0) return 0;
      // }

      // // akt. Datei als tmp/xa.apt rausschreiben
      // strcpy(AP_mod_fnam, OS_get_bas_dir ());
      // strcat(AP_mod_fnam,"tmp/xa.apt");

      // model in tmp saven
      // UI_save_ ();
      // Mod_sav_tmp (); // save the active Submodel AP_modact_nam -> TempFile

      // unload active Application
      if(AP_stat.APP_stat) {
        strcpy (cbuf1, APP_act_nam);
        DLL_run2 ("", -1);                // see PLU_unl
        strcpy (APP_act_nam, cbuf1);
      }

      // test if model is modified;
      // Mod_sav_i already done
      i1 = Mod_sav_ck (1);
      if(i1) {
        irc = AP_save_ex (0);
      }

      // save Viewparameters (Scale, Center, ..)
      // UI_view__ ("Save");

      AP_exit__ (); // shutdown ..

      // gtk_widget_destroy ((GtkWidget*)UI_MainWin);
      GUI_Win_kill (&winMain);
      winMain = GUI_OBJ_INVALID();
      exit(0);


  }



  // return TRUE;
  // printf("ex UI_win_main\n");
  return 0;

}


//================================================================
  int UI_VW_upd (int newView) {
//================================================================
/// \code
/// UI_VW_upd    enable/disable active view-button
///   newView    FUNC_ViewTop FUNC_ViewFront FUNC_ViewSide FUNC_ViewIso
/// \endcode

  static int actView = -1;


  if(actView < 0) {  // init
    UI_VW_set (newView, 0);  // disable new view-button

  } else {
    if(actView != newView) {
      UI_VW_set (actView, 1);  // enable active view-button
      UI_VW_set (newView, 0);  // disable new
    }
  }

  actView = newView;

  return 0;

}
//================================================================
  int UI_VW_set (int iView, int mode) {
//================================================================
/// \code
/// UI_VW_set    enable/disable active view-button
///   iView        FUNC_ViewTop FUNC_ViewFront FUNC_ViewSide FUNC_ViewIso
///   mode         1=TRUE=active; 0=FALSE=unpickable
/// \endcode

  static int actView = -1;


  if(mode == FUNC_Init) {  // init
    UI_VW_upd (FUNC_Init);  // enable/disable active view-button
  }


  switch(iView) {

    case FUNC_ViewTop:
      GUI_set_enable (&bt_top, mode);
      break;

    case FUNC_ViewFront:
      GUI_set_enable (&bt_front, mode);
      break;

    case FUNC_ViewSide:
      GUI_set_enable (&bt_side, mode);
      break;

    // case FUNC_ViewIso:
      // GUI_set_enable (&bt_iso, mode);
      // break;
  }


  return 0;

}


//================================================================
  int UI_open_last () {
//================================================================
// UI_open_last                  display file <tmpDir>MdlLst.txt as subMenu
  

  int   i1;
  char  fnam[128], s1[256];


  printf("UI_open_last -------\n");


  // save Model+Submodels into tempDir as "Model" native
  Mod_sav_i (0);


  // test if model is modified;
  i1 = Mod_sav_ck (1);
  if(i1) {
    // model has changed -
    i1 = AP_save_ex (1);
    if(i1 < 0) return 0;     // cancel
  }


  //----------------------------------------------------------------
  sprintf(fnam, "%sMdlLst.txt", OS_get_tmp_dir());

  i1 = GUI_list1_dlg_w (s1, 256,
                       NULL, " select model", fnam,
                       "1", NULL, "80,16");
  if(i1 < 0) return -1;
    printf("UI_open_last sel |%s|\n",s1);


  //----------------------------------------------------------------
 
  // clear src-Memory, reset Undo, Hide, View-Plane, ConstrPlane. 
  AP_src_new (1);
  // UI_menCB (NULL, "new");   // NEW

  // load selected model
  return AP_Mod_load_fn (s1, 0);

}


//===================================================================
  int UI_lang_men (MemObj *mo) {
//===================================================================
// add menu of radiobuttons for loaded languages
 
  int    i1, ia, ie, ii, lngNr;
  char     *lngLst;   // size 0
  char     *lngName;  // size 40
  MemObj m1, ma, me;


  printf("UI_lang_men |%s|\n",AP_lang);


  // get list of loaded languages (lngCode[lngNr]);
  lngNr = AP_lngTab_get (&lngLst);

  // get list of names of loaded languages (lngName[lngNr]);
  lngName = (char *) MEM_alloc_tmp (lngNr * 40);
  for(i1=0; i1 < lngNr; ++i1) {
    // get name of lang i1
    AP_lngNam_get (&lngName[i1 * 40], &lngLst[i1 * 4]);
  }



  //----------------------------------------------------------------
  // create menu-entry (lngNr buttons with names of loaded languages)
  ia = -1;
  ie = -1;
  ii = 0;
  for(i1=0; i1 < lngNr; ++i1) {
    // add radiobutton
      // printf(" lng %d |%s|%s|\n",i1,lngCode[i1],lngName[i1]);
    m1 = GUI_menu_radiobutt__ (mo,&lngName[i1*40],ii,UI_lang_chg,&lngLst[i1*4]);
    if(!ii) ++ii;  // 0=new menu; 1=add to menu

    // keep language AP_lang
    if (!strcmp(AP_lang, &lngLst[i1*4])) {
      ia = i1;
      ma = m1;
    }

    // keep defLang en
    if (!strcmp(AP_lang, "en")) {
      ie = i1;
      me = m1;
    }
  }


  if(ia < 0) {
    TX_Print ("UI_lang_men - cannot find lang. %s - defaulted english.",AP_lang);
    GUI_menu_radiobutt_set (&me);
    ia = ie;
    ma = me;
  }


    printf(" lang-activate |%s|\n",&lngLst[ia*4]);
  GUI_menu_radiobutt_set (&ma);
  // strcpy(AP_lang, lngLst[ia]);
  // init lang
  MSG_const_init (AP_lang);
  MSG_Init (AP_lang);

  return 0;

}


//================================================================
  int UI_lang_chg (MemObj *mo, void **data) {
//================================================================

  char    *cp1;


  printf("UI_lang_chg |%s|%s| %d\n",GUI_DATA_S1,AP_lang,AP_stat.sysStat); 

  if(AP_stat.sysStat < 2) return 0;

  if(GUI_DATA_EVENT == TYP_EventRelease) return 0;

  cp1 = GUI_DATA_S1;
  if(!strcmp(cp1, AP_lang)) return 0;

    // printf("UI_lang_chg |%s|%s|\n",cp1,AP_lang); 


  strcpy(AP_lang, cp1);

  // load msg/msg_const_c2l<AP_lang>de.txt
  MSG_const_init (AP_lang);
  // open file msg/msg_<sLang>.txt on lun MSG_fp
  MSG_Init (AP_lang);

  // change CAD-toolbox-text
  UI_CAD_reload ();

  // reDisplay init-text
  TX_Print("***** languge modified *****");
  MSG_pri_0 ("MM0");

  // for full functionanlity restart gcad3d.
  MSG_pri_0 ("Lang_chg1");


  // TX_Print("HOW TO IMPROVE DOCUMENTATION: see Help/Translations");


  return 0;
}


//=====================================================================
  int UI_WinInfo2 () {
//=====================================================================

  char    s1[256];



  strcpy(s1, "License: GPL-3");
  strcat(s1, "\nCopyright: 1999-2018 CADCAM-Services Franz Reiter");
  strcat(s1, "\n(support@gcad3d.org)");
  GUI_AboutInfo (INIT_TXT, s1, "http://www.gcad3d.org", "xa_logo.xpm");

  return 0;

}


/*
//=====================================================================
  int UI_WinInfo1 (MemObj *mo, void **data) {
//=====================================================================

  static MemObj win0=GUI_OBJ_NEW;
  MemObj box0, wact;

  int       i1;
  char      cbuf[256];


  i1 = GUI_DATA_I1;

  // printf("UI_WinInfo1 %d\n",i1);

  switch (i1) {


    //---------------------------------------------------------
    case UI_FuncInit:

      if(GUI_OBJ_IS_VALID(&win0)) {           // Win schon vorhanden ?
        // GUI_set_show (&win0, 1);
        return 0;
      }

      win0 = GUI_Win__ ("Info",UI_WinInfo1, "");

      box0 = GUI_box_v (&win0, "");
      // GUI_Pix (box0, "xa_logo.xpm", 0);
      sprintf(cbuf, "%sxa_logo.xpm", OS_get_ico_dir ());
      GUI_img__ (&box0, cbuf, "");

      strcpy(cbuf, INIT_TXT);
      // strcat(cbuf, " - Host ");
      // os_l_id (cbuf);
      // strcat(cbuf, cbuf);
      GUI_label__(&box0, cbuf, "");


      if(!strcmp(AP_lang, "de")) {
      GUI_label__(&box0," Lizenz ist GPL-v3 -","");
      GUI_label__(&box0," darf kostenlos fuer alle Zwecke genutzt werden","");
      GUI_label__(&box0," darf kopiert und weitergegeben werden","");
      GUI_label__(&box0," keine Einschraenkungen fuer Zusatz-Software (DLL's)","");

      GUI_label__(&box0," ","");
      GUI_label__(&box0," JEDE GEWÄHRLEISTUNG WIRD AUSGESCHLOSSEN.","");
      GUI_label__(&box0," JEDE HAFTUNG WIRD AUSGESCHLOSSEN.","");
      GUI_label__(&box0," ","");

      } else {
      GUI_label__(&box0," License is GPL-v3 -","");
      GUI_label__(&box0," may be used for any purposes free of costs -","");
      GUI_label__(&box0," may be copied and distributed without restrictions","");
      GUI_label__(&box0," no restrictions for AddOn-Software (Plugins, DLL's)","");
      GUI_label__(&box0," ","");
      GUI_label__(&box0," THERE IS NO WARRANTY FOR THE PROGRAM. ","");
      GUI_label__(&box0," THE ENTIRE RISK IS WITH YOU. ","");
      GUI_label__(&box0," ","");
      }


      // GUI_label__(&box0," 1999-2015 CADCAM-Services Franz Reiter ","");
      GUI_label__(&box0," http://www.gCAD3D.org ","");
      GUI_label__(&box0," support@gcad3d.org","");

      GUI_Win_up (NULL, &win0, 0);
      GUI_Win_go (&win0);

      break;




    //---------------------------------------------------------
    case UI_FuncKill:
    case UI_FuncExit:
      // printf("Print exit\n");

      // EXIT
      // gtk_widget_destroy (&win0);
      GUI_Win_kill (&win0);
      win0 = GUI_OBJ_INVALID();
  }

  return 0;

}
*/

//=====================================================================
  int UI_RelAbs_act (int mode) {
//=====================================================================
// 1=activ=TRUE, 0=inaktiv=FALSE.

  GUI_set_enable (&boxRelAbs, mode);

  return 0;

}


//=====================================================================
  int UI_set_Ins_Sel_Cat (int mode) {
//=====================================================================
// activate menu Insert Select CATALOG,
// enable selection of all types
// MAN; CAD: TRUE;  VWR: FALSE

  sele_set__ (Typ_goGeom);    // enable selection of all types

  GUI_set_enable (&men_ins, mode);
  GUI_set_enable (&men_sel, mode);
  GUI_set_enable (&men_cat, mode);


  return 0;

}




//=====================================================================
  int UI_RelAbsCB (MemObj *mo, void **data) {
//=====================================================================

  int   i1;



  // // skip disactivation ..
  // if(GTK_TOGGLE_BUTTON (parent)->active == 0) return 0;  // 1=ON;0=OFF
  if(GUI_DATA_EVENT == TYP_EventRelease) return 0;


  // i1 = INT_PTR(data);
  i1 = GUI_DATA_I1;

  printf("UI_RelAbsCB %d\n",i1);


  switch (i1) {

    case UI_FuncUCB1:   // rel
      UI_RelAbs = 0;
      break;

    case UI_FuncUCB2:   // abs
      UI_RelAbs = 1;
      break;

  }

  return 0;

}


/*
//=====================================================================
  int UI_WinIgeImp (GtkWidget *parent, void *data) {
//=====================================================================
// Iges-ImportPanel


  int    i1;
  double d1;
  char   *txt;
  static GtkWidget *win0=NULL, *box0, *cb1, *offset;
  // static GtkWidget *ent_tol, *ent_len;




  printf("UI_WinIgeImp %d\n",INT_PTR(data));

  i1 = INT_PTR(data));


  switch (i1) {


    //---------------------------------------------------------
    case UI_FuncInit:

      if(win0 != NULL) {           // Win schon vorhanden ?
        gtk_widget_destroy (win0);
      }

      win0 = GUI_Window ("Import IGE-File",UI_WinIgeImp,(void*)UI_FuncKill,0);

      box0 = GUI_box_v (win0, 0);

      cb1=GUI_ckbutt__ (box0, "3D -> 2D", FALSE, NULL, NULL, 0);
      GUI_Tip  ("als 3D oder 2D-Elemente einlesen");

      // cb2=GUI_ckbutt__ (box0, "Curve -> Circ", TRUE, NULL, NULL, 10);
      // GUI_Tip  ("Kurven approximieren");

      // ent_tol = GUI_Entry (box0, "Toleranz: ", "0.005", NULL, -60);
      // GUI_Tip  (" max. Abweichung bei Approximation");

      // ent_len = GUI_Entry (box0, "min.Len.: ", "0.2", NULL, -60);
      // GUI_Tip  (" minimale Laenge der Kreisboegen / Linien");


      // X/Y-Offset
      offset = GUI_Entry (box0, " Nullpunkt X, Y, Z ","0,0,0",NULL, -160);
      GUI_Tip  ("die gesamte Geometrie um den angegebenen "
                "X/Y-Wert verschieben");


      GUI_Hsep (box0, 0);
      GUI_button__ (box0, "OK", UI_WinIgeImp, (void*)UI_FuncWork, 10);

      gtk_widget_set_uposition (win0, 0, 0);  // Pos of mainwin
      gtk_widget_show (win0);
      break;




    //---------------------------------------------------------
    case UI_FuncWork:
      printf("IgeImp work\n");

      // work ..
      i1 = 1 - GTK_TOGGLE_BUTTON (cb1)->active;
      // i2 = 1 - GTK_TOGGLE_BUTTON (cb2)->active;
      // cvApp_tol  = atof(gtk_entry_get_text ((GtkEntry*) (ent_tol)));
      // cvApp_lmin = atof(gtk_entry_get_text ((GtkEntry*) (ent_len)));


      // printf(" i1=%d tol=%f len=%f\n",i1,cvApp_tol,cvApp_lmin);

      // cvApp_tol = 0.1;  // nur Test
      txt = gtk_entry_get_text ((GtkEntry*) (offset));


      // add importData to UTF_FilBuf1
      AP_ImportIges (txt, i1, memspc011);

      // die Hauptdatei raus ins Editfenster
      ED_load__ ();
      // xa_fl_TxMem = 1; // weils veraend. wurde

      // imply END-Button (Redraw)
      // UI_AP (UI_FuncSet, UID_but_END, NULL);
      ED_work_END (0);


      // close win
      gtk_widget_destroy (win0);
      win0 = NULL;

      break;




    //---------------------------------------------------------
    case UI_FuncKill:
      printf("IgeImp exit\n");

      // EXIT
      gtk_widget_destroy (win0);
      win0 = NULL;
  }

  return 0;

}
*/


//=====================================================================
  int UI_WinDefTx (MemObj *mo, void **data) {
//=====================================================================
// Das Textsizepanel (Standards/Text)

  static MemObj win0=GUI_OBJ_NEW, ent1, ent2, ent3, ent4;

  MemObj  box0, wtmp1;
  int    i1;
  char   *tx1, cbuf1[128], cbuf2[128], cbuf3[128];


  i1 = GUI_DATA_I1;

  printf("UI_WinDefTx %d\n",i1);


  switch (i1) {


    //---------------------------------------------------------
    case UI_FuncInit:

      if(GUI_OBJ_IS_VALID(&win0)) {           // Win schon vorhanden ?
        // gtk_widget_destroy (win0);
        return -1;
      }

      win0 = GUI_Win__ ("Textsize",UI_WinDefTx,"");

      box0 = GUI_box_v (&win0, "");

      memspc011[0] = '\0';
      UTX_add_fl_u (memspc011, AP_txsiz);
      ent1 = GUI_entry__ (&box0, " Notes:  ", memspc011, NULL,NULL, "8");
      GUI_Tip  (" size of textnotes (N)");

      memspc011[0] = '\0';
      UTX_add_fl_u (memspc011, AP_txdimsiz);
      ent2 = GUI_entry__ (&box0, " Dimensions:", memspc011, NULL,NULL, "8");
      GUI_Tip  (" size of text of Dimensions");

      memspc011[0] = '\0';
      UTX_add_fl_u (memspc011, GR_tx_scale);
      ent3 = GUI_entry__ (&box0, " GeneralTextScale:",memspc011,NULL,NULL,"8");
      GUI_Tip  (" ScaleFactor for all Notes, Dimensions");


      // memspc011[0] = '\0';
      // UTX_add_fl_u (memspc011, GR_tx_scale);
      sprintf(memspc011, "%d",AP_txNkNr);
      ent4 = GUI_entry__ (&box0, " Dec.Places:", memspc011, NULL,NULL, "8");
      GUI_Tip  (" Nr.of digits after comma / Anzahl Nachkommstellen");

      GUI_sep_h (&box0, 5);

      wtmp1 = GUI_box_h (&box0, "");
        GUI_button__ (&wtmp1, " Modify ", UI_WinDefTx, &GUI_FuncUCB1, "e");
        GUI_button__ (&wtmp1, " Reset  ", UI_WinDefTx, &GUI_FuncUCB2, "e");
        GUI_button__ (&wtmp1, " Exit   ", UI_WinDefTx, &GUI_FuncKill, "e");


      GUI_Win_up (NULL, &win0, 0);  // always on top
      GUI_Win_go (&win0);

      break;




    //---------------------------------------------------------
    case UI_FuncUCB1:

      tx1 = GUI_entry_get (&ent1);
      AP_txsiz = atof(tx1);

      tx1 = GUI_entry_get (&ent2);
      AP_txdimsiz = atof(tx1);

      tx1 = GUI_entry_get (&ent3);
      GR_tx_scale = atof(tx1);

      tx1 = GUI_entry_get (&ent4);
      AP_txNkNr = atoi(tx1);

      goto L_update;



    //---------------------------------------------------------
    case UI_FuncUCB2:                   // Reset
      sprintf(cbuf3,"%sgCAD3D.rc",OS_get_cfg_dir());
      printf(" setupFile=|%s|\n",cbuf3);
        i1 = UTX_setup_get (cbuf1, "NTEXTSIZ", cbuf3, 1);
        if(i1 >= 0) {
          GUI_entry_set (&ent1, cbuf1);
          AP_txsiz = atof(cbuf1);
        }

        i1 = UTX_setup_get (cbuf1, "DIMTEXTSIZ", cbuf3, 1);
        if(i1 >= 0) {
          GUI_entry_set (&ent2, cbuf1);
          AP_txdimsiz = atof(cbuf1);
        }

        i1 = UTX_setup_get (cbuf1, "GTEXTSIZ", cbuf3, 1);
        if(i1 >= 0) {
          GUI_entry_set (&ent3, cbuf1);
          GR_tx_scale = atof(cbuf1);
        }

        i1 = UTX_setup_get (cbuf1, "DNTEXT", cbuf3, 1);
        if(i1 >= 0) {
          GUI_entry_set (&ent4, cbuf1);
          AP_txNkNr = atoi(cbuf1);
        }

      goto L_update;




    //---------------------------------------------------------
    case UI_FuncKill:
      printf("UI_WinDefTx exit\n");

      GUI_Win_kill (&win0);
      win0 = GUI_OBJ_INVALID();
  }

  return 0;



  L_update:
    // printf("UI_WinDefTx work %f %f\n",AP_txsiz,AP_txdimsiz);

    GR_InitGFPar (AP_txdimsiz);

    ED_Reset ();                // reset - alles neu
    ED_work_END (0);

    // printf("ex UI_WinDefTx kill %f %f\n",AP_txsiz,AP_txdimsiz);

    return 0;

}


//=====================================================================
  int UI_WinAppr (MemObj *mo, void **data) {
//=====================================================================
// Das DXF-Import Panel ..



  int    i1, mode;
  // double dx, dy;
  char   *tx1, *tx2;
  double d1, d2;
  static MemObj win0=GUI_OBJ_NEW, box0, offset, ent_tol, ent_len,
                ent_ref, ent_obj;


  i1 = GUI_DATA_I1;
  printf("UI_WinAppr %d\n",i1);


  switch (i1) {

    //---------------------------------------------------------
    case UI_FuncInit:

      if(GUI_OBJ_IS_VALID(&win0)) {           // Win schon vorhanden ?
        // gtk_widget_destroy (win0);
        return -1;
      }

      win0 = GUI_Win__ ("Curve -> Circ",UI_WinAppr, "");

      box0 = GUI_box_h (&win0, "");

/*
      // X/Y-Offset
      offset = GUI_Entry (box0, " Nullpunkt X, Y, Z ","0,0,0",NULL, -100);
      GUI_Tip  ("die gesamte DXF-Geometrie um den angegebenen "
                "X/Y-Wert verschieben");
*/


      // Default = UT_TOL_cv
      memspc011[0] = '\0';
      UTX_add_fl_u (memspc011, UT_TOL_cv);
      ent_tol = GUI_entry__ (&box0, "   Toleranz: ", memspc011, NULL,NULL, "6");
      GUI_Tip  (" max. Abweichung bei Approximation");

      d1 = UT_TOL_cv * 20.;
      memspc011[0] = '\0';
      UTX_add_fl_u (memspc011, d1);
      ent_len = GUI_entry__ (&box0, "   min.Len.: ", memspc011, NULL,NULL, "6");
      GUI_Tip  (" minimale Laenge der Kreisboegen / Linien");


      ent_ref = GUI_entry__ (&box0, "   Refsys:  R", "0", NULL,NULL, "6");
      GUI_Tip  (" Projektionsebene; Def=0 = X/Y (Z=0)");

      ent_obj = GUI_entry__ (&box0, "   Curve:  S", "20", NULL,NULL, "6");
      GUI_Tip  (" Objektliste; zB 20-99 (von S20 bis S99)");



      GUI_sep_h (&box0, 0);
      GUI_button__ (&box0, "    Test   ", UI_WinAppr, &GUI_FuncUCB2, "");

      GUI_sep_h (&box0, 0);
      GUI_button__ (&box0, "    Save   ", UI_WinAppr, &GUI_FuncUCB1, "");

      GUI_sep_h (&box0, 0);
      GUI_button__ (&box0, "    Exit   ", UI_WinAppr, &GUI_FuncKill, "");


      GUI_Win_up (NULL, &win0, 0);  // always on top
      GUI_Win_go (&win0);

      break;





    //---------------------------------------------------------
    case UI_FuncUCB2:    // save
    case UI_FuncUCB1:    // TEST
      printf("Appr work %d\n",i1);

      tx1 = GUI_entry_get (&ent_tol);
      d1 = atof(tx1);

      tx1 = GUI_entry_get (&ent_len);
      d2 = atof(tx1);

      tx1 = GUI_entry_get (&ent_obj);
      tx2 = GUI_entry_get (&ent_ref);

      mode = i1 - UI_FuncUCB + 1;   // 1 od 2
      AP_appr (tx1, tx2, d1, d2, mode);

      // Exit nach Save
      if(i1 == UI_FuncUCB1) {
        GUI_Win_kill (&win0);
        win0 = GUI_OBJ_INVALID();
      }
      break;




    //---------------------------------------------------------
    case UI_FuncKill:
      printf("UI_WinAppr exit\n");

      // EXIT
      GUI_Win_kill (&win0);


    case UI_FuncExit:
      win0 = GUI_OBJ_INVALID();

      // imply END-Button (Redraw)
      // UI_AP (UI_FuncSet, UID_but_END, NULL);
      ED_work_END (0);

  }

  return 0;

}


/*
//=====================================================================
  int UI_WinDxfImp (GtkWidget *parent, void *data) {
//=====================================================================
// Das DXF-Import Panel ..



  int    i1;
  double d1;
  char   *txt, *p1;
  static GtkWidget *win0=NULL, *box0, *offset, *scale;
  static int  iuser;




  printf("UI_WinDxfImp %d\n",INT_PTR(data));

  i1 = INT_PTR(data);


  switch (i1) {


    //---------------------------------------------------------
    case UI_FuncInit1: // DXF
    case UI_FuncInit2: // LWO
    case UI_FuncInit3: // 3DS
      iuser = i1;

      if(win0 != NULL) {           // Win schon vorhanden ?
        gtk_widget_destroy (win0);
      }

      win0 = GUI_Window ("Import File",UI_WinDxfImp,(void*)UI_FuncKill,0);

      box0 = GUI_box_h (win0, 0);

      // cb1=GUI_ckbutt__ (box0, "3D-Mode", TRUE, NULL, NULL, 0);
      // GUI_Tip  ("als 2D oder 3D-Elemente einlesen");

      // X/Y-Offset
      offset = GUI_Entry (box0, " Nullpunkt X, Y, Z ","0,0,0",NULL, -100);
      GUI_Tip  ("die gesamte DXF-Geometrie um den angegebenen "
                "X/Y-Wert verschieben");


      if(i1 != UI_FuncInit1) {   // nicht DXF
        scale = GUI_Entry (box0, " Scale ","1",NULL, -80);
      }


      GUI_Hsep (box0, 0);
      GUI_button__ (box0, "OK", UI_WinDxfImp, (void*)UI_FuncWork, 2);

      gtk_widget_set_uposition (win0, 0, 0);  // Pos of mainwin
      gtk_widget_show (win0);
      break;




    //---------------------------------------------------------
    case UI_FuncWork:
      printf("Imp work\n");

      // work ..
      // i1 = 1 - GTK_TOGGLE_BUTTON (cb1)->active;
      i1 = 0;
      txt = GUI_entry_get (offset);

      if(iuser != UI_FuncInit1) {   // nicht DXF
        d1 = strtod (GUI_entry_get (scale), &p1);
      }

      // if(iuser == UI_FuncInit1) AP_ImportDxf (i1, txt, memspc011);
      if(iuser == UI_FuncInit2) AP_ImportLwo (i1, txt, d1, memspc011);
      if(iuser == UI_FuncInit3) AP_Import3ds (i1, txt, d1, memspc011);

      // die Hauptdatei raus ins Editfenster
      ED_load__ ();
      // xa_fl_TxMem = 1; // weils veraend. wurde

      // imply END-Button (Redraw)
      // UI_AP (UI_FuncSet, UID_but_END, NULL);
      ED_work_END (0);

      // close win
      gtk_widget_destroy (win0);
      win0 = NULL;

      break;




    //---------------------------------------------------------
    case UI_FuncKill:
      printf("DxfImp exit\n");

      // EXIT
      gtk_widget_destroy (win0);
      win0 = NULL;
  }

  return 0;

}
*/

//=====================================================================
  int UI_WinToler (MemObj *mo, void **data) {
//=====================================================================
// Toleranzen ...


  static MemObj win0=GUI_OBJ_NEW,ent_msiz,ent_tpt,ent_tcv,ent_dcv,ckb_all;

  int    i1, iSm;
  char   *tx1, cbuf1[128], cbuf3[128];
  double d1;
  MemObj box0, wtmp1;



  i1 = GUI_DATA_I1;
  printf("UI_WinToler %d\n",i1);

  switch (i1) {


    //---------------------------------------------------------
    case UI_FuncInit:  // 90

      if(GUI_OBJ_IS_VALID(&win0)) {           // Win schon vorhanden ?
        // gtk_widget_destroy (win0);
        return -1;
      }

      win0 = GUI_Win__ ("Tolerances",UI_WinToler, "");

      box0 = GUI_box_v (&win0, "");


      ent_msiz = GUI_entry__ (&box0,"Modelsize ","",NULL,NULL, "8");
      MSG_Tip ("UIWTms");
      // GUI_Tip("berechnet alle folgenden Werte neu. Default ist 500 mm.");

      GUI_sep_h (&box0, 3);
      // GUI_label__(box0, "Berechnungstoleranzen:", 0);
      GUI_label__ (&box0, " Tolerances identical objects:", "l");

      // ent_tpt = GUI_label__(&box0, "  PointTol ", "il");
      ent_tpt = GUI_entry__ (&box0, "  PointTol ", "", NULL,NULL, "l,8");
      GUI_set_enable (&ent_tpt, 0);        // inactive

      ent_tcv = GUI_entry__ (&box0, "  CurveTol ", "", NULL,NULL, "l,8");
      MSG_Tip ("UIWTcv");
      // "max Abweichung Curve. (Tol. bei Umwandlung Curve -> Polygon). "
              // "Default ist 0.005 mm.");


      GUI_sep_h (&box0, 3);
      // GUI_label__(box0, "Toleranzen zur Darstellung:", 0);
                            // "Tolerances identical objects:", 0);
      // wtmp1 = GUI_label__(box0, "Tolerances display:          ", 0);
      GUI_label__ (&box0, " Tolerances display:", "l");

      ent_dcv = GUI_entry__ (&box0,"  Polygon ","",NULL,NULL, "l,8");
      MSG_Tip ("UIWTdi");
      // "max. Abweichung bei der Darstellung von Kreisen, Kurven. "
              // "Default ist 0.01 mm.");

      GUI_box_h (&box0, "");

        // printf(" WC_modact_ind=%d\n",WC_modact_ind);
      if(WC_modact_ind < 0) i1 = TRUE;    // MainModel: modify all sM's
      else                  i1 = FALSE;   // in subModel: modify only active sM.
      ckb_all = GUI_ckbutt__ (&box0, "all subModels", i1, NULL, NULL, "");
      MSG_Tip ("UIWTsm");

      wtmp1 = GUI_box_h (&box0, "");
        GUI_button__ (&wtmp1, "  Modify ", UI_WinToler, &GUI_FuncUCB1, "e");
        MSG_Tip ("UIWTmo");
        GUI_button__ (&wtmp1, "  Reset  ", UI_WinToler, &GUI_FuncUCB2, "e");
        MSG_Tip ("UIWTre");
        GUI_button__ (&wtmp1, "  Exit   ", UI_WinToler, &GUI_FuncKill, "e");
        GUI_Tip (MSG_const__(MSG_exit));

      GUI_Win_up (NULL, &win0, 0);  // always on top
      GUI_Win_go (&win0);

      goto L_update;



    //---------------------------------------------------------
    // Callback Button Modify; Modelsize APT_ModSiz changed ...
    case UI_FuncUCB1:  // 1001


      // einlesen ent_tcv u ent_dcv
      tx1 = GUI_entry_get (&ent_tcv);
      UT_TOL_cv = atof(tx1);
      if(UT_TOL_cv < UT_TOL_min1) UT_TOL_cv = UT_TOL_min1;

      tx1 = GUI_entry_get (&ent_dcv);
      UT_DISP_cv = atof(tx1);
      if(UT_DISP_cv < UT_TOL_min1) UT_DISP_cv = UT_TOL_min1;
      // printf("  UT_TOL_cv=%f UT_DISP_cv=%f\n",UT_TOL_cv,UT_DISP_cv);

      // get checkbox all_subModels
      iSm = GUI_ckbutt_get (&ckb_all);  // 0=not sel, 1=selected.
      // printf(" allSm=%d\n",iSm);

      // einlesen ent_msiz; wenn != APT_ModSiz alle Werte neu berechnen
      tx1 = GUI_entry_get (&ent_msiz);
      d1 = atof(tx1);
      // printf("Toler UCB1 %f %f\n",d1,APT_ModSiz);

      if(UTP_comp2db(d1, APT_ModSiz, UT_TOL_min1) == 1) goto L_update;

      APT_ModSiz = d1;
      goto L_reset;



    //---------------------------------------------------------
    // Reset
    case UI_FuncUCB2:   // 1002

      // APT_ModSiz = 500.;
      sprintf(cbuf3,"%sgCAD3D.rc",OS_get_cfg_dir());
      printf(" setupFile=|%s|\n",cbuf3);
        i1 = UTX_setup_get (cbuf1, "MODELSIZ", cbuf3, 1);
        if(i1 >= 0) {
          GUI_entry_set (&ent_msiz, cbuf1);
          APT_ModSiz = atof(cbuf1);
        }

      goto L_reset;




    //---------------------------------------------------------
    case UI_FuncKill:
      printf("Toler exit\n");

      // EXIT
      GUI_Win_kill (&win0);


    case UI_FuncExit:
      win0 = GUI_OBJ_INVALID();
      return 0;

  }



  //=========================================================
  L_reset:

      WC_Init_Tol ();      // errechnet alle Toleranzen aus APT_ModSiz
      GL_InitModelSize (APT_ModSiz, 1);
      // GL_ChangeModelSize (APT_ModSiz);


  // ausgeben der Werte
  L_update:

    // update subModels
    if(iSm == 1) {
      // printf(" update subModelTol %f\n",APT_ModSiz);
      Mod_allmod_MS (APT_ModSiz,UT_TOL_cv,UT_DISP_cv);
    }


    // update windowFields

    // ent_msiz Modelsize
    cbuf1[0] = '\0';
    UTX_add_fl_u (cbuf1, APT_ModSiz);
    GUI_entry_set (&ent_msiz, cbuf1);


    // ent_cpt = compute - PunktTol  (nur Ausgabe)
    // strcpy(cbuf1, "PunktTol       ");
    cbuf1[0] = '\0';
    UTX_add_fl_u (cbuf1, UT_TOL_pt);
    GUI_entry_set (&ent_tpt, cbuf1);


    // ent_tcv = Kurventol.
    cbuf1[0] = '\0';
    UTX_add_fl_u (cbuf1, UT_TOL_cv);
    GUI_entry_set (&ent_tcv, cbuf1);


    // ent_dcv = display Polygon
    cbuf1[0] = '\0';
    UTX_add_fl_u (cbuf1, UT_DISP_cv);
    GUI_entry_set (&ent_dcv, cbuf1);



  L_display:
  APT_set_view_stat ();  // sonst ein opt. Rescale !
  // GL_DefineView (FUNC_ViewReset);  // immediate rescale ..


  // display modelsize in gtk-label
  UI_disp_modsiz ();


  // imply END-Button (Redraw)
  ED_Reset ();  // ED_lnr_act = 0;
  ED_work_END (0);



  return 0;

}


//================================================================
  int UI_disp_modsiz () {
//================================================================
// display modelsize in gtk-label

  char   s1[64];


  s1[0] = '\0';
  UTX_add_fl_u (s1, APT_ModSiz);
  GUI_label_mod (&UI_GrAttr, s1);   // 2011-07-06

  return 0;

}
 

//=================================================================
  void UI_Ed_del (int l_start, int l_end) {
//=================================================================

  // gtk_editable_delete_text(GTK_EDITABLE(text), i_startpos, i_endpos);

}



//=================================================================
  int UI_Ed_hili (int mode) {
//=================================================================
// Hintergrund Editfenster hilite ON / OFF
// mode = ON / OFF

  // printf("UI_Ed_hili %d\n",mode);

/*
  if(mode == ON) {
    gtk_widget_set_style(winED.win, UI_styl_1);
  } else {

    // printf("UI_Ed_hili reset\n");
    // gtk_widget_set_style(&winED, UI_styl_0);
    gtk_widget_restore_default_style (winED.win);
    // gtk_widget_reset_rc_styles (&winED);
  }
*/

  return 0;
}



//=================================================================
  void UI_Ed_ins (int cpos, char *txt, int cNr) {
//=================================================================

  GUI_edi_Insert (&winED, cpos, txt,  cNr);

}



//=================================================================
  int UI_Ed_sel_ln (long lNr) {
//=================================================================

  GUI_edi_sel_ln (&winED, lNr); // select Line, set Curpos to Line.

  return 0;

}


//=================================================================
  void UI_Ed_sel (long l_start, long l_end) {
//=================================================================
// auch zum CurPos setzen

  // printf("UI_Ed_sel %d %d\n",l_start,l_end);

  GUI_edi_sel__ (&winED, l_start, l_end);

  GUI_edi_scroll_s (&winED);

}


//=================================================================
  void UI_Ed_scroll () {
//=================================================================
// auch zum CurPos setzen

  // printf("UI_Ed_scroll\n");

  GUI_edi_scroll_s (&winED);

}




//=================================================================
  int UI_Ed_sel1 (char *cPos, int lLen) {
//=================================================================
// text selektiert darstellen
// auch zum CurPos setzen

  long  i1, i2;

  // printf("UI_Ed_sel1 %d\n",lLen);


  i1 = UTF_offset_ (cPos);
  i2 = i1 + lLen;               // Zeilenende
  UI_Ed_sel (i1, i2);           // den text selektiert darstellen


  return 0;
}


/*
//================================================================
  int UI_reset (int mode) {
//================================================================

  // reset "add to Group"
  GUI_menu_checkbox_set (GTK_CHECK_MENU_ITEM(ckb_grpAdd), FALSE);

  return 0;

}


//================================================================
  int UI_idle_CB (void *data) {
//================================================================
// Bei Retcod=0 wird idle_CB nicht mehr aufgerufen !

  printf("UI_idle_CB %d\n",INT_PTR(data));


  // gtk_idle_add (UI_idle_CB, (void*)0);

  return 1;

}


//=================================================================
  int UI_idle__ () {
//=================================================================


  gtk_idle_add (UI_idle_CB, (void*)0);

  return 0;

}
*/

//=================================================================
  void UI_AP (int func, int widgetID, void* data) {
//=================================================================
// das Hauptinterface zur App (APP ruft UI);
// Checkboxen liefern mit UI_FuncGet ein int zurueck; 0=selektiert, 1=nicht!
// data MUSS IMMER mit (void*) gecastet werden !

// func       see ../ut/func_types.h
// widgetID   see ../xa/xa_uid.h


  int    i1, i2, cpos, cpos1, cpos2;
  long   l1, l2;
  double d1;
  char   c1, cbuf[256];


  // printf("UI_AP %d %d\n",func, widgetID);


  switch(func) {

/*
    //==================================================================
    case UI_FuncInit:

    switch(widgetID) {

      case UID_WinMain:
        // Init und display Windows
        UI_win_main (NULL, (void*)UI_FuncInit);

        AP_UserSelection_reset ();
        AP_UserKeyIn_reset ();      // reset  KeyIn

        return;

    }
*/
    //==================================================================
    case UI_FuncWork:

    switch(widgetID) {

/* UNUSED
      case UID_WinMain:
        // Enter gtk-Mainloop; kein return von dieser Funktion.
        gtk_main();
        return;
*/

      case UID_WinEdit:
        // das Editfile von extern als veraendert melden
        xa_fl_TxMem = 1;
        return;

    }


    //==================================================================
    //==================================================================
    case UI_FuncGet:

    switch(widgetID) {


      case UID_WinEdit:
// SEE ED_unload__
        // Editfenster(gesamten APT-Text) > Memory kopieren
        // printf("Text->Mem modif=%d\n",xa_fl_TxMem);
        if(xa_fl_TxMem != 0) {      //Edittext - memory:  needUpdate
          L_Get_WinEdit_1:
          UTF_FilBuf0Len = UTF_FilBuf0Siz;
          i1 = GUI_edi_Read (UTF_FilBuf0, &UTF_FilBuf0Len, 0L, -1L, &winED);
          if(i1 < 0) {
            i2 = UTF_alloc__ (UTF_FilBuf0Len);
            if(i2 < 0) { TX_Error("out of mem .."); return; }
            goto L_Get_WinEdit_1;
          }
          xa_fl_TxMem = 0;
        }
        return;


      case UID_WinCAD:
        // check if CAD is activ; 0=Ja, 1=Nein.
        if(UI_InpMode == UI_MODE_CAD) {
          *((int*)data) = 0;
        } else {
          *((int*)data) = 1;
        }
        return;

      case UID_WinNC:
        // check if NC is activ
        if(UI_InpMode == UI_MODE_NC) {
          *((int*)data) = 0;
        } else {
          *((int*)data) = 1;
        }
        return;

      case UID_WinWCGEO:
        // check if WC is activ
        if(UI_InpMode == UI_MODE_WCGEO) {
          *((int*)data) = 0;
        } else {
          *((int*)data) = 1;
        }
        return;

      case UID_WinWCTEC:
        // check if WC is activ
        if(UI_InpMode == UI_MODE_WCTEC) {
          *((int*)data) = 0;
        } else {
          *((int*)data) = 1;
        }
        return;



      // case UID_Edit_Line:
        // // die aktuelle Zeile aus dem Editor einlesen
        // GUI_Ed_RdLnAct (&winED, (char*)data, &i1);
        // return;


      case UID_Edit_Cpos:
        // die CursorPosition holen
        *((int*)data) = GUI_edi_getCpos (&winED);
        return;


      // case UID_ckb_search:
        // // Status der Checkbox "suchen" abfragen
        // i1 = GUI_ckbutt_get (&ckb_mdel);
        // *((int*)data) = 1 - i1;
        // return;


      case UID_ckb_comp:
        // Status der Checkbox "compile DLL" abfragen
        i1 = GUI_menu_checkbox_get (&ckb_compl);  // 0=not sel, 1=selected.
        *((int*)data) = 1 - i1;
        return;


      case UID_ckb_Iact:
        // Status der Checkbox "Interaktion" abfragen
        // i1 = GUI_menu_checkbox_get (&ckb_Iact);      // 0=not sel, 1=selected.
        i1 = GUI_ckbutt_get (&ckb_Iact);      // 0=not sel, 1=selected.
        *((int*)data) = 1 - i1;
        return;


      case UID_ckb_impTyp:
        // Status der Checkbox "Import native/Mockup" abfragen
        i1 = GUI_menu_checkbox_get (&ckb_impnat);  // 0=not sel, 1=selected.
        *((int*)data) = 1 - i1;
        return;


      // case UID_ckb_hide:
        // i1 = GTK_TOGGLE_BUTTON (ckb_hide)->active;
        // *((int*)data) = 1 - i1;
        // return;


      // case UID_ckb_view:
        // i1 = GTK_TOGGLE_BUTTON (ckb_view)->active;
        // *((int*)data) = 1 - i1;
        // return;


      // case UID_ckb_nam:
        // Status der Checkbox "Names" abfragen
        // i1 = GTK_TOGGLE_BUTTON (ckb_nam)->active;
        // i1 = GUI_menu_checkbox_get (&ckb_nam);
        // *((int*)data) = 1 - i1;
        // printf("XXXXXXXXXX ckb_nam = %d XXXXXXXXXXXX\n",i1);
        // return;

/*
      case UID_ckb_txt:
        // Status der Checkbox "Text" abfragen
        // i1 = GTK_TOGGLE_BUTTON (ckb_txt)->active;
        i1 = GUI_menu_checkbox_get (&ckb_txt);
        *((int*)data) = 1 - i1;
        return;
*/

/* replaced by UI_stat_3D
      case UID_ckb_3D:
        // Status der Checkbox "3D" abfragen
        i1 = GUI_ckbutt_get (&ckb_3D);
        *((int*)data) = 1 - i1;
        return;
*/

/* replaced by UI_stat_shade
      case UID_ckb_shd:
        // Status der Checkbox "Shade" abfragen
        i1 = GUI_ckbutt_get (&ckb_shd);
        *((int*)data) = 1 - i1;
        return;
*/

      case UID_ckb_bound:
        // Status der Checkbox "Boundary" abfragen
        i1 = GUI_menu_checkbox_get (&ckb_bound);
        *((int*)data) = 1 - i1;
        return;


      case UID_ckb_vwz:
        // Status der Checkbox "ViewZ" abfragen
        i1 = GUI_ckbutt_get (&ckb_vwz);
        *((int*)data) = 1 - i1;
        // printf("Stat ckb_vwz = %d\n",i1);
        return;




    }


    //==================================================================
    //==================================================================
    case UI_FuncSet:

    switch(widgetID) {

/* UNUSED
      case UID_WinMain:
        return;
*/


      case UID_WinEdit:
        if(UI_InpMode != UI_MODE_MAN) return;
        GUI_edi_Write (&winED, UTF_FilBuf0, &UTF_FilBuf0Len, UTF_FilBuf0Siz);
        UI_Ed_fsiz = UTF_FilBuf0Len;
        ED_Reset ();  // ED_lnr_act = 0;
        return;


      case UID_Edit_Line:
        // Text aus data in die aktuelle Zeile an CurPos l2 einfuegen
        // Text in den Editor; noch nicht in Memory !
        l1 = strlen((char*)data);
        l2 = GUI_edi_getCpos (&winED);
          // printf(" txtInsert |%s| pos=%ld len=%ld\n",(char*)data,l2,l1);
        // insert text data at position i1
        GUI_edi_Insert (&winED, l2, (char*)data, l1);
        // den Cursor neu positionieren
        AP_ED_cPos = l2 + l1;
        GUI_edi_setCpos (&winED, AP_ED_cPos);
        //Edittext - memory:  needUpdate
        xa_fl_TxMem = 1;
        // ED_newPos (); //2013-04-10
        return;


/* UNUSED ?
      case UID_Edit_LnNxt:
        // Den Cursor an den Anfang der nxt Line
        GUI_Ed_setCnxtL (&winED);
        return;
*/

/* UNUSED ?
      case UID_Edit_CEOF:
        // wenn Cursor am EOF und last Char kein LF: ein LF eingeben
        i1 = GUI_edi_getCpos (&winED);   // CurPos holen
        if(i1 < UTF_FilBuf0Len) return;
        // EOF-Pos holen
        c1 = GUI_Ed_RdChr (&winED, -1);
        if(c1 == 10) return;
        if(c1 == 0) return;  // startpos
        // add Line-Term
        GUI_edi_Insert (&winED, i1, term_buf, term_anz);
        // den Cursor ans Ende
        // GUI_Ed_setCpos (&winED, i1 + term_anz);
        //Edittext - memory:  needUpdate
        xa_fl_TxMem = 1;
        return;


      case UID_Edit_Cpos:
        // die CursorPosition setzen
        // Achtung: UTF_FilBuf0Len muss aktuell sein !
        // if((int)data < 0) {
        if(data == NULL) {       // NULL = ans EOF
          l1 = -1L;
          // if(AP_src == AP_SRC_EDI) l1 = GUI_Ed_getEof (&winED);
          // else  l1 = UTF_FilBuf0Len;
        // } else {
          // l1 = INT_PTR(data);
        }
          // printf("UI_AP UID_Edit_Cpos: CurPos -> %ld\n",l1);
        GUI_edi_setCpos (&winED, l1);
        // AP_search_init ();
        return;


      case UID_ckb_search:
        // Checkbox ein/ausschalten
        // gtk_toggle_button_set_state (&ckb_mdel, INT_PTR(data));
        return;
*/


      case UID_ckb_comp:
        // Checkbox ein/ausschalten
        GUI_menu_checkbox_set (&ckb_compl, INT_PTR(data));
        return;


      case UID_ckb_vwz:
        // Reset ViewZ
        GUI_ckbutt_set (&ckb_vwz, 0);
        return;


      case UID_ckb_CAD:
        GUI_radiobutt_set (&ckb_cad); // TRUE=1
        return;


      case UID_ckb_MAN:
        GUI_radiobutt_set (&ckb_man); // TRUE=1
        return;


      case UID_but_END:
        // imply END-Button.
        // UI_butCB (NULL, (void*)"butEND");  Abtsurz !! ?
        ED_work_END (0);
        return;


      case UID_inf_sip:
        // den StartIndex Points setzen
        sprintf(cbuf, "%6ld", LONG_PTR(data)); // 999999 rechtsbuendig mit blanks
        // GUI_entry_set ((GtkEntry*) (UI_inf_sip), cbuf);
        return;


      case UID_ouf_scl:
        // Ausgabefeld Scale setzen
        // sprintf(cbuf, "%+10.3f",GL_get_Scale());
        AP_Get_scale (&d1);
        sprintf(cbuf, "%+10.3f",d1);
        GUI_label_mod (&UI_ouf_scl, cbuf);
        return;


      case UID_ouf_lNr:
        // Ausgabefeld ZeilenNummer setzen
          // printf(" set LineNr=%d\n",(long)data);
        sprintf(cbuf, "%6ld", LONG_PTR(data));
        GUI_label_mod (&UI_lNr, cbuf);
        return;


      case UID_ouf_grpNr:
        // Ausgabefeld Anzahl objs in group setzen
          // printf(" set LineNr=%d\n",(long)data);
        sprintf(cbuf, "%6ld", LONG_PTR(data));
        GUI_label_mod (&UI_grpNr, cbuf);
        return;


      case UID_ouf_vwz:
        // Ausgabefeld UI_view_Z setzen
        d1=*((double*)data);
        sprintf(cbuf, "%.2f",d1);
        // cbuf[0] = '\0'; UTX_add_fl_u(cbuf,d1);
        // printf("sur=%f  |%s|\n",d1,cbuf);
        GUI_entry_set (&UI_view_Z, cbuf);
        return;

/*
      case UID_ouf_coz:
        // Ausgabefeld UI_sur_Z setzen
        d1=*((double*)data);
        sprintf(cbuf, "%.2f",d1);
        // cbuf[0] = '\0'; UTX_add_fl_u(cbuf,d1);
        // printf("sur=%f  |%s|\n",d1,cbuf);
        GUI_entry_set ((GtkEntry*) (UI_sur_Z), cbuf);
        return;
*/

      case UID_Main_title:                // Title oben auf den Mainwinrahmen
        sprintf(cbuf,"%s   Mdl: ", INIT_TXT);
        if(data == NULL) {
          strcat(cbuf, AP_mod_fnam);
        } else {
          strcat(cbuf, (char*)data);
        }
        strcat(cbuf, "     Dir: ");
        if(strlen(AP_mod_sym) > 0) strcat(cbuf, AP_mod_sym);
        else                        strcat(cbuf, AP_mod_dir);
        // strcat(cbuf, "     Dir.Out: ");
        // if(strlen(AP_mod_sym) > 0) strcat(cbuf, AP_mod_sym);
        // else                        strcat(cbuf, AP_mod_dir);
        // GUI_WinTit (&winMain, cbuf);
        GUI_WinTit (&winMain, cbuf);
        return;

    }



    //==================================================================
    //==================================================================
    case UI_FuncFocus:

    switch(widgetID) {


      case UID_WinEdit:
        // Focus aus EditWindow
        GUI_edi_Focus (&winED);
        return;

/*
      case UID_WinCAD:
        // Focus aus Hauptwindow
        gtk_window_activate_focus (UI_MainWin);
        return;
*/
    }




    //==================================================================
    //==================================================================
    case UI_FuncEnable:
/* unused ?

    switch(widgetID) {

      case UID_Edit_Line:
        // die aktuelle Zeile selektieren
        cpos = GUI_edi_getCpos (&winED);
        // get starting a. ending Pos of line
        GUI_Ed_getLpos  (&cpos1, &cpos2, cpos, &winED);

        // Cursor darf man hier nicht veraendern, sonst wird das Selectfield
        // ebenfalls falsch gesetzt

        // select_region von cpos bis cpos (und scroll!)
        GUI_Ed_sel__ (&winED, cpos1, cpos2);

        return;

      case UID_ckb_search:
        GUI_set_enable (&ckb_mdel, TRUE);  // make S/M/DEL selectable
        return;

    }
*/




/* UNUSED ?
    //==================================================================
    //==================================================================
    case UI_FuncInsert:

    switch(widgetID) {


      case UID_Edit_Line:
        // insert new line after current line
        cpos = GUI_edi_getCpos (&winED);
        // find eol
        cpos = GUI_Ed_getLend (&winED, cpos);
        // insert newline
        GUI_edi_Insert (&winED, cpos, "\n", 1);
        cpos += 1;
        GUI_edi_setCpos (&winED, cpos);
        //Edittext - memory:  needUpdate
        xa_fl_TxMem = 1;

        return;


    }
*/


    //==================================================================
    //==================================================================
/* UNUSED ?
    case UI_FuncKill:

    switch(widgetID) {

      case UID_Edit_Line:
        // den sel. text killen
        // GUI_Ed_sel_del (&winED);

        // die akt. Zeile loeschen
        cpos = GUI_edi_getCpos (&winED);
        // printf("cpos = %d\n",cpos);

        // get starting a. ending Pos of line
        GUI_Ed_getLpos  (&cpos1, &cpos2, cpos, &winED);
        // printf("cpos1/2 = %d %d\n",cpos1,cpos2);

        // del text from starting to ending Pos
        GUI_Ed_del  (&winED, cpos1, cpos2);

        //Edittext - memory:  needUpdate
        xa_fl_TxMem = 1;
        return;

    }
*/





    //==================================================================
    //==================================================================
    default:
      printf("UI_AP Error E001 %d\n",func);


  }
}


//================================================================
  int UI_def_browser () {
//================================================================

  GUI_Dialog_e2b ("Html-Browser:", OS_browser, 80, "OK", "Cancel");

  return 0;
    
}   

 
//================================================================
  int UI_def_editor () {
//================================================================

  GUI_Dialog_e2b ("Texteditor:", AP_editor, 80, "OK", "Cancel");

  return 0;
    
}   


/* ====================== EOF =========================== */

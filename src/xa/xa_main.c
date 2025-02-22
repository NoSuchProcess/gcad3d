//     xa_main.c          AP_search_init
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
2012-03-16 test if <gcad_dir_bas>icons/ exist  added. RF.

-----------------------------------------------------
*/
/*!
\file  ../xa/xa_main.c
\brief main () des Interaktiven gCAD 
\code
=====================================================
List_functions_start:

main              main for interactive gcad
AP_lngTab_get     get list of installed languages
AP_lngTab_set     set list of installed languages

List_functions_end:
=====================================================

\endcode *//*----------------------------------------

Batch-main ist in xa_batch.c.

*/

#ifdef _MSC_VER
#include "MS_Def0.h"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>              // islower ..


// Achtung: braucht mit w32 __pctype u ___mb_cur_max / unresolvbar !
//#include <ctype.h>

#include "../ut/ut_geo.h"         // OFF, ON ..
#include "../ut/ut_os.h"          // OS_get_bas_dir
#include "../ut/ut_txt.h"         // fnam_del
#include "../ut/ut_txTab.h"              // TxtTab

#include "../gui/gui__.h"
// #include "../ut/func_types.h"          // UI_FuncInit

#include "../xa/xa_ui.h"               // UI_win_main
#include "../xa/xa_uid.h"              // UID_Main_title
#include "../xa/xa_mem.h"
#include "../xa/xa_sele.h"             // Typ_goGeom
#include "../xa/xa.h"                  // AP_mod_fnam
#include "../xa/xa_msg.h"              // MSG_get_str
#include "../xa/gcad_version.h"        // INIT_TXT



//_____________________________________
// EXTERNALS:
// aus ../xa/xa.c:
extern int       AP_argNr;
extern char      **AP_argv;
extern TxtTab    AP_TxTab1;




//_____________________________________
// LOCALS:
static char *os_bin_dir;         // gcad_dir_bin
static char *os_bas_dir;         // gcad_dir_bas
static char *os_loc_dir;         // local
static char *os_cfg_dir;
static char *os_tmp_dir;
static char *os_doc_dir;
static char *os_ico_dir;


#define LNG_MAX_NR 16
typedef  char lngCode[4];
static lngCode lngTab[LNG_MAX_NR];
static int     lngNr;






//_____________________________________
// prototypes:
// int UI_GR_RECONFIG (void *data1, void *data2);



//================================================================
  int gdb_halt () {
//================================================================
// used for halting in debugger.
// break gdb_halt      // in gCAD3D.gdb

  printf(" gdb_halt ..\n");

  return 0;

}


//================================================================
  int main (int paranz, char* argv[]) {
//================================================================


  int    i1, i2, irc;
  char   dirLocal[256], txbuf1[512], txbuf2[512], *p1;
  double d1;
  MemObj win0;


  printf("+++++++++++++ Start xa 2016-03-16  %d\n",paranz);


  // mtrace ();   // malloc-tests


#ifdef _MSC_VER
    // hide the command-window
    OS_hide_win ();

#else
    // display Linux-Version, libc-Version, gtk-Version
    OS_config ();
#endif


  // init primary textBuffer
  UtxTab_init__ (&AP_TxTab1); 


/*
  // set Defaultlanguage
  strcpy(AP_lang, OS_get_lang());
    printf(" AP_lang=|%s|\n",AP_lang);
*/


  AP_argNr = paranz;
  AP_argv  = argv;
  for(i1=0; i1<paranz; ++i1) printf("arg[%d]|%s|\n",i1,AP_argv[i1]);


  // mit --g-fatal-warnings   bricht aber ganz ab !
  // gtk_init (&paranz, &argv);
  // system(". export LC_ALL=C");   // f sort
  memset (&AP_stat, '\0', sizeof(AP_stat));  // init with 0

  APP_act_typ = 0;

  AP_deb_fp = NULL;



  // get application-directories: OS_get_doc_dir
  AP_get_dir__ ();


  // get all directories
  strcpy(txbuf1, argv[0]);
  OS_Init_ (txbuf1);             // zenity ?


  // get system-language
  i1 = OS_get_lang (txbuf1);

  // set AP_lang = language
  strncpy(AP_lang, txbuf1, 3);
    printf(" AP_lang |%s|\n",AP_lang);

  // set list of loaded languages
  AP_lngTab_set ();


  // display debug-switch
#ifdef DEB
  strcpy(txbuf2, "ON");
#else
  strcpy(txbuf2, "OFF");
#endif
  printf("Debug-switch DDEB is %s\n",txbuf2);



  //----------------------------------------------------------------
  // test if directory os_tmp_dir exists; if not:
  // test if <gcad_dir_bas> == <gcad_dir_local>; if not:
  // create directory os_tmp_dir;
  //   extract <os_bas_dir>examples.gz here.
  // create directory os_cfg_dir;
  //   copy  <os_bas_dir>xa/* here.
  //   create <os_cfg_dir>dir.lst.

    // system("rm -rf /home/fwork/gCAD3D"); // TEST ONLY


  // create directory <local>
  if(!OS_checkDirExist (OS_get_loc_dir())) {
    sprintf(txbuf2, "cannot create directory %s.\n"
            " Fix gcad_dir_local in startup-script.", dirLocal);
    GUI_MsgBox(txbuf2);
    exit(0);
  }


  // create directory os_tmp_dir;
  if(!OS_checkDirExist (OS_get_tmp_dir())) {
    sprintf(txbuf2, "cannot create directory %s.\n"
            " Fix gcad_dir_local in startup-script.", OS_get_tmp_dir());
    GUI_MsgBox(txbuf2);
    exit(0);
  }


  // dirLocal = remove "tmp/" from the tmpdir;
  strcpy(dirLocal, OS_get_tmp_dir());
  dirLocal[strlen(dirLocal) - 4] = '\0';
    printf(" local:|%s| base:|%s|\n",dirLocal,OS_get_bas_dir());

  
  // // test if <os_bas_dir> == <gcad_dir_local>; if not:
  // if(!strcmp(OS_get_bas_dir(), dirLocal)) goto L_startup_defaults;

  // // test if <gcad_dir_bas>icons/ exist;
// MIST: /usr/share/gcad3d/icons existiert immer !
  // //   if yes: goto L_startup_defaults;  else unpack examples.gz


  // test if gCAD3D/cfg/gCAD3D.rc exists; if not: unpack examples.gz
  // sprintf(txbuf1, "%scfg/gCAD3D.rc",OS_get_bas_dir());
  sprintf(txbuf1, "%sgCAD3D.rc", OS_get_cfg_dir());
    printf(" test configfile |%s|\n",txbuf1);
  if(OS_checkFilExist (txbuf1, 1)) goto L_startup_defaults;



  
// LINUX ONLY:
// restore all cfg-files;  MS: TODO ..
#ifdef _MSC_VER
#else
  // POSTINSTALL:  extract <os_bas_dir>examples.gz -> local 
  sprintf(txbuf2, "cd %s && tar -xzf /usr/share/gcad3d/examples.gz",
          dirLocal);  //, OS_get_bas_dir());
    printf("%s\n",txbuf2);
  system(txbuf2);


/*
  // copy desktop-link -> ~/gCAD3D/gCAD3D.desktop
  sprintf(txbuf2, "cp -f /usr/share/gcad3d/gcad3d.desktop %s.",
          OS_get_loc_dir());
    printf("%s\n",txbuf2);
  system(txbuf2);
  sprintf(txbuf2, "chmod +x %sgcad3d.desktop",
          OS_get_loc_dir());
    printf("%s\n",txbuf2);
  system(txbuf2);
*/

#endif






/*
  //----------------------------------------------------------------
  // check / try to create tempDir (~/gCAD3D/tmp
  sprintf(txbuf1,"%s",OS_get_tmp_dir());
  i1 = OS_checkFilExist (txbuf1, 1);
  if(i1 == 0) {
    sprintf (txbuf2, "mkdir -p %s", txbuf1);
    printf ("%s\n",txbuf2);
    system (txbuf2);
    i1 = OS_checkFilExist (txbuf1, 1);
    if(i1 == 0) {
      sprintf (txbuf2,
        "cannot create temp-directory %s.\n Fix symbol gcad_dir_tmp.",
        txbuf1);
      GUI_Dialog (NULL, txbuf2);
      goto L_1;
    }
  }

  // check / try to create tempDir (~/gCAD3D/tmp
  sprintf(txbuf1,"%s",OS_get_tmp_dir());
  i1 = OS_checkDirExist(txbuf1);
  if(i1 == 0) {
    // printf ("**** Verzeichnis %s konnte nicht erzeugt werden\n",txbuf1);
    sprintf (txbuf2,
      "cannot create temp-directory %s.\n Fix symbol gcad_dir_tmp.",
      txbuf1);
    GUI_Dialog (NULL, txbuf2);
    goto L_1;
  }


  //----------------------------------------------------------------
  // Init; check / try to create Baseverzeichnisse
  // check / try to create ./dat
  // strcpy(txbuf1, OS_get_bas_dir ());
  // strcat(txbuf1,"dat");
  sprintf(txbuf1,"%sdat/",OS_get_bas_dir());
  i1 = OS_checkDirExist(txbuf1);
  if(i1 == 0) {    // 0=YES
    // printf ("**** Verzeichnis %s konnte nicht erzeugt werden\n",txbuf1);
    printf ("**** cannot create directory %s\n",txbuf1);
  }


  // check / try to create ./tmp
  sprintf(txbuf1,"%s",OS_get_tmp_dir());
  i1 = OS_checkDirExist(txbuf1);
  if(i1 == 0) {
    // printf ("**** Verzeichnis %s konnte nicht erzeugt werden\n",txbuf1);
    sprintf (txbuf2,
      "cannot create temp-directory %s.\n Fix symbol gcad_dir_tmp.",
      txbuf1);
    GUI_Dialog (NULL, txbuf2);
    goto L_1;
  }
*/


  //----------------------------------------------------------------
  L_startup_defaults:
    printf(" L_startup_defaults:\n");

  // test if <cfgdir>dir.lst exists
  sprintf(txbuf1, "%sdir.lst",OS_get_cfg_dir());
  if(!OS_checkFilExist(txbuf1, 1)) goto L_write_defaults;

  // test if <cfgdir>xa.rc exists
  sprintf(txbuf1,"%sxa.rc",OS_get_cfg_dir());
  if(OS_checkFilExist(txbuf1, 1)) goto L_normal_start;



    // allererster Start: Defaults generieren u schreiben
  L_write_defaults:
      printf(" first start ..\n");
    // create Defaults
    strcpy(AP_mod_fnam, "unknown.gcad");
    sprintf(AP_mod_dir, "%sdat",dirLocal);
    sprintf(AP_mod_dir, "%sdat",dirLocal);
    sprintf(AP_dir_prg, "%sprg",dirLocal);

    // get printer
#ifdef _MSC_VER
    sprintf(AP_printer, "print");
#else
    sprintf(AP_printer, "lpr -l -P%s",OS_get_printer());
#endif

    // // get browser
    // p1 = OS_get_browse_htm();
    // UTX_cp_left (AP_browser, p1, 79);

    // get editor
    p1 = OS_get_edi();
    UTX_cp_left (AP_editor, p1, 79);

    AP_defaults_write(); // defaults    -> ~/gCAD3D/cfg/xa.rc
    AP_defaults_dir();   // defaultdirs -> ~/gCAD3D/cfg/dir.lst


    // exit(0);  // TEST ONLY


  //==========================================================
  L_normal_start:

  // load gui-dll
  // sprintf(txbuf1, "%splugins/xa_gui_gtk_2.so",OS_get_bin_dir());
    // printf(" gui-dll=|%s|\n",txbuf1);
  // irc = OS_dll_global (txbuf1);    // dlopen
    // if(irc < 0) return -1;





  //==========================================================
/*
kopieren geht nicht mehr - 
  // copy tmp/ltyp.rc if it does not exist
  // VERSCHWINDET LEIDER MANCHMAL; UNGEKLAERT.
  sprintf(txbuf2, "%sltyp.rc",OS_get_cfg_dir());
  i1 = OS_checkFilExist(txbuf2, 1);
    // printf(" ltyp.rc: i1=%d\n",i1);
  if(i1 == 0) {
    sprintf(txbuf1, "%sxa%cltyp.rc",OS_get_bas_dir(),fnam_del);
      printf("copy |%s|%s|\n",txbuf1,txbuf2);
    OS_file_copy (txbuf1, txbuf2);                // old new
  }
*/



  // Init und display Windows
  L_1:

  // read Defaults from <base>/cfg/xa.rc
  AP_defaults_read ();
  Mod_sym_get2 (AP_mod_sym, AP_mod_dir, 0);

  // init MSG-system
  MSG_const_init (AP_lang);
  MSG_Init (AP_lang);
  OMN_init ();

  // Linetypes laden erfolgt erst in DL_InitAttTab

  // create list of *.dll (*.so) in tmp/Dll.lst
  AP_DllLst_write ();       // create <tmp>plugins.lst
  PRC_lst_processors ();    // create <tmp>cadprocessors.lst


  // init gtk
      // GUI_Init ("");     // Read Colours und Fonts
      GUI_Init__ ("");     // Read Colours und Fonts
      // GUI_Init ("xa/xa.rc");

      Ico_init ();       // create all icons


  // // alle Pfade aus Datei path.setup auslesen und speichern
  // Mod_init_path ();


  // Init und display Windows
  // UI_AP (UI_FuncInit, UID_WinMain, NULL);

  // Init und display Windows
  UI_win_main (&win0, GUI_SETDAT_EI(TYP_EventPress,UI_FuncInit));
  // starts Grafic-window GUI_gl__ UI_GL_draw__ ..


  AP_UserSelection_reset ();
  AP_UserKeyIn_reset ();      // reset  KeyIn


  AP_stat.sysStat = 1;

  // test only - display if messagfile is missing
  MSG_const_init (AP_lang);
  MSG_Init (AP_lang);


  // INIT_TXT = gCAD3D Version <Version> <Datum>
  strcpy(txbuf1, INIT_TXT);
  // GUI: GTK<version> - <subversion>
  GUI_get_version (txbuf2, &i1, &i2);
  TX_Print("%s     GUI: %s%d.%d", txbuf1, txbuf2, i1, i2);


  // strcat(txbuf1, " - Host ");
  // os_l_id (memspc50);
  // strcat(txbuf1, memspc50);
  // TX_Write (txbuf1);


  // display init-text
  MSG_pri_0 ("MM0");
  // TX_Write ("display funcions:");
  // TX_Write ("move:   Shift + drag mouse (do not push mousebuttons)");
  // TX_Write ("rotate: Ctrl + drag mouse");
  // TX_Write ("zoom:   Shift + Ctrl + drag mouse");


  // // init tolerances
  // WC_Init_Tol();

  // // init View-Plane and ConstrPlane  AP_src_new
  // AP_Init_planes ();

  // APT-Texbuffer fuer ersten Start
  UTF_alloc__ (100);

  // (re)create GLU-tess-obj
  // GLT_init__ ();


/* 2012-07-30
  // set GL-shaden
  UI_wireCB (NULL, NULL);

  // config OpenGL-window
  UI_GR_idle_GL1_CB (NULL);       // emit configure_event (UI_GR_GLInit)
  // GUI_update__ ();
  // AP_stat.sysStat = 2;


  // configure_event
  if(AP_stat.sysStat < 2) UI_GR_Reshape (NULL, NULL);


  // unbedingt erforderl wenn kein RUN - sonst Achsenkreuz ganz schief
  UI_GR_view (NULL, (void*)"IsoView");
*/


  // default ist VWR
  UI_src_mem (0);

  // clear src-Memory, reset Undo, Hide, View-Plane, ConstrPlane.
  // AP_src_new ();    // raus 2009-06-20  (kills all Model* for reLoad)


  // enable selection of all types
  UI_set_Ins_Sel_Cat (0);  // 0=FALSE

/*
  // die Startparameter abarbeiten
  i1 = 1;
  i2 = 0;
  // printf(" vor AP_work__ %d\n",paranz);
  L_next_par:
  if(paranz > i1) {
      printf(" nxtpar |%s|\n",argv[i1]);
    if(!strcmp(argv[i1], "new")) i2 = 1;       // new - kein defLoad !
    irc = AP_work__ (argv[i1], argv[i1+1]);
    if(irc > 0) i1 += irc;
    ++i1;
    goto L_next_par;
  }


  // wenn new innerhalb der Inputdaten nix laden
  if(i2 > 0) goto L_start9;
*/


/* 2012-07-30
  // Datei AP_mod_fnam laden (nur wenn noch nix geladen)
  // tmp/Model zerteilen und laden
    // printf(" FilBuf0Len=%d\n",UTF_GetLen0());
  if(UTF_GetLen0() < 24) {
    AP_defLoad (0);
    // sprintf(txbuf1,"%stmp%cModel",OS_get_bas_dir(),fnam_del);
    // Mod_load__ (txbuf1);
    // ED_work_END (0); // ABARBEITEN
  }
*/

  //================================================================
  L_start9:

  // UI_butCB (NULL, "VWR");
  // sele_set__ (Typ_goGeom);    // enable selection of all types (mode "VWR")
  // disactivate menu Insert Select CATALOG,


/* 2012-07-30
  // Title oben auf den Mainwinrahmen
  UI_AP (UI_FuncSet, UID_Main_title, NULL);
*/


  // Startup fertig; in debug: give chance to set watchpoints
  AP_debug__ ("main"); // die func ist ganz leer; ist als stoppoint definiert.



  // mtrace geht leider nicht, da gtk und Mesa zu viele Stoerzeilen machen ..
// #ifdef DEB
  // mtrace(); // starts to write all mallocs/frees --> File <$MALLOC_TRACE>
// #endif



  // UI_idle__ ();


  // // im Modus CAD starten
  // gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON(ckb_cad), TRUE);

/*
  // load last application (ex AP_defaults_read)
  if(strlen(APP_act_nam) > 0) {
    strcpy(txbuf1, APP_act_nam);
#ifdef _MSC_VER
    strcat(txbuf1, ".dll");
#else
    strcat(txbuf1, ".so");
#endif
    DLL_run2 (txbuf1, 0);
  }
*/

  // redraw with first idle ..
  // GUI_Idle (UI_GR_RECONFIG, NULL);    // 2012-07-30
  // GUI_Idle (UI_GR_idle_CB, NULL);

  // Enter gtk-Mainloop; kein return von dieser Funktion.
  // UI_AP (UI_FuncWork, UID_WinMain, NULL);
  GUI_Win_go (&win0);


  return 0;

}


//================================================================
  int AP_get_dir__ () {
//================================================================
// get application-directories
// OS_get_doc_dir

  char    *p1, *p2, s1[512];


  printf("AP_get_dir__ \n");


  //----------------------------------------------------------------
  // os_bin_dir                        ("/home/fwork/binLinux32/")
  p1 = getenv("gcad_dir_bin");
  if(p1) {

  } else {
    // get exename with full path.
    p1 = OS_prc__ ();

    // remove exename.  find last "/"   TODO: Linux only
    p2 = strrchr(p1, fnam_del);  // '/' or '\\'
    if(p2 == NULL) {
      strcpy(s1, "Directory \"gcad_dir_bin\" not found; Installationproblem.\n"
             " Fix gcad_dir_bin in startup-script.");
      GUI_MsgBox (s1);
      exit(0);
    }
    ++p2;          // keep '/'
    *p2 = '\0';    // overwrite exename.
  }
    // printf(" p1=|%s|\n",p1);

  if(strlen(p1) > 255) goto L_err2;
  strcpy(s1, p1);
  UTX_add_fnam_del (s1);

  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_bin_dir, &AP_TxTab1);




  //----------------------------------------------------------------
  // get os_bas_dir  (examples.gz,icons/)        ("../")
  // get "/usr/share/gcad3d/"
  p1 = getenv("gcad_dir_bas");
  if(p1) {
    if(strlen(p1) > 255) goto L_err2;
    strcpy(s1, p1);
    UTX_add_fnam_del (s1);

  } else {
#ifdef _MSC_VER
    // MS-WIN
    strcpy(s1, os_bin_dir);
    UTX_endDelChar (s1, fnam_del);    // remove last char ('/')
    UTX_endDelWord (s1, fnam_del, 0); // remove last word (keep '/')
    // strcat(s1, "icons\\");
#else
    // Linux
    strcpy(s1, "/usr/share/gcad3d/");
#endif
  }
    printf(" gcad_dir_bas = |%s|\n",s1);

  if(!OS_checkFilExist(s1, 1)) {
    strcpy(s1, "Directory \"gcad_dir_bas\" not found; Installationproblem.\n"
           " Fix gcad_dir_bas in startup-script.");
    GUI_MsgBox (s1);
    exit(0);
  }

  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_bas_dir, &AP_TxTab1);



  //----------------------------------------------------------------
  // get os_loc_dir from get gcad_dir_local or $HOME   ("/home/fwork/gCAD3D/")
  p1 = getenv("gcad_dir_local");
  if(!p1) {
    p1 = getenv("HOME");
      // printf(" HOME = |%s|\n",p1);
    if(!p1) {
      strcpy(s1, "Cannot locate $HOME for a temporary directory.\n"
             " Fix gcad_dir_local in startup-script.");
      GUI_MsgBox (s1);
      exit(0);
    }
  } 
  if(strlen(p1) > 255) goto L_err2;
  
  strcpy(s1, p1);
  UTX_add_fnam_del (s1);   // add closing "/"
  strcat(s1, "gCAD3D");
  UTX_add_fnam_del (s1);   // add closing "/"
    // printf(" os_loc_dir = |%s|\n",s1);

  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_loc_dir, &AP_TxTab1);



  //----------------------------------------------------------------
  // os_tmp_dir         ("/home/fwork/gCAD3D/tmp/");
  strcpy(s1, os_loc_dir);
  strcat(s1, "tmp");
  UTX_add_fnam_del (s1);   // add closing "/"

  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_tmp_dir, &AP_TxTab1);



  //----------------------------------------------------------------
  // os_cfg_dir         ("/home/fwork/gCAD3D/cfg/");
  strcpy(s1, os_loc_dir);
  strcat(s1, "cfg");
  UTX_add_fnam_del (s1);   // add closing "/"

  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_cfg_dir, &AP_TxTab1);



  //----------------------------------------------------------------
  // os_doc_dir         ("/mnt/serv1/Devel/dev/gCAD3D/doc/"  html, msg)
  p1 = getenv("gcad_dir_doc");
  if(p1) {
    if(strlen(p1) > 255) goto L_err2;
    strcpy(s1, p1);
    UTX_add_fnam_del (s1);
  
  } else {
#ifdef _MSC_VER
    strcpy(s1, os_bin_dir);           // ("/home/fwork/binLinux32/")
    UTX_endDelChar (s1, fnam_del);    // remove last char ('/')
    UTX_endDelWord (s1, fnam_del, 0); // remove last word (keep '/')
    strcat(s1, "doc\\");
#else
    // strcpy(s1, "/usr/share/doc/gcad3d/");
    strcpy(s1, "/usr/share/gcad3d/doc/");
#endif
    p1 = s1;
  }

  if(!OS_checkFilExist(p1, 1)) {
    strcpy(s1, "Directory \"gcad_dir_doc\" not found; Installationproblem.\n"
           " Fix gcad_dir_doc in startup-script.");
    GUI_MsgBox (s1);
  }


  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_doc_dir, &AP_TxTab1);



  //----------------------------------------------------------------
  // os_ico_dir         ("/home/fwork/gCAD3D/icons/");
  //   OS-MS: can use os_bin_dir!
  p1 = getenv("gcad_dir_ico");
  if(p1) {
    if(strlen(p1) > 255) goto L_err2;
    strcpy(s1, p1);
    UTX_add_fnam_del (s1);

  } else {
#ifdef _MSC_VER
    strcpy(s1, os_bin_dir);           // ("/home/fwork/binLinux32/")
    UTX_endDelChar (s1, fnam_del);    // remove last char ('/')
    UTX_endDelWord (s1, fnam_del, 0); // remove last word (keep '/')
    strcat(s1, "icons\\");
#else
    strcpy(s1, "/usr/share/gcad3d/icons/");
#endif
  }

  if(!OS_checkFilExist(s1, 1)) {
    printf("***** os_ico_dir: %s\n",s1);
    strcpy(s1, "Directory \"os_ico_dir\" not found; Installationproblem.\n"
           " Fix gcad_dir_ico in startup-script.");
    GUI_MsgBox (s1);
  }


  UtxTab_add (&AP_TxTab1, s1);
  UtxTab_query (&os_ico_dir, &AP_TxTab1);



  //----------------------------------------------------------------
  return 0;

  L_err2:
      strcpy(s1, "Error: directoryname too long.\n");
      printf("%s\n",s1);
      GUI_MsgBox (s1);
      exit(0);

}


//================================================================
  int AP_lngNam_get (char *lngNam, char *lngCode) {
//================================================================
// was MSG_lng_init
// provide list of supported languages and language-names
//   find all msg_<LANG>.txt files  in <docdir>/msg/

  char *pl;

  // printf(" AP_lngNam_get |%s|\n",lngCode);

  // - get value of LANG__ of all existing language-files
  MSG_Init (lngCode);
  pl = MSG_get_str ("LANG__");
  if(!pl) return -1;

  strcpy (lngNam, pl);
    // printf(" lang = |%s|\n",lngNam);

  return 0;

}


//================================================================
  int AP_lngTab_get (char **lngLst) {
//================================================================
// was MSG_lng_init
// provide list of supported languages and language-names
//   find all msg_<LANG>.txt files  in <docdir>/msg/

  *lngLst = &lngTab[0][0];

  return lngNr;

}


//================================================================
  int AP_lngTab_set () {
//================================================================
// was MSG_lng_init
// provide list of supported languages and language-names
//   find all msg_<LANG>.txt files  in <docdir>/msg/

  int    ii, iNr, lNr;
  char   *p1, cbuf1[256], *pl;

// uu
// sssss();

  // printf("MSG_lng_init %d\n",*lngNr);


  //----------------------------------------------------------------
  // - make list of all <docdir>/msg/msg_*.txt

#ifdef _MSC_VER
  sprintf(cbuf1,"%smsg",OS_get_doc_dir());
#else
  sprintf(cbuf1,"%smsg/",OS_get_doc_dir());
#endif


  ii = strlen (cbuf1);
    printf(" _scan_- %d |%s|\n",ii,cbuf1);

  iNr = 0;
  OS_dir_scan_ (cbuf1, &iNr);   // Init
    // printf(" _scan_%d |%s|\n",ii,cbuf1);

  lNr = 0;
  for(;;)  {
    OS_dir_scan_ (cbuf1, &iNr);
      // printf(" _scan_%d |%s|\n",iNr,cbuf1);
    if(iNr < 0) break;
    p1 = strstr (&cbuf1[ii], "msg_");
    if(!p1) continue;

    // if(strncmp(&cbuf1[ii], "msg_", 4)) continue;
    p1 += 4;
    if(!strncmp(p1, "const", 4)) continue;


    // extract & copy language-code
    strncpy (lngTab[lNr], p1, 2);
    lngTab[lNr][2] = '\0';
      // printf(" n.scan |%s| %d |%s|\n",lngCode[lNr], lNr, cbuf1);

    if(lNr < LNG_MAX_NR) ++lNr;
  }



  //----------------------------------------------------------------
  lngNr = lNr;


    // TESTBLOCK
    printf("------ ex AP_lngTab_set %d \n",lNr);
    for(ii=0; ii<lNr; ++ii) printf(" %d |%s|\n",ii,lngTab[ii]);
    // END TESTBLOCK


  return 0;

}


//================================================================
  char* OS_get_loc_dir () {
//================================================================
/// returns confDir (with closing '/')  <gcad_dir_local>xa/

  return os_loc_dir;

}

//================================================================
  char* OS_get_cfg_dir () {
//================================================================
/// returns confDir (with closing '/')  <gcad_dir_local>xa/
      
  return os_cfg_dir;

}

//================================================================
  char* OS_get_tmp_dir () {
//================================================================
/// returns tempDir (with closing '/')  <gcad_dir_local>tmp/

  return os_tmp_dir;

}

//================================================================
  char* OS_get_doc_dir () {
//================================================================
/// OS_get_doc_dir           returns path of docu
           
  return os_doc_dir;

} 

//================================================================
  char* OS_get_ico_dir () {
//================================================================
/// returns directory for icons (with closing '/')  <gcad_dir_bin>icons/

  return os_ico_dir;

}

//================================================================
  char* OS_get_bin_dir () {
//================================================================
/// OS_get_bin_dir           returns path of executables, $gcad_dir_bin

  return os_bin_dir;

}

//================================================================
  char* OS_get_bas_dir () {
//================================================================
/// OS_get_bas_dir           returns path of ?

  return os_bas_dir;

}


//================================================================
  int TODO (char *fn, ...) {
//================================================================
/// OS_get_bas_dir           returns path of ?

  printf("************ TODO func %s () ***********\n",fn);


  return 0;

}




/* ====================== EOF =========================== */

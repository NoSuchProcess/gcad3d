// NC_CommandInterpreter - main
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
2013-03-14 remove BRK_AC BRK_ACLN TXA TXG from APT_work_PrgCmd (unused)
2001-11-02 APT_UP_* neu zu (CALL aus CALL "filenam"). RF.
2001-10-22 DL_Delete - fehler korr.
2001-10-19 Refsys gleich aktivieren, P=x,y,Z(zval) zu. RF.
2001-06-24 UTF_GetLinNr korr. RF.
2001-06-15 UI_ckb_ -> UI_AP
2001-06-05 Portierung Unix; ED_GetLinNr -> UTF_GetLinNr,
           DB_StoreObj -> DL_StoreObj

-----------------------------------------------------
*/
/*!
\file  ../ci/NC_Main.c
\brief CommandInterpreter - main 
\code
=====================================================
List_functions_start:

WC_Work__             mainFunc; test for submodels
WC_Work1              mainFunc
WC_Work_upd           call WC_Work1 with 0-terminated memory-line, restore.
APT_work_def          work DefinitionLine (decode, store in DB, display)
APT_work_parent_hide  hide parent-obj
APT_parent_set   set isParent-bit bei allen parents
APT_work_PrgCmd       work programcodes
APT_work_NCCmd
APT_stack_NCCmd

APT_ato_par_srcLn     get atomic-objects (ato1) and parents of sourceLine

APT_obj_ato  create struct from atomicObjs
APT_obj_expr  Create struct from ModelCode

APT_PP_FROM
APT_PP_Write_Lin
APT_PP_Write_Cir
APT_PP_Write
APT_PP_WriOut
APT_ausg_chTool
APT_ausg_NCCmd
APT_transl_rad
APT_transform
APT_transl2       rel coords -> abs coords
APT_rotate3       unused
APT_alloc1        realloc NC_stat__

APT_Draw__        draw (load from DB, APT_DrawXX)
APT_DrawPoint
APT_DrawLine
APT_DrawCirc
APT_DrawCurv
APT_DrawDim3
APT_DrawDimen
APT_DrawTxtG
APT_DrawTxtA
APT_DrawSymB
APT_DrawSol
APT_DrawPln
APT_DrawSur
APT_DrawModel

APT_hiliObj
APT_disp_obj      PT,LN,AC,POL2
// APT_disp_TxtG
APT_disp_TxtA
APT_disp_SymB     disp temp bitmap symbols SYM_TRI_S SYM_STAR_S ..
APT_disp_SymV     display symbols at 2D-pos (not rotated symbols)
APT_disp_SymV1    display symbols in x-y-plane (not rotated symbols eg SYM_CROSS)
APT_disp_SymV2    display oriented symbols in x-y-plane; eg SYM_ARROH
APT_disp_SymV3    disp temp oriented vector-symbols SYM_ARROW SYM_SQUARE ..
APT_disp_ln
APT_disp_ac
APT_disp_cv
APT_disp_nam      display objName at objPosition

APT_curv2ptArr    Polygonpoints into 2D-pointArray

APT_Lay_add

PP_up_list
APT_get_dreh
WC_ask_ModSiz     return APT_ModSiz   see WC_Init_Modsiz WC_Init_Tol
WC_ask_WC_mode    return WC_mode
WC_ask_actPos     return actPosU
WC_ask_actZsur    return WC_sur_Z
WC_ask_Zsur1      return WC_sur1
WC_ask_Zsur2      return WC_sur2

APT_do_auxCmd     handle change Graf. Attribut u. Refsys
APT_eval_if       evaluate expression
APT_UP_get        give APT_line_act of calling line ..
APT_UP_up         .
APT_UP_down
APT_line_set
AP_mdGet          query subModelLevel & APT-LineNr of active subModelCall
// WC_get_obj_typ    der Typ des zuletzt generierten Objekts
WC_get_obj_stat   0=OK, 1=workmode
WC_set_obj_stat   0=OK, 1=workmode
WC_Init_all       0=normal, 1=PP
WC_PP_open
WC_Init_Modsiz    see WC_ask_ModSiz
WC_Init_Tol       recompute all tolerances from APT_ModSiz
WC_Init
APT_Init
APT_reset_view_stat
APT_get_view_stat
APT_set_view_stat
UCS_Reset         see AP_Init_planes GL_SetConstrPln
APT_Reset
WC_EOF
WC_setDisp3D      switch 3D-mode/2D-mode
NC_set_actPos
NC_getGrafAtt     ret GR_Att_act
NC_setGrafAtt     set GR_Att_act
NC_setModSiz      set APT_ModSiz
NC_setRefsys      Change active Plane
WC_actPos_disp
WC_setPosKreuz    disp cross
WC_actPos_save
WC_actPos_dump
WC_actPos_reset

WC_ask_actObj

List_functions_end:
=====================================================

\endcode *//*----------------------------------------

// APT_decode__      new func: ATO_ato_srcLn__


===============================================================================
Compile:
cc -c `gtk-config --cflags` NC_Main.c
cl -c /I ..\include NC_Main.c




===============================================================================
Objekttypen:

 A=Area (Flaechen)
 B=Body (Solids)
 C=Circ
 D=Direction(Vector)
 E
 F
 G=Graf.Attribut
 H
 I=Interaction
 J
 K
 L=Line
 M=Model
 N=Note
 O
 P=Point
 Q
 R=Refsys (Plane?)
 S=Spline/Polygon/CCV
 T=Tooldef.
 U Union                        Userdata ?  Unknown ?
 V=NumVar
 W
 X     implizit fuer X-Coord (zb R=X(1))
 Y     implizit fuer Y-Coord
 Z     implizit fuer Z-Coord



 Layer dzt via Lay-Command; wenn es via Objekt waere (wie GrafAtt),
   koennte man es jedem Einzelobjekt zufuegen !






===============================================================================
Theorie Grafikobjektspeichern (DL):

  Jedes Obj wird in der DL neu gespeichert; es wird der Typ und der
  DB-index mitgespeichert.
  Bei Verarbeitung einer APT-Zeile wird der aktuelle DL-Index gemerkt;
  wird eine weiter zurueck liegende APT-Zeile zurueckgesprungen,
  so werden alle DL-Objekte ab dieser DL-Position geloescht.



------------------------------------------------------
Der 2D -> 3D-Mode (APT_2d_to_3d_mode):
 Die ab nun hereinkommenden Koordinaten werden als 2D-Koord. gespeichert;
 erst vor dem Anzeigen wird transformiert. Dadurch k�nnen alle
 Berechnungen (zB Schnitt 2-er Geraden) im 2D abgewickelt werden.


Der echte 3D-Modus (APT_3d_mode):
 Alle Definitionen erfordern nun volle 3D-Koordinaten; es wird auch ebenso
 gespeichert.



===============================================================================

Das Postproz. geht so:
 es wird zuerst alles aufgel�st in Einzelschritte uund in eine Zwischendatei
 ausgegeben. dann wird der PP gerufen, der die Zwischendatei abarbeitet.

Die Zwischendatei ist Klartext und enth�lt nur mehr Technologieanweisungen
  und Verfahrwege (�hnlich einem CLDATA).

Unterprogramme werden generell fuer den PP nicht aufgel�st. Nicht Unterprogrammf�hige
  Postprozessoren (Agie) muessen das also selbst tun.




----------------------------
Funktionen des Interpreters:
----------------------------

Geht noch nicht:

- Es gibt ein temporaeres/aktuelles Objekt, das ist zB P0, L0, C0.

- Zusammensetzten von Objektnamen zB: V(V1)=V(V20+10)
  Geht noch nicht!
  Am einfachsten via Funktion. Also die Funktion V braucht eine Zahl
  und definiert damit dann den Index.



ref 0,0                    # Referenzpunkt und (optional) Verdrehwinkel setzen
ref p1                     # Die momentane Position erh�lt die mit REF angegebenen
ref p1,a(60)               # Koordinaten (�hnlich dem FROM - Befehl).
                           # Beispiel: Die momentane Position ist 120,100;
                           # mit REF 20,10 wird der neue Nullpunkt auf 100,100
                           # gesetzt. P1[50,50] bezeichnet somit absolut 150,150.
                           #   Es wird das gesamte Koordinatensystem verschoben und
                           #   (optional) gedreht



macgen macnam              L�schen u. Neuanlegen eines (leeren) Macro.
macins macnam,Text         Hintanfuegen im Macro.



mch "fanuc"                # MACHINE; Definition Maschinenfunktionen / Post-
                           # prozessor.  Sollte die erste Anweisung sein.


home 0,0                   # Konturprogramm beenden. Entlang dieses Ausfahrweges
home p1                    # die Korrektur abschalten.




-------------------------------------------------------------------------------
NC-Commands:
  Problem ist der Zeitpunkt der Ausgabe, da die Ausgabe der Geometrie nachhinkt (bei
  Konik1 sogar 2 Elemente !
  Alle Eingabezeilen haben eine Nummer - (nicht die Zeilennummer, die ist beim 
    Fileinput ein Problem) -  APT_line_cnt. Diese wird dem Eingabeelement mitgegeben
    bis zu seiner Ausgabe (in objX.nam).
  Alle NC-Cmds werden zwischengespeichert (in insBuf; die aktuelle APT_line_cnt zus�tzl.
    in insCnt.
  Vor Ausgabe eines Geoelem. werden die f�lligen NC-Cmds ausgegeben.



-------------------------------------------------------------------------------
Draht rein / raus:
  APT_tl_stat ist die ToolNr; 0 = kein Tool.
  RP ver�ndert den Drahtstatus nicht.
  Mit G1/G2/G3 wird automatisch eingf�delt.
  Mit WC und WT kann Draht explizit abgetrennt / eingef�delt werden.
  Mit MV wird explizit ein WC gesetzt.



-------------------------------------------------------------------------------
Theorie Konikbearbeitung:
L10[x(10)]     Normales Abarbeiten der Zeile, als ob es nur 2-achsig w�re.
               Wenn Konik aktiv, dann VORHER pruefen, ob Konik fuer vorher-
               gehenden Element schon ausgegeben wurde ...
| AN(4)        Das obere Element zum vorhergehenden unteren errechnen / ausgeben.
               Wenn Displaymode "Shaded" aktiv, dann ..
               
               
Im Postprozessor sollte zu jedem Pfad unten und oben ausgegeben werden (also warten
  bis das Element fertig ist. 





-------------------------------------------------------------------------------
Ablauf bei der Bearbeitung:

APT_cut_line
  Erh�lt die APT-Eingabezeilen, die eine Bearbeitung implizieren  ; zB "L1".
  Wenn 1 Char "|": Kontur oben; sonst unten.
  Ruft alle folgenden.


APT_cut_zerl
  Die Geometrieelemente der APT-Eingabezeile decodieren (zB "RP P10"); fuer
  untere Kontur und obere Kontur.
  Die Elemente werden in APT_InputU / APT_InputO abgelegt.


APT_Break
  APT_InputU/O zerbrechen (wenn erforderl) und nach APT_KonturU/O speichern.
  Hinterher sind die Anzahl Elemente unten und oben gleich.


APT_bearb_Kontur
  Das n�chste Konturelement errechnen (verschneiden, projizieren ...)
  APT_KonturU > NC_ObjU.
  Wird auch von APT_bearb_O2 gerufen.

APT_bearb_O1
  Konik mit Winkeln, Radius oben:
  Das n�chste Konturelement errechnen

APT_bearb_O2
  Konik via 2 Konturebenen:
  Das n�chste Konturelement errechnen (ruft APT_bearb_Kontur)

APT_bearb_O3
  Konik via UV-Angaben.
  Das n�chste Konturelement errechnen


APT_transl_U/O
  NC_ObjU/O[] translieren ergibt NC_ObjUtr/Otr[]
  Das letzte der beiden > old_APT_ObjU/O und old_APT_ObjUtr/Otr kopieren.


APT_ausg_store_U/O
  Ausgabeelemente sammeln (zwischenspeichern, in NC_ausgU/O)


APT_ausg_cut
  display oder Postprozessieren ...
  Fuer die Darstellung ein eigenes Objekt bereitstellen und dann ausgeben.
  Eines fuer unten ( > NC_dispU)
  und eines kombiniert oben & unten ( > NC_ausgAU/O)


APT_ausg_zerl_ac
  in Polygon zerlegen. Eines der Inputobjekte ist ein AC.


APT_GR_cut_O
  Hier kommt nur mehr ein einzelnes Schneidelement (transliert) an
  (unterer und oberer Part), das grafisch auszugeben ist.
  Ruft APT_DrawRFac.







-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
Kurven:
Einlesen als S10=pol,pt1,..., ptn.

Abspeichern in tmp\crv010.bin.

Die DB_StoreCurv liest aus tmp\crv010.bin und generiert die dyn.Points.

Curves zerbrechen kann man erst NACH APT_bearb_Kontur (also lang nach
 Break), weil erst APT_bearb_Kontur die Normalen auf die Cv's errechnet usw.




Format struct Curv: siehe wincut_ut_crv.cpp.



-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
Kontur unten/oben:
  Eintragen der selektierten und mit Verbinden gefundenen Objekte in eine Liste.
  Die selektierten Objekte werden gruen gehilitet, die von Verbinden gefundenen rot.
  Mit dem CR ist ein Reset notwendig (den alten DispListpointer merken und
  zuruecksetzen).

Verbinden:
  Wird von jeden Select gerufen; entweder unten oder oben; (aber nur bei
  Selektion von Linie oder Kreis) - sucht nur nach lueckenloser Kontur
  (ein Kreis w�re ein Problem = sofortiger Stop)
  Die gefundenen Objekte werden gehilitet und in einer Liste zugefuegt.



*/



#ifdef _MSC_VER
#include "../xa/MS_Def0.h"
#endif

#include <stdio.h>                        /* printf ..   */
#include <ctype.h>                        // isdigit
#include <stdlib.h>                       /* atoi ..     */
#include <string.h>                       /* strcmp ..   */
#include <math.h>                         /* sin, cos .. */


#include "../gui/gui_types.h"              // UI_Func..
#include "../ut/ut_geo.h"                 /* Point-def ..*/
#include "../ut/ut_cast.h"             // INT_PTR
#include "../ut/ut_gtypes.h"              // APED_dbo_oid
#include "../ut/gr_types.h"               // SYM_* ATT_* LTYP_*
#include "../ut/ut_memTab.h"           // MemTab_..
#include "../ut/ut_txt.h"
#include "../ut/ut_TX.h"                  // TX_Print
// #include "../ut/ut_crv.h"
#include "../ut/ut_obj.h"
#include "../ut/ut_txfil.h"               // UTF_GetLinNr
#include "../ut/ut_os.h"                   // OS_Wait
#include "../ut/func_types.h"              // UI_AP u UI_FuncGet

#include "../db/ut_DB.h"

#include "../gr/ut_GL.h"                  // GL_GetCen
#include "../gr/ut_gr.h"                  // Typ_Att_..
#include "../gr/ut_DL.h"

//#include "ED.h"
#include "../xa/xa_ed.h"                    // ED_mode_go
#include "../xa/xa_edi__.h"
#include "../xa/xa_ga.h"                    // GA_hasTexture
#include "../xa/xa_mem.h"                 // memspc55
#include "../xa/xa_ed_mem.h"              // typedef_MemTab(ObjSRC)
// #include "../xa/opar.h"                   // MEMTAB_tmpSpc_get
#include "../xa/xa.h"                       // APP_act_proc AP_stat
#include "../xa/xa_sele.h"                  // Typ_go_*
#include "../xa/xa_ato.h"              // ATO_getSpc_tmp__
#include "../xa/xa_uid.h"             // UID_ckb_nam, UID_ckb_txt, UID_ouf_vwz

#include "../ci/NC_Main.h"
#include "../ci/NC_apt.h"




//===========================================================================
// EXTERNALS:
// from ../xa/xa.c:

// extern int     SRC_ato_anz, SRC_ato_SIZ, *SRC_ato_typ;
// extern double  *SRC_ato_tab;

extern int       AP_src;
// extern  int       aus_anz;
// extern  int       aus_typ[aus_SIZ];
// extern  double    aus_tab[aus_SIZ];

extern int       WC_modact_ind;         // -1=primary Model is active;
                                        // else subModel is being created


extern Point     WC_mod_ori;            // der Model-Origin
extern int       WC_sur_ind;            // Index auf die ActiveConstrPlane
extern Plane     WC_sur_act;            // die aktive Plane
extern Mat_4x3   WC_sur_mat;            // TrMat of ActiveConstrPlane
extern Mat_4x3   WC_sur_imat;           // inverse TrMat of ActiveConstrPlane
extern double    WC_sur_Z;              // der aktive Z-Wert der WC_sur_sur;
extern char      WC_ConstPl_Z[16];      // displayed name_of_Constr.Plane;

extern double     AP_txsiz;       // Notes-Defaultsize
extern double     AP_txdimsiz;    // Dimensions-Text-size
extern int        AP_txNkNr;            // Nachkommastellen
extern ColRGB     AP_defcol;
// extern int        AP_indCol;




// aus xa_edi__.c:
extern int       APT_mac_fil;   // 1=InputFromMemory; 0=InputFromFile.



// aus ../gr/ut_gtx.c:
extern double GR_tx_scale;


// aus ../gr/tess_su.c:
extern int TSU_mode;   // 0=normal darstellen; 1=speichern



// aus ../gr/ut_DL.c
// extern DL_Att  *GR_ObjTab;
extern long    DL_ind_act;


// ex ../xa/xa_proc.c
extern char **process_CmdTab;     // was NCCmdTab



//===========================================================================
typedef_MemTab(ObjTXTSRC);



//===========================================================================
// LOCALS:


//const int APT_LINE_siz = 1024*4;
// #define   APT_LINE_siz     4096 
// char      APT_LINE[APT_LINE_siz];          // die APT-Eingabezeilen


static long      NC_stat_SIZ=0;   // size von NC_stat__
static long      NC_stat_IND=0;   // zeigt auf den naechsten freien index

static NC_recTyp    *NC_stat__;
       long      AP_dli_act;      // index dispList

// static long APT_dbi;              // dbi of active obj
       int  AP_mdLev;             // active subModelLevel
static int  AP_mdLnr[12];         // APT-LineNrs of subModelCall
static int  AP_mdLne[12];         // APT-EndLineNrs of subModelCall

// int       WC_mode;                         // 0 = Edit, > 0 = PP#
int       APT_3d_mode = OFF;               // Inputmode 3D oder 2D

// int       APT_2d_to_3d_Ind  = 0;
// int       APT_2d_to_3d_mode = OFF;         // 2D -> 3D (PLANE ...)

static int APT_view_stat;         // 0=VIEW nicht gesetzt; 1=VIEW gesetzt.


Point   APT_tcpos;                // ToolChangePosition Werkzeugwechselposition
double  APT_Z_clear;
double  APT_Z_deep;
// int     APT_workmode;             // Typ_nc_cut oder Typ_nc_drill;
int     APT_obj_stat;             // 0=permanent, 1=temporary (workmode)

// int     APT_obj_typ;              // der Typ des zuletzt generierten Objekts.
int     APT_subTyp;               // subTyp of created obj; 

double  APT_pock_distk = 1.0;     // Abstand von der Kontur
double  APT_pock_distc = 1.0;     // Abstand Schnitte untereinander
double  APT_pock_dirc  = 0.;      // Richtung Schnitte


Memspc  APTSpcObj;      // memspc201 f. Ausgabeobjekt und seine Bestandteile
Memspc  APTSpcTmp;      // memspc501 f. temporaere Berechnungen


double  WC_sur1    =  0.0;        // Basisebene
double  WC_sur2_Def= 50.1;        // Default-sur2
double  WC_sur2    = 50.1;        // obere Schneidebene
                                  // kann nur der Wert von WC_sur1 od. WC_sur2 sein.
double  WC_thick   = 50.0;        // Abstand untere > obere Ebene

double  WC_sur3    =  0.0;        // Bohren: deep.



int     APT_konik = 0;    // Koniktyp; KONIK_0 od KONIK_1 ..
                          // KONIK_2 ($-Achsig) od. KONIK_3 (via U/V-Werte)
//int     APT_stat_konik=0; // ob die obere Kontur fuer das vorhergehende Elem.
                          // noch auszugeben ist; 0=nix tun, 1=Ausgabe offen



// ab 2001.11.22 in NC_wcut.c
// double  APT_UVal, APT_VVal;  // Fuer Koniktyp 3.
// int     APT_changeTo1 = OFF;  // Change von 3 > 1
// int     APT_changeTo2 = OFF;  // Change von 2 > 1
// int     APT_changeTo3 = OFF;  // Change von 1 > 3
// hier die zu bearb. Objekte einer APT_Gruppe; das ist fertig zum auszugeben.
// int     NC_Obj_anz;
// int     NC_Obj_anzU;   // wieviele Objekte in NC_ObjX; 1 oder 2.
// int     NC_Obj_anzO;
// ObjG2   NC_ObjU[4];
// ObjG2   NC_ObjO[4];
// ObjG2   NC_ObjUtr[4];
// ObjG2   NC_ObjOtr[4];
// int     APT_RadTyp;       // 0=konisch, 1=ISO, 2=userspezifisch.
// double  APT_angN = 0.0;   // Winkel normal zum Bearb.weg
// double  APT_angNold = 0.0;
// double  APT_angNact = 0.0;
// double  APT_angP = 0.0;   // Winkel parallel zum Bearb.weg
// double  APT_angPold = 0.0;

// double  APT_radO;         // Radius der oberen Kontur.
// double  APT_abstO = 0.0;  // Abstand oben (entspr. tan(APT_angN) * WC_thick)
//int     APT_cvInd_11, APT_cvInd_12, APT_cvInd_21, APT_cvInd_22;
// int     APT_cvIndU[4], APT_cvIndO[4];    // die Indexe fuer den 1. u. 2.
                                         // Zwischenpunkt der akt. Kurve
// ObjG2   act_ObjU, act_ObjO;
// Point2  newPosU,    newPosO;
// Point2  newPosUtr;
// ObjG2   ol1_APT_ObjU;
// ObjG2   ol1_APT_ObjO;

// hier werden die Obj. nach dem Zerbrechen zwischengespeichert.
// const int APT_Kontur_max = 100;
// int       APT_Kontur_anz, APT_Kontur_ind;
// ObjG2     APT_KonturU[APT_Kontur_max];
// ObjG2     APT_KonturO[APT_Kontur_max];

// ObjG2     NC_dispU[NC_ausg_max];
// int       NC_disp_anzU;

// hier werden die berabeiteten Objekte wenn erforderl polygonalisiert
//const int NC_poly_max = 500;
// int       NC_poly_anz;
// ObjG2     NC_polyU[NC_poly_max];
// ObjG2     NC_polyO[NC_poly_max];

// hier werden die Obj. aus der Eingabezeile abgelegt.
// const int APT_Input_max = 100;  nun als define!
// int       APT_InputU_anz;
// int       APT_InputO_anz;
// ObjG2     APT_InputU[APT_Input_max];
// ObjG2     APT_InputO[APT_Input_max];

// int     APT_stat_divide = OFF;    // Teilen in 2-Ebenenprogramm

//const int NC_ausg_max = 4;
// int       NC_ausg_anz, NC_ausg_anzU, NC_ausg_anzO;
// ObjG2     NC_ausgU[NC_ausg_max];
// ObjG2     NC_ausgO[NC_ausg_max];





int     APT_dispNam;                 // display ObjNames
int     APT_dispDir;                 // display ObjDirection; 1=yes, 0=not
// int     APT_dispNCCmd;               // display NC-Commands
int     APT_dispPT  = ON;            // display Points
int     APT_dispPL  = ON;            // display Planes
int     APT_dispSOL = ON;

int     APT_disp3D  = ON;

int     APT_TL_Side   = 0;                 // 0=OFF, 1=LFT, -1=RGT
int     NC_TL_act     = -1;


Point2  actPosO;
Point2  actPosOtr;
Vector2 actVecO;

Point2  actPosU;            // die untransformierte, aktuelle Pos als 2D-Punkt
Point2  actPosUtr; 

// hier werden die Objekte zuerst (vor dem Zerbrechen) zwischengespeichert.
int     APT_ObjU_anz, APT_ObjO_anz;
ObjG2   APT_ObjU[20];
ObjG2   APT_ObjO[20];

ObjG2   old_APT_ObjU;
ObjG2   old_APT_ObjUtr; // transformiert; hilighten als weiterfuehrendes Objekt.
ObjG2   old_APT_ObjOtr; // transformiert

ObjG2   old_APT_ObjO;



int     APT_tra;
int     APT_mirr;
int     APT_proj;

Mat_4x3 APT_mat_tra;
Mat_4x3 APT_mat_mirr;
Mat_4x3 APT_mat_proj;

Plane   APT_pln_tra;




int     NC_rapid;        // ON / OFF

int     NC_work_modeU;   // Kreise: ob die APT-Kreise sofort ausgegeben werden sollen
                         // Ja (ON) oder nicht (OFF);
                         // Linear: Rapid ON (G0) oder OFF (G1)

NC_up_rec *NC_up_act;


// Polygonalisieren generell; OFF oder ON (ALLE Arcs zerlegen) 
// int       APTsw_poly = OFF;

// Polygonalisieren von Arc/Line
// int       APTsw_poly_acln = OFF;



// int       NC_ausg = 0;
// ObjG2     NC_ausgAU;
// ObjG2     NC_ausgAO;




//Line2   old_APT_Obj_ln;     // untransformiert

static int     UpMirX,   UpMirY;
static Point2  UpOri,    UpFrom;
static double  UpOriAng, UpOriAng_sin, UpOriAng_cos;

static Point2  RefOri;
static double  RefOriAng, RefOriAng_sin, RefOriAng_cos;



#define PrgMod_normal            0
#define PrgMod_continue_if       1
#define PrgMod_skip_until_label  2
#define PrgMod_skip_until_macend 3
#define PrgMod_skip_until_mac    4
#define PrgMod_skip_until_line   5
#define PrgMod_skip_until_file   6
#define PrgMod_continue_file     7
#define PrgMod_continue_mac      8

static int     UP_mode = OFF;      // Schalter UP-mode; ON  = 0 sind im UP.
                                   //                   OFF = 1 sind im Main.
       int     UP_resolv = ON;     // ON=aufl�sen, OFF=schon mal erledigt.
       char    UP_act_typ;         // welcher UP-Typ aktiv ist;G=GEO,M=MAC. 
static char    UP_typ_DE;          // 0=UP ist DE (Konik-Prog); -1: not.

#define        UP_level_max 16
static char    UP_level_typ[16];   // hier merken, welcher Typ UP (Mac od
                                   // Geo) vorher aktiv war (UP_act_typ).
static char    UP_level_src[16];   // P=Prog, F=File,
static int     UP_level_adr[16];   // RuecksprungzeilenNr
       int     UP_level;
static int     UP_workmode;        // 1=CUT, 2=REV, 3=POCK
       // int     Mod_level;


static int     Prg_line_nr; // nach "END" wird auf diese LineNr zurueckgesetzt

int     APT_line_cnt = 0;   // Zeilenz�hler zur synchronisation von Insert-Lines

int     APT_line_act;
int     APT_line_old = 0;
int     APT_line_stat;



static char    APT_label[64];
static char    APT_macnam[64];
static char    APT_filnam[128];

int     APT_stat_act;             // 0 = normal; 2 = search for jump-Label;

int     APT_OPSTOP_stat = ON;     // ON: Stop; OFF: Skip OSP.

int     APT_tl_stat = 0;          // Soll, ToolNr.
int     APT_tl_act  = 0;          // Ist

int     APT_start = OFF;          // fuer das Startsymbol
int     APT_Stat_Draw = ON;       // genereller Draw-Schalter

int     APT_stat_FROM = OFF; // pro Main darf es nur ein FROM geben!




//----------------------------------------------------------
static int  l_irc        = 25;   // der Fehlercode; 25=OK!
// static int  l_cnt        = 16;

static int  fpOutAnz;            // Anz Ausgabezeilen; nur fuer Demolizenz



//----------------------------------------------------------
// Allgemeine Hilfsfelder:
// Point2    GR_ptArr1[500];
// Point2    GR_ptArr2[500];

int       GR_Att_act;
int       GR_lay_act = 0;

int       GR_pick = OFF;                   // NOPICK





/* ---------------------------------------------------------------------- */
static int PrgCmdAnz = 41;

/*
static char  *PrgCmdTab[] = {
  "PRI",         "DRAW",        "STAT",        "MODE",        "SSTYLS",
  "JUMP",        "HIDE",        "VIEW",        "CONST_PL",    " ",
  "IF",          "MODVAL",      "MODSIZ",      "DEFTX",       "DEFCOL",
  " ",           " ",           " ",           " ",           " ",
  "MAC",         "END",         "CALL",        " ",           " ",
  "GEO",         " ",           "INPUT",       "LDMOD",       "EXECM",
  "DIM",         "HILI",        "NOP",         " ",           " ",
  "LAY",         "SHOW",        "ATTL",        "ATTS",        "PROCESS",
  "ZOOMALL"};
*/

static char  *AppCodTab[] = {
/* 00   67889012  1234567889012  1234567889012  1234567889012  1234567889012 */
  "PRI",         "DRAW",        "STAT",        "MODE",        "SSTYLS",
  "SHOW",        "HIDE",        "VIEW",        "CONST_PL",    "ZOOMALL",
/* 10   67889012  1234567889012  1234567889012  1234567889012  1234567889012 */
  "MODBOX",      "MODVAL",      "MODSIZ",      "DEFTX",       "DEFCOL",
  "ATTL",        "ATTS",        "PROCESS",     "LDMOD",       "EXECM",
/* 20   67889012  1234567889012  1234567889012  1234567889012  1234567889012 */
  "DIM",         "HILI",        "LAY",         "DUMP",        "NOP",
  NULL};

enum Typ_TACT {
  TAC_PRI,       TAC_DRAW,      TAC_STAT,      TAC_MODE,      TAC_SSTYLS,   // 0-
  TAC_SHOW,      TAC_HIDE,      TAC_VIEW,      TAC_CONST_PL,  TAC_ZOOMALL,  // 5-
  TAC_MODBOX,    TAC_MODVAL,    TAC_MODSIZ,    TAC_DEFTX,     TAC_DEFCOL,   // 10-
  TAC_ATTL,      TAC_ATTS,      TAC_PROCESS,   TAC_LDMOD,     TAC_EXECM,    // 15-
  TAC_DIM,       TAC_HILI,      TAC_LAY,       TAC_DUMP,      TAC_NOP};     // 20-
                                                                            // 25-


static char  *PrgCodTab[] = {
/* 00   67889012  1234567889012  1234567889012  1234567889012  1234567889012 */
  "JUMP",        "MAC",         "GEO",         "END",         "IF",         // 0-
  "CALL",        NULL};                                                     // 5-

enum Typ_TPCT {
  TPC_JUMP,      TPC_MAC,       TPC_GEO,       TPC_END,       TPC_IF,       // 0-
  TPC_CALL};


static char      insBuf[10][64];

static int       insAnz, insCnt[10];
static int       insSym[10];        // Buffer fuer zum Command geh�riges Symbol
//int       preAnz,preBuf[10];


// static char      APT_txBuf[256];



double    APT_ModSiz    = 500.;       // 2003-09-13: Test 50. statt 500.

// double    APT_TOL_pt    = 0.0005; // identical Points
double    APT_TOL_ac    = 0.1;    // DISP_AC - Toleranz (Sehnenfehler bei der
                                  // Darstellung von Arcs.
// APT_TOL_ac unused !




// double    APT_infin  = 1000.0;   // UNUSED!

// double    APT_ln_len =  100.0;   // unbegrenzte Linien; wenn Wert h�her,
//                                 // wird recht ungenau gerechnet !


// double    APT_rad_max  = 9999.0;   // groessere Radien zerlegen;



char      *APT_defTxt;



int    APT_prim_typ;
int    APT_hide_parent;              // 0=not (def), else yes





//================================================================
  int AP_mdGet (int *lnr) {
//================================================================
/// query subModelLevel & APT-LineNr of active subModelCall
/// RetCod: AP_mdLev
///    -1     im primary Model
///    0-12   im subModel



  // if(AP_mdLev >= 0) *lnr = AP_mdLnr[AP_mdLev];
  if(AP_mdLev >= 0) *lnr = AP_mdLnr[0];

  // printf("AP_mdGet Lev=%d lnr=%d\n",AP_mdLev,*lnr);

  return AP_mdLev;

}


/*
//=====================================================================
  int WC_get_obj_typ () {
//=====================================================================
// der Typ des zuletzt generierten Objekts.
// Ist zB bei "U0=INT obj1 obj2" unbekannt !

  printf(" ex WC_get_obj_typ %d\n",APT_obj_typ);

  return APT_obj_typ;

}
*/


//=====================================================================
  int WC_get_obj_stat () {
//=====================================================================
/// 0=OK, 1=workmode (obj in statu nascendi; see APT_decode_ccv)

  return APT_obj_stat;

}


//=====================================================================
  int WC_set_obj_stat (int stat) {
//=====================================================================
/// 0=perm, 1=workmode (obj in status nascendi; see APT_decode_ccv)

  // printf(".................. WC_set_obj_stat %d\n",stat);

  APT_obj_stat = stat;

  return 0;

}


//================================================================
  void WC_Init_all (int mode) {
//================================================================
// mode = 0 (normal - interaktiv)
// mode = 1 (PP-Run)


  int i1;


  //---------------------------------------------------------
  // printf ("XXXXXXXX WC_Init_all %d\n",mode);

  APT_obj_stat = 0;

  WC_Init ();

  // Standard; Editmode.
  WC_Init_Tol ();
  GL_InitModelSize (APT_ModSiz, 0);

  // // init AP_box_pm1 AP_box_pm2
  // UT3D_pt_setFree (&AP_box_pm1);
  // UT3D_pt_setFree (&AP_box_pm2);

}


/*
//================================================================
  int WC_PP_open (int mode) {
//================================================================
// mode 1    open ../tmp/nc.iso
//      0    else close file


  printf("WC_PP_open %d\n",mode);


  if(mode == 1) {          // open


    // Das Zwischenausgabefile oeffnen
    // strcpy (WC_outBuf, OS_get_bas_dir ());
    // strcat (WC_outBuf, "/tmp/pp.iso");
    // sprintf(WC_outBuf,"%s%ctmp%cpp.iso",OS_get_bas_dir(),
            // fnam_del,fnam_del);
    sprintf(WC_outBuf,"%snc.iso",OS_get_tmp_dir());


    if ((APT_fpOut = fopen (WC_outBuf, "w+")) == NULL) {
      TX_Error ("open file %s ****",WC_outBuf);
      return -1;
    } else {
      TX_Print ("NC-Bearb. exportiert -> %s",WC_outBuf);
    }
    fpOutAnz = 0;




  } else {   // close

    if(APT_fpOut != NULL) {
      fprintf(APT_fpOut,"%s\n","FINI");
      fclose (APT_fpOut);
      APT_fpOut = NULL;
    }
    return 0;


  }

  sprintf (WC_outBuf , "$$ %s",OS_date1());
  APT_PP_Write();

  if(AP_mod_fnam) {
    printf("AP_mod_fnam |%s|\n",AP_mod_fnam);
    strcpy(WC_outBuf, "$$ ");
    strcpy(WC_outBuf, "$$ ");
    strcat(WC_outBuf, AP_mod_fnam);
    APT_PP_Write();
  }


  return 0;

}
*/


//======================================================================
  int WC_Init_Modsiz (double newssiz) {
//======================================================================
/// see WC_ask_ModSiz

  APT_ModSiz = newssiz;

  WC_Init_Tol ();

  return 0;

}


//======================================================================
  void WC_Init_Tol () {
//======================================================================
/// \code
/// compute all Tolerances from APT_ModSiz    (Defaultvalue = 500)
/// sets:                                                          Default:
/// UT_TOL_pt      identical Points-tolerence                      0.0001     
/// UT_TOL_ln      max deviation of a point from a line            0.001
/// UT_TOL_cv      max deviation of point from curve               0.005
/// UT_DISP_cv     max deviation of displayPolygon from curve      0.05
/// WC_sur2_Def, WC_sur2, APT_ln_len.
///
/// see ../ut/ut_tol_const.h (defaults)
/// \endcode

// TODO:  APT_TOL_ac ?

  double  d1;

  // printf("WC_Init_Tol %f\n",APT_ModSiz);


  if(APT_ModSiz < 1.) APT_ModSiz = 1.;
  if(APT_ModSiz > 1000000.) APT_ModSiz = 1000000.;




  d1  = APT_ModSiz / 5000000.;   // Def.: 0.0001
  UT_TOL_pt  = UTP_db_rnd1sig (d1);


  d1  = APT_ModSiz / 500000.;    // Def.: 0.001
  UT_TOL_ln  = UTP_db_rnd1sig (d1);


  d1  = APT_ModSiz / 100000.;    // Def.: 0.005
  UT_TOL_cv  = UTP_db_rnd1sig (d1);


  // d1 = APT_ModSiz / 50000.;     // Def.: 0.01
  d1 = APT_ModSiz / 10000.;     // Def.: 0.05
  UT_DISP_cv  = UTP_db_rnd1sig (d1);
  // UT_DISP_cv = 0.5;   // NUR TEST


  //WC_sur2    = 50.0;
  WC_sur2_Def  = APT_ModSiz / 10.0 + 0.1;
  WC_sur2      = WC_sur2_Def;


  // length of construction-lines
  d1 = APT_ModSiz * 2.;
  UT_DISP_ln = UTP_db_rnd1sig (d1);


  // TX_Print("APT_ModSiz=%f APT_TOL_pt=%f APT_min_dist=%f WC_sur2=%f",
            // APT_ModSiz,APT_TOL_pt,APT_min_dist,WC_sur2);


  // APT_infin  = APT_ModSiz * 5.0;

  // APT_ln_len =  APT_ModSiz / 2.;
  // APT_ln_len =  APT_ModSiz;


/*
  printf("ex WC_Init_Tol APT_ModSiz=%f\n",APT_ModSiz);
  // printf("........... APT_min_dist=%f\n",APT_min_dist);
  // printf("........... APT_TOL_pt=%f\n",APT_TOL_pt);
  printf("........... UT_TOL_pt=%f\n",UT_TOL_pt);
  printf("........... UT_TOL_ln=%f\n",UT_TOL_ln);
  printf("........... UT_TOL_cv=%f\n",UT_TOL_cv);
  printf("........... UT_DISP_cv=%f\n",UT_DISP_cv);
  printf("........... APT_ln_len=%f\n",APT_ln_len);
*/

}



//**********************************************************************
  void WC_Init () {
//**********************************************************************


  //---------------------------------------------------------
  // printf (">>>>>>>>>>>>>> WC_Init\n");


  DB_Init  (0);

  // Displist Init
  DL_Init ();

  APT_Init ();

  ED_Init  ();

}


//================================================================
  void APT_Init () {
//================================================================

  // int   i1;
  long  l1;



  //---------------------------------------------------------
  //TX_Print ("APT_Init %d",WC_mode);
  // printf (">>>>>>>>>>>>>> APT_Init %d\n",WC_mode);



  APT_line_act = 0;

  // WC_actPos_save (-1);


  GR_lay_act = 0;
  GR_Att_act = 0;
  GR_pick    = OFF;

  APT_Reset ();

  Grp_init ();         // clear group

  APT_line_cnt = 0;  // not used !!

  insAnz = 0;
/*
  for(i1=0;i1<10;++i1) {
    insCnt[i1] = -99;
  }
*/

  APT_3d_mode       = OFF;

  // APT_2d_to_3d_Ind  = 0;
  // APT_2d_to_3d_mode = OFF;

  APT_tra  = 0;
  APT_mirr = 0;
  APT_proj = 0;

  // APT_dispPT        = ON;

  // APTsw_poly      = OFF;
  // APTsw_poly_acln = OFF;

  UP_mode = OFF;
  UpMirX  = OFF;
  UpMirY  = OFF;

  UpOri.x  = 0.0;
  UpOri.y  = 0.0;
  UpOriAng    = 0.0;

  RefOri.x  = 0.0;
  RefOri.y  = 0.0;
  RefOriAng    = 0.0;


  //old_APT_Obj.typ  = Typ_PT2;
  old_APT_ObjU.typ  = Typ_Error;     
  old_APT_ObjO.typ  = Typ_Error;     

  old_APT_ObjO.typ           = Typ_Error;  // fuer den Start oben.

  actPosU.x = 0.0;
  actPosU.y = 0.0;
  //actPosU.z = 0.0;

  actPosO.x = 0.0;
  actPosO.y = 0.0;
  //actPosO.z = 0.0;

  actPosUtr.x = 0.0;
  actPosUtr.y = 0.0;

  actPosOtr.x = 0.0;
  actPosOtr.y = 0.0;

  APT_tl_stat   = 0;
  APT_tl_act    = 0;

  APT_start     = OFF;
  APT_Stat_Draw = ON; 

  // UT3D_pl_XYZ (&WC_sur_act);
  // WC_sur_Z    = WC_sur1;

  NC_TL_act     = -1;
  APT_TL_Side   = 0;
    
  old_APT_ObjUtr.typ = 0;            // kein weiterfuehrendes Obj.
  old_APT_ObjUtr.p1  = actPosUtr;    // fuer Sofortausgabe NCCmds


  APT_stat_FROM = OFF;       // pro Main darf es nur ein FROM geben!

  // die UP_Liste loeschen
  PP_up_list (NULL, NULL, -1);

}



//================================================================
  int APT_reset_view_stat () {
//================================================================
// 0=VIEW nicht gesetzt; 1=VIEW gesetzt.

  APT_view_stat = 0;
  return 0;

}


//================================================================
  int APT_get_view_stat () {
//================================================================
// 0=VIEW nicht gesetzt; 1=VIEW gesetzt.

  // printf("ex APT_get_view_stat %d\n",APT_view_stat);
 
  return APT_view_stat;

}


//================================================================
  int APT_set_view_stat () {
//================================================================
// 0=VIEW nicht gesetzt; 1=VIEW gesetzt.

  APT_view_stat = 1;
  return 0;

}



//================================================================
  void UCS_Reset () {
//================================================================
/// see AP_Init_planes
/// see GL_SetConstrPln

  // reset ucs
  WC_sur_ind = 0;
  UT3D_m3_copy (WC_sur_mat,  UT3D_MAT_4x3);
  UT3D_m3_copy (WC_sur_imat, UT3D_MAT_4x3);   // basic mat: imat = mat !

  UT3D_pl_XYZ (&WC_sur_act);

}

 
/**********************************************************************/
  void APT_Reset () {
/*==================================
*/


  //---------------------------------------------------------
  // printf("AAAAAAAAAAA APT_Reset AAAAAAAAAAAGGGGG\n");


  UP_level          = -1;
  APT_stat_act      =  PrgMod_normal;

  UCS_Reset();     // reset ucs

  ED_file__ (0, NULL);  // close all files ..

}




/**********************************************************************/
  void WC_EOF () {
/*===========
*/

  ObjG2 o1;

  // printf("WC_EOF stat=%d insAnz=%d\n",APT_stat_act,insAnz);



/*
  // unerledigte NCCmds raus
  if(insAnz > 0) {
    TX_Print("WC_EOF: unerledigte NCCmds auf %f,%f",actPosUtr.x,actPosUtr.y);
    // ++APT_line_cnt;
    // o1 = NC_ausgAU;
    // o1.p1 = actPosUtr;
    // o1.nam = APT_line_cnt;
    APT_ausg_NCCmd (0);
  }
*/



    if(APT_stat_act == PrgMod_skip_until_label) {
      TX_Error("Label %s nicht gefunden ****",APT_label);


    } else if(APT_stat_act == PrgMod_skip_until_macend) {
      TX_Error("Macroende END nicht gefunden ****");


    } else if(APT_stat_act == PrgMod_skip_until_line) {
      // wenn als letze Zeile ein call steht, gibts keine Folgezeile; also nix tun
      //TX_Error("nach Macro Zeile %d nicht gefunden ****",Prg_line_nr);
      //OS_beep (1000.,500.);


    } else if(APT_stat_act == PrgMod_skip_until_mac) {
      TX_Error("Macro %s nicht gefunden ****",APT_macnam);


    } else if(APT_stat_act == PrgMod_skip_until_file) {
      TX_Error("PrgMod_skip_until_file ****");

    }




/*
  if(l_irc != 25) {
    TX_Error (" %d",l_irc);
  }
*/

  // fuer das Redraw ist ON erforderlich !
  if(APT_Stat_Draw == OFF) {
    APT_Stat_Draw = ON;
  }


}





//====================================================================
  void WC_setDisp3D (int mode) {
//====================================================================
/// ON = 3D;  else 2D

  if(mode == ON) {
    TX_Print("3-D Modus eingeschalten");
  } else {
    TX_Print("2-D Modus eingeschalten");
  }

  APT_disp3D = mode;

}



//====================================================================
  void NC_set_actPos     (Point2 *posU) {
//====================================================================

  // printf("NC_set_actPos %f,%f\n",posU->x,posU->y);


  actPosU = *posU;

  actPosO = *posU;


  actPosUtr = APT_transl2 (&actPosU); // 2003-06-06
  actPosOtr = APT_transl2 (&actPosO); // 2003-06-06

}



//====================================================================
  int NC_getGrafAtt () {
//====================================================================

  return GR_Att_act;

}

//====================================================================
  void NC_setGrafAtt (int AttInd) {
//====================================================================
/// set GR_Att_act

  GR_Att_act =AttInd;

}


//================================================================
  int NC_setModSiz (double modSiz) {
//================================================================

  // printf("NC_setModSiz %f\n",modSiz);
 
  // activate new modSiz
  APT_ModSiz = modSiz;

  WC_Init_Tol ();                       // compute tolerances


  // display modelsize in label, redraw all
  if(WC_modact_ind < 0)                   // nur im aktiven Model
    GL_InitModelSize (APT_ModSiz, 0);

  return 0;

}


//====================================================================
  int NC_setRefsys (int RefInd) {
//====================================================================
// Change active Plane (Refsys)
// RefInd 0 = Ruecksetzen !

// doku ConstrPlane: see ../xa/xa.c


  Plane    pln1;


  // printf("RRRRRRRRRRRRRRRRRRR  NC_setRefsys %d\n",RefInd);


  if(RefInd == 0) {
    strcpy(WC_ConstPl_Z, "RZ");
    UT3D_pl_XYZ (&pln1);

  } else {
    APED_oid_dbo__ (WC_ConstPl_Z, Typ_PLN, RefInd);
    DB_GetRef (&pln1, RefInd);      // get a copy of plane
  }


  // check for temporaerModus
  if(APT_obj_stat != 0) {
    printf(" preview only ..\n");
    GR_Disp_axis (&pln1, 9, 5);
    return 1;
  }




  // Den Index auf die ActiveConstrPlane speichern
  WC_sur_act = pln1;
  WC_sur_ind = RefInd;
  if(WC_sur_ind != 0) {
    UT3D_m3_loadpl (WC_sur_mat, &WC_sur_act);
    UT3D_m3_invm3 (WC_sur_imat, WC_sur_mat);
  } else {
    UT3D_m3_copy (WC_sur_mat, UT3D_MAT_4x3);
    UT3D_m3_copy (WC_sur_imat, UT3D_MAT_4x3);
  }

  GL_SetConstrPln ();  // GL_constr_pln setzen (hier ist Z-Wert aufgerechnet !)

  UI_Set_ConstPl_Z (); // write Label name_of_the_Constr.Plane out
                       // als "ConstrPln   R20"  (links oben)


/*  Alte Version bis 2006-02-26
      //TX_Print(" change pln > %d",RefInd);
      // _2d_to_3d_mode kann nur im 2D-Mode aktiviert werden !
      if(APT_3d_mode == ON) {
        TX_Error("Referenzsystem kann nur im 2D-Mode aktiviert werden");
        return;
      }


      // Index 0 ist das Hauptachsensystem (= Zuruecksetzen)
      APT_2d_to_3d_Ind  = RefInd;

      if(APT_2d_to_3d_Ind == 0) {
        APT_2d_to_3d_mode = OFF;

      } else {
        APT_2d_to_3d_mode = ON;
        // Plane und Matrix beladen
        DB_GetRef (&APT_2d_to_3d_Pln, APT_2d_to_3d_Ind);
        UT3D_m3_loadpl (APT_2d_to_3d_Mat, &APT_2d_to_3d_Pln);
      }
*/


/*
        TX_Print(" matVX=%f,%f,%f",APT_2d_to_3d_Mat[0][0],APT_2d_to_3d_Mat[1][0],APT_2d_to_3d_Mat[2][0]);
        TX_Print(" matVY=%f,%f,%f",APT_2d_to_3d_Mat[0][1],APT_2d_to_3d_Mat[1][1],APT_2d_to_3d_Mat[2][1]);
        TX_Print(" matVZ=%f,%f,%f",APT_2d_to_3d_Mat[0][2],APT_2d_to_3d_Mat[1][2],APT_2d_to_3d_Mat[2][2]);
        TX_Print(" matPO=%f,%f,%f",APT_2d_to_3d_Mat[0][3],APT_2d_to_3d_Mat[1][3],APT_2d_to_3d_Mat[2][3]);
*/

  // printf("ex NC_setRefsys WC_sur_ind=%d\n",WC_sur_ind);

  return 0;

}


//================================================================
  int WC_actPos_disp () {
//================================================================

  Point     pt1;


  // skip it in GL-Feedback-mode !
  // this function impacts Rescale ! (GL_Rescal0)
  if(NC_TL_act < 0) return 0;

  // alle alten Tempobjekte l�schen
  GL_temp_del_all ();


  pt1 = UT3D_pt_pt2(&actPosU);

  // printf("  PosKreuz=%f,%f\n",pt1.x,pt1.y);
  GR_tmpSym (SYM_CROSS, &pt1);                  // 0 = das Positionskreuz

  DL_Redraw ();

  return 0;

}


/*
//=======================================================================
  int APT_dlInd_last (int lNr) {
//=======================================================================
// den zuletzt gespeicherten DL-Index suchen.

  int  i1, i2;


  i2 = -1;

  for(i1=lNr; i1>0; --i1) {
    if(obj_ind_tab[i1] < 0) continue;
    i2=obj_ind_tab[i1];
    break;
  }

  // printf("ex APT_dlInd_last dli=%d, i1=%d lNr=%d\n",i2,i1,lNr);

  return i2;

}


//=======================================================================
  int APT_dlInd_get (int lNr) {
//=======================================================================
// Kann eine Line nur 1 Obj erzeugen ?  Nein; M20 kann 50 Obj haben !
// das End of Text darf nicht abgefragt werden ! Liefert dann Mist !


  int  i1, i2, dli;


  // test for EOText;
  // for(i1=lNr; i1<APT_max_anz; ++i1) {
  for(i1=lNr; i1<=obj_ind_Nr; ++i1) {
    if(obj_ind_tab[i1] >= 0) goto L_work;
  }
  // keine sinnvolle objID mehr; must be EOText.
  // printf("ex APT_dlInd_get -1 lNr=%d\n",lNr);
  return -1;





  // den naechsten DL-Ind suchen. Wegen Leerzeilen gibts Luecken in der Tab.
  L_work:
  i2 = -1;

  for(i1=lNr; i1<=obj_ind_Nr; ++i1) {
    if(obj_ind_tab[i1] < 0) continue;
    // printf(" i1=%d, obj_ind_tab[i1]=%ld\n",i1,obj_ind_tab[i1]);
    i2=i1;
    break;
  }




  // printf("ex APT_dlInd_get l=%d dli=%d lNr=%d\n",i2,obj_ind_tab[i2],lNr);

  return obj_ind_tab[i2];

}

//=======================================================================
  int APT_disp_reset (int lNr) {
//=======================================================================
// alle Ausgabeobjekte der Zeilen ab lNr loeschen
//  (auch Output von lNr loeschen!)
// sonst nix


  long    i1, dli;


  // printf("''''''''''''APT_disp_reset %d\n",lNr);


  if(lNr >= actPos_SIZ) lNr = actPos_SIZ-1;
  if(lNr < 1) {
    lNr = 0;
    goto L_clear;
    // return 0;
  }


  // den letzen DL-Ind suchen. Wegen Leerzeilen gibts Luecken in der Tab.
  dli = APT_dlInd_get (lNr);
  if(dli < 0) return 0;


  // und die DispList zuruecksetzen
  GL_Delete (dli);


  // und obj_ind_tab komplett ab lNr loeschen.
  L_clear:
  for(i1=lNr; i1<=obj_ind_Nr; ++i1) {
    obj_ind_tab[i1] = -1;
  }
  obj_ind_Nr = lNr;

  return 0;

}

//=======================================================================
  int APT_disp_hili (int lNr) {
//=======================================================================
// hilite Obj of line lNr
// sonst nix


  long   dli;


  // printf("````````````APT_disp_hili %d\n",lNr);
  // DL_DumpObj__ (lNr);


  if(lNr >= actPos_SIZ) lNr = actPos_SIZ-1;
  if(lNr < 1) return 0;


  // loeschen der Hili-Tabelle
  DL_hili_on ((long)-1);


  // den letzen DL-Ind suchen. Wegen Leerzeilen gibts Luecken in der Tab.
  dli = APT_dlInd_get (lNr);
  if(dli < 0) goto L_fertig;


  // das zuletzt bearb. Elem. hiliten
  DL_hili_on (dli);


  L_fertig:
  DL_Redraw();

  return 0;

}
*/

/*
//================================================================
  int WC_actPos_save (int mode) {
//================================================================
// save actPosU --> NC_stat__[APT_line_act]
// und save actPosU als P0
// APT_alloc1

  long   i1;
  Point  pt1;


  // printf("WC_actPos_save %d %d %d %f,%f\n",mode,APT_line_act,NC_stat_IND,
          // actPosU.x,actPosU.y);


  if(mode < 0) {    // Init
    NC_stat_IND = 0;
    if(NC_stat_SIZ < 100) {
      i1 = APT_alloc1(100);
      if(i1 < 0) return -2;
    // } else {
      // printf("WC_actPos_save -- reInit\n");
      // for(i1=0; i1<NC_stat_SIZ; ++i1) actPosU_tab[i1].x = UT_DB_LEER;
    }
    return 0;
  }


  if(APT_line_act < 1) {
    printf("***** WC_actPos_save: APT_line_act=%d !?\n",APT_line_act);
    return 0;
  }


  // in einem MAC keine records speichern ...
  if(UP_level >= 0) {
    // printf(" _save skip lNr %d\n",APT_line_act);
    return 0;
  }


  // wegloeschen ..
  for(i1=NC_stat_IND-1; i1>=0; --i1) {
    if(NC_stat__[i1].lNr < APT_line_act) {
      NC_stat_IND = i1 + 1;
      break;
    }
  }




  // check space
  if(NC_stat_IND >= NC_stat_SIZ) {
    // if(APT_line_act > 1000000) {
      // printf("*********** WC_actPos_save E001\n");
      // return -1;
    // }
    i1 = APT_alloc1(NC_stat_IND+10);
    if(i1 < 0) return -2;
  }


  // die actPosU merken
  // actPosU_tab[APT_line_act] = actPosU;
  NC_stat__[NC_stat_IND].lNr    = APT_line_act;
  NC_stat__[NC_stat_IND].actPos = actPosU;
  NC_stat__[NC_stat_IND].rpd    = NC_rapid;
  NC_stat__[NC_stat_IND].tlNr   = APT_tl_stat;  // NC_TL_act
  ++NC_stat_IND;

  APT_line_old = APT_line_act;


  // die ActPos an den P0 uebergeben
  pt1 = UT3D_pt_pt2(&actPosU);
  DB_StorePoint (0L, &pt1);


  return 0;

}


//================================================================
  int WC_actPos_dump () {
//================================================================

  long   i1;


  printf("====== WC_actPos_dump %ld =============\n",NC_stat_IND);

  for(i1=0; i1<NC_stat_IND; ++i1) {
    // if(actPosU_tab[i1].x == UT_DB_LEER) continue;
    printf(" NC_stat__[%ld] lNr=%ld pos=%f,%f TL=%d Rpd=%d\n",i1,
           NC_stat__[i1].lNr,NC_stat__[i1].actPos.x,NC_stat__[i1].actPos.y,
           NC_stat__[i1].tlNr,NC_stat__[i1].rpd);
  }

  // display also die UP-Liste
  PP_up_list (NULL, NULL, -2);

  return 0;

}


//================================================================
  int WC_actPos_reset (int lNr) {
//================================================================
// reset actPosU und P0 fuer reset into Line lNr.
// Von hinten weg die erste gespeicherte Position suchen,
// deren (LineNr < lNr) ist; diese restoren; alle folgenden loeschen.


  long    i1;
  Point   pt1;


  // printf("WC_actPos_reset %d\n",lNr);
  // WC_actPos_dump ();


  // if(lNr > NC_stat_IND) {
    // printf("***** WC_actPos_reset E001 ******");
    // return -1;
  // }
  if(NC_stat_IND < 1) {

    actPosU   = UT2D_PT_NUL;
    actPosO   = actPosU;
    actPosOtr = actPosU;
    actPosOtr = actPosU;

    APT_tl_stat = 0;
    NC_rapid  = 0;

    goto L_L1;
  }


  for(i1=NC_stat_IND-1; i1>=0; --i1) {
    if(NC_stat__[i1].lNr >= lNr) continue;

    // // wenn es eine NC-Line ist: WC_actPos_disp
    // if(NC_stat__[i1].lNr == lNr) {
      // actPosU = NC_stat__[i1].actPos;
      // WC_actPos_disp ();
      // if(i1 > 0) --i1;
    // }
    break;
  }

  // das folgende loeschen ..
  NC_stat_IND = i1+1;

  // printf(" -reset actPos %d\n",i1);

    actPosU   = NC_stat__[i1].actPos;
    actPosO   = actPosU;
    actPosOtr = actPosU;
    actPosOtr = actPosU;

    APT_tl_stat = NC_stat__[i1].tlNr;
    NC_rapid  = NC_stat__[i1].rpd;

  WC_actPos_disp ();

  // die ActPos an den P0 uebergeben
  pt1 = UT3D_pt_pt2(&actPosU);
  DB_StorePoint (0L, &pt1);


  // das PosKreuz resetten ...
  // geht so nicht; actPosU ist nun die oldPos; das PosKreuz sollte auf die
  // nachfolgende Zeile (wenn es eine gibt!)
  // WC_setPosKreuz ();




  //----------------------------------------------------------------
  L_L1:
  // noch alle NC-ups wegloeschen, deren (LineNr >= lNr) ist.
  PP_up_list (NULL, NULL, lNr);



    // printf("ex WC_actPos_reset %d %f,%f\n",NC_stat_IND,actPosU.x,actPosU.y);
    // WC_actPos_dump ();

  return 0;

}
*/

/*
//=======================================================================
  int WC_actPos_reset (int lNr) {
//=======================================================================
// alle Ausgabeobjekte der Zeilen ab (lNr+1) loeschen
// das Ausgabeobjekte der Zeile lNr hiliten


  long   dli;
  int    i1, i2;


  printf("''''''''''''WC_actPos_reset %d\n",lNr);


  if(lNr >= APT_max_anz) lNr = APT_max_anz-1;
  if(lNr < 1) return 0;


  // APT_disp_reset (lNr);

  // den letzen DL-Ind suchen. Wegen Leerzeilen gibts Luecken in der Tab.
  dli = APT_dlInd_get (lNr);
  if(dli < 0) return 0;


  // und die DispList zuruecksetzen
  GL_Delete (dli+1);


  // loeschen der Hili-Tabelle
  DL_hili_on ((long)-1);

  // das zuletzt bearb. Elem. hiliten
  DL_hili_on (dli);

  DL_Redraw();



  // den aktuellen Punkt wiederherstellen
  actPosU = actPosU_tab[lNr];
  actPosUtr = APT_transl2 (&actPosU);


  // printf("ex WC_actPos_reset %d\n",lNr);
  return 0;

}
*/



//=======================================================================
  int WC_Work__ (int lNr, char* cbuf) {
//=======================================================================
/// \code
/// work sourceObj.
/// RetCod:
///   -2     do nothing
///   -3     obj not yet complete
/// \endcode

// testen, ob ein Ditto gerufen wird; wenn ja zuerst laden.
// seqNr ist Reihenfolge der BasicModels; fuer IGES-Export ..


// static char oldMod[128];

  int      irc, i1, mbNr, mbTyp;
  long     ll;
  char     mNam[128], *p1;
  ModelBas *mb;



  // printf("WWWWWWWWWWWWW WC_Work__ %d |%s| %d\n",lNr,cbuf,APT_stat_act);
  // DB_dump_ModRef ();
  // DB_test__();


  // Init
  if(cbuf == NULL) {
    // oldMod[0] = '\0';
    AP_mdLev = -1;
    APT_Init ();
    return 0;
  }




  // check for Submodelaufruf
  L_start:
  if(cbuf[0] != 'M') goto L_fertig;
  if (!isdigit(cbuf[1])) goto L_fertig;


  // im TSU-mode-store nix tun !
  if(TSU_get_mode() != 0) goto L_fertig;



  //-----------------------------------------------------------
  // Submodelaufruf; is it MockupModel ?
  // printf(" _Work WC_modact_ind=%d\n",WC_modact_ind);
  // printf(">>>>>>>>Ditto: AP_mdLev=%d |%s|\n",AP_mdLev,cbuf);

  p1 = strchr(cbuf, '=');
  UTX_skip_1bl (&p1);

  // Mirror Model: already loaded ..
  if(!strncmp(p1, "MIR", 3)) goto L_fertig;


  // Test for catalog-part
  if(!strncmp(p1, "CTLG",4)) {  // yes, its a catalog-part
    p1 += 4;
    // create tmp/<catalog>_<part>.write (with parameters different to basModel)
    irc = CTLG_Part_Ref1 (mNam, p1);  // -2 = catalogModel
    if(irc < 0) return -1;
    mbTyp = -2;

  } else {
    mbTyp = Mod_get_typ1 (mNam, cbuf);  // get typ & Modelname
    // mbTyp: -1:MBTYP_INTERN  -3:Error >=0:extern
    if(mbTyp < -2) goto L_fertig;       // kein gueltiger Modelname ..
  }
    // printf(" _Work__ mNam |%s| mbTyp=%d\n",mNam,mbTyp);


  // MockupModel's und Images werden direkt in WC_Work1 ausgegeben.;
  // internalModels und externalModels muessen hier als hidden Objekt
  // vorgeladen werden; in WC_Work1 wird Ditto ausgegeben.
  if(mbTyp > 9) goto L_fertig;      // Mockup-Model -->

/*
2011-07-06 raus; Error wenn symbol erst nachtr�glich in dir.lst eingef�gt wird.
  // derzeit keine subModels in external-native Models !!
    printf("     WC_modact_ind=%d\n",WC_modact_ind);
  if(WC_modact_ind >= 0) {            // wenn nicht im aktiven Model
    if(mbTyp >= 0) {                // und wenn external Model -
      TX_Error("external Model may not have subModels; use Mockup");
      return -1;
    }
  }
*/

    // TESTBLOCK
    // DB_dump_ModRef (); // dump refModels
    // END TESTBLOCK



  //-----------------------------------------------------------
  // ist Model schon geladen ? Else create new basicModel-(mdb_dyn)-Record.
  // get ModelNr from Modelname
  mbNr = DB_StoreModBas (mbTyp, mNam);
    // printf(" _Work__ mbTyp=%d mbNr=%d\n",mbTyp,mbNr);


  // get the mdb_dyn-Record.
  mb = DB_get_ModBas (mbNr);
  if(mb == NULL) goto L_err1;
    // UT3D_stru_dump (Typ_SubModel, mb, "  mb:");

  if(mb->DLsiz >= 0) goto L_fertig;   // ja, Model bereits geladen



	//==========================================================
  // model not yet loaded:
  // Submodel ist noch nicht geladen!!!!
  L_mod_load:
    // printf(" _Work__ L_mod_load: |%s| typ=%d\n",mb->mnam,mb->typ);

  // increment Level (spaeter Rekursion !!!
  ++AP_mdLev;
  if(AP_mdLev >= 12) {
    TX_Error("nesting-level of subModel > 12; WC_Work__-E001");
    return -1;
  }
  AP_mdLnr[AP_mdLev] = ED_work_CurSet (-1);   // save ED_lnr_act
    // koennt man hier lNr (Inputparameter) nehmen ?
  AP_mdLne[AP_mdLev] = ED_lnr_bis__ (-1);     // save ED_lnr_bis
    // printf(" new AP_mdLev=%d r=%d e=%d\n",AP_mdLev,
           // AP_mdLnr[AP_mdLev],AP_mdLne[AP_mdLev]);



/*
  //-----------------------------------------------------------
  // check for Kreuzverbindung
  for(i1=0; i1<=actLev; ++i1) {
    if(mbNrTab[i1] == modNr) {
      mdb1 = DB_get_ModBas (mbNrTab[0]);
      mdb2 = DB_get_ModBas (mbNrTab[actLev]);
      printf("Kreuzverb %s %s\n",mdb1->mnam,mdb2->mnam);
      TX_Error("Kreuzverbindung der Modelle %s %s",mdb1->mnam,mdb2->mnam);
      // return -1;
    }
  }
*/


    // save Hidelist --> File
    // GA_hide_fil_tmp (1);

    // save gesamte DB -> File
    DB_save__ ("");

    // save the DYNAMIC_DATA of the actual mainModel
    // DL_sav_dynDat ();


  //----------------------------------------------------------------
  // eine Liste aller noch nicht geladenen Submodels generieren;
  // alle Submodels rekursiv durcharbeiten - ergibt eine Reihenfolge;
  // beim Laden muss Reihenfolge verkehrt sein !
    // printf(" SSSSSSSSSSSSSSSSSSSSS start ResMod |%s|\n",mNam);

  // scan rekursiv die SourceFiles aller basicModels;
  // load Submodels as basicModels (mdb_dyn-Record's - DB_StoreModBas)
  irc = Mod_get_namAll ();
  if(irc < 0) {
    // TX_Error("***** ERROR: subModel %s not loaded ..",mb->mnam);
    TX_Print("***** ERROR: subModel %s not loaded ..",mb->mnam);
    // printf("subModel %s not loaded ..\n",mb->mnam);
    // return -1;
  }

  // Load Models of mdb_dyn-Records; Load Models; Reihenfolge=seqNr.
  // Mod_load_allsm>Mod_load_sm>ED_Run>WC_Work__ == REKURSION !!!
  irc = Mod_load_allsm ();
  if(irc < 0) {
    // TX_Error("***** ERROR: subModel %s not loaded ..",mb->mnam);
    TX_Print("***** ERROR: subModel %s not loaded ..",mb->mnam);
    // printf("subModel %s not loaded ..\n",mb->mnam);
    // return -1;
  }


  // back from rekursion ..
  ED_lnr_bis__ (AP_mdLne[AP_mdLev]);     // reset ED_lnr_bis
  ED_set_lnr_act (AP_mdLnr[AP_mdLev]);   // reset ED_lnr_act
  --AP_mdLev;
    // printf(" YYYYYYYYYYYYYYYYYYYYY after ResMod AP_mdLev=%d ED_lnr_bis>%d\n",
           // AP_mdLev,AP_mdLne[AP_mdLev+1]);
    // DB_dump_ModRef (); // dump refModels



  //----------------------------------------------------------------
  // reload gesamte DB from File, also AP_box_pm1,2
  DB_load__ ("");

  // reload the DYNAMIC_DATA of the actual mainModel
  // DL_load_dynDat ();

  // reload Hidelist from File
  // GA_hide_fil_tmp (2);

  // reset defCol
  // AP_indCol = GL_DefColSet (&AP_defcol);
  


  //----------------------------------------------------------------
  // Testen, ob gesuchtes SubModel nun geladen.
  if(mb->DLsiz <= 0) { //goto L_err1;   // problem if sm exists but is empty ..
    TX_Print("subModel %s empty ? ..",mb->mnam);
  }

  // UT3D_stru_dump (Typ_SubModel, mb, "mb nach Mod_load_allsm:\n");
  // if(mb->DLsiz < 0) exit(0);      

  // bei Aufruf von "M0=.." gibts keine zugehoerige Line;
    // printf(" _work__ |%s|\n",cbuf);
  if(cbuf[1] != '0') {
    // bei der alten Zeilennummer ED_lnr_act wieder weitertun ..
    // die zeile noch mal einlesen
    ED_set_lnr_act (lNr);
    UTF_GetLinNr (cbuf, &ll, lNr);
  }



	//==========================================================
  // weiter im aktuellen Model ...
  L_fertig:

  // reset Error.
  if(AP_stat.errStat) {
    // error is active
    if(APT_obj_stat == 0) {
      // mode=permanent ..
      if(lNr <= AP_stat.errLn) AP_errStat_reset (1);
    }
  }


  // work
  return WC_Work1 (lNr, cbuf);


  //----------------------------------------------------------------
  L_err1:
  TX_Error("WC_Work__: DB_get_ModBas E001");
  return -1;



  L_err2:
  // TX_Error("WC_Work__: Submodel %s does not exist",mNam);
  MSG_err_1 ("NOEX_dir", "%s", mNam);
  return -1;


}



/*
//=======================================================================
  int WC_Work__ (int lNr, char* cbuf) {
//=======================================================================
// testen, ob ein Ditto gerufen wird; wenn ja zuerst laden.
// seqNr ist Reihenfolge der BasicModels; fuer IGES-Export ..
// retCod = 2: nix tun

// Nachteil dieser Version: nach jedem UP-Call (ohne das Submodel fertig
//   abzuarbeiten) immer retour ins rufende; dann im rfenden weiter,
//   wieder ins Submodel usw ...


#define maxLev  10

static int      actLnr, actLev=-1, mbNrTab[10], seqNr;
static ModelRef ditto1;
static long     actDLi;



  int      i1, modNr, modTyp;
  long     l1;
  char     *p1,  *p2, txbuf[256];
  ModelBas *mdb1, *mdb2;



  // Init Levelcounter
  // if(lNr < 0) {
  if(cbuf == NULL) {
    printf(">>>>>>>>resolv actLev reset\n");
    actLev = -1;

    seqNr = 0;

    return 0;
  }




  //-----------------------------------------------------------
  // nur bei Submodelaufrufen was tun ...
  L_start:
  if(cbuf[0] != 'M') goto L_fertig;
  if (!isdigit(cbuf[1])) goto L_fertig;




  //-----------------------------------------------------------
  // MockupModel ?
  printf(">>>>>>>>Ditto: |%s|\n",cbuf);
  modTyp = Mod_get_typ1 (txbuf, cbuf);  // get typ & Modelname

  // MockupModel's und Images werden direkt in WC_Work1 ausgegeben.;
  // internalModels und externalModels muessen hier als hidden Objekt
  // vorgeladen werden; in WC_Work1 wird Ditto ausgegeben.
  if(modTyp > 2) goto L_fertig;      // Mockup-Model -->





  //-----------------------------------------------------------
  // ist Model schon geladen ? Else create new mdb_dyn-Record.
  // get ModelNr from Modelname
  modNr = DB_StoreModBas (modTyp, txbuf);
  ditto1.modNr = modNr;


  // get the mdb_dyn-Record.
  mdb1 = DB_get_ModBas (modNr);
  if(mdb1 == NULL) goto L_err1;
  if(mdb1->DLsiz >= 0) goto L_fertig;   // ja, Model bereits geladen





  //-----------------------------------------------------------
  // check for Kreuzverbindung
  for(i1=0; i1<=actLev; ++i1) {
    if(mbNrTab[i1] == modNr) {
      mdb1 = DB_get_ModBas (mbNrTab[0]);
      mdb2 = DB_get_ModBas (mbNrTab[actLev]);
      printf("Kreuzverb %s %s\n",mdb1->mnam,mdb2->mnam);
      TX_Error("Kreuzverbindung der Modelle %s %s",mdb1->mnam,mdb2->mnam);
      // return -1;
    }
  }




  //-----------------------------------------------------------
  // godown one level ..
  ++actLev;
  if(actLev >= 10) {
    actLev = -1;
    TX_Error("WC_Work__ E005-Level > 10");
    return -1;
  }


  mbNrTab[actLev] = modNr;

  WC_modact_ind        = modNr;



  printf(">>>>>>>>resolv |%s| actLev=%d\n",txbuf,actLev);
  printf("  get BasModel %d |%s|\n",modNr,mdb1->mnam);


  if(actLev == 0) {

    // merken; bei dieser Zeilennummer ED_lnr_act wieder weitertun ..
    actLnr = lNr;

    // DL-Index merken (ab hier nach erfolglosem Versuch loeschen)
    actDLi = GL_Get_DLind();
    
    // save gesamte DB -> File
    DB_save__ ();

    // save the DYNAMIC_DATA of the actual mainModel
    DL_sav_dynDat ();



  } else {

    // DL nach erfolglosem Versuch loschen
    GL_Delete (actDLi);
  }




  // den Pfad holen
  // Mod_get_path (txbuf, ModBasTab[iact].mnam);
  Mod_get_path (txbuf, mdb1->mnam);
  printf(" ModelFnam=|%s|\n",txbuf);


  // DL-StartIndex speichern
  // ModBasTab[iact].DLind = GL_GetActInd();
  mdb1->DLind = GL_Get_DLind();
  // vorab mal sichern (sonst bei kreuzweiser Verbindung problem !)
  // mod1->DLsiz = -modNr[ILEV]-1;   // -1 weil -0 nicht brauchbar !
  mdb1->DLsiz = -1;
  // printf("  mod1->DLind=%d siz=%d\n",mdb1->DLind,mdb1->DLsiz);



  // RUN (abarbeiten)
  DB_Init (1);
  ED_Init ();
  if(ED_work_file (txbuf) < 0) goto L_err2;
  ED_lnr_reset ();
  ED_Run ();





  //-----------------------------------------------------------
  // nach Run:
  // kommt hier mehrmal unnutz zurueck ?
  // printf("...............nach ED_Run actLev=%d\n",actLev);
  if(actLev < 0) return -2;


  // DL_disp_def (ON); // ab nun wieder alles anzeigen


  // DL-size speichern
  // ModBasTab[iact].DLsiz = GL_GetActInd() - ModBasTab[iact].DLind;
  mdb1->DLsiz = GL_Get_DLind() - mdb1->DLind;

  printf(">>>> modNr=%d mnam=|%s| DLind=%d DLsiz=%d\n",modNr,
                 mdb1->mnam,mdb1->DLind,mdb1->DLsiz);


  // Datei mit der richten Reihenfolge der Models
  printf(" seqNr=%d |%s|\n",seqNr,mdb1->mnam);
  mdb1->seqNr = seqNr;
  ++seqNr;


  // Model-Origin speichern;
  // mod1->po = WC_mod_ori;
  mdb1->po = UT3D_PT_NUL;


  // make it unvisible
  printf(" make invis. %d - %d\n",mdb1->DLind,mdb1->DLind+mdb1->DLsiz-1);
  for(l1=0; l1<mdb1->DLsiz; ++l1) {
    DL_unvis_set (mdb1->DLind+l1, 1);
  }


  // reload gesamte DB from File
  DB_load__ ();

  // reload the DYNAMIC_DATA of the actual mainModel
  DL_load_dynDat ();


  WC_modact_ind        = -1;  // main


  // bei der alten Zeilennummer ED_lnr_act wieder weitertun ..
  // die zeile noch mal einlesen
  // actLev = 0;      // von vorn beginnen !
  // L_reset:
  lNr = actLnr;
  ED_set_lnr_act (lNr);
  UTF_GetLinNr (cbuf, &i1, lNr);
  actLev = -1;

  printf(">>>>>>>>continue: |%s|\n",cbuf);
  goto L_start;




  //==========================================================
  L_fertig:

  WC_Work1 (lNr, cbuf);

  return 0;




  L_err1:
  TX_Error("WC_Work__: DB_get_ModBas E001");
  return -1;



  L_err2:
  TX_Error("WC_Work__: Submodel %s does not exist",txbuf);
  return -1;


}
*/


//================================================================
  int WC_Work_upd (int lNr, char* cbuf) {
//================================================================
// cbuf is normal codeline (terminated with \n, including objName).
// terminate cbuf with '\0', but restore on exit.

  int   irc;
  char  *p1;

static int errStat=0; // 0=OK


    // find p1 = PosCodEnd
    p1 = strchr(cbuf, '\n');
    if(p1) *p1 = '\0';


    // reset Error
    AP_errStat_reset (1);

    // reExecute line 
    irc = WC_Work1 (lNr, cbuf);  // 0=OK, -1=Err, 

    // reset PosCodEnd
    if(p1) *p1 = '\n';


  return irc;

}


//=======================================================================
  int WC_Work1 (int lNr, char* cbuf) {
//=======================================================================
/// \code
/// main-interpreter-function
/// Input:
///   lNr
///   cbuf      must be terminated with '\'; without objName.
/// Output:
///   retCod     0 = OK;
///             -1 = Error, stop.
///             -2 = invisible obj (joint, activity), continue
///             -3 = obj not yet complete
///
/// subModels must be loaded (else use WC_Work__).
/// \endcode
/*
Die Hautabarbeitungs routine.

recTyp:
 - 0 = Kommentar/Leersaetze ..  (# Text)
 - 1 = CadDefinitionsaetze (P20=..)
 - 2 = NC-Saetze  (FROM ..)


APT_stat_act:
 PrgMod_normal            0
 PrgMod_skip_until_label  2
 PrgMod_skip_until_macend 3
 PrgMod_skip_until_mac    4
 PrgMod_skip_until_line   5
 PrgMod_skip_until_file   6

*/



  long      dli;
  int       ctyp, i1, i2, rc, irc, Retcod, recTyp;
  // long      l1;
  double    d1;
  char      deli, *w_act, *w_next, *c_act, *c_next;
  char      txtOut[64];
  char      cmd[32], auxc4[4]; 
  Point     pt1;
  ObjAto    ato1 = _OBJATO_NUL;


  // if(strlen(cbuf) < 1) return 0;


  //---------------------------------------------------------------------
  // printf("XXXXXXXXXXXXX WC_Work1 lNr=%d len=%ld\n",lNr,strlen(cbuf));
  // printf("|");UTX_dump_cnl (cbuf, 60); printf("| %d\n",APT_stat_act);
  // printf("  APT_obj_stat=%d\n",APT_obj_stat);
  // printf("         WC_modact_ind=%d AP_modact_nam=|%s|\n",WC_modact_ind,AP_modact_nam);
  // printf("WC_Work1 - AP_stat.batch = %d\n",AP_stat.batch);
  // printf("|%s|\n",cbuf);

    // TEST
    // if(!strncmp(cbuf,"FROM ",5)) return 0;

  // 2001-06-05 zugefuegt:
  // AP_dli_act = 0;



  APT_line_act  = lNr;
  APT_line_stat = 0;
  Retcod        = 0;
  // APT_set_modMax (0);     // 2016-08-26 - dont kill value if mod too high


  if(APT_stat_act == PrgMod_normal) {

/*  2005-12-09 raus; loescht Submodels !
    // vor erster Zeile: Init alles.
    if(APT_line_act == 1) {
      //TX_Write("Line1 - Init");
      WC_Init();
      DB_Init  (0);
      GL_Clear ();
      APT_Init ();
      ED_Init  ();
    }
*/

  // skip all Lines im PrgMod_skip_until_line bis Zeile Prg_line_nr
  // (return nach CALL)
  } else if(APT_stat_act == PrgMod_skip_until_line) {

    if(APT_line_act == Prg_line_nr) {
      ED_skip_end ();   // reset vorherigen mode (STEP oder GO
      APT_stat_act = 0;

    } else {
      goto Fertig;
    }
  }



  c_act = cbuf;


  if(strlen(cbuf) < 1) goto L_exit99;
  if(cbuf[0] == '#') goto L_exit99;
  if(cbuf[0] == '_') goto L_exit99;

  // printf(" AP_sysStat=%d\n",AP_sysStat);

  // Memory --> aus_typ, aus_tab zuweisen
  // get memSpc for atomicObjects
  // ATO_getSpc1 (&SRC_ato_SIZ, &SRC_ato_typ, &SRC_ato_tab);
/*
  SRC_ato_SIZ = 1000;
  SRC_ato_typ = (int*) MEM_alloc_tmp (SRC_ato_SIZ * sizeof(int));
  SRC_ato_tab = (double*) MEM_alloc_tmp (SRC_ato_SIZ * sizeof(double));
*/


  //==========================================================================
  NextCmd:
    // printf("  NextCmd |%s|\n",c_act);

  // den naechsten Command > Line kopieren
  // c_next = UTX_cp_word_term (Line, c, ';');
  //TX_Print("  Line = /%s/",Line);




  // w = UTX_pos_1n (c); 

  // skip starting blanks
  w_act = c_act;
  while (*w_act == ' ') ++w_act;
  

  



  //===================== eine Zeile abarbeiten ==============================
    // TEST
    // // if(!strncmp(w,"FROM ",5)) return 0;
    // if(!strncmp(w,"FROM ",5)) {strcpy(txtOut,"FROM"); goto L_work_2;}


  // skip geloeschte Zeilen
  if (*w_act == '_') goto Fertig;

  /* skip Kommentar-Zeile (gesamten Zeilenrest) */
  if (*w_act == '#') goto Fertig;

  // Fortsetzungszeilen werden generell geskippt.
  if (*w_act == '&') goto Fertig;

  // // skip das Label "DYNAMIC_AREA"
  // if(!strcmp(w, ":DYNAMIC_AREA")) goto Fertig;


  // das erste Wort > txtOut 
  ctyp = UTX_get_word (&w_act, &w_next, txtOut, &d1, &deli);
    // printf("word ctyp=%d deli=|%c| cmd=|%s| w_nxt=|%s| d1=%f\n",
                 // ctyp,   deli,     txtOut,  w_next,    d1);


  if (ctyp == TXT_Typ_EOF) {
    goto Fertig;
  }



  
  L_work_2:
  UTX_cp_word_2_upper (cmd, txtOut);
    // printf(" nxt cmd=|%s|\n",cmd);



  //----------------------------------------------------------------
  // handle lables; eg  "Loop1:"            - ':'  marks as label
  // im Normalmode Labels skippen (beginnen mit :)
  if(APT_stat_act == 0) {
    if (*w_act == ':') {
      goto Done;
    }

  // skip all Lines im PrgMod_skip_until_label (suchen JUMP - Label)
  } else if(APT_stat_act == PrgMod_skip_until_label) {

    if (*w_act != ':') {
      goto Fertig;

    } else {
      if(!strcmp(w_next, APT_label)) {
        ED_skip_end ();
        APT_stat_act = 0;
        APT_line_old = APT_line_act;
        goto Done;
      }
      goto Fertig;
    }

  // skip all Lines im PrgMod_skip_until_macend (ein MAC skippen)
  } else if(APT_stat_act == PrgMod_skip_until_macend) {

    if(!strcmp(cmd, "END")) {
      ED_skip_end ();
      APT_stat_act = 0;
      goto Done;

    } else {
      goto Fertig;
    }

  // skip all Lines im PrgMod_skip_until_mac bis MAC APT_macnam
  // (ein CALL-Up suchen)
  } else if(APT_stat_act == PrgMod_skip_until_mac) {
    //printf("PrgMod_skip_until_mac |%s|%s|%s|\n",cmd,APT_macnam,w_next);

    // soll nach MAC oder nach GEO gesucht werden -
    if(UP_act_typ == 'M') {
      strcpy (auxc4, "MAC");
    } else {
      strcpy (auxc4, "GEO");
    }
    if(!strcmp(cmd, auxc4)) {
      if(!strcmp(w_next, APT_macnam)) {
        //TX_Print("found |%s| reset.",auxc4);
        ED_skip_end ();    // wieder auf vorherigen Mode zurueckschalten !??
        APT_stat_act = 0;  // ab jetzt normal abarbeiten (bis "END")
/*
        // wenn jetzt was gepuffert ist - was ist das ??
        if(preAnz > 0) {
          TX_Error("gepuffert: %d",preBuf[0]);
        }
        preAnz = 0;     // es darf jetzt keine gepufferten Konturelemente geben
*/
        goto Done;
      }
      goto Fertig;

    } else {
      goto Fertig;
    }


  }





  //======================================================================
  // Ab hier gibts wirklich was zu arbeiten.
  // Do_It:

  // printf("////////Start WC_Work %d |%s| %d\n",APT_line_act,w_act,APT_stat_act);
  // printf("line_act=%d line_old=%d\n",APT_line_act,APT_line_old);
  // printf("line_stat=%d mode=%d\n",APT_line_stat,ED_query_mode());

  // Notstop!!
  //if(APT_line_cnt > 50) return -1;


  // nur bei Verarbeitung aus dem Memory (nicht aus File)
  if (ED_get_mac_fil() == ON) goto L_work_12;

  // Wenn in dieser Zeile noch nix getan wurde:
  if (APT_line_stat == 0) {
    APT_line_stat = 1;

/* wird von WC_actPos_reset erledigt !!!
   // nur im Edit-Mode -
   // wenn der User mit dem Cursor zurueckgesetzt (zur Sourcekorrektur) hat:
   // Bei JUMP rueckwaerts deswegen APT_line_old = APT_line_act erforderlich.
   i1 = ED_query_mode();
   if   ((APT_line_act != 1) &&
         (i1 == ED_mode_enter) &&
         (APT_line_act <= APT_line_old)) {

      // // den letzen DL-Ind suchen. Wegen Leerzeilen gibts Luecken in der Tab.
      // dli = APT_dlInd_get (APT_line_act-1);
      // // und die DispList zuruecksetzen
      // GL_Delete (dli);

      //GL_Redraw();  wird sowieso nachfolgend vom Enter erledigt
      // den aktuellen Punkt wiederherstellen
      actPosU = actPosU_tab[APT_line_act-1];
      actPosUtr = APT_transl2 (&actPosU);
    }
*/
  }

  
  //----------------------------------------------------------------
  L_work_12:
  if(deli == '=')  recTyp = 1;  // CadDefRec
  else             recTyp = 2;  // NC-Saetze
    // printf(" recTyp = %d ctyp = %d\n",recTyp,ctyp);


/*
  // Demo-Mode: nur xx Zeilen abarbeiten
  if(APT_line_act > 19) {
    if(l_irc != 25) {
      goto Fertig;
    }
  }
*/

  // is this a NCCommandLine (from/0,0) or a DefinitionLine (pt1=100,100)
  // or a Bearbeitungscommand (L0)
  if(deli == '=') {
    /* ist eine DefinitionLine. */
    APT_defTxt = w_next;
    irc = APT_work_def (txtOut, &w_next);
      // printf(" after APT_work_def %d\n",irc);
    if(irc < 0) return irc;
    if(irc == Typ_Activ) goto L_exit99; // nix sichtbar ...


  } else {

    // Check for direkt-Insert
    if(ctyp == TXT_Typ_string) {
      //printf("DirektInsert |%s|\n",txtOut);
      strcpy (txtOut, "INL");
      // APT_work_NCCmd (txtOut, &w_act);
      PRC__ (15, w_act);   //15="INL"
      goto Done;
    }

    // this is not a definition-command (no '=')
    // do command (HIDE VIEW MODSIZ DEFTX EXECM .. (AppCodTab))
    i1 = APT_work_AppCodTab (cmd, &w_next);
    if(i1 >= 0) goto Done;
    if(i1 == -1) return i1;
    // -2 == not_in_AppCodTab


    // test if its a programming-code (JUMP MAC GEO END IF)
    i1 = UTX_wTab_ck (PrgCodTab, cmd);
    if(i1 >= 0) {
      // work AppCodTab[i1]
      rc = APT_work_PrgCodTab (i1, &w_next);
        // printf(" ex _PrgCodTab-rc=%d\n",rc);

/*
      goto Done;
    }


    // Ist das erste Wort eine Programming-Funktion -
    for (i1=0; i1<PrgCmdAnz; i1++) {
        // printf(" Work1-comp %d !%s!%s!\n",i1,*(&PrgCmdTab[i1]),cmd);

      if (!strcmp(*(&PrgCmdTab[i1]),cmd)) {

        // work programming-function; retCod = typ-of-function.
        // rc = APT_work_PrgCmd (txtOut, &w_next);
        rc = APT_work_PrgCmd (cmd, &w_next);        // 2016-03-16
*/

        // 1 = IFXX Funkion: work rest of Line.
        if(rc == PrgMod_continue_if) {
          // retCod of if-expression is yes (APT_eval_if), work if-command
          c_act = w_next;
          goto NextCmd;


        // JUMP-Funktion. Label APT_label suchen.
        } else if(rc == PrgMod_skip_until_label) {
          APT_stat_act = PrgMod_skip_until_label;
          if(APT_mac_fil == 0) {
            ED_file__ (3, NULL); // rewindActiveFile
          } else {
            ED_Reset();          // restart prog fom Line 1 ..
          }
          ED_skip_start ();
          goto L_exit99;  // do not create a actPos-Record


        // MAC-Funktion. PrgMod_skip_until_macend
        } else if(rc == PrgMod_skip_until_macend) {
          APT_stat_act = PrgMod_skip_until_macend;
          ED_skip_start ();
          // goto Fertig;
          goto L_exit99;  // do not create a actPos-Record


/*
        // END-Funktion. Got END of MAC;
        } else if(rc == PrgMod_continue_mac) {
          APT_stat_act = PrgMod_normal;
          goto L_exit99;  // do not create a actPos-Record
*/


        // CALL-Funktion. PrgMod_skip_until_mac
        } else if(rc == PrgMod_skip_until_mac) {
          APT_stat_act = PrgMod_skip_until_mac;
          ED_Reset();       // restart prog fom Line 1 ..
          ED_skip_start ();
          // actPos-Record wurde schon geschrieben !
          goto L_exit99;  // do not create a actPos-Record


        // return-Funktion. PrgMod_skip_until_line
        } else if(rc == PrgMod_skip_until_line) {
          APT_stat_act = PrgMod_skip_until_line;
          ED_Reset();       // restart prog fom Line 1 ..
          ED_skip_start ();
          goto L_exit99;  // do not create a actPos-Record


        // CALL-File-Funktion. PrgMod_skip_until_file
        } else if(rc == PrgMod_skip_until_file) {
          //APT_stat_act = PrgMod_skip_until_macend;
          Retcod = ED_work_file (APT_filnam);
          goto L_exit99;  // do not create a actPos-Record


        // 
        } else if(rc == PrgMod_continue_file) {
          Retcod = ED_cont_file ();
          goto Fertig;


        }


        // 0: normale Funktion. Fertig.
        goto Done;
      }
    // }


    //-------------------------------------------------------
    // Ist das erste Wort eine NC-Funktion oder ein Verfahrweg-
    L_ck_prc:
    if(process_CmdTab) {
      for (i1=0; i1<1000; i1++) {
        if (!process_CmdTab[i1]) break;
          // printf("vergl NC %d !%s!%s!\n",i1,*(&NCCmdTab[i1]),cmd);
        if (!strcmp(*(&process_CmdTab[i1]),cmd)) {
          PRC__ (i1, w_next);
          goto Done;
        }
      }
    }


    //-------------------------------------------------------
    // somit muss es eine Bearbeitungsanweisung sein.
    // get tempSpace for string w_act
    ATO_tmpSpc_get_s (&ato1, w_act);
    if(!ato1.typ) {TX_Error("WC_Work1 E1"); return -1;}

    // decode w_act
    ATO_ato_srcLn__ (&ato1, w_act);
    if(ato1.nr < 1) goto Done;


    // SRC_ato_anz = APT_decode_ausdr (SRC_ato_typ, SRC_ato_tab, SRC_ato_SIZ, &w);
    if(ato1.nr == 1) {
      // handle change Graf. Attribut u. Refsys (R#;   or G#;(
      if(APT_do_auxCmd (ato1.typ, ato1.val) == 0) goto Done;
    }


    // Test for "G2 L20 ..";
    // AchtunG: "G2" wird jedoch in APT_do_auxCmd bearb.!
    if(ato1.typ[0] == Typ_G_Att) {
      // load attributed objects into PermanentAttributTable GA
      GA_LtypTab (ato1.nr, ato1.typ, ato1.val);
      goto Done;
    }


    // cannot specify ..
    TX_Error(w_act);

  }

  


  Done:
  // printf(" nach APT_cut_line 2 /%s/\n",c);
  // if(*c_next == '\0') goto Fertig;
  // c = c_next;
  // goto NextCmd;





  //=============== Zeile fertig =============================================
  Fertig:;
  // printf(" APT_line_stat=%d APT_line_act=%d\n",APT_line_stat,APT_line_act);
  if(APT_line_act < 0) goto L_exit99; // skip DYN-BLOCK-Zeilen



  // Wenn in dieser Zeile was getan wurde:
    // die actPosU merken und an P0 uebergeben
  // if(APT_line_stat == 1)
    // if(recTyp == 2)
      // WC_actPos_save (1);


  // UI_GR_set_sel_ (-1L);  // clear selection



/*
  // nur bei Verarbeitung aus dem Memory (nicht aus File)
  // und nur beim primary Model
  if((ED_get_mac_fil() != ON) &&
     (WC_modact_ind < 0))              {

    // die von der aktuellen Zeile erzeugte ObjektID merken 2001-06-05
    AP_dli_act = GL_GetActInd();

    // wurde kein neues Objekt erzeugt: nicht speichern !
    i1 = APT_dlInd_last (APT_line_act);
    if(i1 >= AP_dli_act) goto L_exit99;

    obj_ind_tab[APT_line_act] = AP_dli_act;
    if(APT_line_act > obj_ind_Nr) obj_ind_Nr = APT_line_act;
    // printf("AAAAPT_line_act=%d AP_dli_act=%d\n",APT_line_act,AP_dli_act);

  }
*/

  // printf("ex WC_work1 %d\n",Retcod);

  L_exit99:
  return Retcod;


}


/*
//======================================================================
  int APT_dump_itab () {
//======================================================================

  int    i1;

  printf("APT_dump_itab %d\n",obj_ind_Nr);

  for(i1=0; i1<=obj_ind_Nr; ++i1) {
    printf(" %d ind=%d\n",i1,obj_ind_tab[i1]);
  }

  return 0;

}
*/

/*
//======================================================================
  char* APT_concat (char *ptx) {
//======================================================================
// aktuelle Zeile u. Folgezeilen -> mem_cbuf1

  int    ip, lNr;


  // printf("APT_concat:   last char = |%c|\n",ptx[strlen(ptx)-1]);


  if(ptx[strlen(ptx)-1] != '\\') {    // Fortsetzungszeile ?

    return ptx; // nein, normal weiter

  }




  strcpy(mem_cbuf1, ptx);
  mem_cbuf1[strlen(mem_cbuf1)-1] = '\0';



  L_next:

  ED_GetNxtLin (&lNr, APT_txBuf);  // ZeilenNr und Zeile einlesen
  ++APT_line_act; // ??
  // printf(" Line %d |%s|\n",lNr,APT_txBuf);


  // der erste char muss ein "&" sein, sonst Fehler.
  if(APT_txBuf[0] != '&') {
    TX_Error("Fortsetzungszeile muss mit & beginnen");
    printf("Fortsetzungszeile muss mit & beginnen\n");
    return NULL;
  }

  strcat(mem_cbuf1, &APT_txBuf[1]);   // das & skippen


  ip = strlen(mem_cbuf1)-1;
  if(mem_cbuf1[ip] == '\\') {    // Fortsetzungszeile ?
    mem_cbuf1[ip] = '\0';        // remove "\"
    goto L_next;
  }


  // printf("ex APT_concat len=%d |\n",ip);
  // printf("|%s|\n",mem_cbuf1);


  return mem_cbuf1;
}
*/



//========================================================
  int APT_do_auxCmd (int* i_typ, double* i_tab) {
//========================================================
/// handle change Graf. Attribut u. Refsys
// R12 = Aktivieren von Refsys Nr. 12.
// R0  = Reset
// G20 = Aktivieren des Attributtabelleneintrages 20


  int   i1;
  long  dli;
  char  cbuf[16];


  // printf("APT_do_auxCmd %d %f\n",i_typ[0],i_tab[0]);
  // for(i1=0;i1<


    //---------- Change Graf. Attribut  eg. "G20" Typ_G_Att=105
    if(i_typ[0] == Typ_G_Att) {
      //TX_Print(" change att > %d",(int)i_tab[0]);
      NC_setGrafAtt ((int)i_tab[0]);
      return 0;



/*
    //---------- Change Tool
    } else if(i_typ[0] == Typ_Tool) {
      // printf(" change Tool > %d",(int)i_tab[0]);
      i1 = i_tab[0];
      APT_tl_stat = i1;

      ++APT_line_cnt;
      sprintf(cbuf, "T%d",i1);
      APT_stack_NCCmd (cbuf, 0);

      return 0;
*/




    //---------- Change active Plane (Refsys)
    } else if(i_typ[0] == Typ_PLN) {

      //TX_Print(" change pln > %d",(int)i_tab[0]);
      i1 = NC_setRefsys ((int)i_tab[0]);
      // ein dummy-DL-Record, der als DL.irs den neuen RefsysIndex hat.
      // Nur damit kann DL_setRefSys beim MAN-Editor-GoUp und GoDown das
      // richtige UCS anzeigen.
      if(i1 == 0) {
        dli = DL_StoreObj (Typ_apDat, -1L, 0);
        DL_unvis_set (dli, 1);         // make unvisible
      }
      return 0;

/*
      // _2d_to_3d_mode kann nur im 2D-Mode aktiviert werden !
      if(APT_3d_mode == ON) {
        TX_Error("Referenzsystem kann nur im 2D-Mode aktiviert werden");
        return 0;
      }
      // Index 0 ist das Hauptachsensystem (= Zur�cksetzen)
      APT_2d_to_3d_Ind  = i_tab[0];
      if(APT_2d_to_3d_Ind == 0) {
        APT_2d_to_3d_mode = OFF;
      } else {
        APT_2d_to_3d_mode = ON;
        // Plane und Matrix beladen
        DB_GetRef (&APT_2d_to_3d_Pln, APT_2d_to_3d_Ind);
        UT3D_m3_loadpl (APT_2d_to_3d_Mat, &APT_2d_to_3d_Pln);

        TX_Print(" matVX=%f,%f,%f",APT_2d_to_3d_Mat[0][0],APT_2d_to_3d_Mat[1][0]
,APT_2d_to_3d_Mat[2][0]);
        TX_Print(" matVY=%f,%f,%f",APT_2d_to_3d_Mat[0][1],APT_2d_to_3d_Mat[1][1]
,APT_2d_to_3d_Mat[2][1]);
        TX_Print(" matVZ=%f,%f,%f",APT_2d_to_3d_Mat[0][2],APT_2d_to_3d_Mat[1][2]
,APT_2d_to_3d_Mat[2][2]);
        TX_Print(" matPO=%f,%f,%f",APT_2d_to_3d_Mat[0][3],APT_2d_to_3d_Mat[1][3]
,APT_2d_to_3d_Mat[2][3]);

      }
*/


    }


  // nix gfundn
  return -1;

}


//======================================================================
  int APT_work_def (char* cmdIn, char** data) {
//======================================================================
/// \code
/// Work DefinitionLine (decode, store obj in DB, display obj).
/// RetCod: defTyp.  or -3 (obj not yet complete)
/// Input:
///   cmdIn    obj left of '='
///   data     pointer to srcTxt right of '='
/// \endcode

  static int level=0;

  int            irc, i1, defTyp, basTyp, typTyp, iAtt, iPar[4];
  long           l1, dli, dbi, gaNr, defInd;
  char           *ptx;
  ObjGX          odb;
  ObjAtt         *ga1;
  ColRGB         col1;
  ObjAto         ato1 = _OBJATO_NUL;
  MemTab(ObjSRC) mtPar = _MEMTAB_NUL;
  // Ind_Att_ln lnAtt1;


  // printf("======================================================== \n");
  // printf("APT_work_def |%s| = |%s|\n",cmdIn,*data);
  // printf(" APT_obj_stat=%d\n",APT_obj_stat);
  // printf("   lnlen= %ld\n",strlen(*data));
  // UTX_dump_c__ (*data, 50); printf("\n");


  DB_dyn__ (0, Typ_PT, 0L);      // save state of dyn-points


  // init ..
  APT_prim_typ = 0;
  APT_hide_parent = 0;           // 0=not, else yes
  AP_dli_act = -1L;


  // get destination-obj (DB-typ,index)
  i1 = APED_dbo_oid (&defTyp, &defInd, cmdIn);
  if(i1 < 0) {
      TX_Error("ObjHeader: |%s|",cmdIn);
      goto Error;
  }
    // printf("Typ = %d, Ind %d lev=%d\n",defTyp,defInd,level);



  // zusammenziehen -> ptx, wenn es Fortsetzungszeilen gibt.
  // ptx = APT_concat (*data);
  ptx = *data;
  if(ptx == NULL) return -1;


/* replaced by ATO_ato_srcLn__
  // nun die Ausdruecke einlesen, decodieren und merken
  SRC_ato_typ[0] = 0;
  SRC_ato_anz = APT_decode_ausdr (SRC_ato_typ, SRC_ato_tab, SRC_ato_SIZ, &ptx);
*/

  // init ato1
  // ato1.siz    = SRC_ato_SIZ;
  // ato1.typ    = SRC_ato_typ;
  // ato1.val    = SRC_ato_tab;
  // ato1.ilev   = (short*) MEM_alloc_tmp ((int)(SRC_ato_SIZ * sizeof(short)));
  // ato1.nr     = 0;

  // get memSpc
  ATO_tmpSpc_get_s  (&ato1, ptx);
  if(!ato1.typ) {TX_Error("APT_work_def EM1"); return -1;}



  if(!APT_obj_stat) {
    // permanent;
    // get tempSpc for <ato1.siz> parent-records
    MEMTAB_tmpSpc_get (&mtPar, ato1.siz);
    if(MEMTAB_RMAX(&mtPar) != ato1.siz) {
      TX_Error("APT_work_def EM2");
      ATO_tmpSpc_free (&ato1);
      return -1;
    }

    // get atomic-objects (ato1) and parents of source ptx
    APT_ato_par_srcLn (&mtPar, &ato1, ptx);

  } else {
    // temporary; get atomic-objects (ato1) of source ptx
    ATO_ato_srcLn__ (&ato1, ptx);
    mtPar.rNr = 0;
  }


  // // get nr of atomic-objects
  // SRC_ato_anz = ato1.nr;


    // TEST 
    // ATO_dump__ (&ato1, " nach-_ato_srcLn__");
    // TEST END


  // Obj loeschen (ganz loeschen; ZB "L20=")
  if(ato1.nr < 1) {
    // printf("loesche obj Typ = %d, Ind %d\n",defTyp,defInd);
    // Obj aus DB loeschen
    DB_Delete (defTyp, defInd);

    // Obj aus DL loeschen
    dli = DL_find_obj (defTyp, defInd, -1L);
    if(dli >= 0) GL_Del0 (dli);
    DL_Redraw ();

    // goto Fertig;
    goto Error;             // 2011-08-11
  }


  // // im tempModus index -> 0 (ganz wichtig fuer translate !!)
  // if(APT_obj_stat == 1) defInd = 0;


  // die ersten 2 Parameter merken (aus_typ & _tab werden bei Draw geloescht!)
  iPar[0] = ato1.typ[0];
  iPar[1] = ato1.val[0];
  iPar[2] = ato1.typ[1];
  iPar[3] = ato1.val[1];


  // clear subTyp (only used for ray)
  APT_subTyp = 0;


  //----------------------------------------------------------------
  // decode all parameters, create binary obj; store obj in DB.
  i1 = APT_store_obj (&defTyp, &defInd, ato1.nr, ato1.typ, ato1.val);
    // printf("ex _store_obj %d defTyp=%d defInd=%ld\n",i1,defTyp,defInd);
  if(i1 < 0) {
    if(i1 == -2) goto Fertig; // joints,activities,vectors; do not draw; else OK
    defTyp = i1; // -1=Error; -3=object_not_yet_complete
    goto Fertig;
  }


  //----------------------------------------------------------------
  if(!APT_obj_stat) {
    // set isParent-bit in all DL-records of the parent-objects
    APT_parent_set (&mtPar, &ato1);

    // hide parent-objs
    APT_parent_hide (&mtPar);
  }


  //----------------------------------------------------------------
  // create PRCV (polygon of curve)
  if((defTyp < Typ_CV)&&(defTyp != Typ_CI)) goto L_attribs;
  if(defTyp >= Typ_PLN) goto L_attribs;
  // if(!defInd) goto L_attribs;   // temporary-mode
  if(APT_obj_stat) goto L_attribs;   // temporary-mode 
  // create PRCV for DB-obj
  irc = PRCV_set_dbo__ (defTyp, defInd);
  if(irc < 0) return -1;


  //----------------------------------------------------------------
/*
  // skip punkt if noDisp
  if(defTyp == Typ_PT) {
    // printf(" _work_def defTyp=%d disp=%d\n",defTyp,APT_dispPT);
    if(APT_dispPT != ON) goto Fertig;
  }

  // skip plane if noDisp
  if(defTyp == Typ_PLN) {
    // printf(" _work_def defTyp=%d disp=%d\n",defTyp,APT_dispPL);
    if(APT_dispPL != ON) goto Fertig;
  }
*/



  //----------------------------------------------------------------
  // get attributes
  L_attribs:
  // if obj cannot have attributes: goto draw ..
  // .. goto L_draw;

  // reduce typ -> Basistyp (Typ_SURRU -> Typ_SUR ..)
  basTyp = AP_typ_2_bastyp (defTyp);

  // check for point|curve|surface
  typTyp = UTO_ck_typTyp (basTyp);


  //----------------------------------------------------------------
  // fix attributes

  // DL_disp_act = 0;  // ON, normal
  DL_disp_def (0);   // DL_disp_act=0; nicht hidden-normal.


  // apply active attibute.
  // fix attribute for curve
  if(typTyp == Typ_PT) {   // point
    iAtt = GR_Att_act;

  // fix attribute for curve
  } else if(typTyp == Typ_go_LCS) {   // curve
    if(APT_subTyp) {   // APT_subTyp = linetyp; see cfg/ltyp.rc
      iAtt = APT_subTyp;
      // IndAttLn_Set1 (&iAtt, APT_subTyp, 0);
    } else {
      iAtt = GR_Att_act;
    }


  // fix attribute for Notes
  } else if(typTyp == Typ_Note) {   // curve
      iAtt = GR_Att_act;

  // fix attribute for surface/solid
  } else if(typTyp == Typ_SUR) {
    iAtt = GR_Att_act;
  }



  // set attribute for temporary object
  if(defInd == 0L) {                     // temp. CAD-obj

    if(typTyp == Typ_SUR) {
      col1.vsym = 1;            // temp.surf -> symbolic
      memcpy(&iAtt, &col1, sizeof(int));
      goto L_draw;


    } else if(typTyp == Typ_go_LCS) {   // curve
      iAtt = Typ_Att_hili1;    // curves
      goto L_draw;


    } else if(typTyp == Typ_PT) {
      iAtt = ATT_PT_HILI;
      goto L_draw;
    }

    goto L_draw;                                  // 2014-02-18
  }


  // find GA-rec if already exist
  gaNr = GA_find__ (basTyp, defInd);
  if(gaNr < 0) goto L_draw;           // skip, no GA-Record exists
    // GA_dump_1 (gaNr, stdout, "  _work_def-1");    // 2013-08-15



  //----------------------------------------------------------------
  // GA-record exists for active object. Get it.
  GA_getRec (&ga1, gaNr);  // get the ga-record


  // in Submodels keine hidden objects generieren ( diese sind sonst in
  // Dittos des mainmodels sichtbar!)
    // printf(" WC_modact_ind=%d ga1->disp=%d\n",WC_modact_ind,ga1->disp);
  if(WC_modact_ind >= 0) {     // 0-n = sind in Submodel; -1=main
    // Submodel is active (main is -1)
    // if(AP_modact_nam[0] == '\0') {     // active Model is main, wenn name leer
      // i1 = GA_hide__ (8, defInd, defTyp); // ask state; 1=hidden
      // // printf(" obj %d %d = %d\n",defTyp,defInd,i1);
      // if(i1 > 0) return 0;
      if(ga1->disp > 0) goto Fertig;
    // }
  }


  // if obj == hidden: preset hidden fuer naechsten DL-Record (DL_StoreObj)
  if(ga1->disp > 0) DL_disp_def (1); // DL_disp_act = 1; // hidden


  // // Surf/Solid/Model: es ist eine ColRGB; LN/AC/Curve: ist ist LtypNr.
  // iAtt = ga1->iatt;


  // Typ_SUR: if (hasTexture): reset theTexturePosition in the TextureReference
// Textur sollte nur neuberechnet werden, wenn Fl�che ver�ndert wurde ..
// 2010-04-17 raus. RF.
  if(typTyp == Typ_PT) {
    iAtt = ga1->iatt; // iatt is a int; see GL_InitPtAtt


  //----------------------------------------------------------------
  // curve-attributes (linetyp)
  } else if(typTyp == Typ_go_LCS) {
    // curve; is LtypNr.
    // i1 = IndAttLn_get_ltyp (&ga1->iatt);
    // IndAttLn_Set1 (&iAtt, i1, -1);  // change ltyp, keep lim !
    iAtt = ga1->iatt;   // iatt is a Att_ln

  //----------------------------------------------------------------
  // surfaces-attributes (color, symbolic, texture)
  } else if((basTyp == Typ_SUR) ||
            (basTyp == Typ_SOL)) {
    iAtt = ga1->iatt;       // iatt is a ColRGB
      // UT3D_stru_dump (Typ_Color, &iAtt, " col ex GA: ");
    if(TSU_mode == 0) {  // nur bei Draw, nicht bei Store
      if(GA_hasTexture(iAtt) == 1) {
        // get TextureReference-Index
        i1 = GA_getTexRefInd(iAtt);
        // reset its SurfData
        Tex_TexRef_InitS (i1);
      }
    }
  }




  //----------------------------------------------------------------
  L_draw:
  // printf("APT_work_def - work %d %d lev=%d\n",defTyp,defInd,level);

  // get DB-obj from DB;  create|overwrite DL-record; display obj.
  APT_Draw__ (iAtt, defTyp, defInd);


  // set isChild-bit in DispList. DL-record AP_dli_act is a Child - has parents.
  if((mtPar.rNr)&&(AP_dli_act>= 0L)) 
    DL_child_set (AP_dli_act, 1);   // GR_ObjTab[AP_dli_act].sChd = 1;
 


  //----------------------------------------------------------------
  // overwrite display of already present obj's  (zB "L=CUT L L")
  // delete display of zB  ungetrimmtes Basisobj
  // Nicht in IE_cad_test__, weil es sonst bei ReRUN nicht mehr geht ...

    // printf(" typ=%d ind=%ld\n",defTyp,defInd);
    // printf(" aus_[0] = %d %d\n",iPar[0],iPar[1]);
    // printf(" aus_[1] = %d %d\n",iPar[2],iPar[3]);

  // hide parent-obj
  // APT_work_parent_hide (defTyp, defInd, iPar);




  //----------------------------------------------------------------
  Fertig:
    // free 
    ATO_tmpSpc_free (&ato1);
    if(!APT_obj_stat) MEMTAB_tmpSpc_free (&mtPar);

    return defTyp;


  //----------------------------------------------------------------
  Error:
    defTyp = -1;
    goto Fertig;


}


//========================================================================
  int APT_ato_par_srcLn (MemTab(ObjSRC) *mtPar, ObjAto *ato, char *srcLn) {
//========================================================================
/// \code
/// get atomicObjects and parents from sourceLine; full evaluated.
///   (provides dynamic-DB-obj's for geometric expressions)
/// must provide memspc before with ATO_getSpc__
/// Input:
///   srcLn      eg "D(0 0 1)"      // may not have objNames !
///   mtPar      free space for parents (get with MEMTAB_tmpSpc_get)
///   ato        free space for atomic-objs (ATO_getSpc_siz + ATO_tmpSpc_get__)
///              or NULL for no output of atomic-objs
/// Output:
///   mtPar      parents
///   ato        atomic objects;  not if NULL
///   RetCod     0=OK, -1=Err
/// Example:
///  ATO_getSpc__ (&ato);
///  i1 = ATO_ato_srcLn__ (&ato, s1);
///    ATO_dump__ (&ato, " after _ato_srcLn__");
/// \endcode


  int       irc, fMspc1, itsMax, ib, iato=1;
  ObjAto    at1 = _OBJATO_NUL;
  ObjTXTSRC *tso;
  MemTab(ObjTXTSRC) mtso = _MEMTAB_NUL;


  // printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
  // printf("APT_ato_par_srcLn |");
  // UTX_dump_cnl(srcLn,50);printf("|\n");
  // printf(" ilev=%p\n",ato->ilev);

  // max nr of atos
  itsMax = SRCU_tsMax (srcLn);


  if(ato == NULL) {
    iato = 0;         // ato must not be evaluated at end; only parents
    // get temp-spc for atomic-objs 
    ato = &at1;
    // get size necessary for ato
    ib = ATO_getSpc_siz (itsMax);
    // get tmp-memspc (persists only until end of this func; must ATO_tmpSpc_free)
    ATO_tmpSpc_get__ (ato, ib);
    // ATO_getSpc_tmp__ (ato, itsMax);
    // if(!ATO_getSpc_tmp_ck(ato)) {TX_Error("APT_ato_par_srcLn E1"); return -1;}
    if(!at1.typ) {TX_Error("APT_ato_par_srcLn E1"); return -1;}
  }


  // get memspc until end-of-func (alloca|malloc)
  MEMTAB_tmpSpc_get (&mtso, itsMax);
  tso = mtso.data;
  if(tso == NULL) {TX_Print("APT_ato_par_srcLn E2"); return -1;}


  // analyze sourceline; get source-objects -> tso
  irc = APED_txo_srcLn__ (tso, itsMax, srcLn);
    // printf(" _txo_srcLn__ %d\n",irc);
  if(irc < 1) goto L_exit;
    // APED_txo_dump (tso, srcLn, "nach-txo_srcLn__");


  // get atomic-objects from source-objects
  irc = ATO_ato_txo__ (ato, tso, srcLn);
  if(irc < 0) goto L_exit;
    // ATO_dump__ (ato, " nach _ato_txo_");


  // get parents of tso
  ATO_parents__ (mtPar, ato);


  // evaluate atomic-objects (compute)
  irc = ATO_ato_eval__ (ato);


  L_exit:
  // free memspaces
  if(!iato) ATO_tmpSpc_free (ato);
  MEMTAB_tmpSpc_free (&mtso);

  return irc;

}


//================================================================
  int APT_parent_set (MemTab(ObjSRC) *mtPar, ObjAto *ato) {
//================================================================
// set isParent-bit bei allen parents

  int     i1, ie;
  long    dli;

  ie = mtPar->rNr;

  // printf("APT_parent_set %d\n",ie);

  for(i1=0; i1<ie; ++i1) {   // loop tru parent-obj's
    dli = mtPar->data[i1].dli;

    // set dispList-flag sPar (obj is a parent - has childs)
    DL_parent_set (dli, 1);
  }

  return 0;

}


//================================================================
  int APT_parent_hide (MemTab(ObjSRC) *mtPar) {
//================================================================
// replace APT_work_parent_hide
// APT_hide_parent: 1=hide, elso not

  int     i1, ie;
  long    dli;
  
  ie = mtPar->rNr;
  

  for(i1=0; i1<ie; ++i1) {   // loop tru parent-obj's
    dli = mtPar->data[i1].dli;
  
    // hide or redisplay
    DL_unvis__ (dli, APT_hide_parent);

  }

  return 0;

}

 
/* replaced, unused
//================================================================
  int APT_work_parent_hide (int cldTyp, long cldDbi, int *iPar) {
//================================================================
/// \code
/// APT_work_parent_hide               hide parent-obj
/// overwrite display of already present obj's  (eg "S<child>=CUT L<parent> ..")
///                   (set (DL_Att*)GR_ObjTab[child].sChd   = 1)
/// hide parent-obj   (set (DL_Att*)GR_ObjTab[parent].sPar  = 1)
///                   (set (DL_Att*)GR_ObjTab[parent].unvis = 1)
///   child  = GL_GetActInd ();
///   parent = 
///
/// Input:
///   cldTyp,cldDbi      typ/dbi of child  (output-obj)
///   iPar[0],iPar[1]    Typ_cmdNCsub / T_CUT|T_MIR|..
///   iPar[2],iPar[3]    typ/dbi of parent (basic-obj; first obj of "CUT"
///
/// \endcode


#define oaSIZ 10

  int     i1, ii, fnc, parTyp;
  long    parDbi, dli_par, dli_chd;
  ObjDB   parTab[oaSIZ];



  printf("APT_work_parent_hide cldTyp=%d cldDbi=%ld\n",cldTyp,cldDbi);


  if(APT_obj_stat == 1) goto L_done;       // mode temporary: do nothing ..

  if(iPar[0] != Typ_cmdNCsub) goto L_done;


  fnc = iPar[1];
  parTyp = iPar[2];  // parent
  parDbi = iPar[3];  // parent

    // printf("   _work_parent_hide parTyp=%d parDbi=%ld\n",parTyp,parDbi);
    // printf("  fnc=%d cldTyp=%d cldDbi=%ld\n",fnc,cldTyp,cldDbi);


  // //----------------------------------------------------------------
  // // mode temporary: unhilite parent-obj.
  // if(APT_obj_stat == 1) {   // 1=temp
    // // CUT: unhilite parent-obj.                             // 2013-03-15
    // if((iPar[0] == Typ_cmdNCsub) && (iPar[1] == T_CUT)){
      // // find dli of parent
      // dli = DL_find_obj (parTyp, parDbi, -1L);
      // if(dli >= 0) DL_hili_off (dli);
    // }
    // goto L_done;   // temp-Mode: done ..
  // }



  //----------------------------------------------------------------
  // store childInfo, parentInfo in DL and ParTab; hide parent.
  // Obj's from "TRA","PRJ","MIR" "REV" have parents;
  // Obj's from "CUT" have parents (in bin. struct); do not hide.

  // ignore these funcs 
  if((fnc != T_CUT) &&
     (fnc != T_CCV) &&
     (fnc != T_PRJ) &&
     (fnc != T_REV) &&
     (fnc != T_TRA) &&
     (fnc != T_MIR))   goto L_done;


  // get the dispListIndex of child = active DL-Index (AP_dli_act ?)
  dli_chd = GL_GetActInd ();


  if(fnc == T_CCV) {
    // set DL-flags; do not hide; do nor create PartTab-Record.
    // get all parents of active contour; set DL.sPar u. sChd
    ii = 0;
    OPAR_get_src (&ii, parTab, oaSIZ, cldTyp, cldDbi);
    for(i1=0; i1<ii; ++i1) {   // loop tru parent-obj's
      // get the dispListIndex of parent
      dli_par = DL_find_obj (parTab[i1].typ, parTab[i1].dbInd, dli_chd - 1);
      // set dispList-flag sChd and sPar
      DL_parent_set (dli_par, 1);
      DL_child_set (dli_chd, 1);
    }
    goto L_done;
  }


  // if (child == parent): do nothing (S20 = CUT S20 ..)
  if((parTyp == cldTyp)&&(parDbi == cldDbi)) goto L_done;


  // get the dispListIndex of parent
  dli_par = DL_find_obj (parTyp, parDbi, dli_chd - 1);
  if(dli_par < 0) return 0;   // no parent ..


  // set dispList-flag sChd and sPar for child & parent
  DL_parent_set (dli_par, 1);
  DL_child_set (dli_chd, 1);


  // hide parent.
  // but not contour-obj's
  if(APT_hide_parent) {   // See APT_tra_obj
    if(fnc != T_CCV)
      DL_parent_hide (dli_par);
  }


  // create the PartTab-Record (store childInfo, parentInfo)
  // but not for "CUT" records
  if(fnc != T_CUT)
    OPAR_set (parTyp, parDbi, cldTyp, cldDbi);


  //----------------------------------------------------------------
//  wurde nach IE_cad_test__ verlegt ...
  // // is obj in DB already defined ?
  // i1 = DB_QueryDef (basTyp, (long)cldDbi);
// 
  // if(i1 >= 0) {   // ja, existiert schon ...
    // // Obj in DL suchen
    // dli = DL_find_obj (basTyp, (long)cldDbi);
    // // overwrite DL-Record dli !
    // if(dli >= 0) {
      // // printf(" overwrite dli=%d\n",dli);
      // DL_SetInd (dli);
      // // Overwrite the APT-LinNr (sonst kein find -> hilite moeglich)
      // DL_SetLnr (dli, APT_line_act);
    // }
  // }



  L_done:

  return 0;

}
*/


//================================================================
  int APT_Draw__ (int iAtt, int typ, long ind) {
//================================================================
/// \code
/// obj ex DB laden (DB_GetXX), ausgeben via APT_DrawXX
/// create/modify obj: see DL_SetInd
/// is using memspc201
/// Input:
///   ind    dbi
/// \endcode

  int       irc, i1;
  long      l1, dli;
  double    d1;
  char      *s1;
  void      *po;
  Point     pt1;
  ObjGX     *ox1;
  // AText     *atx1;
  // GText     *gtx1;
  Memspc    tSpc1 = UME_NEW;


  // printf("APT_Draw__ typ=%d ind=%ld iAtt=%d\n",typ,ind,iAtt);
  // UT3D_stru_dump (Typ_Ltyp, &iAtt, " Ind_Att_ln:");
  // printf("  APT_obj_stat=%d UP_level=%d\n",APT_obj_stat,UP_level);
  // printf("  WC_mode=%d ED_query_mode=%d\n",WC_mode,ED_query_mode());
  // if(typ==Typ_SUR) printf("  S:vtex=%d\n",((ColRGB*)&iAtt)->vtex);

  // UI_GR_DrawInit ();  2010-06-16 raus !

  switch (typ) {



    case Typ_PT:
        po = DB_get_PT (ind);
          // UT3D_stru_dump (Typ_PT, (Point*)po, " PT-po: ");

        if(APT_obj_stat == 1) {  // 1=temp                 // 2013-03-16
          // get the temp-dispListindex for the active CAD-inputfield
          l1 = IE_get_inp_dli();
          // if CAD is active: overwrite symbol for the active inputfield
          if(l1) DL_SetInd (l1);
          APT_DrawSymB (iAtt, ind, SYM_CIR_S, ATT_COL_RED, (Point*)po);

        } else {
          // 0=permanent
          APT_DrawPoint (iAtt, ind, (Point*)po);
        }
      break;




    case Typ_LN:
        po = DB_get_LN (ind);
        //TX_Print(" aktive zsur=%f",WC_sur_Z);
/*      if(APT_3d_mode == OFF) {
          if((ln1.p1.z == 0.0)   &&   //2001-09-19
             (ln1.p2.z == 0.0)) {
            ln1.p1.z = WC_sur_Z;
            ln1.p2.z = WC_sur_Z;
          }
        }   */
        // if(UP_level < 0) {   // in UP-s nix darstellen
          APT_DrawLine (iAtt, ind, (Line*)po);
        // }
      break;



    case Typ_CI:
        po = DB_get_CI (ind);
/*      if(APT_3d_mode == OFF) {
          if((ci1.pc.z == 0.0)   &&   //2001-09-19
             (ci1.p1.z == 0.0)   &&
             (ci1.p2.z == 0.0)) {
            ci1.p1.z = WC_sur_Z;
            ci1.p2.z = WC_sur_Z;
            ci1.pc.z = WC_sur_Z;
          }
        }   */
        // if(UP_level < 0) {   // in UP-s nix darstellen
          APT_DrawCirc (iAtt, ind, (Circ*)po);
        // }
      break;



    case Typ_CV:
    case Typ_CVLNA:
    case Typ_CVPSP3:
    case Typ_CVPOL:
    case Typ_CVPOL2:
    case Typ_CVBSP:
    case Typ_CVRBSP:
    case Typ_CVELL:
    case Typ_CVCLOT:
    case Typ_CVTRM:
        po = DB_GetCurv (ind);
        // UME_init (&tSpc1, memspc201, sizeof(memspc201));
        APT_DrawCurv (iAtt, ind, (ObjGX*)po, WC_sur_Z, &tSpc1);
      break;



    case Typ_Note:           // N - Default
    case Typ_GTXT:
    case Typ_Tag:
    case Typ_Dimen:
        // gtx1 = DB_GetGTxt (ind);
        ox1 = DB_GetGTxt (ind);
          // printf(" NOTE-typ=%d\n",ox1->typ);
          // UT3D_stru_dump(Typ_ObjGX, ox1, "ex DB_GetGTxt:");

        // if(UP_level >= 0) break;       // in UP-s nix darstellen

        if(ox1->form == Typ_Dimen) {
            // APT_DrawDimen (Typ_Att_def, ind, ox1);  // 2009-07-12
            APT_DrawDimen (iAtt, ind, ox1);

        } else if(ox1->form == Typ_ATXT) {
          // atx1 = ox1->data;
          // draw Tag / Bitmap-Image / LDRS
          // irc = APT_DrawTxtA (Typ_Att_Symb, ind, atx1);
          irc = APT_DrawTxtA (iAtt, ind, ox1->data);
          if(irc < 0) goto Error;

        } else if(ox1->form == Typ_Dim3) {
          // draw 3D-Dim
          // irc = APT_DrawDim3 (Typ_Att_Symb, ind, ox1);
          irc = APT_DrawDim3 (iAtt, ind, ox1);
          if(irc < 0) goto Error;

        // } else if(ox1->typ == Typ_TEXB) {
          // // load Texture
          // irc = APT_LoadTex (ind, ox1);
          // if(irc < 0) goto Error;

        } else {
          // gtx1 = ox1->data;
          // if(gtx1->size >= 0.) {
            // APT_DrawTxtG (0, &gtx1.pt, gtx1.size, gtx1.dir, gtx1.txt);
            // APT_DrawTxtG (Typ_Att_def, ind, gtx1);   // 2009-07-12
            // APT_DrawTxtG (iAtt, ind, gtx1);
            APT_DrawTxtG (iAtt, ind, ox1->data);
          // }
        }
      break;



    case Typ_PLN:
      // printf(" Typ_PLN: Ind %d\n",ind);
      // NC_setRefsys (ind);      // gleich aktivieren; raus 31.8.2002

      //if(UP_level < 0) {   // in UP-s nix darstellen
          po = DB_get_PLN (ind);
          if(APT_obj_stat != 0) {       // 0=perm 1=temp
            // GR_Disp_axis ((Plane*)po, 9, 5);
            dli = DL_StoreObj (Typ_PLN, 0L, 0);
            GL_DrawSymVX (&dli, 9, (Plane*)po, 5, 1.);  // hili
          } else {
            APT_DrawPln (1, ind, (Plane*)po);     // 5=att=col.
          }
      //}

      break;



    case Typ_SUR:
    case Typ_SURPTAB:
    case Typ_SURMSH:
        // if(UP_level < 0) {   // in UP-s nix darstellen
          // APT_DrawSur (AP_indCol, ind);  // 5=att=col.
          APT_DrawSur (iAtt, ind);  // 5=att=col.
        // }
      break;


    case Typ_SOL:
        // if(UP_level < 0) {   // in UP-s nix darstellen
          // APT_DrawSol (AP_indCol, ind);  // 5=att=col.
          APT_DrawSol (iAtt, ind);  // 5=att=col.
        // }
      break;



    case Typ_VC:
        printf("DRAW VC %d %d\n",ED_query_mode(),APT_obj_stat);
      // if(ED_query_mode() != ED_mode_enter) break;
      if(ED_query_mode() == ED_mode_go) break;
      // if(APT_obj_stat != 1) break;  // 2013-09-30
      // if(WC_mode != 0) break;
      // if(UP_level < 0) {   // in UP-s nix darstellen

      UI_disp_vec1 (Typ_Index, PTR_LONG(ind), NULL);

      // UI_GR_get_selNam  (&i1, &l1, &s1);
      // if(i1 != 0) sele_get_pos (&pt1);   // GR_selTyp
      // else pt1 = GL_GetCen();
      // l1 = ind;
      // UI_disp_vec1 (Typ_Index, (void*)l1, &pt1);
      break;




    case Typ_Model:
    case Typ_Mock:
      po = DB_get_ModRef (ind);
      // APT_DrawModel (Typ_Att_Fac, ind, mod1);  // Typ_Att_Fac1 ?
      APT_DrawModel (((ModelRef*)po)->modNr, ind, (ModelRef*)po);
      break;



    case Typ_Tra:
      // ox1 = DB_GetTra (ind);
      // printf(" ED_mode=%d\n",ED_query_mode());
      // nur im Edit-Mode (nicht im Run-Mode)
      // if(ED_query_mode() == ED_mode_go) break;
      if(ED_query_mode() != ED_mode_enter) break;
      if(APT_obj_stat != 1) break;

      // UI_disp_vec2(ind); // wurde in APT_decode_tra beladen
      UI_disp_tra (DB_GetTra (ind));
      break;



    case Typ_VAR:                          // nicht sichtbare
          // if((ED_query_mode() == ED_mode_enter) ||
          if((AP_src != AP_SRC_MEM) ||
             (APT_obj_stat    == 1))                      {
            d1 = DB_GetVar (ind);      // ind ist oft 0 !!
            TX_Print ("VAR[%d] = %f",ind,d1);
          }
      break;




    case Typ_Txt:
    case Typ_G_Att:
    case Typ_Tool:
    case Typ_Activ:     // unsichtbar ..
    case Typ_Joint:     // unsichtbar ..
      break;



    default:
      TX_Error(" Definition; Typ %d unbekannt ****",typ);
      goto Error;

  }


  Fertig:
  // TX_Print("ex APT_Draw__");

  // UI_GR_DrawExit ();  2010-06-16 raus !

  return typ;


  Error:
    typ = -1;
    goto Fertig;


}


//================================================================
  int APT_eval_if (int *aus_typ, double *aus_tab) {
//================================================================
/// evaluate expression;
// Input:
//   austyp    3 types; TypVal, Typ_ope__, TypVal
//   austab             value,  operator,  value 
// Output:
//     -1       Error.
//     0        skip command; continue (expr-result is no)
//     1        do command (eg "V1=10"), continue


  int   iOpe, rc;

  // printf("APT_eval_if %f,%d,%f\n",aus_tab[0],aus_typ[1],aus_tab[2]);

  
  if(aus_typ[0] != Typ_Val)   { rc=0; goto L_e1;}
  if(aus_typ[1] != Typ_ope__) { rc=1; goto L_e1;}
  if(aus_typ[2] != Typ_Val)   { rc=2; goto L_e1;}

  iOpe = aus_tab[1];
  rc = 1;

  if(iOpe == Typ_ope_eq) {
    if(!(UTP_comp2db (aus_tab[0],aus_tab[2], UT_TOL_pt))) {
      rc = 0;
    }

  } else if(iOpe == Typ_ope_ne) {
    if(UTP_comp2db (aus_tab[0],aus_tab[2], UT_TOL_pt)) {
      rc = 0;      }

  } else if(iOpe == Typ_ope_lt) {
    if(aus_tab[0] >= aus_tab[2]) {
      rc = 0;
    }

  } else if(iOpe == Typ_ope_gt) {
    if(aus_tab[0] <= aus_tab[2]) {
      rc = 0;
    }

  } else if(iOpe == Typ_ope_ge) {
    if(aus_tab[0] < aus_tab[2]) {
      rc = 0;
    }

  } else if(iOpe == Typ_ope_le) {
    if(aus_tab[0] > aus_tab[2]) {
      rc = 0;
    }

  } else {
    rc = 9;
    goto L_e1;
  }

  return rc;

  L_e1:
    TX_Error(" APT_eval_if - E-%d ",rc);
    return -1;

}
 

//================================================================
  int APT_work_TPC_IF (char** data) {
//================================================================
// see also PRG_eval_expr
 
  int       i1, irc;
  char      *p1, *p2;
  ObjAto    ato1;


  // printf("APT_work_TPC_IF |%s|\n",*data);


  // search ';' - terminate expression
  p1 = *data;
  p2 = strchr (p1, ';');
  if(p2 == NULL) {
    TX_Error("cannot find ; in IF-Exprssion");
    return -1;
  }
  *p2 = '\0';
  ++p2;
  UTX_pos_skipLeadBlk(p2); // p2 is the following command now
  *data = p2;
    // printf(" IF-cond:|%s|\n", p1);
    // printf(" IF-cmd: |%s|\n", *data);



  //----------------------------------------------------------------
  // get memSpc for ato
  // ATO_getSpc__ (&ato1);
  ATO_getSpc_tmp__ (&ato1, 6);

  // decode data
  ATO_ato_srcLn__ (&ato1, p1);
    // ATO_dump__ (&ato1, " start _AppCodTab");

  // printf("IF (%f,%d,%f)\n",aus_tab[0],aus_typ[1],aus_tab[2]);
  return APT_eval_if (ato1.typ, ato1.val);

}


//================================================================
  int APT_work_PrgCodTab (int icod, char** data) {
//================================================================
// retcod: PrgMod_normal - PrgMod_continue_mac
    
  int       i1, irc;


  // printf("APT_work_PrgCodTab %d |%s|\n",icod,PrgCodTab[icod]);
  // if(data)printf("  |%s|\n",*data);


  // codes without values. Use direct.
  irc = PrgMod_normal;  //0;
  switch (icod) {


    //----------------------------------------------------------------
    case TPC_JUMP:
    // im Editmode nix tun; sonst ja.
    if(ED_query_mode() == ED_mode_enter) goto Fertig;

    // Jump-Label merken
    strcpy (APT_label, *data);

    // im primary Model (ex Memory) reset Linenr to StartOfModel
    // in subProg (ex File) muesste ein Rewind machen

    // ist subModel (File) oder ein primary-Model aktiv ?
    // if(APT_mac_fil == 0) ED_file__ (3, NULL); // rewindActiveFile

    // Jump erzwingen.
    irc = PrgMod_skip_until_label;
    goto Fertig;



    //----------------------------------------------------------------
    case TPC_MAC:
    // im Editmode und im STEP-Mode schon was tun
    i1 = ED_query_mode();
    if((i1 == ED_mode_enter)||(i1 == ED_mode_step)) goto Fertig;

    irc = PrgMod_skip_until_macend;
    UP_act_typ = 'M';

    goto Fertig;



    //----------------------------------------------------------------
    case TPC_GEO:
    // im Editmode und im STEP-Mode schon was tun
    i1 = ED_query_mode();
    if((i1 == ED_mode_enter)||(i1 == ED_mode_step)) goto Fertig;

    irc = PrgMod_skip_until_macend;
    UP_act_typ = 'G';

    goto Fertig;



    //----------------------------------------------------------------
    case TPC_END:
    // im Editmode einfach uebergehen
    // printf(" End - mode=%d",ED_query_mode());

    if(ED_query_mode() == ED_mode_enter) goto Fertig;



    // hierher kommt nur der CALL/WORK - Mode !


    // MAC oder GEO unterscheiden  (G od M)
    if(UP_act_typ == 'G') goto Do_END_GEO;



    //------- END MAC ---------
      // printf("END MAC Lev=%d typ=%c\n",UP_level,UP_act_typ);
      // das close Inputfile hat schon ED_Read_Line gemacht !
    irc = PrgMod_normal;

    // Bei hoeheren Levels nix tun
    if(UP_level >= 0) {

      irc = APT_UP_down ();  // restore Prg_line_nr, UP_act_typ, decr. UP_level

      if(UP_level >= 0) { goto Fertig; }
    }

      // printf(" lNr nach MAC: %d\n",Prg_line_nr);
    // explizit einen actPos-Record fuer das CALL speichern
    i1 = APT_line_act;
    APT_line_act = Prg_line_nr - 1;
    // WC_actPos_save (1);                // actPosU merken
    APT_line_act = i1;

    UP_mode = OFF;     // Rueckkehr ins Main.
    ED_lnr_mac (1);    // reset ED_lnr_bis after execute MAC
    // rc = PrgMod_continue_mac;
    goto Fertig;


    //----------------------------------------------------------------
    case TPC_IF:
      return APT_work_TPC_IF (data); 


    //----------------------------------------------------------------
    case TPC_CALL:
      return APT_work_TPC_CALL (data);


    //----------------------------------------------------------------
    default:
      TX_Error("APT_work_PrgCodTab E000_%d",icod);
  }




    //====================================================================
    Do_END_GEO:
      // printf("END GEO Lev=%d typ=%c\n",UP_level,UP_act_typ);
      // printf("    WC_mode=%d\n",WC_mode);
      // printf("    UpOri=%f,%f\n",UpOri.x,UpOri.y);


    irc = PrgMod_normal;

    UP_resolv = OFF;  // damit APT_transl2 transliert 2002-03-07
    actPosUtr = APT_transl2 (&actPosU); // 2001-11-02
    actPosOtr = APT_transl2 (&actPosO);
    actPosU = actPosUtr;                // 2001-11-02
    // hier fuer PP auch noch actPosO ??


    // Bei h�heren Levels nix tun
    if(UP_level >= 0) {
      irc = APT_UP_down ();  // restore Prg_line_nr, UP_act_typ, decr. UP_level
      if(UP_level > 0) { goto Fertig; }   // raus 2002-03-07
    }

    // Rueckkehrin ins Main.
    UP_mode = OFF;


    UpOri.x  = 0.0;
    UpOri.y  = 0.0;
    UpOriAng = 0.0;

/*
    TX_Print("EEEEEEEEEEEEE END_GEO UpOri=%f,%f\n",UpOri.x,UpOri.y);
    TX_Print("     actPosU=%f,%f\n",actPosU.x,actPosU.y);
    TX_Print("     actPosUtr=%f,%f\n",actPosUtr.x,actPosUtr.y);
*/

    // actPos wieder zuruecksetzen
    actPosU.x = actPosUtr.x;
    actPosU.y = actPosUtr.y;

    actPosO.x = actPosOtr.x;
    actPosO.y = actPosOtr.y;

    goto Fertig;


  //====================================================================
  Fertig:

  return irc;


  //----------------------------------------------------------------
  Fehler1:
    TX_Error(" bei den Parametern: %s\n",PrgCodTab[icod]);
    printf("*** Err APT_work_PrgCodTab |%s|\n",*data);


}


//================================================================
  int APT_work_AppCodTab (char *cmd, char** data) {
//================================================================
/// \code
/// work commands HIDE VIEW MODSIZ DEFTX EXECM .. (AppCodTab)
///  retCod:  >=0  cmd in AppCodTab; cmd executed.
///           -1   error executing commend.
///           -2   cmd not in AppCodTab;
/// \endcode


  int       i1, otyp, icod;
  long      ind;
  char      auxBuf[256], *p1;
  Point     pt1;
  ObjAto    ato1;


  // printf("APT_work_AppCodTab cmd=|%s|\n",cmd);
  // if(data)printf("  data=|%s|\n",*data);


  // test if its a command (HIDE VIEW MODSIZ DEFTX EXECM .. (AppCodTab))
  icod = UTX_wTab_ck (AppCodTab, cmd);
    // printf("  _AppCodTab-icod=%d\n",icod);
  if(icod < 0) return -2;



  //================================================================
  // codes without values. Use direct.
  switch (icod) {

    //----------------------------------------------------------------
    case TAC_MODE:
      return APT_work_MODE (data);

    //----------------------------------------------------------------
    case TAC_ZOOMALL:
      return UI_GR_ScalAuto (0);

    //----------------------------------------------------------------
    case TAC_LDMOD:
      // load model from file
      return AP_Mod_load_fn (*data, 1);

    //----------------------------------------------------------------
    case TAC_EXECM:
        printf(" EXECM |%s|\n",*data);
      // DLL-starten
      return AP_exec_dll (*data);

    //----------------------------------------------------------------
    case TAC_PROCESS:
      // APT_get_Txt (auxBuf, *data, ato1.val[1]);   // processorname
      // data="<processName> <processor>"; get blank 
      p1 = strchr (*data, ' ');
      if(!p1) goto Fehler1;
      ++p1;
        // printf(" _AppCodTab-PROCESS-auxBuf=|%s|\n",auxBuf);
        // printf(" w1=|%s|\n",APP_act_proc);
      // load & init processor
      return PRC_init (p1);
      // break;
  }



  //================================================================
  // codes with values. First decode values.
  // get memSpc for ato
  ATO_getSpc__ (&ato1);

  // decode data
  ATO_ato_srcLn__ (&ato1, *data);
    // ATO_dump__ (&ato1, " start _AppCodTab");


  switch (icod) {

    //----------------------------------------------------------------
    case TAC_PRI:
      // decode 
      APT_decode_print (memspc011, *data, ato1.typ, ato1.val, ato1.nr);
      TX_Print("%s",memspc011);
      break;

    //----------------------------------------------------------------
    case TAC_DRAW:
      for(i1=0; i1<ato1.nr; ++i1) {
        // printf ("  DRAWWWWWW: %d %d %f\n",i1,ato1.typ[i1],ato1.val[i1]);
        if(ato1.typ[i1] == Typ_Val) {
          ED_set_delay ((int)ato1.val[i1]);

        } else if(ato1.typ[0] == Typ_cmdNCsub) {

          if((int)ato1.val[0] == 0) {               // Achtung: "T_ON"
            APT_Stat_Draw = ON;

          } else if((int)ato1.val[0] == 1) {        // Achtung: "T_OFF"
            APT_Stat_Draw = OFF;
            // printf ("  DRAWWWWWW: OFF\n");

          } else {
            goto Fehler1;
          }

        } else {
          goto Fehler1;
        }
      }
      break;

    //----------------------------------------------------------------
    case TAC_STAT:
      TX_Print("Stat:");
      DL_Stat ();
      DB_Stat ();
      break;

    //----------------------------------------------------------------
    case TAC_SSTYLS:
      return GA_sStylTab (ato1.nr, ato1.typ, ato1.val);

    //----------------------------------------------------------------
    case TAC_SHOW:
      if(WC_modact_ind >= 0) break;       // skip command in Submodels
      return APT_work_SHOW (ato1.nr, ato1.typ, ato1.val);

    //----------------------------------------------------------------
    case TAC_HIDE:
      // used only at initphase; eg "HIDE C20 P20 P24 S21 P25 P26 P22"
      // interactive use SHOW.
        // printf("HIDE |%s|\n",*data);
      GA_hideTab (ato1.nr, ato1.typ, ato1.val);
      // GA_hide__ (7, 0, 0); // Display Info Hidden about Objects
      // DL_Redraw ();
      break;

    //----------------------------------------------------------------
    case TAC_VIEW:
      if(WC_modact_ind >= 0) break;       // skip command in Submodels
      return APT_work_VIEW (ato1.nr, ato1.typ, ato1.val);

    //----------------------------------------------------------------
    case TAC_CONST_PL:
      pt1           = DB_GetPoint  ((long)ato1.val[0]);
      WC_sur_act.vx = DB_GetVector ((long)ato1.val[1]);
      WC_sur_act.vy = DB_GetVector ((long)ato1.val[2]);
      WC_sur_act.vz = DB_GetVector ((long)ato1.val[3]);
      WC_sur_Z = ato1.val[4];
      // ConstPlID ist R20 (Typ_PLN) oder VC (DZ od DY od DZ)
      if((ato1.typ[5] == Typ_PLN)||(ato1.typ[5] == Typ_VC)) {
        APED_oid_dbo__ (auxBuf, ato1.typ[5],(long)ato1.val[5]);
        AP_Set_ConstPl_Z (auxBuf);
      }
        // UT3D_stru_dump(Typ_VC, &WC_sur_act.vx, "VX=");
        // UT3D_stru_dump(Typ_VC, &WC_sur_act.vy, "VY=");
        // UT3D_stru_dump(Typ_VC, &WC_sur_act.vz, "VZ=");
      // das plane.p setzen !
      UT3D_pl_ptpl (&WC_sur_act, &pt1);
      // GL_constr_pln setzen aus WC_sur_act, WC_sur_Z;
      GL_SetConstrPln ();
      break;

    //----------------------------------------------------------------
    case TAC_MODBOX:
      if((ato1.typ[0] != Typ_PT)||(ato1.typ[1] != Typ_PT)) goto Fehler1;
      AP_box_pm1 = DB_GetPoint ((long)ato1.val[0]);
      AP_box_pm2 = DB_GetPoint ((long)ato1.val[1]);
      AP_mdlbox_invalid_reset ();
      break;

    //----------------------------------------------------------------
    case TAC_MODVAL:
      // get value-nr (0=first_value in text)
      if(ato1.nr == 2) i1 = ato1.val[1];
      else             i1 = 0;
      // get typ and dbi from *data
      APED_dbo_oid (&otyp, &ind, *data);              // get typ,dbi from ID
      SRCU_win_var (otyp, ind, i1);
      break;

    //----------------------------------------------------------------
    case TAC_MODSIZ:
      NC_setModSiz (ato1.val[0]);
    
      // APT_ModSiz  = ato1.val[0];
      // WC_Init_Tol ();
      // if(WC_modact_ind < 0)                   // nur im aktiven Model
        // GL_InitModelSize (APT_ModSiz, 0);

      if(ato1.typ[1] == Typ_Val) {
        UT_TOL_cv = ato1.val[1];
        if(ato1.typ[2] == Typ_Val) {
          UT_DISP_cv = ato1.val[2];
        }
      }
      // printf(" >>>>>>>>>>> MODSIZ %f %f %f\n",APT_ModSiz,UT_TOL_cv,UT_DISP_cv);
      break;

    //----------------------------------------------------------------
    case TAC_DEFTX:
      // printf(" DEFTX %f %f ato1.nr=%d\n",ato1.val[0],ato1.val[1],ato1.nr);
      AP_txsiz    = ato1.val[0];
      AP_txdimsiz = ato1.val[1];
      GR_tx_scale = 1.; 
      // AP_txNkNr   = 2; 
      AP_txNkNr = IMAX(0, 3 - UTP_dbqsiz(APT_ModSiz));
      if(ato1.nr > 2) {
        GR_tx_scale = ato1.val[2];
        if(ato1.nr > 3) {
          AP_txNkNr = ato1.val[3];
        }
      }
      GR_InitGFPar (AP_txdimsiz);
      break;

    //----------------------------------------------------------------
    case TAC_DEFCOL:
      // printf(" DEFCOL %f %f %f\n",ato1.val[0],ato1.val[1],ato1.val[2]);
      // defCol wird hier zwar als neuer DL-Record angelegt; in subModels wird
      // die DL aber geloescht, defCol muss dann mit GL_DefColSet nochmal
      // angelegt werden !
      AP_defcol.cr = (unsigned char)ato1.val[0];
      AP_defcol.cg = (unsigned char)ato1.val[1];
      AP_defcol.cb = (unsigned char)ato1.val[2];
      // AP_indCol = GL_DefColSet (&AP_defcol);
      APcol_actColTra (&AP_defcol); // set AP_actcol 2017-04-21
      GL_DefColSet (&AP_defcol);    // set GL_defCol
      break;

    //----------------------------------------------------------------
    case TAC_ATTL:
      return APT_work_ATTL (ato1.nr, ato1.typ, ato1.val);

    //----------------------------------------------------------------
    case TAC_ATTS:
      return APT_work_ATTS (ato1.nr, ato1.typ, ato1.val);

    //----------------------------------------------------------------
    case TAC_DIM:
      GR_Att_act = Typ_Att_dim;
      break;

    //----------------------------------------------------------------
    case TAC_HILI:
      GR_Att_act = Typ_Att_hili1;
      break;

    //----------------------------------------------------------------
    case TAC_LAY:
      return APT_work_LAY (ato1.nr, ato1.typ, ato1.val, data);

    //----------------------------------------------------------------
    case TAC_DUMP:
      if(ato1.typ[0] == Typ_String) {    // APT_get_String
        APT_get_String (auxBuf, *data, ato1.val[0]);

        if(!strcmp (auxBuf, "GA")) {
          GA_dump__ (NULL);

        } else if(!strcmp (auxBuf, "DL")) {
          DL_DumpObjTab ();

        } else if(!strcmp (auxBuf, "AT")) {
          GL_AttTab_dump__ ();

        } else if(!strcmp (auxBuf, "TX")) {
          Tex_dump__ (NULL);

        } else if(!strcmp (auxBuf, "SD")) {
          Mod_sym_dump ();

        } else goto Fehler1;
      } else goto Fehler1;
      break;


    //----------------------------------------------------------------
    case TAC_NOP:
      if(ato1.typ[0] == Typ_cmdNCsub) {

        if((int)ato1.val[0] == 0) {               // Achtung: "T_ON"
          GR_pick = ON;

        } else if((int)ato1.val[0] == 1) {        // Achtung: "T_OFF"
          GR_pick = OFF;

        } else {
          goto Fehler1;
        }

      } else {
        goto Fehler1;
      }
      break;


    //----------------------------------------------------------------
    default:
      TX_Error("APT_work_AppCodTab E000_%d",icod);
  }


  return 0;

  Fehler1:
   sprintf(auxBuf,"APT_work_AppCodTab E001 |%s|",cmd);
   if(data) strcat(auxBuf, *data);
   TX_Error(auxBuf);
   return -1;

}

 //====================================================================
  int APT_work_LAY (int aus_anz,int aus_typ[],double aus_tab[],
                     char **data) {
//====================================================================

  int    i1, i2, i3;
  char   *cp, auxBuf[128];




    if(aus_typ[0] == Typ_Txt) {

      cp = APT_get_Txt (auxBuf, *data, aus_tab[0]);
      UTX_cp_word_2_upper (auxBuf, auxBuf);
      //TX_Print(" auxBuf=/%s/ %d",auxBuf,aus_typ[2]);

      if(!strcmp (auxBuf, "ALL")) {
        i1 = -1;
      } else {
        goto Fehler1;
      }

    } else {
      i1 = (int)aus_tab[0];
    }

/*
    TX_Print("lay %d %d",aus_anz,i1);
    for(i2=0; i2<aus_anz; ++i2) {
      TX_Print(" %d %d %f",i2,aus_typ[i2],aus_tab[i2]);
    }
*/


    if(aus_anz == 1) {
      GR_lay_act = i1;





    // LAY 12 ON
    } else if(aus_anz == 2) {
      if(aus_typ[1] == Typ_cmdNCsub) {

        if((int)aus_tab[1] == 0) {          // Achtung: "T_ON"
          DL_Lay_mod (i1, FUNC_Show, ON);

        } else if(aus_tab[1] == 1) {        // Achtung: "T_OFF"
          DL_Lay_mod (i1, FUNC_Show, OFF);

        } else {
          goto Fehler1;
        }

      } else {
          goto Fehler1;
      }



    // LAY 12 DIM ON
    } else if(aus_anz == 3) {

      if((aus_typ[1] == Typ_Txt)&&(aus_typ[2] == Typ_cmdNCsub)) {

        cp = APT_get_Txt (auxBuf, *data, aus_tab[1]);
        UTX_cp_word_2_upper (auxBuf, auxBuf);
        //TX_Print(" auxBuf=/%s/ %d",auxBuf,aus_typ[2]);

        if((int)aus_tab[2] == 0) {               // Achtung: "T_ON"
          i3=ON;

        } else if(aus_tab[2] == 1) {             // Achtung: "T_OFF"
          i3=OFF;

        } else {
          goto Fehler1;
        }

        i2=-1;
        if(!strcmp (auxBuf, "NOP")) {
          i2=FUNC_Pick;

        } else if(!strcmp (auxBuf, "DIM")) {
          i2=FUNC_Dim;

        } else if(!strcmp (auxBuf, "HILI")) {
          i2=FUNC_Hili;

        }

        if(i2 > 0) {
          DL_Lay_mod (i1, i2, i3);

        } else {
          goto Fehler1;
        }

      } else {
          goto Fehler1;
      }





    // LAY 12 ADD ....
    } else {
      if(aus_typ[1] == Typ_Txt) {

        cp = APT_get_Txt (auxBuf, *data, aus_tab[1]);
        UTX_cp_word_2_upper (auxBuf, auxBuf);
        //TX_Print(" auxBuf=/%s/ %d",auxBuf,aus_typ[2]);

        if(!strcmp (auxBuf, "ADD")) {
          i2=APT_Lay_add (i1, aus_anz-3, *data, &aus_typ[2], &aus_tab[2]);
          if(i2 < 0) goto Fehler1;

        } else {
          goto Fehler1;
        }
      }
    }

  return 0;


  Fehler1:
   TX_Error("APT_work_LAY E001");
   return -1;
}


//====================================================================
  int APT_work_MODE (char **data) {
//====================================================================

#define WTAB_SIZ 10
  int    i1;
  char   **pa, auxBuf[128];


  // printf("APT_work_MODE |%s|\n",*data);

  // change data to uppercase
  UTX_cp_word_2_upper (*data, *data);


  // get tempSpace for 10 pointers
  UTX_wTab_tmpSpc (pa, WTAB_SIZ);

  // get pointers to words -> pa
  UTX_wTab_str (pa, WTAB_SIZ, *data);
    // while (*pa) { printf(" |%s|\n",*pa); ++pa; }


  UTX_cp_word_2_upper (auxBuf, pa[0]);
    // printf(" auxBuf=|%s|\n",auxBuf);


  //----------------------------------------------------------------
  if(!strcmp (pa[0], "3D")) {
    //TX_Print("start 3D-Input");
    APT_3d_mode = ON;


  //----------------------------------------------------------------
  } else if(!strcmp (pa[0], "2D")) {
    //TX_Print("start 2D-Input");
    APT_3d_mode = OFF;


  //----------------------------------------------------------------
  } else if(!strcmp (pa[0], "MEN")) {
    if(!strcmp (pa[1], "OFF")) UI_men__ ("Men_off");     // remove menu
    else                 UI_men__ ("Men_on");      // restore menu


  //----------------------------------------------------------------
  } else if(!strcmp (auxBuf, "BRW")) {
    if(!strcmp (pa[1], "OFF")) UI_men__ ("Brw_off");
    else                 UI_men__ ("Brw_on");


  //----------------------------------------------------------------
  } else if(!strcmp (auxBuf, "BAR1")) {
    if(!strcmp (pa[1], "OFF")) UI_men__ ("Bar1_off");
    else                 UI_men__ ("Bar1_on");


  //----------------------------------------------------------------
  } else if(!strcmp (auxBuf, "BAR2")) {
    if(!strcmp (pa[1], "OFF")) UI_men__ ("Bar2_off");
    else                 UI_men__ ("Bar2_on");


  //----------------------------------------------------------------
  // Sehnentoleranz fuer Kreise; dzt unused !
  // } else if(!strcmp (pa[0], "DISP_AC")) {
    // APT_get_Val (&APT_TOL_ac, cp, 0.);
    // printf(" change Disp_AC %f",APT_TOL_ac);


  //----------------------------------------------------------------
  // "PlanesDisplay OFF" from APT-code
  } else if(!strcmp (pa[0], "DISP_PL")) {
    if(WC_modact_ind >= 0) goto Fertig; // skip command in Submodels

    if(!strcmp (pa[1], "ON")) {          // view (std)
      APT_dispPL = ON;
      UI_upd_plDisp ();
      DL_disp_PL (0);

    } else if(!strcmp (pa[1], "OFF")) {  // hide
      APT_dispPL = OFF;
      UI_upd_plDisp ();
      DL_disp_PL (1);

    } else {
      goto Fehler1;
    }


  //----------------------------------------------------------------
  } else if(!strcmp (pa[0], "DISP_PT")) {
    if(WC_modact_ind >= 0) goto Fertig; // skip command in Submodels
      // printf(" change Disp_PT |%s|\n",auxBuf);

    if(!strcmp (pa[1], "ON")) {           // view (std)
      APT_dispPT = ON;
      GL_InitPtAtt (0);
      UI_upd_ptDisp ();

    } else if(!strcmp (pa[1], "OFF")) {   // hide
      APT_dispPT = OFF;
      GL_InitPtAtt (1);
      UI_upd_ptDisp ();

    } else {
      // set thickness points
      i1 = atoi(pa[1]);
      GL_InitPtAtt (i1);

    }


  //----------------------------------------------------------------
  } else {
    goto Fehler1;
  }

  //TX_Print(" change APTsw_poly to %d",APTsw_poly);


  Fertig:
  return 0;


  Fehler1:
   TX_Error("APT_work_MODE E001");
   return -1;
}

   
//====================================================================
  int APT_work_TPC_CALL (char **data) {
//====================================================================
/*
Ablauf Makro:
  CALL CTLG    ??

  CALL Kreis1V
    schaltet APT_stat_act auf 4 (PrgMod_skip_until_mac); es werden nun von
    ProgrammBeginn alle Lines geliefert;
  MAC Kreis1V
    schaltet APT_stat_act auf 0 bis END;
  END
    schaltet APT_stat_act auf PrgMod_skip_until_line (od PrgMod_continue_file)
    Es wird Prg_line_nr auf die lNr hinter dem CALL gesetzt
    ProgAblauf beginnt wieder in Zeile 1.
*/

  int       i1, rc;
  char      *p1;
  ModelBas  *mb;
  ObjAto    ato1;


  // printf("APT_work_TPC_CALL |%s|\n",*data);


  // get memSpc for ato
  ATO_getSpc_tmp__ (&ato1, 6); 

  // decode data
  ATO_ato_srcLn__ (&ato1, *data);
    // ATO_dump__ (&ato1, " start _TPC_CALL");


    // im Editmode nix tun, sonst ja
    // if(ED_query_mode() == ED_mode_enter) goto Fertig;



  //----------------------------------------------------------------
   // check for catalog-call (Typ_cmdNCsub T_CTLG)
   if(ato1.typ[0] == Typ_cmdNCsub) {
    if(ato1.val[0] != T_CTLG) goto Fehler1;
        // printf(" catalog-call; WC_modact_ind=%d\n",WC_modact_ind);
      // wenn im primary Model "CALL CTLG" steht darf man nix tun !
      if(WC_modact_ind < 0) goto Fertig;

      mb = DB_get_ModBas (WC_modact_ind);
      if(mb->mnam == NULL) goto Fehler1;
        // printf(" WC_modact_ind=%d mnam=|%s|\n",WC_modact_ind,mb->mnam);
      sprintf(APT_macnam, "%s",mb->mnam);

      // get APT_filnam = filename of file tmp/<mnam>.write
      CTLG_fnWrite_modelnam (APT_filnam, APT_macnam);
        // printf("CALL to File |%s|%s|\n",APT_filnam,APT_macnam);
      // ++UP_level; set rc = PrgMod_skip_until_file;
      // this makes: now execute file <APT_filnam>, then continue normal ..
      rc = APT_UP_up ('F');
      goto L_call_9;
   }


  //----------------------------------------------------------------
  // call "<filename>"
    for (i1=0; i1<ato1.nr; ++i1) {

      // das erste Wort muss der UP-Name sein
      if(i1 == 0) {

        //------------------------------------------------------
        // CALL internal "MAC" ?
        if(ato1.typ[i1] == Typ_Txt) {
          // APT_get_Txt (APT_macnam, *data, ato1.val[i1]);
          strcpy (APT_macnam, *data);

          // printf("CALL internal |%s|\n",APT_macnam);
          // Unterprogrammlevel und Ruecksprungaddresse merken
          rc = APT_UP_up ('P');


        //------------------------------------------------------
        // CALL "<filename>"
        // wenn das erste Zeichen ein " ist, dann ist es ein filename
        } else if(ato1.typ[i1] == Typ_String) {
          // remove starting/ending '"'
          p1 = UTX_CleanBracks (*data, '\"', '\"');    // remove "
            // printf(" p1=|%s|\n",p1);
          // change symDir/filenam into absolute filename
          rc = Mod_fNam_sym (APT_filnam, p1);
          // rc < 0 if p1 has no directory-delimiter ('/')
          if(rc < 0) {
            sprintf(APT_filnam, "%s%s.write", OS_get_tmp_dir(), p1);
          }
          // Unterprogrammlevel und Ruecksprungaddresse merken
          rc = APT_UP_up ('F');
          goto L_call_9;
/*
          // fuer den PP den Filetyp entfernen
          UTX_fnam_s (APT_macnam, APT_filnam);
          UTX_ftyp_cut (APT_macnam);
          // printf("CALL to File |%s|%s|\n",APT_filnam,APT_macnam);
          // // nur zum Test:
          rc = APT_UP_up ('F');
*/
        } else {
          goto Fehler1;
        }
      }
    }

    // nun ist:
    // APT_macnam - upName
    // APT_filnam - fileName (nur bei CALL File)
    // UP_level
    // UP_level_adr  =  APT_line_act + 1;
    // UP_level_typ  =  UP_act_typ
    // UP_level_src  =  'P' (Prog) od 'F' (File)

  L_call_9:
    UP_act_typ = 'M';
    UP_mode = ON;
    ED_lnr_mac (0); // save LineNr; open ED_lnr_bis for execute MAC

    // goto Fertig;

  Fertig:

    printf(" ex_TPC_CALL UP_act_typ=%c\n",UP_act_typ);


  return rc;

  Fehler1:
   TX_Error("APT_work_TPC_CALL E001");
   return -1;
}

 
/*
//================================================================
  int APT_work_PrgCmd (char* cmd, char** data) {
//================================================================
// Input:
//   funcInd       index of function in APT_work_PrgCmd
// Retcode:
//    0            PrgMod_normal
//    1-8          PrgMod_* (skip .. continue ..)


  int       aus_anz, otyp, aus_SIZ, *aus_typ;
  double    *aus_tab;

  int       rc, i1, i2, i3, UP_typ, UP_ind;
  long      ind, dli;
  char      auxBuf[128], *sptr, *cp;
  double    d1, d2;
  void      *vp1;
  Point     pt1;
  Point2    pt21;
  Vector    vc1;
  // Line      ln1;
  NC_up_rec *upAct;
  ModelBas  *mb;
  // int       aus_anz;
  // int       aus_typ[10];
  // double    aus_tab[10];





  rc = 0;
  aus_typ = SRC_ato_typ;
  aus_tab = SRC_ato_tab;
  aus_anz = SRC_ato_anz;
  aus_SIZ = SRC_ato_SIZ;


  // printf("APT_work_PrgCmd |%s|\n",cmd);
  // if(data)printf("  |%s|\n",*data);



  //=====================================================================
  if (!strcmp (cmd, "ZOOMALL")) {
    UI_GR_ScalAuto (0);
    goto Fertig;



  //=====================================================================
  } else if (!strcmp (cmd, "JUMP")) {

    // im Editmode nix tun; sonst ja.
    if(ED_query_mode() == ED_mode_enter) goto Fertig;

    // Jump-Label merken
    strcpy (APT_label, *data);

    // im primary Model (ex Memory) reset Linenr to StartOfModel
    // in subProg (ex File) muesste ein Rewind machen

    // ist subModel (File) oder ein primary-Model aktiv ?
    // if(APT_mac_fil == 0) ED_file__ (3, NULL); // rewindActiveFile
    
    // Jump erzwingen.
    rc = PrgMod_skip_until_label;
    goto Fertig;






  //=====================================================================
  } else if (!strcmp (cmd, "MAC")) {

    // im Editmode und im STEP-Mode schon was tun
    i1 = ED_query_mode();
    if((i1 == ED_mode_enter)||(i1 == ED_mode_step)) goto Fertig;

    rc = PrgMod_skip_until_macend;
    UP_act_typ = 'M';

    goto Fertig;





  //=====================================================================
  } else if (!strcmp (cmd, "GEO")) {

    // im Editmode und im STEP-Mode schon was tun
    i1 = ED_query_mode();
    if((i1 == ED_mode_enter)||(i1 == ED_mode_step)) goto Fertig;

    rc = PrgMod_skip_until_macend;
    UP_act_typ = 'G';

    goto Fertig;





  //=====================================================================
  } else if (!strcmp (cmd, "END")) {

    // im Editmode einfach uebergehen
    // printf(" End - mode=%d",ED_query_mode());

    if(ED_query_mode() == ED_mode_enter) goto Fertig;



    // hierher kommt nur der CALL/WORK - Mode !


    // MAC oder GEO unterscheiden  (G od M)
    if(UP_act_typ == 'G') goto Do_END_GEO;



    //------- END MAC ---------
      // printf("END MAC Lev=%d typ=%c\n",UP_level,UP_act_typ);
      // das close Inputfile hat schon ED_Read_Line gemacht !
    rc = PrgMod_normal;

    // Bei hoeheren Levels nix tun
    if(UP_level >= 0) {

      rc = APT_UP_down ();  // restore Prg_line_nr, UP_act_typ, decr. UP_level

      if(UP_level >= 0) { goto Fertig; }
    }

      // printf(" lNr nach MAC: %d\n",Prg_line_nr);

    // explizit einen actPos-Record fuer das CALL speichern
    i1 = APT_line_act;
    APT_line_act = Prg_line_nr - 1;
    // WC_actPos_save (1);                // actPosU merken
    APT_line_act = i1;

    UP_mode = OFF;     // Rueckkehr ins Main.
    ED_lnr_mac (1);    // reset ED_lnr_bis after execute MAC
    // rc = PrgMod_continue_mac;
    goto Fertig;







    //====================================================================
    Do_END_GEO:
      // printf("END GEO Lev=%d typ=%c\n",UP_level,UP_act_typ);
      // printf("    WC_mode=%d\n",WC_mode);
      // printf("    UpOri=%f,%f\n",UpOri.x,UpOri.y);


    rc = PrgMod_normal;


  // // unerledigte NCCmds raus
  // if(insAnz > 0) {
    // ObjG2 o1;
    // TX_Print("END_GEO: unerledigte NCCmds auf %f,%f",actPosUtr.x,actPosUtr.y);
    // // ++APT_line_cnt;
    // // o1 = NC_ausgAU;
    // // o1.p1 = actPosUtr;
    // // o1.nam = APT_line_cnt;
    // APT_ausg_NCCmd (0);
  // }

    // // PP: wenn UP aufgel�st wird, noch END raus.
    // if (WC_mode != 0) {
      // strcpy (WC_outBuf, "END");
      // APT_PP_Write();
    // }


    UP_resolv = OFF;  // damit APT_transl2 transliert 2002-03-07
    actPosUtr = APT_transl2 (&actPosU); // 2001-11-02
    actPosOtr = APT_transl2 (&actPosO);
    actPosU = actPosUtr;                // 2001-11-02
    // hier fuer PP auch noch actPosO ??


    // Bei h�heren Levels nix tun
    if(UP_level >= 0) {
      rc = APT_UP_down ();  // restore Prg_line_nr, UP_act_typ, decr. UP_level
      if(UP_level > 0) { goto Fertig; }   // raus 2002-03-07
    }


    // // PP: nach UP muss im Main mit G92 zurueckgesetzt werden;
    // // dafuer nun nochmal ein FROM.
    // if (WC_mode != 0) {
      // // printf("  UP_mode=%d  UP_resolv=%d UP_act_typ=%c\n",
           // // UP_mode,UP_resolv,UP_act_typ);
      // strcpy (WC_outBuf,"FROM ");

///
//       // im PP-mode wurde nicht transliert;
//       // um den wirklichen Punkt festzustellen, nun
//       // die actPosU noch translieren
//       // vorm UP-Ende noch die Pos fuer nachfolgendes FROM errechnen
//       ++UP_level;      // damit APT_transl2 transliert
//       UP_resolv = OFF;  // damit APT_transl2 transliert
//       actPosUtr = APT_transl2 (&actPosU);
//       actPosOtr = APT_transl2 (&actPosO);
//       --UP_level;      // damit APT_transl2 transliert
///
        // // printf("PP: FROM nach WORK actPosU=%f,%f\n", actPosU.x, actPosU.y);
        // // printf("    FROM nach WORK %f,%f\n", actPosUtr.x, actPosUtr.y);
// 
      // UTX_add_fl_u2 (WC_outBuf, actPosUtr.x, actPosUtr.y);
      // UP_mode = OFF;  // sonst schreibt der APT_PP_Write nicht
      // APT_PP_Write();
    // }




    // Konik aus
    // APT_bearb_ch_km (KONIK_0);
    TX_Error("APT_bearb_ch_km nicht aktiv");
    //APT_konik  = KONIK_0;


    // Rueckkehrin ins Main.
    UP_mode = OFF;


    UpOri.x  = 0.0;
    UpOri.y  = 0.0;
    UpOriAng = 0.0;


    // TX_Print("EEEEEEEEEEEEE END_GEO UpOri=%f,%f\n",UpOri.x,UpOri.y);
    // TX_Print("     actPosU=%f,%f\n",actPosU.x,actPosU.y);
    // TX_Print("     actPosUtr=%f,%f\n",actPosUtr.x,actPosUtr.y);




    // actPos wieder zuruecksetzen
    actPosU.x = actPosUtr.x;
    actPosU.y = actPosUtr.y;

    actPosO.x = actPosOtr.x;
    actPosO.y = actPosOtr.y;


    //old_APT_ObjU.p1 = actPosU;


    goto Fertig;




  //=====================================================================
  // } else if (!strcmp (cmd, "NEW")) {
     // UI_menCB (NULL, "new");


  //=====================================================================
  } else if (!strcmp (cmd, "EXECM")) {

    printf(" EXECM |%s|\n",*data);

    // DLL-starten
    AP_exec_dll (*data);

    goto Fertig;


  // //=====================================================================
  // } else if (!strcmp (cmd, "INPUT")) {
// 
    // printf(" INPUT |%s|\n",*data);
// 
    // // input form
    // DLG_input (*data);
// 
    // goto Fertig;





  //=====================================================================
  } else if (!strcmp (cmd, "LDMOD")) {

    printf(" LDMOD |%s|\n",*data);

    // changes symbolic_path --> real_path !
    Mod_sym_get1 (AP_mod_dir, *data, 0);
      // printf(" symbolicPath=|%s|\n",AP_mod_dir);

    UTX_fnam_s (AP_mod_fnam, *data);
      // printf(" Filename=|%s|\n",AP_mod_fnam);

    AP_Mod_load__ (1); // load AP_mod_dir/AP_mod_fnam

    goto Fertig;

    
  
  //=====================================================================
  }


  // offen LDMAC
  // sucht im Memory alle MAC's, die im zu ladenden File enthalten sind
  // und loescht diese; dann wird File hintangefuegt.



  //=====================================================================
  // nun die Ausdruecke aus data einlesen, decodieren und merken
  aus_anz = APT_decode_ausdr (aus_typ, aus_tab, aus_SIZ, data);
  if(aus_anz < 0) goto Error;

    // TEST
    // for(i1=0; i1<aus_anz; ++i1)
    // printf(" _PrgCmd %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);
    // END TEST


  sptr = *data;
  APT_defTxt = *data;

  // printf("APT_work_PrgCmd aus_anz=%d sptr=|%s|\n",aus_anz,sptr);
  // for(i1=0; i1<aus_anz; ++i1) {
    // printf(" %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);
  // }



  //================================================================
  // for all following commands data is present in aus_typ/aus_tab.
  //================================================================


  //=====================================================================
  if (!strcmp (cmd, "MODSIZ")) {
    if(aus_typ[0] != Typ_Val) goto Fehler1;

    NC_setModSiz (aus_tab[0]);

    // APT_ModSiz  = aus_tab[0];
    // WC_Init_Tol ();
    // if(WC_modact_ind < 0)                   // nur im aktiven Model
      // GL_InitModelSize (APT_ModSiz, 0);

    if(aus_typ[1] == Typ_Val) {
      UT_TOL_cv = aus_tab[1];
      if(aus_typ[2] == Typ_Val) {
        UT_DISP_cv = aus_tab[2];
      }
    }

    // printf(" >>>>>>>>>>> MODSIZ %f %f %f\n",APT_ModSiz,UT_TOL_cv,UT_DISP_cv);

    // goto L_Out_Format1;

    goto Fertig;


  //=====================================================================
  } else if (!strcmp (cmd, "MODVAL")) {
    printf(" MODVAL |%s| %d\n",*data,aus_anz);
    // printf(" aus_typ[0]=%d aus_tab[0]=%lf\n",aus_typ[0],aus_tab[0]);
    // printf(" aus_typ[1]=%d aus_tab[1]=%lf\n",aus_typ[1],aus_tab[1]);

    // get value-nr (0=first_value in text)
    if(aus_anz == 2) i1 = aus_tab[1];
    else             i1 = 0;

    // get typ and dbi from *data
    APED_dbo_oid (&otyp, &ind, *data);              // get typ,dbi from ID
    SRCU_win_var (otyp, ind, i1);
    goto Fertig;



  //=====================================================================
  } else if (!strcmp (cmd, "DEFTX")) {
    // printf(" DEFTX %f %f aus_anz=%d\n",aus_tab[0],aus_tab[1],aus_anz);

    AP_txsiz    = aus_tab[0];
    AP_txdimsiz = aus_tab[1];
    GR_tx_scale = 1.;
    // AP_txNkNr   = 2;
    AP_txNkNr = IMAX(0, 3 - UTP_dbqsiz(APT_ModSiz));
    if(aus_anz > 2) {
      GR_tx_scale = aus_tab[2];
      if(aus_anz > 3) {
        AP_txNkNr = aus_tab[3];
      }
    }

    GR_InitGFPar (AP_txdimsiz);

    goto Fertig;


  //=====================================================================
  } else if (!strcmp (cmd, "DEFCOL")) {
    // printf(" DEFCOL %f %f %f\n",aus_tab[0],aus_tab[1],aus_tab[2]);
    // defCol wird hier zwar als neuer DL-Record angelegt; in subModels wird
    // die DL aber geloescht, defCol muss dann mit GL_DefColSet nochmal
    // angelegt werden !


    AP_defcol.cr = (unsigned char)aus_tab[0];
    AP_defcol.cg = (unsigned char)aus_tab[1];
    AP_defcol.cb = (unsigned char)aus_tab[2];

    // AP_indCol = GL_DefColSet (&AP_defcol);
    GL_DefColSet (&AP_defcol);

    goto Fertig;


  //=====================================================================
  } else if (!strcmp (cmd, "HIDE")) {
    // used only at initphase; eg "HIDE C20 P20 P24 S21 P25 P26 P22"
    // interactive use SHOW.
      // printf("HIDE |%s|\n",*data);
    GA_hideTab (aus_anz, aus_typ, aus_tab);
    // GA_hide__ (7, 0, 0); // Display Info Hidden about Objects
    // DL_Redraw ();

    goto Fertig;



  //=====================================================================
  } else if (!strcmp (cmd, "SHOW")) {
    if(WC_modact_ind >= 0) goto Fertig; // skip command in Submodels
    return APT_work_SHOW (aus_anz, aus_typ, aus_tab);



  //=====================================================================
  } else if (!strcmp (cmd, "VIEW")) {
    if(WC_modact_ind >= 0) goto Fertig; // skip command in Submodels
    return APT_work_VIEW (aus_anz, aus_typ, aus_tab);



  //=====================================================================
  } else if (!strcmp (cmd, "ATTL")) {
    return APT_work_ATTL (aus_anz, aus_typ, aus_tab);



  //=====================================================================
  } else if (!strcmp (cmd, "ATTS")) {
    return APT_work_ATTS (aus_anz, aus_typ, aus_tab);



  //=====================================================================
  } else if (!strcmp (cmd, "SSTYLS")) {
    GA_sStylTab (aus_anz, aus_typ, aus_tab);

    goto Fertig;


  //=====================================================================
  } else if (!strcmp (cmd, "PROCESS")) {
    // APT_get_Txt (auxBuf, sptr, aus_tab[0]);   // processname
      // printf(" prNam=|%s|\n",auxBuf);
    APT_get_Txt (auxBuf, sptr, aus_tab[1]);   // processorname
      // printf(" auxBuf=|%s|\n",auxBuf);
      // printf(" w1=|%s|\n",APP_act_proc);

    // load & init processor
    PRC_init (auxBuf);
    // PRC__ (-1, "INIT__");
    

    goto Fertig;



  //=====================================================================
  } else if (!strcmp (cmd, "CONST_PL")) {    // the default-constructionplane

      // printf("CONST_PL |%s|\n",*data);

    // for(i1=0; i1<aus_anz; ++i1) {
      // printf(" %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);
    // }
      // // restore ConstPln:
      // printf("CONST_PL %d %f\n",aus_typ[0],aus_tab[0]);
      // printf("       X %d %f\n",aus_typ[1],aus_tab[1]);
      // printf("       Y %d %f\n",aus_typ[2],aus_tab[2]);
      // printf("       Z %d %f\n",aus_typ[3],aus_tab[3]);
      // printf("      dz %d %f\n",aus_typ[4],aus_tab[4]);
      // printf("      ID %d %f\n",aus_typ[5],aus_tab[5]);

    pt1           = DB_GetPoint  ((long)aus_tab[0]);
    WC_sur_act.vx = DB_GetVector ((long)aus_tab[1]);
    WC_sur_act.vy = DB_GetVector ((long)aus_tab[2]);
    WC_sur_act.vz = DB_GetVector ((long)aus_tab[3]);
    WC_sur_Z = aus_tab[4];


    // ConstPlID ist R20 (Typ_PLN) oder VC (DZ od DY od DZ)
    if((aus_typ[5] == Typ_PLN)||(aus_typ[5] == Typ_VC)) {
      APED_oid_dbo__ (auxBuf, aus_typ[5],(long)aus_tab[5]);
      AP_Set_ConstPl_Z (auxBuf);
    }

      // UT3D_stru_dump(Typ_VC, &WC_sur_act.vx, "VX=");
      // UT3D_stru_dump(Typ_VC, &WC_sur_act.vy, "VY=");
      // UT3D_stru_dump(Typ_VC, &WC_sur_act.vz, "VZ=");


    // das plane.p setzen !
    UT3D_pl_ptpl (&WC_sur_act, &pt1);

    // GL_constr_pln setzen aus WC_sur_act, WC_sur_Z;
    GL_SetConstrPln ();

    // WC_sur_act setzen
    // UI_AP (UI_FuncSet, UID_ouf_coz, (void*)&WC_sur_Z);

    goto Fertig;



  //================================================================
  } else if (!strcmp (cmd, "IF")) {

    // printf("IF (%f,%d,%f)\n",aus_tab[0],aus_typ[1],aus_tab[2]);
    rc = APT_eval_if (aus_typ, aus_tab);
    if(rc < 0) goto Fehler1;
    goto Fertig;

    // i1 = aus_typ[1];
    // rc = 1;
    // if(i1 == Typ_ope_eq) {
      // if(!(UTP_comp2db (aus_tab[0],aus_tab[2], UT_TOL_pt))) {
        // //printf(" ungleich; skip the rest of he Line\n");
        // rc = 0;
      // }
    // } else if(i1 == Typ_ope_ne) {
      // if(UTP_comp2db (aus_tab[0],aus_tab[2], UT_TOL_pt)) {
        // rc = 0;      }
    // } else if(i1 == Typ_ope_lt) {
      // if(aus_tab[0] >= aus_tab[2]) {
        // rc = 0;
      // }
    // } else if(i1 == Typ_ope_gt) {
      // if(aus_tab[0] <= aus_tab[2]) {
        // rc = 0;
      // }
    // } else if(i1 == Typ_ope_ge) {
      // if(aus_tab[0] < aus_tab[2]) {
        // rc = 0;
      // }
    // } else if(i1 == Typ_ope_le) {
      // if(aus_tab[0] > aus_tab[2]) {
        // rc = 0;
      // }
    // } else {
      // goto Fehler1;
    // }
    // // printf("ex IF rc=%d\n",rc);
    // goto Fertig;



  //=====================================================================
  } else if (!strcmp (cmd, "CALL")) {
// Ablauf Makro:
//   CALL Kreis1V
//     schaltet APT_stat_act auf 4 (PrgMod_skip_until_mac); es werden nun von
//     ProgrammBeginn alle Lines geliefert;
//   MAC Kreis1V
//     schaltet APT_stat_act auf 0 bis END;
//   END
//     schaltet APT_stat_act auf PrgMod_skip_until_line (od PrgMod_continue_file)
//     Es wird Prg_line_nr auf die lNr hinter dem CALL gesetzt
//     ProgAblauf beginnt wieder in Zeile 1.

    // im Editmode nix tun, sonst ja
    // if(ED_query_mode() == ED_mode_enter) goto Fertig;


//    // check for catalog-call (Typ_cmdNCsub T_CTLG)
//    if(aus_typ[0] == Typ_cmdNCsub) {
//     if(aus_tab[0] != T_CTLG) goto Fehler1;
//         // printf(" catalog-call; WC_modact_ind=%d\n",WC_modact_ind);
//       // wenn im primary Model "CALL CTLG" steht darf man nix tun !
//       if(WC_modact_ind < 0) goto Fertig;
// 
//       mb = DB_get_ModBas(WC_modact_ind);
//       if(mb->mnam == NULL) goto Fehler1;
//         // printf(" WC_modact_ind=%d mnam=|%s|\n",WC_modact_ind,mb->mnam);
//       sprintf(APT_macnam, "%s",mb->mnam);
//       // den zugehoerigen filename fuer das .write-File machen
//       CTLG_fnWrite_modelnam (APT_filnam, APT_macnam);
//         // printf("CALL to File |%s|%s|\n",APT_filnam,APT_macnam);
//       rc = APT_UP_up ('F');
//       goto L_call_9;
//    }
// 
// 
//     for (i1=0; i1<aus_anz; ++i1) {
// 
//       // das erste Wort muss der UP-Name sein
//       if(i1 == 0) {
//         if(aus_typ[i1] == Typ_Txt) {
//           APT_get_Txt (APT_macnam, sptr, aus_tab[i1]);
// 
//           // printf("CALL internal |%s|\n",APT_macnam);
//           // Unterprogrammlevel und Ruecksprungaddresse merken
//           rc = APT_UP_up ('P');
// 
// 
//         // wenn das erste Zeichen ein " ist, dann ist es ein filename
//         } else if(aus_typ[i1] == Typ_String) {
//           i1 = APT_str2fnam (APT_filnam, *data);
//           // open exportfile
//           if(i1 == 1) {   // relativ --> tmp/
//             UTX_safeName (APT_filnam, 0); // change '. ', do not change '/'
//             strcat(APT_filnam, ".write");
//           }
// 
//           APT_get_String (APT_filnam, sptr, aus_tab[i1]);
//             printf(" APT_filnam n get_String |%s|\n",APT_filnam);
//           // cp = sptr + (int)aus_tab[i1] + 1;
//           // UTX_cp_word_term (APT_macnam, cp, '.');
// 
//           // wenn Filename nicht mit "/" beginnt, baseverzeichnis voranstellen.
//           // use better APT_str2fnam ...
//           //if(APT_filnam[0] != "/") {
//           if(APT_filnam[0] != '/') {
//             sprintf(WC_outBuf, "%s/%s",OS_get_bas_dir(),APT_filnam);
//             strcpy(APT_filnam, WC_outBuf);
//           }
//
          // fuer den PP den Filetyp entfernen
          UTX_fnam_s (APT_macnam, APT_filnam);
          UTX_ftyp_cut (APT_macnam);

          // printf("CALL to File |%s|%s|\n",APT_filnam,APT_macnam);

          // // nur zum Test:
          // ED_work_file (APT_filnam);

          // Unterprogrammlevel und Ruecksprungaddresse merken
          rc = APT_UP_up ('F');


        } else {
          goto Fehler1;
        }
      }
    }

    // nun ist:
    // APT_macnam - upName
    // APT_filnam - fileName (nur bei CALL File)
    // UP_level
    // UP_level_adr  =  APT_line_act + 1;
    // UP_level_typ  =  UP_act_typ
    // UP_level_src  =  'P' (Prog) od 'F' (File)

    L_call_9:

    UP_act_typ = 'M';
    UP_mode = ON;
    ED_lnr_mac (0); // save LineNr; open ED_lnr_bis for execute MAC

    goto Fertig;




//
  //=====================================================================
  } else if (!strcmp (cmd, "WORK")) {


    // TESTDISP
    for(i1=0; i1<aus_anz; ++i1) {
      printf(" WORK %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);
    }


    // // im Editmode nix tun, sonst ja
    // if(ED_query_mode() == ED_mode_enter) goto Fertig;


    // "WORK P#"
    // if((aus_typ[0] == Typ_PT)||(aus_typ[0] == Typ_TmpPT)) {
    if(aus_typ[0] == Typ_PT) {
      ind = aus_tab[0];
      i1 = APT_wcut_ln1 (&pt21, ind);
      if(i1 < 0 ) return i1;
      NC_set_actPos (&pt21);
      goto Fertig;

    // } else if(aus_typ[0] == Typ_CI) {
      // ind = aus_tab[0];
      // i1 = DB_GetObjDat (&vp1, Typ_CI, ind);
      // if(i1 < 0) goto Fehler1;
      // wcg_spc_malloc (100);
      // i1 = wcg_OutObj (Typ_CI, vp1, Typ_CI, vp1);
      // wcg_spc_free ();
      // if(i1 < 0 ) return i1; 
      // NC_set_actPos (&pt21);
      // goto Fertig;

    }


    UpOriAng = 0.0;
    UpMirX = OFF;
    UpMirY = OFF;


    // das erste Wort muss der Funktionstyp sein (cut/rev/pock)
    // APT_get_Txt (auxBuf, sptr, aus_tab[0]);
    APT_get_Txt (auxBuf, sptr, 0.);
    UTX_cp_word_2_upper (cmd, auxBuf);
      // printf(" cmd=|%s| typ=%d val=%d\n",cmd,aus_typ[0],aus_tab[0]);


    // "WORK PR <dx> <dy> <dz>"        // work linear relativ
    if (!strcmp (cmd, "PR")) {         // see also RPR

      i1 = APT_decode_pt1 (&pt1, 1, aus_typ, aus_tab);
      if(i1 < 0) goto Error;

      pt1.x += actPosU.x;
      pt1.y += actPosU.y;

      ind = DB_StorePoint (-1L, &pt1);

      i1 = APT_wcut_ln1 (&pt21, ind);
      if(i1 < 0 ) return i1;
      NC_set_actPos (&pt21);
      goto Fertig;
    }



    // 1=CUT, 2=REV, 3=POCK
    if (!strcmp (cmd, "CUT")) {
      UP_workmode = 1;
    } else if (!strcmp (cmd, "REV")) {
      UP_workmode = 2;
    } else if (!strcmp (cmd, "POCK")) {
      UP_workmode = 3;
    } else if (!strcmp (cmd, "SEP_CUT")) {
      UP_workmode = 4;
    } else if (!strcmp (cmd, "SEP_REV")) {
      UP_workmode = 5;

    } else if (!strcmp (cmd, "CUT_CIR")) {
      // WORK CUT_CIR Radius,Startrichtung,Umfahrungswinkel,Ausfahrstrecke
      //              [,Stopwinkel]
      if(aus_typ[1] != Typ_Val) goto Fehler1;
      if(aus_typ[2] != Typ_Val) goto Fehler1;
      if(aus_typ[3] != Typ_Val) goto Fehler1;
      if(aus_typ[4] != Typ_Val) goto Fehler1;
      Pock_CUT_CIRC (&actPosU,aus_tab[1],aus_tab[2],aus_tab[3],aus_tab[4]);
      NC_set_actPos (&actPosU);
        // UT3D_stru_dump (Typ_PT2, &actPosU, "n.CUT_CIRC actPosU:");
      goto Fertig;


    } else if (!strcmp (cmd, "POCK_CIR")) {
      // WORK POCK_CIR Radius_aussen, Radius_Bohrloch <Mittelpunkt>
      if(aus_typ[1] != Typ_Val) goto Fehler1;
      if(aus_typ[2] != Typ_Val) goto Fehler1;
      Pock_POCK_CIRC (&actPosU, aus_tab[1], aus_tab[2]);
      NC_set_actPos (&actPosU);
        // UT3D_stru_dump (Typ_PT2, &actPosU, "n.POCK_CIR actPosU:");
      goto Fertig;;
      
    } else {
      goto Fehler1;
    }

    // i1 = 0;

    L_NxtPar1:
      // ++i1;
    for (i1=1; i1<aus_anz; ++i1) {
      // das zweite Wort muss der UP-Name sein
      if(i1 == 1) {

        printf(" ..aus_typ[%d]=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);

        if(aus_typ[i1] == Typ_PT) {
          UP_typ = Typ_PT;
          UP_ind = aus_tab[i1];

        } else if(aus_typ[i1] == Typ_LN) {
          UP_typ = Typ_LN;
          UP_ind = aus_tab[i1];

        } else if(aus_typ[i1] == Typ_CI) {
          UP_typ = Typ_CI;
          UP_ind = aus_tab[i1];

        } else if(aus_typ[i1] == Typ_CV) {
          UP_typ = Typ_CV;
          UP_ind = aus_tab[i1];

        } else if(aus_typ[i1] == Typ_SUR) {
          UP_typ = Typ_SUR;
          UP_ind = aus_tab[i1];

        } else if(aus_typ[i1] == Typ_Txt) {
          APT_get_Txt (APT_macnam, sptr, aus_tab[i1]);
          // printf("WORK normal |%s|\n",APT_macnam);

          // Unterprogrammlevel und Ruecksprungaddresse merken
          rc = APT_UP_up ('P');

        // wenn das erste Zeichen ein " ist, dann ist es ein filename
        } else if(aus_typ[i1] == Typ_String) {
          APT_get_String (APT_filnam, sptr, aus_tab[i1]);
          // fuer den PP den Filetyp entfernen
          cp = sptr + (int)aus_tab[i1] + 1;
          UTX_cp_word_term (APT_macnam, cp, '.');

          rc = APT_UP_up ('F');

        } else {
          goto Fehler1;
        }

      //  noch nach Winkel / Mirrorflags suchen
      } else {
      L_NxtPar2:
        if((i1 > 1)&&(UP_workmode == 3)) {      // KonturStartpunkt von POCK 
          // printf(" pockPt %d %f\n",aus_typ[i1],aus_tab[i1]);
          if(aus_typ[i1] == Typ_PT) {
            pt1 = DB_GetPoint ((long)aus_tab[i1]);
          } else goto Fehler1;
          // ++i1; if(i1<aus_anz) goto L_NxtPar2;

        } else if((aus_typ[i1] == Typ_Val)||(aus_typ[i1] == Typ_Angle)) {
          UpOriAng = aus_tab[i1];
          UpOriAng_sin = sin(UT_RADIANS(UpOriAng));
          UpOriAng_cos = cos(UT_RADIANS(UpOriAng));
          // ++i1; if(i1<aus_anz) goto L_NxtPar2;

        } else if(aus_typ[i1] == Typ_Txt) {
          APT_get_Txt (auxBuf, sptr, aus_tab[i1]);
          UTX_cp_word_2_upper (cmd, auxBuf);
          if (!strcmp (cmd, "XMIR")) {
            UpMirX = ON;
          } else if (!strcmp (cmd, "YMIR")) {
             UpMirY = ON;
          } else {
            goto Fehler1;
          }
          // ++i1; if(i1<aus_anz) goto L_NxtPar2;

        } else {
          goto Fehler1;
       }
      }
    }

    // printf("WORK %d A%d\n",UP_workmode,(int)aus_tab[i1]);
    // printf("     |%s|%s|\n",APT_macnam,APT_filnam);
    // printf("     actPosU %f %f\n",actPosU.x,actPosU.y);
    // printf("     ang=%f mirx=%d miry=%d\n",UpOriAng,UpMirX,UpMirY);

//---------------
//     // check if upNam is in file tmp/geo.lst (ein DE-Konik-Programm m. CON0)
//     UP_typ_DE = NC_up_ckNam (APT_macnam);
// 
//     // 0:Ja.
//     if(UP_typ_DE == 0) {
//       rc = APT_UP_down ();  // restore Prg_line_nr, UP_act_typ, decr. UP_level
// 
//       UP_act_typ = 'G';
// 
// 
//       // das UP setzen und den Endpunkt retournieren
//       if(UP_workmode == 1) {
//         pt21 = NC_up_Work_ (APT_macnam,actPosU,UpOriAng,UpMirX,UpMirY);
//       } else if(UP_workmode == 3) {
//         pt21 = NC_up_Pock_ (APT_macnam,actPosU,UpOriAng,UpMirX,UpMirY);
//       }
//       printf("NC_up_????_ -> %f,%f\n",pt21.x,pt21.y);
// 
// 
// 
//       // weiter am Endpunkt des UP
//       NC_set_actPos (&pt21);
// 
//       UP_act_typ = 'M';
//       goto Fertig;;
//       // goto L_wg_pp;
//     }
//---------------
      // das UP setzen und den Endpunkt retournieren
      if(UP_workmode == 1) {                 // WORK CUT
        i1 = NC_up_Work_ (&pt21,
                          UP_typ,UP_ind,actPosU,UpOriAng,UpMirX,UpMirY);

      } else if(UP_workmode == 3) {          // WORK POCK
        i1 = NC_up_Pock_ (&pt21,
                          UP_typ,UP_ind,&pt1,actPosU,UpOriAng,UpMirX,UpMirY);

      } else if((UP_workmode == 4)||(UP_workmode == 5)) {
        i1 = NC_up_Sep_ (&pt21,
                         UP_typ,UP_ind,actPosU,UpOriAng,UpMirX,UpMirY,
                         UP_workmode);
      }
      // printf("NC_up_????_ -> %f,%f\n",pt21.x,pt21.y);
      if(i1 < 0) return i1;

      // weiter am Endpunkt des UP
      NC_set_actPos (&pt21);

      // if(UP_workmode == 1) goto L_NxtPar1;

    goto Fertig;


    // ============================================================
    // Check for FROM; war noch keines, dann eines in den PP ausgeben.
    APT_PP_FROM ();

    // Da nach dem WORK ein FROM erforderlich, werden all UP's aufgel�st.
    
    // den Origin merken
    UpOri.x = actPosU.x;
    UpOri.y = actPosU.y;
    //TX_Print(" set UpOri %f %f",UpOri.x,UpOri.y);

    // zum Rotieren ist das Center notwendig (UpOri wird um FROM abge�ndert)
    //UpCen.x = actPosU.x;
    //UpCen.y = actPosU.y;
    //TX_Print("set UpCen to %f %f\n",UpCen.x,UpCen.y);

    // das From merken (zum Drehpunkt und Spiegelcenter berechnen)
    UpFrom.x = 0.0;
    UpFrom.y = 0.0;

    // Istpos an das neue Refenzsystem anpassen; muss man hier auch drehen ?
    actPosU.x = actPosU.x - UpOri.x;
    actPosU.y = actPosU.y - UpOri.y;

    actPosO.x = actPosU.x;
    actPosO.y = actPosU.y;


    // =============== nur im PP-Mode ==========================
    L_wg_pp:
    if(WC_mode != 0) {
      
      strcpy (WC_outBuf, "WORK ");
      strcat (WC_outBuf, APT_macnam);
      strcat (WC_outBuf, "   ");
      UTX_add_fl_u (WC_outBuf, UpOriAng);
      sprintf(auxBuf,"   %1d",UpMirX);
      strcat (WC_outBuf, auxBuf);
      sprintf(auxBuf,"   %1d",UpMirY);
      strcat (WC_outBuf, auxBuf);

      APT_PP_Write();

    }

    // Check in Liste, ob UP schon mal ausgegeben wurde oder nicht.
    // Wenn nein: dann den MAC-Header raus
    // wenn ja: UP gar nicht mehr aufl�sen.
    if(UP_typ_DE == 0) goto L_wg_fertig;  // Skip Konikprog
    
    UP_resolv = PP_up_list (&upAct, APT_macnam, 1);

    // if(UP_resolv == ON) {
    if(UP_resolv == 0) {

      strcpy (WC_outBuf, "GEO ");
      strcat (WC_outBuf, APT_macnam);
      APT_PP_Write();

      // UP wird (f. PP) aufgel�st; im UP muss ein FROM kommen
      old_APT_ObjU.typ = Typ_Error;
      old_APT_ObjO.typ = Typ_Error;

      //} else {
      //  rc = 0;    // wuerde bedeuten, das UP aufl�sen !
    }

    L_wg_fertig:
    UP_act_typ = 'G';

    UP_mode = ON;

    goto Fertig;
//



  //======================= PRINT =========================================
  } else if ((!strcmp (cmd, "PRI")) || (!strcmp (cmd, "PRINT"))) {

    // decode 
    APT_decode_print (memspc011, *data, aus_typ, aus_tab, aus_anz);
    TX_Print("%s",memspc011);


//
  //================================================================
  } else if (!strcmp (cmd, "TXA")) {

    // printf(" TXA |%s|\n",WC_outBuf);

    WC_outBuf[0] = '\0';

    if(aus_typ[0] == Typ_PT) {
      pt1 = DB_GetPoint ((long)aus_tab[0]);
      i2 = 1;

    } else if(aus_typ[0] == Typ_Val) {
      pt1.x = aus_tab[0];
      pt1.y = aus_tab[1];
      pt1.z = 0.;
      i2 = 2;

    } else {
      goto Fehler1;
    }

    for(i1=i2; i1<aus_anz; ++i1) {

      if(aus_typ[i1] == Typ_Txt) {
        APT_get_Txt (auxBuf, sptr, aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);


      } else if(aus_typ[i1] == Typ_String) {
        APT_get_String (auxBuf, sptr, aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);

      } else {
        goto Fehler1;
      }

    }

    printf(" TXA |%s|\n",WC_outBuf);
    APT_disp_TxtA (0, &pt1, WC_outBuf);


  //================================================================
  } else if (!strcmp (cmd, "TXG")) {

    // printf(" TXG %d\n",aus_anz);

    WC_outBuf[0] = '\0';

    if(aus_typ[0] == Typ_PT) {
      pt1 = DB_GetPoint ((long)aus_tab[0]);
      i2 = 1;

    } else if(aus_typ[0] == Typ_Val) {
      pt1.x = aus_tab[0];
      pt1.y = aus_tab[1];
      pt1.z = 0.;
      i2 = 2;

    } else {
      goto Fehler1;
    }

    if(aus_typ[i2] != Typ_Val) goto Fehler1;  // Size
    d1 = aus_tab[i2];
    ++i2;

    if(aus_typ[i2] != Typ_Val) goto Fehler1;  // angle
    d2 = aus_tab[i2];
    ++i2;

    for(i1=i2; i1<aus_anz; ++i1) {

      if(aus_typ[i1] == Typ_Txt) {
        APT_get_Txt (auxBuf, sptr, aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);

      } else if(aus_typ[i1] == Typ_String) {
        APT_get_String (auxBuf, sptr, aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);

      } else {
        goto Fehler1;
      }

    }

      // printf(" TXG |%s| %f %f\n",WC_outBuf,d1,d2);
    APT_disp_TxtG (0, &pt1, d1, d2, WC_outBuf);
//





  //======================= DRAW ==========================================
  } else if (!strcmp (cmd, "DRAW")) {


    for(i1=0; i1<aus_anz; ++i1) {
      // printf ("  DRAWWWWWW: %d %d %f\n",i1,aus_typ[i1],aus_tab[i1]);

      if(aus_typ[i1] == Typ_Val) {
        ED_set_delay ((int)aus_tab[i1]);


      } else if(aus_typ[0] == Typ_cmdNCsub) {

        if((int)aus_tab[0] == 0) {               // Achtung: "T_ON"
          APT_Stat_Draw = ON;

        } else if((int)aus_tab[0] == 1) {        // Achtung: "T_OFF"
          APT_Stat_Draw = OFF;
          // printf ("  DRAWWWWWW: OFF\n");

        } else {
          goto Fehler1;
        }


      } else {
        goto Fehler1;
      }

    }





  //======================= MODE ==========================================
  } else if (!strcmp (cmd, "MODE")) {
      // printf(" MODE aus_typ[0]=%d\n",aus_typ[0]);
      if(aus_anz > 1)
      // printf("      aus_typ[1]=%d\n",aus_typ[0]);


    if(aus_typ[0] == Typ_Txt) {
      cp = APT_get_Txt (auxBuf, sptr, aus_tab[0]);
      UTX_cp_word_2_upper (auxBuf, auxBuf);
        // printf(" auxBuf=|%s|\n",auxBuf);


      if(!strcmp (auxBuf, "3D")) {
        //TX_Print("start 3D-Input");
        APT_3d_mode = ON;


      } else if(!strcmp (auxBuf, "2D")) {
        //TX_Print("start 2D-Input");
        APT_3d_mode = OFF;


      } else if(!strcmp (auxBuf, "MEN")) {
        if(aus_tab[1] > 0.5) UI_men__ ("Men_off");     // remove menu
        else                 UI_men__ ("Men_on");      // restore menu


      } else if(!strcmp (auxBuf, "BRW")) {
        if(aus_tab[1] > 0.5) UI_men__ ("Brw_off");
        else                 UI_men__ ("Brw_on");


      } else if(!strcmp (auxBuf, "BAR1")) {
        if(aus_tab[1] > 0.5) UI_men__ ("Bar1_off");
        else                 UI_men__ ("Bar1_on");


      } else if(!strcmp (auxBuf, "BAR2")) {
        if(aus_tab[1] > 0.5) UI_men__ ("Bar2_off");
        else                 UI_men__ ("Bar2_on");


      // Sehnentoleranz fuer Kreise; dzt unused !
      } else if(!strcmp (auxBuf, "DISP_AC")) {
        APT_get_Val (&APT_TOL_ac, cp, 0.);
        // printf(" change Disp_AC %f",APT_TOL_ac);


      // "PlanesDisplay OFF" from APT-code
      } else if(!strcmp (auxBuf, "DISP_PL")) {
        if(WC_modact_ind >= 0) goto Fertig; // skip command in Submodels
        APT_get_Txt (auxBuf, cp, 0.);
        UTX_cp_word_2_upper (auxBuf, auxBuf);
        if(!strcmp (auxBuf, "ON")) {          // view (std)
          APT_dispPL = ON;
          UI_upd_plDisp ();
          DL_disp_PL (0);

        } else if(!strcmp (auxBuf, "OFF")) {  // hide
          APT_dispPL = OFF;
          UI_upd_plDisp ();
          DL_disp_PL (1);

        } else {
          goto Fehler1;
        }


      }else if(!strcmp (auxBuf, "DISP_PT")) {
        if(WC_modact_ind >= 0) goto Fertig; // skip command in Submodels
        if(aus_typ[1] == Typ_Val) {
          i1 = aus_tab[1];
          GL_InitPtAtt (i1);
          goto Fertig;
        }
        APT_get_Txt (auxBuf, cp, 0.);
        UTX_cp_word_2_upper (auxBuf, auxBuf);
          // printf(" change Disp_PT |%s|\n",auxBuf);

        if(!strcmp (auxBuf, "ON")) {           // view (std)
          APT_dispPT = ON;
          GL_InitPtAtt (0);
          UI_upd_ptDisp ();

        } else if(!strcmp (auxBuf, "OFF")) {   // hide
          APT_dispPT = OFF;
          GL_InitPtAtt (1);
          UI_upd_ptDisp ();

        } else {
          goto Fehler1;
        }


      } else {
        goto Fehler1;
      }
        

    } else {
      goto Fehler1;
    }
    //TX_Print(" change APTsw_poly to %d",APTsw_poly);




//
  //================================================================
  } else if (!strcmp (cmd, "TOL_POLY")) {

    if(aus_typ[0] == Typ_Val) {
      UT_TOL_cv = aus_tab[0];

    } else {
      goto Fehler1;
    }

    if (WC_mode != 0) {
      sprintf(WC_outBuf,"TOL POLY %f",UT_TOL_cv);
      APT_PP_Write();
    }

    //TX_Print(" change UT_TOL_cv to %f",UT_TOL_cv);


  //================================================================
  } else if (!strcmp (cmd, "BRK_AC")) {

    if(aus_typ[0] == Typ_cmdNCsub) {

      if((int)aus_tab[0] == 0) {               // Achtung: "T_ON"
        APTsw_poly = ON;
      } else if((int)aus_tab[0] == 1) {        // Achtung: "T_OFF"
        APTsw_poly = OFF;
      } else {
        goto Fehler1;
      }

    } else {
      goto Fehler1;
    }
    //TX_Print(" change APTsw_poly to %d",APTsw_poly);


  //================================================================
  } else if (!strcmp (cmd, "BRK_ACLN")) {

    if(aus_typ[0] == Typ_cmdNCsub) {

      if((int)aus_tab[0] == 0) {               // Achtung: "T_ON"
        APTsw_poly_acln = ON;
      } else if((int)aus_tab[0] == 1) {        // Achtung: "T_OFF"
        APTsw_poly_acln = OFF;
      } else {
        goto Fehler1;
      }

    } else {
      goto Fehler1;
    }
    //TX_Print(" change APTsw_poly_acln to %d",APTsw_poly_acln);
//



  //======================= DIM ==============================================
  } else if (!strcmp (cmd, "DIM")) {   // dimmen, nicht Dimension !

    //TX_Print("dim:");

    GR_Att_act = Typ_Att_dim;
    //DB_DIM ();



  //======================= HILI ==============================================
  } else if (!strcmp (cmd, "HILI")) {

    //TX_Print("hili:");

    GR_Att_act = Typ_Att_hili1;



  //======================= NOP ==============================================
  } else if (!strcmp (cmd, "NOP")) {

    //TX_Print("nop:");


    if(aus_typ[0] == Typ_cmdNCsub) {

      if((int)aus_tab[0] == 0) {               // Achtung: "T_ON"
        GR_pick = ON;

      } else if((int)aus_tab[0] == 1) {        // Achtung: "T_OFF"
        GR_pick = OFF;

      } else {
        goto Fehler1;
      }

    } else {
      goto Fehler1;
    }


  //======================= LAY ==============================================
  } else if (!strcmp (cmd, "LAY")) {

    if(aus_typ[0] == Typ_Txt) {

      cp = APT_get_Txt (auxBuf, sptr, aus_tab[0]);
      UTX_cp_word_2_upper (auxBuf, auxBuf);
      //TX_Print(" auxBuf=/%s/ %d",auxBuf,aus_typ[2]);

      if(!strcmp (auxBuf, "ALL")) {
        i1 = -1;
      } else {
        goto Fehler1;
      }

    } else {
      i1 = (int)aus_tab[0];
    }


    // TX_Print("lay %d %d",aus_anz,i1);
    // for(i2=0; i2<aus_anz; ++i2) {
      // TX_Print(" %d %d %f",i2,aus_typ[i2],aus_tab[i2]);
    // }


    if(aus_anz == 1) {
      GR_lay_act = i1;


    // LAY 12 ON
    } else if(aus_anz == 2) {
      if(aus_typ[1] == Typ_cmdNCsub) {

        if((int)aus_tab[1] == 0) {          // Achtung: "T_ON"
          DL_Lay_mod (i1, FUNC_Show, ON);

        } else if(aus_tab[1] == 1) {        // Achtung: "T_OFF"
          DL_Lay_mod (i1, FUNC_Show, OFF);

        } else {
          goto Fehler1;
        }

      } else {
          goto Fehler1;
      }


    // LAY 12 DIM ON
    } else if(aus_anz == 3) {

      if((aus_typ[1] == Typ_Txt)&&(aus_typ[2] == Typ_cmdNCsub)) {

        cp = APT_get_Txt (auxBuf, sptr, aus_tab[1]);
        UTX_cp_word_2_upper (auxBuf, auxBuf);
        //TX_Print(" auxBuf=/%s/ %d",auxBuf,aus_typ[2]);

        if((int)aus_tab[2] == 0) {               // Achtung: "T_ON"
          i3=ON;

        } else if(aus_tab[2] == 1) {             // Achtung: "T_OFF"
          i3=OFF;

        } else {
          goto Fehler1;
        }

        i2=-1;
        if(!strcmp (auxBuf, "NOP")) {
          i2=FUNC_Pick;

        } else if(!strcmp (auxBuf, "DIM")) {
          i2=FUNC_Dim;
          
        } else if(!strcmp (auxBuf, "HILI")) {
          i2=FUNC_Hili;
          
        }

        if(i2 > 0) {
          DL_Lay_mod (i1, i2, i3);

        } else {
          goto Fehler1;
        }

      } else {
          goto Fehler1;
      }



    // LAY 12 ADD ....
    } else {
      if(aus_typ[1] == Typ_Txt) {

        cp = APT_get_Txt (auxBuf, sptr, aus_tab[1]);
        UTX_cp_word_2_upper (auxBuf, auxBuf);
        //TX_Print(" auxBuf=/%s/ %d",auxBuf,aus_typ[2]);

        if(!strcmp (auxBuf, "ADD")) {
          i2=APT_Lay_add (i1, aus_anz-3, sptr, &aus_typ[2], &aus_tab[2]);
          if(i2 < 0) goto Fehler1;

        } else {
          goto Fehler1;
        }
      }
    }



  //======================= STAT ===========================================
  } else if (!strcmp (cmd, "STAT")) {

    TX_Print("Stat:");
    DL_Stat ();
    DB_Stat ();



  //================================================================
  } else if (!strcmp (cmd, "DUMP")) {

    if(aus_typ[0] == Typ_String) {    // APT_get_String
      APT_get_String (auxBuf, sptr, aus_tab[0]);

      if(!strcmp (auxBuf, "GA")) {
        GA_dump__ (NULL);

      } else if(!strcmp (auxBuf, "DL")) {
        DL_DumpObjTab ();

      } else if(!strcmp (auxBuf, "AT")) {
        GL_AttTab_dump__ ();

      } else if(!strcmp (auxBuf, "TX")) {
        Tex_dump__ (NULL);

      } else if(!strcmp (auxBuf, "SD")) {
        Mod_sym_dump ();


      } else goto Fehler1;
    } else goto Fehler1;




  //=====================================================================
  } else {

    TX_Error(" Programmfunktion %s nicht implementiert\n",cmd);

    return -1;


  }



  Fertig:
    // printf("exit APT_work_PrgCmd %d\n",rc);
  return rc;



  Fehler1:
    TX_Error(" bei den Parametern: %s\n",cmd);
    printf("*** Err APT_work_Prg |%s|\n",*data);


  Error:
    return -1;
}
*/


//================================================================
  int APT_work_ATTS (int aus_anz, int aus_typ[], double aus_tab[]) {
//================================================================
/// \code
/// modify surface-attributes
/// ATTS "S" A21            // symbolic
/// ATTS "T2" A21           // transp.
/// ATTS "C" A21            // defCol
/// ATTS "C00ff00" A21      // change color
///
/// TODO: set/reset Texture-attributes ?
/// TODO: CLEAR TRANSPARENCY, symbolic, color ..
/// \endcode


  int       i1, i2, i3, ii, iAtt, iCol, iTra, iTex;
  long      dbi, dli;
  char      s1[80], *p1;
  ColRGB    col1;
  TexRef    *tr;
  ObjAto    ato1;


  printf("APT_work_ATTS %d |%s|\n",aus_anz,APT_defTxt);
  for(ii=0; ii<aus_anz; ++ii)
    printf(" %d typ=%d tab=%lf\n",ii,aus_typ[ii],aus_tab[ii]);


  iAtt = -1;
  iCol = -1;
  iTra = -1;
  iTex = -1;

  for(ii=0; ii<aus_anz; ++ii) {

    if(aus_typ[ii] == Typ_String) {
      // get string
      APT_get_String (s1, APT_defTxt, aus_tab[ii]);
        printf(" s1=|%s|\n",s1);

      //---symbolic--------------------------------------------------
      if(s1[0] == 'S') {
        iAtt = 2;
        continue;
      }

      //---transparency----------------------------------------------
      if(s1[0] == 'T') {
        iTra = s1[1] - '0';
          // printf(" iTra=%d\n",iTra);
        continue;
      }

      //---color-----------------------------------------------------
      if(s1[0] == 'C') {
        if(strlen(s1) == 7) {
          iCol = 0;
          sscanf(&s1[1],"%02x%02x%02x",&i1,&i2,&i3);   // get col out of string
            printf(" col1 = %d %d %d\n",i1,i2,i3);
          Col_set__ (&col1, i1,i2,i3);

        } else {
          iAtt = 1;
        }
        continue;
      }

      //---texture---------------------------------------------------
      if(s1[0] == 'X') {
          iTex = 0;
        if(strlen(s1) > 1) {
          // nun die Ausdruecke aus data einlesen, decodieren und merken
          p1 = &s1[1];
          ATO_getSpc_tmp__ (&ato1, 10);
          ATO_ato_srcLn__ (&ato1, p1);
            // ATO_dump__ (&ato1, "APT_work_ATTS");
          iTex = ato1.nr;
            // for(i2=0; i2<aus_anz; ++i2)
              // printf(" %d typ=%d tab=%lf\n",i2,ia[i2],da[i2]);
        }
        continue;
      }


      //-------------------------------------------------------------
      goto L_errp;
    }

    if((aus_typ[ii] == Typ_SUR)  ||
       (aus_typ[ii] == Typ_SOL))     {
      dbi = aus_tab[ii];
      dli = DL_find_smObj (aus_typ[ii], dbi, -1L, WC_modact_ind);

      if(iTex >= 0) {
        // apply active texture
        GA_Tex__ (dli, 5, aus_typ[ii], dbi);
        // modify texture
        if(iTex > 0) {
          i2 = Tex_getRefInd ();   //printf(" RefInd=%d\n",i2);
          Tex_getRef (&tr, i2);
          tr->uscx = ato1.val[0];
        }
        if(iTex > 1) tr->uscy = ato1.val[1];
        if(iTex > 2) tr->udx  = ato1.val[2];
        if(iTex > 3) tr->udy  = ato1.val[3];
        if(iTex > 4) tr->uar  = ato1.val[4];

        // update modified texture
        DL_Draw_obj (dli, aus_typ[ii], dbi);
        // update display
        DL_Redraw ();
      }


      if(iAtt >= 0) {
        // 1=shaded, 2=symbolic, 3=transparent
        GA_sStyl__ (dli, iAtt, aus_typ[ii], dbi);
      }

      if(iCol >= 0) {
        APcol_actCol__ (&col1);
        // apply active colour AP_actcol. 0=setColor; 1=resetColor
        GA_Col__ (dli, iCol, aus_typ[ii], dbi);
      }

      if(iTra >= 0) {
        // appy transparency; 0=reset; 1=halfTransp.; 2=fullTransp;
        GA_Tra__ (dli, iTra, aus_typ[ii], dbi);
      }


    } else goto L_erro;

  }





  return 0;

  L_errp:
    TX_Error(" APT_work_ATTS unknown parameters %s ..\n",s1);
    return -1;

  L_erro:
    TX_Error(" APT_work_ATTS wrong objecttyp ..\n");
    return -1;

}


//================================================================
  int APT_work_ATTL (int aus_anz, int aus_typ[], double aus_tab[]) {
//================================================================
// modify line-attributes

  int      i1, ii;
  long     dbi, dli;


  // printf("APT_work_ATTL %d\n",aus_anz);
  // for(i1=0; i1<aus_anz; ++i1)
    // printf(" %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);

  ii = 0;

  for(i1=0; i1<aus_anz; ++i1) {

    if(aus_typ[i1] == Typ_Val) {
      ii = aus_tab[i1];
      continue;
    }

    // find dli from typ,dbi
    dbi = aus_tab[i1];
    dli = DL_find_smObj (aus_typ[i1], dbi, -1L, WC_modact_ind);

    GA_lTyp__ (dli, ii, aus_typ[i1], dbi, 1);

  }

  DL_Redraw ();

  return 0;

}


//================================================================
  int APT_work_SHOW (int aus_anz, int aus_typ[], double aus_tab[]) {
//================================================================
// hide / show
  
  int         i1, ii, is;
  long        l1, dli;

    
  printf("APT_work_SHOW %d\n",aus_anz);
  for(i1=0; i1<aus_anz; ++i1)
    printf(" %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);


  if(aus_typ[aus_anz - 1] == Typ_cmdNCsub) {
    i1 = aus_tab[aus_anz - 1];
    if(i1 == T_OFF_) {
      is = 3; // 3=HIDE,
    }
    --aus_anz;
  } else {
    is = 2; // 2=SHOW
  }
    // printf(" is=%d\n",is);


  for(i1=0; i1<aus_anz; ++i1) {
    l1 = aus_tab[i1];

    // find dli from typ,dbi
    dli = DL_find_smObj (aus_typ[i1], l1, -1L, WC_modact_ind);

    // update PermanentAttributeList;  is: 3=HIDE, 2=SHOW
    GA_view__ (dli, is, 0, 0L);                  // HIDE
    // GA_view__ (-1L, 1, i1, l1);  // does not modify DL !

  }

  DL_Redraw ();           // update display

  return 0;

}


//================================================================
  int APT_work_VIEW (int aus_anz, int aus_typ[], double aus_tab[]) {
//================================================================
// view (center, scale, rotation-angle, tilt-angle)
// VIEW(ANG(60) ANG(30))

  int         i1, ii;
  double      da[10], a1, a2;
  Point       *pp1;
  Vector      vex, vez, vcy;


  // printf("APT_work_VIEW %d\n",aus_anz);
  // for(i1=0; i1<aus_anz; ++i1)
    // printf(" %d typ=%d tab=%f\n",i1,aus_typ[i1],aus_tab[i1]);


  a1 = UT_DB_LEER;
    // printf(" a1=%lf\n",a1);


  // old version: 10 values ..
  if(aus_anz == 10) {
    memcpy (da, aus_tab, sizeof(double) * 10);
    goto L_work;
  }
  

  // get view-parameters;
  GL_View_get (da);

  ii = -1;


  L_nxt:
  ++ii;
  if(ii >= aus_anz) goto L_work;

  // point
  if(aus_typ[ii] == Typ_PT) {
    pp1 = DB_get_PT ((long)aus_tab[ii]);
    memcpy (&da[3], pp1, sizeof(Point));
    // Z-value of view-plane
    UI_AP (UI_FuncSet, UID_ouf_vwz, (void*)&da[5]); // in box raus
    goto L_nxt;
  }


  // scale
  if(aus_typ[ii] == Typ_Val) {
    da[6] = aus_tab[ii];
    goto L_nxt;
  }


  // angle1 = rotation-angle
  if((aus_typ[ii] == Typ_Angle) && (a1 == UT_DB_LEER)) {
    a1 = UT_RADIANS(aus_tab[ii]);
    a2 = 0.;
    goto L_nxt;
  }


  // angle2 = tilt-angle
  if(aus_typ[ii] == Typ_Angle) {
    a2 = UT_RADIANS(aus_tab[ii]);
    goto L_nxt;
  }



  TX_Error(" view-definition not implemented ");
  return -1;



  //----------------------------------------------------------------
  L_work:

  if(a1 != UT_DB_LEER) {
      // printf(" a1=%lf a2=%lf\n",a1,a2);
    vex = UT3D_VECTOR_X;
    vez = UT3D_VECTOR_Z;

    // rotate;
    UT3D_vc_rotvcangr (&vex, a1, &vex, &vez);
    
    // tilt
    UT3D_vc_perp2vc (&vcy, &vez, &vex);  // vcy from vcx and vcz
    UT3D_vc_rotvcangr (&vex, a2, &vex, &vcy);
    UT3D_vc_perp2vc (&vez, &vex, &vcy);

    memcpy (&da[0], &vex, sizeof(Vector));
    memcpy (&da[7], &vez, sizeof(Vector));

  }


  // set View:
  GL_View_set (da);


  APT_view_stat = 1;  // 1=VIEW gesetzt.

  return 0;

}
 

//================================================================
  int APT_line_set (int i1) {
//================================================================
 
  APT_line_act = i1;

  return 0;
}


//======================================================================
  int APT_UP_get () {
//======================================================================
/// returns APT_line_act of calling line ..

  // if(UP_level >= 0) return UP_level_adr[UP_level] - 1;
  if(UP_level >= 0) return UP_level_adr[0] - 1;

  return APT_line_act;

}


//======================================================================
  int APT_UP_up (char src) {
//======================================================================
// src = P (Prog) od F (File)


  int irc;


  if(UP_level < UP_level_max) {
    ++UP_level;
  } else {
    TX_Error("max UP-Level ueberschritten");
    return -1;
  }


  UP_level_adr[UP_level] = APT_line_act + 1;
  UP_level_typ[UP_level] = UP_act_typ;
  UP_level_src[UP_level] = src;


  printf("APT_UP_up %c lev=%d adr=%d\n",src,UP_level,UP_level_adr[UP_level]);


  irc = PrgMod_skip_until_mac;

  if(src == 'F') irc = PrgMod_skip_until_file;


  return irc;

}


//======================================================================
  int APT_UP_down () {
//======================================================================

  int  irc;
  // char src;


  irc = PrgMod_skip_until_line;

  // bei call-File stehen die Pointer korrekt auf die weitrfuehrende
  // Zeile; kein Vorlesen notwendig ..
  if(UP_level_src[UP_level] == 'F') irc = PrgMod_normal;
  


  Prg_line_nr = UP_level_adr[UP_level];
  UP_act_typ  = UP_level_typ[UP_level];
  // src         = UP_level_src[UP_level];
  --UP_level;


  // if(UP_level >= 0) {
    // if(UP_level_src[UP_level] == 'F') irc = PrgMod_continue_file;
  // }


  // printf("APT_UP_down lev=%d adr=%d\n",UP_level,Prg_line_nr);
  // printf("          UP_act_typ=%c irc=%d\n",UP_act_typ,irc);

  return irc;

}



/*
//================================================================
  int APT_work_NCCmd (char* cmdIn, char** data) {
//================================================================


  //long      apt_ind;
  int       i1;
  long      l1;
  double    d1;
  char      cmd[32], auxBuf[64], *cp, *sptr;
  // Point2    pt20;
  Point     pt1;
  //Line      ln1;
  // Vector    vc1, vc2;


  int       aus_anz;
  int       aus_typ[10];
  double    aus_tab[10];



  printf("APT_work_NCCmd |%s|%s|\n",cmdIn,*data);

  UTX_cp_word_2_upper (cmd, cmdIn);



  // nun die Ausdruecke aus data einlesen, decodieren und merken
  if(strlen(*data) > 0) {
    aus_anz = APT_decode_ausdr (aus_typ, aus_tab, 10, data);
    if(aus_anz < 0) goto Error;

  } else {
    aus_anz = 0;
  }

  L_1:
  sptr = *data;



  // TEST
  // printf(" _work_NCCmd aus_anz=%d\n",aus_anz);
  // for(i1=0;i1<aus_anz; ++i1) 
    // printf(" aus_[%d] = %d %f\n",i1,aus_typ[i1],aus_tab[i1]);




  //=====================================================================
  // ein 3D-Refsys; Berechnung erst vor Anzeige.
  if (!strcmp (cmd, "REF")) {

    // printf("REF %d %f\n",aus_typ[0],aus_tab[0]);

    if(aus_typ[0]==Typ_PLN) {
      i1 = aus_tab[0];
      NC_setRefsys (i1);                // Refsys aktivieren

    } else {
      goto ParErr;
    }



  //=====================================================================
  // Translate;
  } else if (!strcmp (cmd, "TRA")) {

    if(aus_typ[0] != Typ_PLN) goto ParErr;

    APT_tra = aus_tab[0];
    DB_GetRef (&APT_pln_tra, APT_tra);
    UT3D_m3_loadpl (APT_mat_tra, &APT_pln_tra);




  //=====================================================================
  // Project
  } else if (!strcmp (cmd, "PROJ")) {

    if(aus_typ[0] != Typ_PLN) goto ParErr;

    APT_proj = aus_tab[0];




  //=====================================================================
  // Mirror
  } else if (!strcmp (cmd, "MIRR")) {

    if(aus_typ[0] != Typ_PLN) goto ParErr;

    APT_mirr = aus_tab[0];






  //=====================================================================
  // ein 2D-Refsys fuer NC
  } else if (!strcmp (cmd, "ORI")) {

    d1 = 0.0;


    if(aus_typ[0]==Typ_PT) {

      pt1 = DB_GetPoint ((long)aus_tab[0]);


    }else if((aus_typ[0] == Typ_Val)        &&
             (aus_typ[1] == Typ_Val))    {

      pt1.x = aus_tab[0];
      pt1.y = aus_tab[1];

      if(aus_typ[2] == Typ_Val) {
        d1 = aus_tab[2];
      }


    } else {
      goto ParErr;
    }


    // wurde der optionale Winkel angegeben ?
    if(aus_typ[aus_anz-1] == Typ_Angle) {
      d1 = aus_tab[aus_anz-1];
    }



    //TX_Print("Refsys %f %f %f\n",pt1.x,pt1.y,d1);
    RefOri.x = actPosU.x - pt1.x;
    RefOri.y = actPosU.y - pt1.y;
    RefOriAng   = d1;

    RefOriAng_sin = sin(UT_RADIANS(RefOriAng));
    RefOriAng_cos = cos(UT_RADIANS(RefOriAng));


    strcpy (WC_outBuf,"ORIGIN ");
    UTX_add_fl_u2 (WC_outBuf, RefOri.x, RefOri.y);
    strcat (WC_outBuf,",");
    UTX_add_fl_u (WC_outBuf, RefOriAng);


    if (WC_mode == 0) {
      TX_Print(WC_outBuf);
    } else {
      APT_PP_Write();
    }






  //=====================================================================
  } else if (!strcmp (cmd, "PLANE")) {
    //TX_Print("Plane %d / %f",aus_typ[0],aus_tab[0]);
    TX_Error(" PLANE wurde durch Refsys R#= .. ersetzt");

//
//     pt1 = UT3D_PT_NUL;
// 
// 
//     // plane x(val)
//     if((aus_typ[0] == Typ_PT)  &&
//        (aus_typ[1] == Typ_PT)  &&
//        (aus_typ[2] == Typ_PT))     {
// 
//       APT_2d_to_3d_mode = ON;
//       //TX_Print("Plane aus 3 Punkten");
// 
//       pt1 = DB_GetPoint ((long)aus_tab[0]);
//       pt2 = DB_GetPoint ((long)aus_tab[1]);
//       pt3 = DB_GetPoint ((long)aus_tab[2]);
// 
//       UT3D_vc_2pt (&vc1, &pt1, &pt2);
//       UT3D_vc_2pt (&vc2, &pt1, &pt3);
// 
// 
// 
// 
//     // Das Achsensystem (eigentlich eine Plane) aus einer Equation bestimmen
//     // fuer sp�tere Transformation.
// 
//     // plane x(val)
//     } else if(aus_typ[0] == Typ_XVal) {
//       pt1.x = aus_tab[0];
//       vc1 = UT3D_VECTOR_Y;
//       vc2 = UT3D_VECTOR_Z;
//       APT_2d_to_3d_mode = ON;
// 
//     // plane y(val)
//     } else  if(aus_typ[0] == Typ_YVal) {
//       pt1.y = aus_tab[0];
//       vc1 = UT3D_VECTOR_X;
//       vc2 = UT3D_VECTOR_Z;
//       APT_2d_to_3d_mode = ON;
// 
//     // plane z(val)
//     } else  if(aus_typ[0] == Typ_ZVal) {
//       pt1.z = aus_tab[0];
//       vc1 = UT3D_VECTOR_X;
//       vc2 = UT3D_VECTOR_Y;
//       if(UTP_comp_0(pt1.z)) {
//         APT_2d_to_3d_mode = OFF;
//       } else {
//         APT_2d_to_3d_mode = ON;
//       }
// 
//     } else {
//       goto ParErr;
//     }
// 
// 
//     //TX_Print("Nullpunkt %.2f,%.2f,%.2f",pt1.x,pt1.y,pt1.z);
// 
//     // Plane aus Nullpunkt, X_Vektor u. Vektor in Plane
//     UT3D_pl_pt2vc (&APT_2d_to_3d_Pln, &pt1, &vc1, &vc2);
// 
// 
//     // Matrix mit Plane beladen
//     UT3D_m3_loadpl (APT_2d_to_3d_Mat, &APT_2d_to_3d_Pln);
//
// 



  //=====================================================================
  } else if (!strcmp (cmd, "FROM")) {

    if       (aus_typ[0]==Typ_PT) {

      pt1 = DB_GetPoint ((long)aus_tab[0]);


    }else if((aus_typ[0] == Typ_Val)        &&
             (aus_typ[1] == Typ_Val))    {

      pt1.x = aus_tab[0];
      pt1.y = aus_tab[1];


    } else {
      goto ParErr;
    }


    // printf("FROM %f,%f UP_mode=%d UP_act_typ=/%c/\n",pt1.x,pt1.y,
                // UP_mode,UP_act_typ);


     // Im Mainprogramm; ev noch testen ob in MAC innerhalb GEO!
    if((UP_mode == OFF)||(UP_act_typ == 'M')) {

      if(APT_stat_FROM != OFF) {    // pro Main darf es nur ein FROM geben!
        // TX_Error("im Main darf es nur ein FROM geben!");
        printf("im Main darf es nur ein FROM geben!\n");
      }

      actPosU.x = pt1.x;
      actPosU.y = pt1.y;

      actPosO.x = pt1.x;
      actPosO.y = pt1.y;

      actPosUtr.x = pt1.x;
      actPosUtr.y = pt1.y;

      actPosOtr.x = pt1.x;
      actPosOtr.y = pt1.y;

      //TX_Print("  MAIN: from %f %f\n",pt1.x,pt1.y);

      //  merken, sonst wirds automatisch nochmal erzeugt.
      old_APT_ObjU.typ = Typ_PT2;
      old_APT_ObjO.typ = Typ_PT2;

      old_APT_ObjU.p1  = actPosU;
      old_APT_ObjO.p1  = actPosU;
      ++APT_line_cnt;
      old_APT_ObjU.nam = APT_line_cnt;

    // Im UP; FROM darfs nur im 1.Level geben.
    } else {

      UpOri.x = UpOri.x - pt1.x;
      UpOri.y = UpOri.y - pt1.y;
      //TX_Print(" change UpOri to %f %f",UpOri.x,UpOri.y);

      // Istpos an das neue Refenzsystem anpassen; muss man hier auch drehen ?
      old_APT_ObjU.typ = Typ_PT2;
      old_APT_ObjO.typ = Typ_PT2;

      actPosU.x = actPosU.x + pt1.x;
      actPosU.y = actPosU.y + pt1.y;

      actPosO.x = actPosO.x + pt1.x;
      actPosO.y = actPosO.y + pt1.y;

      old_APT_ObjU.p1  = actPosU;
      old_APT_ObjO.p1  = actPosU;

      // das From merken (zum Drehpunkt und Spiegelcenter berechnen)
      UpFrom.x = pt1.x;
      UpFrom.y = pt1.y;
      //TX_Print(" change UpFrom to %f %f",UpFrom.x,UpFrom.y);

    }



      actPosUtr = APT_transl2 (&actPosU);

      old_APT_ObjU.p1  = actPosU;

      old_APT_ObjUtr.p1  = actPosUtr;



    // UT3D_stru_dump (Typ_PT2, &actPosU, "from actPosU");
    // UT3D_stru_dump (Typ_PT2, &actPosUtr, "from actPosUtr");
    // printf(" WC_mode=%d\n",WC_mode);


    
    if(WC_mode != 0) {  // if(PP) und if noch kein FROM..
      //TX_Print(" PP: write FROM from FROM\n");
      strcpy (WC_outBuf,"FROM ");
      //UTX_add_fl_u2 (WC_outBuf, actPosUtr.x, actPosUtr.y);
      UTX_add_fl_u2 (WC_outBuf, actPosU.x, actPosU.y);
      APT_PP_Write();

    } else {
      // Interaktiv Symbol wie bei STOP
      APT_disp_SymV (SYM_TRIANG, 0, &actPosUtr, 0.5);     // Dreieck klein
    }



    APT_stat_FROM = ON;






  //=====================================================================
  // Funktionen, die an den PP weitergereicht werden:
  } else if ((!strcmp (cmd, "FED"))     ||
             (!strcmp (cmd, "TEC")))  {

    strcpy (WC_outBuf,cmd);
    strcat (WC_outBuf," ");
    strcat (WC_outBuf,*data);

    // i1 = aus_tab[0];

    ++APT_line_cnt;
    APT_stack_NCCmd (WC_outBuf, 0);    







  //=====================================================================
  // Funktionen, die an den PP weitergereicht werden:
  } else if (!strcmp (cmd, "PN")) {

    goto L_Out_Format1;







  //=====================================================================
  } else if (!strcmp (cmd, "MCH")) {

    if       (aus_typ[0]==Typ_Txt) {
      cp = *data;
      cp=cp+(int)aus_tab[0];
      //printf(" Startpos %f |%s|\n",aus_tab[0],cp);
      UTX_cp_word_term (auxBuf, cp, '"');
      UTX_cp_word_2_upper (cmd, auxBuf);


      if(!strcmp (cmd, "FANUC")) {
        printf("fanuc\n");


      } else if(!strcmp (cmd, "CHARMI")) {
        printf("charmi\n");


      } else if(!strcmp (cmd, "AGI123")) {
        printf("agi123\n");


      } else {
        printf("Maschine unbekannt\n");
      }


    } else {
      goto ParErr;
    }


    strcpy (WC_outBuf,"MCH ");
    strcat (WC_outBuf, cmd);


    if (WC_mode == 0) {
      TX_Print(WC_outBuf);
    } else {
      APT_PP_Write();
    }





  //=====================================================================
  } else if (!strcmp (cmd, "STOP")) {

//
//     if (WC_mode == 0) {
//       strcpy (APT_txBuf,"STOP auf  ");
//       UTX_add_fl_u2 (APT_txBuf, actPosUtr.x, actPosUtr.y);
//       TX_Print(APT_txBuf);
//       APT_disp_SymV (SYM_TRIANG, 0, &actPosUtr, 0.6);     // Dreieck gross
// 
//     } else {
//       strcpy (WC_outBuf,"STOP");
//       APT_PP_Write();
// 
//     }
//


    ++APT_line_cnt;
    APT_stack_NCCmd ("STOP", SYM_TRIANG);    







  //=====================================================================
  } else if (!strcmp (cmd, "OSP")) {

    strcpy (WC_outBuf,"OSP");

    //TX_Print("OSP anz=%d typ=%d",aus_anz,aus_typ[0]);

    // Check for ON oder OFF
    if(aus_typ[0] == Typ_cmdNCsub) {
      if((int)aus_tab[0] == 0) {
        APT_OPSTOP_stat = ON;
        strcat(WC_outBuf," 0");
      } else if((int)aus_tab[0] == 1) {
        APT_OPSTOP_stat = OFF;
        strcat(WC_outBuf," 1");
      } else {
        goto ParErr;
      }
      APT_PP_Write();
      return 0;
    }




    i1 = 0;
    if(APT_OPSTOP_stat == ON) {
//       
//       if(WC_mode == 0) {
//         strcpy (APT_txBuf,"OPSTOP auf  ");
//         UTX_add_fl_u2 (APT_txBuf, actPosUtr.x, actPosUtr.y);
//         TX_Print(APT_txBuf);
//         // Als Symbol darstellen
//         //APT_disp_SymV (SYM_TRIANG, 0, &actPosUtr, 0.3);     // Dreieck klein
//       }
//       
      i1 = SYM_TRIANG;
    }

    ++APT_line_cnt;
    APT_stack_NCCmd ("OSP", i1);    





//
//   //================================================================
//   } else if (!strcmp (cmd, "ZSUR1")) {
// 
//     if(aus_anz == 0) {
//       WC_sur_Z = WC_sur1;
//       goto L_Out_Format0;
//     }
// 
//     if(aus_typ[0] != Typ_Val) goto ParErr;
// 
//     WC_sur1  = aus_tab[0];
//     WC_thick = fabs(WC_sur2 - WC_sur1);
// 
//     // if (WC_mode != 0) {
//       // sprintf(WC_outBuf,"ZSUR1 %f",WC_sur1);
//       // APT_PP_Write();
//     // }
// 
//     goto L_Out_Format1;
// 
// 
// 
// 
//   //================================================================
//   } else if (!strcmp (cmd, "ZSUR2")) {
// 
//     if(aus_anz == 0) {
//       WC_sur_Z = WC_sur2;
//       goto L_Out_Format0;
//     }
// 
// 
//     if(aus_typ[0] != Typ_Val) goto ParErr;
// 
//     WC_sur2  = aus_tab[0];
//     WC_thick = fabs(WC_sur2 - WC_sur1);
// 
//     // if (WC_mode != 0) {
//       // sprintf(WC_outBuf,"ZSUR2 %f",WC_sur2);
//       // APT_PP_Write();
//     // }
// 
//     goto L_Out_Format1;
// 
// 
// 
// 
// 
//   //================================================================
//   } else if (!strcmp (cmd, "POCKET")) {
// 
// 
//     for(i1=0; i1<aus_anz; ++i1) {
//       if(aus_typ[i1] != Typ_Val) goto ParErr;
//       if(i1 == 0) APT_pock_distk = aus_tab[0];
//       if(i1 == 1) APT_pock_distc = aus_tab[1];
//       if(i1 == 2) APT_pock_dirc  = aus_tab[2];
//     }
// 
//     // wird nur intern benutzt !
//     goto Fertig;
//     // goto L_Out_Format1;
///





  //================================================================
  } else if (!strcmp (cmd, "MODORI")) {       // Model-Origin

    if (aus_typ[0]==Typ_PT) {
      WC_mod_ori = DB_GetPoint ((long)aus_tab[0]);
    } else {
      goto ParErr;
    }





  //================================================================
  } else if (!strcmp (cmd, "TL")) {

    // printf(" TL %d\n",aus_typ[0]);


    if (aus_typ[0] == Typ_Val) {
      i1 = aus_tab[0];
      APT_tl_stat = i1;

      ++APT_line_cnt;
      sprintf(auxBuf, "TL %d",i1);
      APT_stack_NCCmd (auxBuf, 0);

      return 0;



    } else if(aus_typ[0] == Typ_cmdNCsub) {
      UTX_cp_word_2_upper (cmd, *data);


      // printf(" TL %d\n",(int)aus_tab[0]);
      i1 = aus_tab[0];
      if(i1 == T_OFF_) {
        APT_TL_Side = 0;
      } else if(i1 == T_RGT) {
        APT_TL_Side = -1;
      } else if(i1 == T_LFT) {
        APT_TL_Side = 1;
      } else {
        goto ParErr;
      }

      
    } else {
      goto ParErr;
    }

    strcpy (WC_outBuf,"TL ");
    strcat (WC_outBuf, cmd);



    if (WC_mode == 0) {


      ++APT_line_cnt;
      APT_stack_NCCmd (WC_outBuf, 0);

      // TX_Print(WC_outBuf);


    } else {
      if(APT_TL_Side == 0) {
        strcpy (WC_outBuf,"G40");
      } else if(APT_TL_Side == -1) {
        strcpy (WC_outBuf,"G42");
      } else if(APT_TL_Side == 1) {
        strcpy (WC_outBuf,"G41");
      }

      APT_PP_Write();
    }



  //================================================================
  } else if (!strcmp (cmd, "TCPOS")) {

    // Toolchangeposition (ein 3D-Punkt) holen.
    if (aus_typ[0]==Typ_PT) {
      pt1 = DB_GetPoint ((long)aus_tab[0]);

    }else if((aus_typ[0] == Typ_Val)        &&
             (aus_typ[1] == Typ_Val))    {

      pt1.x = aus_tab[0];
      pt1.y = aus_tab[1];

    } else {
      goto ParErr;
    }

    APT_tcpos  = pt1;
    goto L_Out_Format1;





  //================================================================
  } else if (!strcmp (cmd, "DEEP")) {

    // Bohrtiefe (ein Z-Wert) holen
    if(aus_typ[0] != Typ_Val) goto ParErr;

    APT_Z_deep  = aus_tab[0];
    WC_sur3     = WC_sur1 - APT_Z_deep;

    goto L_Out_Format1;





  //================================================================
  } else if (!strcmp (cmd, "CLEAR")) {

    // Clearance-Plane (ein Z-Wert) holen
    if(aus_typ[0] != Typ_Val) goto ParErr;

    APT_Z_clear  = aus_tab[0];
    WC_sur2 = WC_sur1 + APT_Z_clear;

    goto L_Out_Format1;




  //================================================================
  } else if (!strcmp (cmd, "DRILL")) {

    // Beginn Berbeitungszyklus Bohren
    APT_workmode = Typ_nc_drill;
    goto Fertig;



  //================================================================
  } else if (!strcmp (cmd, "CYCLE")) {

    // nur fuer cycle off genutzt.
    if   (aus_typ[0] == Typ_cmdNCsub) {
      if((int)aus_tab[0] == 1) {      // Achtung: "T_OFF" w�re hier erforderl.
        TX_Print(" cycle off");
        APT_workmode = Typ_nc_cut;
        goto Fertig;
      }
    }

    goto ParErr;
*/


/*
  //=====================================================================
  } else if (!strcmp (cmd, "CONIC")) {

    if   (aus_typ[0] == Typ_cmdNCsub) {
      if((int)aus_tab[0] == 1) goto L_CONIC_1;    // Achtung: "T_OFF" w�re hier erforderl.
    }

    goto ParErr;



    L_CONIC_1:
    // Konik aus
    APT_konik  = KONIK_0;

    UTX_cp_word_2_upper (cmd, *data);

    strcpy (WC_outBuf,"CONIC ");
    strcat (WC_outBuf, cmd);



    if (WC_mode == 0) {
      TX_Print(WC_outBuf);
    } else {
      APT_PP_Write();
    }
*/


/*
  //================================================================
  } else if (!strcmp (cmd, "COOL")) {

    if   (aus_typ[0] == Typ_cmdNCsub) {
      UTX_cp_word_2_upper (cmd, *data);

    } else {
      goto ParErr;
    }


    strcpy (WC_outBuf,"COOL ");
    strcat (WC_outBuf, cmd);


    ++APT_line_cnt;
    APT_stack_NCCmd (WC_outBuf, 0);    


    // if (WC_mode == 0) {
      // TX_Print(WC_outBuf);
    // } else {
      // APT_PP_Write();
    // }





  //================================================================
  } else if (!strcmp (cmd, "WC")) {

    ++APT_line_cnt;
    APT_stack_NCCmd ("WC", 0);    



  //================================================================
  } else if (!strcmp (cmd, "WT")) {

    ++APT_line_cnt;
    APT_stack_NCCmd ("WT", 0);    




  //================================================================
  } else if ((!strcmp (cmd, "INL"))     ||
             (!strcmp (cmd, "INS"))     ||
             (!strcmp (cmd, "INE")))  {

    //TX_Print("Insert |%s|%s| ins=%d %d %d",cmd,*data,insCnt[0],insCnt[1],insCnt[2]);


    strcpy (WC_outBuf,cmd);
    strcat (WC_outBuf," ");


    for(i1=0; i1<aus_anz; ++i1) {
      printf ("  print: %d %d %f\n",i1,aus_typ[i1],aus_tab[i1]);
      printf ("  |%s| %d\n",WC_outBuf, (int)strlen(WC_outBuf));

      if((aus_typ[i1] == Typ_Val)  ||
         (aus_typ[i1] == Typ_XVal) ||
         (aus_typ[i1] == Typ_YVal) ||
         (aus_typ[i1] == Typ_ZVal))    {
        sprintf(auxBuf,"% .2f",aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);

      } else if(aus_typ[i1] == Typ_Txt) {
        APT_get_Txt (auxBuf, sptr, aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);

      } else if(aus_typ[i1] == Typ_String) {
        if(strlen(sptr) > 60) goto ParErr;
        APT_get_String (auxBuf, sptr, aus_tab[i1]);
        strcat (WC_outBuf, auxBuf);

      } else {
        goto ParErr;
      }

    }


    // merken
    ++APT_line_cnt;
    if(APT_stack_NCCmd (WC_outBuf, 0) < 0) return -1;    




  //================================================================
  } else if (!strcmp (cmd, "TEST")) {
    TX_Print("Test");
    // Test Hilite






  //=====================================================================
  } else {

    TX_Error(" APT-Funktion %s nicht implementiert\n",cmd);

    return -1;

  }



  L_exit:
  return 0;




  //=========================================================
  L_Out_Format0:

  strcpy (WC_outBuf,cmd);

  goto L_Out_Format_Go;




  //=========================================================
  L_Out_Format1:

  strcpy (WC_outBuf,cmd);
  strcat (WC_outBuf," ");
  strcat (WC_outBuf,*data);

  goto L_Out_Format_Go;


  
  //=========================================================
  L_Out_Format_Go:

  if (WC_mode != 0) {
    //TX_Print(WC_outBuf);
    APT_PP_Write();
  }


  Fertig:
  return 0;


  




  //================= Fehler =============================
  ParErr:
  TX_Print("Falsche Parameter bei:  %s",cmdIn);
  OS_beep ();

  Error:
  return -1;

}


//============================================================================
  int APT_stack_NCCmd (char* buf, int symbol) {
//============================================================================
// ein NC-Command; es muss aber zum richtigen Zeitpunkt raus !
// Zwischenspeichern in insBuf; parallel dazu die akt. APT_line_cnt merken.
// Ausgeben mit APT_ausg_NCCmd

// APT_stack_NCCmd ("STOP", SYM_TRIANG);

  // int i1;


  // printf("\nAPT_stack_NCCmd %d /%s/ sym=%d\n",insAnz,buf,symbol);
  // printf("  old_APT_ObjU.typ=%d\n",old_APT_ObjU.typ);
  // printf("  actPosUtr=%f %f\n",actPosUtr.x,actPosUtr.y);




  //TX_Print ("  UP_mode=%d UP_resolv=%d UP_act_typ=%c\n",UP_mode,UP_resolv,UP_act_typ);



  if((insAnz >= 9)||(insAnz < 0)) {
    TX_Error("APT_stack_NCCmd E001");
    return -1;
  }
  if(strlen(buf) > 60) {
    TX_Error("APT_stack_NCCmd E002");
    return -1;
  }



  //TX_Print("APT_stack_NCCmd /%s/ %d cnt=%d",buf,insAnz,APT_line_cnt);
  insCnt[insAnz] = APT_line_cnt;
  strcpy(insBuf[insAnz], buf);
  insSym[insAnz] = symbol;       // das zugeh�rige Symbol

  // printf(" stacked %d /%s/ %d\n",insAnz,insBuf[insAnz],insSym[insAnz]);
  ++insAnz;






  // L_Ins_Done:

  // wenn gar nix im Stack, sofort raus
  // if(old_APT_ObjU.typ == Typ_PT2) {

    // old_APT_ObjUtr.nam = APT_line_cnt; // damits sofort raus geht.

    //old_APT_ObjUtr.p1  = actPosUtr;
    //TX_Print("APT_stack_NCCmd Sofortausgabe /%s/ %f,%f",buf,
    //                             old_APT_ObjUtr.p1.x,old_APT_ObjUtr.p1.y);


    APT_ausg_NCCmd (0);
  // }





///
//   // nur Testausgeben des Stack:
//   for(i1=0; i1<insAnz; ++i1) {
//     TX_Print(" stack %d /%s/ cnt=%d",i1,insBuf[i1],insCnt[i1]);
//   }
///


  return 1;



}


//===========================================================================
  int APT_input_get (char* inpLin) {
//=================
// Alle Inputdaten (wenn erforderlich Fortsetzungszeilen holen) in das
// Feld APT_LINE einschlichten.
// Dzt unused !

  int  i1, ll, i2;
  char *cp, *cp1, *cp2;


  cp = UTX_pos_1n (inpLin);
  UTX_CleanCR (cp);   // foll. blanks usw weg


  APT_LINE[0] ='\0';
  ll = 2;





  //===========================================================================
  StoreLine:


  ll += strlen(cp);
  if(ll > APT_LINE_siz) {
    TX_Error("APT_input_get: Zeilenpuffer zu klein");
    return 0;
  }



  // wenn der letzte Char der vorherigen Zeile ein "," war, und der erste Char
  // der aktuellen Zeile ein "|" ist, dann muss das "," raus (war nur notwendig
  // um Fortsetzungszeile zu deklarieren).
  i1=strlen(APT_LINE);
  if(i1 < 2) goto StoreLine_1;
  if(strcmp(&APT_LINE[i1-1], ",")) goto StoreLine_1;
  if(strncmp(cp, "|", 1)) goto StoreLine_1;

  //TX_Print(" skip the komma");
  APT_LINE[i1-1] = '\0';




  // aktuelle Zeile speichern
  StoreLine_1:

  strcat(APT_LINE, cp);

  // wenn letzter Char war ","; wenn der Char davor ein "|", dann muss "," weg.
  i1 = strlen(APT_LINE);
  cp1 = &APT_LINE[i1-1];
  cp2 = &APT_LINE[i1-2];
  //if((!strcmp(cp1,","))&&(!strcmp(cp1,"|"))) {
  if(!strcmp(cp1,",")) {
    *cp1 = ' ';
    //TX_Print(" remove komma /%s/",APT_LINE);
  }



  // Pruefen auf Folgezeile. Dazu muesste der letzte Char der Zeile ein Komma oder
  // ein "|" sein.
  i1 = strlen(cp);
  cp1 = &cp[i1-1];
  //TX_Print( "last char %d = |%s|",i1,cp1);


  //if((!strcmp(cp1,","))||(!strcmp(cp1,"|"))) {
  if(!strcmp(cp1,",")) {
    //TX_Print(" .. Fortsetzung folgt .. %d %d",ll,APT_LINE_siz);

    // Fortsetzungszeile holen
    //ED_GetNxtLin (&i1, APT_txBuf);
    ++APT_line_act;
    UTF_GetLinNr (APT_txBuf, &i2, APT_line_act);
    //UTX_CleanCR (APT_txBuf);   // foll. blanks usw weg
    //TX_Print(" got NxtLin |%s|",APT_txBuf);


    // der erste char muss ein "&" sein, sonst Fehler. 
    cp = UTX_pos_1n (APT_txBuf);
    if(strncmp(cp, "&", 1)) {
      //TX_Error("Fortsetzungszeile muss mit & beginnen");
      return 0;
    }
    ++cp;  // das & skippen
    cp = UTX_pos_1n (cp);
    goto StoreLine;

  }




  //===========================================================================
  //TX_Print(" ex APT_input_get /%s/",APT_LINE);

  return 1;
}


//================================================================
  void APT_PP_FROM () {
//================================================================

  printf("APT_PP_FROM %d actPos=%f,%f\n",old_APT_ObjU.typ,actPosU.x,actPosU.y);




  // Check for FROM; war keines, dann eines in den PP ausgeben.
  if(old_APT_ObjU.typ == Typ_Error) {

    //TX_Print("APT_PP_FROM: %d %f,%f\n",old_APT_ObjU.typ, actPosU.x, actPosU.y);

    old_APT_ObjU.typ = Typ_PT2;
    old_APT_ObjO.typ = Typ_PT2;  // fuer den Start oben.

    old_APT_ObjU.p1  = actPosU;
    ++APT_line_cnt;
    old_APT_ObjU.nam = APT_line_cnt;



    // ol1_APT_ObjU.typ = Typ_PT2;
    // ol1_APT_ObjO.typ = Typ_PT2;

    old_APT_ObjO.p1  = actPosU;



    //APT_konik = KONIK_0;
    // APT_UVal    = 0.0;
    // APT_VVal    = 0.0;

    // APT_RadTyp = 0;       // 0=konisch, 1=ISO, 2=userspezifisch. Nicht modal.



    if(WC_mode != 0) {                                   // if (PP) ..
      // wenn im Main noch kein FROM war ..
      if(((UP_mode == OFF)&&(APT_stat_FROM == OFF)) ||
          (UP_mode == ON)  ) { 
        //TX_Print(" PP: write FROM in APT_PP_FROM\n");
        strcpy (WC_outBuf,"FROM ");
        UTX_add_fl_u2 (WC_outBuf, actPosU.x, actPosU.y);
        APT_PP_Write();
      }
    //} else {
    //  APT_disp_Sym (5, &actPosUtr);

    }


  }

}


//===========================================================================
  int APT_PP_Write_Lin (Point *pt1) {
//===========================================================================


  if(strlen(WC_outBuf) > 0) strcat (WC_outBuf," ");


  strcat (WC_outBuf,"G1 ");


  UTX_add_fl_u2 (WC_outBuf, pt1->x, pt1->y);


  return 1;


}





//===========================================================================
  int APT_PP_Write_Cir (Circ* ci1) {
//===========================================================================


  if(strlen(WC_outBuf) > 0) strcat (WC_outBuf," ");


  if(ci1->rad < 0.0) {
    strcat (WC_outBuf,"G2 ");
  } else {
    strcat (WC_outBuf,"G3 ");
  }

  UTX_add_fl_u2 (WC_outBuf, ci1->p2.x,
                            ci1->p2.y);
  strcat (WC_outBuf,",");
  UTX_add_fl_u2 (WC_outBuf, ci1->pc.x,
                            ci1->pc.y);

 return 0;

}


//================================================================
  int APT_PP_Write () {
//================================================================
// fertige Zeile OutBuf in das PP-File raus.


  // int    i1;



  if (WC_mode == 0) goto Fertig;

  ++fpOutAnz;



  // printf ("APT_PP_Write:|%s|\n",WC_outBuf);
  // printf ("  UP_mode=%d  res=%d act_typ=%c\n",UP_mode,UP_resolv,UP_act_typ);


  // Beim ersten Mal sollte ein UP schon ausgegeben werden.


  // Nur im Main oder wenn GEO-Up das erste mal l�uft was ausgeben
  if((UP_mode == OFF)||(UP_act_typ == 'M')||(UP_resolv  == ON)) {
     //((UP_resolv == ON)&&(UP_act_typ == 'G'))) {

    //TX_Print ("APT_PP_Write out:|%s|\n",WC_outBuf);
///
//     if(fpOutAnz > 19) {
//       if(l_irc != 25) {
//         goto Fertig;
//       }
//     }
///
    goto Ausgeben;

  }


  Fertig:
  return 0;



  //===============================================================
  Ausgeben:
  //TX_Print(" ausg0 /%s/  ins=%d pre=%d",WC_outBuf,insCnt[0],preBuf[0]);
  //TX_Print(" ausg1 /%s/  ins=%d pre=%d",WC_outBuf,insCnt[1],preBuf[1]);
  



  // L_Ausg2:
  return APT_PP_WriOut (WC_outBuf);

  
}



//============================================================================
  int APT_PP_WriOut (char *txOut) {
//============================================================================

  fprintf (APT_fpOut,"%s\n",txOut);

  return 1;


}

//============================================================================
  int APT_ausg_chTool (char* buf) {
//============================================================================
// Werkzeugwechsel
// Unterschied Interaktiv / Postproz:
//  Bei PP wird chTool erst mit unt/ob Element gerufen, bei Interakt schon mit
//  dem (frueher fertigen) unt. Elem.





  //TX_Print("APT_ausg_chTool /%s/  %f,%f",buf,o1->p1.x,o1->p1.y);


///
//   // hier ist der richtige Zeitpunkt fuer bestimmte NCCommands:
//   if(!strcmp(buf, "WC")) {
//     APT_tl_stat  = OFF;
//     //TX_Print(" ><><><><> WC %d",APT_tl_stat);
//     goto L_chTool;
// 
//   } else if (!strcmp(buf, "WT")) {
//     APT_tl_stat  = ON;
//     //TX_Print(" ><><><><> WT");
//     goto L_chTool;
// 
//   } else 
///

  if (!strcmp(buf, "RP")) {
    NC_rapid   = ON;
    //TX_Print(" ><><><><> RP");
    goto L_chTool;
  }

  return 0;




  //=========== PP-Ausgabe ==================================================
  L_chTool:
  if (WC_mode == 0) goto L_Interakt;




  return 1;






  //=================================================
  L_Interakt:


      //TX_Print("GO %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
  if(APT_tl_act != APT_tl_stat) {

    // Symbol abschneiden
    if(APT_tl_stat != 0) {
      //TX_Print("SYM_SQUARE auf %f,%f",o1->p1.x,o1->p1.y);
      APT_disp_SymV (SYM_SQUARE, 0, &actPosUtr, 0.5);
    }

    // Symbol einf�deln
    if(APT_tl_stat == 0) {
      //TX_Print("SYM_CROSS1 auf %f,%f",o1->p1.x,o1->p1.y);
      APT_disp_SymV (SYM_CROSS1, 0, &actPosUtr, 0.5);          // Schere
    }

    APT_tl_act = APT_tl_stat;
  }

  return 1;

}




//============================================================================
  int APT_ausg_NCCmd (int mode) {
//============================================================================
// einen zwischengespeicherten NC-Befehl raus.
// Ausgegeben wird Obj NC_ausgAU

static  int    oldLen;
static  Point2 oldPos;

  int    i1;
  Point  pt1;



  if(mode < 0) {
    oldLen = 0;
    oldPos.x = UT_VAL_MAX;
    return 0;
  }


  // printf("APT_ausg_NCCmd anz=%d %d\n",insAnz,insCnt[0]);

  

  if(insAnz < 1) return 0;


  //WC_outBuf[0] = '\0';
  strcpy(WC_outBuf, " ");

  //TX_Print("APT_ausg_NCCmd /%s/ %f %f",insBuf[0],o1->p1.x,o1->p1.y);




  //=========== PP-Ausgabe ==================================================
  if (WC_mode == 0) goto L_Interakt;
  L_Ausgeben:
  if(insAnz < 1) return 0;


  // wenn was im Insertbuffer - raus ?
  // if(insCnt[0] > o1->nam) return 0;

    //TX_Print(" pp ausg /%s/",insBuf[0]);

    // hier ist der richtige Zeitpunkt fuer bestimmte NCCommands:
    APT_ausg_chTool (insBuf[0]);

    // Nur im Main oder wenn GEO-Up das erste mal l�uft was ausgeben
    if((UP_mode == OFF)||(UP_act_typ == 'M')||(UP_resolv == 0)) {
      fprintf(APT_fpOut,"%s\n",insBuf[0]);
    }
    // done, raus aus dem Buffer.
    for(i1=1; i1<insAnz; ++i1) {
      insCnt[i1-1] = insCnt[i1];
      insSym[i1-1] = insSym[i1];
      strcpy(insBuf[i1-1],insBuf[i1]);
    }
    --insAnz;
    goto L_Ausgeben;

  return 1;




  //=================================================
  L_Interakt:

  // wenn was im Insertbuffer - raus ?
  // if(insCnt[0] > o1->nam)   goto L_IntRaus;

    //TX_Print(" int ausg %d /%s/",insSym[0],insBuf[0]);

    // hier ist der richtige Zeitpunkt fuer bestimmte NCCommands:
    APT_ausg_chTool (insBuf[0]);


    // Symbol hinmalen
    if(insSym[0] > 0) {
      // APT_disp_SymV (insSym[0], 0, &o1->p1, 0.6);
      APT_disp_SymV (insSym[0], 0, &actPosUtr, 0.5);
    }

    // den Text hinmalen
    // war der vorherige Text auch an der gleichen Posi ?
    // printf("oldPos=%f %f oldLen=%d\n",oldPos.x,oldPos.y,oldLen);
    if(UT2D_comp2pt(&oldPos, &actPosUtr, UT_TOL_pt)) {
      for(i1=0; i1<oldLen-1; ++i1) {
        strcat(WC_outBuf, " ");
      }
      strcat(WC_outBuf, ";");
      strcat(WC_outBuf, insBuf[0]);
    } else {
      strcat(WC_outBuf, insBuf[0]);
    }
    oldLen = strlen(WC_outBuf);


    //pt1 = UT3D_pt_pt2(&actPosUtr);
    // pt1 = UT3D_pt_pt2(&o1->p1);   // Startpunkt fuer zB STOP !
    pt1 = UT3D_pt_pt2(&actPosUtr);   // Startpunkt fuer zB STOP !
    pt1.z = WC_sur_Z;


    // done, aus dem Buffer rausloeschen.
    for(i1=1; i1<insAnz; ++i1) {
      insCnt[i1-1] = insCnt[i1];
      insSym[i1-1] = insSym[i1];
      strcpy(insBuf[i1-1],insBuf[i1]);
    }
    --insAnz;
    if(insAnz > 0) {
      strcat(WC_outBuf,";");
      goto L_Interakt;
    }


  L_IntRaus:
  if(APT_dispNCCmd == ON) {
    if(strlen(WC_outBuf) > 0)  APT_disp_TxtA (0, &pt1, WC_outBuf);
  }

  oldPos = actPosUtr;

  return 1;
}
*/




/*===========================================================================*/
   double APT_transl_rad (double rad_in) {
/*=======================
Wenn gespiegelt - Drehrichtung umdrehen
*/

  //double rad_out;

  if(UP_mode == ON) {
    if((UpMirX == ON)||(UpMirY == ON)) {
      return -rad_in;
    }
  }
  return rad_in;
}


/*
//===========================================================================
  int WC_ask_geo () {
//===========================================================================
// Ist UP aktiv, zB "M" = Mac, kann trotzden UP aktiv sein, wenn n�mlich GEO MAC ruft.


  int mode, i1;


  mode = OFF;



  if(UP_mode == ON) {

    TX_Print ("WC_ask_geo UP_mode=%d UP_resolv=%d Geo_level=%d UP_act_typ=/%c/",
               UP_mode,UP_resolv,Geo_level,UP_act_typ);

    if(Geo_level >= 0) {
      for(i1=0; i1==Geo_level; ++i1) {
        TX_Print(" Geo_level - %d /%c/",i1,Geo_level_UP_typ[Geo_level]);
      }
      mode = ON;
    }






  }

  return mode;

}
*/



//=======================================================================
  int APT_transform (Point *pt1) {
//=======================================================================

  // printf("APT_transform\n");





  //==============================================================
  if(APT_tra) {

    printf("APT_transform _tra=%d\n",APT_tra);
    // printf(" pt1=%f,%f,%f\n",pt1->x,pt1->y,pt1->z);

    UT3D_pt_traptm3 (pt1, APT_mat_tra, pt1);




  //==============================================================
  } else if(APT_proj) {

    printf("APT_transform _proj=%d\n",APT_proj);





  //==============================================================
  } else if(APT_mirr) {

    printf("APT_transform _mirr=%d\n",APT_mirr);


  }




  return 0;

}


//=======================================================================
  Point2 APT_transl2 (Point2* ptin) {
//=======================================================================
/*
Relative Koord auf absolute Koord. umwandeln.

WC_mode     0 = Edit, > 0 = PP#
UP_mode     0=ON sind im UP,  1=OFF sind im Main
UP_act_typ  G=GEO,  M=MAC.
UP_resolv   1=ON aufloesen, 1=OFF schon mal erledigt.



RefOri nicht aktiv
*/

  Point2   ptout, ptaux1;


  // printf("transl2 %f,%f UP_act_typ=%c\n",ptin->x,ptin->y,UP_act_typ);

/*
  printf("   WC_mode=%d UP_mode=%d UP_level=%d UP_resolv=%d\n",
             WC_mode,UP_mode,UP_level,UP_resolv);
  printf("   UpOri= %f %f\n",UpOri.x,UpOri.y);
  //TX_Print("   ref= %f %f\n",RefOri.x,RefOri.y);
  //TX_Print("   ang= %f %f %f\n",RefOriAng,RefOriAng_sin,RefOriAng_cos);
  printf(" up %d %d %d %f\n",UP_mode,UpMirX,UpMirY,UpOriAng);
*/



/*
  if (WC_mode != 0) {             // nur beim PP-Lauf
    // In einem aufzul�senden UP muessen die untranslierten Koordinaten raus,
    // im Main immer die translierten.
    if((UP_mode == ON)&&(UP_resolv == 0)) {
      //TX_Print ("APT_transl2 f. PP %f,%f",ptin->x,ptin->y);
      //TX_Print("Nix transl");
      return *ptin;
    }
  }
*/


  // if((UP_mode == ON)&&(UP_level >= 0)) {   // letzte Version m Geo_ war so.

  // nur wenn im UP und wenns GEO ist
  if((UP_mode == ON)&&(UP_act_typ == 'G')) {

    if(UpMirX == ON) {
      //printf("XMIR\n");
      ptout.x = ptin->x - UpFrom.x;
      ptout.y = ptin->y - UpFrom.y;

      ptout.x = ptout.x;
      ptout.y = -ptout.y;

      ptout.x = ptout.x + UpFrom.x;
      ptout.y = ptout.y + UpFrom.y;



    } else if (UpMirY == ON) {
      //printf("YMIR\n");
      ptout.x = ptin->x - UpFrom.x;
      ptout.y = ptin->y - UpFrom.y;

      ptout.x = -ptout.x;
      ptout.y = ptout.y;

      ptout.x = ptout.x + UpFrom.x;
      ptout.y = ptout.y + UpFrom.y;



    } else {
      ptout.x = ptin->x;
      ptout.y = ptin->y;

    }


    // UP-Mode: drehen
    if(!UTP_comp2db(UpOriAng,0.0,UT_TOL_pt)) {
      // printf("   UpRot %f %f %f %f",ptout.x,ptout.y,UpOri.x,UpOri.y);

      // auf den Drehpunkt umrechnen
      ptout.x = ptout.x - UpFrom.x;
      ptout.y = ptout.y - UpFrom.y;

      ptaux1.x = ptout.x * UpOriAng_cos - ptout.y * UpOriAng_sin;
      ptaux1.y = ptout.x * UpOriAng_sin + ptout.y * UpOriAng_cos;

      ptout.x  = ptaux1.x + UpOri.x + UpFrom.x;
      ptout.y  = ptaux1.y + UpOri.y + UpFrom.y;





    } else {

      // printf("   transl in UP to %f %f\n",UpOri.x,UpOri.y);
      ptout.x = ptout.x + UpOri.x;
      ptout.y = ptout.y + UpOri.y;

    }





  //---------------------------------------------
  } else {

    // printf("   kein transl\n" );
    ptout.x = ptin->x;
    ptout.y = ptin->y;

  }




/*
  if(!UTP_comp2db(RefOriAng,0.0,UT_TOL_pt)) {
    ptout.x = RefOri.x + (ptout.x * RefOriAng_cos - ptout.y * RefOriAng_sin);
    ptout.y = RefOri.y + (ptout.x * RefOriAng_sin + ptout.y * RefOriAng_cos);


  } else {
    ptout.x = ptout.x + RefOri.x;
    ptout.y = ptout.y + RefOri.y;

  }


  return ptout;
*/




  // printf(" ex transl2 %f,%f < %f,%f\n",ptout.x,ptout.y,ptin->x,ptin->y);


  return ptout;

}





/*===========================================================================*/
  Point2 APT_rotate3 (Point* ptin) {
/*==================
Wird derzeit nicht benutzt!
*/

  Point2  ptout, ptaux, ptaux1;


  // TX_Print("APT_rotate3  %f %f\n",RefOri.x,RefOri.y);
  // TX_Print("APT_rotate3 %f %f",RefOriAng_sin,RefOriAng_cos);
  //printf(" up %d %d %f\n",UpMirX,UpMirY,UpOriAng);


  if(UP_mode == ON) {

    if(UpMirX == ON) {
      //printf("XMIR\n");
      ptaux.x = ptin->x;
      ptaux.y = -ptin->y;


    } else if (UpMirY == ON) {
      //printf("YMIR\n");
      ptaux.x = -ptin->x;
      ptaux.y = ptin->y;



    } else {
      ptaux.x = ptin->x;
      ptaux.y = ptin->y;

    }


    // UP-Mode: drehen
    if(!UTP_comp2db(UpOriAng,0.0,UT_TOL_pt)) {
      //printf("UpRot\n");
      ptaux1.x = ptaux.x * UpOriAng_cos - ptaux.y * UpOriAng_sin;
      ptaux1.y = ptaux.x * UpOriAng_sin + ptaux.y * UpOriAng_cos;
      ptaux.x  = ptaux1.x;
      ptaux.y  = ptaux1.y;

    }





  //---------------------------------------------
  } else {

    ptaux.x = ptin->x;
    ptaux.y = ptin->y;

  }




  if(!UTP_comp2db(RefOriAng,0.0,UT_TOL_pt)) {
    ptout.x = ptaux.x * RefOriAng_cos - ptaux.y * RefOriAng_sin;
    ptout.y = ptaux.x * RefOriAng_sin + ptaux.y * RefOriAng_cos;


  } else {
    ptout.x = ptaux.x;
    ptout.y = ptaux.y;

  }

  return ptout;

}


/*
//====================================================================
  int APT_alloc1 (long Ind) {
//====================================================================


  long l1, newSiz;


  if(Ind+10 < NC_stat_SIZ) return 0;

  newSiz = APT_ind_INC;

  while (Ind >= newSiz) newSiz += APT_ind_INC;
  // printf("  %d %d %d\n",Ind,impTabSiz,newSiz);

  // printf("::::APT_alloc1 %d %d %d %d\n",Ind,NC_stat_SIZ,newSiz,APT_ind_INC);

  // obj_ind_tab = (long*)realloc(obj_ind_tab, newSiz*sizeof(long));
  // actPosU_tab = (Point2*)realloc(actPosU_tab, newSiz*sizeof(Point2));
  NC_stat__ = (NC_recTyp*)realloc(NC_stat__, newSiz*sizeof(NC_recTyp));

  if(NC_stat__ == NULL) {
    TX_Error ("******** out of memory - APT_alloc1 *********");
    return -1;
  }

  L_init:
  // for(l1=NC_stat_SIZ; l1<newSiz; ++l1) NC_stat__[l1].lNr = -1;

  NC_stat_SIZ = newSiz;

  // printf("ex IGE_r_allocD %d %d %d\n",Ind,impTabSiz,newSiz);

  return 0;



}
*/

/*
dzt unused
//====================================================================
  int APT_disp_tempObj (ObjG2* obj1, int attr, double zval) {
//====================================================================

  Line      ln1;
  long      ind;


  printf("APT_disp_tempObj %d\n",obj1->typ);


  if(obj1->typ == Typ_PT2) {
    // Punkt darf nur bei Nullwegen kommen; nix tun.
    //TX_Print("PT = Nullweg");


  } else if(obj1->typ == Typ_LN2) {

    //TX_Print("GO %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
    // APT_disp_ln (&obj1->p1, &obj1->p2, attr, zval, zval);

    ln1.p1   = UT3D_pt_pt2 (&obj1->p1);
    ln1.p1.z = zval;

    ln1.p2   = UT3D_pt_pt2 (&obj1->p2);
    ln1.p2.z = zval;

    ind = -1;
    GR_DrawLine (&ind, attr, &ln1);



  } else if(obj1->typ == Typ_CI2) {
    //TX_Print("GO %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
    //TX_Print(" APT_GR_cut AC2: rad=%f", objU->rad);
    // APT_disp_ac (obj1, attr, zval);




  } else if(obj1->typ == Typ_CVPOL2) {
    //TX_Print("GO %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
    //TX_Print(" APT_GR_cut AC2: rad=%f", objU->rad);
    // APT_disp_cv (obj1, attr, zval);




  } else {
    TX_Error("**** Error - APT_disp_tempObj unbekannter Typ: %d\n",obj1->typ);
    return -1;

  }

  return 0;


}
*/


/*
//================================================================
  int APT_disp_obj (ObjG2* obj1, int attr, double zval) {
//================================================================

  //Line      ln1;
  //long      apt_ind;


  // int       i_obj;

  // printf("APT_disp_obj %d\n",obj1->typ);


    if(obj1->typ == Typ_PT2) {
      // Punkt darf nur bei Nullwegen kommen; nix tun.
      //TX_Print("PT = Nullweg");


    } else if((obj1->typ == Typ_LN2)||(obj1->typ == Typ_LN)) {

      // printf("ln %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
      APT_disp_ln (&obj1->p1, &obj1->p2, attr, zval, zval);

 
    } else if ((obj1->typ == Typ_CI2)||(obj1->typ == Typ_CI)) {
      // printf("Ci %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
      // printf(" APT_GR_cut AC2: rad=%f", objU->rad);
      APT_disp_ac (obj1, attr, zval);



 
    } else if(obj1->typ == Typ_CVPOL2) {
      //TX_Print("GO %f %f\n",pt_tab[defInd].x,pt_tab[defInd].y);
      //TX_Print(" APT_GR_cut AC2: rad=%f", objU->rad);
      APT_disp_cv (obj1, attr, zval);



 
    } else {
      TX_Error("**** Error - APT_disp_obj unbekannter Typ: %d\n",obj1->typ);
      return -1;

    }





  return 0;

}
*/

//===========================================================================
  int APT_DrawDim3 (int typ, long apt_ind, ObjGX *dim3) {
//===========================================================================

  if(APT_Stat_Draw == OFF) return 0;

  AP_dli_act = DL_StoreObj (Typ_GTXT, apt_ind, typ);

  GL_DrawDim3 (&AP_dli_act, typ, dim3->data);


  return 0;

}


//===========================================================================
  void APT_DrawDimen (int typ, long apt_ind, ObjGX *dim1) {
//===========================================================================

  if(APT_Stat_Draw == OFF) return;

  AP_dli_act = DL_StoreObj (Typ_Dimen, apt_ind, typ);

  GR_DrawDimen (&AP_dli_act, typ, dim1->data);


  return;

}


//===========================================================================
  void APT_DrawTxtG (int iAtt, long apt_ind, GText *tx1) {
//===========================================================================


  // printf("APT_DrawTxtG iAtt=%d apt_ind=%ld |%s|\n",iAtt,apt_ind,tx1->txt);


  if(APT_Stat_Draw == OFF) return;

  AP_dli_act = DL_StoreObj (Typ_GTXT, apt_ind, iAtt);

  // GR_DrawTxtG (&AP_dli_act, typ, &tx1->pt, tx1->size, tx1->dir, tx1->txt);
  GR_DrawTxtG (&AP_dli_act, iAtt, tx1, apt_ind);

}


//===========================================================================
  int APT_DrawTxtA (int typ, long apt_ind, AText *tx1) {
//===========================================================================
// LDRP
// TODO: use Typ_Bitmap for Typ_ATXT & Typ_Tag

  int    irc, i1;
  double scl;
  Point  *pt1 = NULL;
  char   cBuf[256];


  // UT3D_stru_dump (Typ_ATXT, tx1, "APT_DrawTxtA");
  // printf("  typ=%d ind=%ld\n",typ,apt_ind);


  if(APT_Stat_Draw == OFF) return 0;


  AP_dli_act = DL_StoreObj (Typ_Tag, apt_ind, typ);

  if(tx1->aTyp == 1) {   // Image-BMP
    // // den symbol. Pfad aufloesen.
    // // Mod_get_path (cBuf, tx1->txt);
    // // printf(" cBuf=|%s| scl=%f\n",cBuf,tx1->scl);
    // i1  = tx1->ltyp; // LineTyp
    // scl = tx1->scl;  // direkt crash in Linux !
    // // irc = GL_Draw_BMP (&AP_dli_act, &tx1->p1, &tx1->p2, i1, scl, cBuf);
    // // return GL_Draw_BMP (&AP_dli_act, &tx1->p1, &tx1->p2, i1, scl, tx1->txt);
    return GL_Draw_BMP (&AP_dli_act, tx1, apt_ind);


  } else if(tx1->aTyp == 3) {   // LeaderLine + Balloon + 3D-Text  LDRC
    GL_DrawTxtLBG (&AP_dli_act, tx1, apt_ind);


  } else if(tx1->aTyp == 4) {   // LeaderLine + 3D-Text   LDRP
    // GL_DrawTxtLG (&AP_dli_act, typ, &tx1->p1, &tx1->p2, tx1->txt);
    GL_DrawTxtLG (&AP_dli_act, tx1, apt_ind);


  } else if(tx1->aTyp > 4) {   // symTyp > 4 sind Symbols
    GL_DrawTxtsym (&AP_dli_act, tx1->aTyp - 5, &tx1->p1, (Vector*)&tx1->p2,
                   tx1->col, tx1->scl);
    if(APT_dispNam) APT_disp_nam (Typ_ATXT, apt_ind, (void*)tx1);

  // } else if(tx1->typ == 2) {   // Block-tag
  } else {   // typ = 2 = Block-tag; 0=normale A-text
    GL_Draw_Tag (&AP_dli_act, tx1, apt_ind);
    // GL_Draw_Tag (&AP_dli_act, &tx1->p1, &tx1->p2, tx1->txt,
                 // tx1->typ, tx1->col, tx1->ltyp);

  // } else {
    // GR_DrawTxtA (&AP_dli_act, typ, &tx1->p1, tx1->txt);
  }

  return 0;

}

/*
//================================================================
  int APT_LoadTex (long apt_ind, ObjGX *otx) {
//================================================================
// load texture

 
  // Texture   *tex;
  // char      cBuf[256];


  printf("APT_LoadTex %d\n",apt_ind);
  UT3D_stru_dump (Typ_ObjGX, otx, "_LoadTex:");

  // tex = otx->data;

  AP_dli_act = DL_StoreObj (Typ_TEXB, apt_ind, 0);

  GL_LoadTex (&AP_dli_act, otx->data);

  return 0;

}
*/

//===========================================================================
  int APT_DrawSol (int iatt, long apt_ind) {
//===========================================================================

  int     i1;
  ObjGX   *bd1;


  // printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
  // printf("APT_DrawSol %d\n",apt_ind);


  if(APT_Stat_Draw == OFF) return 0;


  // do not export hidden objets for Mockups ..
  if(TSU_mode == 1) {
    i1 = GA_hide__ (8, apt_ind, Typ_SOL); // ask state; 1=hidden
    // printf(" hide=%d\n",i1);
    if(i1 == 1) return 0;
  }


  bd1 = DB_GetSol (apt_ind);
  // in bd1.oiTab nun Typen und Indexe der APT-Objekte.
  if(bd1->typ == Typ_Error) return -1;


  if(TSU_mode == 0)  // nur draw OpenGL
  AP_dli_act = DL_StoreObj (Typ_SOL, apt_ind, iatt);


  GR_CreSol__ (&AP_dli_act, iatt, bd1);


  return 0;

}





//===========================================================================
  int APT_DrawPln (int typ, long dbi, Plane *pl1) {
//===========================================================================
// display symbolic plane (rectangle)


  if(APT_Stat_Draw == OFF) return 0;


  AP_dli_act = DL_StoreObj (Typ_PLN, dbi, typ);

  GR_DrawPlane (&AP_dli_act, typ, pl1);

  if(APT_dispNam) APT_disp_nam (Typ_PLN, dbi, pl1);

  return 0;

}



//===========================================================================
  int APT_DrawSur (int att, long apt_ind) {
//===========================================================================

  ObjGX     *ox1;


  // printf("APT_DrawSur A%ld att=%d\n",apt_ind,att);
  // printf(" DL_ind_act=%ld\n",DL_ind_act);
  

  if(APT_Stat_Draw == OFF) return 0;

  ox1 = DB_GetSur (apt_ind, 1);
    // UTO_dump_s_ (ox1, "APT_DrawSur\n");

  return GR_DrawSur (ox1, att, apt_ind);

}


//===========================================================================
  int APT_DrawModel (int typ, long apt_ind, ModelRef *mod1) {
//===========================================================================


  // printf("APT_DrawModel %d %d\n",typ,apt_ind);

  // test: display
  // Mod_test1();


  if(APT_Stat_Draw == OFF) return 0;

  return GR_DrawModel (apt_ind, typ, mod1);

}


/*
//===========================================================================
  void APT_disp_TxtG (int typ,Point* pt1,float size,float angle,char* txt) {
//===========================================================================
// GrafText hinmalen
// typ = 0


  long      apt_ind;



  if(APT_Stat_Draw == OFF) return;

  apt_ind = DB_StorePoint (-1L, pt1);

  // Achtung! Als Attribut wird der Symboltyp gespeichert !!!
  AP_dli_act = DL_StoreObj (Typ_GTXT, apt_ind, typ);

  // printf("APT_disp_TxtG typ=%d ind=%ld %f,%f\n",typ,apt_ind,pt1->x,pt1->y);


  GR_DrawTxtG (&AP_dli_act, typ, pt1, size, angle, txt);

}
*/


//===========================================================================
  void APT_disp_TxtA (int typ, Point* pt1, char* txt) {
//===========================================================================
/// \code
/// display AlfaText
///   att     colorIndex; eg Typ_Att_def; see INF_ATT_CV
/// \endcode


  long      apt_ind;



  if(APT_Stat_Draw == OFF) return;

//  p1 = UT3D_pt_pt2 (pt1);

  apt_ind = DB_StorePoint (-1L, pt1);

  // Achtung! Als Attribut wird der Symboltyp gespeichert !!!
  AP_dli_act = DL_StoreObj (Typ_ATXT, apt_ind, typ);

  //TX_Print("APT_disp_Sym typ=%d ind=%d %f,%f",typ,apt_ind,p1.x,p1.y);


  GR_DrawTxtA (&AP_dli_act, typ, pt1, txt);
  // GL_DrawTxtA (&AP_dli_act, typ, pt1, txt);

}


//===========================================================================
  int APT_disp_SymB2 (int symTyp, int att, Point2* pt1) {
//===========================================================================
/// \code
/// APT_disp_SymB          disp temp bitmap symbols SYM_TRI_S SYM_STAR_S ..
/// Input:
///   symTyp   SYM_TRI_S|SYM_STAR_S|SYM_CIR_S|SYM_SQU_B
///   att      see INF_COL_SYMB
/// see also GL_DrawSymB
/// \endcode

  Point   pt3;

  pt3 = UT3D_pt_pt2 (pt1);

  return APT_disp_SymB (symTyp, att, &pt3);

}


//===========================================================================
  int APT_DrawSymB (int iatt, long dbi, int symTyp, int att, Point* pt1) {
//===========================================================================
/// \code
/// APT_disp_SymB          disp temp bitmap symbols SYM_TRI_S SYM_STAR_S ..
/// Input:
///   symTyp   SYM_TRI_S|SYM_STAR_S|SYM_CIR_S|SYM_SQU_B
///   att      see INF_COL_SYMB
/// see also GL_DrawSymB
/// \endcode

  long    l1;

  // UT3D_stru_dump (Typ_PT, pt1, "APT_disp_SymB: ");

  if(APT_Stat_Draw == OFF) return 0;      // OFF=1

  l1 = DL_StoreObj (Typ_SymB, dbi, iatt);
    // printf(" _DrawSymB dbi=%ld l1=%ld\n",dbi,l1);

          
  GL_DrawSymB (&l1, iatt, symTyp, pt1); // att=1=sw, 2=rot, 4=bl,

  return 0;

}


//===========================================================================
  int APT_disp_SymB (int symTyp, int att, Point* pt1) {
//===========================================================================
/// \code
/// APT_disp_SymB          disp temp bitmap symbols SYM_TRI_S SYM_STAR_S ..
/// Input:
///   symTyp   SYM_TRI_S|SYM_STAR_S|SYM_CIR_S|SYM_SQU_B
///   att      see INF_COL_SYMB
/// see also GL_DrawSymB
/// \endcode

  long    l1;

  // UT3D_stru_dump (Typ_PT, pt1, "APT_disp_SymB: ");

  l1 = DL_StoreObj (Typ_SymB, -1L, 0);

  GL_DrawSymB (&l1, att, symTyp, pt1); // att=1=sw, 2=rot, 4=bl,

  return 0;

}


//===========================================================================
  void APT_disp_SymV (int typ, int att, Point2* pt1, double scale) {
//===========================================================================
/// APT_disp_SymV     display symbols at 2D-pos (not rotated symbols)

  Point     p1;


  // printf("APT_disp_SymV %d %d %f,%f %f\n",typ,att,pt1->x,pt1->y,scale);

  if(APT_Stat_Draw == OFF) return;

  p1   = UT3D_pt_pt2 (pt1);
  p1.z = WC_sur_Z;

  APT_disp_SymV1 (typ, att, &p1, scale);


}


//================================================================
  void APT_disp_SymV1 (int typ, int att, Point* p1, double scale) {
//================================================================
/// \code
/// APT_disp_SymV1    display symbols in x-y-plane (not rotated symbols)
///   typ:    symtyp; SYM_TRIANG, SYM_AXIS, SYM_SQUARE, SYM_CROSS
///   att:    symtyp; SYM_TRIANG, SYM_AXIS, SYM_SQUARE, SYM_CROSS
/// oriented symbols in x-y-plane: see APT_disp_SymV2
/// \endcode


  if(APT_Stat_Draw == OFF) return;

  AP_dli_act = DL_StoreObj (Typ_SymV, -1L, typ);

  // printf("APT_disp_SymV1 %d %d %f,%f%f\n",typ,AP_dli_act,p1->x,p1->y,p1->z);

  GL_DrawSymV (&AP_dli_act, typ, att, p1, scale);

}


//================================================================
  void APT_disp_SymV2 (int typ, int att, Point* pt1, Point* pt2, double scale) {
//================================================================
/// \code
/// APT_disp_SymV2    display oriented symbols in x-y-plane; eg SYM_ARROH
/// Input:
///   typ  SYM_ARROH (arrowhead)
///   att  Typ_Att_Symb
///   pt1  position Head
///   pt2  direction
/// \endcode

  // Point     p1, p2;
  //long      apt_i1, apt_i2;


  if(APT_Stat_Draw == OFF) return;

  //p1 = UT3D_pt_pt2 (pt1);
  //p2 = UT3D_pt_pt2 (pt2);

  //apt_i1 = DB_StorePoint (-1L, &p1);
  //apt_i2 = DB_StorePoint (-1L, &p2);

  // Achtung! Als Attribut wird der Symboltyp gespeichert !!!
  AP_dli_act = DL_StoreObj (Typ_SymV, -1L, typ);

  //TX_Print("APT_disp_Sym typ=%d ind=%d %f,%f",typ,apt_ind,p1.x,p1.y);

  GL_DrawSymV2 (&AP_dli_act, typ, att, pt1, pt2, scale);


}


/* UNUSED
//================================================================
  void APT_disp_Vec (int att, long dli, Point* pt1, Vector* vc1) {
//================================================================
// Vektor-Symbole darstellen.

  if(APT_Stat_Draw == OFF) return;

  // Achtung! Als Attribut wird der Symboltyp gespeichert !!!
  // AP_dli_act = DL_StoreObj (Typ_SymV, 0L, typ);

  // AP_dli_act = DL_StoreObj (Typ_VC, -1L, att);
  AP_dli_act = DL_StoreObj (Typ_VC, dli, att);

  //TX_Print("APT_disp_Sym typ=%d ind=%d %f,%f",typ,apt_ind,p1.x,p1.y);

  // GR_Draw_vc (&AP_dli_act, vc1, pt1, att, 0);

  if(att == 2) {
    GL_DrawVec (&AP_dli_act, att, pt1, vc1);
  } else {
    GL_DrawSymV3 (&AP_dli_act, SYM_ARROW, att, pt1, vc1, 10.);
  }

}
*/


//============================================================================
  void APT_disp_SymV3 (int typ, int att, Point* pt1, Vector* vc1, double scale) {
//============================================================================
/// \code
/// APT_disp_SymV3     disp temp oriented vector-symbols SYM_ARROW SYM_SQUARE ..
/// see GL_DrawSymV3
/// \endcode


  if(APT_Stat_Draw == OFF) return;

  printf("APT_disp_SymV3 typ=%d %f,%f,%f\n",typ,pt1->x,pt1->y,pt1->z);
  UT3D_stru_dump (Typ_VC, vc1, "  vc1:");

  // Achtung! Als Attribut wird der Symboltyp gespeichert !!!
  // AP_dli_act = DL_StoreObj (Typ_SymV, (long)0, typ);

  AP_dli_act = DL_StoreObj (Typ_SymV, -1L, att);

  //TX_Print("APT_disp_Sym typ=%d ind=%d %f,%f",typ,apt_ind,p1.x,p1.y);

  GL_DrawSymV3 (&AP_dli_act, typ, att, pt1, vc1, scale);


}



/*===========================================================================*/
  void APT_disp_ln (Point2* pt1, Point2* pt2, int attInd,
                                              double zval1,double zval2) {
/*================
Display Line von pt1 > pt2
Es ist (fuer's Display) eine transformierte Line erforderlich.
Bei Typ_LN ist zum nachfolgenden Schnittpunktberechnen usw
  ein untransformiertes Objekt zu merken.
*/
  Line      ln1;
  long      apt_ind;


  ln1.p1   = UT3D_pt_pt2 (pt1);
  ln1.p1.z = zval1;

  ln1.p2 = UT3D_pt_pt2 (pt2);
  ln1.p2.z = zval2;

  apt_ind = DB_StoreLine ((long)-1, &ln1);


  APT_DrawLine (attInd, apt_ind, &ln1);


}



/*===========================================================================*/
  void APT_disp_ac (ObjG2 *objU, int attInd, double zval) {
/*================
*/
  Circ      ci1;
  long      apt_ind;



  // printf("APT_disp_ac; rad=%f\n",objU->rad);
  // printf("       p1=%f,%f\n",objU->p1.x,objU->p1.y);
  // printf("       p2=%f,%f\n",objU->p2.x,objU->p2.y);



  ci1 = UT3D_ci_obj2 (objU);
  ci1.p1.z = zval;
  ci1.p2.z = zval;
  ci1.pc.z = zval;

  apt_ind = DB_StoreCirc ((long)-1, &ci1);

  APT_DrawCirc (attInd, apt_ind, &ci1);


}


/* UU
//===========================================================================
  void APT_disp_cv (ObjG2 *objU, int attInd, double zval) {
//===========================================================================

  Curv      cv1;
  long      apt_ind;


  // printf("APT_disp_cv: Nr=%d ind=%d %d",(int)objU->rad,
             // (int)objU->pc.x,(int)objU->pc.y);
  //TX_Print("APT_disp_cv: z=%f",zval);

  cv1 = UT3D_cv_obj2 (objU);

  // dynam. Curv erzeugen
  // apt_ind = DB_StoreCurv ((long)-1, &cv1, 0);
  //cv1.p?.z = zval;    OFFEN !!

  // printf("YYYYYYYYYYYYYYYYY APT_DrawCurv YYYYYYYYYYY\n");
  // APT_DrawCurv (attInd, apt_ind, &cv1, zval);


}
*/

//================================================================
  int APT_disp_dir (Point *p1, Point *p2) {
//================================================================
/// \code
/// APT_disp_dir         display obj-direction with arrow
/// see also GL_Draw_cvp_dir
/// \endcode

// TODO: do not display Arrow for hidden or parent-objects;
// Problem: wird erst viel sp�ter auf hidden gesetzt !!!!
// ABHILFE: 
//  -) statt reRun loop tru DL and display direction only for not hidden obj's



  int    att = Typ_Att_Symb;
  Vector vc1;
  long   dli;


  UT3D_vc_2pt (&vc1, p1, p2);
     // GR_Disp_pt (&pTab[ipe], SYM_TRI_S, ATT_COL_RED);


  // if temp-mode: use dli = -9  else get new DL-record
  if(APT_obj_stat) {
    // 1=temporary (workmode)
    dli = DLI_DIR_TMP;
  } else {
    // get new DL-record
    dli = DL_StoreObj (Typ_SymV, -1L, att);
  }

  GL_DrawSymV3 (&dli, SYM_ARRO3H, att, p1, &vc1, 1.);
    
  // APT_disp_SymV3 (SYM_ARRO3H, att, p1, &vc1, 1.);
  // APT_disp_SymV2 (SYM_ARROH, Typ_Att_Symb, &ln1->p2, &ln1->p1, 1.0);


  return 0;

}

 
//===========================================================================
  void APT_disp_nam (int typ, long ind, void* e1) {
//===========================================================================
/// \code
/// display objName at objPosition.
/// Input:
///   typ,ind    typ & DB-ind of obj to display
///   e1         data-struct of obj
/// \endcode
//
// TODO: ganz neu machen;
// - loop tru DL;
// - skip all submodels ..
// - skip all parents..
// - display ...



  double UT3D_parbsp_par1();
  double UT3D_par_par1plg();

  double d1;
  char   oNam[32];
  Point  pTx, *pta;
  Vector vc1;


  // printf("APT_disp_nam %d %ld\n",typ,ind);
  // UT3D_stru_dump (typ, e1, "APT_disp_nam:");


  if(!APT_dispNam) return;


  // set oNam = text to display
  // oNam[0] = '.';
  oNam[0] = '_';
  APED_oid_dbo__ (&oNam[1], typ, ind);



  // set pTx=textPosition
  // set pta = pointer to startpoint of obj
  // set vc1 = direction of obj
  switch (typ) {

    case Typ_PT:
      APT_disp_TxtA (0, (Point*)e1, oNam);
      return;


    case Typ_LN:
      UT3D_pt_mid2pt (&pTx, &((Line*)e1)->p1, &((Line*)e1)->p2);
      // UT3D_vc_2pt (&vc1, &((Line*)e1)->p1, &((Line*)e1)->p2);
      // pta = &((Line*)e1)->p1;
      break;


    case Typ_CI:
      UT3D_pt_midci (&pTx, (Circ*)e1);
      break;


    case Typ_CVBSP:
      d1 = UT3D_parbsp_par1 (0.5, e1);
      UT3D_pt_evparCrvBSpl (&pTx, e1, d1);  // pt for nameTxt
      // d1 = ((CurvBSpl*)e1)->v1;
      // UT3D_pt_evparCrvBSpl (pta, e1, d1);  // pt for nameTxt
      // UT3D_vc_evalparCv (&vc1, (CurvBSpl*)e1, d1);
      break;


    case Typ_CVPOL:
      d1 = UT3D_par_par1plg (0.5, e1);
      UT3D_pt_evalplg (&pTx, e1, d1);
      // d1 = ((CurvPoly*)e1)->v1;
      // ? (pta, e1, d1);  // pt for nameTxt
      // UT3D_vc_evalplg (&vc1, (CurvPoly*)e1, d1);
      break;

    case Typ_ATXT:
      pTx = ((AText*)e1)->p1;
      break;

    case Typ_PLN:
      APT_disp_TxtA (0, &((Plane*)e1)->po, oNam);
      return;


    default:
      printf("***** APT_disp_nam E001 typ=%d dbi=%ld\n",typ,ind);

      return;

  }

  // display Text
  APT_disp_TxtA (0, &pTx, oNam);

  // // display direction of obj ..
  // APT_disp_SymV3 (SYM_ARRO3H, Typ_Att_Symb, pta, &vc1, 1.);

  return;
}



//=============================================================================
  int PP_up_list (NC_up_rec **upAct, char* macnam, int workmode) {
//=============================================================================
/*
init this list mit PP_up_list (NULL, NULL, -1);
Reset this list to APT-LineNr lnr mit PP_up_list (NULL, NULL, lNr);

Speichert nur Names; wenn Name vorhanden: RC=OFF,
 else RC=ON.

Input:
  macnam       NULL: Reset-Funktion
  workmode     1=CUT, 2=REV, 3=POCK 4=SEP_CUT, 5=SEP_REV
               -1=InitList; -2=Dump;

Output:
  upAct       Pointer to the active (open) NC_up_rec
  Retcod:     Index of NC_up_tab, -1=EOM-Error;
    0 = ganz neu (gespeichert)
    1 = schon mal angefragt, Ditto existiert.
   -1 = EOM-Error
*/


  // static char  uplist[NC_UPLISTSIZ][32];
  // static short upmode[NC_UPLISTSIZ];
  static NC_up_rec  NC_up_tab[NC_UPLISTSIZ];
  static int        upNr = 0;


  int          i1, irc;


  // printf("PPPPPPPPPPPP PP_up_list %d |%s| mode=%d\n",upNr,macnam,workmode);


  // Init List
  if(workmode == -1) {
    upNr = 0;
    irc = 0;
    goto Fertig;
  }


  // dump list ..
  if(workmode == -2) {
    printf("=========== PP_up_list %d ================\n",upNr);
    for(i1=0; i1<upNr; ++i1)
      printf(" %d %d |%s| lNr=%ld ind=%ld siz=%ld\n",i1,
             NC_up_tab[i1].mode, NC_up_tab[i1].cNam, NC_up_tab[i1].lNr,
             NC_up_tab[i1].ditt.ind, NC_up_tab[i1].ditt.siz);
    irc = 0;
    goto Fertig;
  }


  if(macnam == NULL) {
    // reset List; workmode is APT-LinNr
    if(upNr < 1) goto Fertig;
    for(i1=0; i1<upNr; ++i1) {
      // workmode is APT-lNr; alle UP-Recs loeschen, deren lNr >= workmode ist.
      if(workmode > NC_up_tab[i1].lNr) continue;
      // printf(" delet up's (incl)%d -\n",i1);
      upNr = i1;
      break;
    }
    goto Fertig;
  }




  // find macnam with same workmode in List
  for (i1=0; i1<upNr; ++i1) {
    if(NC_up_tab[i1].mode != workmode) continue;
    if(strcmp (macnam, NC_up_tab[i1].cNam)) continue;
    // printf("PP_up_list %d %s schon erledigt\n",i1,macnam);
    *upAct = &NC_up_tab[i1];
    irc = 1;
    goto Fertig;
  }


  // noch nicht bearb.; neu anlegen.
  if(upNr >= NC_UPLISTSIZ) {
    TX_Error("PP_up_list Overflow");
    irc = -1;
    goto Fertig;
  }


  // printf("PP_up_list save %d %s\n",i1,macnam);
  strcpy (NC_up_tab[upNr].cNam, macnam);
  NC_up_tab[upNr].mode = workmode;
  NC_up_tab[upNr].lNr  = APT_line_act;

  *upAct = &NC_up_tab[upNr];
  irc = 0;

  ++upNr;


  Fertig:
    // printf("ex PP_up_list rc=%d |%s|\n",irc,macnam);
  return irc;



}

/*===========================================================================*/
  void APT_get_dreh (int *Dreh, double *rad_o, double rad_i) {
/*=================
*/

  if(rad_i > 0.0) {

    *Dreh  = CCW;
    *rad_o = rad_i;
  } else {

    *Dreh  = CW;
    *rad_o = -rad_i;
  }

  return;

}


/*
  int WC_ask_WC_mode () {
  return WC_mode;
}

  Point2 WC_ask_actPos () {
  return actPosU;
}

  ObjG2  WC_ask_actObj () {
  return old_APT_ObjU;
}

  double WC_ask_actZsur () {
  return WC_sur_Z;
}


  double WC_ask_Zsur2 () {
  return WC_sur2;
}

  double WC_ask_Zsur1 () {
  return WC_sur1;
}
*/



/*===========================================================================*/
  double WC_ask_ModSiz () {
/*=====================
see WC_Init_Modsiz WC_Init_Tol ..
*/

  return APT_ModSiz;

}



/*===========================================================================*/
  void APT_DrawPoint (int Typ, long dbi, Point *pt1) {
/*==================
// Typ: 0=normal, black;  1=red (hilited);
*/

  // char  oNam[8];

  // UT3D_stru_dump(Typ_PT, pt1, "APT_DrawPoint %d",dbi);
  // printf("   Draw=%d disp=%d\n",APT_Stat_Draw,APT_dispPT);
  // printf(" _DrawPoint Typ=%d ind=%ld\n",Typ,dbi);



  if(APT_Stat_Draw == OFF) return;      // OFF=1
  // if(APT_dispPT != ON) return;


  AP_dli_act = DL_StoreObj (Typ_PT, dbi, Typ);
    // printf(" AP_dli_act=%ld\n",AP_dli_act);


  GR_DrawPoint (&AP_dli_act, Typ, pt1);

  if(APT_dispNam) APT_disp_nam (Typ_PT, dbi, (void*)pt1);

}


//================================================================
  void APT_DrawLine (int iAtt, long dbi, Line *ln1) {
//================================================================
// Input:
//   iAtt     see GR_Disp_ln2  (see ~/gCAD3D/cfg/ltyp.rc)
//   dbi      dbi


  // printf("APT_DrawLine %d %ld\n",iAtt,dbi);

  if(APT_Stat_Draw == OFF) return;

  AP_dli_act = DL_StoreObj (Typ_LN, dbi, iAtt);
  GR_DrawLine (&AP_dli_act, iAtt, ln1);

  if(APT_dispNam) APT_disp_nam (Typ_LN, dbi, (void*)ln1);
/*
      //TX_Print(" dyn Line %d; Z=%f",dbi,ln1->p1.z);
      if(UTP_comp2db(WC_sur1, ln1->p1.z, UT_TOL_pt)) {  // nur fuer ZSUR1
        //Print("von %f,%f nach %f,%f",ln1->p1.x,ln1->p1.y,ln1->p2.x,ln1->p2.y);
        APT_disp_SymV2 (SYM_ARROH, Typ_Att_Symb, &ln1->p2, &ln1->p1, 1.0);
        // Point2  pt20, pt21, pt22;
        // Line    lnp;
        // long    ind1;
        // // die gluQuadrics brauchen wahrscheinl. zu viel Platz; probier ma Lines
        // // die in der X/Y-Plane ein wenig gedreht werden.
        // pt20 = UT2D_pt_pt3(&ln1->p1);
        // pt21 = UT2D_pt_pt3(&ln1->p2);
        // UT2D_pt_rotptangr(&pt22, &pt21, &pt20, UT_RADIANS(30.));
        // TX_Print(" AuxLin %f,%f",pt22.x,pt22.y);
        // UT2D_pt_tra2ptlen(&pt22, &pt21, &pt22, APT_len_arr);
        // TX_Print(" Pfeil %f,%f",pt22.x,pt22.y);
        // lnp.p1 = ln1->p2;
        // lnp.p2.x = pt22.x;
        // lnp.p2.y = pt22.y;
        // lnp.p2.z = WC_sur1;
        // ind1 = DB_StoreLine ((long)-1, &lnp);
        // AP_dli_act = DL_StoreObj (Typ_LN, ind1, Typ);
        // GR_DrawLine (&AP_dli_act, Typ, &lnp);
      }
*/
    // }

}


//================================================================
  void APT_DrawCirc (int Typ, long dbi, Circ *ci1) {
//================================================================
// TODO: replace with GR_CreCirc

  // char  oNam[8];
  Point pt1;


  if(APT_Stat_Draw == OFF) return;


  // printf("UUUUUUUUUUUUUUUUUUUUUUUUUUUUU ");
  // printf("APT_DrawCirc %ld\n",dbi);
  // UT3D_stru_dump (Typ_CI, ci1, "APT_DrawCirc: ");


  // // check if obj already exists
  // AP_dli_act = DL_find_obj (Typ_CI, dbi);
  // printf(" ex find_obj %d\n",AP_dli_act);
  // if(AP_dli_act < 0)


  AP_dli_act = DL_StoreObj (Typ_CI, dbi, Typ);
  // AP_dli_act = DL_StoreObj (Typ_CI, dbi, Typ);      //2006-01-16 AC 


  GR_DrawCirc (&AP_dli_act, dbi, Typ, ci1);


  // // disp direction
  // if(APT_dispDir) {
    // // APT_disp_SymV2 (SYM_ARROH, Typ_Att_Symb, &ci1->p2, &pt1, 1.0);
    // Vector vc1;
    // // get tangent to circ on endpt
    // UT3D_vc_tng_ci_pt (&vc1, &ci1->p2, ci1);
    // APT_disp_SymV3 (SYM_ARRO3H, Typ_Att_Symb, &ci1->p2, &vc1, 1.); 
  // }

  //if((APT_dispNam)&&(dbi < APT_CI_SIZ)) {
  if(APT_dispNam) APT_disp_nam (Typ_CI, dbi, (void*)ci1);
/*
      //TX_Print(" dyn Circ %d; Z=%f",dbi,ci1->p1.z);
      if(UTP_comp2db(WC_sur1, ci1->p1.z, UT_TOL_pt)) {  // nur fuer ZSUR1
        double  d1;
        Point2  pt20, pt21, pt22;
        pt20 = UT2D_pt_pt3(&ci1->p2);
        pt21 = UT2D_pt_pt3(&ci1->pc);
        d1 = RAD_90;
        if(ci1->rad < 0.) d1 = -d1;
        UT2D_pt_rotptangr(&pt22, &pt20, &pt21, d1);
        pt1.x = pt22.x;
        pt1.y = pt22.y;
        pt1.z = WC_sur1;
        APT_disp_SymV2 (SYM_ARROH, Typ_Att_Symb, &ci1->p2, &pt1, 1.0);
      }
*/
    // }
 
}


/*
//===========================================================================
  void APT_DrawRFac (int Typ, ObjG2 *o1,double z1,ObjG2 *o2,double z2) {
//===========================================================================


  if(APT_Stat_Draw == OFF) return;

  //TX_Print("APT_DrawRFac %d",Typ);


  AP_dli_act = DL_StoreObj (Typ_SURRU, -1L, Typ);
  GL_DrawRFac (&AP_dli_act, Typ, o1, z1, o2, z2);

}
*/


//===========================================================================
  void APT_DrawCurv (int att, long dbi, ObjGX *cv1, double zval,
                     Memspc *tbuf1) {
//===========================================================================
/// \code
/// dbi ist DB-index; nicht DL-Index !
/// see GR_DrawCurv GR_Draw_dbo GR_Disp_dbo UTO_obj_Draw__
/// Input:
///   dbi      dbi
///   tbuf1    only for Typ_CVTRM
/// \endcode


  int       irc, typ;
  // int       irc, i1, i2, i3, typ, ptNr, ptMax, iMax, siz;
  // double    d1;
  // Point2    *pt2a;
  // Line2     *ln21;
  // Point     *pta;
  // Circ      ci1;
  ObjGX     *oTab, *ox1, *ox2, *ox3;
  // Curv2CCV  *ccv21;
  // ObjG2     o21;
  // ObjX      xu;


  if(APT_Stat_Draw == OFF) return;


  // printf("APT_DrawCurv: typ=%d form=%d att=%d ind=%ld\n",
             // cv1->typ,cv1->form,att,dbi);
  // printf("  APT_2d_to_3d_mode=%d\n",APT_2d_to_3d_mode);
  // UTO_dump__ (cv1, "Curv:");
  // UTO_dump_s_ (cv1, "Curv:");


  // if(APT_2d_to_3d_mode != OFF) {
    // ox1 = (ObjGX*)memspc54;
    // UTO_ox_tra (&ox1, cv1, APT_2d_to_3d_Mat);
  // } else {
    ox1 = cv1;
  // }
  // UTO_ox_tra kann typ von Typ_CVPOL2 -> Typ_CVPOL aendern !!


  typ = ox1->typ;


  if(typ == Typ_CVLNA) {
    AP_dli_act = DL_StoreObj (Typ_CVLNA, dbi, att);
    GL_DrawLtab (&AP_dli_act, att, (Line*)ox1->data, ox1->siz);


  } else if(typ == Typ_CVPSP3) {
    // AP_dli_act = DL_StoreObj (Typ_CV, dbi, att);
    AP_dli_act = DL_StoreObj (Typ_CVPSP3, dbi, att);
    GR_DrawCvPpsp3 (&AP_dli_act, att, ox1, zval);


  } else if(typ == Typ_CVBSP) {
    // AP_dli_act = DL_StoreObj (Typ_CV, dbi, att);
    AP_dli_act = DL_StoreObj (Typ_CVBSP, dbi, att);
    GR_DrawCvBSp (&AP_dli_act, dbi, att, ox1->data);
    if(APT_dispNam) APT_disp_nam (Typ_CVBSP, dbi, ox1->data);


  } else if(typ == Typ_CVRBSP) {
    // AP_dli_act = DL_StoreObj (Typ_CV, dbi, att);
    AP_dli_act = DL_StoreObj (Typ_CVRBSP, dbi, att);
    GR_DrawCvRBSp (&AP_dli_act, att, ox1->data);


  } else if(typ == Typ_CVPOL) {
    // AP_dli_act = DL_StoreObj (Typ_CV, dbi, att);
    AP_dli_act = DL_StoreObj (Typ_CVPOL, dbi, att);  // 2011-08-05
    GR_DrawCvPol (&AP_dli_act, att, ox1->data);
    if(APT_dispNam) APT_disp_nam (Typ_CVPOL, dbi, ox1->data);
    

  } else if(typ == Typ_CVPOL2) {
    AP_dli_act = DL_StoreObj (Typ_CVPOL2, dbi, att);
    GL_DrawPoly2D (&AP_dli_act, att, ox1->siz, ox1->data, zval);


  } else if(typ == Typ_CVELL) {
    AP_dli_act = DL_StoreObj (Typ_CVELL, dbi, att);
    GR_DrawCvEll (&AP_dli_act, att, ox1->data);


  } else if(typ == Typ_CVCLOT) {
    AP_dli_act = DL_StoreObj (Typ_CVCLOT, dbi, att);
    GR_DrawCvClot (&AP_dli_act, att, ox1->data);


  // } else if(typ == Typ_CVComp) {
    // // AP_dli_act = DL_StoreObj (Typ_CVComp, dbi, Typ_Att_hili);
    // AP_dli_act = DL_StoreObj (Typ_CVTRM, dbi, Typ_Att_hili);
    // GR_DrawCVComp (&AP_dli_act, att, ox1, tbuf1);

  } else if(typ == Typ_CVTRM) {
    AP_dli_act = DL_StoreObj (Typ_CVTRM, dbi, Typ_Att_hili);
    // GR_DrawCvCCV (&AP_dli_act, att, ox1->data, tbuf1);
    GR_DrawCvCCV (&AP_dli_act, dbi, att, ox1, tbuf1);

// TODO: test for 2D-Curve
  // } else if(typ == Typ_CVTRM2) {
    // // GR_DrawCvCCV2 (&AP_dli_act, att, ox1->data);
    // GR_DrawCvCCV2 (&AP_dli_act, att, ox1);


  } else {
    TX_Error("APT_DrawCurv E001 typ=%d",typ);
  }

  return;



  L_EOM:
  TX_Error("APT_DrawCurv EOM");
  return;

}


//===========================================================================
  void  APT_curv2ptArr (Point2 *pta, int *ianz, ObjG2 *o1) {
//===========================================================================
// // die Polygonpunkte in ein Point2-Array holen


  long  ind1, ind2, ID1, ID2, ptNr, ptInd, incr, ptAnz;
  Point pt1;

  //TX_Print("APT_curv2ptArr %d",o1->typ);
  //TX_Print("   Ind=%d/%d Offs=%d/%d",(int)o1->pc.x,(int)o1->pc.y,o1->ID,o1->ID1);



  ind1 = o1->pc.x;
  ind2 = o1->pc.y;

  ID1  = o1->ID;
  ID2  = o1->ID1;



  // der erste Punkt ist cv1->p1
  if(ind1 < ind2) {
    incr  = 1;
    ptAnz = ind2 - ind1 + 1;
  } else {
    incr  = -1;
    ptAnz = ind1 - ind2 + 1;
  }


  pta[0] = o1->p1;
  //TX_Print(" Vert 0=%f,%f",pta[0].x,pta[0].y);


  ptNr  = 1;
  ptInd = ID1 + ind1;


  if(ID1 == -2) goto   L_CvEnd;     // check for Line


  NextVert:
    if(ptNr > ptAnz) goto L_CvEnd;

    pt1 = DB_GetPoint (ptInd);
    pta[ptNr] = UT2D_pt_pt3 (&pt1);
    //TX_Print(" Vert %d=%f,%f",ptNr,pta[ptNr].x,pta[ptNr].y);

    ++ptNr;
    if(ptNr >= 500) {
      TX_Error("APT_curv2ptArr E001");
      return;
    }

    ptInd = ptInd + incr;
    goto NextVert;




  // der letzte Punkt ist cv1->p2
  L_CvEnd:
  pta[ptNr] = o1->p2;
  //TX_Print(" last Vert %d=%f,%f",ptNr,pta[ptNr].x,pta[ptNr].y);

  *ianz = ptNr + 1;

  //TX_Print(" ex APT_curv2ptArr ianz=%d",*ianz);

}


//***********************************************************************
int APT_Lay_add(int layNr,int aus_anz,char* sptr,int* aus_typ,double* aus_tab){
//***********************************************************************

  int       i1, iparm;
  char      auxBuf[32], *cp;


/*
  TX_Print("DB_Lay_add %d %d",aus_anz,layNr);
  for(i1=0; i1<aus_anz; ++i1) {
    TX_Print(" %d %d %f",i1,aus_typ[i1],aus_tab[i1]);
  }
*/


  if(layNr < 0) goto Error;


  iparm = -1;


  // das GroupBit1 mal fuer alle Obj, die NICHT in Lay layNr sind, ON setzen.
  DL_Lay_act_g1 (layNr, ON);


  Next_Parm:
  ++iparm;
  //TX_Print("Next_Parm %d",iparm);
  if(iparm > aus_anz) goto Fertig;

  cp = APT_get_Txt (auxBuf, sptr, aus_tab[iparm]);
  UTX_cp_word_2_upper (auxBuf, auxBuf);
  //TX_Print(" auxBuf=/%s/",auxBuf);

  ++iparm;




  if(!strcmp (auxBuf, "LAY")) {
    if(aus_typ[iparm] != Typ_Val) goto Error;
    i1 = aus_tab[iparm];

    // grp_1 fuer alle Obj, die NICHT im Layer i1 sind, auf OFF.
    DL_Lay_act_g1 (layNr, OFF);
    goto Next_Parm;





  } else if(!strcmp (auxBuf, "COL")) {
    if(aus_typ[iparm] != Typ_Val) goto Error;
    i1 = aus_tab[iparm];

    // grp_1 fuer alle Obj, die NICHT Farbe i1 haben, auf OFF.
    DL_Lay_col_g1 (i1, OFF);
    goto Next_Parm;





  } else if(!strcmp (auxBuf, "LTYP")) {
    if(aus_typ[iparm] != Typ_Val) goto Error;
    i1 = aus_tab[iparm];

    // grp_1 fuer alle Obj, die NICHT ltyp i1 haben, auf OFF.
    DL_Lay_ltyp_g1 (i1, OFF);
    goto Next_Parm;





  } else if(!strcmp (auxBuf, "THICK")) {
    if(aus_typ[iparm] != Typ_Val) goto Error;
    i1 = aus_tab[iparm];

    // grp_1 fuer alle Obj, die NICHT Dicke i1 haben, auf OFF.
    DL_Lay_thk_g1 (i1, OFF);
    goto Next_Parm;





  } else if(!strcmp (auxBuf, "TYP")) {
    cp = APT_get_Txt (auxBuf, sptr, aus_tab[iparm]);
    UTX_cp_word_2_upper (auxBuf, auxBuf);
    i1 = DB_Typ_Char(auxBuf);

    // grp_1 fuer alle Obj, die NICHT Typ i1 haben, auf OFF.
    DL_Lay_typ_g1 (i1, OFF);
    goto Next_Parm;






  } else {
    goto Error;
  }





  Fertig:
  // nun sollen alle Obj, die das grp_1-Bit ON haben, dem Layer layNr
  // zugefuegt werden.
  DL_Lay_add (layNr);


  return 1;

  Error:
  return -1;

}


/* replaced by ATO_ato_srcLn__
//==============================================================================
  int APT_decode__ (ObjAto *ato, char **data) {
//==============================================================================
/// \code
/// DO NOT USE; new func: ATO_ato_srcLn__
/// decode textline <data> into types and values (aus_typ, aus_tab)
/// NEEDS TERMINATING '\0' !
///  uses memspc54 memspc55 memspc012 for ato !
/// Names ("xx #name") must have been removed
/// Input:
///   data     pointer to sourceText;
/// Output:
///   ato->typ     pointer to array of integers (atomicObj - types)
///   ato->val     pointer to array of doubles  (atomicObj - values)
///   retCod   nr of atomic objects
///
/// see also
///   ATO_ato_srcLn__
///   APT_obj_ato      create struct from atomicObjs
///   APT_obj_expr      create struct from ModelCode (text).
///   UTF_GetLinNr          copy sourcecodeline
/// \endcode

// returns addresses of global aus_typ & aus_tab !
// liefert adressen !


  int       aus_anz, aus_SIZ, *aus_typ;
  double    *aus_tab;


  // printf("APT_decode__ |%s|\n",*data);


  // if(aus_typ == NULL) {
    // get memSpc for &aus_typ, &aus_tab
    // ATO_getSpc1 (&aus_SIZ, &aus_typ, &aus_tab);
  // }

  // get memspc
  ATO_getSpc__ (ato);

 
  APT_defTxt = *data;
  ato->txt = *data;

  // nun die Ausdruecke aus data einlesen, decodieren und merken
  // aus_typ & aus_tab must have been connected before !
  // aus_anz = APT_decode_ausdr (aus_typ, aus_tab, aus_SIZ, data);
  ato->nr = APT_decode_ausdr (ato->typ, ato->val, ato->siz, data);

  // *atyp = aus_typ;
  // *atab = aus_tab;

    // ATO_dump__ (ato);
  
  return ato->nr;

}
*/


//================================================================
  int APT_obj_expr (void *data, int typ, char *cbuf) {
//================================================================
/// \code
/// get struct from ModelCode (text).
/// copies struct -> data
///
/// Input:
///   typ    requested typ of object; eg Typ_PT; 
///          data must have corresponding size.
///          Result unknown: set typ=0; size of data = OBJ_SIZ_MAX
///   cbuf   sourceCode;  eg "P21"
/// Output: 
///   data   struct corresponding to <typ>
/// Returncodes:
///   -1     Error else type of data
///
/// see also IE_decode_Ln APT_store_obj APED_dec_txt APT_decode__
/// \endcode

  // int       aus_anz, aus_SIZ, *aus_typ;
  // double    *aus_tab;


  int    i1, dbTyp, oSiz;
  void   *op1;
  ObjAto ato1;


  // printf("APT_obj_expr %d |%s|\n",typ,cbuf);


  // get memSpc for atomicObjects  uses memspc54 memspc55 memspc53
  ATO_getSpc__ (&ato1);


  // get ato from ModelCode (text).
  i1 = ATO_ato_srcLn__ (&ato1, cbuf); // 
  if(i1) {
    TX_Error ("Error APT_obj_expr 1-%d",i1);
    return -1;
  }
    // ATO_dump__ (&ato1, " APT_obj_expr-1");

 
  // use Typ_Val  direct for typ=Typ_VAR
  if((typ == Typ_VAR) && (ato1.typ[0] == Typ_Val)) {
    memcpy (data, (void*)&ato1.val[0], sizeof(double));
    dbTyp = Typ_Val;
    goto L_exit;
  }

 
  // get binObj of DB-obj
  dbTyp = DB_GetObjDat (&op1, &i1, ato1.typ[0], (long)ato1.val[0]);
  if(dbTyp <= 0) {
    TX_Error ("Error APT_obj_expr 2-%d",dbTyp);
    return -1;
  }
    // printf(" dbTyp=%d i1=%d\n",dbTyp,i1);
    // UT3D_stru_dump (dbTyp, op1, "APT_obj_expr-2");


  // test if dbTyp == typ
  if(typ) {
    if(dbTyp != typ) {
      TX_Error ("APT_obj_expr E3-%d-%d",dbTyp,typ);
      return -1;
    }
    oSiz = UTO_siz_stru (typ);

  } else {
    oSiz = OBJ_SIZ_MAX;
  }


  // copy binObj op1 -> data
  i1 = UTO_copy_stru (data, &oSiz, dbTyp, op1, 1);
  if(i1 < 0) {
    TX_Error ("Error APT_obj_expr E4-%d",i1);
    return -1;
  }

  L_exit:
  return dbTyp;


// old version:
  // aus_obj.txt = cbuf;
  // APT_defTxt = cbuf;   // for strings
  // aus_obj.nr = APT_decode_ausdr (ato1.typ, ato1.val, ato1.siz, &cbuf);
  // if(ato1.nr < 1) return -1;
  // return APT_obj_ato (data, typ, &ato1);

}


//================================================================
  int APT_obj_ato (void *data, int typ, ObjAto *aus_obj) {
//================================================================
/// \code
/// APT_obj_ato      create struct from atomicObjs
/// Output:
///   data       memspc for the obj of typ <typ>; eg a struct Vector or Point ..
/// see also APT_store_obj
/// \endcode

// Data must be provided in aus_anz/aus_typ/aus_tab
// APT_defTxt = pointer to sourceText
// use APT_decode__ or APT_decode_ausdr before.
// see also APT_store_obj

  int       aus_anz, aus_SIZ, *aus_typ;
  double    *aus_tab;
  char      *aus_txt;

  int   i1, irc;

  // printf("APT_obj_ato typ=%d\n",typ);
  // ATO_dump__ (aus_obj, "_obj_ato-1");


  if(aus_obj == NULL) {
    TX_Error ("APT_obj_ato E000");
    return -1;
  }

  aus_typ = aus_obj->typ;
  aus_tab = aus_obj->val;
  aus_anz = aus_obj->nr;
  aus_SIZ = aus_obj->siz;
  aus_txt = aus_obj->txt;


  // printf("APT_obj_ato typ=%d aus_anz=%d \n",typ,aus_anz);
  // ATO_dump__ (aus_obj);


  if(TYP_IS_GEOMPAR(typ)) {   // Typ_Val - Typ_Typ = all types of values
    // Typ_Angle
    if(typ == Typ_Angle) {
      irc = APT_decode_ang (data, aus_anz, aus_typ, aus_tab);
      goto L_exit;
    }

    if(aus_typ[0] == Typ_Val)   {
      *((double*)data) = aus_tab[0];
      irc = 0;
    } else {
      // TX_Print("APT_obj_expr E002 %d",typ); return -1;}
      irc = APT_decode_var (data, aus_anz, aus_typ, aus_tab);
    }


  } else if(typ == Typ_VAR) {  // 2013-03-15
      irc = APT_decode_var (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_Angle) {
      irc = APT_decode_ang (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_PT) {
    irc = APT_decode_pt (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_LN) {
    irc = APT_decode_ln (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_CI) {
    irc = APT_decode_ci (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_VC) {
    irc = APT_decode_vc (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_GTXT) {
    // nur "N21" decodieren (1 parameter)
    irc = APT_decode_note (data, aus_anz, aus_typ, aus_tab);


  } else if((typ == Typ_CV)     ||
            (typ == Typ_CVLNA)  ||
            (typ == Typ_CVPOL)  ||
            (typ == Typ_CVPOL2) ||
            (typ == Typ_CVELL)  ||
            (typ == Typ_CVCLOT) ||
            (typ == Typ_CVBSP)  ||
            (typ == Typ_CVRBSP) ||
            (typ == Typ_CVTRM))    {
    // nur "S21" decodieren (1 parameter)
    // irc = APT_decode_curv (data, aus_anz, aus_typ, aus_tab);
    irc = APT_decode_cv (data, &i1, aus_anz, aus_typ, aus_tab);
    // typ = ((ObjGX*)data)->typ;
    typ = Typ_ObjGX; // nur f UT3D_stru_dump !!!


  } else if(typ == Typ_PLN) {
    irc = APT_decode_pln1 ((Plane*)data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_SUR) {
    UME_init (&APTSpcTmp, memspc501, sizeof(memspc501));
    irc = APT_decode_sur (data, aus_anz, aus_typ, aus_tab, &APTSpcTmp);


  } else if(typ == Typ_SOL) {
    irc = APT_decode_sol (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == Typ_Tra) {
    irc = APT_decode_tra (data, aus_anz, aus_typ, aus_tab);


  } else if((typ == Typ_Model) ||
            (typ == Typ_Mock))    {
    irc = APT_decode_model (data, aus_anz, aus_typ, aus_tab);


  } else if(typ == TYP_FilNam) {
    APT_get_Txt (data, aus_txt, aus_tab[0]);
      // printf(" TYP_FilNam=|%s|\n",data);


  } else if(typ == Typ_Txt) {
    // can be a value !?
    // APT_get_Txt (data, aus_txt, aus_tab[0]);
    if(aus_typ[0] == Typ_Val) {
      i1 = aus_tab[0];
      sprintf(data, "%d", i1);
        // printf(" Typ_Txt=|%s|\n",(char*)data);
      irc = 0;
    } else {
        printf("*************** APT_obj_ato I001\n");
    }



  } else {
    // TX_Error("APT_obj_expr E001 %d",typ);
    TX_Print("APT_obj_ato E001 %d",typ);
    return -1;
  }

  L_exit:

    // if(irc >= 0)
    // UT3D_stru_dump (typ, data, "ex APT_obj_ato - irc=%d",irc);

  return irc;

}


// EOF

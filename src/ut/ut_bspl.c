//
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
UT3D_pt_projptbspl
  OUT OF MEM in UT3D_bez_bspl__ bei mehr als 70 Punkten !!!!!!!!!!!
Wenn in der struct zusaetzl Richtung (dir als 1 oder -1 waere)
  und zusaetzl Flag 'closed' dann koennte man ueber das CurveEnde druebergehen
  v0=0.5 v1=0.1 dir=1 sollte uebers Ende druebergehen

-----------------------------------------------------
Modifications:
2003-06-26 Tangente bei deg=2 korr. SK.
2003-06-02 bspl_eval_Tg neu zu. RF.
2003-06-02 bspl_bsp_ptn: memspc51 durch Memspc ersetzt. RF.
2002.05.06 Korrektur ptNr -> segNr. RF.

-----------------------------------------------------
*/
/*!
\file  ../ut/ut_bspl.c
\brief B-Spline-Curve-Utilityfunctions
\code
=====================================================
List_functions_start:

UT3D_bsp_ck_closed__       check if B-SplCurv is closed (ignores trimmed)
UT3D_bsp_ck_closed_tr      check if B-SplCurv is closed and untrimmed
UT3D_bsp_ck_pt             check if B-SplineCurve is a point
UT3D_bsp_ck_minSiz         check for minSiz / underSize
UT3D_bsp_ck_maxDeg         returns max degree
UT3D_bsp_ck_planar         check if spline is planar to given vector
UT3D_cbsp_ck_trim          test is spline is trimmed
UT3D_ncvbsp_orient         orient group of B-SplCurves (same directiorn)
UT3D_2ncvbsp_orient        orient 2 groups of BSplCurves for surface
UT3D_4cvbsp_3cvbsp         make 4 curves from 3 curves for a CoonsPatchSurf.

UT3D_parCv_bsplpt          curve parameter <-- point on b-spline curve
UT3D_ptNr_bsplTol          estimate nr of polygonPoints for B-spline
UT3D_du_bsplTol            get du for polygonalization of bSpline from tol
UT3D_bsp_degrad            degrade bsp to line
UT3D_bsp_infTg             Eine Tabelle der Wendepunkte liefern (nur Parameter)

UT3D_cbsp_ox               create BSP-Curv from ObjGX-obj
UT3D_cbsp_1pt              create BSP-Curv from point
UT3D_cbsp_2pt              create BSP-Curv from Line
UT3D_cbsp_ci               create BSP-Curv from Circ
UT3D_cbsp_ell              create BSP-Curv from Ellipse
UT3D_cbsp_clt              create BSP-Curv from Clothoid
UT3D_cbsp_parl_pln         Bspl-Curv parallel to Bspl-Curv on plane, dist
bspl_bsp_ptn               make B-spline cvo from pointTable pTab
UT3D_cbsp_2cbsp            join 2 BSP-Curves
UT3D_cbsp_trim             trim a bspline-curve

UT3D_pt_cbsp_lim           get limits (startpoint, endpoint, parameters)

UT3D_pt_projptbspl         project point onto b-spline curve
UT3D_pt_int2bspl           intersect 2 b-spline curves
UT3D_pt_intlnbspl          intersect line with b-spline curve
UT3D_pt_intplbspl          Intersect Plane / BSplineCurve

UT2D_pt_tng_cbsp_vc        tangent vector-bSpline
UT2D_pt_tng_cbsp_pt        tangent point-bSpline
UT3D_pt_tng_cbsp_vc__      get tangent vector-bSpline     USE UT2D_pt_tng_cbsp_vc
UT3D_vc_tng_cbsp_pt__      get tangent point-bSplineCurve USE UT2D_pt_tng_cbsp_pt

UT2D_pt_evpar_cbsp         point from knotValue
UT3D_pt_evparCrvBSpl       point <-- evaluate curve parameter
                        ACHTUNG: kann Extrapolation noch nicht !
UT3D_pt_evalparCv          Punkt an B-Spline aus Parameterwert
bspl_pt_cbspDeg1           evaluate Point on B-SplCurve with degree 1.
UT3D_vc_evalparCv          Tangente an B-Spline aus Parameterwert

UT3D_par1_parbsp           get parameter 0-1 from knotValue
UT3D_parbsp_par1           get knotValue from parameter 0-1
bspl_segNr_par             get segmentNr from parameter
bspl_tmpSpc_siz            get size of necessary tempSpace for UT3D_pt_projptbspl

UT3D_bsp_cpsbsp            copy Bsp-Curve out of Bsp-Surf (nicht interpolierend)
UT3D_knotvec_m             create & copy knotvektor
bspl_knotvec__             Erstellen des knotvektor
bspl_pol_bsp               make Polygon from B-Spline-Kurve
bspl_cv_bsp                make Polygon from B-Spline mit fixer ptNr
bspl_cvpol_cvbsp  UNUSED   Aus einem B-Spline eine Polygon bilden.
bspl_eval_Pt               Zwischenpunkte einer B-Spline-Kurve errechnen
bspl_eval_Tg               Evaluate a tangent on a B-spline.

UT3D_pt_projptbsp_ext      lineare extrapolation
bspl_eval_expPt            lineare extrapolation

List_functions_end:
=====================================================
- test functions:
UT3D_testbsplbez           B-spline curve segments <--> Bezier curves
- see also:
../ut/ubscrv.c

\endcode *//*----------------------------------------


-----------------------------------------
Rationale B-Splines haben nicht alle Gewichte gleich.

Polynomial B-Splines:  alle Gewichte sind  gleich - 1.0 (bei CatV5 !)

B-Spline Bezier polygon (see GORD74) ??

uniformly spaced integer knots ...

NonUniform_B-splines  - nicht evenly spaced ?

Non Uniform Rational B Splines = NURBS

interpolating controlpoint = the surf (curv) intersects the controlpoint

approximating controlpoint=the surf (curv) does not intersect the controlpoint

A weigth of 0: the controlpoint does not affect the curve.

Knots define the range that a given basis Function is active.

The knot vector is a collection of "Knots" that tie together the basisfunctions.

Man kann knots uebereinanderlegen.

3 knots am gleichen Platz koennen eine Ecke in der Kurve machen ?

The number of controlpoints id equal to the order of the curve.

The sum of all basis functions that affect the curve is always 1.





---------------------------------------------------------------
*/

#ifdef _MSC_VER
#include "../xa/MS_Def0.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// #include "../ut/ut_umem.h"             // UME_..
#include "../ut/ut_geo.h"
#include "../ut/ut_bspl.h"
// #include "../ut/ut_obj.h"           // UTO_stru_2_obj
#include "../ut/ut_plg.h"              // UT3D_plg_lvTab
#include "../ut/ut_ox_base.h"             // OGX_SET_INDEX
// #include "../xa/xa_mem.h"                 // memspc51-55

#include "../ut/func_types.h"               // SYM..

#include "../db/ut_DB.h"               // DB_Get..

// #include "../xa/xa_mem.h"              // memspc51, memspc50


// includes from ../ut/ut_geo_const.h
extern double UT_TOL_0Cos;     // tolerance for zero cosine
// extern double UT_TOL_cv;    // die max erlaubte Abweichung von der
                               // Originalkurve

// ex ../ci/NC_Main.c:
extern double APT_ModSiz;






//================================================================
  int UT3D_4cvbsp_3cvbsp (CurvBSpl **cv1tab, CurvBSpl **cv2tab,
                          Memspc *objSeg) {
//================================================================
// UT3D_4cvbsp_3cvbsp         make 4 curves from 3 curves for a CoonsPatchSurf.
// Input:
//   cv2tab has 2 curves; which must have a common point.
//   cv1tab has 1 curve, closing the cv2tab-curves to closed boundary.
//          for the second curve the place for the pointer is defined but free.
// Output:
//   create cv1tab[1] as a curve with the same knots as cv1tab[0] and
//          with all controlpoints at the common-point of cv2tab.
// CurvData is created in objSeg.


// Der Punkt muss das letze Objekt sein,
// sonst gehts ned ..
//


  int       i1;
  CurvBSpl  *auxtab[3];
  static CurvBSpl  bspTU[2], bspTV[2];
 
  auxtab[0] = cv1tab[0];
  auxtab[1] = cv2tab[0];
  auxtab[2] = cv2tab[1];

  i1 = UCBS_PrepCoons3BspCrvs (bspTU, bspTV, objSeg, auxtab);
    // printf(" _PrepCoons3BspCrvs %d\n",i1);


  cv1tab[0] = &bspTU[0];
  cv1tab[1] = &bspTU[1];
  cv2tab[0] = &bspTV[0];
  cv2tab[1] = &bspTV[1];

  // UT3D_stru_dump (Typ_CVBSP, cv1tab[0], "C1[0]");
  // UT3D_stru_dump (Typ_CVBSP, cv1tab[1], "C1[1]");
  // UT3D_stru_dump (Typ_CVBSP, cv2tab[0], "C2[0]");
  // UT3D_stru_dump (Typ_CVBSP, cv2tab[1], "C2[1]");

  return i1;






/*
  int       i1, ii;
  Point     p11s, p21s, p21e, p22s, p22e, *pcm;
  CurvBSpl  *cv1;


  printf("UT3D_4cvbsp_3cvbsp ------\n");
  UT3D_stru_dump (Typ_CVBSP, cv1tab[0], "C1[0]");
  UT3D_stru_dump (Typ_CVBSP, cv2tab[0], "C2[0]");
  UT3D_stru_dump (Typ_CVBSP, cv2tab[1], "C2[1]");



  // get Startpt of 1. Curve of cv1tab
  UT3D_pt_evalparCv (&p11s, cv1tab[0], cv1tab[0]->v0);

  // get Startpt of 1. Curve of cv2tab
  UT3D_pt_evalparCv (&p21s, cv2tab[0], cv2tab[0]->v0);

  // get Endpt of 1. Curve of cv2tab
  UT3D_pt_evalparCv (&p21e, cv2tab[0], cv2tab[0]->v1);

  // get Startpt of 2. Curve of cv2tab
  UT3D_pt_evalparCv (&p22s, cv2tab[1], cv2tab[1]->v0);

  // get Endpt of 2. Curve of cv2tab
  UT3D_pt_evalparCv (&p22e, cv2tab[1], cv2tab[1]->v1);




  i1 = UT3D_comp4pt (&p21s, &p21e, &p22s, &p22e, UT_DISP_cv);
    printf(" rc ex comp4pt = %d\n",i1);
  if(i1 == 0) { // 0: keine Verbindung
    TX_Error("UT3D_4cvbsp_3cvbsp no common point error ..");
    return -1;

  } else if(i1 == 1) { // 1: p1e=p2a
    // invert 1.Curve of cv2tab
    cv1 = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cv1 == NULL) goto L_err_eom;
    i1 = UCBS_RevBspCrv (cv1, objSeg, cv2tab[0]);
    if(i1 < 0) return -1;
    cv2tab[0] = cv1;
    MEM_swap__ (&p21s, &p21e, sizeof(Point));
    pcm = &p21s;
    goto LCE; // endpoints are common;

  } else if(i1 == 2) { // 2: p1a=p2a
    pcm = &p21s;
    goto LCS; // startpoints are common;

  } else if(i1 == 3) { // 3: p1e=p2e
    pcm = &p21e;
    goto LCE; // endpoints are common;

  } else if(i1 == 4) { // 4: p1a=p2e
    // invert 2.Curve of cv2tab
    cv1 = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cv1 == NULL) goto L_err_eom;
    i1 = UCBS_RevBspCrv (cv1, objSeg, cv2tab[1]);
    if(i1 < 0) return -1;
    cv2tab[1] = cv1;
    MEM_swap__ (&p22s, &p22e, sizeof(Point));
    pcm = &p21s;
    goto LCS; // startpoints are common;
  }


  // startpoints are common;
  LCS:
    printf(" --LCS\n");
    ii = 0;
    pcm = &p21s;
    // check if endpoint of 1.Curve of cv2tab == Startpt of cv1tab
    if(UT3D_comp2pt(&p21e, &p11s, UT_DISP_cv) != 1) goto L_invert;
    goto L_copy;


  // endpoints are common;
  // endPt of c21 must be startPt of c11
  LCE:
    printf(" --LCE\n");
    UT3D_stru_dump (Typ_PT, &p21e, "--LCE-p21e");
    ii = 1;
    // check if endpoint of 1.Curve of cv2tab == Startpt of cv1tab
    if(UT3D_comp2pt(&p21e, &p11s, UT_DISP_cv) != 1) goto L_invert;
    goto L_copy;



  // invert  1.Curve of cv1tab
  L_invert:
  printf(" --invert c11\n");
  cv1 = UME_reserve (objSeg, sizeof(CurvBSpl));
  if(cv1 == NULL) goto L_err_eom;
  i1 = UCBS_RevBspCrv (cv1, objSeg, cv1tab[0]);
  if(i1 < 0) return -1;
  cv1tab[0] = cv1;


  L_copy:
  // duplicate the 1. Curve of cv1tab
  // copy the struct (without data)
  cv1tab[1] = UME_reserve (objSeg, sizeof(CurvBSpl));
  i1 = UCBS_CpyBspCrv (cv1tab[1], objSeg, cv1tab[0]);
  if(i1 < 0) goto L_err_eom;
    // UT3D_stru_dump (Typ_CVBSP, cv1tab[1], "C1[1]");

  // change all points of curv ii to *pcm;
  UT3D_stru_dump (Typ_PT, pcm, "pcm");
  cv1 = cv1tab[ii];
  for (i1=0; i1<cv1->ptNr; ++i1) cv1->cpTab[i1] = *pcm;


  printf("ex UT3D_4cvbsp_3cvbsp\n");
  MEM_swap__ (cv1tab, cv2tab, sizeof(void*) * 2);

  UT3D_stru_dump (Typ_CVBSP, cv1tab[0], "C1[0]");
  UT3D_stru_dump (Typ_CVBSP, cv1tab[1], "C1[1]");
  UT3D_stru_dump (Typ_CVBSP, cv2tab[0], "C2[0]");
  UT3D_stru_dump (Typ_CVBSP, cv2tab[1], "C2[1]");




  return 0;


  L_err_eom:
    return -1;
*/
}

//=======================================================================
  int UT3D_2ncvbsp_orient (int cv1Nr, CurvBSpl **cv1tab,
                           int cv2Nr, CurvBSpl **cv2tab, Memspc *objSeg) {
//=======================================================================
// UT3D_2ncvbsp_orient         orient 2 groups of BSplCurves for surface
// RetCod:    0=OK; -1=curves do not provide a net.

// Both groups consists of b-splines only.
// Only the sequence of curves is ok, the direction is to be fixed.

/*
Zuerst in U-Richtung (dann in V-Richtung):

get 1. U-Curve (U-curve-0):
  loop tru all V-curves;
    test if startPoint or endPoint of V-curve lies on U-curve-0
    startPoint is on U-Curve: OK; continue.
    endPoint is on U-Curve: turn curve, ok; continue.
    none: exit; error on position.

once again, with U/V changed.
*/

  int      i1, i2, ii, irc;
  long     l1;
  double   da[10];
  Point    *pTab, *pt1, *pt2, pa[10];
  CurvBSpl *cv1, *cv2;
  // Memspc   tmpSpc=UME_NEW;


  // printf("======================================================= \n");
  // printf("UT3D_2ncvbsp_orient %d %d\n",cv1Nr,cv2Nr);
  // for(i1=0;i1<cv1Nr;++i1)UT3D_stru_dump(Typ_CVBSP,cv1tab[i1], "1-%d",i1);
  // for(i1=0;i1<cv2Nr;++i1)UT3D_stru_dump(Typ_CVBSP,cv2tab[i1], "2-%d",i1);



  // UME_alloc_tmp (&tmpSpc, 5000000);  // 5MB tempSpace
  // UME_alloc_tmp (&tmpSpc, 1000000);  // 1MB tempSpace


  //----------------------------------------------------------------
  cv1 = cv1tab[0];
    // printf(" test start/end of cv2-curves on curve cv1[0]\n");


  // loop tru all curves of group-2
  for(i1=0; i1<cv2Nr; ++i1) {
    cv2 = cv2tab[i1];

    // get startpoint of this curve
    pt2 = &cv2->cpTab[0];

    // test if this point lies on cv1
    // get the point nearest to pt2 on cv1
    ii = 10;
    irc = UT3D_pt_projptbspl (&ii, pa, da, cv1, pt2);
      // UT3D_stru_dump(Typ_PT, pa, "sta %d %d %d",i1,irc,ii);
    if(irc < 0) goto L_err_int;
    if(ii < 1) goto L_ck_ep1;
    for(i2=0; i2<ii; ++i2) {
      if(UT3D_comp2pt(pt2, &pa[i2], UT_DISP_cv) == 1) goto L_cont1; // 1=ident
    }

    // get endpoint of this curve
    L_ck_ep1:
    pt2 = &cv2->cpTab[cv2->ptNr - 1];

    // get the point nearest to pt2 on cv1
    ii = 10;
    irc = UT3D_pt_projptbspl (&ii, pa, da, cv1, pt2);
      // UT3D_stru_dump(Typ_PT, pa, "end %d %d %d",i1,irc,ii);
    if(irc < 0) goto L_err_int;
    if(ii < 1) goto L_err_int;

    for(i2=0; i2<ii; ++i2) {
      if(UT3D_comp2pt(pt2, &pa[i2], UT_DISP_cv) == 1) goto L_turn1; // 1=ident
    }

    L_cont1:
    continue;

    L_turn1:
      // printf("  _2ncvbsp_orient:  *** turn cv2tab[%d]  ***\n",i1);
    cv2 = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cv2 == NULL) goto L_err_mem;
    ii = UCBS_RevBspCrv (cv2, objSeg, cv2tab[i1]);
    if(ii < 0) goto L_err_ret;
    cv2tab[i1] = cv2;
  }



  //----------------------------------------------------------------
  cv1 = cv2tab[0];
    // printf(" test start/end of cv1-curves on curve cv2[0]\n");

  // loop tru all curves of group-2
  for(i1=0; i1<cv1Nr; ++i1) {
    cv2 = cv1tab[i1];

    // get startpoint of this curve
    pt2 = &cv2->cpTab[0];

    // test if this point lies on cv1
    // get the point nearest to pt2 on cv1
    ii = 10;
    irc = UT3D_pt_projptbspl (&ii, pa, da, cv1, pt2);
      // UT3D_stru_dump(Typ_PT, pa, "sta %d %d %d",i1,irc,ii);
    if(irc < 0) goto L_err_int;
    if(ii < 1) goto L_ck_ep2;
    for(i2=0; i2<ii; ++i2) {
      if(UT3D_comp2pt(pt2, &pa[i2], UT_DISP_cv) == 1) goto L_cont2; // 1=ident
    }

    // get endpoint of this curve
    L_ck_ep2:
    pt2 = &cv2->cpTab[cv2->ptNr - 1];

    // get the point nearest to pt2 on cv1
    ii = 10;
    irc = UT3D_pt_projptbspl (&ii, pa, da, cv1, pt2);
      // UT3D_stru_dump(Typ_PT, pa, "end %d %d %d",i1,irc,ii);
    if(irc < 0) goto L_err_int;
    if(ii < 1) goto L_err_int;

    for(i2=0; i2<ii; ++i2) {
      if(UT3D_comp2pt(pt2, &pa[i2], UT_DISP_cv) == 1) goto L_turn2; // 1=ident
    }

    L_cont2:
    continue;

    L_turn2:
      // printf("  _2ncvbsp_orient:  *** turn cv1tab[%d]  ***\n",i1);
    cv2 = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cv2 == NULL) goto L_err_mem;
    ii = UCBS_RevBspCrv (cv2, objSeg, cv1tab[i1]);
    if(ii < 0) goto L_err_ret;
    cv1tab[i1] = cv2;
  }



  return 0;


  //----------------------------------------------------------------
  L_err_int:
    TX_Error(" no connection - pos = %f %f %f",pt2->x,pt2->y,pt2->z);
    return -1;

  L_err_mem:
    TX_Error(" out of mem ..");
    return -1;

  L_err_ret:
    TX_Error(" Error in UCBS_RevBspCrv ..");
    return -1;

}



/* old version until 2011-09-01
//=======================================================================
  int UT3D_2ncvbsp_orient (int cv1Nr, CurvBSpl **cv1tab,
                           int cv2Nr, CurvBSpl **cv2tab, Memspc *objSeg) {
//=======================================================================
// UT3D_2ncvbsp_orient         orient 2 groups of BSplCurves for surface
// RetCod:    0=OK; -1=curves do not provide a net.

// Both groups constist of b-splines only.
// Both groups are already oriented (have same direction).
// 1) turn all curves of group-2 if necessary;
//      cannot use vectors - does not work with twisted curves.
//      all Curves of group-2 must start at curv-1 of group-1.
// 2) invert order of group-2 if necessary;
//    All Curves of group-2 must start at curv-1 of group-1.
//    First and last obj of group-2 can be a point !


  int      i1, i2, ii, irc;
  long     l1;
  double   da[10];
  Vector   vc1, vc2;
  Point    *pTab, *pt1, *pt2, pa[10];
  CurvBSpl *cv1, *cv2;
  Memspc   tmpSpc=UME_NEW;


  printf("UT3D_2ncvbsp_orient %d %d\n",cv1Nr,cv2Nr);
  for(i1=0;i1<cv1Nr;++i1)UT3D_stru_dump(Typ_CVBSP,cv1tab[i1], "1-%d",i1);
  for(i1=0;i1<cv2Nr;++i1)UT3D_stru_dump(Typ_CVBSP,cv2tab[i1], "2-%d",i1);
  printf(" free tmpSpc=%d\n",UME_ck_free(objSeg));


  // TEST ONLY
  // check size of curve cv1
  cv1 = cv1tab[0];
  OGX_siz_obj (&l1, cv1, Typ_CVBSP, 1, 1);
    printf(" siz cv1=%ld\n",l1);
  UME_alloc_tmp (&tmpSpc, 5000000);
  printf(" free %d\n",UME_ck_free(&tmpSpc));
  // UME_malloc (&tmpSpc, 5550000000,
                       // 5550000000);
// exit(0);
  // TEST ONLY



  //----------------------------------------------------------------
  // turn all curves of group-2 if necessary.

  // loop tru all curves of group-2
  cv1 = cv1tab[0];

  for(i1=0; i1<cv2Nr; ++i1) {
    cv2 = cv2tab[i1];
    // skip points
    if(UT3D_bsp_ck_pt (cv2) == 0) continue;

    // get startpoint of this curve
    pt2 = &cv2->cpTab[0];

    // test if this point lies on cv1
    // get the point nearest to pt2 on cv1
    ii = 10;
    // irc = UT3D_pt_projptbspl (&ii, pa, da, objSeg, cv1, pt2);
    irc = UT3D_pt_projptbspl (&ii, pa, da, &tmpSpc, cv1, pt2);
    if(irc < 0) return -1;
    if(ii < 1) goto L_ck_ep;

    for(i2=0; i2<ii; ++i2) {
      if(UT3D_comp2pt(pt2, &pa[i2], UT_DISP_cv) == 1) goto L_test_2; // 1=ident
    }

    // get endpoint of this curve
    L_ck_ep:
    pt2 = &cv2->cpTab[cv2->ptNr - 1];

    // get the point nearest to pt2 on cv1
    ii = 10;
    irc = UT3D_pt_projptbspl (&ii, pa, da, objSeg, cv1, pt2);
    if(irc < 0) return -1;
    if(ii < 1) return -1;

    for(i2=0; i2<ii; ++i2) {
      if(UT3D_comp2pt(pt2, &pa[i2], UT_DISP_cv) == 1) goto L_turn; // 1=ident
    }

    return -1;


  }








  // turn all curves of group-2
  L_turn:
    printf("UT3D_2ncvbsp_orient:  *** inv all grp2-curves ***\n");
  for(i1=0; i1<cv2Nr; ++i1) {
      // UT3D_stru_dump (Typ_CVBSP, cv2tab[i1], "CV2[%d]",i1);
    cv1 = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cv1 == NULL) return -1;
    ii = UCBS_RevBspCrv (cv1, objSeg, cv2tab[i1]);
    if(ii < 0) return -1;
      // UT3D_stru_dump (Typ_CVBSP, cv1, "CV2R[%d]",i1);
    cv2tab[i1] = cv1;
  }





  //----------------------------------------------------------------
  // invert order of group-2 if necessary.
  L_test_2:

  // test if startpoints fit
  // get startpt of 1. curve of group-1
  cv1 = cv1tab[0];
  pt1 = &cv1->cpTab[0];
    UT3D_stru_dump (Typ_PT, pt1, "  cv0-p0-Grp1");

  // get startpt of 1. curve of group-2
  cv2 = cv2tab[0];
  pt2 = &cv2->cpTab[0];
    UT3D_stru_dump (Typ_PT, pt2, "  cv0-p0-Grp2");
  

  // test if points fit
  if(UT3D_comp2pt(pt1, pt2, UT_DISP_cv) == 1) {   // 1=ident
    return 0;
  }


  // test if startpoint of last curve of group-2 fits
  cv2 = cv2tab[cv2Nr - 1];
  pt2 = &cv2->cpTab[0];
    UT3D_stru_dump (Typ_PT, pt2, "  cvLast-p0-Grp2");


  // test if points fit
  if(UT3D_comp2pt(pt1, pt2, UT_DISP_cv) == 0) {   // 0=not_ident
    return -1;
  }


  // revers order of group-2.
    printf("UT3D_2ncvbsp_orient:  *** invert order of all grp2-curves ***\n");
  MEM_inv_rtab (cv2Nr, cv2tab, sizeof(void*));
  return 0;

}
*/

/* old version until 2011-08-31
//=======================================================================
  int UT3D_2ncvbsp_orient (int cv1Nr, CurvBSpl **cv1tab,
                           int cv2Nr, CurvBSpl **cv2tab, Memspc *objSeg) {
//=======================================================================
// UT3D_2ncvbsp_orient         orient 2 groups of BSplCurves for surface
// RetCod:    0=OK; -1=curves do not provide a net.

// Both groups are already oriented (have same direction).
// -) invert order of group-2 if necessary;
// -) turn all curves of group-2 if necessary;

// All Curves of group-2 must start at curv-1 of group-1.
// First and last obj of group-2 can be a point !

  int      irc, i1, ii;
  Point    ps11, ps21, pe11, pe21;
  CurvBSpl *cvr;


  printf("UT3D_2ncvbsp_orient %d %d\n",cv1Nr,cv2Nr);
    // printf(" mem free %d\n",UME_ck_free(objSeg));


  // get Startpt von 1. Curve of group-1
  UT3D_pt_evalparCv (&ps11, cv1tab[0], cv1tab[0]->v0);
    // UT3D_stru_dump (Typ_PT, &ps11, "ps11");
    // GR_Disp_pt (&ps11, SYM_STAR_S, 3);


  // get Startpt of 1. Curve of group-2
  UT3D_pt_evalparCv (&ps21, cv2tab[0], cv2tab[0]->v0);
    // UT3D_stru_dump (Typ_PT, &ps21, "ps21");
    // GR_Disp_pt (&ps21, SYM_STAR_S, 3);

  if(UT3D_comp2pt(&ps11,&ps21,UT_DISP_cv) == 1) {   // 1=ident
    return 0;
  }


  //----------------------------------------------------------------
  // get endPoint of 1. Curve of group-1
  UT3D_pt_evalparCv (&pe11, cv1tab[0], cv1tab[0]->v1);
    // UT3D_stru_dump (Typ_PT, &pe21, "pe21");


  // if (endPoint of 1. Curve of Schar1)==(Startpt of 1. Curve of Schar2)
  // then die Reihenfolge der Kurven von Schar2 umdrehen; fertig.
  if(UT3D_comp2pt(&pe11,&ps21,UT_DISP_cv) == 1) {   // 1=ident
      // printf("UT3D_2ncvbsp_orient:  *** inv grp2-order ***\n");
    MEM_inv_rtab (cv2Nr, cv2tab, sizeof(void*));
    return 0;
  }


  //----------------------------------------------------------------
  // alle Kurven der Schar2 komplett umdrehen;
    // printf("UT3D_2ncvbsp_orient:  *** inv all grp2-curves ***\n");
  for(i1=0; i1<cv2Nr; ++i1) {
      // UT3D_stru_dump (Typ_CVBSP, cv2tab[i1], "CV2[%d]",i1);
    cvr = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cvr == NULL) return -1;
    ii = UCBS_RevBspCrv (cvr, objSeg, cv2tab[i1]);
    if(ii < 0) return -1;
      // UT3D_stru_dump (Typ_CVBSP, cvr, "CV2R[%d]",i1);
    cv2tab[i1] = cvr;
  }



  // get Startpt of 1. Curve of group-2
  UT3D_pt_evalparCv (&ps21, cv2tab[0], cv2tab[0]->v0);
    // UT3D_stru_dump (Typ_PT, &ps21, "ps21");


  if(UT3D_comp2pt(&ps11,&ps21,UT_DISP_cv) == 1) {   // 1=ident
    return 0;
  }

    // printf("UT3D_2ncvbsp_orient:  *** inv grp2-order ***\n");
  MEM_inv_rtab (cv2Nr, cv2tab, sizeof(void*));


  return 0;

}
*/

//=======================================================================
  int UT3D_ncvbsp_orient (int cv1Nr, CurvBSpl **cv1tab, Memspc *objSeg) {
//=======================================================================
// UT3D_ncvbsp_orient         orient group of B-SplCurves (same direction)
// check vectors between controlPoints 0-1

  int      i1, ii;
  Vector   vc1, vc2;
  Point    *pTab;
  CurvBSpl *cvr;


  // printf("------------------------------------ \n");
  // printf("UT3D_ncvbsp_orient %d\n",cv1Nr);

  if(cv1Nr < 1) return -2;


  // the first and the last curve may be points.
  // get the vector from the middle.
  ii = cv1Nr / 2;
  pTab = cv1tab[ii]->cpTab;
  UT3D_vc_2pt (&vc1, &pTab[0], &pTab[1]);
    // GR_Disp_vc (&vc1, &pTab[0], 9, 0);
    // UT3D_stru_dump (Typ_VC, &vc1, "  curv=%d VC-p0-p1",ii);



  for(i1=0; i1<cv1Nr; ++i1) {
      // UT3D_stru_dump (Typ_CVBSP, cv1tab[i1], "CV[%d]",i1);

    // get vector cp0-cp1
    pTab = cv1tab[i1]->cpTab;
    ii = cv1tab[i1]->ptNr - 1;
    UT3D_vc_2pt (&vc2, &pTab[0], &pTab[ii]);
      // GR_Disp_vc (&vc2, &pTab[0], 9, 0);
      // UT3D_stru_dump (Typ_VC, &vc2, "  curv=%d VC-p0-p1",i1);
    // skip points
    if(UT3D_compvcNull(&vc2)) continue;


    // check for opposite direction
    ii = UT3D_sid_2vc (&vc1, &vc2);
    if(ii >= 0) continue;

      // printf("UT3D_ncvbsp_orient: **** invert curve %d ***\n",i1);
    cvr = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cvr == NULL) return -1;
    ii = UCBS_RevBspCrv (cvr, objSeg, cv1tab[i1]);
    if(ii < 0) return -1;
    cv1tab[i1] = cvr;

  }


  return 0;

}


//=====================================================================
  int UT3D_cbsp_dbo (CurvBSpl **cbsp, Memspc *objSeg, int typ, long dbi,
                     Memspc *tmpSeg) {
//=====================================================================
/// \code
/// UT3D_cbsp_dbo            create B_SplineCurve from Point/Line/Circ/Curve
/// create B_SplineCurve from a single object.
/// \endcode

 
  // int       irc, cvTyp;
  // CurvBSpl  cv1;
  // void      *oStru;

  int       irc;
  ObjGX     oxh;


  // printf("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD \n");
  // printf("UT3D_cbsp_dbo %d %ld\n",typ,dbi);


  // get space for the curve
  *cbsp = UME_reserve (objSeg, sizeof(CurvBSpl));

  // convert obj's -> bspl-curv
  OGX_SET_INDEX (&oxh, typ, dbi);
  irc = UT3D_cbsp_ox (*cbsp, &oxh, 0., objSeg, tmpSeg);
  if (irc < 0) return -1;

  return Typ_CVBSP;

}


//================================================================
  int UT3D_cbsp_trim (CurvBSpl *cvo, CurvBSpl *cvi,
                      Memspc *memDat, Memspc *memTmp) {
//================================================================
/// \code
/// UT3D_cbsp_trim                    trim a bspline-curve
/// IN:  cvi, trimmed by the parameters v0,v1;
/// OUT: the resulting knots go from v0 to v1;
///      exceeding controlpoints are removed.
///      Knotvektor & controlpoints are in memDat.
/// see also UCBS_SubDivBspCrv
/// \endcode


  int        irc, m, r;
  double     tb0, tv0, tb1, tv1;
  void       *vp1;
  CurvBSpl   cvh, cv1, cv2;


  // UT3D_stru_dump (Typ_CVBSP, cvi, "UT3D_cbsp_trim:");

  vp1 = memTmp->next;

  //----------------------------------------------------------------
  L_kv0:
  m = cvi->ptNr + cvi->deg;  // knotvector =: (t0, ..., tm)

  tb0 = cvi->kvTab[0];
  tv0 = cvi->v0;

  tb1 = cvi->kvTab[m];
  tv1 = cvi->v1;
    // printf(" tb0=%f tb1=%f tv0=%f tv1=%f\n",tb0,tb1,tv0,tv1);


  if(tv0 <= tb0) {
    cv1 = *cvi;
    goto L_kv1;
  }


  // get knotvector for 0-v0 -> cvh/memTmp and v0-1 -> cv1/memDat
  cvh.deg = cvi->deg;
  cvh.v0  = tb0;
  cvh.v1  = tv0;

  cv1.deg = cvi->deg;
  cv1.v0  = tv0;
  cv1.v1  = tb1;


  irc = UBS_DivKnotVec (&(cvh.kvTab), memTmp,
                        &(cv1.kvTab), memDat,
                        &r, cvi->kvTab, m, cvi->deg, tv0);
  if (irc < 0) return -2;


  // number of controlpoints of cvh
  cvh.ptNr = r;
  if (tv0 > cvi->kvTab[r]) ++(cvh.ptNr);


  // space for controlpoints of cvh
  cvh.cpTab = memTmp->next;
  irc = UME_add (memTmp, sizeof(Point) * cvh.ptNr);
  if (irc < 0) goto L_outOfSpace;



  // number of controlpoints of cv1
  cv1.ptNr = m - r;

  // space for controlpoints of cv1
  cv1.cpTab = memDat->next;
  irc = UME_add (memDat, sizeof(Point) * cv1.ptNr);
  if (irc < 0) goto L_outOfSpace;


  // divide controlpoints of bspline curve
  irc = UBS_DivCtrlPtsBsp (cvh.cpTab, cv1.cpTab,
                          cvi->deg, cvi->kvTab, cvi->ptNr, cvi->cpTab,
                          tv0, r, 0, 1, memTmp);
  if (irc < 0) return -2;
    // UT3D_stru_dump (Typ_CVBSP, &cv1, "  _cbsp_trim cv1:");



  //----------------------------------------------------------------
  L_kv1:

  if(tv1 >= tb1) {
    *cvo = cv1;
    goto L_exit;
  }


  // get knotvector for 0-v1 -> cv2/memDat and v1-1 -> cvh/memTmp
  cv2.deg = cvi->deg;
  cv2.v0  = tv0;
  cv2.v1  = tv1;

  cvh.deg = cvi->deg;
  cvh.v0  = tv1;
  cvh.v1  = tb1;

  m = cv1.ptNr + cv1.deg;  // knotvector =: (t0, ..., tm)

  irc = UBS_DivKnotVec (&(cv2.kvTab), memDat,
                        &(cvh.kvTab), memTmp,
                        &r, cv1.kvTab, m, cv1.deg, tv1);
  if (irc < 0) return -2;


  // number of controlpoints of cv2
  cv2.ptNr = r;
  if (tv1 > cv1.kvTab[r]) ++(cv2.ptNr);
    // printf(" cv2.ptNr=%d\n",cv2.ptNr);



  // space for controlpoints of cv2
  cv2.cpTab = memDat->next;
  irc = UME_add (memDat, sizeof(Point) * cv2.ptNr);
  if (irc < 0) goto L_outOfSpace;



  // number of controlpoints of cvh
  cvh.ptNr = m - r;
    // printf(" cvh.ptNr=%d\n",cvh.ptNr);

  // space for controlpoints of cvh
  cvh.cpTab = memTmp->next;
  irc = UME_add (memTmp, sizeof(Point) * cvh.ptNr);
  if (irc < 0) goto L_outOfSpace;


  // divide controlpoints of bspline curve
  irc = UBS_DivCtrlPtsBsp (cv2.cpTab, cvh.cpTab,
                          cv1.deg, cv1.kvTab, cv1.ptNr, cv1.cpTab,
                          tv1, r, 0, 1, memTmp);
  if (irc < 0) return -2;
    // UT3D_stru_dump (Typ_CVBSP, &cv2, "  _cbsp_trim cv2:");


  *cvo = cv2;



  //----------------------------------------------------------------
  L_exit:

    memTmp->next = vp1;   // reset memTmp


    // UT3D_stru_dump (Typ_CVBSP, cvo, "ex UT3D_cbsp_trim:");

  return 0;


  L_outOfSpace:
    TX_Error("UT3D_cbsp_trim: out of space");
    return -3;

}


//================================================================
  int UT3D_cbsp_2cbsp (CurvBSpl *cvo, Memspc *objSeg, double smf,
                       CurvBSpl *cv1, CurvBSpl *cv2, Memspc *tmpSeg) {
//================================================================
// UT3D_cbsp_2cbsp            join 2 BSP-Curves


  int      irc;
  Point    p1, p2, p3, p4;
  CurvBSpl *cvr;


  // printf("UT3D_cbsp_2cbsp %f\n",smf);
  // UT3D_stru_dump(Typ_CVBSP, cv1, "cv1:");
  // UT3D_stru_dump(Typ_CVBSP, cv2, "cv2:");


  // get all 4 endpoints
  UT3D_pt_evalparCv(&p1, cv1, cv1->v0);
    // UT3D_stru_dump(Typ_PT, &p1, "p1");
  UT3D_pt_evalparCv(&p2, cv1, cv1->v1);
    // UT3D_stru_dump(Typ_PT, &p2, "p2");
  UT3D_pt_evalparCv(&p3, cv2, cv2->v0);
    // UT3D_stru_dump(Typ_PT, &p3, "p3");
  UT3D_pt_evalparCv(&p4, cv2, cv2->v1);
    // UT3D_stru_dump(Typ_PT, &p4, "p4");


  // test the endpoints
  irc = UT3D_compn4pt (&p1, &p2, &p3, &p4);
    // printf("UT3D_compn4pt irc=%d\n",irc);


  // turn curves if necessary
  if(irc == 2) {         // revert cv1  (p1-p3 nearest)
    cvr = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cvr == NULL) return -1;
    irc = UCBS_RevBspCrv (cvr, objSeg, cv1);
    if (irc < 0) return -1;
    cv1 = cvr;
      // UT3D_stru_dump(Typ_CVBSP, cv1, "cv1-rev:");
    

  } else if(irc == 3) {  // revert cv2  (p2-p4 nearest)
    cvr = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cvr == NULL) return -1;
    irc = UCBS_RevBspCrv (cvr, objSeg, cv2);
    if (irc < 0) return -1;
    cv2 = cvr;
      // UT3D_stru_dump(Typ_CVBSP, cv2, "cv2-rev:");


  } else if(irc == 4) {  // revert cv1 & cv2  (p1-p4 nearest)
    cvr = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cvr == NULL) return -1;
    irc = UCBS_RevBspCrv (cvr, objSeg, cv1);
    if (irc < 0) return -1;
    cv1 = cvr;
    cvr = UME_reserve (objSeg, sizeof(CurvBSpl));
    if(cvr == NULL) return -1;
    irc = UCBS_RevBspCrv (cvr, objSeg, cv2);
    if (irc < 0) return -1;
    cv2 = cvr;
  }


  // join curves ..
  irc = UCBS_Comp2BspCrvs (cvo, objSeg, cv1, cv2, smf, tmpSeg);
  if (irc < 0) return -1;


  return 0;

}


//================================================================
  int UT3D_cbsp_ox (CurvBSpl *cbsp, ObjGX *ox1, double smf,
                     Memspc *objSeg, Memspc *tmpSeg) {
//================================================================
// UT3D_cbsp_ox              create BSP-Curv from ObjGX-obj
// see also UT3D_cbsp_dbo


  int   irc, i1, oTyp, deg, ptNr;
  void  *vp;
  Point *pta;


  // printf("=============================================== \n");
  // printf("UT3D_cbsp_ox typ=%d form=%d\n",ox1->typ,ox1->form);
  // UTO_dump__ (ox1, "");
  // UTO_dump_s_ (ox1, "");



  // get obj-struct
  oTyp = UTO_obj_getp (&vp, &i1, ox1);
  if(oTyp < 0) return -1;
    // printf(" typ=%d form=%d oTyp=%d\n",ox1->typ,ox1->form,oTyp);



  // Point > degenerated Line (2 x same point )
  if(ox1->typ == Typ_PT) {
    irc = UT3D_cbsp_2pt (cbsp, objSeg, (Point*)vp, (Point*)vp);
    // irc = UT3D_cbsp_1pt (cbsp, objSeg, (Point*)vp);


  // ln > cbsp
  } else if(ox1->typ == Typ_LN) {
    // UT3D_stru_dump (Typ_LN, vp, "LN:\n");
    irc = UT3D_cbsp_2pt (cbsp, objSeg, &((Line*)vp)->p1, &((Line*)vp)->p2);


  // circ > cbsp
  } else if(ox1->typ == Typ_CI) {
    // UT3D_stru_dump (Typ_CI, vp, "AC:\n");
    irc = UT3D_cbsp_ci (cbsp, objSeg, (Circ*)vp, tmpSeg);


  // already bsp (copy)
  } else if(ox1->typ == Typ_CV) {


    //----------------------------------------------------------------
    if(oTyp == Typ_CVBSP) {

      // test if spline is limited
      if(UT3D_cbsp_ck_trim ((CurvBSpl*)vp)) {
        // change into untrimmend spline
        UT3D_cbsp_trim (cbsp, (CurvBSpl*)vp, objSeg, tmpSeg);
      } else {
        *cbsp = *((CurvBSpl*)vp);
      }
      irc = 0;


    //----------------------------------------------------------------
    } else if(oTyp == Typ_CVPOL) {


      // test if polygon is limited
      ptNr = UT3D_ck_plg_trim ((CurvPoly*)vp);
        // printf(" ptnr-plg=%d\n",ptNr);
      if(ptNr > 0) {
        // get points of trimmed polygon
        pta = MEM_alloc_tmp ((int)(ptNr  * sizeof(Point)));
        irc = UT3D_pta_plg (&ptNr, pta, (CurvPoly*)vp);
          // for(i2=0;i2<i1;++i2)UT3D_stru_dump(Typ_PT,&pta[i2]," pta[%d]:",i2);
      } else {
        ptNr = ((CurvPoly*)vp)->ptNr;
        pta = ((CurvPoly*)vp)->cpTab;
      }

      if(smf < -0.5) deg = 1;
      else          deg = 2;

      // create bspline of polygon pta
      if(deg == 1)  goto L_pol_1;
      if(smf >= 1.) goto L_pol_2;

      if(((CurvPoly*)vp)->ptNr > 10) goto L_pol_2;

      L_pol_1:
      i1 = 0;  // 0=Standard; 1=optimized;
      // 1=optimized=genauer, aber f GordonSurfs nicht verwendbar).
      irc = UCBS_BspCrvPts (cbsp, objSeg, ptNr, pta, deg, i1, tmpSeg);
            // ((CurvPoly*)vp)->ptNr, ((CurvPoly*)vp)->cpTab, deg, i1, tmpSeg);
      goto L_done;


      L_pol_2:
      irc = UT3D_bsp_pta__ (cbsp, ptNr, pta, UT_TOL_cv, objSeg, tmpSeg);
                            // ((CurvPoly*)vp)->ptNr, ((CurvPoly*)vp)->cpTab,
                            // UT_TOL_cv, objSeg, tmpSeg);
        // UT3D_stru_dump (Typ_CVBSP, cbsp, " cvApprox:");


    //----------------------------------------------------------------
    } else if(oTyp == Typ_CVELL) {
      irc = UT3D_cbsp_ell (cbsp, objSeg, (CurvElli*)vp, tmpSeg);


    } else if(oTyp == Typ_CVCLOT) {
      irc = UT3D_cbsp_clt (cbsp, objSeg, (CurvClot*)vp, tmpSeg);


    } else {
      TX_Error("UT3D_cbsp_ox E002 %d %d",ox1->typ,oTyp);
      irc = -1;
    }



  //------------------------------------------
  } else {
    TX_Error("UT3D_cbsp_ox E001 %d %d",ox1->typ,ox1->form);
    // TX_Print("UT3D_cbsp_dbo not yet supported typ %s",DB_Char_Typ(typ));
    irc = -1;
  }


  L_done:
    // UT3D_stru_dump (Typ_CVBSP, cbsp, "ex UT3D_cbsp_ox:");

  return irc;

}


//=========================================================================
  int UT3D_cbsp_1pt (CurvBSpl *bsp, Memspc *memSeg, Point *p1) {
//=========================================================================
// UT3D_cbsp_1pt              create BSP-Curv from point
// deg = 1

  int   irc;

  // set v0,v1
  bsp->ptNr = 1;
  bsp->deg  = 1;
  bsp->v0   = 0.;
  bsp->v1   = 1.;

  // create cpTab
  bsp->cpTab = memSeg->next;
  irc = UME_add (memSeg, sizeof(Point));
  bsp->cpTab[0] = *p1;
  if(irc < 0) return -1;
    
  // create kvTab
  bsp->kvTab = NULL;

    // UT3D_stru_dump (Typ_CVBSP, bsp, "ex UT3D_cbsp_2pt:");
    // GR_Disp_CvBSp (bsp, 9, memSeg);

  return 0;

}


//=========================================================================
  int UT3D_cbsp_2pt (CurvBSpl *bsp, Memspc *memSeg, Point *p1, Point *p2) {
//=========================================================================
// create BSP-Curv from Line
// deg = 1

  int   irc;

  // set v0,v1
  bsp->ptNr = 2;
  bsp->deg  = 1;
  bsp->v0   = 0.;
  bsp->v1   = 1.;

  // create cpTab
  bsp->cpTab = memSeg->next;
  irc = UME_add (memSeg, sizeof(Point) * 2);
  bsp->cpTab[0] = *p1;
  bsp->cpTab[1] = *p2;
  if(irc < 0) return -1;

  // create kvTab
  bsp->kvTab = memSeg->next;
  irc = UME_add (memSeg, sizeof(double) * 4);
  if(irc < 0) return -1;
  bsp->kvTab[0] = 0.;
  bsp->kvTab[1] = 0.;
  bsp->kvTab[2] = 1.;
  bsp->kvTab[3] = 1.;
  // bspl_knotvec__ (kvTab, ptNr, deg);

    // UT3D_stru_dump (Typ_CVBSP, bsp, "ex UT3D_cbsp_2pt:");
    // GR_Disp_CvBSp (bsp, 9, memSeg);

  return 0;

}


//=============================================================================
  int UT3D_cbsp_ci (CurvBSpl *bsp, Memspc *memSeg, Circ *ci1, Memspc *tmpSeg) {
//=============================================================================
// create BSP-Curv from Circ

  int    ptNr, ptmax;
  Point  *pTab;
  void   *memPos;


  // UT3D_stru_dump (Typ_CI, ci1, "UT3D_cbsp_ci:");

  // save tmpSeg-pos
  memPos = UME_get_next (tmpSeg);

  // put pTab into tmpSeg
  pTab = tmpSeg->next;
  ptmax = UME_ck_free(tmpSeg) / sizeof(Point);

  // umwandeln in normales Polygon
  UT3D_cv_ci (pTab, &ptNr, ci1, ptmax, UT_DISP_cv);
    // GR_Disp_pTab (ptNr, pTab, SYM_STAR_S, 9);

  // update tmpSeg
  UME_add (tmpSeg, sizeof(Point) * ptNr);


  // Polygon > B-Spline ...
  UCBS_BspCrvPts (bsp, memSeg, ptNr, pTab, 2, 0, tmpSeg);

  // release workspace
  UME_adjust (tmpSeg, memPos);

    // UT3D_stru_dump (Typ_CVBSP, bsp, "ex UT3D_cbsp_ci:");
    // GR_Disp_CvBSp (bsp, 9, memSeg);

  return 0;

}


//=============================================================================
  int UT3D_cbsp_clt (CurvBSpl *bsp, Memspc *memSeg,
                     CurvClot *cv1,
                     Memspc *tmpSeg) {
//=============================================================================
// UT3D_cbsp_clt         create BSP-Curv from Clothiod

  int     i1, irc, ptNr;
  Point   *pTab;
  void    *memPos;

  memPos = UME_get_next (tmpSeg);

  // get polgon from clot
  irc = UCV_PlgCltCrv (&ptNr, &pTab, memSeg, cv1, tmpSeg);
  if(irc < 0) return -1;
    // printf(" irc=%d ptNr=%d\n",irc,ptNr);
    // for(i1=0;i1<ptNr;++i1)UT3D_stru_dump(Typ_PT,&pTab[i1],"  p[%d]",i1);

  // Polygon > B-Spline ...
  irc = UCBS_BspCrvPts (bsp, memSeg, ptNr, pTab, 2, 0, tmpSeg);

  // release workspace
  UME_adjust (tmpSeg, memPos);


  return irc;

}


//===============================================================================
  int UT3D_cbsp_parl_pln (CurvBSpl *cvo,
                          long dbi, int mdli, Vector *vz, double dist,
                          Memspc *memSeg1, Memspc *tmpSeg) {
//===============================================================================
// UT3D_cbsp_parl_pln         Bspl-Curv parallel to Bspl-Curv on plane, dist
// Output:
//   cvo          outCurve,  all data for cvo is in memSeg1


  int      irc, ii, ptNr, ptNo, iClo;
  CurvBSpl *cvi;
  Point    *pta, *pTabi;


  printf("UT3D_cbsp_parl_pln dbi=%ld\n",dbi);
  // UT3D_stru_dump (Typ_CVBSP, cvi, "UT3D_cbsp_parl_pln");


  DB_GetObjDat ((void**)&cvi, &ii, Typ_CV, dbi);

  // iClo = cvi->clo;
  iClo = UT3D_bsp_ck_closed_tr (cvi);


  // get the PRCE for this curve
  irc = PRCV_npt_dbo__ (&pTabi, &ptNr, Typ_CV, dbi, mdli);
    printf(" _npt_dbo__ irc=%d ptNr=%d\n",irc,ptNr);

    // TESTBLOCK
    // GL_view_npt (pTabi, ptNr, ATT_PT_GREEN, -1L);
    // END TESTBLOCK


  // get spc f pTabo in tmpSeg
  pta = UME_reserve (tmpSeg, sizeof(Point) * ptNr);
  if(!pta) {
    TX_Error("Out of tempSpace in UT3D_plg_parl_pln");
    return -1;
  }


  // offset pTabi -> pta
  irc = UT3D_npt_parl_pln (pta, &ptNo, pTabi, ptNr, vz, dist, iClo);
  if(irc < 0) return -1;

    // TESTBLOCK
    // GL_view_npt (pta, ptNo, ATT_PT_GREEN, -1L);
    // END TESTBLOCK


  // get bspl-curv from polygon
  UT3D_bsp_pta__ (cvo, ptNo, pta, UT_TOL_cv, memSeg1, tmpSeg);


    UT3D_stru_dump (Typ_CVBSP, cvo, " ex-UT3D_cbsp_parl_pln");

  return 0;

}


//=============================================================================
  int UT3D_cbsp_ell (CurvBSpl *bsp, Memspc *memSeg,
                     CurvElli *el1,
                     Memspc *tmpSeg) {
//=============================================================================
// create BSP-Curv from Ellipse

  int    ptNr, ptmax;
  Point  *pTab;
  void   *memPos;


  // UT3D_stru_dump (Typ_CVELL, el1, "UT3D_cbsp_ell:");

  // save tmpSeg-pos
  memPos = UME_get_next (tmpSeg);

  // put pTab into tmpSeg
  pTab = tmpSeg->next;
  ptmax = UME_ck_free(tmpSeg) / sizeof(Point);

  // umwandeln in normales Polygon
  ptNr = UT3D_ptNr_ell (el1, UT_DISP_cv);
  if(ptNr > ptmax) goto L_e1;
  UT3D_npt_ell (ptNr, pTab, el1);
  // UT3D_cv_ell (pTab, &ptNr, el1, ptmax, UT_DISP_cv);
    // GR_Disp_pTab (ptNr, pTab, SYM_STAR_S, 9);

  // update tmpSeg
  UME_add (tmpSeg, sizeof(Point) * ptNr);


  // Polygon > B-Spline ...
  UCBS_BspCrvPts (bsp, memSeg, ptNr, pTab, 2, 0, tmpSeg);

  // release workspace
  UME_adjust (tmpSeg, memPos);

    // UT3D_stru_dump (Typ_CVBSP, bsp, "ex UT3D_cbsp_ell:");
    // GR_Disp_CvBSp (bsp, 9, memSeg);

  return 0;

  L_e1:
    TX_Error("UT3D_cbsp_ell EOM"); return -1;

}


//================================================================
  int UT3D_bsp_infTg (int *dNr, double *dTab, CurvBSpl *bsp1) {
//================================================================
// UT3D_bsp_infTg Eine Tabelle der Wendepunkte liefern (nur die Parameter)
// Stimmt nur ungefaehr !
// Es werden die Wendepunkte des Kontrollpolygons gesucht, die parameter dazu
// bestimmt (aber nur ungefaehr; ohne Beruecksichtugung des Degree).
// Input:
//   dNr     size of dTab
// RetCod:   0=OK;  -1=degenereated: no start- and endVector.


  int       i1, i2, dSiz;
  double    d1;
  Vector    v1, v2, v3, v4, v5;


  // printf("UT3D_bsp_infTg................................%d\n",*dNr);

  dSiz = *dNr;
  *dNr = 0;


  UT3D_vc_2pt (&v2, &bsp1->cpTab[0], &bsp1->cpTab[1]);
  UT3D_vc_2pt (&v3, &bsp1->cpTab[1], &bsp1->cpTab[2]);
    // UT3D_stru_dump (Typ_VC, &v2, " v2:");
    // UT3D_stru_dump (Typ_VC, &v3, " v3:");
  if((UT3D_compvcNull(&v2))&&(UT3D_compvcNull(&v3))) return -1; // 2011-08-17 RF


  UT3D_vc_setLength (&v2, &v2, 1.);
  UT3D_vc_setLength (&v3, &v3, 1.);
      // GR_Disp_vc (&v2, &bsp1->cpTab[0], 11, 0);
      // GR_Disp_vc (&v3, &bsp1->cpTab[1], 11, 0);


  for(i1=3; i1<bsp1->ptNr; ++i1) {
    // printf(" i1=%d\n",i1);
    // v1, v2, v3
    // v1, v2, v3 mit 3 aufeinanderfolgenden vecs beladen ..
    v1 = v2;
    v2 = v3;
    UT3D_vc_2pt (&v3, &bsp1->cpTab[i1-1], &bsp1->cpTab[i1]);
    UT3D_vc_setLength (&v3, &v3, 1.);
      // GR_Disp_vc (&v3, &bsp1->cpTab[i1-1], 12, 0);

    // skip test wenn v1-v2 parallel
    if(UT3D_vc_ck_parl_vc(&v1, &v2, RAD_1) == 1) continue;

    // die 2 Kreuzprodukte bilden
    UT3D_vc_perp2vc (&v4, &v1, &v2);
    UT3D_vc_perp2vc (&v5, &v2, &v3);
      // GR_Disp_vc (&v4, &bsp1->cpTab[i1-2], 11, 0);
      // GR_Disp_vc (&v5, &bsp1->cpTab[i1-1], 11, 0);

    // Winkelunterschied der beiden Vektoren;
    // bei mehr als 90 Grad Differenz: Wendepunkt.
    d1 = UT3D_acos_2vc (&v4, &v5);
    if(d1 > 0.) continue;


    // printf(" Wendepunkt an P[%d]\n",i1-1);
    // Punkt #[i1-1] ist ein Wendepunkt;
    // zugehoerigen Parameterwert feststellen
    // kvNr = ptNr+deg+1
    i2 = (bsp1->deg/2) + i1;
    d1 = (bsp1->kvTab[i2-1] + bsp1->kvTab[i2]) / 2.;
    // printf(" par [%d]=%f\n",i2,d1);

    // save d1 in dTab
    if(*dNr >= dSiz) {TX_Error("UT3D_bsp_infTg EOM"); return -1;}

    dTab[*dNr] = d1;
    ++(*dNr);
  }


  if(*dNr < 1) goto L_done;


  // remove all values outside v0/v1;
  // und wenn die Kurve verkehrt liegt, die Tabelle umdrehen

  if(bsp1->v0 < bsp1->v1) {   // normal aufsteigend ..
    // remove all values outside v0/v1
    for(i1=0;i1<*dNr;++i1) {
      if(dTab[i1] > bsp1->v1) {*dNr = i1; break;}
    }
    i2 = -1;
    for(i1=0;i1<*dNr;++i1) if(dTab[i1] < bsp1->v0) i2 = i1;
    if(i2 >= 0) MEM_del_nrec (dNr, dTab, 0, i2+1, sizeof(double));

  } else {                    // verkehrt ..
    // remove all values outside v0/v1
    for(i1=0;i1<*dNr;++i1) {
      if(dTab[i1] < bsp1->v1) {*dNr = i1; break;}
    }
    i2 = -1;
    for(i1=0;i1<*dNr;++i1) if(dTab[i1] > bsp1->v0) i2 = i1;
    if(i2 >= 0) MEM_del_nrec (dNr, dTab, 0, i2+1, sizeof(double));
    // remove all values outside v0/v1
    MEM_inv_dtab (*dNr, dTab);
  }



  L_done:
  // display points (Wendepunkte)
  // Point pt1; for(i1=0;i1<*dNr;++i1) {
  // UT3D_pt_evparCrvBSpl (&pt1, bsp1, dTab[i1]);
  // GR_Disp_pt (&pt1, SYM_STAR_S, 2); }
  // printf("ex UT3D_bsp_infTg %d\n",*dNr);


  return 0;

}


//================================================================
  int UT3D_bsp_degrad (ObjGX *ox1, CurvBSpl *cv1, Memspc *memSeg1) {
//================================================================
// UT3D_bsp_degrad            degrade bsp to line

  int     irc;
  Line    *ln1;


  // UT3D_stru_dump (Typ_CVBSP, cv1, "UT3D_bsp_degrad");


  // add Line to memSpc
  ln1 = (Line*)memSeg1->next;
  irc = UME_add (memSeg1, sizeof(Line));
  if(irc < 0) return -1;

  // mit ps, pe begrenzen
  UT3D_pt_evparCrvBSpl (&ln1->p1, cv1, cv1->v0);
  UT3D_pt_evparCrvBSpl (&ln1->p2, cv1, cv1->v1);


  // *ox1 = UTO_stru_2_obj (Typ_LN, Typ_LN, (void*)ln1);
  ox1->typ  = Typ_LN;
  ox1->form = Typ_LN;
  ox1->siz  = 1;
  ox1->data = (void*)ln1;




  // UT3D_stru_dump (Typ_LN, ln1, "ex UT3D_bsp_degrad");

  return 0;
}


//================================================================
  int UT3D_bsp_ck_pt (CurvBSpl *cv1) {
//================================================================
/// UT3D_bsp_ck_pt       check if B-SplineCurve is a point
/// RetCod:   0=yes, cv1 is a point; -1=no.

  int   i1;
  Point  *p1, *p2;

  // UT3D_stru_dump (Typ_CVBSP, cv1, "UT3D_bsp_ck_pt:");

  p1 = &cv1->cpTab[0];
  for(i1 = 1; i1 < cv1->ptNr; ++i1) {
    p2 = &cv1->cpTab[i1];
    if(UT3D_comp2pt(p1, p2, UT_TOL_pt) == 0) return -1;
    p1 = p2;
  }
 
  return 0;

}



//=========================================================
  int UT3D_bsp_ck_closed__ (CurvBSpl *cv1) {
//=========================================================
/// \code
/// UT3D_bsp_ck_closed__      check if B-SplCurv is closed (ignores trimmed)
/// Retcod:
/// rc =  1:  NO, not closed
/// rc =  0:  YES, curve is closed
/// rc = -2:  degenerated (shorter UT_TOL_cv)
/// 
/// \endcode

  int     i1, ie, irc;
  double  d1;

  ie = cv1->ptNr - 1;

  // test if first-point == last-point
  if(UT3D_comp2pt(&cv1->cpTab[0], &cv1->cpTab[ie], UT_TOL_cv)) {

    if(ie < 2) return -2;   // 2 points only, degen !

    // find 2 points with a dist > UT_TOL_cv
    for(i1 = 2; i1 <= ie; ++i1) {
      // get max dist between 2 points
      d1 = UT3D_lenB_2pt (&cv1->cpTab[i1 - 1], &cv1->cpTab[i1]);
      if(d1 > UT_TOL_cv) return 0; // closed
    }

    return -1; // degenerated

  }  

  return 1; // not closed

}


//=========================================================
  int UT3D_bsp_ck_closed_tr (CurvBSpl *cv1) {
//=========================================================
/// \code
/// UT3D_bsp_ck_closed_tr     check if B-SplCurv is closed and untrimmed
/// Retcod:
/// rc =  1:  NO, not closed
/// rc =  0:  YES, curve is closed AND endpoint == end_of_curve (untrimmed)
/// rc = -1:  NO, curve is closed BUT endpoint is trimmed
/// rc = -2:  degenerated
/// \endcode
// see also UT3D_rbspl_ck_closed UTO_cv_ck_clo

  int     irc, ike;
  double  dv, d1, vTot;
  // double  d_ae, d_am;


    // feststellen ob geschlossen
    // letzter Punkt der ersten Kurve
    // ie = cv1->ptNr - 1;
      // UT3D_stru_dump (Typ_PT,  &cv1->cpTab[i1], " p[i1]");


//  UT_DISP_cv GEHT NICHT, wenn gesamte Kurve viel zu klein.
    // test if first-point == last-point
  irc = UT3D_bsp_ck_closed__ (cv1);
  if(irc) goto L_exit;  // printf(" not closed ..\n");


    // yes closed (first and last point is identical)

    // test if curve is trimmed ..
    ike = cv1->ptNr + cv1->deg;
    vTot = cv1->kvTab[ike] - cv1->kvTab[0];
    dv = cv1->v1 - cv1->v0;
    // compute modulo vTOT
    d1 = fmod (dv, vTot);
      // printf(" d1=%lf dv=%lf, vTot=%lf\n",d1,dv,vTot);

    if (fabs(d1) < UT_TOL_min0) irc = 0;
    else                        irc = -1;

/*
    if((UTP_comp2db(cv1->v0, cv1->kvTab[0], UT_TOL_min0) == 0)   ||    //0=diff
       (UTP_comp2db(cv1->v1, cv1->kvTab[ike], UT_TOL_min0) == 0))   {
      irc = -1;     // printf(" not closed (trimmed)..\n");
      goto L_exit;
    }
    irc = 0;   // closed, not trimmed
*/
/*
  // Punkt in der Mitte
  im = (cv1->ptNr - 1) / 2;
  // Test, ob der mittlere punkt weiter weg ist als der letzte
  d_ae = UT3D_lenB_2pt (&cv1->cpTab[0], &cv1->cpTab[ie]);
  d_am = UT3D_lenB_2pt (&cv1->cpTab[0], &cv1->cpTab[im]);
  // Closed, wenn (d_am > d_ae) && (d_ae < UT_DISP_cv)
  if((d_am > d_ae)&&(d_ae < UT_DISP_cv)) irc = 0; // yes, closed
  else                                   irc = 1; // not closed
*/

  L_exit:
    // printf("ex UT3D_bsp_ck_closed_tr %d\n",irc);
  return irc;

}


//================================================================
  int UT3D_bsp_ck_planar (CurvBSpl *cv1, Vector *vz) {
//================================================================
/// UT3D_bsp_ck_planar         check if spline is planar to given vector
/// Input:
///   vc         normalvector of plane
/// RetCod = 0   OK, ist planar
///         -1   no, 1-n points of pTab are not in plane pPln-vc


  Point   *pa;

  UT3D_stru_dump (Typ_VC, vz, "UT3D_bsp_ck_planar");
  
  pa = cv1->cpTab;

  return UT3D_pta_ck_planar (cv1->ptNr - 1, pa, &pa[1], vz);

}


//================================================================
  int UT3D_cbsp_ck_trim (CurvBSpl *cv1) {
//================================================================
/// \code
/// UT3D_cbsp_ck_trim          test is spline is trimmed
///   RetCod
///      0         polygon is unlimited
///     >0         polygon is limited;  nr of points (not exactly)
///
/// see also UT3D_cbsp_trim
/// \endcode

  int    i1;


  // printf("UT3D_cbsp_ck_trim %f %f\n",cv1->v0,cv1->v1);


  if(UTP_comp2db (cv1->kvTab[0], cv1->v0, UT_TOL_pt) == 0) goto L_lim;
  i1 = cv1->ptNr + cv1->deg;
  if(UTP_comp2db (cv1->kvTab[i1], cv1->v1, UT_TOL_pt) == 0) goto L_lim;
  // polygon is unlimited
  return 0;


  // polygon is limited; compute nr of points
  L_lim:
    // printf("ex UT3D_cbsp_ck_trim 1\n");

  return 1;

}


//================================================================
  int UT3D_bsp_ck_minSiz (CurvBSpl *cv1) {
//================================================================
// UT3D_bsp_ck_minSiz         check for minSiz / underSize
// wenn alle Kontrollpunktabstaende < disp_cv ?
// irc = 0 = cv is > minSiz (normal)
// irc = 1 0 cv is too small (all distances < UT_TOL_cv)
// degenerate to Line or Polygon ?


  int     irc, i1;
  double  d1;

  // UT3D_stru_dump (Typ_CVBSP,  cv1, "UT3D_bsp_ck_minSiz");
  // printf(" TOL_pt=%f TOL_cv=%f\n",UT_TOL_pt,UT_TOL_cv);

  for(i1=1; i1<cv1->ptNr; ++i1) {
    d1 = UT3D_lenB_2pt (&cv1->cpTab[i1-1], &cv1->cpTab[i1]);
    // printf(" d1[%d]=%f\n",i1,d1);
    if(d1 > UT_TOL_cv) return 0;
  }

  // printf("ex UT3D_bsp_ck_minSiz - cv is too small\n");
  return 1;

}


//=========================================================================
  int UT3D_bsp_cpsbsp (CurvBSpl *cvo, SurBSpl *su1, char cDir, int cvNr,
                        Memspc *memSeg1) {
//=========================================================================
// copy Bsp-Curve out of Bsp-Surf (nicht interpolierend !)


  int    irc, i1, i2, iInd, iInc;

// ptNr    ... number of control points
// deg     ... degree of B-spline curve
// v0      ... B-spline curve start parameter
// v1      ... B-spline curve end parameter
// kvTab[ptNr+deg+1] knot values (non-decreasing, <= v0 < v1 <= )
// cpTab[ptNr]       control points


  if(cDir != 'U') goto L_copy_V;

  // make space for ptVNr controlpoints
  cvo->cpTab = (Point*)memSeg1->next;
  i1 = su1->ptVNr;
  irc = UME_add (memSeg1, sizeof(Point)*i1);
  if(irc < 0) return -1;

  // copy controlpoints
  iInd = cvNr;                  // V: ?
  iInc = su1->ptUNr;            // V: 1
  for(i2=0; i2<i1; ++i2) {
    cvo->cpTab[i2] = su1->cpTab[iInd];
    iInd += iInc;
    // printf(" p[%d] <= %d\n",i2,iInd);
    // GR_Disp_pt (&cvo->cpTab[i2], SYM_STAR_S, 0);
  }

  // make space for knotvalues
  cvo->kvTab = (double*)memSeg1->next;
  i1 = su1->ptVNr + su1->degV + 1;
  irc = UME_add (memSeg1, sizeof(double)*i1);
  if(irc < 0) return -1;

  // copy knotvalues
  for(i2=0; i2<i1; ++i2) {
    cvo->kvTab[i2] = su1->kvTabV[i2];
  }

  cvo->ptNr = su1->ptVNr;
  cvo->deg  = su1->degV;
  cvo->v0   = cvo->kvTab[0];
  cvo->v1   = cvo->kvTab[i1-1];

  // UT3D_stru_dump (Typ_CVBSP, cvo, "");
  // GR_Disp_CvBSp (cvo, 2, memSeg1);


  goto L_fertig;



  //================================================================
  L_copy_V:

  // make space for ptUNr controlpoints
  cvo->cpTab = (Point*)memSeg1->next;
  i1 = su1->ptUNr;
  irc = UME_add (memSeg1, sizeof(Point)*i1);
  if(irc < 0) return -1;

  // copy controlpoints
  iInd = cvNr * su1->ptUNr;
  iInc = 1;
  for(i2=0; i2<i1; ++i2) {
    cvo->cpTab[i2] = su1->cpTab[iInd];
    iInd += iInc;
    // printf(" p[%d] <= %d\n",i2,iInd);
    // GR_Disp_pt (&cvo->cpTab[i2], SYM_STAR_S, 0);
  }

  // make space for knotvalues
  cvo->kvTab = (double*)memSeg1->next;
  i1 = su1->ptUNr + su1->degU + 1;
  irc = UME_add (memSeg1, sizeof(double)*i1);
  if(irc < 0) return -1;

  // copy knotvalues
  for(i2=0; i2<i1; ++i2) {
    cvo->kvTab[i2] = su1->kvTabU[i2];
  }

  cvo->ptNr = su1->ptUNr;
  cvo->deg  = su1->degU;
  cvo->v0   = cvo->kvTab[0];
  cvo->v1   = cvo->kvTab[i1-1];

  // UT3D_stru_dump (Typ_CVBSP, cvo, "");
  // GR_Disp_CvBSp (cvo, 2, memSeg1);




  //================================================================
  L_fertig:
  return 0;

}


/*=======================================================================*/
  int UT3D_pt_intlnbspl (int *nxp, Point *xptab,
                         Memspc *memSeg1, Line *ln, CurvBSpl *bspl) {
/*=====================
UT3D_pt_intlnbspl    intersect line with b-spline curve

liefert nix wenn ausserhalb ...
ev Curve linear verlaengern ?


UT3D_pt_intlnbspl    Author: Thomas Backmeister       26.5.2003

Intersection of line with b-spline curve.

IN:
  int *nxp         ... max Nr of outputPoints
  Line *ln         ... line
  CurvBSpl *_bspl  ... b-spline curve
  Memspc *memSeg1  ... space for temp. data
OUT:
  int *nxp         ... number of intersection points
  Point *xptab     ... intersection points
Returncode:
  0 = OK
 -1 = out of mem (Memspc too small)
 -2 = out of mem (xptab too small)
*/

  int     nbcv, i1, i2, irc, ptxNr, xptSiz;
  ObjGX   bezTab;
  CurvBez *bcvtab;
  Point   pTab1[UT_BEZDEG_MAX+1];
  void    *memPos0, *memPos1;


  // printf("UT3D_pt_intlnbspl: bbbbbbbbbbbbbbbbbbbbbbbb\n");


  // init
  xptSiz  = *nxp;
  *nxp = 0;
  memPos0 = memSeg1->next;


  // Bezier curves from b-spline curve
  UT3D_bez_bspl__ (&bezTab, memSeg1, bspl);


  // intersection of line with Bezier curves
  bcvtab = bezTab.data;
  memPos1 = memSeg1->next;   // merken; ab hier nur tempData

  // loop durch die Curves
  for (i1=0; i1<bezTab.siz; ++i1) {
    // printf(" >>>>>>>>>>>>>>>>>> test bcvtab[%d]\n",i1);

      // GR_Disp_bez (&bcvtab[i1], 7, memSeg1);
    
    ptxNr = 0;
    irc = UT3D_pt_intlnbcv (&ptxNr, pTab1, UT_BEZDEG_MAX,
                            ln, &bcvtab[i1], memSeg1, UT_TOL_cv);
    if(irc < 0) goto Fertig;

    // die neuen Schnittpunkte >  xptab zufuegen
    if(*nxp + ptxNr >= xptSiz) {
       TX_Error("UT3D_pt_intlnbspl E002");
       irc = -2;
       goto Fertig;
    }
    for(i2=0; i2<ptxNr; ++i2) {
      xptab[*nxp] = pTab1[i2];
      ++(*nxp);
    }

    // reset memSeg1 (UT3D_pt_intlnbcv braucht nur temp. Objekte)
    memSeg1->next = memPos1;
  }



  Fertig:
  // reset memSeg1
  memSeg1->next = memPos0;


  return irc;
}


//=====================================================================
  int UT3D_pt_intplbspl (int *pNr, Point *pa,
                      Plane *pln, CurvBSpl *bspl, Memspc *memSeg1, double tol) {
//=====================================================================
// UT3D_pt_intplbspl    Intersect Plane / BSplineCurve

// Input:
// pNr = max.Nr.inters.points/parameters

// Output:
// pNr = Nr.inters.points/parameters

// via Bezier und inters. Ln/Plane ?
// oder eine Ableitung der Curve wo die Nullstellen (Durchgaenge durch die
// Plane leicht auffindbar sind ??? (Plane transformieren oder ableiten ??)
// geht lt Karls S nur iterativ !
// see UT3D_pt_intlnbspl



  int     irc, i1, pSiz, p1Nr;
  ObjGX   bezTab;
  CurvBez *bcvtab;
  void    *memPos0, *memPos1;


  // printf("UT3D_pt_intplbspl tol=%f\n",tol);
  // UT3D_stru_dump (Typ_PLN,   pln, "");
  // UT3D_stru_dump (Typ_CVBSP, bspl, "B-Spl:");


  // init
  pSiz  = *pNr;
  *pNr  = 0;
  memPos0 = memSeg1->next;



  // B-Spline umwandeln in mehrere Bezier-Segmente
  UT3D_bez_bspl__ (&bezTab, memSeg1, bspl);
  // UT3D_stru_dump (Typ_ObjGX, &bezTab, "bezTab");


  bcvtab  = bezTab.data;
  memPos1 = memSeg1->next;   // merken; ab hier nur tempData

    // nur Test:
    // GR_Disp_bez (&bcvtab[0], 2, memSeg1);
    // *pNr = pSiz;
    // UT3D_pt_intplbez (pNr, pa, pln, &bcvtab[2], memSeg1, tol);
    // goto Fertig;



  //----------------------------------------------------------------
  // Loop durch die BezCurves
  for (i1=0; i1<bezTab.siz; ++i1) {

    // GR_Disp_bez (&bcvtab[i1], 2, memSeg1);

    // Schnittpunkte Plane / Beziersegment errechnen
    // see UT3D_pt_intlnbcv UT3D_pt_projptbez UT3D_pt_int2bcv ..
    p1Nr = pSiz - *pNr;
    irc = UT3D_pt_intplbez (&p1Nr, &pa[*pNr], pln, &bcvtab[i1], memSeg1, tol);
    if(irc < 0) return irc;
    *pNr += p1Nr;

    // reset memSeg1 (UT3D_pt_intlnbcv braucht nur temp. Objekte)
    memSeg1->next = memPos1;
  }



  Fertig:
  // reset memSeg1
  memSeg1->next = memPos0;


  // GR_Disp_pTab (*pNr, pa, SYM_STAR_S, 2);

  return 0;

}


/*=======================================================================*/
  int UT3D_pt_int2bspl (int *nxp, Point *xptab,
                        Memspc *memSeg1, CurvBSpl *bspl1, CurvBSpl *bspl2) {
/*=====================
UT3D_pt_int2bspl     intersect 2 b-spline curves

UT3D_pt_int2bspl     Author: Thomas Backmeister       20.5.2003

Intersection of two b-spline curves.


IN:
  int *nxp         ... max Nr of outputPoints
  CurvBSpl *bspl1  ... b-spline curve 1
  CurvBSpl *bspl2  ... b-spline curve 2
  Memspc *memSeg1  ... space for temp. data; bis zu 100 KB erforderlich !!
OUT:
  int *nxp         ... number of intersection points
  Point *xptab     ... intersection points
Returncode:
  0 = OK
 -1 = out of mem (Memspc too small)
 -2 = out of mem (xptab too small)
*/


  int     nbcv1, nbcv2, i1, i2, i3, irc, ptxNr, xptSiz;
  ObjGX   bezTab1, bezTab2;
  CurvBez *bcvtab1, *bcvtab2;
  Point   pTab1[UT_BEZDEG_MAX+1];
  void    *memPos0, *memPos1;


  // printf("UT3D_pt_int2bspl\n");

  memPos0 = memSeg1->next;


  // Bezier curves from each b-spline curve
  irc = UT3D_bez_bspl__ (&bezTab1, memSeg1, bspl1);
  if(irc < 0) goto Fertig;
  // UME_dump(memSeg1, " n. bez1: ");
  irc = UT3D_bez_bspl__ (&bezTab2, memSeg1, bspl2);
  if(irc < 0) goto Fertig;
  // UME_dump(memSeg1, " n. bez2: ");



  // init
  xptSiz  = *nxp;
  *nxp    = 0;
  
  bcvtab1 = bezTab1.data;
  bcvtab2 = bezTab2.data;
  memPos1 = memSeg1->next;   // merken; ab hier nur tempData


  // mutual intersection of Bezier curves
  for (i1=0; i1<bezTab1.siz; ++i1) {

    for (i2=0; i2<bezTab2.siz; ++i2) {
      // printf("  _pt_int2bspl %d %d\n",i1,i2);

      ptxNr = 0;
      irc = UT3D_pt_int2bcv (&ptxNr, pTab1, UT_BEZDEG_MAX,
                             &bcvtab1[i1], &bcvtab2[i2], memSeg1, UT_TOL_cv);
      if(irc < 0) goto Fertig;

      // die neuen Schnittpunkte >  xptab zufuegen
      if(*nxp + ptxNr >= xptSiz) {
         TX_Error("UT3D_pt_int2bspl E002");
         irc = -2;
         goto Fertig;
      }
      for(i3=0; i3<ptxNr; ++i3) {
        xptab[*nxp] = pTab1[i3];
        ++(*nxp);
      }

      // reset memSeg1 (UT3D_pt_intlnbcv braucht nur temp. Objekte)
      // UME_dump(memSeg1, " space used: ");
      memSeg1->next = memPos1;

    }
  }



  Fertig:
  // reset memSeg1
  memSeg1->next = memPos0;

  return irc;
}


//============================================================================
  int UT3D_pt_nearptbspl (Point *ptx, double *px, CurvBSpl *cv1, Point *pt) {
//============================================================================
// give back nearest point of controlPoints.
 

  int     irc, i1, iMin;
  double  d1, dMin;

  // UT3D_stru_dump (Typ_CVBSP,  cv1, "UT3D_pt_nearptbspl");

  dMin = UT_VAL_MAX;

  for(i1=0; i1<cv1->ptNr; ++i1) {
    d1 = UT3D_lenB_2pt (&cv1->cpTab[i1], pt);
    if(d1 < dMin) {
      dMin = d1;
      iMin = i1;
    }
  }

  *ptx = cv1->cpTab[iMin];
  // den Parameter dieses Punktes -> px;
  i1 = iMin + (cv1->deg / 2);
  if(px) *px = cv1->kvTab[i1];

  // printf("ex UT3D_pt_nearptbspl %d %d\n",iMin,i1);
  return 0;

}


//================================================================
  int UT3D_pt_cbsp_lim (Point *p1, Point *p2, double *v1, double *v2,
                       CurvBSpl *cv1) {
//================================================================
/// \code
/// UT3D_pt_cbsp_lim         get limits (startpoint, endpoint, parameters)
/// 
/// Output:
///   p1        startpoint (none if p1 == NULL)
///   p2        endpoint (none if p2 == NULL)
///   v1        parameter (0-1) for startpoint (v1=NULL: none)
///   v2        parameter (0-1) for endpoint (v2=NULL: none)
/// 
/// 
/// see also UTO_2pt_lim_ox UTO_2pt_lim_dbo UTO_pt_ox
/// \endcode

// TODO: use it in UTO_2pt_limstru



  // printf("UT3D_pt_cbsp_lim \n");


  if(p1) UT3D_pt_evalparCv(p1, cv1, cv1->v0);
  if(p2) UT3D_pt_evalparCv(p2, cv1, cv1->v1);
  // if(v1) *v1 = cv1->v0;
  if(v1) *v1 = UT3D_par1_parbsp (&cv1->v0, cv1);
  // if(v2) *v2 = cv1->v1;
  if(v2) *v2 = UT3D_par1_parbsp (&cv1->v1, cv1);

    // if(v1) printf(" v1=%lf\n",*v1);
    // if(v2) printf(" v2=%lf\n",*v2);

  return 0;

}


/*=======================================================================*/
  int UT3D_pt_projptbspl (int *nxp, Point *ptab, double *ttab,
                           CurvBSpl *bspl, Point *pt) {
/*=======================
UT3D_pt_projptbspl    project point onto b-spline curve

UT3D_pt_projptbspl    Author: Thomas Backmeister       4.6.2003

Project a point onto a b-spline curve.
Keine Loesung (nxp=0): es wird der naehere Kurvenendpunkt -> ptab[0] kopiert.

IN:
  int *nxp         ... size of ptab and ttab
  CurvBSpl *bspl   ... b-spline curve
  Point *pt        ... point
  Memspc *memSeg1  ... Memory fuer temporeaere Objekte; memSeg1 wird am Ende
                       unverandert retourniert.
OUT:
  int *nxp         ... number of projection points
  Point *ptab      ... array of projection points; first point is nearest.
  double *ttab     ... array of corresponding parameter values of
                       projection points on b-spline curve
                       none if ttab==NULL
Returncode:
  0 = OK
 -1 = out of mem (Memspc too small)
 -2 = out of mem (ptab too small)
*/

  int      nbcv, i1, i3, i4, irc, ptxNr, xptSiz, bNr;
  ObjGX    bezTab;
  CurvBez  *bcvtab;
  Point    pTab1[UT_BEZDEG_MAX+1];
  double   d1, kvMin, kvMax, tTab1[UT_BEZDEG_MAX+1];
  void     *memPos0, *memPos1, *vp1;

  Memspc   tmpSpc=UME_NEW, *memSeg1;


  // printf("UT3D_pt_projptbspl %d\n",bspl->ptNr);
  // UT3D_stru_dump (Typ_PT, pt, "");
  // UT3D_stru_dump (Typ_CVBSP, bspl, "");
  // printf(" free tmpSpc=%d\n",UME_ck_free(memSeg1));


  // get tempspace (gets lost on func-return)
  // UME_alloc_tmp (&tmpSpc, 250000);
  bNr = bspl_tmpSpc_siz (bspl);
  vp1 = malloc(bNr);
  if(!vp1) return -1;
  UME_init (&tmpSpc, vp1, bNr);  // 500K tempSpace
  memSeg1 = &tmpSpc;


  // init
  xptSiz  = *nxp;
  *nxp    = 0;
  memPos0 = memSeg1->next;


  // test for minSiz
  i1 = UT3D_bsp_ck_minSiz (bspl);
  if(i1 > 0) {  // curve too small; use nearest point.
    UT3D_pt_nearptbspl (ptab, ttab, bspl, pt);
    *nxp = 1;
    return 0;
  }



  // Bezier curves from b-spline curve
  irc = UT3D_bez_bspl__ (&bezTab, memSeg1, bspl);
  if(irc < 0) goto L_exit;


  // projection of point onto Bezier curves
  bcvtab = bezTab.data;


  // Startpos memSeg1 merken; ab hier nur tempData
  memPos1 = memSeg1->next;


  for (i1=0; i1<bezTab.siz; ++i1) {

    // Testausg. Kurve
    // printf("######### projptbspl curveseg %d ###########\n",i1);
    // UT3D_stru_dump (Typ_CVBEZ, &bcvtab[i1], "");

    
    // project point
    irc = UT3D_pt_projptbez (&ptxNr, pTab1, tTab1, memSeg1, &bcvtab[i1], pt);
    if(irc < 0) goto Fertig;


    // convert Bezier curve parameter into b-spline curve parameter
    for (i3=0; i3<ptxNr; ++i3) {
        // printf(" va[%d]=%f\n",i1,bcvtab[i1].va);
        // printf(" vb[%d]=%f\n",i1,bcvtab[i1].vb);
        // printf(" 1: tTab1[%d]=%f\n",i3,tTab1[i3]);
      tTab1[i3] = (1.0-tTab1[i3]) * bcvtab[i1].va + tTab1[i3] * bcvtab[i1].vb;
        // printf(" 2: tTab1[%d]=%f\n",i3,tTab1[i3]);
    }


    // new projection points with corresponding parameter values
    // --> add to ptab / ttab
    if(*nxp + ptxNr >= xptSiz) {
       TX_Error("UT3D_pt_projptbspl: E002");
       irc = -2;
       goto Fertig;
    }

    // add found points;
    for(i3=0; i3<ptxNr; ++i3) {
      // test if point already exists
      for(i4=0; i4 < *nxp; ++i4) {
        if(UT3D_comp2pt(&ptab[i4],&pTab1[i3],UT_TOL_cv)) goto L_nxt_pt;
      }
      // copy point out
        // printf("  sp[%d] pt=%f %f %f uVal=%f\n",*nxp,
              // pTab1[i3].x,pTab1[i3].y,pTab1[i3].z,  tTab1[i3]);
      ptab[*nxp] = pTab1[i3];
      if(ttab) ttab[*nxp] = tTab1[i3];
      ++(*nxp);
      L_nxt_pt:
        continue;
    }


    // reset memSeg1
    memSeg1->next = memPos1;
  }


  Fertig:
  // reset memSeg1
    // printf(" free tmpSpc=%d\n",UME_ck_free(memSeg1));
  memSeg1->next = memPos0;


  if(*nxp == 0) {
    // keine Loesung: extrapolate
    // printf(" keine Loesung: extrapolate----\n");

    // pt1 = proj pp1 --> tangent. Verlaengerung of bsp2
    irc = UT3D_pt_projptbsp_ext (&ttab[0], &ptab[0], pt, bspl);
    // if(i1 == 1) bsp2.v0 = d1;
    // else        bsp2.v1 = d1;



/* alte Version:
    // den naeheren Punkt (t=0. oder t=1.) ausgeben
    kvMin = bspl->kvTab[0];
    kvMax = bspl->kvTab[bspl->ptNr+bspl->deg];
    UT3D_pt_evalparCv (&ptab[0], bspl, kvMin);
    ttab[0] = kvMin;
    UT3D_pt_evalparCv (&ptab[1], bspl, kvMax);
    i1 = UT3D_ipt_cknear_npt (pt, ptab, 2);
    if(i1 > 0) {
      // printf(" swap .. %f %f\n",kvMin,kvMax);
      MEM_swap__(&ptab[0], &ptab[1], sizeof(Point));
      ttab[0] = kvMax;
    }
      // UT3D_stru_dump (Typ_PT, &ptab[0]," no sol:%d %f",i1,ttab[0]);
    // printf(" no sol.; tval=%f\n",ttab[0]);
*/



  } else if(*nxp > 1) {
    // mind 2 Loesungen: den naehesten Punkt als ersten Punkt anbieten
    i1 = UT3D_ipt_cknear_npt (pt, ptab, *nxp);
    if(i1 != 0) {
        // printf(" swap %d - 0\n",i1);
      MEM_swap__(&ptab[0], &ptab[i1], sizeof(Point));
      if(ttab) MEM_swap__(&ttab[0], &ttab[i1], sizeof(double));
    }
  }


/*
    // TESTBLOCK:
    printf("ex UT3D_pt_projptbspl %d\n",*nxp);
    for(i1=0; i1<*nxp; ++i1) {
      UT3D_stru_dump (Typ_PT, &ptab[i1],"  %d %f",i1,ttab[i1]);
      // GR_Disp_pt (&ptab[i1], SYM_STAR_S, 2);
    }
    printf("\n");
    // END TESTBLOCK:
*/

  //----------------------------------------------------------------
  L_exit:
  // free tmpSpc
  free(vp1);

  return irc;
}


/*=======================================================================*/
  int UT3D_pt_evparCrvBSpl (Point *pt, CurvBSpl *bspl, double u) {
/*========================
UT3D_pt_evparCrvBSpl    point <-- evaluate curve parameter

UT3D_pt_evparCrvBSpl    Author: Thomas Backmeister       12.9.2003

Evaluate a point on a b-spline curve.
The underlying algorithm is "de Boor".
The point is computed for any parameter value within the maximal
support of the curve (we do not consider the limit parameters
bspl->v0 and bspl->v1).

This function evaluates without using polynomials.
(in contrast to UT3D_pt_evalparCv)
It needs work space of size (curve degree * sizeof(Point)).

IN:
  CurvBSpl *bspl  ... b-spline curve
  double u        ... parameter value
  Memspc *workSeg ... work space (control points)
OUT:
  Point *pt       ... curve point
Returncodes:
  0 = OK
 -1 = out of work space
 -2 = input error
*/

  int rc, d, n, m, r, s;
  int i1, i2, ih, jh, n1;
  double a, b;
  Point *ptab;


// UT3D_pt_evparCrvRBSpl ist nicht rekursiv; daher lokal moeglich !
  // char      tmpspc[100000];
  // Memspc    tmpSeg, *workSeg;
  // void *memstart;



  // // Init Datensegmente
  // UME_init (&tmpSeg, tmpspc, sizeof(tmpspc));
  // workSeg = &tmpSeg;


  // printf("\nUT3D_pt_evparCrvBSpl: u=%f\n",u);
  // UT3D_stru_dump (Typ_CVBSP, bspl, "");


  // some abbreviations
  d = bspl->deg;
  n = bspl->ptNr - 1;
  m = n + d + 1;

  // check parameter
  if (u < bspl->kvTab[d]) {
    // printf("UT3D_pt_evparCrvBSpl: parameter below %f\n",u);
    u = bspl->kvTab[d];
  } 
  if (u > bspl->kvTab[bspl->ptNr]) {
    // printf("UT3D_pt_evparCrvBSpl: parameter above %f\n",u);
    u = bspl->kvTab[bspl->ptNr];
  }

  // determine segment of parameter
  r = -1;
  for (s=d; s<m-d; ++s) {
    if (bspl->kvTab[s] <= u && u <= bspl->kvTab[s+1]) {
      r = s;
      break;
    }
  }
  if (r == -1) {
    printf("UT3D_pt_evparCrvBSpl: input error\n");
    // TX_Error("UT3D_pt_evparCrvBSpl: input error");
    // exit (0);
    return -2;
  }

  // provide work space
  // memstart = workSeg->next;
  // ptab = workSeg->next;
  // rc = UME_add (workSeg, sizeof(Point) * (d+1));
  // if (rc < 0) goto L_outOfWorkSpace;
  ptab = (Point*)MEM_alloc_tmp ((int)(sizeof(Point) * (d+1)));
  if(!ptab) {TX_Error("UT3D_pt_evparCrvBSpl: EOM"); return -1;}

 
  // init control points
  ih = r - d;
  for (i1=0; i1<d+1; ++i1) ptab[i1] = bspl->cpTab[ih+i1];

  // compute curve point
  n1 = d;
  for (i1=0; i1<d; ++i1) {
    ih = r + 1;
    jh = ih - n1;
    for (i2=0; i2<n1; ++i2) {
      a = (u - bspl->kvTab[jh+i2]) / (bspl->kvTab[ih+i2] - bspl->kvTab[jh+i2]);
      b = 1.0 - a;
      ptab[i2].x = b * ptab[i2].x + a * ptab[i2+1].x;
      ptab[i2].y = b * ptab[i2].y + a * ptab[i2+1].y;
      ptab[i2].z = b * ptab[i2].z + a * ptab[i2+1].z;
    }
    --n1;
  }
  *pt = ptab[0];

  // release work space
  // workSeg->next = memstart;


  // printf("ex UT3D_pt_evparCrvBSpl %f,%f,%f\n",pt->x,pt->y,pt->z);
  return 0;


  //----------------------------------------------------------------
  // L_outOfWorkSpace:
    // TX_Error("UT3D_pt_evparCrvBSpl: out of work space");
    // return -1;
}


//================================================================
  double UT3D_par1_parbsp (double *kv, CurvBSpl* cv1) {
//================================================================
/// UT3D_par1_parbsp          get parameter 0-1 from knotValue
// see UT3D_parbsp_par1


  double pv, u1, u2, uTot, uMin, uMax;


  printf("UT3D_par1_parbsp kv=%lf\n",*kv);


  u1 = cv1->v0;
  u2 = cv1->v1;

  if(cv1->dir) MEM_swap_2db (&u1, &u2);


  uMin = cv1->kvTab[0];
  uMax = cv1->kvTab[cv1->ptNr + cv1->deg];
    printf("    _par1_parbsp u1=%f u2=%f uMin=%f uMax=%f\n",u1,u2,uMin,uMax);

  if(u2 < u1) {
    // umax - u1 + u2 - umin
    uTot = uMax - u1 + u2 - uMin;
    if(*kv > u1) pv = (*kv - u1) / uTot;
    else         pv = (uMax - u1 + *kv - uMin) / uTot;

  } else {
    uTot = u2 - u1;
    pv = (*kv - u1) / uTot;
  }

  if(cv1->dir) pv = 1. - pv;   // removed 2017-02-15

    printf("ex UT3D_par1_parbsp pv=%f kv=%f uTot=%f\n",pv,*kv,uTot);

  return (pv);

}


/* REPLACED with UT3D_par1_parbsp
//================================================================
  double UT3D_par1_parBSp (double *kv, CurvBSpl* cv1) {
//================================================================
// UT3D_par1_parBSp           Knotenwert in einen Parameterwert von 0-1 aendern

  // double p1, d1, d2;

  // printf("UT3D_par1_parBSp %f %f %f\n",*kv,cv1->v0,cv1->v1);

    // d1 = cv1->v0;
    // d2 = cv1->v1 - cv1->v0;
    // p1 = (*kv - d1) / d2;

  // printf("ex UT3D_par1_parBSp %f\n",p1);

  return (*kv - cv1->v0) / (cv1->v1 - cv1->v0);

}
*/

//================================================================
  double UT3D_parbsp_par1 (double pv, CurvBSpl* cv1) {
//================================================================
/// UT3D_parbsp_par1           get knotValue from parameter 0-1
// see UT3D_par1_parbsp

  
  double kv, u1, u2, uTot, uMin, uMax;

  u1 = cv1->v0;
  u2 = cv1->v1;

  if(cv1->dir) {
    MEM_swap_2db (&u1, &u2);
    pv = 1. - pv;
  }   

  uMin = cv1->kvTab[0];
  uMax = cv1->kvTab[cv1->ptNr + cv1->deg];
    // printf("UT3D_parbsp_par1 u1=%f u2=%f uMin=%f uMax=%f\n",u1,u2,uMin,uMax);

  if(u2 < u1) {
    // umax - u1 + u2 - umin
    uTot = uMax - u1 + u2 - uMin;
  } else {
    uTot = u2 - u1;
  }

  kv = u1 + (uTot * pv);
  // only if curve is closed:
  if(!UT3D_bsp_ck_closed__(cv1))
    if(kv > uMax) kv = kv - uMax + uMin;

  // printf("UT3D_parbsp_par1 kv=%f pv=%f uTot=%f\n",kv,pv,uTot);

  return (kv);

}


//===============================================================
  int UT3D_pt_evalparCv (Point *pto, CurvBSpl *cv1,  double uVal) {
//===============================================================
// UT3D_pt_evalparCv      Punkt an B-Spline aus Parameterwert


  // UT3D_stru_dump (Typ_CVBSP, cv1, "UT3D_pt_evalparCv %f\n",uVal);

  bspl_eval_Pt (cv1->ptNr-1, cv1->deg, cv1->cpTab, cv1->kvTab, uVal, pto);

  // UT3D_stru_dump (Typ_PT, pto, "ex UT3D_pt_evalparCv:");

  return 0;

}


//===============================================================
  int UT3D_vc_evalparCv (Vector *vco, CurvBSpl *cv1,  double uVal) {
//===============================================================
// UT3D_vc_evalparCv      Tangente an B-Spline aus Parameterwert


  // printf("UT3D_vc_evalparCv par=%f ptNr=%d deg=%d\n",
         // uVal,cv1->ptNr-1,cv1->deg);


  if(cv1->deg == 1) {
    UT3D_vc_2pt (vco, &cv1->cpTab[0], &cv1->cpTab[1]);

  } else {
    bspl_eval_Tg (cv1->ptNr-1, cv1->deg, cv1->cpTab, cv1->kvTab, uVal, vco);
  }

  UT3D_vc_setLength (vco, vco, 1.);


    // printf("ex-vc_evalparCv %lf %lf %lf\n",vco->dx,vco->dy,vco->dz);

  return 0;

}




/*=======================================================================*/
  int UT3D_parCv_bsplpt (double *up, double *dist, CurvBSpl *bspl, Point *pt) {
/*===================
UT3D_parCv_bsplpt       curve parameter <-- point on b-spline curve

UT3D_parCv_bsplpt       Author: Thomas Backmeister       12.6.2003

Computation of parameter value of point on b-spline curve.
If point is not on curve, the parameter value of the closest
projection point is computed.

IN:
  CurvBSpl *bspl   ... b-spline curve
  Point *pt        ... point on b-spline curve
OUT:
  double *up       ... parameter value
  double *dist         distance curve-pt
Returncode:
  0 = OK
  1 = no solution (point has no projection on the curve)
 <0 = internal error
*/

  int      i1, np, ind, irc;
  double   tTab[32], dist1;
  Point    pTab[32];


  // printf("UT3D_parCv_bsplpt\n");
  // UT3D_stru_dump (Typ_CVBSP, bspl, "  bspl:");
  // UT3D_stru_dump (Typ_PT, pt, "  pt:");


  // project point onto b-spline curve
  np = 32;  // size of pTab / tTab !
  irc = UT3D_pt_projptbspl (&np, pTab, tTab, bspl, pt);
  if (irc < 0) return irc;


  // find projection point with minimal distance from input point
  *dist = UT_VAL_MAX;
  ind = 0;
  irc = 1;
  for (i1=0; i1<np; ++i1) {
    dist1 = UT3D_len_2pt (&pTab[i1], pt);
      // printf("   test dist=%f pt=%f %f %f\n",dist,
            // pTab[i1].x,pTab[i1].y,pTab[i1].z);
    if (dist1 < *dist) {
      *dist = dist1;
      ind = i1;
      irc = 0;
    }
  }

  // curve parameter value of closest projection point
  *up = 0.0;
  if (irc == 0) *up = tTab[ind];

  L_fertig:
  // printf("ex UT3D_parCv_bsplpt: irc=%d u=%f pt=%f %f %f\n",irc,*up,
                                // pt->x,pt->y,pt->z);

  return irc;
}


//================================================================
  int UT3D_du_bsplTol (double *du, CurvBSpl *cvbs, double tol) {
//================================================================
// UT3D_du_bsplTol            get du for polygonalization of bSpline from tol

  int  pNr;

  UT3D_ptNr_bsp (&pNr, cvbs, tol);

  *du =  (cvbs->v1 - cvbs->v0) / pNr;

  return 0;

}


//================================================================
  int UT3D_ptNr_bsp (int *iu, CurvBSpl *bsp, double tol1) {
//================================================================
/// \code
/// UT3D_ptNr_rbsp     estimate nr of polygonPoints for B-spline
/// see also UT3D_ptNr_rbsp SUSbsp_ck_tol
/// \endcode

  int    i2, i3, ip;
  double ao, a1, d1, d2;
  Vector vc1, vc2;


  // UT3D_stru_dump (Typ_CVBSP, bsp, "UT3D_ptNr_bsp:\n");


  if(bsp->ptNr < 3) {*iu = 2; return 0;}

  if(bsp->deg == 1) { *iu = bsp->ptNr; return 0;}


  //===============================================================
  // Toleranz voff: loop durch Kurve, max. Abweichung pro Laenge suchen

  *iu = 0;
  tol1 /= 8.;

  // degree
  d1 = bsp->deg - 1.;     // printf(" _ptNr_bsp-d1=%f\n",d1);
  tol1 /= d1;



  // printf("------------ %d\n",i3);
  // aTot = 0.;
  // dTot = 0.;
  i3 = 2;
  d2 = UT3D_len_2pt (&bsp->cpTab[0], &bsp->cpTab[1]);

  // startvector
  UT3D_vc_2pt (&vc2, &bsp->cpTab[0], &bsp->cpTab[1]);

  for(i3=2; i3<bsp->ptNr; ++i3) {
      // UT3D_stru_dump(Typ_PT, &bsp->cpTab[i3], "P[%d][%d]=",i1,i2);
      // GR_Disp_pt (&bsp->cpTab[i3], SYM_STAR_S, 2);
    d1 = d2;
    vc1 = vc2;
    UT3D_vc_2pt (&vc2, &bsp->cpTab[i3-1], &bsp->cpTab[i3]);

    // opening-angle
    ao = UT3D_angr_2vc__ (&vc1, &vc2);
    // ao *= (1. / bsp->wTab[i3]);
    // aTot += ao;

    // dist
    d2 = UT3D_len_2pt (&bsp->cpTab[i3-1], &bsp->cpTab[i3]);

    // get angle for rad = d1 + d2
    a1 = UT2D_angr_ciSec (tol1, (d1 + d2));

    ip = ao / a1;
    *iu += ip + 2;

      // printf(" %d ao=%f d2=%f ip=%d\n",i3,UT_DEGREES(ao),d2,ip);
  }


  if(*iu < 2) *iu = 2;

  // printf("ex UT3D_ptNr_bsp %d\n",*iu);


  return 0;

}

/* old Version; replaced by UT3D_ptNr_bsp
//===================================================================
  int UT3D_ptNr_bsplTol (int *iu, CurvBSpl *bsp, double tol1) {
//===================================================================
// UT3D_ptNr_bsplTol      estimate nr of polygonPoints for B-spline
// Anzahl von Polygonpunkten fuer bestimmte Toleranz ermitteln
// Wird fuer SRU benutzt, muss also mit hoeherer Kurvelaenge steigen !
// see also UT3D_ptNr_rbsp SUSbsp_ck_tol


  int    i2, i3;
  double aMax, lTot, d1;
  Vector vc1, vc2;


  // printf("UT3D_ptNr_bsplTol v0=%f v1=%f tol=%f\n",bsp->v0,bsp->v1,tol1);
  // UT3D_stru_dump (Typ_CVBSP, bsp, "  ");

  if((bsp->deg == 1) ||(bsp->ptNr < 3)) {
    *iu = bsp->ptNr;
    return 0;
  }


  //===============================================================
  // Toleranz voff: loop durch Kurve, max. Abweichung pro Laenge suchen
  // die laengste Kurve und die max. Abweichung errechnen
  // laengste Kurve: die Teilstrecken zu den Kontrollpunkten addieren -> lTot
  aMax = 0.;
  lTot = 0.;

  tol1 /= 8.;  // else too much bufferOverflows .. 2014-06-04


  // printf("------------ %d\n",i3);
  i3 = 2;
  lTot += UT3D_len_2pt (&bsp->cpTab[0], &bsp->cpTab[1]);
  UT3D_vc_2pt (&vc2, &bsp->cpTab[0], &bsp->cpTab[1]);

  for(i3=2; i3<bsp->ptNr; ++i3) {
      // UT3D_stru_dump(Typ_PT, &bsp->cpTab[i3], "P[%d][%d]=",i1,i2);
      // GR_Disp_pt (&bsp->cpTab[i3], SYM_STAR_S, 2);
    vc1 = vc2;
    UT3D_vc_2pt (&vc2, &bsp->cpTab[i3-1], &bsp->cpTab[i3]);
    d1 = UT3D_angr_2vc__ (&vc1, &vc2);
    lTot += UT3D_len_2pt (&bsp->cpTab[i3-1], &bsp->cpTab[i3]);
    // printf("  d1[%d]=%f l=%f\n",i3,UT_DEGREES(d1),lTot);
    // d1 /= ld;  // normieren ; soll sein Anzahl Punkte / mm.
    if(d1 > aMax) aMax = d1;
  }

  // printf("  ptNr=%d aMax=%f lTot=%f\n",bsp->ptNr,UT_DEGREES(aMax),lTot);
  // d1 = bsp->ptUNr * (UT_DEGREES(aMax) / 20.);    // ptNr * (pro 20 Grad)
  // d1 = bsp->ptNr * (aMax / pow(tol1, 0.4));    // ptNr * (0.4=pro 20 Grad)
  // 0.2=19, 0.3=27, 0.4=36, .6=39, .7=90, 1=221
  // d1 = lTot * (aMax / pow(tol1, 0.2));    // ptNr * (0.4=pro 20 Grad)

  // d1 = lTot / tol1 * acos(aMax) / 25.;    // ptNr * (0.4=pro 20 Grad)
  // *iu = d1;

  // radius = lTot / openingAngle
  d1 = lTot / aMax;

  // winkelToleranz = UT2D_angr_ciSec (tol, r)
  d1 = UT2D_angr_ciSec (tol1, d1);

  // ptNr = openingAngle / winkelToleranz
  *iu = aMax / d1;
  // aMax die max. Winkelabweichung zwischen 2 Kontrollpunkten !!

  if(*iu < 2) *iu = 2;

    // printf("ex UT3D_ptNr_bsplTol segNr=%d tol=%f\n",*iu,tol1);

  return 0;

}
*/

//============================================================================
  int bspl_bsp_ptn (CurvBSpl *cvo,Memspc *memSeg1,Point *pTab,int pNr,int deg) {
//============================================================================
// make B-spline cvo from pointTable pTab
// verwendet memSeg1 f. kvTab.
// RC  0   = OK
//    -1   = memSeg1 zu klein



  int      i1;
  // int      kvTabSiz, kvTabNr;
  double   *kvTab;


  // printf("bspl_bsp_ptn pNr=%d deg=%d\n",pNr,deg);



  // kvTab = (void*) memspc51;
  kvTab = memSeg1->next;
  // kvTabSiz = sizeof(memspc51) / sizeof(double);
  // kvTabNr  = 0;


  // size von kvTab = ptNr + deg + 1;
  // platz in memSeg1 reservieren; wird in bspl_knotvec__ belegt.
  if(UME_add (memSeg1, sizeof(double) * (pNr+deg+1)) < 0) {
    TX_Error("bspl_bsp_ptn E001");
    return -1;
  }



  // get knotvector
  i1 = bspl_knotvec__ (kvTab, pNr, deg);
  if(i1 < 0) {
    TX_Error("Degree zu hoch");
    return -1;
  }


  // fill
  cvo->ptNr  = pNr;
  cvo->deg   = deg;
  cvo->cpTab = pTab;       // die Punkte
  cvo->kvTab = kvTab;

  cvo->v0    = 0.;
  cvo->v1    = kvTab[pNr + deg];


  return 0;

}

//================================================================
  int UT3D_knotvec_m (double **knTab, double *v0, double *v1,
                      int ptNr, int deg, Memspc *workSeg) {
//================================================================
// UT3D_knotvec_m             create & copy knotvektor
// Daten werden im workSeg erzeugt !

  int   i1, irc;


  // printf("UT3D_knotvec_m %d %d\n",ptNr,deg);


  i1 = ptNr + deg + 1;

  // reserve ptNr doubles in wrkSpc
  *knTab = workSeg->next;
  irc = UME_add (workSeg, sizeof(double) * i1);
  if(irc < 0) return -4;

  // created knotvalues
  bspl_knotvec__ (*knTab, ptNr, deg);

  *v0 = (*knTab)[0];
  *v1 = (*knTab)[i1-1];

  return 0;

}


//========================================================================
  int bspl_knotvec__ (double *knTab, int ptNr, int deg) {
//========================================================================
/*
Erstellen des knotvektor:
Input: cpNr (Anzahl Controlpoints) und degree.
Zuerst deg * 0 ausgeben (bei deg=3. "0,0,0").
dann eine Reihe von 0-n; Anzahl = (cpNr-deg+1)
(Bei cpNr=6 und deg=3 ist Anz.=4; Reihe ist "0,1,2,3").
Dann deg * Endwert (bei 6/3 also "3,3,3")
*/

  int  i1, i2, ie, nr;


  // printf("bspl_knotvec__ %d %d\n",ptNr,deg);

  if(deg >= ptNr) return -1;

  nr = ptNr - deg + 1;

  i2 = 0;
  for(i1=0; i1<deg; ++i1) {
    knTab[i2] = 0.;
    ++i2;
  }

  for(i1=0; i1<nr; ++i1) {
    ie = i1;
    knTab[i2] = ie;
    ++i2;
  }

  for(i1=0; i1<deg; ++i1) {
    knTab[i2] = ie;
    ++i2;
  }


/*
  // nur Testausg:
  for(i1=0; i1<i2; ++i1) {
    printf(" knv[%d] = %f\n",i1,knTab[i1]);
  }
*/

  return i2;

}


//========================================================================
 int bspl_pol2_bsp2 (Point *pTab1, Point *pTab2, int *ptNr, int ptMax,
                    CurvBSpl *cv1, CurvBSpl *cv2, double tol) {
//========================================================================
// Ruled surf aus 2 Kurven. Dzt wird via Parmeter, nicht via Laenge zerteilt.




  int     ptOut, ptStackNr;
  double  ucs1, uce1, ucs2, uce2;
  double  us1, ue1, us2, ue2;
  double  dist1, dist2, pVal1, pVal2;
  double  uStack1[20], uStack2[20];
  Point   pt1, pt2, pts1, pts2, pte1, pte2, ptStack1[20], ptStack2[20];


  ucs1 = cv1->v0;
  uce1 = cv1->v1;

  ucs2 = cv2->v0;
  uce2 = cv2->v1;



  // den ersten Pt sofort raus
  ptOut = 0;
  UT3D_pt_evalparCv (&pTab1[ptOut], cv1, ucs1);
  UT3D_pt_evalparCv (&pTab2[ptOut], cv2, ucs2);
  ++ptOut;          // der aktuelle Punkt ist immer pTab[ptOut-1].




  // den letzen Punkt auf den Stack.
  ptStackNr = 0;
  uStack1[ptStackNr] = uce1;
  uStack2[ptStackNr] = uce2;
  // uStack[ptStackNr] = kve;
  UT3D_pt_evalparCv (&ptStack1[ptStackNr], cv1, uce1);
  UT3D_pt_evalparCv (&ptStack2[ptStackNr], cv2, uce2);
  // irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, kve, &ptStack[ptStackNr]);
  ++ptStackNr;

  pts1 = pTab1[0];
  pts2 = pTab2[0];
  us1  = ucs1;
  us2  = ucs2;




  //-------------------------------------------
  // der Startpunkt ist pts; den naechsten Endpunkt vom ptStack holen;
  // dessen u-Wert ist uStack.

  L_next_inPoint:
  // den naechsten Endpunkt vom Stack -> pte holen
  pte1 = ptStack1[ptStackNr-1];
  ue1  = uStack1[ptStackNr-1];
  pte2 = ptStack2[ptStackNr-1];
  ue2  = uStack2[ptStackNr-1];

  // u-Mittelwert errechnen
  pVal1 = (us1 + ue1) / 2.0;
  pVal2 = (us2 + ue2) / 2.0;

  // printf("next pVal=%f us=%f ue=%f ptStackNr=%d\n",pVal,us,ue,ptStackNr);

  // den Punkt dazu holen (Zwischenpunkt)
  UT3D_pt_evalparCv (&pt1, cv1, pVal1);
  UT3D_pt_evalparCv (&pt2, cv2, pVal2);
  // irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, pVal, &pt1);
  // printf(" irc=%d pt1=%f %f %f\n",irc,pt1.x,pt1.y,pt1.z);
  // if(irc < 0) return irc;


  // die Abweichung feststellen
  dist1 = UT3D_nlen_3pt (&pts1, &pt1, &pte1);
  dist2 = UT3D_nlen_3pt (&pts2, &pt2, &pte2);
  // if(ptStackNr > 1) {dist = 0.001;} else {dist = 10.0;} // TEST


  // mind. 3 Punkte; sonst geht eine S-Form nicht !!
  if((dist1 > tol)||(dist2 > tol)||((ptOut < 2)&&(ptStackNr<2))) {
    // Abweichung zu gross; Punkt speichern
    // printf(" >>>> save %d\n",ptStackNr);
    ptStack1[ptStackNr] = pt1;
    ptStack2[ptStackNr] = pt2;
    uStack1[ptStackNr]  = pVal1;
    uStack2[ptStackNr]  = pVal2;
    ++ptStackNr;
    if(ptStackNr >= 20) { TX_Error("bspl_pol2_bsp2 E001"); return -1; }
    goto L_next_inPoint;
  }


  // Abweichung tolerierbar. pte raus.
  pTab1[ptOut] = pte1; // copy Point
  pTab2[ptOut] = pte2; // copy Point
  if(ptOut >= ptMax) {
    TX_Error("bspl_pol_bsp E001");
    return -1;
  }
  ++ptOut;
  --ptStackNr;
  pts1 = pte1;
  pts2 = pte2;
  us1 = ue1;
  us2 = ue2;
  if(ptStackNr > 0) goto L_next_inPoint;


  //-------------------------------------------
  L_fertig:

  *ptNr = ptOut;

  return 0;

}





//========================================================================
  int bspl_cv_bsp (int *ptNr, Point *pTab, CurvBSpl *cv1) {
//========================================================================
// bspl_cv_bsp               make Polygon from B-Spline mit fixer ptNr
// Input:
//   ptNr    fixed nr of points; must be > 2

  int     ip;
  double  dp, ap;


  // printf("bspl_cv_bsp %d\n",*ptNr);

  if(*ptNr < 2) return -1;

  dp = (cv1->v1 - cv1->v0 ) / (*ptNr - 1);

  ap = cv1->v0;
  ip = 0;


  L_next:
  UT3D_pt_evalparCv (&pTab[ip], cv1, ap);

  ap += dp;
  ++ip;
  if(ip < *ptNr) goto L_next;


  return 0;

}


//========================================================================
  int bspl_pol_bsp (int *ptNr, Point *pTab,
                    CurvBSpl *cv1, int ptMax, double tol) {
//========================================================================
/// \code
/// bspl_pol_bsp               make Polygon from B-Spline-Kurve
/// Method: make point in half; test deviation; redo until deviation < tol.
/// RetCode: 0 = OK
///         -1 = overflow pTab  (E001)
/// does not respect dir; use UT3D_pta_bsp
/// see also bspl_cv_bsp (fixed pointNr)
/// \endcode

// tol (typ. 0.003) kleinste Strecke zur Berechnung der Abweichung
// Die Anzahl von Polygonsegment entspricht der Toleranz (NUR OUT !!)

// TODO: extract v0<v1 (reverse points afterwards;
//       extract adding points outside v0-v1


#define AUXTABSIZ 200

  int    i1, i2, ianz, ideg, irc, ptOut, ptStackNr, iout, idir, iClo;
  double d1, *vTab, pVal, kvs, kve, u1, u2, us, ue, dist,
         uStack[AUXTABSIZ], uTab[AUXTABSIZ];
  Point  *ptCrv, pt1, pts, pte,
         ptStack[AUXTABSIZ];

  double   *lTab, d2;
  CurvPoly plg1;


  // printf("=================================================== \n");
  // printf("bspl_pol_bsp ptNr=%d deg=%d tol=%f\n",cv1->ptNr,cv1->deg,tol);
  // printf("          v0=%f v1=%f ptMax=%d\n",cv1->v0,cv1->v1,ptMax);
  // UT3D_stru_dump (Typ_CVBSP, cv1, "");
  // ptMax=50; // TEST ONLY
  // for(i1=0;i1<cv1->ptNr;++i1) {
    // pTab[i1]=cv1->cpTab[i1]; GR_Disp_txi (&pTab[i1], i1, 2); }
   // *ptNr = cv1->ptNr; return 0;
  //----------------------------------------------------------------
 


  if(ptMax < 2) goto L_err_ptab;

  ptOut = 0;

  ianz  = cv1->ptNr - 1;
  ideg  = cv1->deg;
  ptCrv = cv1->cpTab;
  vTab  = cv1->kvTab;

  kvs   = cv1->v0;
  kve   = cv1->v1;
    // printf(" kvs=%f kve=%f\n",kvs,kve);

  if(kvs < kve) idir = 1;
  else          idir = -1;
    // printf("  kvs=%f kve=%f idir=%d\n",kvs,kve,idir);

  u1 = vTab[0];
  u2 = vTab[cv1->ptNr + ideg];
    // printf(" u1=%f u2=%f\n",u1,u2);

  if(ideg > 1) goto L_cbsp;



  //================================================================
  // handle Polygon (deg=1);
  if(UTP_comp2db(kvs,u1,UT_TOL_0Cos) == 0) goto L_plg;
  if(UTP_comp2db(kve,u2,UT_TOL_0Cos) == 0) goto L_plg;


  //----------------------------------------------------------------
  // untrimmed polygon. Copy all points.
  ptOut = cv1->ptNr;
  if(ptOut > ptMax) goto L_err_ptab;
  memcpy (pTab, cv1->cpTab, sizeof(Point) * ptOut);  // copy points
  goto L_exit;



  //----------------------------------------------------------------
  // ein getrimmtes Polygon. Umwandeln in ein Plg, trimmen, raus.
  L_plg:
    // printf(" L_plg:\n");

  // max ianz points;
  ptOut  = cv1->ptNr;
  lTab = (void*)&pTab[ptOut];
  // check space for lTab
  i1 = (ptMax - ptOut) * 3;
  if(i1 < ianz) goto L_err_ptab;   // lTab needs space for ptOut doubles
  d1 = UT3D_plg_lvTab (lTab, cv1->cpTab, ptOut);  // fill lTab; get total length
  plg1.ptNr  = ptOut;
  plg1.cpTab = cv1->cpTab;
  plg1.lvTab = lTab;
  // plg1.v0 = (kvs - u1) / (u2 - u1) * d1;
  // plg1.v1 = (kve - u1) / (u2 - u1) * d1;
  plg1.v0 =  UT3D_par1_parbsp (&kvs, cv1) * d1;
  plg1.v1 =  UT3D_par1_parbsp (&kve, cv1) * d1;
    // printf(" d1=%f v0=%f v1=%f\n",d1,plg1.v0,plg1.v1);
  irc = UT3D_pta_plg (&ptOut, pTab, &plg1);
  if(irc < 0) goto L_err_ptab;
  goto L_exit;



  //================================================================
  L_cbsp:
    // printf(" L_cbsp:\n");

  // bspl_pol_bs wird immer von v0 nach v1 abgearbeitet.
  // u1 ist aber immer kleiner als u2.


  // wenn kvs=v0 outside Curve: add Point.
  iout = UTP_db_ck_in2db (kvs, u1, u2);
    // printf(" iout=%d\n",iout);
  if(iout == 0) goto L_1;

  if(iout != 0) {
    if(idir > 0) kvs = u1;
    else         kvs = u2;
  }

  d1 = cv1->v0 - kvs;
  // if(idir < 0) d1 = kvs - cv1->v0;
  // else         d1 = cv1->v0 - kvs;
    // printf(" d1=%f\n",d1);

  // get Startpt CV
  bspl_eval_expPt (&pTab[ptOut], cv1, kvs, d1);
    // UT3D_stru_dump (Typ_PT, &pTab[ptOut], " kvs=%f PrimPt",kvs);
  ++ptOut;          // der aktuelle Punkt ist immer pTab[ptOut-1].



  //----------------------------------------------------------------
  L_1:

  // testen ob kve innerhalb Bereich liegt
  iout = UTP_db_ck_in2db (kve, u1, u2);
    // printf(" iout=%d kve=%lf u1=%lf u2=%lf\n",iout,kve,u1,u2);
  if(iout != 0) {
    if(idir > 0) kve = u2;
    else         kve = u1;
  }
    // printf(" startCurve; kvs=%f kve=%f\n",kvs,kve);


  // den ersten Pt sofort raus
  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, kvs, &pTab[ptOut]);
    // UT3D_stru_dump (Typ_PT, &pTab[ptOut], " kvs=%f StartPt",kvs);
  ++ptOut;          // der aktuelle Punkt ist immer pTab[ptOut-1].



  // nur 2 Punkte -
  if(ianz == 1) {
    pTab[ptOut] = ptCrv[1]; // copy Point
    ++ptOut;
    goto L_fertig;
  }


  // den letzen Punkt zuerst auf den Stack.
  ptStackNr = 0;
  uStack[ptStackNr] = kve;
  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, kve, &ptStack[ptStackNr]);
    // UT3D_stru_dump (Typ_PT, &ptStack[ptStackNr], " kve=%f EndPt",kve);
  ++ptStackNr;


  // alle Wendepunkte suchen und VERKEHRT auf den stack ..
  i1 = AUXTABSIZ;
  irc = UT3D_bsp_infTg (&i1, uTab, cv1);
    // printf(" _bsp_infTg irc=%d i1=%d\n",irc,i1);
  if(irc < 0) {
    // bspline from point ?
    // add endPt ptStack[0]
    pTab[ptOut] = ptStack[0];
    ++ptOut;
    goto L_exit;
  }
    // for(i2=0;i2<i1;++i2) printf(" uTab[%d]=%f\n",i2,uTab[i2]);


  // Wendepunkte VERKEHRT auf den stack ..
  if(i1 > 0) {
    for(i2=i1-1; i2>=0; --i2) {
      if(uTab[i2] >= kve) continue;       // 2012-03-28
      if(uTab[i2] <= kvs) continue;
      uStack[ptStackNr] = uTab[i2];
      irc = bspl_eval_Pt (ianz,ideg,ptCrv,vTab,uTab[i2],&ptStack[ptStackNr]);
      ++ptStackNr;
      if(ptStackNr >= AUXTABSIZ) goto L_err_stack;
    }
  }



  //---------- TEST display stack
  // for(i1=0;i1<ptStackNr;++i1) {
    // printf(" uStack[%d]=%f\n",i1,uStack[i1]);
    // GR_Disp_pt (&ptStack[i1], SYM_STAR_S, 2);
  // }
  //---------- TEST display stack



  pts = pTab[0];
  us = kvs;


  // ck if closed
  iClo = UT3D_bsp_ck_closed_tr (cv1); // 1=closed, 0=not.
    // printf(" iClo=%d\n",iClo);


  //=====================================================================
  // der Startpunkt ist pts; den naechsten Endpunkt als pte vom ptStack holen;
  // dessen u-Wert ue kommt vom uStack.

  L_next_inPoint:
  // den naechsten Endpunkt vom Stack -> pte holen
  pte = ptStack[ptStackNr-1];
  ue  = uStack[ptStackNr-1];
    // printf(" L_next_inPoint: ue=%lf\n",ue);


  // u-Mittelwert errechnen
  pVal = (us + ue) / 2.0;
    // printf("next pVal=%f us=%f ue=%f ptStackNr=%d\n",pVal,us,ue,ptStackNr);

  // den Punkt dazu holen (Zwischenpunkt)
  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, pVal, &pt1);
    // printf(" irc=%d pt1=%f %f %f\n",irc,pt1.x,pt1.y,pt1.z);
  if(irc < 0) return irc;


  if(ptStackNr == 1) {
    // wenn pts == pte, dann ist es eine closed curve; noch einen Zwischenpunkt!
    if(iClo == 0) {
      // yes,closed
      if(UT3D_comp2pt(&pts, &pte, UT_TOL_pt) == 1) {
        // printf(" #### closed bsp ...\n");
        dist = UT_VAL_MAX;
        goto L_test;
      }
    }
  }


  // die Abweichung feststellen
  dist = UT3D_nlen_3pt(&pts, &pt1, &pte);
    // printf(" dist=%f\n",dist);
  // if(ptStackNr > 1) {dist = 0.001;} else {dist = 10.0;} // TEST


  // if(dist > tol) {
  // mind. 3 Punkte; sonst geht eine S-Form nicht !!
  L_test:
    // printf(" ptOut=%d ptStackNr=%d\n",ptOut,ptStackNr);
  if((dist > tol)||((ptOut < 2)&&(ptStackNr<2))) {
    // Abweichung zu gross; Punkt speichern
    // printf(" >>>> save in stack as [%d]\n",ptStackNr);
    ptStack[ptStackNr] = pt1;
    uStack[ptStackNr]  = pVal;
    ++ptStackNr;
    if(ptStackNr >= AUXTABSIZ) goto L_err_stack;
    goto L_next_inPoint;
  }


  // dist < tol; test if start,mid,end equal
  if(ptOut == 1) {
    if(ptStackNr == 2) {  // [0]=endpt; [1]=midpt
        // UT3D_stru_dump (Typ_PT, &ptStack[0], " ps0");
        // UT3D_stru_dump (Typ_PT, &ptStack[1], " ps1");
        // UT3D_stru_dump (Typ_PT, &pts, " pts");
        // UT3D_stru_dump (Typ_PT, &pt1, " pt1");
      // if ptStart == midPt 
      if(UT3D_comp2pt(&pTab[0], &ptStack[1], tol)) {   
        // && if ptend == midPt
        if(UT3D_comp2pt(&ptStack[0], &ptStack[1], tol)) {   
            // printf(" kvs==kve !\n");
          goto L_fertig;
        }
      }
    }
  }


  // Abweichung tolerierbar. pte raus.
    // UT3D_stru_dump (Typ_PT, &pte, " out pt %d",ptOut);
  pTab[ptOut] = pte; // copy Point
  // printf(" save pt as [%d]\n",ptOut);
  if(ptOut >= ptMax) goto L_err_ptab;
  ++ptOut;
  --ptStackNr;
  pts = pte;
  us = ue;
  if(ptStackNr > 0) goto L_next_inPoint;




  //-------------------------------------------
  L_fertig:
    // printf(" L_fertig: iout=%d\n",iout);

  if(ptOut < 2) {
    pTab[ptOut] = ptStack[0];
    ++ptOut;
  }

  // wenn v1 outside Curve: add Point.
  if(iout == 0) goto L_exit;
  d1 = cv1->v1 - kve;
  // get Endpt CV outside curve
  bspl_eval_expPt (&pTab[ptOut], cv1, kve, d1);
  ++ptOut;






  //----------------------------------------------------------------
  L_exit:
  *ptNr = ptOut;


  //------- Testausg------
  // printf("ex bspl_pol_bsp ptNr=%d\n",ptOut);
  // for(i1=0;i1<ptOut;++i1) {
    // UT3D_stru_dump (Typ_PT, &pTab[i1], "P[%d]=",i1);
    // // APT_disp_SymB (SYM_TRI_S, 2, &pTab[i1]);
  // }
  //------- Testausg------



  return 0;

  L_err_ptab:
    TX_Error("bspl_pol_bsp E001 - bufferOverflow");
    return -1;
  

  L_err_stack:
    TX_Print("bspl_pol_bsp E_stackOverflow");
    return -1;

}


//========================================================================
  int bspl_cvpol_cvbsp (int *ptNr, Point ptPol[],
                        int ianz, int ideg, double ps, double pe,
                        Point ptCrv[], double *vTab) {
//========================================================================
/*
 Aus einem B-Spline eine Polygon bilden.

IRC -2: ptPol zu klein 


Output:
   ptNr   = Anzahl Punkte in ptPol (Input: max Anzahl!)
   ptPol  = Output Polygonpunkte

Input:
   ianz   = Anzahl Segmente (Controlpoints - 1)
   ideg   = Degree
   ps, pe = Start / Endparameterwerte
   ptCrv  = Controlpoints
   vTab   = Knotentabelle


------------------------------------------
Verfahren:
  Im ersten Step wird die Schrittweite bestimmt; das ist das kleinste
  sinnvolle Increment.
  Dann den gesamten Spline in diese minimalen Schritte zerlegen und
  alle Zwischenpunkte, die eine zu geringe Abweichung haben, skippen.

*/

  int     ii, irc, iact, vlp, vvlp, maxInd;
  double  d1, d2, minInc, startInc, pVal, pEnd, peV1, peV2;
  Point   pt1, pt2;
  Vector  vc1;


  // printf("bspl_cvpol_cvbsp anz=%d deg=%d ps=%f pe=%f\n",ianz,ideg,ps,pe);


  // nur Test:
  // UT_TOL_cv = 0.1;    // die max erlaubte Abweichung von der



  maxInd = *ptNr - 1;


  // Feststellen der minimalen Schrittweite. Dazu den Punkt in der
  // Kurvenmitte holen, d1 = Abstand von Kurvenanfang - Kurvenmitte.
  // d2 = Parameterbereich Anfang - Ende;
  // minInc = d2 / (d1 * 1000) ist nun ein Parameterwert
  // fuer einen 0.0001 mm-Bereich.
  

  // den 1. Punkt sofort raus.
  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, ps, &pt1);
  if(irc < 0) return irc;
  ptPol[0] = pt1;
  // printf("  store 0. %f,%f,%f\n",pt1.x,pt1.y,pt1.z);
  

  // Kurvenmittelpunkt
  d1 = (pe-ps) / 2.;
  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, d1, &pt2);
  if(irc < 0) return irc;
  // printf("Mittelpunkt = %f,%f,%f irc=%d\n",pt2.x,pt2.y,pt2.z,irc);
  d1 = UT3D_len_2pt(&pt1, &pt2);
  // printf("halbe Laenge=%f\n",d1);
  

  minInc = (pe-ps) / (d1 * 500.);   // 500 = 1/1000 mm
  // printf("minInc=%f, ParBereich=%f\n",minInc,pe-ps);



  // bei weniger als 3 Punkten: Startp., Mittelp. Endpunkt raus.
  if(ps+(3*minInc) < pe) goto L_start_1;
  ptPol[1] = pt2;  // Mittelpunkt;
  iact = 2;
  goto L_Fertig;




  //---------------------------------------------------------
  L_start_1:
  pVal = ps;
  ii   = 0;
  iact = 1;


  // startInc soll als Laenge dem UT_TOL_cv entsprechen; solange mit
  // UT_TOL_cv incrementieren, bis es passt.
  L_find_startInc:
  // printf("L_find_startInc\n");
  pVal += minInc;
  ++ii;
  if(pVal >= pe) goto L_Fertig;

  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, pVal, &pt1);
  if(irc < 0) {
    printf("bspl_cvpol_cvbsp E456\n");
    return irc;
  }
  
  // Abweichung errechnen (Abst. Lotpunkt).
  d1 = UT3D_len_2pt (&ptPol[0], &pt1);
  if(d1 < UT_TOL_cv) goto L_find_startInc;





  startInc = pVal - ps;
  peV1 = pe - startInc;
  // peV2 = peV1 - startInc;
  peV2 = peV1 - minInc;
  // printf(" startInc = %f; ii=%d\n",startInc,ii);
  // printf(" V2=%f V1=%f pe=%f\n",peV2,peV1,pe);


  vvlp = -1;
  vlp  = 0;
  goto L_StorePoint;




  // 3 Punkte:
  // vlp - der vorletzte Punkt; // vvlp der vorvorletzte Punkt;
  // pt1 - der aktuelle Punkt. vvlp -> vlp ist die letzte Tangente,
  // abst der Abstand von pt1 -> (vvlp -> vlp).
  // Fuer den Zyklusstart sind also mind. 3 Punkte notwendig.

  // Konturstart: die ersten 2 Punkte haben den Abstand UT_TOL_cv; die
  //   letzen beiden ebenfalls.

  // Konturabschluss:
  //  die letzen 2 Punkte haben den gleichen fixen Abst,
  //  peV2 = u-Parameter des vorvorletzten Punktes
  //  peV1 = u-Parameter des vorletzten Punktes
  //  der vorvorletzte wird so bestimmt:
  //   wenn sein Abst zum vorletzen kleiner ist als UT_TOL_cv, (<pev1<pev2)
  //   wird er gespeichert und der vorletzte geskippt;
  //   wenn er zw. vorletzen und letzten liegt, (>pev2) wird er geskippt.
  //===============================================================
  NextPt:
    // printf("NextPt\n");

  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, pVal, &pt1);
  if(irc < 0) return irc;
  ++ii;
  

  // Abweichung errechnen (Abst. Lotpunkt).
  UT3D_pt_projptptvc(&pt2, &d1, NULL, &pt1, &ptPol[vvlp], &vc1);
  // d1 = UT3D_len_2pt (&pt1, &pt2);
  if(d1 > UT_TOL_cv) goto L_StorePoint;



  // Abweichung noch < UT_TOL_cv
  pVal += minInc;
  goto L_test_end;



  //------------------------
  L_StorePoint:
  // printf("  store %d %f,%f,%f dist=%d\n",iact,pt1.x,pt1.y,pt1.z,ii);
  ptPol[iact] = pt1;
  ++iact;
  if(iact >= maxInd) return -2;  // ptPol zu klein ..

  // den vektor vvlp -> vlp
  ++vvlp;
  ++vlp;
  UT3D_vc_2pt (&vc1, &ptPol[vvlp], &ptPol[vlp]);


  // es wurde punkt gespeichert, weiter mit startInc
  pVal += startInc;
  ii=0;


  L_test_end:
  if(pVal < peV2) goto NextPt;


  // printf("    end mit %f\n",pVal);


  // wenn dieser Punkt gespeichert wurde, ueberspeichern.
  if(ii == 0) --iact;


  if(pVal > peV1) {
    // pt1 liegt zw V1 und End; mit peV1 abspeichen.
    irc = bspl_eval_Pt (ianz,ideg,ptCrv,vTab,peV1,&pt1);
  }
    // else: pt1 liegt zw V2 und V1; abspeichern.

  ptPol[iact] = pt1;
  // printf("  store %d %f,%f,%f dist=%d\n",iact,pt1.x,pt1.y,pt1.z,ii);
  ++iact;
  if(iact >= maxInd) return -2;  // ptPol zu klein ..




  //===============================================================
  L_Fertig:
  // den Endpunkt
  irc = bspl_eval_Pt (ianz, ideg, ptCrv, vTab, pe, &pt1);
  if(irc < 0) return irc;
  ptPol[iact] = pt1;
  // printf("  store %d %f,%f,%f dist=%d\n",iact,pt1.x,pt1.y,pt1.z,ii);

  *ptNr = iact + 1;

  return 0;

}


//================================================================
  int bspl_pt_cbspDeg1 (Point *pto, int ptNr,
                        Point  *cpTab, double *kvTab, double uVal) {
//================================================================
// evaluate Point on B-SplCurve with degree 1.

 
  int       i1;
  double    d1, lpar;


  // printf("bspl_pt_cbspDeg1 %d %f\n",ptNr,uVal);
  // for(i1=0;i1<=ptNr;++i1)printf("kv[%d]=%f\n",i1,kvTab[i1]);


  for(i1=2; i1<=ptNr; ++i1) {
      // printf(" kv[%d]=%f\n",i1,kvTab[i1]);
    if(kvTab[i1] < uVal) continue;
      // printf(" kv[%d]%f\n",i1-1,kvTab[i1-1]);

    d1 = kvTab[i1] - kvTab[i1-1];
    if(fabs(d1) < UT_TOL_min0) {          // 2013-03-14 RF
      lpar = d1;
      UT3D_vc_2pt ((Vector*)pto, &cpTab[i1-2], &cpTab[i1-1]);
      goto L_done;
    }

    lpar = (uVal - kvTab[i1-1]) / d1;
      // printf(" lpar = %f\n",lpar);
    UT3D_pt_evpar2pt (pto, lpar, &cpTab[i1-2], &cpTab[i1-1]);
    goto L_done;
  }

  printf(" bspl_pt_cbspDeg1 E001\n");


  L_done:
    // GR_Disp_pt (pto, SYM_STAR_S, 9);

  return 0;

}


//========================================================================
  int  bspl_eval_Pt (int    segNr, int    ideg, Point  ContPt[],
                     double *vk,   double uVal, Point  *P) {
//========================================================================
/*                           i             i            i
                             i             i            o


Zwischenpunkte einer B-Spline-Kurve errechnen

ACHTUNG!! macht fehler bei deg=1 !!!!



aufgrund eines parameterwertes der sich im bereich der werte des knotenvektors
befinden muss, wird eine gewisse anzahl benachbarter kontrollpunkte ausgewaehlt.
diese kontrollpunkte werden gewichtet aufsummiert (welche da mitspielen wird in
dem b-array rekursiv berechnet, hier stehen die gewichte drin).

die anzahl der ausgewaehlten kontrollpunkte haengt direkt vom gewuenschten
 polynomgrad ab. die gewichte der einzelnen punkte haengt ausser
 vom polynomgrad noch
von der lage von uVal zu seinen benachbarten werten im knotenvektor ab
 und erechnen sich ueber die auswertung der so genannten Bernsteinpolynome.


 zum algorithmus der punktberechnung

- zuerst werden jene beiden werte des knotenvektors gesucht, die den uVal einsc
liessen: VK[i] <= uVal < VK[i+1]. ist das nirgends erfuellt, wird i so bestimmt
 dass gilt VK[i+1) = uVal. als start fuer die rekursive berechnung wird hier b[
] = 1.0 gesetzt.

- in einem weiteren schritt werden b[i-1] und b[i+1] aus b[i] und den aktuellen
werten der bernsteinpolynme an uVal berechnet.

- das wird wiederholt, bis sich der geforderte polynomgrad aufgebaut hat und so
it alle gewichte bestimmt sind (im normalfall sind die meisten gewichte der kon
rollpunkte 0)

- zum schluss addiert man die punkte gewichtet zusammen.




  Evaluate a point on a 3D (polynomial, uniform or not uniform) B-spline.

  The B-spline is defined by a series of control points, the degree of
  the polynomials, which make up the curve, and the knot vector.

  The point coordinates are computed using a parameter value

 -----------------------------------------------------------------------

   INPUT:
   ------
     long   segNr    ... is the number of segments
                         the number of control points  is segNr + 1 (0 - segNr)
     long   ndeg     ... degree of the polynomial arcs making up the curve
                         2 <= ndeg <= n
                         Achtung: ndeg = ideg -1 !
     Point  ContPt[] ... array of control points
     double *vk      ... knot vector. vk[i] <= vk[i+1], i = 0,1,...,n+ndeg !!!
                         vk[0] < vk[n+ndeg+1]
     double uVal     ... parameter value used to compute the point.
                         vk[0] <= u <= vk[n+ndeg+1]
     double *tmp     ... Tempspace for internal use; size = segNr + ideg + 1.


   OUTPUT:
   -------
     Point  *P       ... the computed point

     int    rc       ... Returncode:
                         0 = point computed
                         1 = n invalid
                         2 = ndeg invalid
                         3 = vk invalid
                         4 = u invalid
                         5 = internal Tempspace too small



***********************************************************************/


  double d,e,x,y,z;  //tmp[1000];
  long   i,k,m;

  int    ndeg, rc;

  double *b;


/* ...................................................................*/

  // printf("bspl_eval_Pt segNr=%d ideg=%d uVal=%f\n",segNr,ideg,uVal);


/*
  // check if uVal im Bereich
  if(uVal <= vk[0]) {
    printf(" underflow\n");


  } else if(uVal >= vk[segNr+ideg+1]) {
    printf(" overflow\n");
  }
*/



  b    = NULL;
  ndeg = ideg;



  /* check input */
  rc = 1;
  if (segNr<1) goto L_Err;

  rc = 2;
  if (ndeg<1 || ndeg>segNr) goto L_Err;  // entspr. ideg<2 !

  m = segNr +  ndeg + 1;   // geht bis zum letzten Index !
  // printf(" m=%d\n",m);
  // printf("     m=%d segNr=%d ideg=%d uVal=%f\n",m,segNr,ideg,uVal);

/*
  rc = 3;
  if (vk[0]>vk[m-1]) goto L_Err;
  for (i=0; i<m; ++i) {
    if (vk[i]>vk[i+1]) goto L_Err;
  }

*/

  rc = 4;
  // if (uVal<vk[0] || uVal>vk[m]) goto L_Err;
  if (uVal<vk[0]) uVal = vk[0];
  if (uVal>vk[m]) uVal = vk[m];

  // printf(" uVal=%f\n",uVal);



  if(m > 1000) { rc = 5; goto L_Err; }


  rc = 0;


  // spezialFall deg=1 behandeln
  if(ideg == 1) {
    bspl_pt_cbspDeg1 (P, segNr+1, ContPt, vk, uVal);
    return 0;
  }



  //-----------------------------------------------
  // allocate memory for computation of Bij
  // b =  (double*)  calloc (m+1,sizeof(double));
  // b = tmp;
  b =  (double*) MEM_alloc_tmp ((int)((m + 3) * sizeof(double)));
  if(!b) {TX_Error("bspl_eval_Pt EOM"); return 5;}


  /* init Bij */

  for (i=0; i<=m-1; ++i) {
    b[i] = 0.0;
  }
  for (i=0; i<m-1; ++i) {
    if ((uVal >= vk[i]) && (uVal < vk[i+1])) {b[i] = 1.0; goto INITOK;}
  }
  for (i=0; i<=m-1; ++i) {
    if (uVal == vk[i]) {b[i-1] = 1.0; goto INITOK;}
  }




  INITOK:

  // compute Bij
  for (k=1; k<=ndeg; ++k) {

    for (i=0; i<=m-k-1; ++i) {

      d = 0.0;
      if (b[i] != 0.0) {
        d = vk[i+k] - vk[i];
        if (d != 0.0) d = (uVal - vk[i]) / d * b[i];
      }

      e = 0.0;
      if (b[i+1] != 0.0) {
        e = vk[i+k+1] - vk[i+1];
        if (e != 0.0) e = (vk[i+k+1] - uVal) / e * b[i+1];
      }

      b[i] = d + e;

    }

    b[m-k] = 0.0;

  }


/*
  for(i=0; i<=m; ++i) {
    printf(" i=%2d vk=%9.4f b=%9.4f\n",i,vk[i],b[i]);
  }
*/



  /* compute point */
  x = 0.; y = 0.; z = 0.;


  for (i=0; i<=segNr; ++i) {
    // printf("%d b=%9.4f p=%9.4f %9.4f %9.4f\n",i,b[i],
            // ContPt[i].x,ContPt[i].y,ContPt[i].z);
    x +=  (ContPt[i].x) * b[i];
    y +=  (ContPt[i].y) * b[i];
    z +=  (ContPt[i].z) * b[i];
  }

  P->x = x;
  P->y = y;
  P->z = z;




  //=====================================================
  L_Err:

  // if (b != NULL) free(b);


  // printf("ex bspl_eval_Pt rc=%d u=%f p=%f,%f,%f\n",rc,uVal,P->x,P->y,P->z);


  return (rc);
}




/*====================================================================*/
  int  bspl_eval_Tg (int    segNr,  int    ideg,   Point  ContPt[],
                     double *vk,   double uVal,    Vector  *tg)
/*=================          i             i              i
*                            i             i              o
*
* Evaluate a tangent on a 3D (polynomial, uniform or not uniform) B-spline.
*
* The B-spline is defined by e series of control points, the degree of
* the polynomials, which make up the curve, and the knot vector.
*
* The point tangent vector is  computed using a parameter value
*
*-----------------------------------------------------------------------
*
*  INPUT:
*  ------
*    int    segNr    ... segNr+1 is the number of control points (0, 1, ...).
*                        segNr > 1
*    int    ideg     ... degree of the polynomial arcs making up the curve
*                        2 <= ideg <= n
*    Point  ContPt[] ... array of control points
*    double *vk      ... knot vector. vk[i] <= vk[i+1], i = 0,1,...,n+ideg !!!
*                        vk[0] < vk[n+ideg+1]
*    double uVal     ... parameter value used to compute the point.
*                        vk[0] <= uVal <= vk[n+ideg+1]
*  OUTPUT:
*  -------
*    Vector *tg      ... the computed tangent vector (not normalized)
*
*    int    rc       ... Returncode:
*                        0 = tangent computed
*                        1 = segNr invalid
*                        2 = ideg invalid
*                        3 = vk invalid
*                        4 = uVal invalid
*                        5 = internal Tempspace too small
*
*
*-----------------------------------------------------------------------
*
*
***********************************************************************/
  {

  double d,e;
  long   i,m,n1,m1;

  int    rc;
  Point  p1;
  Point  *p;


/* ...................................................................*/


  p = NULL;

/* check input */

  rc = 1;
  if (segNr<2) goto GOBACK;

  rc = 2;
  if (ideg<2 || ideg>segNr) goto GOBACK;

  m = segNr + ideg + 1;
/*
  rc = 3;
  if (vk[0]>vk[m]) goto GOBACK;
  for (i=0; i<m; ++i) {
    if (vk[i]>vk[i+1]) goto GOBACK;
  }
*/


  rc = 0;

  // allocate memory for computation of new control polygon
  // p =  (Point*)  calloc (segNr+2,sizeof(Point));
  p = (Point*) MEM_alloc_tmp ((int)((segNr+2) * sizeof(Point)));


  //  compute new control polygon

  e = ideg;

  d = vk[ideg] - vk[0];
  if (d != 0.0) {
    p[0].x = ContPt[0].x / d * e;
    p[0].y = ContPt[0].y / d * e;
    p[0].z = ContPt[0].z / d * e;
  } else {
    p[0].x = 0.0;
    p[0].y = 0.0;
    p[0].z = 0.0;
  }

  d = vk[ideg+segNr+1] - vk[segNr+1];
  if (d != 0.0) {
    p[segNr+1].x = -ContPt[segNr].x / d * e;
    p[segNr+1].y = -ContPt[segNr].y / d * e;
    p[segNr+1].z = -ContPt[segNr].z / d * e;
  } else {
    p[segNr+1].x = 0.0;
    p[segNr+1].y = 0.0;
    p[segNr+1].z = 0.0;
  }

  for (i=1; i<=segNr; ++i) {
    d = vk[ideg+i] - vk[i];
    if (d != 0.0) {
      p[i].x = (ContPt[i].x-ContPt[i-1].x) / d * e;
      p[i].y = (ContPt[i].y-ContPt[i-1].y) / d * e;
      p[i].z = (ContPt[i].z-ContPt[i-1].z) / d * e;
    } else {
      p[i].x = 0.0;
      p[i].y = 0.0;
      p[i].z = 0.0;
    }
  }


  // Compute the tangent
  n1 = segNr+1;
  m1 = ideg-1;

  rc = bspl_eval_Pt (n1, m1, p, vk, uVal, &p1);

  // copy 
  tg->dx = p1.x;
  tg->dy = p1.y;
  tg->dz = p1.z;


GOBACK:

  // if (p != NULL) free(p);

  // printf("ex bspl_eval_Tg rc=%d u=%e vc=%f,%f,%f\n",rc,uVal,
                          // tg->dx,tg->dy,tg->dz);

  return (rc);
}


//===========================================================================
  int UT3D_pt_projptbsp_ext (double *uo,Point *po,Point *p0,CurvBSpl *bspl) {
//===========================================================================
// lineare extrapolation
// Returncodes:
//   1   v0 changed (uo = v0)
//   2   v1 changed (uo = v1)


  int     ii;
  double  d1, u0, *u1, *u2;
  Point   ptx, *p1, *p2;
  Vector  vct;


  // UT3D_stru_dump (Typ_PT, p0, "UT3D_pt_projptbsp_ext ");
  // UT3D_stru_dump (Typ_CVBSP, bspl, "");


  // welcher ist der naehere Punkt
  p1 = &bspl->cpTab[0];
  p2 = &bspl->cpTab[bspl->ptNr - 1];

  u1 = &bspl->kvTab[0];
  u2 = &bspl->kvTab[bspl->ptNr + bspl->deg];


  ii = UT3D_minLen_3pt (&d1, p0, p1, p2);
  if(ii == 1) {
    u0 = *u1;
  }  else {
    u0 = *u2;
    p1 = p2;
  }
    // GR_Disp_pt (p1, SYM_TRI_S, 2);

  // Tangente holen
  UT3D_vc_evalparCv (&vct, bspl, u0);
    // GR_Disp_vc (&vct, p1, 9, 0);

  // project Point --> tangente
  UT3D_pt_projptptvc (po, &d1, NULL, p0, p1, &vct);
    // GR_Disp_pt (po, SYM_STAR_S, 2);

  // Abstand CurvEndpkt - po messen (d1 = Abst p0 - po)
  bspl_eval_Pt (bspl->ptNr-1, bspl->deg, bspl->cpTab, bspl->kvTab, u0, &ptx);
  d1 = UT3D_len_2pt (po, &ptx);

  // add len-offset to parameter
  // printf(" d1=%f u0=%f u1=%f u2=%f\n",d1,u0,*u1,*u2);
  if(ii == 2) d1 = -d1;
  if(*u2 > *u1) d1 = -d1;
  if(uo) *uo = u0 + d1;

  // printf("ex UT3D_pt_projptbsp_ext %d %f\n",ii,*uo);

  return ii;

}


//=========================================================================
  int bspl_eval_expPt (Point *pto, CurvBSpl *cv1, double uv, double dist) {
//=========================================================================
// uv is outside curve;  iout is from func UTP_db_ck_in2db
// extrapolate len=dist

  double  d1;
  Point   pt0;
  Vector  vct;

  // get Endpt CV
  bspl_eval_Pt (cv1->ptNr - 1, cv1->deg, cv1->cpTab, cv1->kvTab, uv, &pt0);
    // GR_Disp_pt (&pt0, SYM_STAR_S, 3);
  // get tangente
  UT3D_vc_evalparCv (&vct, cv1, uv);
  // extrapolated point
  UT3D_pt_traptvclen (pto, &pt0, &vct, dist);
    // GR_Disp_pt (pto, SYM_STAR_S, 2);


  return 0;

}


//================================================================
  int bspl_segNr_par (int *segNr, double up, CurvBSpl *cv1) {
//================================================================
// get segmentNr from parameter
//   up          parameter
//   segNr

// TODO:
// muesste besser via knotentabelle gehen

  int      i1;
  double   u1, d1;
  Point    *pTab;
  // void     *tPos;


  // printf("bspl_segNr_par %f\n",up);

  // tPos = tmpSpc1->next;        // save tmpSpc
  pTab = cv1->cpTab;


  // loop tru controlpoints & 
  *segNr = cv1->ptNr - 2;
  for(i1=1; i1 < cv1->ptNr - 1; ++i1) {
    UT3D_parCv_bsplpt (&u1, &d1, cv1, &pTab[i1]);
      // printf(" u1=%f\n",u1);
    // reset tmpSpc
    // tmpSpc1->next = tPos;

    if(u1 > up)  { *segNr = i1 - 1; break;}

  }

  // printf("ex bspl_segNr_par %d\n",*segNr);


  return 0;

}


/*
//=================================================================
  int UT3D_pt_tng_cbsp_vc__ (Point *pto,
                             Vector *vc1, CurvBSpl *cv1, int isol) {
//=================================================================
/// get tangent vector-bSpline.
/// Bspline must be planar to constr.Plane.
///   isol    0=solution-1; 1=solution-2;
/// 2013-04-20 Reiter
///     0      OK
///    -1      Error isol
///    -2      internal error
///    -3      EOM

  int      i0, i1, i2, ii, isf, pNr, iRun;
  double   d1, d2, dd, *da;
  Point    pt1, pt2;
  Point2   p2b, *p2a, p21, p22; 
  Vector   vcb;
  Vector2  v01, v12, v02, v2b;
    

  printf("UT3D_pt_tng_cbsp_vc__ %d\n",isol); 
    // UT3D_stru_dump (Typ_VC, vc1, "  vc1:");
    // UT3D_stru_dump (Typ_CVBSP, cv1, "  bsp:");


// transfer vector -> 2D
  vcb = UTRA_vc_abs2rel__ (vc1);
  UT2D_vc_vc3 (&v2b, &vcb);
    UT3D_stru_dump (Typ_VC2, &v2b, "  v2b:");


//----------------------------------------------------------------
// create a 2D-test-polygon; #-of-points: twice the # of controlPoints.
  d2 = cv1->v1 - cv1->v0;
  pNr = (cv1->ptNr * 2);
  d2 /= pNr;
  ++pNr;

// use memspc50 for pNr 2D-points;
  // p2a = (Point2*)memspc50;
  p2a = (Point2*)MEM_alloc_tmp (pNr * sizeof(Point2));
  if(!p2a) {TX_Error("UT3D_pt_tng_cbsp_vc__ EOM"); return -3;}
// use memspc51 for pNr parameters
  // da = (double*)memspc51;
  da = (double*)MEM_alloc_tmp (pNr * sizeof(double));
  if(!da) {TX_Error("UT3D_pt_tng_cbsp_vc__ EOM"); return -3;}

  iRun = 0;

  d1 = cv1->v0;
    // printf(" d2=%f i0=%d\n",d2,i0);
  for(i1=0; i1<pNr; ++i1) {
      // printf(" i1=%d d1=%f\n",i1,d1);
    UT3D_pt_evparCrvBSpl (&pt1, cv1, d1);
    da[i1] = d1;
    pt2 = UTRA_pt_abs2rel__ (&pt1);
    p2a[i1] = UT2D_pt_pt3 (&pt2);
      // UT3D_stru_dump (Typ_PT2, &p2a[i1], "  p2a[%d]",i1);
      // GR_Disp_pt2 (&p2a[i1], SYM_STAR_S, 2);
    d1 += d2;
  }
    // printf(" pNr=%d\n",pNr);


//----------------------------------------------------------------
// find tangent-polgon-segments
// test next 3 points; i0,i1,i2.
  L_nxt0:
  i1 = 0;
  i2 = 1;
  UT2D_vc_2pt (&v12, &p2a[0], &p2a[1]);
  isf = 0;  // solution found
    // UT3D_stru_dump (Typ_VC2, &v12, "  0-1_v12");


  L_nxt1:
    i0 = i1;
    i1 = i2;
    ++i2;
    v01 = v12;
    UT2D_vc_2pt (&v12, &p2a[i1], &p2a[i2]); // vector for new v12
      // UT3D_stru_dump (Typ_VC2, &v12, "  v12[%d]",i2);

    // is vector v2 between v0-v1 ?
    ii = UT2D_ckvc_in2vc (&v01, &v2b, &v12);
      // printf(" nxt sol: i0=%d i1=%d i2=%d ii=%d\n",i0,i1,i2,ii);
    if(ii == 0) {
      // solution found ..
      if(isf >= isol) goto L_res;
      i0 = i1;
      i1 = i2;
      ++i2;
      v01 = v12;
      UT2D_vc_2pt (&v12, &p2a[i1], &p2a[i2]); // vector for new v12
        // UT3D_stru_dump (Typ_VC2, &v12, " isf-v12[%d]",i2);
      ++isf;
    }
    if(i2 < (pNr - 1)) goto L_nxt1;
    return -1;


//----------------------------------------------------------------
// solution is between point i0/i1/i2;
// make 2 new points between i0-i1  and i1-i2; this gives 5 points.
// test this 5 points, where mid-vector changes
  L_res:
    // printf(">>>>>>>>>>> iRun=%d i0=%d i1=%d i2=%d\n",iRun,i0,i1,i2);
  UT2D_vc_2pt (&v02, &p2a[i0], &p2a[i2]);
  ii = UT2D_slen_nor2vc (&d1, &v2b, &v02);
  if(fabs(d1) < UT_TOL_pt) goto L_found;
  if(iRun > 100) goto Err_no_sol;

  // copy points and parameters -> 0,2,4
  if(i0 != 0) { p2a[0] = p2a[i0]; da[0] = da[i0];}
  p21 = p2a[i1];  d1 = da[i1];
  p22 = p2a[i2];  d2 = da[i2];
  p2a[2] = p21;   da[2] = d1;
  p2a[4] = p22;   da[4] = d2;

  // set parameters and points for 1,3
  da[1] = (da[0] + da[2]) / 2.;
  da[3] = (da[2] + da[4]) / 2.;

  // get midpoints 1, 3
  UT3D_pt_evparCrvBSpl (&pt1, cv1, da[1]);
  pt2 = UTRA_pt_abs2rel__ (&pt1);
  p2a[1] = UT2D_pt_pt3 (&pt2);
    // GR_Disp_pt2 (&p2a[1], SYM_STAR_S, 3);

  UT3D_pt_evparCrvBSpl (&pt1, cv1, da[3]);
  pt2 = UTRA_pt_abs2rel__ (&pt1);
  p2a[3] = UT2D_pt_pt3 (&pt2);
    // GR_Disp_pt2 (&p2a[3], SYM_STAR_S, 4);

  pNr = 5;
  isol = 0;  // isol only for first run
  ++iRun;
  goto L_nxt0;



//----------------------------------------------------------------
  L_found:
      // printf(" d1=%f\n",d1);
      // printf(" pb-p1=%f\n",UT2D_len_2pt(&p2b,&p2a[i1]));
// transfer back into 3D
  // L_out:
  pt1 = UT3D_pt_pt2 (&p2a[1]);
  *pto = UTRA_pt_rel2abs__ (&pt1);

  return 0;


  Err_no_sol:
    TX_Error ("UT3D_pt_tng_cbsp_vc__ E001");
    return -2;
}
*/
/* UNUSED
//=================================================================
  int UT3D_vc_tng_cbsp_pt__ (Vector *vco,
                             Point *ptb, CurvBSpl *cv1, int isol) {
//=================================================================
/// \code
/// get tangent point-bSpline.
/// Bspline must be planar to constr.Plane.
///   isol    0=solution-1; 1=solution-2;
/// 2013-04-20 Reiter
///     1      OK, point is on bSpline
///     0      OK, point is not on bSpline
///    -1      Error isol
///    -2      internal error
///    -3      EOM
/// \endcode


  int      i0, i1, i2, ii, isf, pNr, iRun;
  double   d1, d2, dd, *da;
  Point    pt1, pt2;
  Point2   p2b, *p2a, p21, p22;
  Vector2  v0, v1, v2, v02;


  printf("UT3D_vc_tng_cbsp_pt__ %d\n",isol);
    // UT3D_stru_dump (Typ_PT, ptb, "  ptb:");
    // UT3D_stru_dump (Typ_CVBSP, cv1, "  cv:");

// transfer tangent-point -> 2D
  pt1 = UTRA_pt_abs2rel__ (ptb);
  p2b = UT2D_pt_pt3 (&pt1);
    // UT3D_stru_dump (Typ_PT2, &p2b, "  p2b:");


//----------------------------------------------------------------
// create a 2D-test-polygon; #-of-points: twice the # of controlPoints.
  d2 = cv1->v1 - cv1->v0;
  pNr = (cv1->ptNr * 2);
  d2 /= pNr;
  ++pNr;

// use memspc50 for pNr 2D-points;
  // p2a = (Point2*)memspc50;
  p2a = (Point2*)MEM_alloc_tmp (pNr * sizeof(Point2));
  if(!p2a) {TX_Error("UT3D_vc_tng_cbsp_pt__ EOM"); return -3;}
// use memspc51 for pNr parameters
  // da = (double*)memspc51;
  da = (double*)MEM_alloc_tmp (pNr * sizeof(double));
  if(!da) {TX_Error("UT3D_vc_tng_cbsp_pt__ EOM"); return -3;}

  iRun = 0;

  d1 = cv1->v0;
    // printf(" d2=%f i0=%d\n",d2,i0);
  for(i1=0; i1<pNr; ++i1) {
      // printf(" i1=%d d1=%f\n",i1,d1);
    UT3D_pt_evparCrvBSpl (&pt1, cv1, d1);
    da[i1] = d1;
    pt2 = UTRA_pt_abs2rel__ (&pt1);
    p2a[i1] = UT2D_pt_pt3 (&pt2);
      // UT3D_stru_dump (Typ_PT2, &p2a[i1], "  p2a[%d]",i1);
      // GR_Disp_pt2 (&p2a[i1], SYM_STAR_S, 2);
    d1 += d2;
  }


//----------------------------------------------------------------
// find tangent-polgon-segments
// test next 3 points; i0,i1,i2.
// v0 = pb-i0;  v1 = pb-i1; v2 = pb-i2.
// test if v2 is between v0-v1. Yes: found possible tangent ..

  L_nxt0:
  i1 = 0;
  i2 = 1;
  UT2D_vc_2pt (&v1, &p2b, &p2a[i1]);
  UT2D_vc_2pt (&v2, &p2b, &p2a[i2]);
  isf = 0;  // solution found
    // UT3D_stru_dump (Typ_VC2, &v1, "  v0");
    // UT3D_stru_dump (Typ_VC2, &v2, "  v1");


  L_nxt1:
    i0 = i1;
    i1 = i2;
    ++i2;
    v0 = v1;
    v1 = v2;
    UT2D_vc_2pt (&v2, &p2b, &p2a[i2]); // vector for new v12
      // UT3D_stru_dump (Typ_VC2, &v2, "  v2[%d]",i2);

    // is vector v2 between v0-v1 ?
    ii = UT2D_ckvc_in2vc (&v0, &v1, &v2);
      // printf(" i0=%d i1=%d i2=%d ii=%d\n",i0,i1,i2,ii);
    if(ii) {
      // solution found ..
      if(isf >= isol) goto L_res;
      i0 = i1;
      i1 = i2;
      ++i2;
      v0 = v1;
      v1 = v2;
      UT2D_vc_2pt (&v2, &p2b, &p2a[i2]); // vector for new v12
      ++isf;
    }
    if(i2 < (pNr - 1)) goto L_nxt1;
    return -1;


//----------------------------------------------------------------
// solution is between point i0/i1/i2;
// make 2 new points between i0-i1  and i1-i2; this gives 5 points.
// test this 5 points, where mid-vector changes
  L_res:
    // printf(">>>>>>>>>>> iRun=%d i0=%d i1=%d i2=%d\n",iRun,i0,i1,i2);
  UT2D_vc_2pt (&v02, &p2a[i0], &p2a[i2]);
  ii = UT2D_slen_nor2vc (&d1, &v1, &v02);
  if(fabs(d1) < UT_TOL_pt) goto L_found;
  if(iRun > 100) goto Err_no_sol;

  // copy points and parameters -> 0,2,4
  if(i0 != 0) { p2a[0] = p2a[i0]; da[0] = da[i0];}
  p21 = p2a[i1];  d1 = da[i1];
  p22 = p2a[i2];  d2 = da[i2];
  p2a[2] = p21;   da[2] = d1;
  p2a[4] = p22;   da[4] = d2;

  // set parameters and points for 1,3
  da[1] = (da[0] + da[2]) / 2.;
  da[3] = (da[2] + da[4]) / 2.;

  // get midpoints 1, 3
  UT3D_pt_evparCrvBSpl (&pt1, cv1, da[1]);
  pt2 = UTRA_pt_abs2rel__ (&pt1);
  p2a[1] = UT2D_pt_pt3 (&pt2);
    // GR_Disp_pt2 (&p2a[1], SYM_STAR_S, 3);

  UT3D_pt_evparCrvBSpl (&pt1, cv1, da[3]);
  pt2 = UTRA_pt_abs2rel__ (&pt1);
  p2a[3] = UT2D_pt_pt3 (&pt2);
    // GR_Disp_pt2 (&p2a[3], SYM_STAR_S, 4);

  pNr = 5;
  isol = 0;  // isol only for first run
  ++iRun;
  goto L_nxt0;



//----------------------------------------------------------------
  L_found:
      // printf(" d1=%f\n",d1);
      // printf(" pb-p1=%f\n",UT2D_len_2pt(&p2b,&p2a[i1]));

  // liegt p2b zwischen p2a[i0] - p2a[i2] ?
  ii = UT2D_slenq_ptptvc (&d1, &p2a[i0], &p2a[i2], &p2b);
  // d2 = sqrt(fabs(d1));
      // printf(" ii=%d\n",ii);
  if(ii > 0) {
    d1 = UT2D_len_2pt (&p2a[i2], &p2b);
      printf(" d1=%f\n",d1);
    if(d1 > (UT_DISP_cv*3.)) goto L_out;
  }
  if(ii < 0) {  
    d1 = UT2D_len_2pt (&p2a[i0], &p2b);
      printf(" d1=%f\n",d1);
    if(d1 > (UT_DISP_cv*3.)) goto L_out;
  }
    // printf(" ************** on Curve !\n");
  // translate p2a[2] into direction v02 length = APT_ln_len
  UT2D_pt_traptvclen (&p2a[2], &p2b, &v02, APT_ModSiz);


// transfer back into 3D
  L_out:
  pt1 = UT3D_pt_pt2 (&p2a[1]);
  pt2 = UTRA_pt_rel2abs__ (&pt1);
  // set vco = ptb -> ptt
  UT3D_vc_2pt (vco, ptb, &pt2);
    // GR_Disp_vc (vco, ptb, 9, 1);

  return 0;


  Err_no_sol:
    TX_Error ("UT3D_vc_tng_cbsp_pt__ E001");
    return -2;
}
*/

//================================================================
  int UT2D_pt_evpar_cbsp (Point2 *pto, CurvBSpl2 *cv, double u) {
//================================================================

  int     rc, d, n, m, r, s;
  int     i1, i2, ih, jh, n1;
  double  a, b;
  Point2  *ptab;


  // some abbreviations
  d = cv->deg;
  n = cv->ptNr - 1;
  m = n + d + 1;

  // check parameter
  if (u < cv->kvTab[d]) {
    // printf("UT3D_pt_evparCrvBSpl: parameter below %f\n",u);
    u = cv->kvTab[d];
  }
  if (u > cv->kvTab[cv->ptNr]) {
    // printf("UT3D_pt_evparCrvBSpl: parameter above %f\n",u);
    u = cv->kvTab[cv->ptNr];
  }

  // determine segment of parameter
  r = -1;
  for (s=d; s<m-d; ++s) {
    if (cv->kvTab[s] <= u && u <= cv->kvTab[s+1]) {
      r = s;
      break;
    }
  }
  if (r == -1) {
    printf("UT2D_pt_evpar_cbsp: input error\n");
    return -2;
  }

  // provide work space
  ptab = (Point2*)MEM_alloc_tmp ((int)(sizeof(Point2) * (d+1)));
  if(!ptab) {TX_Error("UT2D_pt_evpar_cbsp: EOM"); return -1;}


  // init control points
  ih = r - d;
  for (i1=0; i1<d+1; ++i1) ptab[i1] = cv->cpTab[ih+i1];

  // compute curve point
  n1 = d;
  for (i1=0; i1<d; ++i1) {
    ih = r + 1;
    jh = ih - n1;
    for (i2=0; i2<n1; ++i2) {
      a = (u - cv->kvTab[jh+i2]) / (cv->kvTab[ih+i2] - cv->kvTab[jh+i2]);
      b = 1.0 - a;
      ptab[i2].x = b * ptab[i2].x + a * ptab[i2+1].x;
      ptab[i2].y = b * ptab[i2].y + a * ptab[i2+1].y;
    }
    --n1;
  }
  *pto = ptab[0];

  // printf("ex UT3D_pt_evparCrvBSpl %f,%f,%f\n",pt->x,pt->y,pt->z);

  return 0;


  //----------------------------------------------------------------
  // L_outOfWorkSpace:
    // TX_Error("UT2D_pt_evpar_cbsp: out of work space");
    // return -1;
}


//=================================================================
  int UT2D_pt_tng_cbsp_vc (Point2 *ptg,
                           Vector2 *vc1, CurvBSpl2 *cv1, int isol) {
//=================================================================
/// get tangent vector-bSpline.
/// Bspline must be planar to constr.Plane.
///   isol    0=solution-1; 1=solution-2;
/// 2013-04-20 Reiter
/// RetCod:
///    >0      OK, max nr of solutions
///    -1      Error isol
///    -2      internal error
///    -3      EOM

// see also UT3D_pt_tng_cbsp_vc__ UT2D_pt_tng_cbsp_pt

  int      i0, i1, i2, ii, isf, pNr, iRun, iMaxSol;
  double   d1, d2, dd, *da;
  Point    pt1, pt2;
  Point2   p2b, *pa, p21, p22;
  Vector2  v01, v12, v02, vcn;


  // printf("UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU \n");
  // printf("UT2D_pt_tng_cbsp_vc %d\n",isol);
    // UT3D_stru_dump (Typ_VC2, vc1, "  vc1:");
    // UT3D_stru_dump (Typ_CVBSP2, cv1, "  bsp:");

  iMaxSol = isol + 2;

  UT2D_vc_setLength (&vcn, vc1, 1.);


//----------------------------------------------------------------
// create a 2D-test-polygon; #-of-points: twice the # of controlPoints.
  d2 = cv1->v1 - cv1->v0;
  pNr = (cv1->ptNr * 2);
  d2 /= pNr;
  ++pNr;

// use memspc50 for pNr 2D-points;
  pa = (Point2*)MEM_alloc_tmp ((int)(pNr * sizeof(Point2)));
  if(!pa) {TX_Error("UT2D_pt_tng_cbsp_vc EOM"); return -3;}
// use memspc51 for pNr parameters
  da = (double*)MEM_alloc_tmp ((int)(pNr * sizeof(double)));
  if(!da) {TX_Error("UT2D_pt_tng_cbsp_vc EOM"); return -3;}

  iRun = 0;

  d1 = cv1->v0;
    // printf(" d2=%f i0=%d\n",d2,i0);
  for(i1=0; i1<pNr; ++i1) {
      // printf(" i1=%d d1=%f\n",i1,d1);
    UT2D_pt_evpar_cbsp (&pa[i1], cv1, d1);
    da[i1] = d1;
      // UT3D_stru_dump (Typ_PT2, &pa[i1], "  pa[%d]",i1);
      // GR_Disp_pt2 (&pa[i1], SYM_STAR_S, 2);
    d1 += d2;
  }
    // printf(" pNr=%d\n",pNr);


//----------------------------------------------------------------
// find tangent-polgon-segments
// test next 3 points; i0,i1,i2.
  L_nxt0:
  i1 = 0;
  i2 = 1;
  UT2D_vc_2pt (&v12, &pa[0], &pa[1]);
  isf = 0;  // solution found
    // UT3D_stru_dump (Typ_VC2, &v12, "  0-1_v12");


  L_nxt1:
    i0 = i1;
    i1 = i2;
    ++i2;
    v01 = v12;
    UT2D_vc_2pt (&v12, &pa[i1], &pa[i2]); // vector for new v12
      // UT3D_stru_dump (Typ_VC2, &v12, "  v12[%d]",i2);

    // is vector v2 between v0-v1 ?
    ii = UT2D_ckvc_inA_vc_vc (&v01, &vcn, &v12);
      // printf(" nxt sol: i0=%d i1=%d i2=%d ii=%d\n",i0,i1,i2,ii);
    if(ii == 0) {
      // solution found ..
      if(isf >= isol) goto L_res;
      i0 = i1;
      i1 = i2;
      ++i2;
      v01 = v12;
      UT2D_vc_2pt (&v12, &pa[i1], &pa[i2]); // vector for new v12
        // UT3D_stru_dump (Typ_VC2, &v12, " isf-v12[%d]",i2);
      ++isf;
    }
    if(i2 < (pNr - 1)) goto L_nxt1;
      // printf("*****  UT2D_pt_tng_cbsp_vc exit -1\n");
    return -1;


//----------------------------------------------------------------
// solution is between point i0/i1/i2;
// make 2 new points between i0-i1  and i1-i2; this gives 5 points.
// test this 5 points, where mid-vector changes
  L_res:
    // printf(">>>>>>>>>>> iRun=%d i0=%d i1=%d i2=%d\n",iRun,i0,i1,i2);
  UT2D_vc_2pt (&v02, &pa[i0], &pa[i2]);
  ii = UT2D_slen_nor_vc_vcNo (&d1, &vcn, &v02);
    // printf(" _nor_vc_ d1=%f tol=%f\n",d1,UT_TOL_pt);
  if(fabs(d1) < UT_TOL_pt) goto L_found;
  if(iRun > 100) goto Err_no_sol;

  // copy points and parameters -> 0,2,4
  if(i0 != 0) { pa[0] = pa[i0]; da[0] = da[i0];}
  p21 = pa[i1];  d1 = da[i1];
  p22 = pa[i2];  d2 = da[i2];
  pa[2] = p21;   da[2] = d1;
  pa[4] = p22;   da[4] = d2;

  // set parameters and points for 1,3
  da[1] = (da[0] + da[2]) / 2.;
  da[3] = (da[2] + da[4]) / 2.;

  // get midpoints 1, 3
  UT2D_pt_evpar_cbsp (&pa[1], cv1, da[1]);
    // GR_Disp_pt2 (&pa[1], SYM_STAR_S, 3);

  UT2D_pt_evpar_cbsp (&pa[3], cv1, da[3]);
    // GR_Disp_pt2 (&pa[3], SYM_STAR_S, 4);

  pNr = 5;
  isol = 0;  // isol only for first run
  ++iRun;
  goto L_nxt0;



//----------------------------------------------------------------
  L_found:
      // printf(" L_found: d1=%f isf=%d\n",d1,isf);
      // printf(" pb-p1=%f\n",UT2D_len_2pt(&p2b,&pa[i1]));
      // UT3D_stru_dump (Typ_PT2, &pa[i1], "  pa[%d]",i1);
  *ptg = pa[i1];

  return iMaxSol; // isf;


  Err_no_sol:
    TX_Error ("UT2D_pt_tng_cbsp_vc E001");
    return -2;
}


//=================================================================
  int UT2D_pt_tng_cbsp_pt (Point2 *ptg, int *isolMax,
                           CurvBSpl2 *cv, Point2 *pt1, int isol) {
//=================================================================
/// \code
/// get tangent point-bSpline.
/// Bspline must be planar to constr.Plane.
///   isol     0-n; 0=solution-1; 1=solution-2;
/// 2013-04-20 Reiter
///     1      OK, point is on bSpline
///     0      OK, point is not on bSpline
///    -1      Error isol; isolMax gives max nr of solutions
///    -2      internal error
///    -3      EOM
/// \endcode

// see also UT3D_vc_tng_cbsp_pt__


  int      i0, i1, i2, ii, isf, pNr, iRun;
  double   d1, d2, dd, *da;
  Point2   *pa, p21, p22;
  Vector2  v0, v1, v2, v02;
  

  // printf("============================================= \n");
  // printf("UT2D_pt_tng_cbsp_pt %d\n",isol);
  // UT3D_stru_dump(Typ_PT2, pt1, "  pt1:");


//----------------------------------------------------------------
// create a 2D-test-polygon; #-of-points: twice the # of controlPoints.
  d2 = cv->v1 - cv->v0;
  pNr = (cv->ptNr * 2);
  d2 /= pNr;
  ++pNr;

// use memspc50 for pNr 2D-points;
  // p2a = (Point2*)memspc50;
  pa = (Point2*)MEM_alloc_tmp ((int)(pNr * sizeof(Point2)));
  if(!pa) {TX_Error("UT2D_pt_tng_cbsp_pt EOM"); return -3;}
// use memspc51 for pNr parameters
  // da = (double*)memspc51;
  da = (double*)MEM_alloc_tmp ((int)(pNr * sizeof(double)));
  if(!da) {TX_Error("UT2D_pt_tng_cbsp_pt EOM"); return -3;}

  iRun = 0;

  // create testpolygon 
  d1 = cv->v0;
    // printf(" d2=%f i0=%d\n",d2,i0);
  for(i1=0; i1<pNr; ++i1) {
      // printf(" i1=%d d1=%f\n",i1,d1);
    UT2D_pt_evpar_cbsp (&pa[i1], cv, d1);
    da[i1] = d1;
      // UT3D_stru_dump (Typ_PT2, &p2a[i1], "  p2a[%d]",i1);
      // GR_Disp_pt2 (&pa[i1], SYM_STAR_S, 2);
    d1 += d2;
  }


//----------------------------------------------------------------
// find tangent-polgon-segments
// test next 3 points; i0,i1,i2.
// v0 = pb-i0;  v1 = pb-i1; v2 = pb-i2.
// test if v2 is between v0-v1. Yes: found possible tangent ..

  L_nxt0:
  i1 = 0;
  i2 = 1;
  UT2D_vc_2pt (&v1, pt1, &pa[i1]);
  UT2D_vc_2pt (&v2, pt1, &pa[i2]);
  isf = 0;  // solution found
    // UT3D_stru_dump (Typ_VC2, &v1, "  v0");
    // UT3D_stru_dump (Typ_VC2, &v2, "  v1");


  L_nxt1:
    i0 = i1;
    i1 = i2;
    ++i2;
    v0 = v1;
    v1 = v2;
    UT2D_vc_2pt (&v2, pt1, &pa[i2]); // vector for new v12
      // UT3D_stru_dump (Typ_VC2, &v2, "  v2[%d]",i2);

    // is vector v2 between v0-v1 ?
    ii = UT2D_ckvc_inA_vc_vc (&v0, &v1, &v2);
      // printf(" i0=%d i1=%d i2=%d ii=%d\n",i0,i1,i2,ii);
    if(ii) {
      // solution found ..
      if(isf >= isol) goto L_res;
      i0 = i1;
      i1 = i2;
      ++i2;
      v0 = v1;
      v1 = v2;
      UT2D_vc_2pt (&v2, pt1, &pa[i2]); // vector for new v12
      ++isf;
    }
    if(i2 < (pNr - 1)) goto L_nxt1;
      // printf("**** UT2D_pt_tng_cbsp_pt err-no-sol %d\n",isf);
    *isolMax = isf - 1;
    return -1;


//----------------------------------------------------------------
// solution is between point i0/i1/i2;
// make 2 new points between i0-i1  and i1-i2; this gives 5 points.
// test this 5 points, where mid-vector changes
  L_res:
    // printf(">>>>>>>>>>> iRun=%d i0=%d i1=%d i2=%d\n",iRun,i0,i1,i2);
  UT2D_vc_2pt (&v02, &pa[i0], &pa[i2]);
  ii = UT2D_slen_nor2vc (&d1, &v1, &v02);
  if(fabs(d1) < UT_TOL_pt) goto L_found;
  if(iRun > 100) goto Err_no_sol;

  // copy points and parameters -> 0,2,4
  if(i0 != 0) { pa[0] = pa[i0]; da[0] = da[i0];}
  p21 = pa[i1];  d1 = da[i1];
  p22 = pa[i2];  d2 = da[i2];
  pa[2] = p21;   da[2] = d1;
  pa[4] = p22;   da[4] = d2;

  // set parameters and points for 1,3
  da[1] = (da[0] + da[2]) / 2.;
  da[3] = (da[2] + da[4]) / 2.;

  // get midpoints 1, 3
  UT2D_pt_evpar_cbsp (&pa[1], cv, da[1]);
    // GR_Disp_pt2 (&pa[1], SYM_STAR_S, 3);

  UT2D_pt_evpar_cbsp (&pa[3], cv, da[3]);
    // GR_Disp_pt2 (&pa[3], SYM_STAR_S, 4);

  pNr = 5;
  isol = 0;  // isol only for first run
  ++iRun;
  goto L_nxt0;



//----------------------------------------------------------------
  L_found:
      // printf(" L_found: d1=%f\n",d1);
      // printf(" pb-p1=%f\n",UT2D_len_2pt(pt1,&pa[i1]));
  *ptg = pa[i1];
    // GR_Disp_pt2 (ptg, SYM_STAR_S, 4);

  // liegt pt1 zwischen pa[i0] - pa[i2] ?
  ii = UT2D_slenq_ptptvc (&d1, &pa[i0], &pa[i2], pt1);
  // d2 = sqrt(fabs(d1));
      // printf(" ii=%d\n",ii);
  if(ii > 0) {
    d1 = UT2D_len_2pt (&pa[i2], pt1);
      // printf(" d1=%f\n",d1);
    if(d1 > (UT_DISP_cv * 3.)) goto L_out;
  }
  if(ii < 0) {
    d1 = UT2D_len_2pt (&pa[i0], pt1);
      // printf(" d1=%f\n",d1);
    if(d1 > (UT_DISP_cv * 3.)) goto L_out;
  }

  // printf("************* pt1 is on Curve !\n"); // i1 is nearest
    // printf(" i0=%d,i1=%d,i2=%d\n",i0,i1,i2);
    // printf(" di0=%lf,di1=%lf,di2=%lf\n",da[i0],da[i1],da[i2]);
    // UT3D_stru_dump (Typ_PT2, &pa[i0], " i0");
    // UT3D_stru_dump (Typ_PT2, &pa[i1], " i1");
    // UT3D_stru_dump (Typ_PT2, &pa[i2], " i2");
    // UT3D_stru_dump (Typ_PT2, pt1, " pt1");
    // UT3D_stru_dump (Typ_VC2, &v1, " v1");
    // UT3D_stru_dump (Typ_VC2, &v02, " v02");
  // length vector
  // should be maxBoxSiz / 4
  d1 = APT_ModSiz / 2.;
  UT2D_pt_traptvclen (ptg, pt1, &v02, d1);
  // return 1;

//----------------------------------------------------------------
  L_out:
    // UT3D_stru_dump(Typ_PT2, ptg, " ptg:");
    // printf("ex UT2D_pt_tng_cbsp_pt irc=0\n");
  return 0;

//----------------------------------------------------------------
  Err_no_sol:
      printf(" err-no-sol -2\n");
    return -2;
}



//================================================================
//================================================================
// Liste_TESTFUNKTIONEN:
//================================================================
//================================================================

/*


//================================================================
  int UT3D_testbsplbez (CurvBSpl *bspl, Memspc *workSeg) {
//================================================================
// UT3D_testbsplbez    B-spline curve segments <--> Bezier curves

// UT3D_testbsplbez    Author: Thomas Backmeister       6.6.2003

// IN:
//   CurvBSpl *bspl  ... b-spline curve
//   Memspc *workSeg ... work space
// OUT:
// Returncodes:
//   0 = OK
//  -1 = out of work space
//  -2 = internal error


  int rc, p;
  long nbseg;
  double va, vb, t, u;
  void *workstart;
  Point pt;
  CurvBez bcv;

  printf("UT3D_testbsplbez\n");

  workstart = workSeg->next;

  nbseg = bspl->ptNr - bspl->deg;
  printf("number of segments= %d\n", nbseg);


  for (p=0; p<nbseg; ++p) {

    // segment start- and endparameter
    va = bspl->kvTab[bspl->deg+p];
    vb = bspl->kvTab[bspl->deg+p+1];
    if (bspl->v0 >= vb) continue;
    if (bspl->v1 <= va) break;
    if (bspl->v0 > va) va = bspl->v0;
    if (bspl->v1 < vb) vb = bspl->v1;

    // Bezier curve from b-spline curve segment
    bcv.ptNr  = 0;
    bcv.va = va;
    bcv.vb = vb;

    bcv.cptab = workSeg->next;
    rc = UME_add (workSeg, sizeof(Point) * (bspl->deg + 1));
    if (rc < 0) goto L_outOfMem;

    rc = UT3D_bez_bsplseg (&bcv, va, vb, 2*bspl->deg, &(bspl->kvTab[p+1]),
                           bspl->deg+1, &(bspl->cpTab[p]));
    if (rc != 0) continue;


    // ---- TEST b-spline segment ----
    printf("\nB-SPLINE segment %d\n", p);
    for (t=0.0; t<=1.0; t+=0.20) {
      u = (1.0-t) * va + t * vb;
      rc = UT3D_pt_evparCrvBSpl (&pt, bspl, u, workSeg);
      if (rc < 0) return -2;
      printf("t= %f  u= %f   pt= %f %f %f\n",t,u,pt.x,pt.y,pt.z);
      //cre_obj (Typ_PT, Typ_PT, 1, (void*)&pt);
    }

    // ---- TEST Bezier curve ----
    printf("\nBEZIER curve:\n");
    for (t=0.0; t<=1.0; t+=0.20) {
      UT3D_bez_eval_Pt (&pt, &bcv, t);
      printf("t= %f                pt= %f %f %f\n",t,pt.x,pt.y,pt.z);
      //cre_obj (Typ_PT, Typ_PT, 1, (void*)&pt);
    }
    printf("\n");

    // release memory
    workSeg->next = workstart;
  }

  return 0;

L_outOfMem:
    TX_Error("UT3D_testbsplbez: out of work space");
    return -1;
}

*/

//=============== EOF =====================

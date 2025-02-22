//**************************************************************************
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
2015-08-03 MemTab_del - delete all records following <ipos>. RF.
2014-01-27 MemTab_add new mode 4. RF.
2009-01-05 aus ut_mem.c extahiert; UME_mTab -> MemTab RF.
2008-03-21 UME_reall_add,UME_realloc,UME_reall_save: spcOff zu. RF.
2003-05-30 Erste Version aus ut_string.c. RF.

-----------------------------------------------------
*/
#ifdef globTag
void MemTab(){}
#endif
/*!
\file  ../ut/ut_memTab.c
\brief fixed-length-records in memory: add,insert,delete, realloc. .. MemTab_ 
\code
=====================================================
List_functions_start:

MemTab_ini          init memory-table, malloc
MemTab_spc          init memory-table with fixed memoryspace; no realloc, free.
MEMTAB_tmpSpc_get   get memspace for rTot records                  INLINE
MEMTAB_tmpSpc_free  free memspace from MEMTAB_tmpSpc_get           INLINE
MemTab_add          save/check/reserve in memSpc; malloc/realloc if necessary.
MemTab_sav          save objDat to memSpc; malloc/realloc if necessary.
MemTab_uniq_sav     if data is uniq only: save data with MemTab_add (.. 1, 0);
MemTab_reserve      reserve space to memSpc; malloc/realloc if necessary.
MemTab_check        check free space to memSpc; malloc/realloc if necessary.
MemTab_ins          insert records in MemTab
MemTab_del          delete records in MemTab
MemTab_mod          modify records 
MemTab_load         returns datablock and its nr of records
MemTab_get          retrieve (copy out) records 
MemTab_clear        reset  (memTab->rNr = 0;)
MemTab_free         free MemTab-structure.
MemTab_wrf          write MemTab to file
MemTab_rdf          read MemTab from file

MEMTAB__            get data-record from index                     INLINE
MEMTAB_DAT          get data-record complete                       INLINE
MEMTAB_IND          get index (pointer to next free = nr of used)  INLINE
MEMTAB_RESET_IND    reset index (nr of used = 0)                   INLINE
MEMTAB_RMAX         get max. nr of records                         INLINE
MEMTAB_RSIZ         get record size                                INLINE
MEMTAB_RFREE        get nr of free records                         INLINE
MEMTAB_POS          get position of next data-record               INLINE
MEMTAB_IND_POS      get record-index from position                 INLINE
MEMTAB_IS_EMPTY     test if MemTab.data == NULL                    INLINE
MemTab_dump

MemTabI_add_s       add n int's to int-table (add consecutive values)

List_functions_end:
=====================================================
- necessary includes:
#include "../ut/ut_memTab.h"              // MemTab

- see also:
../doc/gCAD3D_prog_de.txt section Memoryfunktionen

\endcode *//*----------------------------------------


- OFFLINE tests:
main
dumpTab


- external functionreferences:
UTI_round_i2b
UTI_round_b2i
MEM_del_nrec
MEM_ins_nrec


-----------------------------------------------------
- usage examples:

#include "../ut/ut_memTab.h"          // MemTab_..
#include "../ut/ut_geo.h"             // Point
// typedef struct {double x, y, z;} Point;

typedef_MemTab(Point);

int f1 () {
  MemTab(Point) pTab = _MEMTAB_NUL;
  long          ld;
  Point         pt1, pta[]={{0,0,0},{1,0,0},{1,1,0}};

  // alloc for 100 points
  MemTab_ini (&pTab, sizeof(Point), Typ_PT, 100);
  // add records to pTab
  MemTab_sav (&pTab, &ld, &pta[0], 1);   // add 1 record
  MemTab_sav (&pTab, &ld, &pta[1], 2);   // add 2 records

  printf("recNr = %d\n",pTab.rNr);       // nr of records in pTab (Count)

  // get 1 record starting with record # 2
  ii = 0;
  MemTab_get (&pt1, &pTab, 2, 1);
    // dumpTab (&pTab, 2);
    printf(" pt1=%f,%f,%f\n",pt1.x,pt1.y,pt1.z);

  // get 1 record direct out of tab.data:
  // pt1 = pTab.data[ii];

  // insert 1 record before record # 2
  pt1.x = 3; pt1.y = 4; pt1.z = 5;
  MemTab_ins (&pTab, &ld, &pt1, 2, 1);

  // modify 1 record starting with record # 2
  pt1.x = 5; pt1.y = 4; pt1.z = 3;
  MemTab_mod (&pTab, &pt1, 2, 1);

  // delete 1 record starting with record # 2
  MemTab_del (&pTab, 2, 1);

  // print nr of records stored:
  printf(" nr of records: %d\n",pTab.rNr);

  // reset the data (clear)
  // MemTab_clear (&pTab);         // memTab->rNr = 0; 

  f2 (&pTab);

  // free memory
  MemTab_free (&pTab);

  printf(" sizof = %d\n",sizeof(MemTab));
}




//----------------------------------------------------------------
// how to loop tru MemTabs:
int f2 (MemTab(Point) *mTb) {
  // mTab->data has type Point* !
  int    i1;
  Point  *pTab;
  
  for(i1=0; i1 < mTb->rNr; ++i1) {
    // data is (Point*) !
    printf(" rec[%d]=%f,%f,%f\n",i1,
            mTb->data[i1].x,
            mTb->data[i1].y,
            mTb->data[i1].z);
  }

  pTab = mTb->data;

  return 0;

}



//----------------------------------------------------------------
// get the pointer to record nr. 5 out of MemTab:
typedef_MemTab(IndTab);
  MemTab(IndTab) mt = ..

  IndTab *recX;
  recX = MEMTAB__ (&mt, 5);







\endcode *//*----------------------------------------

Testprog: ../ut/tst_memTab.c


*/


// #include "../xa/MS_Def0.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#include "../ut/AP_types.h"
#include "../ut/ut_memTab.h"
#include "../ut/ut_uti.h"              // UTI_round_32up
#include "../ut/ut_cast.h"             // INT_PTR
#include "../ut/ut_types.h"            // UINT_8_MAX



//==========================================================================
  int MemTab_spc (MemTab *memTab, void *data, int dSiz, int rSiz, int typ) {
//==========================================================================
/// MemTab_spc          init memory-table with fixed memoryspace; no realloc, free
/// \code
/// INPUT:
///   data    memspc to connect
///   dSiz    size of data in bytes
///   rSiz    recordsize in bytes
///   typ     info; Typ_PT=Point Typ_Int4=int Typ_Float8=double Typ_Int1=char;
///
/// Example get MemTab in tempSpace:
///  int lNr=100; int mSizb; Line *lna; MemTab(Line) mln = _MEMTAB_NUL;
///  mSizb = lNr * sizeof(Line); lna = (Line*) MEM_alloc_tmp (mSizb);
///  MemTab_spc (&mln, lna, mSizb, sizeof(Line), Typ_LN);

/// \endcode

  // printf("MemTab_ini rSiz=%d typ=%d incSiz=%d\n",rSiz,typ,incSiz);


  memTab->data   = data;
  memTab->rMax   = dSiz / rSiz;

  memTab->rNr    = 0;
  memTab->rSiz   = rSiz;
  memTab->incSiz = UINT_8_MAX;
  memTab->typ    = typ;
  memTab->spcTyp = 1;     // fixed; CANNOT-reallocate


  // printf("ex MemTab_ini rSiz=%d incSiz=%d\n",memTab->rSiz,memTab->incSiz);

  return 0;

}


//=========================================================================
  int MemTab_ini (void *mtbv, int rSiz, int typ, int incSiz) {
  // int MemTab_ini (MemTab *memTab, int rSiz, int typ, int incSiz) {
//=========================================================================
/// \code
/// MemTab_ini           init memory-table; if already in use: clear.
/// use MemTab for a list of records;
/// use Memspc for a different types of records in the same memoryblock.
/// 
/// INPUT:
///   rSiz    recordsize in bytes
///   typ     info; Typ_PT=Point Typ_Int4=int Typ_Float8=double Typ_Int1=char;
///   incSiz  nr of records to increment memTab;
///           if Memspc is too small: add UTI_round_b2i(incSiz * rSiz)
///
/// Examples:
///   MemTab(int) MIFA = _MEMTAB_NUL;
///   MemTab_ini (&MIFA, sizeof(int), Typ_Int4, 1000);
/// or:
/// static MemTab(int) MIFA = {NULL, 0, 0, sizeof(int), Typ_Int4, 10, 0, 0};
///   // 10: UTI_round_b2i(10) = 1024 records to add when reallocating
/// \endcode

// SEE TODO ON TOP ..

  MemTab *memTab = mtbv;

  // printf("MemTab_ini rSiz=%d typ=%d incSiz=%d\n",rSiz,typ,incSiz);

  if(memTab->data == NULL) {

    memTab->data   = NULL;
    memTab->rMax   = 0;

  // } else {
      // // printf(" MemTab_ini -rMax=%d\n",memTab->rMax);
    // MemTab_clear (memTab);
  }
 
  memTab->rNr    = 0;
  memTab->rSiz   = rSiz;
  memTab->incSiz = UTI_round_i2b(incSiz);
  memTab->typ    = typ;
  memTab->spcTyp = 0;


  // printf("ex MemTab_ini rSiz=%d incSiz=%d\n",memTab->rSiz,memTab->incSiz);

  return 0;

}


//=========================================================================
  int MemTab_add (void *mtbv, long *spcOff,
                  void* objDat, int recNr, int mode) {
//=========================================================================
/// \code
/// MemTab_add          save/check/reserve in memSpc; malloc/realloc if necessary.
/// struct must be initialized with MemTab_ini; free it with MemTab_free.
/// Fixed Recordsizes only !
/// 
/// Input:
///   memTab        where to store data; must have type according to objDat.
///   objDat        data to be saved in memSpc (NULL if mode=1 or 2)
///   recNr         nr of records to be allocated / saved
///   mode          0=save (get space, save data, incr. counter)
///                 1=reserve (get space and add <recNr> to internal counter)
///                 2=check (now MT has at least <recNr> free records)
///                 4=save; but create <recNr> copies of record <objDat>
/// Output:
///   mtbv->rNr   nr of records already in use (including objDat)
///   spcOff        displacement-offset; 0: no reallocate.
/// RetCode 
///   0             OK
///   1             OK after reallocate
///  -1             outOfMemory-Error.
/// 
/// mtbv:
///   mtbv->rMax  nr of records already allocated
///   mtbv->rNr   nr of records already in use
///   mtbv->rSiz  recordsize in bytes
/// 
/// Ablauf:
///   if (recNr > 0)  get space for <recNr> records.
///   else            get space for <recNrFree> records.
///   if (objDat == NULL)  do not save, do malloc/realloc.
///   recNrFree =  UTI_round_b2i(mtbv->incSiz)
///
/// Examples:
/// // typedef_MemTab(Point);
/// MemTab(Point)    pa2D = _MEMTAB_NUL;
/// ips = MEMTAB_IND (&ts1->pa2D);                // get next free position
/// irc = MemTab_add (&pa2D, &ld, NULL, ptNr, 1); // reserve pNr points in pa2D
/// pa = MEMTAB_DAT (&ts1->pa2D);                 // get datablock of points
/// irc = PRCV_get_dbo_1 (&pa[ips]);              // get points & save in pa2D
/// \endcode


  int    i1, irc=0;
  long   newRecNrMax, ii, iAddNr;
  void   *oldPos;
  char   *oPos, *cp1;
  MemTab *memTab = mtbv;



  // printf("MemTab_add max=%d used=%d siz=%d nr=%d\n",
          // memTab->rMax, memTab->rNr, memTab->rSiz, recNr);
  // UT3D_stru_dump (Typ_MemTab, memTab, " _add-in");


  *spcOff = 0;

  if(memTab->data == NULL) {
    oldPos       = NULL;
    memTab->rNr  = 0;             // used records 
    memTab->rMax = 0;             // actual size
  } else {
    oldPos = memTab->data;
  }


  if(recNr > 0) {
    if((memTab->rNr + recNr) < memTab->rMax) goto L_save;
  }


  //----------------------------------------------------------------
  // not enough space ..
  // test if memspc can be reallocated
  if(memTab->incSiz == UINT_8_MAX) {    // 2018-01-15  (was -1)
    // -1 = cannot realloc (fixed space)
    TX_Error("MemTab_sav EOM");
    return -2;
  }

  if(memTab->rSiz < 1) {
    // 0 = uninitialized
    TX_Error("MemTab_sav E001 (uninitialized)");
    return -2;
  }

  if(memTab->incSiz) iAddNr = UTI_round_b2i (memTab->incSiz);
  else               iAddNr = UTI_round_32up (recNr);

  ii = memTab->rNr + recNr;
  newRecNrMax = memTab->rMax;
  while(newRecNrMax < ii) newRecNrMax += iAddNr;
    // printf(" newRecNrMax=%ld iAddNr=%ld ii=%ld\n",newRecNrMax,iAddNr,ii);

  irc = 1;  // realloc !
    // printf("MemTab_add realloc\n");

  // malloc | realloc
  memTab->data = realloc(memTab->data, newRecNrMax * memTab->rSiz);
  if(memTab->data == NULL) {
    TX_Error("UME_alloc EOM");
    return -1;
  }

  memTab->rMax = newRecNrMax;

  // compute displacement-offset
  if(oldPos) *spcOff = (char*)memTab->data - (char*)oldPos;




  //----------------------------------------------------------------
  L_save:
  // if((objDat)&&(recNr > 0)) {
  // mode 0=save, 1=reserve, 2=check
  if(mode == 0) {
    oPos = (char*)memTab->data;
    memcpy(&oPos[memTab->rNr * memTab->rSiz], objDat, recNr * memTab->rSiz);

  } else if(mode == 4) {
    // create <recNr> copies of record <objDat>      // 2014-01-27
      // printf(" m4: rNr=%d rSiz=%d\n",memTab->rNr,memTab->rSiz);
    oPos = (char*)memTab->data;
    cp1 = &oPos[memTab->rNr * memTab->rSiz];
    for(i1=0; i1<recNr; ++i1) {
      memcpy(cp1, objDat, memTab->rSiz);
      cp1 += memTab->rSiz;
    }
  }

  if(mode != 2) {
    // for 0|1|4
    memTab->rNr += recNr;
  }

  //----------------------------------------------------------------
  L_exit:

    // printf("ex UME_alloc_add max=%d used=%d\n",memTab->rMax,memTab->rNr);
    // UT3D_stru_dump (Typ_MemTab, memTab, "ex-_add");

  return irc;

}


//=========================================================================
  int MemTab_sav (MemTab *memTab, long *spcOff, void* objDat, int recNr) {
//=========================================================================
// MemTab_sav          save objDat to memSpc; malloc/realloc if necessary.

  return MemTab_add (memTab, spcOff, objDat, recNr, 0);

}


//====================================================================
    int MemTab_uniq_sav (MemTab *memTab, long *spcOff, void* objDat) {
//====================================================================
/// \code
/// MemTab_uniq_add     if data is uniq only: save data with MemTab_add (.. 1, 0);
/// API see MemTab_add
/// RetCode
///   2        objDat already exists
/// \endcode


  int     i1, ie;
  long    lsiz;
  char    *mdat;


  printf("MemTab_uniq_sav %d %d\n",memTab->rNr,memTab->rSiz);


  ie = memTab->rNr;      // nr of records
  lsiz = memTab->rSiz;   // recordSize
  mdat =memTab->data;    // start of data

  for(i1=0; i1<ie; ++i1) {
    if(! memcmp(mdat, objDat, lsiz)) return 2;  // objDat already exists
    mdat += lsiz;
  }


  return MemTab_add (memTab, spcOff, objDat, 1, 0);

}


//================================================================
  int MemTab_reserve (MemTab *memTab, long *spcOff, int recNr) {
//================================================================
// MemTab_reserve      reserve space to memSpc; malloc/realloc if necessary.
// updates data-pointer;
// get first free position before with MEMTAB_IND

  return MemTab_add (memTab, spcOff, NULL, recNr, 1);

}

//================================================================
  int MemTab_check (MemTab *memTab, long *spcOff, int recNr) {
//================================================================
// MemTab_check        check free space to memSpc; malloc/realloc if necessary.
// does not update data-pointer


  // printf("MemTab_check recNr=%d\n",recNr);

  return MemTab_add (memTab, spcOff, NULL, recNr, 2);

}


//================================================================
  int  MemTab_load (void **oTab, int *iNr, MemTab *mt) {
//================================================================
/// \code
/// MemTab_load            returns datablock and its nr of records
/// use:
///   MemTab_load ((void**)&oTab, &iNr, (MemTab*)&delTab);
/// see also MemTab_get
/// \endcode


  *iNr  = mt->rNr;
  *oTab = mt->data;

  return 0;

}


//================================================================
  int MemTab_get (void* objDat, MemTab *mt, int ipos, int recNr) {
//================================================================
/// \code
/// MemTab_get          retrieve (copy out) records 
///   get <recNr> records starting with record <ipos>
/// 
/// Input:
///   ipos       1. record to replace in mt->data
///   recNr      nr of records to overwrite 
/// Output:
///   objDat     data out of memSpc
/// RetCodes:
///    -1        not yet allocated (ipos+recNr > mt->rNr)
/// \endcode

  char   *oPos;

  if(mt->rNr < (ipos + recNr)) return -1;

  oPos = (char*)mt->data;

  memcpy(objDat, &oPos[ipos * mt->rSiz], recNr * mt->rSiz);

  return 0;

}


//================================================================
  int MemTab_mod (MemTab *mt, void* objDat, int ipos, int recNr) {
//================================================================
/// \code
/// MemTab_mod               modify records 
/// modify <recNr> records starting with record <ipos>
/// 
/// Input:
///   objDat     data to be saved in memSpc
///   ipos       1. record to replace in mt->data
///   recNr      nr of records to overwrite 
/// RetCodes:
///    -1        not yet allocated (ipos+recNr > mt->rNr)
/// \endcode

  char   *oPos;

  if(mt->rNr < (ipos + recNr)) return -1;

  oPos = (char*)mt->data;

  memcpy(&oPos[ipos * mt->rSiz], objDat, recNr * mt->rSiz);

  return 0;

}


//================================================================
  int MemTab_del (MemTab *mt, int ipos, int delRecNr) {
//================================================================
/// \code
/// MemTab_del               delete records in MemTab
///   delete <delRecNr> records starting with record <ipos>
/// Input:
///   ipos       1. record to delete in mt->data
///   delRecNr   nr of records to delete 
///              -1 delete all records following <ipos> and including <ipos>
/// \endcode


  if(delRecNr < 0) {
    // delete all records including <ipos>
    mt->rNr = ipos;
    return 0;
  }

  return MEM_del_nrec (&mt->rNr, mt->data, ipos, delRecNr, mt->rSiz);

}


//================================================================
  int MemTab_ins (MemTab *mt, long *spcOff,
                    void* objDat, int ipos, int insRecNr) {
//================================================================
/// \code
/// MemTab_ins               insert records in MemTab
///   insert <insRecNr> records before record <ipos>
/// \endcode

  int      irc;


  // check space
  irc = MemTab_add (mt, spcOff, NULL, insRecNr, 2);
  if(irc < 0) return irc;

  // insert
  MEM_ins_nrec (&mt->rNr, mt->data, ipos, objDat, insRecNr, mt->rSiz);


  return 0;

}


//================================================================
  int MemTab_clear (MemTab *memTab) {
//================================================================
/// MemTab_clear        reset  (memTab->rNr = 0;)

  if(memTab->data) {
    memTab->rNr = 0;
  }

  return 0;

}


//================================================================
  int MemTab_free (MemTab *memTab) {
//================================================================
/// MemTab_free        free MemTab-structure.

  if(memTab->data) {
    free (memTab->data);
    memTab->data = NULL;
    memTab->rMax = 0;             // actual size
  }
  memTab->rNr = 0;

  return 0;

}


//================================================================
  int MemTab_wrf (FILE *fp1, MemTab *mt) {
//================================================================
// write MemTab to file


  fwrite(mt, sizeof(MemTab), 1, fp1);

  if(mt->rNr > 0)
  fwrite(mt->data, mt->rSiz, mt->rNr, fp1);


  return 0;
}


//================================================================
  int MemTab_rdf (FILE *fp1, MemTab *mt1) {
//================================================================
// read MemTab from file
// mt1 must already have rSiz

  int     ii, rNr;
  long    l1;
  MemTab  mt2;


  // printf("MemTab_rdf rNr=%d rMax=%d rSiz=%d\n",
             // mt1->rNr,mt1->rMax,mt1->rSiz);

  // copy mt1
  mt2 = *mt1;
     // UT3D_stru_dump (Typ_MemTab, &mt2, " -mt-act");


  // overwrite mt1
  fread(mt1, sizeof(MemTab), 1, fp1);
     // printf("    -mt-in rNr=%d rMax=%d rSiz=%d\n",
             // mt1->rNr, mt1->rMax, mt1->rSiz);


  // keep old dataPos & size
  mt1->data = mt2.data;
  mt1->rMax = mt2.rMax;

  // add space if necessary
  if(mt1->rNr > mt2.rMax) {
    ii = mt1->rNr;
    rNr = ii - mt2.rMax + 4;
    MemTab_add (mt1, &l1, NULL, rNr, 2);    // can overwrite rNr
      // printf(" _rdf-add %d records\n",rNr);
      // UT3D_stru_dump (Typ_MemTab, mt1, " -mt-add");
    mt1->rNr = ii;
  }

  // overwrite data
  if(mt1->rNr > 0) {
    fread(mt1->data, mt1->rSiz, mt1->rNr, fp1);
  }

  return 0;
}


//================================================================
  int MemTab_dump (MemTab *memTab, char *info) {
//================================================================

  int   i1, ityp, isiz, *ia;
  char  *data;
  void  *vp;


  printf("============ MemTab_dump %s =============\n", info);

  if(!memTab)          { printf(".. undefined ..\n");     return -1;}
  if(memTab->rMax < 1) { printf(".. uninitialized ..\n"); return -1;}

  

  UT3D_stru_dump (Typ_MemTab, memTab, "MemTab_dump");

  if(memTab->rNr < 1)  { printf(".. empty ..\n"); return -1;}

  ityp = memTab->typ;
  isiz = UTO_siz_stru (memTab->typ);
    printf(" ityp=%d isiz=%d rNr=%d\n",ityp,isiz,memTab->rNr);



  //----------------------------------------------------------------
  // Typ_Int1
  if(ityp != Typ_Int1) goto L_I4;

  data = MEMTAB_DAT (memTab);
  for(i1=0; i1<memTab->rNr; ++i1) {
    vp = PTR_INT((int)data[i1]);
    UT3D_stru_dump (ityp, vp, "[%d]",i1);
  }
  return 0;




  //----------------------------------------------------------------
  L_I4:
  // Typ_Int4
  if(ityp != Typ_Int4) goto L_1;

  ia = MEMTAB_DAT (memTab);
  for(i1=0; i1<memTab->rNr; ++i1) {
    vp = PTR_INT(ia[i1]);
      // printf(" i4[%d]=%ld\n",i1,(long)vp);
    UT3D_stru_dump (ityp, vp, "[%d]",i1);
  }
  return 0;



  //----------------------------------------------------------------
  L_1:
  // Typ_PT ..
  data = (char*)memTab->data;

  for(i1=0; i1<memTab->rNr; ++i1) {
      // printf(" data=%d\n",*((int*)data));
    UT3D_stru_dump (ityp, data, "[%d]",i1);
    data += isiz;
  }

  printf("\n");


  return 0;

}


//======================== EOF ======================================*/

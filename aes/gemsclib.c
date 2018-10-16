/*      GEMSCLIB.C      07/10/84 - 02/02/85     Lee Lorenzen            */
/*      for 2.0         10/8/85  - 10/15/85     MDF                     */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include "portab.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"
#include "dos.h"
#include "gem_rsc.h"

#include "gemrslib.h"
#include "gemdos.h"
#include "geminit.h"
#include "gemshlib.h"
#include "gemsclib.h"

#include "string.h"

/************************************************************************/
/*                                                                      */
/* sc_read() -- get info about current scrap directory                  */
/*                                                                      */
/*      copies the current scrap directory path to the passed-in        */
/*      address and returns TRUE if a valid path has already been set.  */
/*                                                                      */
/************************************************************************/

WORD sc_read(BYTE *pscrap)
{
    WORD    len;

    /* current scrap directory */
    len = strlencpy(pscrap, D.g_scrap);
    strcpy(pscrap+len, "\\");      /* cat on backslash  */
    return( len != 0 );
}


/************************************************************************/
/*                                                                      */
/* sc_write() -- sets the current scrap directory                       */
/*                                                                      */
/*      pscrap must be the long address of a valid path.  Returns       */
/*      TRUE if no error occurs in validating the path name.            */
/*                                                                      */
/************************************************************************/

WORD sc_write(const BYTE *pscrap)
{
    WORD    len;

    len = strlencpy(D.g_scrap, pscrap);     /* new scrap directory  */
    if (D.g_scrap[--len] == '\\')           /* remove backslash     */
      D.g_scrap[len] = '\0';
    dos_sdta(&D.g_dta);                     /* use our dta          */
    return !dos_sfirst(D.g_scrap, F_SUBDIR);/* make sure path ok    */
}

#if CONF_WITH_PCGEM
/************************************************************************/
/*                                                                      */
/* sc_clear() -- delete scrap files from current scrap directory        */
/*                                                                      */
/*      Assumes D.g_scrap holds a valid directory path.                 */
/*      Returns TRUE on success.                                        */
/*                                                                      */
/************************************************************************/

WORD sc_clear(void)
{
    BYTE    *ptmp;
    WORD    ret;
    const char *scrapmask = "\\SCRAP.*";

    if (D.g_scrap[0] == '\0')
      return FALSE;

    ptmp = D.g_scrap;
    while(*ptmp)                            /* find null */
      ptmp++;

    strcpy(ptmp, scrapmask);                /* Add mask */

    dos_sdta(&D.g_dta);                     /* make sure dta ok */

    ret = dos_sfirst(D.g_scrap, F_SUBDIR);
    while(ret == 0)
    {
        strcpy(ptmp + 1, D.g_dta.d_fname);  /* Add file name */
        dos_delete(D.g_scrap);              /* delete scrap.* */
        strcpy(ptmp, scrapmask);            /* Add mask */
        ret = dos_snext();
    }

    *ptmp = '\0';                           /* keep just path name */

    return(TRUE);
}
#endif

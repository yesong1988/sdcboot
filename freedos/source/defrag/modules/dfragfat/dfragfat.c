/*    

   Dfragfat.c - Main code for the defragmention routines.

   Copyright (C) 2002 Imre Leber

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   If you have any questions, comments, suggestions, or fixes please
   email me at:  imre.leber@worldonline.be
*/

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "fte.h"
#include "misc.h"
#include "expected.h"
#include "defrpars.h"
#include "FllDfrag\flldfrg.h"
#include "..\dtstruct\vlhandle.h"
#include "ordering\ordrdfrg.h"
#include "crunch\crunch.h"
#include "unfrgfls\unfrag.h"

/*
** For constants to use as parameters look in ..\..\modlgate\defrpars.h
*/

int DefragFAT(int method)
{
    int retVal;
    RDWRHandle handle;

    /* Get handle to volume */
    handle = GetCurrentVolumeHandle();
    if (!handle) RETURN_FTEERR(FALSE);    
    
    /* Mention what comes next */
    LargeMessage("Defragmenting volume . . .");
    SmallMessage(" Defragmenting volume . . .");

    
    /* Notice that we assume right input from the interface */
    switch (method)
    {
       case FULL_OPTIMIZATION:
            retVal = FullyDefragmentVolume(handle);
            break;

       case UNFRAGMENT_FILES:
            retVal = UnfragmentFiles(handle);
            break;

       case FILES_FIRST:
            retVal = FilesFirstDefrag(handle);
            break;

       case DIRECTORIES_FIRST:
            retVal = DirectoriesFirstDefrag(handle);
            break;

       case DIRECTORIES_FILES:
            retVal = DirectoriesWithFilesDefrag(handle);
            break;

       case CRUNCH_ONLY:
            retVal = CrunchVolume(handle);
            break;
       
       case QUICK_TRY:
            retVal = QuickTryDefragmentVolume(handle);
            break;
    
       case COMPLETE_QUICK_TRY:
           retVal = CompleteQuickTryDefragmentVolume(handle);
           break;

       default:
            assert(FALSE);
    }

    BackupFat(handle);
    CommitCache();

    /* Release allocated memory */
    DestroyDirReferedTable();
    DestroyFatReferedMap();
    DestroyFatReferedTable();

    if (retVal) LogMessage("Volume successfully defragmented.\n");
    return retVal;
}

//#define DEBUG
#ifdef DEBUG

RDWRHandle theHandle;
/*static*/ int MarkUnmovables32(RDWRHandle handle);
int main(int argc, char *argv[])
{
    unsigned long l;

/*
    if (argc != 3)
    {
       printf("DfragFAT <drive or image file> <method>\n"
              "\n"
         "method: FULL or UNFRAG\n");
       return 1;
    }
*/

#ifdef _WIN32
   InitSectorCache();
    StartSectorCache();
#endif

    if (!InitReadWriteSectors(argv[1], &theHandle))
       RETURN_FTEERR(FALSE);


    if (!CreateFixedClusterMap(theHandle))
      RETURN_FTEERR(FALSE);

    printf("%d", DefragFAT(QUICK_TRY));

#ifdef _WIN32
    CloseSectorCache();
#endif

/*    
    if (stricmp(argv[2], "FULL") == 0)
       DefragFAT(FULL_OPTIMIZATION);
    else if (stricmp(argv[2], "UNFRAG") == 0)
       DefragFAT(UNFRAGMENT_FILES);
*/
    return 0;
}

void UpdateInterfaceState(void){}
void SmallMessage(char* buffer){printf("%s\n", buffer);}
void LargeMessage(char* buffer){printf("%s\n", buffer);}
void DrawOnDriveMap(CLUSTER cluster, int symbol){cluster=cluster;symbol=symbol;}
void DrawDriveMap(CLUSTER maxcluster){maxcluster=maxcluster;}
void DrawMoreOnDriveMap(CLUSTER cluster, int symbol, unsigned long n){cluster=cluster;symbol=symbol;}
void LogMessage(char* message){message=message;}

RDWRHandle GetCurrentVolumeHandle(void)
{
   return theHandle;
}

void IndicatePercentageDone(CLUSTER cluster, CLUSTER totalclusters){}
unsigned long GetRememberedFileCount() {return 100;}
int  QuerySaveState(void){return 0;}

#ifndef _WIN32
int CommitCache(void){}
#endif

#endif

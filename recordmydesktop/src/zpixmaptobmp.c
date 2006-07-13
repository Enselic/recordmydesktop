/*********************************************************************************
*                             recordMyDesktop                                    *
**********************************************************************************
*                                                                                *
*             Copyright (C) 2006  John Varouhakis                                *
*                                                                                *
*                                                                                *
*    This program is free software; you can redistribute it and/or modify        *
*    it under the terms of the GNU General Public License as published by        *
*    the Free Software Foundation; either version 2 of the License, or           *
*    (at your option) any later version.                                         *
*                                                                                *
*    This program is distributed in the hope that it will be useful,             *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*    GNU General Public License for more details.                                *
*                                                                                *
*    You should have received a copy of the GNU General Public License           *
*    along with this program; if not, write to the Free Software                 *
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
*                                                                                *
*                                                                                *
*                                                                                *
*    For further information contact me at biocrasher@gmail.com                  *
**********************************************************************************/


#include <recordmydesktop.h>

int ZPixmapToBMP(XImage *imgz,BRWindow *brwin,char *fname,int nbytes,int scale){
        FILE *fpbmp;
        int i,k;
        int siz=52+nbytes/(pow(scale,2));
        int offs=54+1024;
        short int rsrvd=0;
        int hsiz=40;
        int width=brwin->rgeom.width/scale,height=brwin->rgeom.height/scale,nbts=nbytes/(pow(scale,2));
        unsigned short int planes=1;
        unsigned short int bpp=24;
        unsigned int cmpr=0;
        unsigned int ncols=0;
        char *dtap=imgz->data;

        /*Write header*/
        fpbmp=fopen(fname,"wb");
        fputc('B',fpbmp);
        fputc('M',fpbmp);
        fwrite(&siz,4,1,fpbmp);
        fwrite(&rsrvd,2,1,fpbmp);
        fwrite(&rsrvd,2,1,fpbmp);
        fwrite(&offs,4,1,fpbmp);
        fwrite(&hsiz,4,1,fpbmp);
        fwrite(&(width),4,1,fpbmp);
        fwrite(&(height),4,1,fpbmp);
        fwrite(&planes,2,1,fpbmp);
        fwrite(&bpp,2,1,fpbmp);
        fwrite(&cmpr,4,1,fpbmp);
        fwrite(&nbts,4,1,fpbmp);
        fwrite(&(width),4,1,fpbmp);
        fwrite(&(height),4,1,fpbmp);
        fwrite(&(ncols),4,1,fpbmp);
        fwrite(&(ncols),4,1,fpbmp);
        for(i=0;i<1024;i++)
            fputc(0,fpbmp);
        /*Data*/
        for(k=(nbytes/imgz->bytes_per_line)-1;k>=0;k-=scale){
            for(i=0;i<imgz->bytes_per_line/4;i+=scale){
                fwrite(&dtap[(i*4)+k*(imgz->bytes_per_line)],1,1,fpbmp);
                fwrite(&dtap[(i*4)+k*(imgz->bytes_per_line)+1],1,1,fpbmp);
                fwrite(&dtap[(i*4)+k*(imgz->bytes_per_line)+2],1,1,fpbmp);
            }
        }
 
        fclose(fpbmp);
        return 0;
}

 

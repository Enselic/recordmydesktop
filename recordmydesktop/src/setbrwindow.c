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
*    For further information contact me at johnvarouhakis@gmail.com              *
**********************************************************************************/


#include <recordmydesktop.h>
void SizePack2_8_16(int *start,int *size,int limit){
    int octoffset,hexoffset;

    //align in two
    //an odd x can always go down and still be in recording area.
    //Resolutions come in even numbers
    //so if x is an odd numer, width max is an odd number, too
    //thus since x will go down one then width can go up one too and still
    //be inbounds
    (*size)+=((*size)%2)|((*start)%2);
    //but if x goes down 1 and width is already even,it becomes odd so:
    (*size)+=((*size)%2);
    (*start)-=(*start)%2;


    //32 bit pack align
    //we already have disible by two width,so
    //it's 2, 4 or 6
    octoffset=((*size)%8);
    if(octoffset==2){
        (*size)-=2;

    }
    else if(octoffset==6){
        if((*size)+(*start)+2<=limit)
            (*size)+=2;
        else if((*start)>=2){
            (*start)-=2;
            (*size)+=2;
        }
        else{
            (*start)+=2;
            (*size)-=4;
        }
    }

    else if(octoffset==4){
        if(((*size)+(*start)+2<=limit)&&((*start)>=2)){
            (*start)-=2;
            (*size)+=4;
        }
        else if((*size)+(*start)+4<=limit){
            (*size)+=4;
        }
        else if((*start)>=4){
            (*start)-=4;
            (*size)+=4;
        }
        else{
            (*start)+=2;
            (*size)-=4;
        }
    }

    //16 divisble width(needed for shared memory only,but applied anyway since theora wants it, too)
    //we already have divisibility by 8 so module
    //by 16 is euther 8 or 0
    hexoffset=((*size)%16);
    if(hexoffset){
        if(((*size)+(*start)+4<=limit)&&((*start)>=4)){
            (*start)-=4;
            (*size)+=8;
        }
        else if((*size)+(*start)+8<=limit){
            (*size)+=8;
        }
        else if((*start)>=8){
            (*start)-=8;
            (*size)+=8;
        }
        else{
            (*start)+=4;
            (*size)-=8;
        }
    }

}



int SetBRWindow(Display *dpy,BRWindow *brwin,DisplaySpecs *specs,ProgArgs *args){
    //before we start recording we have to make sure the ranges are valid
    if(args->windowid==0){//root window
        //first set it up
        brwin->windowid=specs->root;
        brwin->geom.x=brwin->geom.y=0;
        brwin->geom.width=specs->width;
        brwin->geom.height=specs->height;
        brwin->rgeom.x=args->x;
        brwin->rgeom.y=args->y;
        brwin->rgeom.width=((args->width)?args->width:specs->width-brwin->rgeom.x);
        brwin->rgeom.height=((args->height)?args->height:specs->height-brwin->rgeom.y);
        //and then check validity
        if((brwin->rgeom.x+brwin->rgeom.width>specs->width)||
            (brwin->rgeom.y+brwin->rgeom.height>specs->height)){
            fprintf(stderr,"Window size specification out of bounds!(current resolution:%dx%d)\n",specs->width,specs->height);
            return 1;
        }
    }
    else{
        Window wchid;
        int transl_x,transl_y;

        XWindowAttributes attribs;
        XGetWindowAttributes(dpy,args->windowid,&attribs);
        if((attribs.map_state==IsUnviewable)||(attribs.map_state==IsUnmapped)){
            fprintf(stderr,"Window must be mapped and visible!\n");
            return 1;
        }
        XTranslateCoordinates(dpy,specs->root,args->windowid,attribs.x,attribs.y,&transl_x,&transl_y,&wchid);
        brwin->windowid=specs->root;
        brwin->geom.x=attribs.x-transl_x;
        brwin->geom.y=attribs.y-transl_y;
        brwin->geom.width=attribs.width;
        brwin->geom.height=attribs.height;
        if((brwin->geom.x+brwin->geom.width>specs->width)||
            (brwin->geom.y+brwin->geom.height>specs->height)){
            fprintf(stderr,"Window must be on visible screen area!\n");
            return 1;
        }

        brwin->rgeom.x=brwin->geom.x+args->x;
        brwin->rgeom.y=brwin->geom.y+args->y;
        brwin->rgeom.width=((args->width)?args->width:brwin->geom.width-args->x);
        brwin->rgeom.height=((args->height)?args->height:brwin->geom.height-args->y);
        if((args->x+brwin->rgeom.width>brwin->geom.width)||
            (args->y+brwin->rgeom.height>brwin->geom.height)){
            fprintf(stderr,"Specified Area is larger than window!\n");
            return 1;
        }
    }
    fprintf(stderr, "Initial recording window is set to:\n"
                    "X:%d   Y:%d    Width:%d    Height:%d\n"
                    ,brwin->rgeom.x,brwin->rgeom.y,brwin->rgeom.width,brwin->rgeom.height);
    SizePack2_8_16(&brwin->rgeom.x,&brwin->rgeom.width,specs->width);
    SizePack2_8_16(&brwin->rgeom.y,&brwin->rgeom.height,specs->height);

    fprintf(stderr, "Adjusted recording window is set to:\n"
                    "X:%d   Y:%d    Width:%d    Height:%d\n"
                    ,brwin->rgeom.x,brwin->rgeom.y,brwin->rgeom.width,brwin->rgeom.height);



    brwin->nbytes=(((brwin->rgeom.width+15)>>4)<<4)*(((brwin->rgeom.height+15)>>4)<<4)*
                  ((specs->depth==16)?2:4);


    return 0;
}

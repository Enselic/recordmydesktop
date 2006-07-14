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
//         brwin->nbytes=(brwin->rgeom.width*brwin->rgeom.height*4);
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
    //align in two
    //an odd x can always go down and still be in recording area.
    //Resolutions come in even numbers
    //so if x is an odd numer, width max is an odd number, too
    //thus since x will go down one then width can go up one too and still
    //be inbounds
    brwin->rgeom.width+=(brwin->rgeom.width%2)|(brwin->rgeom.x%2);
    brwin->rgeom.height+=(brwin->rgeom.height%2)|(brwin->rgeom.y%2);
    //but if x goes down 1 and width is already even,it becomes odd so:
    brwin->rgeom.width+=(brwin->rgeom.width%2);
    brwin->rgeom.height+=(brwin->rgeom.height%2);
    brwin->rgeom.x-=brwin->rgeom.x%2;
    brwin->rgeom.y-=brwin->rgeom.y%2;

    brwin->nbytes=(((brwin->rgeom.width+15)>>4)<<4)*(((brwin->rgeom.height+15)>>4)<<4)*4;

    return 0;
}

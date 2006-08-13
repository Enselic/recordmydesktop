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

int QueryExtensions(Display *dpy,ProgArgs *args,int *damage_event,int *damage_error){
    if(!XDamageQueryExtension( dpy, damage_event, damage_error)){
        fprintf(stderr,"XDamage extension not found!!!\n");
        return 1;
    }
    if((!args->noshared)&&(XShmQueryExtension(dpy)==False)){
        args->noshared=1;
        fprintf(stderr,"Shared Memory extension not present!\nContinuing without it.\n");
    }
    return 0;
}

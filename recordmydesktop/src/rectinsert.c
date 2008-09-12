/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007,2008 John Varouhakis                     *
*                                                                             *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
*                                                                             *
*                                                                             *
*                                                                             *
*   For further information contact me at johnvarouhakis@gmail.com            *
******************************************************************************/


#include "recordmydesktop.h"
#include "rectinsert.h"


/**
* Collide two rectangles and dictate most sane action for insertion,
* as well as provide the updated rectangle(s)
* \param wgeom1 resident rectangle
*
* \param wgeom2 New rectangle
*
* \param wgeom_return Pointer to rectangles to be inserted
*
* \param ngeoms number of entries in wgeom_return
*
* \retval 0 No collision
*
* \retval 1 wgeom1 is covered by wgeom2
*
* \retval 2 wgeom2 is covered by wgeom1
*
* \retval -1 wgeom1 was broken (new is picked up in wgeom_return)
*
* \retval -2 wgeom2 was broken (new is picked up in wgeom_return)
*
* \retval -10 Grouping the two geoms is possible
*
*/
static int CollideRects(WGeometry *wgeom1,
                        WGeometry *wgeom2,
                        WGeometry **wgeom_return,
                        int *ngeoms) {
    //1 fits in 2
    if((wgeom1->x>=wgeom2->x)&&
        (wgeom1->x+wgeom1->width<=wgeom2->x+wgeom2->width)&&
        (wgeom1->y>=wgeom2->y)&&
        (wgeom1->y+wgeom1->height<=wgeom2->y+wgeom2->height)){
        *ngeoms=0;
        return 1;
    }
    //2 fits in 1
    else if((wgeom2->x>=wgeom1->x)&&
        (wgeom2->x+wgeom2->width<=wgeom1->x+wgeom1->width)&&
        (wgeom2->y>=wgeom1->y)&&
        (wgeom2->y+wgeom2->height<=wgeom1->y+wgeom1->height)){
        *ngeoms=0;
        return 2;
    }
    //no collision
    else if((wgeom1->x+wgeom1->width<wgeom2->x)||
        (wgeom2->x+wgeom2->width<wgeom1->x)||
        (wgeom1->y+wgeom1->height<wgeom2->y)||
        (wgeom2->y+wgeom2->height<wgeom1->y)){
        *ngeoms=0;
        return 0;
    }
    else{
//overlapping points are considered enclosed
//this happens because libxdamage may generate many events for one change
//and some of them may be in the the exact same region
//so identical rects would be considered not colliding
//in order though to avoid endless recursion on the RectInsert
//function should always start at the next element(which is logical since
//if any rect makes it to a points none of it's part collides with previous
//nodes on the list, too)
        int x1[2]={wgeom1->x,wgeom1->x+wgeom1->width};
        int y1[2]={wgeom1->y,wgeom1->y+wgeom1->height};
        int x2[2]={wgeom2->x,wgeom2->x+wgeom2->width};
        int y2[2]={wgeom2->y,wgeom2->y+wgeom2->height};
        int enclosed[2][4],tot1,tot2;
        enclosed[0][0]=(((x1[0]>=x2[0])&&(x1[0]<=x2[1])&&
                    (y1[0]>=y2[0])&&(y1[0]<=y2[1]))?1:0);
        enclosed[0][1]=(((x1[1]>=x2[0])&&(x1[1]<=x2[1])&&
                    (y1[0]>=y2[0])&&(y1[0]<=y2[1]))?1:0);
        enclosed[0][2]=(((x1[0]>=x2[0])&&(x1[0]<=x2[1])&&
                    (y1[1]>=y2[0])&&(y1[1]<=y2[1]))?1:0);
        enclosed[0][3]=(((x1[1]>=x2[0])&&(x1[1]<=x2[1])&&
                    (y1[1]>=y2[0])&&(y1[1]<=y2[1]))?1:0);
        enclosed[1][0]=(((x2[0]>=x1[0])&&(x2[0]<=x1[1])&&
                    (y2[0]>=y1[0])&&(y2[0]<=y1[1]))?1:0);
        enclosed[1][1]=(((x2[1]>=x1[0])&&(x2[1]<=x1[1])&&
                    (y2[0]>=y1[0])&&(y2[0]<=y1[1]))?1:0);
        enclosed[1][2]=(((x2[0]>=x1[0])&&(x2[0]<=x1[1])&&
                    (y2[1]>=y1[0])&&(y2[1]<=y1[1]))?1:0);
        enclosed[1][3]=(((x2[1]>=x1[0])&&(x2[1]<=x1[1])&&
                    (y2[1]>=y1[0])&&(y2[1]<=y1[1]))?1:0);
        tot1=enclosed[0][0]+enclosed[0][1]+enclosed[0][2]+enclosed[0][3];
        tot2=enclosed[1][0]+enclosed[1][1]+enclosed[1][2]+enclosed[1][3];
        if((tot1==2)&&(tot2==2)){//same width or height, which is the best case
            //group
            if((enclosed[1][0]&&enclosed[1][1])&&
               (wgeom1->width==wgeom2->width)){
                wgeom_return[0]=(WGeometry *)malloc(sizeof(WGeometry));
                *ngeoms=1;
                wgeom_return[0]->x=wgeom1->x;
                wgeom_return[0]->y=wgeom1->y;
                wgeom_return[0]->width=wgeom1->width;
                wgeom_return[0]->height=wgeom2->height+wgeom2->y-wgeom1->y;
                return -10;
            }
            else if((enclosed[1][0]&&enclosed[1][2])&&
                    (wgeom1->height==wgeom2->height)){
                *ngeoms=1;
                wgeom_return[0]->x=wgeom1->x;
                wgeom_return[0]->y=wgeom1->y;
                wgeom_return[0]->width=wgeom2->width+wgeom2->x-wgeom1->x;
                wgeom_return[0]->height=wgeom1->height;
                return -10;
            }
            else if((enclosed[1][3]&&enclosed[1][1])&&
                    (wgeom1->height==wgeom2->height)){
                *ngeoms=1;
                wgeom_return[0]->x=wgeom2->x;
                wgeom_return[0]->y=wgeom2->y;
                wgeom_return[0]->width=wgeom1->width+wgeom1->x-wgeom2->x;
                wgeom_return[0]->height=wgeom2->height;
                return -10;
            }
            else if((enclosed[1][3]&&enclosed[1][2])&&
                    (wgeom1->width==wgeom2->width)){
                *ngeoms=1;
                wgeom_return[0]->x=wgeom2->x;
                wgeom_return[0]->y=wgeom2->y;
                wgeom_return[0]->width=wgeom2->width;
                wgeom_return[0]->height=wgeom1->height+wgeom1->y-wgeom2->y;
                return -10;
            }
            //if control reaches here therewasn't a group and we go on
        }
        if(tot2==2){
            //break geom2
            wgeom_return[0]->x=wgeom2->x;
            wgeom_return[0]->y=wgeom2->y;
            wgeom_return[0]->width=wgeom2->width;
            wgeom_return[0]->height=wgeom2->height;
            *ngeoms=1;
            if(enclosed[1][0]&&enclosed[1][1]){
                wgeom_return[0]->y=y1[1];
                wgeom_return[0]->height-=y1[1]-y2[0];
            }
            else if(enclosed[1][0]&&enclosed[1][2]){
                wgeom_return[0]->x=x1[1];
                wgeom_return[0]->width-=x1[1]-x2[0];
            }
            else if(enclosed[1][3]&&enclosed[1][1])
                wgeom_return[0]->width-=x2[1]-x1[0];
            else if(enclosed[1][3]&&enclosed[1][2])
                wgeom_return[0]->height-=y2[1]-y1[0];
            return -2;
        }
        else if(tot1==2){
            //if the first one breaks(which is already inserted)
            //then we reenter the part that was left and the one
            //that was to be inserted
            wgeom_return[1]->x=wgeom2->x;
            wgeom_return[1]->y=wgeom2->y;
            wgeom_return[1]->width=wgeom2->width;
            wgeom_return[1]->height=wgeom2->height;
            wgeom_return[0]->x=wgeom1->x;
            wgeom_return[0]->y=wgeom1->y;
            wgeom_return[0]->width=wgeom1->width;
            wgeom_return[0]->height=wgeom1->height;
            *ngeoms=1;
            if(enclosed[0][0]&&enclosed[0][1]){
                wgeom_return[0]->y=y2[1];
                wgeom_return[0]->height-=y2[1]-y1[0];
            }
            else if(enclosed[0][0]&&enclosed[0][2]){
                wgeom_return[0]->x=x2[1];
                wgeom_return[0]->width-=x2[1]-x1[0];
            }
            else if(enclosed[0][3]&&enclosed[0][1])
                wgeom_return[0]->width-=x1[1]-x2[0];
            else if(enclosed[0][3]&&enclosed[0][2])
                wgeom_return[0]->height-=y1[1]-y2[0];
            return -1;

        }
        else if(tot2==1){   //in which case there is also tot1==1
                            //but we rather not break that
                            //break geom2 in two
            *ngeoms=2;
            if(enclosed[1][0]){
//first
                wgeom_return[0]->x=x1[1];
                wgeom_return[0]->y=y2[0];
                wgeom_return[0]->width=wgeom2->width-x1[1]+x2[0];
                wgeom_return[0]->height=wgeom2->height;
//second
                wgeom_return[1]->x=x2[0];
                wgeom_return[1]->y=y1[1];
                wgeom_return[1]->width=x1[1]-x2[0];
                wgeom_return[1]->height=wgeom2->height-y1[1]+y2[0];
            }
            else if(enclosed[1][1]){
//first
                wgeom_return[0]->x=x2[0];
                wgeom_return[0]->y=y2[0];
                wgeom_return[0]->width=wgeom2->width-x2[1]+x1[0];
                wgeom_return[0]->height=wgeom2->height;
//second
                wgeom_return[1]->x=x1[0];
                wgeom_return[1]->y=y1[1];
                wgeom_return[1]->width=x2[1]-x1[0];
                wgeom_return[1]->height=wgeom2->height-y1[1]+y2[0];
            }
            else if(enclosed[1][2]){
//first(same as [1][0])
                wgeom_return[0]->x=x1[1];
                wgeom_return[0]->y=y2[0];
                wgeom_return[0]->width=wgeom2->width-x1[1]+x2[0];
                wgeom_return[0]->height=wgeom2->height;
//second
                wgeom_return[1]->x=x2[0];
                wgeom_return[1]->y=y2[0];
                wgeom_return[1]->width=x1[1]-x2[0];
                wgeom_return[1]->height=wgeom2->height-y2[1]+y1[0];
            }
            else if(enclosed[1][3]){
//first(same as [1][1])
                wgeom_return[0]->x=x2[0];
                wgeom_return[0]->y=y2[0];
                wgeom_return[0]->width=wgeom2->width-x2[1]+x1[0];
                wgeom_return[0]->height=wgeom2->height;
//second
                wgeom_return[1]->x=x1[0];
                wgeom_return[1]->y=y2[0];
                wgeom_return[1]->width=x2[1]-x1[0];
                wgeom_return[1]->height=wgeom2->height-y2[1]+y1[0];
            }
            return -2;
        }
        else{//polygons collide but no point of one is in the other
            //so we just keep the two parts of geom2 that are outside geom1

            //break geom2 in two
            //two cases:
            //geom2 crossing vertically geom1
            //and geom2 crossing horizontally geom1
            //The proper one can be found by simply checking x,y positions
            *ngeoms=2;
            if(wgeom2->y<wgeom1->y){
                //common
                wgeom_return[0]->x=wgeom_return[1]->x=x2[0];
                wgeom_return[0]->width=wgeom_return[1]->width=wgeom2->width;

                wgeom_return[0]->y=y2[0];
                wgeom_return[0]->height=wgeom2->height-y2[1]+y1[0];
                wgeom_return[1]->y=y1[1];
                wgeom_return[1]->height=y2[1]-y1[1];
            }
            else{
                //common
                wgeom_return[0]->y=wgeom_return[1]->y=y2[0];
                wgeom_return[0]->height=wgeom_return[1]->height=wgeom2->height;

                wgeom_return[0]->x=x2[0];
                wgeom_return[0]->width=wgeom2->width-x2[1]+x1[0];
                wgeom_return[1]->x=x1[1];
                wgeom_return[1]->width=x2[1]-x1[1];
            }
            return -2;
        }
    }
}

int RectInsert(RectArea **root,WGeometry *wgeom){

    int total_insertions=0;
    RectArea *temp=NULL,*newnode=(RectArea *)malloc(sizeof(RectArea));
    //align
    //we do need to know boundaries
    wgeom->width+=(wgeom->width%2)|(wgeom->x%2);
    wgeom->height+=(wgeom->height%2)|(wgeom->y%2);
    wgeom->width+=(wgeom->width%2);
    wgeom->height+=(wgeom->height%2);
    wgeom->x-=wgeom->x%2;
    wgeom->y-=wgeom->y%2;
//     fprintf(stderr," %d %d %d %d\n",wgeom->x,

    newnode->geom.x=wgeom->x;
    newnode->geom.y=wgeom->y;
    newnode->geom.width=wgeom->width;
    newnode->geom.height=wgeom->height;
    newnode->prev=newnode->next=NULL;
    if(*root==NULL){
        *root=newnode;
        total_insertions=1;
    }
    else{
        WGeometry *wgeom_return[2];
        wgeom_return[0]=(WGeometry *)malloc(sizeof(WGeometry));
        wgeom_return[1]=(WGeometry *)malloc(sizeof(WGeometry));

        int ngeoms=0,insert_ok=1,i=0;
        temp=*root;
        while(insert_ok){   //if something is broken list does not procceed
                            //(except on -1 collres case)
            int collres=CollideRects(&(temp->geom),wgeom,wgeom_return,&ngeoms);
            if((!collres))
                insert_ok=1;
            else{
                for(i=0;i<ngeoms;i++){
                    wgeom_return[i]->width+=(wgeom_return[i]->width%2)|
                                            (wgeom_return[i]->x%2);
                    wgeom_return[i]->height+=(wgeom_return[i]->height%2)|
                                             (wgeom_return[i]->y%2);
                    wgeom_return[i]->width+=(wgeom_return[i]->width%2);
                    wgeom_return[i]->height+=(wgeom_return[i]->height%2);
                    wgeom_return[i]->x-=wgeom_return[i]->x%2;
                    wgeom_return[i]->y-=wgeom_return[i]->y%2;
                }
                insert_ok=0;
                switch(collres){
                    case 1://remove current node,reinsert new one
                        total_insertions--;
                        if(temp->prev!=NULL){//no root
                            if(temp->next!=NULL){//
                                RectArea *temp1=temp->next;
                                temp->prev->next=temp->next;
                                temp->next->prev=temp->prev;
                                free(temp);
                                if((wgeom->width>0)&&(wgeom->height>0))
                                    total_insertions+=RectInsert(&temp1,wgeom);
                            }
                            else{
                                temp->prev->next=newnode;
                                newnode->prev=temp->prev;
                                total_insertions++;
                                free(temp);
                            }
                        }
                        else{//root
                            if((*root)->next!=NULL){
                                (*root)=(*root)->next;
                                (*root)->prev=NULL;
                                if((wgeom->width>0)&&(wgeom->height>0))
                                    total_insertions+=RectInsert(root,wgeom);
                            }
                            else if((wgeom->width>0)&&(wgeom->height>0)){
                                *root=newnode;
                                total_insertions++;
                            }
                            else{
                                *root=NULL;
                                total_insertions++;
                            }
                            free(temp);
                         }
                        break;
                    case 2://done,area is already covered
                        free(newnode);
                        break;
                    case -1://current node is broken and reinserted
                            //(in same pos)
                            //newnode is also reinserted
                        if((wgeom_return[0]->width>0)&&
                           (wgeom_return[0]->height>0)){
                            temp->geom.x=wgeom_return[0]->x;
                            temp->geom.y=wgeom_return[0]->y;
                            temp->geom.width=wgeom_return[0]->width;
                            temp->geom.height=wgeom_return[0]->height;
                            if(temp->next==NULL){
                                temp->next=newnode;
                                newnode->prev=temp;
                                total_insertions++;
                            }
                            else{
                                insert_ok=1;
                            }
                        }
                        else{//it might happen that the old and now broken node
                            //is of zero width or height
                            //(so it isn't reinserted)
                            if((temp->prev==NULL)&&(temp->next!=NULL)){
                                *root=(*root)->next;
                                (*root)->prev=NULL;
                            }
                            else if((temp->next==NULL)&&(temp->prev!=NULL)){
                                temp->prev->next=newnode;
                                newnode->prev=temp->prev;
                            }
                            else if((temp->next==NULL)&&(temp->prev==NULL))
                                (*root)=newnode;
                            else{
                                total_insertions--;
                                temp->next->prev=temp->prev;
                                temp->prev->next=temp->next;
                                total_insertions+=RectInsert(&temp->next,
                                                             wgeom);
                            }
                            free(temp);
                        }
                        break;
                    case -2://new is broken and reinserted
                        if(temp->next==NULL){
                            total_insertions+=ngeoms;
                            newnode->geom.x=wgeom_return[0]->x;
                            newnode->geom.y=wgeom_return[0]->y;
                            newnode->geom.width=wgeom_return[0]->width;
                            newnode->geom.height=wgeom_return[0]->height;
                            temp->next=newnode;
                            newnode->prev=temp;
                            if(ngeoms>1){
                                RectArea *newnode1=
                                (RectArea *)malloc(sizeof(RectArea));

                                newnode1->geom.x=wgeom_return[1]->x;
                                newnode1->geom.y=wgeom_return[1]->y;
                                newnode1->geom.width=wgeom_return[1]->width;
                                newnode1->geom.height=wgeom_return[1]->height;
                                newnode->next=newnode1;
                                newnode1->prev=newnode;
                                newnode1->next=NULL;
                            }
                        }
                        else{
                            for(i=0;i<ngeoms;i++){
                                if((wgeom_return[i]->width>0)&&
                                   (wgeom_return[i]->height>0))
                                    total_insertions+=
                                    RectInsert(&temp->next,wgeom_return[i]);
                            }
                        }
                        break;
                    case -10://grouped
                        if(temp->prev==NULL){
                            if(temp->next==NULL){//empty list
                                    newnode->geom.x=wgeom_return[0]->x;
                                    newnode->geom.y=wgeom_return[0]->y;
                                    newnode->geom.width=wgeom_return[0]->width;
                                    newnode->geom.height=
                                    wgeom_return[0]->height;

                                    *root=newnode;
                                    free(temp);
                            }
                            else{
                                total_insertions--;
                                *root=temp->next;
                                (*root)->prev=NULL;
                                free(temp);
                                if((wgeom_return[0]->width>0)&&
                                   (wgeom_return[0]->height>0))
                                    total_insertions+=
                                    RectInsert(root,wgeom_return[0]);
                            }
                        }
                        else if(temp->next==NULL){//last, enter anyway
                            newnode->geom.x=wgeom_return[0]->x;
                            newnode->geom.y=wgeom_return[0]->y;
                            newnode->geom.width=wgeom_return[0]->width;
                            newnode->geom.height=wgeom_return[0]->height;
                            temp->prev->next=newnode;
                            newnode->prev=temp->prev;
                            free(temp);
                        }
                        else{//remove node and reinsert, starting where we were
                            total_insertions--;
                            RectArea *temp1=temp->next;
                            temp->prev->next=temp->next;
                            temp->next->prev=temp->prev;
                            free(temp);
                            if((wgeom_return[0]->width>0)&&
                               (wgeom_return[0]->height>0))
                                total_insertions+=
                                RectInsert(&temp1,wgeom_return[0]);
                        }
                        break;
                }
            }
            if(insert_ok){
                if(temp->next==NULL){
                    temp->next=newnode;
                    newnode->prev=temp;
                    total_insertions++;
                    break;
                }
                else{
                    temp=temp->next;
                }
            }
            else{
                break;
            }
        };

        free(wgeom_return[0]);
        free(wgeom_return[1]);
    }
    return total_insertions;
}

void ClearList(RectArea **root){

    RectArea *temp;
    temp=*root;
    if(temp!=NULL){
        while(temp->next!=NULL){
            temp=temp->next;
            free(temp->prev);

        }
        free(temp);
        *root=NULL;
    }
}

void BlocksFromList(RectArea **root,
                    unsigned int x_offset,
                    unsigned int y_offset,
                    unsigned int blocknum_x,
                    unsigned int blocknum_y){

    RectArea *temp;
    unsigned int i,
                 j,
                 blockno,
                 row_start,
                 row_end,
                 column_start,
                 column_end;
    temp=*root;
    for(i=0;i<blocknum_x*blocknum_y;i++){
        yblocks[i]=ublocks[i]=vblocks[i]=0;
    }

    while(temp!=NULL){
        column_start=(temp->geom.x-x_offset)/Y_UNIT_WIDTH;
        column_end=(temp->geom.x+(temp->geom.width-1)-x_offset)/Y_UNIT_WIDTH;
        row_start=(temp->geom.y-y_offset)/Y_UNIT_WIDTH;
        row_end=(temp->geom.y+(temp->geom.height-1)-y_offset)/Y_UNIT_WIDTH;
        for(i=row_start;i<row_end+1;i++){
            for(j=column_start;j<column_end+1;j++){
                blockno=i*blocknum_x+j;
                yblocks[blockno]=1;
                ublocks[blockno]=1;
                vblocks[blockno]=1;
            }
        }
        temp=temp->next;
    }

}



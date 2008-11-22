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

#include "config.h"

#include "rmd_rectinsert.h"
#include "rmd_types.h"


/**
* Collide two rectangles and dictate most sane action for insertion,
* as well as provide the updated rectangle(s)
* \param xrect1 resident rectangle
*
* \param xrect2 New rectangle
*
* \param xrect_return Pointer to rectangles to be inserted
*
* \param nrects number of entries in xrect_return
*
* \retval 0 No collision
*
* \retval 1 xrect1 is covered by xrect2
*
* \retval 2 xrect2 is covered by xrect1
*
* \retval -1 xrect1 was broken (new is picked up in xrect_return)
*
* \retval -2 xrect2 was broken (new is picked up in xrect_return)
*
* \retval -10 Grouping the two rects is possible
*
*/
static int CollideRects(XRectangle *xrect1,
                        XRectangle *xrect2,
                        XRectangle xrect_return[],
                        int *nrects) {
    //1 fits in 2
    if((xrect1->x>=xrect2->x)&&
        (xrect1->x+xrect1->width<=xrect2->x+xrect2->width)&&
        (xrect1->y>=xrect2->y)&&
        (xrect1->y+xrect1->height<=xrect2->y+xrect2->height)){
        *nrects=0;
        return 1;
    }
    //2 fits in 1
    else if((xrect2->x>=xrect1->x)&&
        (xrect2->x+xrect2->width<=xrect1->x+xrect1->width)&&
        (xrect2->y>=xrect1->y)&&
        (xrect2->y+xrect2->height<=xrect1->y+xrect1->height)){
        *nrects=0;
        return 2;
    }
    //no collision
    else if((xrect1->x+xrect1->width<xrect2->x)||
        (xrect2->x+xrect2->width<xrect1->x)||
        (xrect1->y+xrect1->height<xrect2->y)||
        (xrect2->y+xrect2->height<xrect1->y)){
        *nrects=0;
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
        int x1[2]={xrect1->x,xrect1->x+xrect1->width};
        int y1[2]={xrect1->y,xrect1->y+xrect1->height};
        int x2[2]={xrect2->x,xrect2->x+xrect2->width};
        int y2[2]={xrect2->y,xrect2->y+xrect2->height};
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
               (xrect1->width==xrect2->width)){
                *nrects=1;
                xrect_return[0].x = xrect1->x;
                xrect_return[0].y = xrect1->y;
                xrect_return[0].width = xrect1->width;
                xrect_return[0].height = xrect2->height + xrect2->y - xrect1->y;
                return -10;
            }
            else if((enclosed[1][0]&&enclosed[1][2])&&
                    (xrect1->height==xrect2->height)){
                *nrects=1;
                xrect_return[0].x = xrect1->x;
                xrect_return[0].y = xrect1->y;
                xrect_return[0].width = xrect2->width + xrect2->x - xrect1->x;
                xrect_return[0].height = xrect1->height;
                return -10;
            }
            else if((enclosed[1][3]&&enclosed[1][1])&&
                    (xrect1->height==xrect2->height)){
                *nrects=1;
                xrect_return[0].x = xrect2->x;
                xrect_return[0].y = xrect2->y;
                xrect_return[0].width = xrect1->width + xrect1->x - xrect2->x;
                xrect_return[0].height = xrect2->height;
                return -10;
            }
            else if((enclosed[1][3]&&enclosed[1][2])&&
                    (xrect1->width==xrect2->width)){
                *nrects=1;
                xrect_return[0].x = xrect2->x;
                xrect_return[0].y = xrect2->y;
                xrect_return[0].width = xrect2->width;
                xrect_return[0].height = xrect1->height + xrect1->y - xrect2->y;
                return -10;
            }
            //if control reaches here therewasn't a group and we go on
        }
        if(tot2==2){
            //break rect2
            xrect_return[0].x = xrect2->x;
            xrect_return[0].y = xrect2->y;
            xrect_return[0].width = xrect2->width;
            xrect_return[0].height = xrect2->height;
            *nrects=1;
            if(enclosed[1][0]&&enclosed[1][1]){
                xrect_return[0].y = y1[1];
                xrect_return[0].height -= y1[1] - y2[0];
            }
            else if(enclosed[1][0]&&enclosed[1][2]){
                xrect_return[0].x = x1[1];
                xrect_return[0].width -= x1[1] - x2[0];
            }
            else if(enclosed[1][3]&&enclosed[1][1])
                xrect_return[0].width -= x2[1] - x1[0];
            else if(enclosed[1][3]&&enclosed[1][2])
                xrect_return[0].height -= y2[1] - y1[0];
            return -2;
        }
        else if(tot1==2){
            //if the first one breaks(which is already inserted)
            //then we reenter the part that was left and the one
            //that was to be inserted
            xrect_return[1].x = xrect2->x;
            xrect_return[1].y = xrect2->y;
            xrect_return[1].width = xrect2->width;
            xrect_return[1].height = xrect2->height;
            xrect_return[0].x = xrect1->x;
            xrect_return[0].y = xrect1->y;
            xrect_return[0].width = xrect1->width;
            xrect_return[0].height = xrect1->height;
            *nrects=1;
            if(enclosed[0][0]&&enclosed[0][1]){
                xrect_return[0].y = y2[1];
                xrect_return[0].height -= y2[1] - y1[0];
            }
            else if(enclosed[0][0]&&enclosed[0][2]){
                xrect_return[0].x = x2[1];
                xrect_return[0].width -= x2[1] - x1[0];
            }
            else if(enclosed[0][3]&&enclosed[0][1])
                xrect_return[0].width -= x1[1] - x2[0];
            else if(enclosed[0][3]&&enclosed[0][2])
                xrect_return[0].height -= y1[1] - y2[0];
            return -1;

        }
        else if(tot2==1){   //in which case there is also tot1==1
                            //but we rather not break that
                            //break rect2 in two
            *nrects=2;
            if(enclosed[1][0]){
//first
                xrect_return[0].x = x1[1];
                xrect_return[0].y = y2[0];
                xrect_return[0].width = xrect2->width - x1[1] + x2[0];
                xrect_return[0].height = xrect2->height;
//second
                xrect_return[1].x = x2[0];
                xrect_return[1].y = y1[1];
                xrect_return[1].width = x1[1] - x2[0];
                xrect_return[1].height = xrect2->height - y1[1] + y2[0];
            }
            else if(enclosed[1][1]){
//first
                xrect_return[0].x = x2[0];
                xrect_return[0].y = y2[0];
                xrect_return[0].width = xrect2->width - x2[1] + x1[0];
                xrect_return[0].height = xrect2->height;
//second
                xrect_return[1].x = x1[0];
                xrect_return[1].y = y1[1];
                xrect_return[1].width = x2[1] - x1[0];
                xrect_return[1].height = xrect2->height - y1[1] + y2[0];
            }
            else if(enclosed[1][2]){
//first(same as [1][0])
                xrect_return[0].x = x1[1];
                xrect_return[0].y = y2[0];
                xrect_return[0].width = xrect2->width - x1[1] + x2[0];
                xrect_return[0].height = xrect2->height;
//second
                xrect_return[1].x = x2[0];
                xrect_return[1].y = y2[0];
                xrect_return[1].width = x1[1] - x2[0];
                xrect_return[1].height = xrect2->height - y2[1] + y1[0];
            }
            else if(enclosed[1][3]){
//first(same as [1][1])
                xrect_return[0].x = x2[0];
                xrect_return[0].y = y2[0];
                xrect_return[0].width = xrect2->width - x2[1] + x1[0];
                xrect_return[0].height = xrect2->height;
//second
                xrect_return[1].x = x1[0];
                xrect_return[1].y = y2[0];
                xrect_return[1].width = x2[1] - x1[0];
                xrect_return[1].height = xrect2->height - y2[1] + y1[0];
            }
            return -2;
        }
        else{//polygons collide but no point of one is in the other
            //so we just keep the two parts of rect2 that are outside rect1

            //break rect2 in two
            //two cases:
            //rect2 crossing vertically rect1
            //and rect2 crossing horizontally rect1
            //The proper one can be found by simply checking x,y positions
            *nrects=2;
            if(xrect2->y<xrect1->y){
                //common
                xrect_return[0].x = xrect_return[1].x = x2[0];
                xrect_return[0].width  = xrect_return[1].width = xrect2->width;

                xrect_return[0].y = y2[0];
                xrect_return[0].height = xrect2->height - y2[1] + y1[0];
                xrect_return[1].y = y1[1];
                xrect_return[1].height = y2[1] - y1[1];
            }
            else{
                //common
                xrect_return[0].y = xrect_return[1].y = y2[0];
                xrect_return[0].height = xrect_return[1].height = xrect2->height;

                xrect_return[0].x = x2[0];
                xrect_return[0].width = xrect2->width - x2[1] + x1[0];
                xrect_return[1].x = x1[1];
                xrect_return[1].width = x2[1] - x1[1];
            }
            return -2;
        }
    }
}

int RectInsert(RectArea **root,XRectangle *xrect){

    int total_insertions=0;
    RectArea *temp=NULL,*newnode=(RectArea *)malloc(sizeof(RectArea));
    //align
    //we do need to know boundaries
    xrect->width+=(xrect->width%2)|(xrect->x%2);
    xrect->height+=(xrect->height%2)|(xrect->y%2);
    xrect->width+=(xrect->width%2);
    xrect->height+=(xrect->height%2);
    xrect->x-=xrect->x%2;
    xrect->y-=xrect->y%2;
//     fprintf(stderr," %d %d %d %d\n",xrect->x,

    newnode->rect.x=xrect->x;
    newnode->rect.y=xrect->y;
    newnode->rect.width=xrect->width;
    newnode->rect.height=xrect->height;
    newnode->prev=newnode->next=NULL;
    if(*root==NULL){
        *root=newnode;
        total_insertions=1;
    }
    else{
        XRectangle xrect_return[2];

        int nrects=0,insert_ok=1,i=0;
        temp=*root;
        while(insert_ok){   //if something is broken list does not procceed
                            //(except on -1 collres case)
            int collres = CollideRects(&temp->rect, xrect, &xrect_return[0], &nrects);
            if((!collres))
                insert_ok=1;
            else{
                for(i=0;i<nrects;i++){
                    xrect_return[i].width += (xrect_return[i].width % 2) |
                                             (xrect_return[i].x % 2);
                    xrect_return[i].height += (xrect_return[i].height % 2) |
                                              (xrect_return[i].y % 2);
                    xrect_return[i].width += (xrect_return[i].width % 2);
                    xrect_return[i].height += (xrect_return[i].height % 2);
                    xrect_return[i].x -= xrect_return[i].x % 2;
                    xrect_return[i].y -= xrect_return[i].y % 2;
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
                                if((xrect->width>0)&&(xrect->height>0))
                                    total_insertions+=RectInsert(&temp1,xrect);
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
                                if((xrect->width>0)&&(xrect->height>0))
                                    total_insertions+=RectInsert(root,xrect);
                            }
                            else if((xrect->width>0)&&(xrect->height>0)){
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
                        if (xrect_return[0].width > 0 &&
                            xrect_return[0].height > 0) {
                            temp->rect.x = xrect_return[0].x;
                            temp->rect.y = xrect_return[0].y;
                            temp->rect.width = xrect_return[0].width;
                            temp->rect.height = xrect_return[0].height;
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
                                                             xrect);
                            }
                            free(temp);
                        }
                        break;
                    case -2://new is broken and reinserted
                        if(temp->next==NULL){
                            total_insertions+=nrects;
                            newnode->rect.x = xrect_return[0].x;
                            newnode->rect.y = xrect_return[0].y;
                            newnode->rect.width = xrect_return[0].width;
                            newnode->rect.height = xrect_return[0].height;
                            temp->next=newnode;
                            newnode->prev=temp;
                            if(nrects>1){
                                RectArea *newnode1=
                                (RectArea *)malloc(sizeof(RectArea));

                                newnode1->rect.x = xrect_return[1].x;
                                newnode1->rect.y = xrect_return[1].y;
                                newnode1->rect.width = xrect_return[1].width;
                                newnode1->rect.height = xrect_return[1].height;
                                newnode->next=newnode1;
                                newnode1->prev=newnode;
                                newnode1->next=NULL;
                            }
                        }
                        else{
                            for(i=0;i<nrects;i++){
                                if(xrect_return[i].width > 0 &&
                                   xrect_return[i].height > 0)
                                    total_insertions+=
                                    RectInsert(&temp->next, &xrect_return[i]);
                            }
                        }
                        break;
                    case -10://grouped
                        if(temp->prev==NULL){
                            if(temp->next==NULL){//empty list
                                    newnode->rect.x = xrect_return[0].x;
                                    newnode->rect.y = xrect_return[0].y;
                                    newnode->rect.width = xrect_return[0].width;
                                    newnode->rect.height = xrect_return[0].height;

                                    *root=newnode;
                                    free(temp);
                            }
                            else{
                                total_insertions--;
                                *root=temp->next;
                                (*root)->prev=NULL;
                                free(temp);
                                if(xrect_return[0].width > 0 &&
                                   xrect_return[0].height > 0)
                                    total_insertions+=
                                    RectInsert(root, &xrect_return[0]);
                            }
                        }
                        else if(temp->next==NULL){//last, enter anyway
                            newnode->rect.x = xrect_return[0].x;
                            newnode->rect.y = xrect_return[0].y;
                            newnode->rect.width = xrect_return[0].width;
                            newnode->rect.height = xrect_return[0].height;
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
                            if(xrect_return[0].width > 0 &&
                               xrect_return[0].height > 0)
                                total_insertions+=
                                RectInsert(&temp1, &xrect_return[0]);
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

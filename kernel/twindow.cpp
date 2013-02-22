#include "twindow.h"
#include "all.h"


void window_init(void)
{
	winctl.winlay=layer_alloc( layerctl);
	winctl.nWindows=0;
	memset(winctl.iwi,0,sizeof winctl.iwi);
	layer_updown(layerctl,winctl.winlay,layerctl->height-2);

}



int t_CreateWindow(const char* title,const WindowInfo *wi)
{
	InternWindowInfo intern;
	int winID;//handle
	uid_t winlay=winctl.winlay;
	if(winctl.nWindows >= MAX_WINDOWS)
		return -1;//failure
	if(strlen(title)>=MAX_TITLELEN)
	{
		k_memmov(title,intern.title,MAX_TITLELEN);
	}
	else
	{
		int sz=strlen(title);
		k_memmov(title,intern.title,sz);
		memset(intern.title+sz,'\0',100-sz);
	}

	{
		intern.x = wi->reg.x;
		intern.y = wi->reg.y;
		intern.width = wi->reg.width;
		make_max(intern.width,MIN_WIDTH);// >=30
		intern.height = wi->reg.height;
		make_max(intern.height,MIN_HEIGHT);//>=30
		intern.flags=0;
		intern.wh=wi->wh;
		intern.parent=wi->parent;
		int i;
		for(i=0;i<MAX_CHILDREN;i++)
		{
			intern.children[i]=(InternWindowInfo*)0;
		}
	}
	//add to winctl
	{//Ç±ÇÍÇÕåäÇñÑÇﬂÇÈï˚éÆÇ…ïœÇ¶ÇÈ
		winID=winctl.nWindows;
		winctl.iwi[winctl.nWindows]=intern;
		winctl.nWindows++;
		//we have to modify 'order'
		int data[1]={winctl.nWindows-1};
		insert_array(winctl.order,0 ,// top level
				(void*)data,1,winctl.nWindows,MAX_WINDOWS,sizeof(int));
	}
	

	//äOògÇ»Ç«
	
	wi->wh(winID,MSG_CREATE,wi->reg.x|wi->reg.y<<16,wi->reg.width | wi->reg.height<<16);
	
	return winID;
}

int createComponent(int cmpID,const WindowInfo *wi)
{
	int handle=wi->parent;
	int compwin=t_CreateWindow("",wi);
	winctl.iwi[compwin].parent=handle;
	InternWindowInfo *target=&winctl.iwi[handle];
	//get the vacant id
	int vacant,i;
	for(vacant=cmpID,i=0;i<MAX_CHILDREN;i++)
	{
		if(target->children[vacant]==(InternWindowInfo*)0)
			break;
		vacant++;
		if(vacant>=MAX_CHILDREN)
			vacant-=MAX_CHILDREN;
	}
	target->children[vacant]=&winctl.iwi[compwin];
	return vacant;
}


int t_DestroyWindow(int handle)
{
	if(handle < 0 || handle >= winctl.nWindows)
	{
		return -1;//no such a window
	}	
	//ask if the window should be destroyed
	if(t_SendMessage(handle,MSG_DESTROY,0,0) != 0)
	{
		//We have to wait destruction for the window.
		return -2;
	}
	//destroys all children windows 
	int i;
	for(i=0;i<MAX_CHILDREN;i++)
	{
		InternWindowInfo *child=winctl.iwi[handle].children[i];
		if(child==(InternWindowInfo*)0)
		{
			continue;
		}
		int res=t_DestroyWindow(child-winctl.iwi);
		if(res < 0)//failure
		{
			//stop destruction
			return -3;
		}
	}
	//erase from winctl
	//Ç±Ç±ÇÕåäÇêÿÇËãlÇﬂÇÈïKóvÇ™Ç»Ç≠Ç»ÇÈ
	InternWindowInfo* iwi=&winctl.iwi[handle];
	//memmng_free_4k(iwi->bitmap,iwi->width * iwi->height);
	Rect winrect={iwi->x,iwi->y,iwi->x+iwi->width,iwi->y+iwi->height};
	erase_array(winctl.iwi,handle,handle+1,winctl.nWindows,sizeof(InternWindowInfo));
	winctl.nWindows--;
	//fills in where the window was with COL8_INV,with repainting
	layer_boxfill8(layerctl,winctl.winlay,winrect,COL8_INV,LAYER_REPAINT);

	//ÇŸÇ©ÇÃwindowÇ…çƒï`âÊÇë£Ç∑
	for(int i=0;i<winctl.nWindows;i++)
	{
		/*order[i]ÇÃÅAè¡Ç¶ÇΩwindowÇ…âBÇ≥ÇÍÇƒÇ¢ÇΩïîï™Ççƒï`âÊÇ∑ÇÈÅBÇ»Ç®Ç±ÇÃÇ∆Ç´
		ÇªÇÃïîï™Ç™ÇŸÇ©ÇÃwindowÇ…âBÇ≥ÇÍÇƒÇ¢ÇÈÇ©Ç«Ç§Ç©ÇÕçló∂ÇµÇ»Ç¢
		*/
		InternWindowInfo *info=winctl.iwi+winctl.order[i];
		Rect iwin={info->x,info->y,info->x+info->width,info->y+info->height};
		rect_and(&iwin,winrect);
		t_SendMessage(winctl.order[i],MSG_REPAINT,(iwin.left-info->x)|((iwin.top-info->y)<<16),
			(iwin.right-iwin.left)|((iwin.bottom-iwin.top)<<16));
		
	}
	return 0;
}

int t_SendMessage(int handle,int msg,int arg0,int arg1)
{
	WindowHandler handler=winctl.iwi[handle].wh;
	return handler(handle,msg,arg0,arg1);
}

int getWindow(int xpos , int ypos)
{
	int i;
	for(i=0;i<winctl.nWindows;i++)
	{
		int winID=winctl.order[i];
		InternWindowInfo* t=winctl.iwi+winID;
		Rect rect={t->x,t->y,t->x+t->width,t->y+t->height};
		Rect point={xpos,ypos,xpos+1,ypos+1};
		if(rect_checkbounds(point,rect) == 0)
			return winID;
	}
	return -1;
}


int getWindowInfo(int handle,WindowInfo *wi)
{
	if(handle<0 || handle >=winctl.nWindows)
		return -1;
	InternWindowInfo *iwi=&winctl.iwi[handle];
	wi->reg.x=iwi->x;
	wi->reg.y=iwi->y;
	wi->reg.width=iwi->width;
	wi->reg.height=iwi->height;
	wi->wh=iwi->wh;
	wi->parent=iwi->parent;
	return 0;
}


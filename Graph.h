#pragma once


struct Graph
{
	struct Graph* next;

	int range;
	int mask;

	//Tk_Window tkwin;
	//Tcl_Interp* interp;

	int flags;
	//XDisplay* x;

	int visible;
	int w_x, w_y;
	int w_width, w_height;

	Pixmap pixmap;

	int* pixels;

	//XFontStruct* fontPtr;
	//Tk_3DBorder border;

	int borderWidth;
	int relief;

	Tk_TimerToken draw_graph_token;
};

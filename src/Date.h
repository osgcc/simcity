#pragma once


struct SimDate
{
	struct SimDate* next;
	int reset;
	int month;
	int year;
	int lastmonth;
	int lastyear;

	//Tk_Window tkwin;
	//Tcl_Interp *interp;

	int flags;

	//XDisplay *x;

	int visible;
	int w_x, w_y;
	int w_width, w_height;

	//Pixmap pixmap;

	int* pixels;

	//XFontStruct *fontPtr;
	//Tk_3DBorder border;

	int borderWidth;
	int padX;
	int padY;
	int width;
	int monthTab;
	int monthTabX;
	int yearTab;
	int yearTabX;

	//Tk_TimerToken draw_date_token;

};

#include "tools.h"

/* -------------------------------------------------------------------------
Package tools
Copyright (c) 2006 Oleg Sklyar
See: ../LICENSE for license, LGPL
------------------------------------------------------------------------- */

int GTK_OK = 0;

/*----------------------------------------------------------------------- */
double
distanceXY (const PointXY pt1, const PointXY pt2) {
    return sqrt ( (long double)( (pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y) ) );
}

/*----------------------------------------------------------------------- */
double
distancexy (int x1, int y1, int x2, int y2) {
    return sqrt ( (long double)( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) ) );
}

/*----------------------------------------------------------------------- */
PointXY
pointFromIndex (const int index, const int xsize) {
    PointXY res;
    res.y = floor (index / xsize);
    res.x = index - res.y * xsize;
    return res;
}

/*----------------------------------------------------------------------- */
int
indexFromPoint (const PointXY pt, const int xsize) {
    return (int)( fabs(pt.x + pt.y * xsize) );
}

/*----------------------------------------------------------------------- */
int
indexFromXY (const int x, const int y, const int xsize) {
    return x + y * xsize;
}

/*----------------------------------------------------------------------- */
int
isImage (SEXP x) {
    if ( strcmp( CHAR( asChar( GET_CLASS(x) ) ), "Image") != 0 && 
         strcmp( CHAR( asChar( GET_CLASS(x) ) ), "IndexedImage") != 0) return 0;
    if ( LENGTH( GET_DIM(x) ) < 2 ) return 0;
    return 1;
}

/*----------------------------------------------------------------------- */
int
getColorMode(SEXP x) {
  int colorMode;
  colorMode = INTEGER ( GET_SLOT(x, mkString("colormode") ) )[0];
  return(colorMode);
}

/*----------------------------------------------------------------------- */
// If type=0, returns the total number of frames
// If type=1, returns the number of frames to be rendered, according to the colorMode
int
getNumberOfFrames(SEXP x, int type) {
  int n,colorMode;
  int k,p,kp;
  colorMode=getColorMode(x);

  if (type==1 && colorMode==MODE_COLOR) kp=3;
  else kp=2;
  
  n=1;
  p=GET_LENGTH(GET_DIM(x));
  if (p>kp) {
    for (k=kp;k<p;k++) n=n*INTEGER(GET_DIM(x))[k];
  }

  return(n);
}

/*----------------------------------------------------------------------- */
int
getNumberOfChannels(SEXP x) {
  int colorMode;
  int nbChannels;
  colorMode=getColorMode(x);

  if (colorMode!=MODE_COLOR) nbChannels=1;
  else {
    if (LENGTH(GET_DIM(x))<3) nbChannels=1;
    else nbChannels=INTEGER(GET_DIM(x))[2];
  }
  return(nbChannels);
}

/*----------------------------------------------------------------------- */
// colorMode must be MODE_GRAYSCALE or MODE_COLOR
void getColorStrides(SEXP x,int index,int *redstride,int *greenstride,int *bluestride) {
  int width,height,colorMode,nbChannels;

  width=INTEGER(GET_DIM(x))[0];
  height=INTEGER(GET_DIM(x))[1];
  colorMode=getColorMode(x);
  
  *redstride=index*width*height;
  *greenstride=index*width*height;
  *bluestride=index*width*height;

  if (colorMode==MODE_TRUECOLOR) warning("getColorStrides cannot be called if colorMode=TrueColor");

  if (colorMode==MODE_COLOR) {
    nbChannels=getNumberOfChannels(x);
    *redstride=index*nbChannels*width*height;
    *greenstride=-1;
    *bluestride=-1;
    if (nbChannels>1) *greenstride=index*nbChannels*width*height+width*height;
    if (nbChannels>2) *bluestride=index*nbChannels*width*height+2*width*height;
  }
}

#ifndef CDrawAction_H
#define CDrawAction_H

#include "../stdafx.h"
#include <typeinfo>



class CDrawAction{
public:
    virtual void show()=0;
    virtual void drawToDC(HDC hdc,BYTE *bytes)=0;
    virtual CDrawAction* clone()=0;
    virtual ~CDrawAction(){};
};
class CFill:public CDrawAction{
    POINT start;
    COLORREF color;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CFill(POINT _start,COLORREF _color){
            color=_color;
            start=_start;
        }
    void drawToDC(HDC hdc,BYTE *bytes){
        DeleteObject(SelectObject(hdc,CreateSolidBrush(color)));
        ExtFloodFill(hdc,start.x,start.y,GetPixel(hdc,start.x,start.y),FLOODFILLSURFACE);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CFill(start,color);
        return temp;
    }
    ~CFill()override{}
};
class CLine:public CDrawAction{
    POINT start,end;
    COLORREF color;
    int size;
    public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CLine(POINT _start,POINT _end,int _size,COLORREF _color){
            color=_color;
            size=_size;
            start=_start;
            end=_end;
        }
    void drawToDC(HDC hdc,BYTE *bytes){
        DeleteObject(SelectObject(hdc,CreatePen(PS_SOLID,size,color)));
        MoveToEx(hdc,start.x,start.y,0);
        LineTo(hdc,end.x,end.y);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CLine(start,end,size,color);
        return temp;
    }
    ~CLine()override{}
};
class CEllipse : public CDrawAction{
    POINT start,end;
    bool transparent;
    int size;
    COLORREF color,fillColor;
    public:
        void show(){printf("nazwa=%s\n",typeid( this ).name());}
        CEllipse(POINT _start,POINT _end,int _size,COLORREF _color,COLORREF _fillColor,bool _transparent){
            color=_color;
            transparent=_transparent;
            fillColor=_fillColor;
            size=_size;
            start=_start;
            end=_end;
        }
    void drawToDC(HDC hdc,BYTE *bytes){
        DeleteObject(SelectObject(hdc,CreatePen(PS_SOLID,size,color)));
        if(transparent)
        {

            DeleteObject(SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));
        }
        else
        {
            DeleteObject(SelectObject(hdc,CreateSolidBrush(fillColor)));
        }
        //
        Ellipse(hdc,start.x,start.y,end.x,end.y);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CEllipse(start,end,size,color,fillColor,transparent);
        return temp;
    }
    ~CEllipse()override{}
};
class CRectangle : public CDrawAction{
    POINT start,end;
    bool transparent;
    int size;
    COLORREF color,fillColor;
    public:
        void show(){printf("nazwa=%s\n",typeid( this ).name());}
        CRectangle(POINT _start,POINT _end,int _size,COLORREF _color,COLORREF _fillColor,bool _transparent){
            color=_color;
            transparent=_transparent;
            fillColor=_fillColor;
            size=_size;
            start=_start;
            end=_end;
        }
    void drawToDC(HDC hdc,BYTE *bytes){
        DeleteObject(SelectObject(hdc,CreatePen(PS_SOLID,size,color)));
        if(transparent)
        {

            DeleteObject(SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));
        }
        else
        {
            DeleteObject(SelectObject(hdc,CreateSolidBrush(fillColor)));
            //FillRect()
        }
        //
        Rectangle(hdc,start.x,start.y,end.x,end.y);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CRectangle(start,end,size,color,fillColor,transparent);
        return temp;
    }
    ~CRectangle()override{}
};
class CPixel : public CDrawAction{
    POINT start;
    COLORREF color;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CPixel(POINT _start,COLORREF _color){
        color=_color;
        start=_start;
    }
    void drawToDC(HDC hdc,BYTE *bytes){
        SetPixel(hdc,start.x,start.y,color);
    }
    CDrawAction* clone(){
        return new CPixel(start,color);
    }
    ~CPixel(){}
};
class CClean:public CDrawAction{
    int width,height;
public:
    CClean(int _width,int _height){width=_width;height=_height;};
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    void drawToDC(HDC hdc,BYTE *bytes){
        RECT rect;rect.top=0;rect.left=0;rect.right=width;rect.bottom=height;
        HBRUSH brush=CreateSolidBrush(RGB(255,255,255));
        FillRect(hdc,&rect,brush);
        DeleteObject(brush);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CClean(width,height);
        return temp;
    }
    ~CClean()override{};
};
class CRubber: public CDrawAction{
private:
    POINT start;
    int size;
    COLORREF color;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CRubber(POINT _start,int _size,COLORREF _color){
        start=_start;
        size=_size;//bo size 1 daje 0! ale fail przy redo sie pogrubiala linia xd ;P
        color=_color;
    };
    void drawToDC(HDC hdc,BYTE *bytes){
        RECT rect;rect.top=start.y-size/2;rect.left=start.x-size/2;rect.right=start.x+size/2;rect.bottom=start.y+size/2;
        HBRUSH brush=CreateSolidBrush(color);
        FillRect(hdc,&rect,brush);
        DeleteObject(brush);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CRubber(start,size,color);
        return temp;
    }
    ~CRubber()override{};
};
class CSpray:public CDrawAction{
    COLORREF color;
    POINT start;
    int size;
    int seed;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CSpray(POINT _start,int _size,COLORREF _color,int _seed){
        start=_start;
        size=_size;
        color=_color;
        seed=_seed;
        //seed=GetTickCount();
    }
    void drawToDC(HDC hdc,BYTE *bytes){
        srand(seed);
        for(int i=0;i<100;i++)
        {
            int x=start.x-size/2+rand()%size;
            int y=start.y-size/2+rand()%size;
            COLORREF colorP=GetPixel(hdc,x,y);
            COLORREF colorF=RGB((GetRValue(colorP)*3+GetRValue(color))/4,(GetGValue(colorP)*3+GetGValue(color))/4,(GetBValue(colorP)*3+GetBValue(color))/4);
            SetPixel(hdc,x,y,colorF);
        }
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CSpray(start,size,color,seed);
        return temp;
    }
    ~CSpray()override{}
};
class CSprayFast:public CDrawAction{///unusable not draw to restoreDC???
    COLORREF color;
    POINT start;
    int size;
    double cover;
    int seed;
    BYTE *bits;
    int width;
    int height;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CSprayFast(POINT _start,int _size,double _cover,COLORREF _color,int _seed,int _width,int _height){
        start=_start;
        size=_size;
        cover=_cover;
        color=_color;
        seed=_seed;
        width=_width;
        height=_height;
        //seed=GetTickCount();
    }
    void drawToDC(HDC hdc,BYTE *bytes){
        srand(seed);
        int max=log(size)*(size);
        for(int i=1;i<(max);i++)
        {
            int x=(start.x-size/2+rand()%size);
            int y=(start.y-size/2+rand()%size);
            if( (x-start.x)*(x-start.x) +(y-start.y)*(y-start.y)<size*size/4)
            if((x<width)&&(x>=0))
                if((y<height)&&(y>=0))
                {
                    //prevColor=*(int*)(bytes+(y*width+x)*4);
                    float cover=(this)->cover/100.0f;
                    BYTE *v=bytes+(y*width+x)*4;
                    /**(BYTE*)(v)=    ((GetBValue(color)+*(BYTE*)(v)*cover)/100);
                    *(BYTE*)(v+1)=  ((GetGValue(color)+*(BYTE*)(v+1)*cover)/100);
                    *(BYTE*)(v+2)=  ((GetRValue(color)+*(BYTE*)(v+2)*cover)/100);*/
                    float bb=*(BYTE*)(v)  /255.0f;
                    float bg=*(BYTE*)(v+1)/255.0f;
                    float br=*(BYTE*)(v+2)/255.0f;
                    float fr=GetRValue(color)/255.0f;
                    float fg=GetGValue(color)/255.0f;
                    float fb=GetBValue(color)/255.0f;
                    float outr,outg,outb;
                    outr = (fr * cover) + (br * (1.0 - cover));
                    outg = (fg * cover) + (bg * (1.0 - cover));
                    outb = (fb * cover) + (bb * (1.0 - cover));
                    *(BYTE*)(v)     =outb*255;
                    *(BYTE*)(v+1)   =outg*255;
                    *(BYTE*)(v+2)   =outr*255;
                }


        //SetPixel(hdc,start.x-size/2+rand()%size,start.y-size/2+rand()%size,color);
        }
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CSprayFast(start,size,cover,color,seed,width,height);
        return temp;
    }
    ~CSprayFast()override{}
};
class CText:public CDrawAction{
    POINT start;
    wchar_t text[256];
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CText(POINT _start,wchar_t *_text){
        wsprintfW(text,L"%s",_text);
        start=_start;
        //seed=GetTickCount();
    }
    void drawToDC(HDC hdc,BYTE *bytes){
        TextOutW(hdc,start.x,start.y,text,wcslen(text));
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CText(start,text);
        return temp;
    }
    ~CText()override{}
};
class CText2:public CDrawAction{
    RECT rect;
    wchar_t text[1024];
    LOGFONT logfont;
    COLORREF color,color2;
    bool transparent;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CText2(RECT _rect,wchar_t *_text,LPLOGFONT lpfont,COLORREF _color,COLORREF _color2,bool _transparent){
        transparent=_transparent;
        rect=_rect;
        wsprintfW(text,L"%ls",_text);
        CopyMemory(&logfont,lpfont,sizeof(LOGFONT));
        color=_color;
        color2=_color2;
    }
    void drawToDC(HDC hdc,BYTE *bytes){

        HFONT newFont=CreateFontIndirect(&logfont);
        HFONT oldFont=(HFONT)SelectObject(hdc,newFont);
        SetTextColor(hdc,color);
        SetBkColor(hdc,color2);
        if(transparent) SetBkMode(hdc, TRANSPARENT);
        else SetBkMode(hdc, OPAQUE);

        DrawTextW(hdc,text,wcslen(text),&rect,DT_EDITCONTROL|DT_WORDBREAK);
        DeleteObject(SelectObject(hdc,oldFont));
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CText2(rect,text,&logfont,color,color2,transparent);
        return temp;
    }
    ~CText2()override{}
};
class CNegColor:public CDrawAction{
    POINT start,end;
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CNegColor(POINT _start,POINT _end){
        start=_start;
        end=_end;
    }
    void drawToDC(HDC hdc,BYTE *bytes){
        COLORREF color;
        for(int i=start.x;i<end.x;i++)
           for(int j=start.y;j<end.y;j++)
           {
               color=GetPixel(hdc,i,j);
               SetPixel(hdc,i,j,RGB( 255-GetRValue(color), 255-GetGValue(color), 255-GetBValue(color) ));
           }
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CNegColor(start,end);
        return temp;
    }
    ~CNegColor()override{}
};
class CPasteBMP : public CDrawAction{
    POINT start,end;
    BYTE *data;
    int width,height;

private:
    CPasteBMP(POINT _start,POINT _end,BYTE *_data,int _width,int _height){
        start=_start;
        end=_end;
        width=_width;
        height=_height;
        data=new BYTE[width*height*4];
        CopyMemory(data,_data,sizeof(BYTE)*width*height*4);
    }
public:
    void show(){printf("nazwa=%s\n",typeid( this ).name());}
    CPasteBMP(POINT _start,POINT _end,HDC src,int _width,int _height){

        start=_start;
        end=_end;
        width=_width;
        height=_height;

        COLORREF color;
        data=new BYTE[width*height*4];
        for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
        {
            color=GetPixel(src,j,i);
            data[(i*width+j)*4+2]=GetRValue(color);
            data[(i*width+j)*4+1]=GetGValue(color);
            data[(i*width+j)*4]=GetBValue(color);
            data[(i*width+j)*4+3]=0;
        }
    }
    void drawToDC(HDC hdc,BYTE *bytes){
            BYTE *data2;
            //printf("width=%d height=%d",width,height);
            HDC hDC = GetDC(NULL);
            HDC hdc2= CreateCompatibleDC(hdc);

            HBITMAP bitmap;
            bitmap= create32DIB(hDC,width,height,&data2);///CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&data2, NULL, 0);

            CopyMemory(data2,data,sizeof(BYTE)*width*height*4);
            if(bitmap==0) MessageBox(0,0,0,0);
            HBITMAP old=(HBITMAP) SelectObject(hdc2,bitmap);
            StretchBlt(hdc,start.x,start.y,width,height,hdc2,0,0,width,height,SRCCOPY);

            SelectObject(hdc2,old);

            DeleteObject(bitmap);
            DeleteDC(hdc2);
            ReleaseDC(NULL, hDC);


        /*for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
        {
            SetPixel(hdc,start.x+j,start.y+i,RGB( data[(i*width+j)*4],data[(i*width+j)*4+1] ,data[(i*width+j)*4+2] ));
        }*/
    }
    ~CPasteBMP()override{
        delete[]data;

    }
    CDrawAction* clone(){
        CDrawAction *temp=new CPasteBMP(start,end,data,width,height);
        return temp;
    }
};
class CPolygon :public CDrawAction{
    bool transparent;
    int size;
    COLORREF color,fillColor;
    POINT *points;
    int pCount;
    public:
        void show(){printf("nazwa=%s\n",typeid( this ).name());}
        CPolygon(POINT *_points,int _pCount,int _size,COLORREF _color,COLORREF _fillColor,bool _transparent){
            color=_color;
            transparent=_transparent;
            fillColor=_fillColor;
            size=_size;
            pCount=_pCount;
            points=new POINT[pCount];
            CopyMemory(points,_points,pCount*sizeof(POINT));
        }
    void drawToDC(HDC hdc,BYTE *bytes){
        DeleteObject(SelectObject(hdc,CreatePen(PS_SOLID,size,color)));
        if(transparent)
        {

            DeleteObject(SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));
        }
        else
        {
            DeleteObject(SelectObject(hdc,CreateSolidBrush(fillColor)));
            //FillRect()
        }
        //
        Polygon(hdc,points,pCount);
    }
    CDrawAction* clone(){
        CDrawAction *temp=new CPolygon(points,pCount,size,color,fillColor,transparent);
        return temp;
    }
    ~CPolygon()override{
    delete []points;
    }
};
#endif // CDrawAction_H


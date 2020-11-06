#include "NanoCanvas.h"
#include "nanovg/nanovg.h"

namespace NanoCanvas
{
    Image::Image(Canvas& canvas,const char* filePath, int imageFlags)
    {
        m_canvas = &canvas;
        auto vg = canvas.nvgContext();
        if(vg && strlen(filePath))
            imageID = nvgCreateImage(vg, filePath, imageFlags);
    }
    Image::Image(Canvas& canvas, const Memory& memory, int imageFlags)
    {
        m_canvas = &canvas;
        auto vg = canvas.nvgContext();
        if(vg && memory.isValid() )
        {
            imageID = nvgCreateImageMem(vg,imageFlags,
                                            (unsigned char*)(memory.data),
                                            memory.size);
        }
    }
    
    Image::Image(Canvas& canvas, int w, int h, const Memory& memory, int imageFlags)
    {
        m_canvas = &canvas;
        auto vg = canvas.nvgContext();
        if(vg && memory.isValid() )
        {
            imageID = nvgCreateImageRGBA(vg, w, h, imageFlags,
                                            (unsigned char*)(memory.data));
        }
    }
    
    Image::~Image()
    {
        if(m_canvas)
        {
            auto vg = m_canvas->nvgContext();
            if(vg)
                nvgDeleteImage(vg,imageID);
        }
    }
    
    void Image::update(const Memory& memory)
    {
        if(m_canvas)
        {
            auto vg = m_canvas->nvgContext();
            if(vg)
                nvgUpdateImage(vg,imageID,(const unsigned char*)(memory.data));
        }
    }
    
    void Image::size(int& width,int& height)
    {
        if(m_canvas)
        {
            auto vg = m_canvas->nvgContext();
            if(vg)
                nvgImageSize(vg,imageID,&width,&height);
        }
    }
}



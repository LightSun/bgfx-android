#include "NanoCanvas.h"
#include "nanovg/nanovg.h"
#include "lua/SkMemory.h"

namespace NanoCanvas {
    Image::Image(NVGcontext *ctx, const char *filePath, int imageFlags) {
        _ctx = ctx;
        if (strlen(filePath))
            imageID = nvgCreateImage(ctx, filePath, imageFlags);
    }

    Image::Image(NVGcontext *ctx, const SkMemory &memory, int imageFlags) {
        _ctx = ctx;
        imageID = nvgCreateImageMem(ctx, imageFlags,
                                    (unsigned char *) (memory.data),
                                    memory.size);
    }

    Image::Image(NVGcontext *ctx, int w, int h, const SkMemory &memory, int imageFlags) {
        _ctx = ctx;
        imageID = nvgCreateImageRGBA(ctx, w, h, imageFlags,
                                     (unsigned char *) (memory.data));
    }

    Image::~Image() {
        if (_ctx) {
            nvgDeleteImage(_ctx, imageID);
        }
    }

    void Image::update(const SkMemory &memory) {
        if (_ctx) {
            nvgUpdateImage(_ctx, imageID, (const unsigned char *) (memory.data));
        }
    }

    void Image::size(int &width, int &height) {
        if (_ctx) {
            nvgImageSize(_ctx, imageID, &width, &height);
        }
    }
}



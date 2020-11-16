#include "NanoCanvas.h"
#include "nanovg/nanovg.h"

namespace NanoCanvas {

    Font::Font(NVGcontext *ctx, const char *fname, const char *ttfPath) {
        if (strlen(fname) && strlen(ttfPath)) {
            face = nvgCreateFont(ctx, fname, ttfPath);
        }
        name = fname;
    }

    Font::Font(NVGcontext *ctx, const char *fname,
               const bgfx::Memory &memory, bool invalidateMem) {
        if (strlen(fname)) {
            face = nvgCreateFontMem(ctx, fname,
                                    (unsigned char *) memory.data,
                                    memory.size, invalidateMem);
        }
        name = fname;
    }
}


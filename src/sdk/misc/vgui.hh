#pragma once

namespace sdk {

    namespace vgui {

        using panel = unsigned int;
        using font = unsigned long;

        enum font_flags {
            none,
            italic = 1,
            underline = 2,
            strike = 4,
            symbol = 8,
            anti_alias = 16,
            gaussian_blur = 32,
            rotary = 64,
            drop_shadow = 128,
            additive = 256,
            outline = 512,
            custom = 1024,
            bitmap = 2048
        };

    }

}
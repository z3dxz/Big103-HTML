#pragma once

#include <thread>
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"

#include "big103.h"

#ifdef _WIN32
#include "titlebar.h"
#include "dpi_windows.h"
#else
#include "dpi_linux.h"
#endif

int CreateSFMLWindow(); 
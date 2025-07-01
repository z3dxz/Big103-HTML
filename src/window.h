#pragma once

#include <thread>

#include "big103.h"

#ifdef _WIN32
#include "titlebar.h"
#include "dpi_windows.h"
#else
#include "dpi_linux.h"
#endif

int CreateSFMLWindow(); 
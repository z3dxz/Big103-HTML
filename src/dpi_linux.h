#pragma once
#include <X11/Xlib.h>
#include <iostream>

inline float getDPI() {

    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cout << "X display can not be opened" << std::endl;
        return 96.0f; // Std DPI
    }

    int screen = DefaultScreen(display);
    
    int sx = DisplayWidth(display, screen);
    int sy = DisplayHeight(display, screen);
    
    int sxMM = DisplayWidthMM(display, screen);
    int syMM = DisplayHeightMM(display, screen);

    
    
    XCloseDisplay(display);
    
    float dpiX = static_cast<float>(sx) / (sxMM / 25.4f);  // 25.4mm in an inch
    float dpiY = static_cast<float>(sy) / (syMM / 25.4f);
    
    std::cout << dpiX << "\n";
    std::cout << dpiY << "\n";
    return (dpiX + dpiY) / 2.0f;
}

#pragma once

#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#include <SFML/Window.hpp>

inline void SetTitlebarDark(sf::RenderWindow& window) {
    HWND hwnd = window.getSystemHandle();
    BOOL enable = true;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enable, sizeof(enable));

    RECT p;
    GetWindowRect(hwnd, &p);
    
    // Update window to apply changes
    window.setSize(window.getSize()+sf::Vector2u(1,1));
    window.setSize(window.getSize()-sf::Vector2u(1,1));
}

#endif
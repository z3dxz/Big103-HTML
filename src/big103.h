#pragma once
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <regex>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
bool PushBIG103Iteration(std::string* title, std::string* artist, int* interation, int* ips);
void Initialize();
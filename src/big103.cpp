#include "big103.h"
#include <fstream>
#include <nlohmann/json.hpp>

// Now handled by CMake linked .txt file for customization

std::string webhookURL = "Undefined";
std::string sourceURL = "Undefined";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool isOnBusRide() {
    time_t now = time(0);
    tm localTime;

    #ifdef _WIN32
    localtime_s(&localTime, &now);
    #else
    localtime_r(&now, &localTime);

    #endif

    int hour = localTime.tm_hour;
    int minute = localTime.tm_min;
    int wday = localTime.tm_wday;

    if (wday == 0 || wday == 6) {
        return false;
    }

    bool isWednesday = (wday == 3);
    bool isTuesday = (wday == 2);

    if (!isWednesday) {
        // Regular schedule
        if ((hour == 6 && minute >= 41) || (hour == 7 && minute <= 34) ||
            ((hour == 14 && minute >= 30) || (hour == 15 && minute <= 20))&&(!isTuesday)) {
            return true;
        }
    }
    else {
        // Late Start
        if ((hour == 7 && minute >= 41) || (hour == 8 && minute <= 34) /*||
            (hour == 14 && minute >= 30) || (hour == 15 && minute <= 20)*/) {
            return true;
        }
    }

    return false;
}

void newsongcomeson(std::string picture);

void webhook(std::string input, std::string input2, std::string picturefield, const char* webhookurl, bool shouldAppendNotify) {
    CURL* curl;
    CURLcode res;

    bool ping = false;//isOnBusRide();

    if (ping && shouldAppendNotify) {
        input2 += " ||<@!1003695775047495710>||";
    }

    std::string content_s = "{\"content\": \"" + input2 + "\", \"username\": \"" + input + "\", \"avatar_url\": \"" + picturefield + "\"}";
    
    const char* content = content_s.c_str(); //R"aw({"content": "hi!"})aw";

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, webhookurl);
        struct curl_slist* list = NULL;
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
        res = curl_easy_perform(curl);
        curl_slist_free_all(list);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

std::string request(std::string* atitle, std::string* aartist) {

    CURL* curl = curl_easy_init();
    if(!curl) return "Curl not initialized";
    
    if(sourceURL == "ERROR" || sourceURL == "Undefined" || sourceURL == "") {
        std::cout << "There is an error with the source URL: Error: " << sourceURL << "\n";
    }

    std::string surl = sourceURL + "&t=" + std::to_string(std::time(nullptr));
    const char* url = surl.c_str();

    std::string content;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    std::string title = "Title";
    std::string artist = "Artist";

    try {
    nlohmann::json data = nlohmann::json::parse(content);

    auto& firstPerf = data["performances"][0];

    title = firstPerf["title"];
    artist = firstPerf["artist"];
    
    } catch (nlohmann::json::parse_error& e) {
        return "JSON Error";
    } catch (nlohmann::json::type_error& e) {
        return "JSON Error";
    }

    *atitle = title;
    *aartist = artist;

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return "**" + title + "** \\u00B7 " + artist;

    curl_global_cleanup();
    return "Not Found";
}



std::string requestforpicture() {

    CURL* curl = curl_easy_init();
    if(!curl) return "Curl not initialized";
    
    if(sourceURL == "ERROR" || sourceURL == "Undefined" || sourceURL == "") {
        std::cout << "There is an error with the source URL: Error: " << sourceURL << "\n";
    }

    std::string surl = sourceURL + "&t=" + std::to_string(std::time(nullptr));
    const char* url = surl.c_str();

    std::string content;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    std::string picture = "";

    try {
    nlohmann::json data = nlohmann::json::parse(content);

    auto& firstPerf = data["performances"][0];

    picture = firstPerf["mediumimage"];
    
    } catch (nlohmann::json::parse_error& e) {
        return "JSON Error";
    } catch (nlohmann::json::type_error& e) {
        return "JSON Error";
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return picture;

    curl_global_cleanup();
    return "Not Found";
}

int iter;
int ips0;

void notify() {
    std::cout << "Notifying\n";
    sf::SoundBuffer buffer;
    
    if (buffer.loadFromFile("babafixed.wav")) {
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.play();
        sf::sleep(sf::seconds(3));
    }
    else {
        std::cout << "Failed to play\n";
    }
}

void notifykind() {
    std::cout << "Notifying kindnesskey\n";
    
    sf::SoundBuffer buffer;

    if (buffer.loadFromFile("kindnesskey.wav")) {
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.play();
        sf::sleep(sf::seconds(3));
    }
    else {
        std::cout << "Failed to play\n";
    }
    
}

bool requestDone = true;
std::string k;
std::string last;


time_t timet;

std::string picture;

std::string testlast = "testlast";

void SongToFile(std::string k) {
  std::ofstream mrs;
  std::cout << "Recording MRS..\n";
  mrs.open("mrs.txt");
  mrs << k.c_str();
  mrs.close();
}

bool CheckIfSongIsInFile(std::string teststring) {
    std::ifstream file("mrs.txt");
    if(!file.is_open()) {
        // Thats fine
        std::cout << "No mrs.txt file, ignoring\n";
        return false;
    }

    std::string txt;
    while (getline(file, txt)) {
        std::cout << txt << std::endl;
    }
    file.close();

    if(txt == teststring) {
        std::cout << "Already been logged. skipping\n";
        return true;
    }

    return false;
}

std::string ReadWebhookURL() {
    std::ifstream file("webhookURL.txt");
    if(!file.is_open()) {
        // Thats fine
        std::cout << "No webhookURL.txt file, returning ERROR\n";
        return "ERROR";
    }

    std::string txt;
    while (getline(file, txt)) {
        //std::cout << txt << std::endl;
    }

    file.close();

    return txt;
}


std::string ReadSourceURL() {
    std::ifstream file("sourceURL.txt");
    if(!file.is_open()) {
        // Thats fine
        std::cout << "No sourceURL.txt file, returning ERROR\n";
        return "ERROR";
    }

    std::string txt;
    while (getline(file, txt)) {
        //std::cout << txt << std::endl;
    }

    file.close();

    return txt;
}


void Initialize(){
    sourceURL = ReadSourceURL();
    webhookURL = ReadWebhookURL();
}

bool PushBIG103Iteration(std::string* title, std::string* artist, int* iteration, int* ips) {
    bool didPlay = false;
    std::string k0 = request(title, artist);

    if (title->empty() || artist->empty()) {
        return 0;
    }

    //||<@!1003695775047495710>||
    if (k0 != testlast) {
        picture = requestforpicture();
        newsongcomeson(picture);
        testlast = k0;
    }

    k = k0;
    ips0 = 0;
    requestDone = false;

    if (!requestDone) {
        std::string req = k;
        std::string titlefield = *title + " \\u00B7";

        std::string artistfield = *artist;

        size_t found = artistfield.find(", ");
        while (found != std::string::npos) {
            artistfield.replace(found, 2, "-");
            found = artistfield.find(", ");
        }
        if (titlefield != last) {
            if (k.find("Blues T") != std::string::npos) {
                notifykind();
                didPlay = true;
            }
            if (k.find("52") != std::string::npos) {
                notify();
                didPlay = true;
            }
            if (k.find("Last Christmas") != std::string::npos) {
                notify();
                didPlay = true;
            }
            if (k.find("Ulterior Motives") != std::string::npos) {
                notify();
                didPlay = true;
            }
            if (k.find("Beatboy") != std::string::npos) {
                notify();
                didPlay = true;
            }
            if (k.find("Hazy Shade Of Winter") != std::string::npos) {
                notifykind();
                didPlay = true;
            }
            if ((k.find("Yes") != std::string::npos) && (!((k.find("Owner") != std::string::npos)))) {
                notify();
                didPlay = true;
            }
            if (k.find("Barbie Girl") != std::string::npos) {
                notify();
                didPlay = true;
            }
            if(!CheckIfSongIsInFile(k)) {
                std::cout << "Logging..";

                if(webhookURL == "ERROR" || webhookURL == "Undefined" || webhookURL == "") {
                    std::cout << "There is an error with the webhook URL: Error: " << sourceURL << "\n";
                }

                webhook(titlefield, artistfield, picture, webhookURL.c_str(), true);
            }
            SongToFile(k);
            last = titlefield;
        }
        requestDone = true;
    }

    iter++; ips0++;
    *iteration = iter;
    *ips = ips0;
    return didPlay;
}
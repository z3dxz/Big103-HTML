#include "big103.h"

std::string sourceURL = "https://www.audacy.com/big103boston";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool isOnBusRide() {
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

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
        if ((hour == 6 && minute >= 45) || (hour == 7 && minute <= 38) ||
            ((hour == 14 && minute >= 35) || (hour == 15 && minute <= 25))&&(!isTuesday)) {
            return true;
        }
    }
    else {
        // Late Start
        if ((hour == 7 && minute >= 45) || (hour == 8 && minute <= 38) ||
            (hour == 14 && minute >= 35) || (hour == 15 && minute <= 25)) {
            return true;
        }
    }

    return false;
}

void newsongcomeson(std::string picture);
const char* WEBHOOKmain = "https://discord.com/api/webhooks/1162926636207968346/UIiAmkjE6C7wYw_4EPrcXbYR-GIHJbuSKweWmSjJF0svulWhkzLpJkHxacRjGfhri0Ak";
//const char* WEBHOOKmain = "https://discord.com/api/webhooks/1319083487910694912/VpZvswc9gXRjhZ8P2TA2MFha27qvl0A-M3IsHiK9iH5X4vlwaBioUS7DnyfU4f1vIg8K"; // magic
const char* WEBHOOKTest = "https://discord.com/api/webhooks/1205359534500085791/aaB2dZt5WsJqFDKPlwYussFN_knYTmIaZ7lk1jvJOZ7E7-N3MA2un2ZDgGtsOcmw78Qv";

void webhook(std::string input, std::string input2, std::string picturefield, const char* webhookurl, bool shouldAppendNotify) {
    CURL* curl;
    CURLcode res;

    bool ping = isOnBusRide();

    if (ping && shouldAppendNotify) {
        input2 += " ||<@!1003695775047495710>||";
    }

    std::string content_s = "{\"content\": \"" + input2 + "\", \"username\": \"" + input + "\", \"avatar_url\": \"" + picturefield + "\"}";
    std::cout << "\n[][][]" << content_s << "[][][]\n";
    const char* content = content_s.c_str(); //R"aw({"content": "hi!"})aw";

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, webhookurl);

        // create a curl list of header rows:
        struct curl_slist* list = NULL;

        // add Content-Type to the list:
        list = curl_slist_append(list, "Content-Type: application/json");

        // set this list as HTTP headers:
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);

        res = curl_easy_perform(curl);
        curl_slist_free_all(list);     // and finally free the list

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}


std::string htmlDecode(const std::string& html) {
    std::string decoded;
    size_t pos = 0;
    while (pos < html.length()) {
        size_t ampPos = html.find('&', pos);
        if (ampPos == std::string::npos) {
            decoded += html.substr(pos);
            break;
        }
        decoded += html.substr(pos, ampPos - pos);
        size_t semicolonPos = html.find(';', ampPos);
        if (semicolonPos != std::string::npos) {
            std::string entity = html.substr(ampPos, semicolonPos - ampPos + 1);
            if (entity == "&amp;") {
                decoded += '&';
            }
            else if (entity == "&apos;") {
                decoded += '\'';
            }
            else if (entity == "&quot;") {
                decoded += '"';
            }
            else if (entity == "&lt;") {
                decoded += '<';
            }
            else if (entity == "&gt;") {
                decoded += '>';
            }
            else {
                // Handle other entities as needed
                decoded += entity;
            }
            pos = semicolonPos + 1;
        }
        else {
            decoded += html.substr(pos);  // If no ending semicolon, treat it as text
            break;
        }
    }
    return decoded;
}

std::vector<std::string> arr;

void AppendSong(std::string song) {
    arr.push_back(song);
    if (arr.size() > 20) {
        arr.erase(arr.begin());
    }
}

bool CheckIfRecent(std::string song) {
    return (std::find(arr.begin(), arr.end(), song) != arr.end());
}

bool CheckSongRequirementsY(std::string song) {
    if (song.find("BIG 103") != std::string::npos) {
        return false;
    }
    if (song.find("Free Consult") != std::string::npos) {
        return false;
    }
    return true;
}


bool CheckSongRequirements(std::string song) {
    if (song.find("BIG 103") != std::string::npos) {
        return false;
    }
    if (song.find("Free Consult") != std::string::npos) {
        return false;
    }
    if (CheckIfRecent(song)) {
        return false;
    }
    return true;
}


std::string makeAlphanumeric(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (isalnum(c) || c == ' ') {
            if (c == ' ') {
                result += '+';
            }
            else {
                result += c;
            }
        }
    }
    return result;
}


std::string replaceSpacesWithPlus(std::string str) {
    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == ' ') {
            str[i] = '+';
        }
    }
    return str;
}

std::string convertNonAlphanumericToPluses(const std::string& input) {
    std::string result = input;
    for (char& c : result) {
        if (!isalnum(c) && c != ' ') {
            c = '+';
        }
    }
    return result;
}

std::string getyear(std::string song) {

    CURL* curl = curl_easy_init();
    std::string numbers = "0000";
    if (curl) {

        std::string k = "https://www.google.com/search?q=";
        std::string f = k + "What+Year+Was+" + makeAlphanumeric(song);
        const char* url = f.c_str();

        // Initialize a string to store the received HTML
        std::string html;

        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        std::regex pattern("BNeawe iBp4i AP7Wnd\">\\d{4}"); // Match "lala" followed by four digits

        std::smatch match;
        if (std::regex_search(html, match, pattern)) {
            std::string lalaWithNumbers = match[0];
            numbers = lalaWithNumbers.substr(21); // Get the four numbers

            //std::cout << "Found: " << lalaWithNumbers << std::endl;
            //std::cout << "Numbers: " << numbers << std::endl;
        }
        else {
            //std::cout << "No matching pattern found." << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return numbers;
}



std::string request(std::string* atitle, std::string* aartist) {


    CURL* curl = curl_easy_init();
    if (curl) {

        int randomNumber = std::rand();
        std::string randomString = std::to_string(randomNumber);
        std::string f = sourceURL + "?" + randomString;
        const char* url = f.c_str();

        // Initialize a string to store the received HTML
        std::string html;

        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        std::string title;
        std::string artist;

        {
            std::string searchString = "now playing";
            std::string startTag = "<h3>";
            std::string endTag = "</h3>";

            size_t startPos = html.find(searchString);
            if (startPos != std::string::npos) {
                size_t h3Pos = html.find(startTag, startPos);
                if (h3Pos != std::string::npos) {
                    size_t endPos = html.find(endTag, h3Pos);
                    if (endPos != std::string::npos) {
                        std::string result = html.substr(h3Pos + 4, endPos - h3Pos + endTag.length() - 9);
                        title = result;
                    }
                }
            }
        }

        {

            std::string searchString = "now playing";
            std::string startTag = "<span>";
            std::string endTag = "</span>";

            size_t startPos = html.find(searchString);
            if (startPos != std::string::npos) {
                size_t h3Pos = html.find(startTag, startPos);
                if (h3Pos != std::string::npos) {
                    size_t endPos = html.find(endTag, h3Pos);
                    if (endPos != std::string::npos) {
                        std::string result = html.substr(h3Pos + 6, endPos - h3Pos + endTag.length() - 13);
                        artist = result;
                    }
                }
            }
        }

        std::string decodedTitle = htmlDecode(title);
        std::string decodedArtist = htmlDecode(artist);

        *atitle = decodedTitle;
        *aartist = decodedArtist;

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return "**" + decodedTitle + "** \\u00B7 " + decodedArtist;
    }

    curl_global_cleanup();
    return "Not Found";
}



std::string requestforpicture() {

    CURL* curl = curl_easy_init();
    if (curl) {

        int randomNumber = std::rand();
        std::string randomString = std::to_string(randomNumber);
        std::string k = sourceURL;
        std::string f = k + + "?" + randomString;
        const char* url = f.c_str();

        // Initialize a string to store the received HTML
        std::string html;

        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        std::string picture;

        {
            std::string searchString = "now playing";
            std::string startTag = "<img src=\"";
            std::string endTag = "\" alt";

            size_t startPos = html.find(searchString);
            if (startPos != std::string::npos) {
                size_t h3Pos = html.find(startTag, startPos);
                if (h3Pos != std::string::npos) {
                    size_t endPos = html.find(endTag, h3Pos);
                    if (endPos != std::string::npos) {
                        std::string result = html.substr(h3Pos + 10, endPos - h3Pos + endTag.length() - 15);
                        picture = result;
                    }
                }
            }
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return picture;
    }
    curl_global_cleanup();

    return "Not Found";
}

int iter;
int ips0;

void notify() {
    //PlaySound("babafixed.wav", NULL, SND_FILENAME | SND_ASYNC);
    std::cout << "Notifying baba";
    sf::SoundBuffer buffer;
    
    if (buffer.loadFromFile("babafixed.wav")) {
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.play();
        sf::sleep(sf::seconds(3));
    }
    else {
        Beep(5000, 500);
        std::cout << "Failed to play";
    }
}

void notifykind() {
    std::cout << "Notifying kindnesskey";
    //PlaySound("kindnesskey.wav", NULL, SND_FILENAME | SND_ASYNC);
    
    sf::SoundBuffer buffer;

    if (buffer.loadFromFile("kindnesskey.wav")) {
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.play();
        sf::sleep(sf::seconds(3));
    }
    else {
        std::cout << "Failed to play";
    }
    
}

bool requestDone = true;
std::string k;
std::string last;


time_t timet;

std::string picture;

std::string testlast = "testlast";

bool PushBIG103Iteration(std::string* title, std::string* artist, int* iteration, int* ips) {
    bool didPlay = false;
    std::string k0 = request(title, artist);

    if (title->empty() || artist->empty()) {
        return 0;
    }


    //||<@!1003695775047495710>||
    if (k0 != testlast) {

        std::cout << ":::::::\n";
        picture = requestforpicture();
        std::cout << picture << "\n:::::::::::::::";
        newsongcomeson(picture);
        webhook("BIG103", k0, "", WEBHOOKTest, false);
        testlast = k0;
    }


    if (CheckSongRequirements(k0)) {
        k = k0;
        ips0 = 0;
        AppendSong(k);
        requestDone = false;
    }

    if (!requestDone && ips0 > 2) {
        //Beep(500, 500);
        if (CheckSongRequirementsY(k0)) {
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
            if (k.find("Buggles") != std::string::npos) {
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

            std::string year = getyear(std::string(*artist + *title));
            std::string req = "*" + year + "* | " + k;
            std::string titlefield = *title + " \\u00B7 " + year;

            if (year == "0000") {
                titlefield = *title;
            }

            std::string artistfield = *artist;

            size_t found = artistfield.find(", ");
            while (found != std::string::npos) {
                artistfield.replace(found, 2, "-");
                found = artistfield.find(", ");
            }
            if (titlefield != last) {
                webhook(titlefield, artistfield, picture, WEBHOOKmain, true);
                last = titlefield;
            }
            requestDone = true;
        }

    }

    iter++; ips0++;
    *iteration = iter;
    *ips = ips0;
    return didPlay;
}
#include "window.h"

sf::Text ns_txt;
sf::Text title_field;
sf::Text artist_field;
sf::Text info_txt;
sf::RectangleShape bk;
sf::RectangleShape albumart;

int defwidth = 850;
int defheight = 180;

int width = defwidth;
int height = defheight;

bool isRunning = true;

int plays1 = 0;

void ExecuteThread() {
    while (isRunning) {
        // Your thread's work here.
        std::string xtitle = "Song Title";
        std::string xartist = "Song Artist";
        int iterations = 0;
        int ips = 0;
        bool didp = PushBIG103Iteration(&xtitle, &xartist, &iterations, &ips);
        if (didp) {
            plays1++;
        }

        std::string extras = "Iterations: " + std::to_string(iterations) + "   " + "IPS: " + std::to_string(ips) + "   " + "Notified: " + std::to_string(plays1);

        title_field.setString(xtitle.c_str());
        artist_field.setString(xartist.c_str());
        info_txt.setString(extras.c_str());
        sf::sleep(sf::seconds(1));
    }
}


void SetScale(float scale) {
    float aArtOffset = 135.0f;
    ns_txt.setPosition((int)(15.0f*scale), (int)(10.0f*scale));
    title_field.setPosition((int)((5.0f + aArtOffset) *scale), (int)(58.0f*scale));
    artist_field.setPosition((int)((5.0f + aArtOffset) * scale), (int)(98.0f * scale));
    info_txt.setPosition((int)((5.0f + aArtOffset) * scale), (int)(135.0f * scale));
    albumart.setPosition(sf::Vector2f((int)(15.0f * scale), (int)(55.0f * scale)));
    
    ns_txt.setCharacterSize((int)(25.0f * scale));
    title_field.setCharacterSize((int)(32.0f * scale));
    artist_field.setCharacterSize((int)(22.0f * scale));
    info_txt.setCharacterSize((int)(14.0f * scale));
    bk.setSize(sf::Vector2f(width, height));
    albumart.setSize(sf::Vector2f((int)(110.0f*scale), (int)(110.0f * scale)));
}

const std::string bkShader = R"(

    uniform float iTime;
    const float gamma = 0.3;

    float gc(float cv) {
        return pow(cv, 1.0 / gamma);
    }

    void main()  {
        vec2 position = gl_FragCoord.xy / vec2(800.0, 600.0);
        position = (position - 0.5) * 2.0;
        float green = 0.2 + 0.3 * cos(iTime + position.x * 5.0 - position.y * 3.0);
        float blue  = 0.2 + 0.3 * sin(iTime + position.x * 4.0 + position.y * 2.0);
        green = gc(green);
        blue = gc(blue);

        gl_FragColor = vec4(0, green, blue, 1.0);
    }

)";

bool load_album_art(const std::string& url, sf::Texture& texture) {
    CURL* curl;
    CURLcode res;
    std::string imageData; // Using std::string to hold binary image data

    // Initialize cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_cleanup(curl);
    }

    // Clean up cURL global environment
    curl_global_cleanup();

    // Load image data into texture from std::string
    if (texture.loadFromMemory(imageData.data(), imageData.size())) {
        return true;
    }
    else {
        std::cerr << "Failed to load texture from memory." << std::endl;
    }
    return false;
}

sf::Texture* a_art;

void newsongcomeson(std::string picture) {
    bool st = load_album_art(picture, *a_art);
    if (st) {;
        albumart.setTexture(a_art);

    }
}

int CreateSFMLWindow(){

    a_art = new sf::Texture();
    std::string title = "Big103-HTML for Unix";

    #ifdef _WIN32
    title = "Big103-HTML for Windows";
    #endif

    sf::RenderWindow window(sf::VideoMode(defwidth, defheight), title.c_str(), sf::Style::Default);
   
    
	// Poll Events
	#ifdef _WIN32
	SetTitlebarDark(window);
	#endif

    window.setFramerateLimit(60);
    
    sf::Font font;
    if (!font.loadFromFile("font.ttf"))
    {
        printf("no");
    }

    ns_txt.setFont(font);
    ns_txt.setString("Notification System");
    ns_txt.setFillColor(sf::Color::Yellow);

    title_field.setFont(font);
    title_field.setFillColor(sf::Color::White);

    artist_field.setFont(font);
    artist_field.setFillColor(sf::Color(255, 255, 255, 128));

    info_txt.setFont(font);
    info_txt.setFillColor(sf::Color(255,255,255,90));


    sf::Shader shader;
    if (!shader.loadFromMemory(bkShader, sf::Shader::Fragment))
    {
        std::cout << "Error loading shader\n" << std::endl;
        return -1;
    }

    SetScale(1.0f);

    albumart.setOutlineColor(sf::Color(0, 0, 0));
    albumart.setOutlineThickness(2);

    std::thread th(ExecuteThread);

    sf::Clock clock;

    float dpi = getDPI();
    std::cout << "Detected DPI: " << dpi << "\n";
    float scale_dpi = ((dpi*100.0f)/96.0f)/100.0f;

    sf::Vector2u s = window.getSize();
    sf::Uint32 n_xs = (uint32_t)((float)s.x*scale_dpi);
    sf::Uint32 n_ys = (uint32_t)((float)s.y*scale_dpi);

    window.setSize({n_xs, n_ys});

    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
            case sf::Event::Closed: {

                window.close();
                std::exit(0);
                break;
            }
            case sf::Event::Resized: {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                width = event.size.width;
                height = event.size.height;
                break;
            }
            }
        }

        float time = clock.getElapsedTime().asSeconds();
        shader.setUniform("iTime", time);

        float scale = (float)(window.getSize().y) / (float)defheight;

        window.clear();
        
        SetScale(scale);

        window.draw(bk, &shader);
        window.draw(ns_txt);
        window.draw(title_field);
        window.draw(artist_field);
        window.draw(info_txt);
        window.draw(albumart);

        window.display();
    }
    th.join(); // Wait for the thread to finish

    delete a_art;
	return 0;
}
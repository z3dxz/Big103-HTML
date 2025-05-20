
#include <thread>
#include "big103.h"

#ifdef _WIN32
#include "titlebar.h"
#endif

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

// GLSL SHADER: https://www.shadertoy.com/view/4ljGD1 CREDI GOES TO ADOB
const std::string fragmentShaderCode = R"(
uniform sampler2D iChannel0;
uniform vec2 iResolution;
uniform float iTime;

float squared(float value) { return value * value; }

float getAmp(float frequency) { return texture2D(iChannel0, vec2(frequency / 512.0, 0.0)).x; }

float getWeight(float f) {
    return (getAmp(f - 2.0) + getAmp(f - 1.0) + getAmp(f + 2.0) + getAmp(f + 1.0) + getAmp(f)) / 5.0;
}

void main()
{    
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uvTrue = fragCoord.xy / iResolution.xy;
    vec2 uv = -1.0 + 2.0 * uvTrue;
    
    float lineIntensity;
    float glowWidth;
    vec3 color = vec3(0.0);
    
    for(float i = 0.0; i < 5.0; i++) {
        
        uv.y += (0.2 * sin(uv.x + i / 7.0 - iTime * 0.6));
        float Y = uv.y + getWeight(squared(i) * 20.0) *
            (texture2D(iChannel0, vec2(uvTrue.x, 1.0)).x - 0.5);
        lineIntensity = 0.4 + squared(1.6 * abs(mod(uvTrue.x + i / 1.3 + iTime, 2.0) - 1.0));
        glowWidth = abs(lineIntensity / (100.0 * Y));
        color += vec3(glowWidth * (2.0 + sin(iTime * 0.13)),
                      glowWidth * (2.0 - sin(iTime * 0.23)),
                      glowWidth * (2.0 - cos(iTime * 0.19)));
    }    
    
    gl_FragColor = vec4(color, 1.0) * .4;
}
)";

const std::string test2 = R"(
#version 330 core

uniform vec2 iResolution;
uniform float iTime;

float opSmoothUnion( float d1, float d2, float k )
{
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float sdSphere( vec3 p, float s )
{
    return length(p) - s;
} 

float map(vec3 p)
{
    float d = 2.0;
    for (int i = 0; i < 16; i++) {
        float fi = float(i);
        float time = iTime * (fract(fi * 412.531 + 0.513) - 0.5) * 2.0;
        d = opSmoothUnion(
            sdSphere(p + sin(time + fi * vec3(52.5126, 64.62744, 632.25)) * vec3(2.0, 2.0, 0.8), mix(0.5, 1.0, fract(fi * 412.531 + 0.5124))),
            d,
            0.4
        );
    }
    return d;
}

vec3 calcNormal( in vec3 p )
{
    const float h = 1e-5; // or some other value
    const vec2 k = vec2(1, -1);
    return normalize(
        k.xyy * map(p + k.xyy * h) + 
        k.yyx * map(p + k.yyx * h) + 
        k.yxy * map(p + k.yxy * h) + 
        k.xxx * map(p + k.xxx * h)
    );
}

void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution;

    // screen size is 6m x 6m
    vec3 rayOri = vec3((uv - 0.5) * vec2(iResolution.x / iResolution.y, 1.0) * 6.0, 3.0);
    vec3 rayDir = vec3(0.0, 0.0, -1.0);
    
    float depth = 0.0;
    vec3 p;

    for (int i = 0; i < 64; i++) {
        p = rayOri + rayDir * depth;
        float dist = map(p);
        depth += dist;
        if (dist < 1e-6) {
            break;
        }
    }

    depth = min(6.0, depth);
    vec3 n = calcNormal(p);
    float b = max(0.0, dot(n, vec3(0.577)));
    vec3 col = (0.5 + 0.5 * cos((b + iTime * 3.0) + uv.xyx * 2.0 + vec3(0, 2, 4))) * (0.85 + b * 0.35);
    col *= exp(-depth * 0.15);

    // maximum thickness is 2m in alpha channel
    gl_FragColor = vec4(col, 1.0 - (depth - 0.5) / 2.0);
}
)";

const std::string fragmentShaderCode2 = R"(

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
        // Set the fragment color with smooth gradient
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

int main() {

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
    if (!shader.loadFromMemory(fragmentShaderCode2, sf::Shader::Fragment))
    {
        std::cout << "Error loading shader" << std::endl;
        return -1;
    }

    SetScale(1.0f);

    albumart.setOutlineColor(sf::Color(0, 0, 0));
    albumart.setOutlineThickness(2);

    std::thread th(ExecuteThread);

    sf::Clock clock;

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
        shader.setUniform("iResolution", sf::Glsl::Vec2(window.getSize()));
        

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
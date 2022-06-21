/*
   Real time Weather station example for e-radionica.com Inkplate 10
   For this example you will need only USB cable and Inkplate 10.
   Select "Inkplate 10(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 10(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 10 to display API data,
   e.g. Metaweather public weather API, and weatherstack for real time data

   IMPORTANT:
   Make sure to change your desired city, timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Time zone for adding hours
int timeZone = 2;

// City name to de displayed on the bottom
char city[128] = "OSIJEK";

// Coordinates sent to the api
char lon[] = "45.5510548";
char lat[] = "18.5947808";

// Change to your wifi ssid and password
char ssid[] = "";
char pass[] = "";

// Uncomment this for MPH and Fahrenheit output, also uncomment it in the begining of Network.cpp
// #define AMERICAN

// Change to your api key, if you don't have one, head over to:
// https://openweathermap.org/guide , register and copy the key provided
char apiKey[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Header file for easier code readability
#include "Network.h"

// Including fonts used
#include "Fonts/Roboto_Light_120.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"

// Including icons generated by the py file
#include "icons.h"

// Delay between API calls, about 1000 per month, which is the free tier limit
#define DELAY_MS 267800L

// Inkplate object
Inkplate display(INKPLATE_1BIT);

// All our network functions are in this object, see Network.h
Network network;

// Contants used for drawing icons
char abbrs[32][32] = {"01d", "02d", "03d", "04d", "09d", "10d", "11d", "13d", "50d",
                      "01n", "02n", "03n", "04n", "09n", "10n", "11n", "13n", "50n"};
const uint8_t *logos[18] = {
    icon_01d, icon_02d, icon_03d, icon_04d, icon_09d, icon_10d, icon_11d, icon_13d, icon_50d,
    icon_01n, icon_02n, icon_03n, icon_04n, icon_09n, icon_10n, icon_11n, icon_13n, icon_50n,
};

const uint8_t *s_logos[18] = {
    icon_s_01d, icon_s_02d, icon_s_03d, icon_s_04d, icon_s_09d, icon_s_10d, icon_s_11d, icon_s_13d, icon_s_50d,
    icon_s_01n, icon_s_02n, icon_s_03n, icon_s_04n, icon_s_09n, icon_s_10n, icon_s_11n, icon_s_13n, icon_s_50n,
};

// Variables for storing temperature
char temps[4][8] = {
    "0F",
    "0F",
    "0F",
    "0F",
};

// Variables for storing hour strings
char hours[4][8] = {
    "",
    "",
    "",
    "",
};

// Variable for counting partial refreshes
RTC_DATA_ATTR long refreshes = 0;

// Constant to determine when to full update
const int fullRefresh = 10;

// Variables for storing current time and weather info
char currentTemp[16] = "0F";
char currentWind[16] = "0m/s";

char currentTime[16] = "9:41";

char currentWeather[32] = "-";
char currentWeatherAbbr[8] = "01d";

char abbr1[16];
char abbr2[16];
char abbr3[16];
char abbr4[16];

// functions defined below
void drawWeather();
void drawCurrent();
void drawTemps();
void drawCity();
void drawTime();


// otherwise your code could hang out when you send code to your Inkplate.
// You can easily check if your Inkplate has second MCP by turning it over and 
// if there is missing chip near place where "MCP23017-2" is written, but if there is
// chip soldered, you don't have to uncomment line and use external MCP I/O expander

void setup()
{
    // Begin serial and display
    Serial.begin(115200);
    display.begin();

    // Initial cleaning of buffer and physical screen
    display.clearDisplay();
    display.display();

    // Calling our begin from network.h file
    network.begin(city);

    // Welcome screen
    display.setCursor(215, 400);
    display.setTextSize(3);
    display.print(F("Welcome to Inkplate 10 weather example!"));
    display.display();

    // Wait a bit before proceeding
    delay(5000);

    // Clear display
    display.clearDisplay();

    // Get all relevant data, see Network.cpp for info
    network.getTime(currentTime);
    if (refreshes % fullRefresh == 0)
    {
        while (!network.getData(city, temps[0], temps[1], temps[2], temps[3], currentTemp, currentWind, currentTime,
                                currentWeather, currentWeatherAbbr, abbr1, abbr2, abbr3, abbr4))
        {
            Serial.println("Retrying fetching data!");
            delay(5000);
        }
        network.getHours(hours[0], hours[1], hours[2], hours[3]);
    }

    // Draw data, see functions below for info
    drawWeather();
    drawCurrent();
    drawTemps();
    drawCity();
    drawTime();

    // Refresh full screen every fullRefresh times, defined above
    if (refreshes % fullRefresh == 0)
        display.display();
    else
        display.partialUpdate();

    // Go to sleep before checking again
    ++refreshes;
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
}

// Function for drawing weather info
void drawWeather()
{
    // Searching for weather state abbreviation
    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbrs[i], currentWeatherAbbr) == 0)
            display.drawBitmap(75, 62, logos[i], 152, 152, BLACK);
    }

    // Draw weather state
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);
    display.setCursor(60, 305);
    display.println(currentWeather);
}

// Function for drawing current time
void drawTime()
{
    // Drawing current time
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(1200 - 20 * strlen(currentTime), 35);
    display.println(currentTime);
}

// Function for drawing city name
void drawCity()
{
    // Drawing city name
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(600 - 9 * strlen(city), 795);
    display.println(city);
}

// Function for drawing temperatures
void drawTemps()
{
    // Drawing 4 black rectangles in which temperatures will be written
    int rectWidth = 225;
    int rectSpacing = (1200 - rectWidth * 4) / 5;

    display.fillRect(1 * rectSpacing + 0 * rectWidth, 412, rectWidth, 330, BLACK);
    display.fillRect(2 * rectSpacing + 1 * rectWidth, 412, rectWidth, 330, BLACK);
    display.fillRect(3 * rectSpacing + 2 * rectWidth, 412, rectWidth, 330, BLACK);
    display.fillRect(4 * rectSpacing + 3 * rectWidth, 412, rectWidth, 330, BLACK);

    int textMargin = 6;

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 450 + textMargin + 40);
    display.println(hours[0]);

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 450 + textMargin + 40);
    display.println(hours[1]);

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 450 + textMargin + 40);
    display.println(hours[2]);

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, 450 + textMargin + 40);
    display.println(hours[3]);

    // Drawing temperature values into black rectangles
    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 450 + textMargin + 120);
    display.print(temps[0]);
    display.println(F("C"));

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 450 + textMargin + 120);
    display.print(temps[1]);
    display.println(F("C"));

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 450 + textMargin + 120);
    display.print(temps[2]);
    display.println(F("C"));

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, 450 + textMargin + 120);
    display.print(temps[3]);
    display.println(F("C"));

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr1, abbrs[i]) == 0)
            display.drawBitmap(1 * rectSpacing + 0 * rectWidth + textMargin, 450 + textMargin + 150, s_logos[i], 48, 48,
                               WHITE, BLACK);
    }

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr2, abbrs[i]) == 0)
            display.drawBitmap(2 * rectSpacing + 1 * rectWidth + textMargin, 450 + textMargin + 150, s_logos[i], 48, 48,
                               WHITE, BLACK);
    }

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr3, abbrs[i]) == 0)
            display.drawBitmap(3 * rectSpacing + 2 * rectWidth + textMargin, 450 + textMargin + 150, s_logos[i], 48, 48,
                               WHITE, BLACK);
    }

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr4, abbrs[i]) == 0)
            display.drawBitmap(4 * rectSpacing + 3 * rectWidth + textMargin, 450 + textMargin + 150, s_logos[i], 48, 48,
                               WHITE, BLACK);
    }
}

// Current weather drawing function
void drawCurrent()
{
    // Drawing current information

    // Temperature:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(365, 206);
    display.print(currentTemp);

    int x = display.getCursorX();
    int y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);

#ifdef AMERICAN
    display.println(F("F"));
#else
    display.println(F("C"));
#endif

    // Wind:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(720, 206);
    display.print(currentWind);

    x = display.getCursorX();
    y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);

#ifdef AMERICAN
    display.println(F("mph"));
#else
    display.println(F("m/s"));
#endif

    // Labels underneath
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(322, 288);
    display.println(F("TEMPERATURE"));

    display.setCursor(750, 288);
    display.println(F("WIND SPEED"));
}

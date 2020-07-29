/*
   Weather station example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display API data,
   e.g. Metaweather public weather API

   IMPORTANT:
   Make sure to change your desired city, timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   28 July 2020 by e-radionica.com
*/

// ---------- CHANGE HERE  -------------:

// Time zone for adding hours
int timeZone = 2;

// City search query
char city[128] = "ZAGREB";

// Change to your wifi ssid and password
char *ssid = "";
char *pass = "";

// ----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Header file for easier code readability
#include "Network.h"

// Including fonts used
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_120.h"

// Including icons generated by the py file
#include "icons.h"

// Delay between API calls
#define DELAY_MS 15000

// Inkplate object
Inkplate display(INKPLATE_1BIT);

// All our network functions are in this object, see Network.h
Network network;

// Contants used for drawing icons
char abbrs[32][16] = {"sn", "sl", "h", "t", "hr", "lr", "s", "hc", "lc", "c"};
const uint8_t *logos[16] = {icon_sn, icon_sl, icon_h, icon_t, icon_hr, icon_lr, icon_s, icon_hc, icon_lc, icon_c};

// Variables for storing temperature
char temps[8][4] = {
    "0F",
    "0F",
    "0F",
    "0F",
};

// Variables for storing days of the week
char days[8][4] = {
    "",
    "",
    "",
    "",
};

// Variable for counting partial refreshes
long refreshes = 0;

// Constant to determine when to full update
const int fullRefresh = 10;

// Variables for storing current time and weather info
char currentTemp[16] = "0F";
char currentWind[16] = "0m/s";

char currentTime[16] = "9:41";

char currentWeather[32] = "-";
char currentWeatherAbbr[8] = "th";

// functions defined below
void drawWeather();
void drawCurrent();
void drawTemps();
void drawCity();
void drawTime();

void setup()
{
    // Begin serial and display
    Serial.begin(115200);
    display.begin();

    // Initial cleaning of buffer and physical screen
    display.clearDisplay();
    display.clean();

    // Calling our begin from network.h file
    network.begin(city);

    // If city not found, do nothing
    if (network.location == -1)
    {
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("City not in Metaweather Database"));
        display.display();
        while (1)
            ;
    }

    // Welcome screen
    display.setCursor(50, 290);
    display.setTextSize(3);
    display.print(F("Welcome to Inkplate 6 weather example!"));
    display.display();

    // Wait a bit before proceeding
    delay(5000);
}

void loop()
{
    // Clear display
    display.clearDisplay();

    // Get all relevant data, see Network.cpp for info
    network.getTime(currentTime);
    network.getDays(days[0], days[1], days[2], days[3]);
    network.getData(city, temps[0], temps[1], temps[2], temps[3], currentTemp, currentWind, currentTime, currentWeather, currentWeatherAbbr);

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
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
    (void)esp_light_sleep_start();
    ++refreshes;
}

// Function for drawing weather info
void drawWeather()
{
    // Searching for weather state abbreviation
    for (int i = 0; i < 10; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbrs[i], currentWeatherAbbr) == 0)
            display.drawBitmap(50, 50, logos[i], 152, 152, BLACK);
    }

    // Draw weather state
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);
    display.setCursor(40, 270);
    display.println(currentWeather);
}

// Function for drawing current time
void drawTime()
{
    // Drawing current time
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(800 - 20 * strlen(currentTime), 35);
    display.println(currentTime);
}

// Function for drawing city name
void drawCity()
{
    // Drawing city name
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(400 - 9 * strlen(city), 570);
    display.println(city);
}

// Function for drawing temperatures
void drawTemps()
{
    // Drawing 4 black rectangles in which temperatures will be written
    int rectWidth = 150;
    int rectSpacing = (800 - rectWidth * 4) / 5;

    display.fillRect(1 * rectSpacing + 0 * rectWidth, 300, rectWidth, 220, BLACK);
    display.fillRect(2 * rectSpacing + 1 * rectWidth, 300, rectWidth, 220, BLACK);
    display.fillRect(3 * rectSpacing + 2 * rectWidth, 300, rectWidth, 220, BLACK);
    display.fillRect(4 * rectSpacing + 3 * rectWidth, 300, rectWidth, 220, BLACK);

    int textMargin = 6;

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 300 + textMargin + 70);
    display.print(days[0]);
    display.println(F("C"));

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 300 + textMargin + 70);
    display.print(days[1]);
    display.println(F("C"));

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 300 + textMargin + 70);
    display.print(days[2]);
    display.println(F("C"));

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, 300 + textMargin + 70);
    display.print(days[3]);
    display.println(F("C"));

    // Drawing temperature values into black rectangles
    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 300 + textMargin + 160);
    display.println(temps[0]);

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 300 + textMargin + 160);
    display.println(temps[1]);

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 300 + textMargin + 160);
    display.println(temps[2]);

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, 300 + textMargin + 160);
    display.println(temps[3]);
}

// Current weather drawing function
void drawCurrent()
{
    // Drawing current information

    // Temperature:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(245, 150);
    display.print(currentTemp);

    int x = display.getCursorX();
    int y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("C"));

    // Wind:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(480, 150);
    display.print(currentWind);

    x = display.getCursorX();
    y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("m/s"));

    // Labels underneath
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(215, 210);
    display.println(F("TEMPERATURE"));

    display.setCursor(500, 210);
    display.println(F("WIND SPEED"));
}

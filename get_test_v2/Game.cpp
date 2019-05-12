#include "Game.h"
//#include "HomePage.h"
#include "LaserString.h"
#include "RectNote.h"

#include <string>
#include <string.h>
#include <WiFi.h>
#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <DFRobotDFPlayerMini.h>

#define HOME_STATE 0
#define USER_SELECT_STATE 1
#define SONG_SELECT_STATE 2
#define SONG_GET_STATE 3
#define GAME_PLAY_STATE 4
#define LEADERBOARD_STATE 5

static void Game::setUp(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player) 
{
    //sets up all LED pins, phototransistor pins
    //initializes score and game state
    tft = input_tft;
    mp3_player = input_tft;

    a_string = LaserString(a_LED_pin, a_analog_pin);
    s_string = LaserString(s_LED_pin, s_analog_pin);
    d_string = LaserString(d_LED_pin, d_analog_pin);
    f_string = LaserString(f_LED_pin, f_analog_pin);

    RectNote a_rects[100];
    a_start = 0;
    a_end = 0;
    RectNote s_rects[100];
    s_start = 0;
    s_end = 0;
    RectNote d_rects[100];
    d_start = 0;
    d_end = 0;
    RectNote f_rects[100];
    f_start = 0;
    f_end = 0;

    std::string user = "";
    std::string song_name = "";
    std::string artist_name = "";
    song = 0;  //number in SD card
    song_len = 0;

    score = 0;
    state = 0;
}

static void Game::gamePlay(int song_elapsed, char* request_buffer, char* reponse_buffer)
{
    //state machine going through total game play
    if (state == HOME_STATE)
    {
        //starting state = home page / title screen
        //HomePage::display();
        state = USER_SELECT_STATE;
    } else if (state == USER_SELECT_STATE)
    {
        //enter user name page
        //user = UserSelect::getName();
        state = SONG_SELECT_STATE;
    } else if (state == SONG_SELECT_STATE)
    {
        //get the name, artist, song number
        // song_name = get song;
        // artist_name = get artist;
        // song = get song number;
        if select finished
        {
            getSongData(request_buffer);
            state = SONG_GET_STATE;
        }
    } else if (state == SONG_GET_STATE)
    {
        //get all data necessary for game play for song
        char note_arr[500] = {0};
        float note_time_arr[500] = {0};
        float duration_arr[500] = {0};

        //parse the data and get note times
        parseSongData(string(response_buffer), note_arr, note_time_arr, duration_arr);
        extractTimes(note_arr, note_time_arr, duration_arr);

        (*mp3_player).play(song);
        (*tft).fillRect(0, 380, 800, 100, RA8875_WHITE);
        state = GAME_PLAY_STATE;
    } else if (state == GAME_PLAY_STATE)
    {
        if (fabs((a_rects[a_end].getStart() * 1000) - elapsed) <= 3000)
            a_end++;
        if (fabs((s_rects[a_end].getStart() * 1000) - elapsed) <= 3000)
            s_end++;
        if (fabs((d_rects[d_end].getStart() * 1000) - elapsed) <= 3000)
            d_end++;
        if (fabs((f_rects[f_end].getStart() * 1000) - elapsed) <= 3000)
            f_end++;

        for (int i = a_start; i < a_end; i++)
            a_rects[i].update(380, tft);
        for (int i = s_start; i < s_end; i++)
            s_rects[i].update(380, tft);
        for (int i = d_start; i < d_end; i++)
            d_rects[i].update(380, tft);
        for (int i = f_start; i < f_end; i++)
            f_rects[i].update(380, tft);
        (*tft).fillRect(0, 380, 800, 100, RA8875_WHITE);

        state = LEADERBOARD_STATE;
    } else if (state == LEADERBOARD_STATE)
    {
        state = HOME_STATE;
    }
}




static void Game::getSongData(char* request_buffer)
{
    char title[100] = {0};
    strcpy(title, song_name.c_str());
    sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_song.py?song=%s HTTP/1.1\r\n", title);
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "\r\n");
}

static void Game::parseSongData(string data_to_process, char* note_arr, float* note_time_arr, float* duration_arr)
{
    int array_index = 0;
    while (data_to_process.size() > 0)
    {
        int note_index = data_to_process.find("'");
        if (note_index != -1)
            note_arr[array_index] = data_to_process.at(noteindex + 1);
        else
            break;
        if (note_index + 5 < data_to_process.size()) 
            data_to_process = data_to_process.substr(note_index + 5, data_to_process.size() - (note_index + 5));
        else
            break;

        int time_end = data_to_process.find(",");
        if (time_end != -1) 
            note_time_arr[array_index] = atof(data_to_process.substr(0, time_end).c_str());
        else
            break;
        if (time_end + 2 < data_to_process.size()) 
            data_to_process = data_to_process.substr(time_end + 2, data_to_process.size() - (time_end + 2));
        else
            break;

        int duration_end = data_to_process.find(")");
        if (duration_end != -1) 
            duration_arr[array_index] = atof(data_to_process.substr(0, duration_end).c_str());
        else
            break;
        array_index++;
    }
}

static void Game::extractTimes(char* note_arr, float* note_time_arr, float* duration_arr) 
{
    RectNote* a_index = a_rects;
    RectNote* s_index = s_rects;
    RectNote* d_index = d_rects;
    RectNote* f_index = f_rects;

    float start_time;
    float end_time;
    int dur;
    for (int i = 0; i < strlen(note_arr); i++)
    {
        if (note_arr[i] == 'a') 
        {
            start_time = note_time_arr[i];
            end_time = note_time_arr[i] + duration_arr[i];
            dur = int(duration_arr[i] * 1000);
            if (dur <= 500)
                dur = 20;
            else
                dur = int(dur / 25);
            *(a_index++) = RectNote(dur, 150, 50, RA8875_GREEN);
            // a_arr[aindex++] = note_time_arr[i];
            // a_arr[aindex++] = note_time_arr[i] + duration_arr[i];
        }
        if (note_arr[i] == 's') 
        {
            start_time = note_time_arr[i];
            end_time = note_time_arr[i] + duration_arr[i];
            dur = int(duration_arr[i] * 1000);
            if (dur <= 500)
                dur = 20;
            else
                dur = int(dur / 25);
            *(s_index++) = RectNote(dur, 150, 50, RA8875_RED);
            // s_arr[sindex++] = note_time_arr[i];
            // s_arr[sindex++] = note_time_arr[i] + duration_arr[i];
        }
        if (note_arr[i] == 'd') 
        {
            start_time = note_time_arr[i];
            end_time = note_time_arr[i] + duration_arr[i];
            dur = int(duration_arr[i] * 1000);
            if (dur <= 500)
                dur = 20;
            else
                dur = int(dur / 25);
            *(d_index++) = RectNote(dur, 150, 50, RA8875_YELLOW);
            // d_arr[dindex++] = note_time_arr[i];
            // d_arr[dindex++] = note_time_arr[i] + duration_arr[i];
        }
        if (note_arr[i] == 'f') 
        {
            start_time = note_time_arr[i];
            end_time = note_time_arr[i] + duration_arr[i];
            dur = int(duration_arr[i] * 1000);
            if (dur <= 500)
                dur = 20;
            else
                dur = int(dur / 25);
            *(f_index++) = RectNote(dur, 150, 50, RA8875_BLUE);
            // f_arr[findex++] = note_time_arr[i];
            // f_arr[findex++] = note_time_arr[i] + duration_arr[i];
        }
    }
}
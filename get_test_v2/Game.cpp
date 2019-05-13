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

Game::Game(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player):
    a_string(a_LED_pin, a_analog_pin),
    s_string(s_LED_pin, s_analog_pin),
    d_string(d_LED_pin, d_analog_pin),
    f_string(f_LED_pin, f_analog_pin)
{
    //sets up all LED pins, phototransistor pins
    //initializes score and game state
    tft = input_tft;
    mp3_player = input_mp3_player;
    
    a_start = 0;
    a_end = 0;
    s_start = 0;
    s_end = 0;
    d_start = 0;
    d_end = 0;
    f_start = 0;
    f_end = 0;

    playing = false;
    user = "";
    song_name = "Seven_Nation_Army";
    artist_name = "";
    song = 6;  //number in SD card
    song_len = 0;

    score = 0;
    state = 0;
}

void Game::setUpLED()
{
    a_string.beginLights();
    s_string.beginLights();
    d_string.beginLights();
    f_string.beginLights();
}

void Game::gamePlay(int elapsed, char* request_buffer, char* response_buffer)
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
        // if select finished
        // {
        //     getSongData(request_buffer);
        //     state = SONG_GET_STATE;
        // }
        getSongData(request_buffer);
        state = SONG_GET_STATE;
    } else if (state == SONG_GET_STATE)
    {
        //get all data necessary for game play for song
        char note_arr[500] = {0};
        float note_time_arr[500] = {0};
        float duration_arr[500] = {0};

        //parse the data and get note times
        parseSongData(response_buffer, note_arr, note_time_arr, duration_arr);
        extractTimes(note_arr, note_time_arr, duration_arr);

        (*tft).fillRect(0, 380, 800, 100, RA8875_WHITE);
        state = GAME_PLAY_STATE;
    } else if (state == GAME_PLAY_STATE)
    {
        //start playing song if just begun
        if (!playing)
        {
            playing = true;
            (*mp3_player).play(song);
        }

        //add new rectangles that should appear on screen
        if (fabs((a_rects[a_end].getStart() * 1000) - elapsed) <= 3000)
            a_end++;
        if (fabs((s_rects[s_end].getStart() * 1000) - elapsed) <= 3000)
            s_end++;
        if (fabs((d_rects[d_end].getStart() * 1000) - elapsed) <= 3000)
            d_end++;
        if (fabs((f_rects[f_end].getStart() * 1000) - elapsed) <= 3000)
            f_end++;

        //move rectangles down screen (displays from start, furthest along, to end, most recent)
        for (int i = a_start; i < a_end; i++)
            a_rects[i].update(380, tft);
        for (int i = s_start; i < s_end; i++)
            s_rects[i].update(380, tft);
        for (int i = d_start; i < d_end; i++)
            d_rects[i].update(380, tft);
        for (int i = f_start; i < f_end; i++)
            f_rects[i].update(380, tft);
        (*tft).fillRect(0, 380, 800, 100, RA8875_WHITE);

        //set LEDs, ready timing for scoring
        if (a_rects[a_start].toPress())
        {
            a_string.LEDControl(true);
            a_string.setRefTime(millis());
        }
        if (s_rects[s_start].toPress())
        {
            s_string.LEDControl(true);
            s_string.setRefTime(millis());
        }
        if (d_rects[d_start].toPress())
        {
            d_string.LEDControl(true);
            d_string.setRefTime(millis());
        }
        if (f_rects[f_start].toPress())
        {
            f_string.LEDControl(true);
            f_string.setRefTime(millis());
        }

        //turn of LEDs if necessary, eliminate rectangles from being displayed
        if (a_rects[a_start].passed())
        {
            a_string.LEDControl(false);
            a_start++;
            a_string.toScoreNote();
        }
        if (s_rects[s_start].passed())
        {
            s_string.LEDControl(false);
            s_start++;
            s_string.toScoreNote();
        }
        if (d_rects[d_start].passed())
        {
            d_string.LEDControl(false);
            d_start++;
            d_string.toScoreNote();
        }
        if (f_rects[f_start].passed())
        {
            f_string.LEDControl(false);
            f_start++;
            f_string.toScoreNote();
        }
        
        a_string.userAction();
        s_string.userAction();
        d_string.userAction();
        f_string.userAction();

        score += a_string.scoreAction(millis());
        score += s_string.scoreAction(millis());
        score += d_string.scoreAction(millis());
        score += f_string.scoreAction(millis());

        a_string.displayFeedback(millis(), tft, 100);
        s_string.displayFeedback(millis(), tft, 300);
        d_string.displayFeedback(millis(), tft, 500);
        f_string.displayFeedback(millis(), tft, 700);

        char score_str[50] = "Score: ";
        char score_num[15];
        sprintf(score_num, "%d", score);
        strcat(score_str, score_num);
        tft->textTransparent(RA8875_BLACK);
        tft->textSetCursor(390, 440);
        tft->textWrite(score_str);
        

//        if (elapsed > song_len)
//          state = LEADERBOARD_STATE;
    } else if (state == LEADERBOARD_STATE)
    {
        //state = HOME_STATE;
    }
}

int Game::getState()
{
    return state;
}

int Game::getScore()
{
    return score;
}

void Game::getSongData(char* request_buffer)
{
    char title[100] = {0};
    strcpy(title, song_name.c_str());
    sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_song.py?song=%s HTTP/1.1\r\n", title);
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "\r\n");
}

void Game::parseSongData(char* response_buffer, char* note_arr, float* note_time_arr, float* duration_arr)
{
    int array_index = 0;
    std::string data_to_process = std::string(response_buffer);
    while (data_to_process.size() > 0)
    {
        int note_index = data_to_process.find("'");
        if (note_index != -1)
            note_arr[array_index] = data_to_process.at(note_index + 1);
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

void Game::extractTimes(char* note_arr, float* note_time_arr, float* duration_arr) 
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
            *(a_index++) = RectNote(start_time, end_time, dur, 150, 50, RA8875_GREEN);
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
            *(s_index++) = RectNote(start_time, end_time, dur, 150, 250, RA8875_RED);
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
            *(d_index++) = RectNote(start_time, end_time, dur, 150, 450, RA8875_YELLOW);
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
            *(f_index++) = RectNote(start_time, end_time, dur, 150, 650, RA8875_BLUE);
        }
    }
}

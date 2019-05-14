#include "Leaderboard.h"

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <string>

Leaderboard::Leaderboard() {
    this->song_name = "";
    this->array_size = 0;
}

void Leaderboard::setSongName(std::string song_name) {
    this->song_name = song_name;
}

void Leaderboard::postToLeaderboard(char* request_buffer, int score, std::string user) {
    char temp[100] = {0};
    sprintf(temp, "score=%d&user=%s&song=%s", score, user.c_str(), song_name.c_str());
    char len[100] = {0};
    sprintf(len, "Content-Length: %d\r\n", strlen(temp));
    
    sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/leaderboards.py");
    strcat(request_buffer, " HTTP/1.1\r\n");
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
    strcat(request_buffer, len);
    strcat(request_buffer, "\r\n");
    strcat(request_buffer, temp);
    strcat(request_buffer, "\r\n");
}

void Leaderboard::getLeaderboard(char* request_buffer) {
    sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/leaderboards.py?song=");
    strcat(request_buffer, song_name.c_str());
    strcat(request_buffer, " HTTP/1.1\r\n");
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "\r\n");
}

void Leaderboard::parseLeaderboard(char* response_buffer) {
    std::string str = response_buffer;
    int array_index = 0;
    while (str.size() > 0) {
        int score_end = str.find(",");
        if (score_end != -1) {
            scores[array_index] = atoi(str.substr(1, score_end).c_str());
            str = str.substr(score_end+1);
        }
        else {
            break;
        }
        int user_end = str.find("]");
        if (user_end != -1){
            users[array_index] = (str.substr(0, user_end));
        }
        else {
            break;
        }
        str = str.substr(user_end+1);
        array_index++;
    }
    array_size = array_index;
}

//void Leaderboard::displayScore(Adafruit_RA8875* tft) {
//    char score_message[150] = "Great job!\r\nYour score is:
//}

void Leaderboard::displayLeaderboard(Adafruit_RA8875* tft) {
    char title[100] = "Leaderboards: ";
    strcat(title, song_name.c_str());
    tft->textTransparent(RA8875_CYAN);
    tft->textSetCursor(0, 0);
    tft->textWrite(title);
    int place_count = 1;
    char result_str[100];
    char score_str[100];
    int cursor_y;
    for (int index = 0; index < array_size; index++) {
        memset(result_str, 0, sizeof(result_str));
        memset(score_str, 0, sizeof(score_str));
        sprintf(result_str, "%d", place_count);
        strcat(result_str, ". ");
        strcat(result_str, users[index].c_str());
        strcat(result_str, ": ");
        sprintf(score_str, "%d", scores[index]);
        strcat(result_str, score_str);
        strcat(result_str, " points");
        cursor_y = (1+index)*20;
        tft->textSetCursor(10, cursor_y);
        tft->textWrite(result_str);
        place_count++;
    }
}

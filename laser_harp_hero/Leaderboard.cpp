/*
 * Class that handles the getting, parsing, and displaying of Leaderboards that are stored on a server-side database.
 */
#include "Leaderboard.h"
#include "LaserString.h"
#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <string>

Leaderboard::Leaderboard() {
    this->song_name = ""; //initialize song_name to empty string and array size to 0
    this->array_size = 0;
}

void Leaderboard::setSongName(std::string song_name) {
    this->song_name = song_name;
}

void Leaderboard::postToLeaderboard(char* request_buffer, int score, std::string user) {
    char temp[100] = {0};
    sprintf(temp, "score=%d&user=%s&song=%s", score, user.c_str(), song_name.c_str()); //create query string for post request
    char len[100] = {0};
    sprintf(len, "Content-Length: %d\r\n", strlen(temp));

    //build post request buffer
    sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/leaderboards.py");
    strcat(request_buffer, " HTTP/1.1\r\n");
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
    strcat(request_buffer, len);
    strcat(request_buffer, "\r\n");
    strcat(request_buffer, temp);
    strcat(request_buffer, "\r\n");
}

void Leaderboard::getLeaderboard(char* request_buffer) { //make get request for leaderboard for a specific song
    sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/leaderboards.py?song=");
    strcat(request_buffer, song_name.c_str());
    strcat(request_buffer, " HTTP/1.1\r\n");
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "\r\n");
}

//parses the leaderboard and stores values into two arrays: an array of users and array of their scores. 
void Leaderboard::parseLeaderboard(char* response_buffer) {
    std::string str = response_buffer; //turn response buffer into c++ string
    int array_index = 0;
    while (str.size() > 0) {
        int score_end = str.find(","); //find comma that signifies end of score
        if (score_end != -1) { //if found comma
            scores[array_index] = atoi(str.substr(1, score_end).c_str()); //add score to scores array
            str = str.substr(score_end+1); //reset string to move past the score
        }
        else {
            break; //didn't find comma --> we're done.
        }
        int user_end = str.find("]"); //find square brackets that signify end of user.
        if (user_end != -1){ //if found square brackets
            users[array_index] = (str.substr(0, user_end)); //add user to users array
        }
        else {
            break; //didn't find brackets --> we're done
        }
        str = str.substr(user_end+1); //reset string for next iteration
        array_index++; //increment array index
    }
    array_size = array_index; //the array size is the final array index!
}

//displays score after the game is over
int Leaderboard::displayScore(Adafruit_RA8875* tft, LaserString* string_1, LaserString* string_2, LaserString* string_3, LaserString* string_4, int score) {
    char score_message[150];
    sprintf(score_message, "Great job! Your score is %d.", score); //create text to display
    tft->textTransparent(RA8875_CYAN); //set text color and write to display
    tft->textSetCursor(0, 10);
    tft->textWrite(score_message);

    //displays user instructions
    char message[200] = "Use first laser to play this song again. Use second laser to play a different song.";
    tft->textSetCursor(0, 50);
    tft->textWrite(message);
    memset(message, 0, sizeof(message));
    strcpy(message, "Use third laser to view leaderboards. Use fourth laser to play as a different user!");
    tft->textSetCursor(0, 70);
    tft->textWrite(message);

    //checks which strings are broken in order to handle state transitions
    if (string_1->broken())
        return 1;
    else if (string_2->broken())
        return 2;
    else if (string_3->broken())
        return 3;
    else if (string_4->broken())
        return 4;
    else
        return 0;
}

//displays leaderboards for a particular song
int Leaderboard::displayLeaderboard(Adafruit_RA8875* tft, LaserString* string_1, LaserString* string_2, LaserString* string_3) {
    char title[100] = "Leaderboards: ";
    strcat(title, song_name.c_str()); //display title of song
    tft->textTransparent(RA8875_CYAN);
    tft->textSetCursor(0, 10);
    tft->textWrite(title);
    int place_count = 1; //keeps track of the place in the leaderboard that we're printing
    char result_str[100];
    char score_str[100];
    int cursor_y;
    for (int index = 0; index < array_size; index++) {
        //build up the string that displays the current place, the username, and the score
        memset(result_str, 0, sizeof(result_str));
        memset(score_str, 0, sizeof(score_str));
        sprintf(result_str, "%d", place_count);
        strcat(result_str, ". ");
        strcat(result_str, users[index].c_str());
        strcat(result_str, ": ");
        sprintf(score_str, "%d", scores[index]);
        strcat(result_str, score_str);
        strcat(result_str, " points");
        //calculate the y position that we're going to write the current place at
        cursor_y = (1+index)*20 + 10;
        tft->textSetCursor(10, cursor_y);
        tft->textWrite(result_str); //write to the tft display
        place_count++;
    }

    //display instructions for user
    char message[200] = "Use first laser to play this song again. Use second laser to play a different song.";
    tft->textSetCursor(0, 300);
    tft->textWrite(message);
    memset(message, 0, sizeof(message));
    strcpy(message, "Use third laser to play as a different user!");
    tft->textSetCursor(0, 320);
    tft->textWrite(message);

    //determine which lasers are broken in order to handle state transitions
    if (string_1->broken())
        return 1;
    else if (string_2->broken())
        return 2;
    else if (string_3->broken())
        return 3;
    else
        return 0;
}

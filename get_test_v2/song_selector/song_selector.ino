char song_list[500];
int song_indices[30]; //scores indices of song titles
int num_songs = 0;
char chosen_song[100];

void get_songs() {
  //need to put get_songlist on server
  char request[500];
  sprintf(request, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_songlist.py HTTP/1.1\r\n");
  strcat(request, "Host: 608dev.net\r\n");
  strcat(request, "\r\n");
  do_http_request("608dev.net", request, song_list, 500, RESPONSE_TIMEOUT, true);
  int i = 0;
  int j = 0;
  while (i <= strlen(song_list)) {
    if (song_list[i] == "'") {
      song_indices[j] = i;
      j++;
    }
    i++;
  }
  num_songs = j%4;
}

void select_song() {
  chosen = song_list.substr(song_indices[0],song_indices[1]);
  Serial.println(chosen);
  int counter = 0
  while (f_hand == false) {
    int a_bins = analogRead(A0);
    float a_voltage = (a_bins/4096.0)*3.3;
    int s_bins = analogRead(A11);
    float s_voltage = (s_bins / 4096.0) * 3.3;
    int f_bins = analogRead(A7);
    float f_voltage = (f_bins / 4096.0) * 3.3;

    if (f_voltage >= 0.9) {
      f_hand = true
    }
    
    if (a_voltage >= 0.9) {
      counter = (counter+1)%num_songs;
      chosen = song_list.substr(song_indices[counter*2+1],song_indices[counter*2+2])
      Serial.println(chosen)
    }

    if (s_voltage >= 0.9) {
      counter = (counter-1)%num_songs;
      chosen = song_list.substr(song_indices[counter*2+1],song_indices[counter*2+2])
      Serial.println(chosen)
    }

  }

  f_hand = false;
  
}

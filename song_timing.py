import pygame
import time
import playsound
import sqlite3
from  song_nums import update_db

beatmaps_db = "beatmaps.db"
local_song_db = "song_nums.db"

def doIt(song, artist):
    song_file = "Songs/" + song + ".mp3"
    song_table = song.replace(" ", "_")
    song_table = song_table.replace("'", "")
    song_table = song_table.replace(",", "")
    pygame.init() 
    pygame.display.set_mode((100,100))

    conn = sqlite3.connect(beatmaps_db)
    c = conn.cursor()
    command = 'DROP TABLE IF EXISTS ' + song_table + ';'
    c.execute(command)
    command = 'CREATE TABLE IF NOT EXISTS ' + song_table + ' (key text, start real, duration real);'
    c.execute(command)

    all_notes = []

    record = True
    start = time.time()
    playsound.playsound(song_file, False)
    
    while record: 
        event = pygame.event.get()
        if len(event) == 0:
            continue
        else:  
            event = event[0]
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_a:
                all_notes.append(("a", True, time.time() - start))
            if event.key == pygame.K_s:
                all_notes.append(("s", True, time.time() - start))
            if event.key == pygame.K_d:
                all_notes.append(("d", True, time.time() - start))
            if event.key == pygame.K_f:
                all_notes.append(("f", True, time.time() - start))
            if event.key == pygame.K_SPACE:
                all_notes.append(("space", True, time.time() - start))
        elif event.type == pygame.KEYUP:
            if event.key == pygame.K_a:
                all_notes.append(("a", False, time.time() - start))
            if event.key == pygame.K_s:
                all_notes.append(("s", False, time.time() - start))
            if event.key == pygame.K_d:
                all_notes.append(("d", False, time.time() - start))
            if event.key == pygame.K_f:
                all_notes.append(("f", False, time.time() - start))
            if event.key == pygame.K_SPACE:
                record = False

    start_times = [0 for i in range(4)]
    for notes in all_notes:
        if notes[1]:  #down press
            if notes[0] == "a":
                start_times[0] = notes[2]
            elif notes[0] == "s":
                start_times[1] = notes[2]
            elif notes[0] == "d":
                start_times[2] = notes[2]
            elif notes[0] == "f":
                start_times[3] = notes[2]
        if not notes[1]: #release
            if notes[0] == "a":
                duration = round(notes[2] - start_times[0], 3)
                start = start_times[0]
            elif notes[0] == "s":
                duration = round(notes[2] - start_times[1], 3)
                start = start_times[1]
            elif notes[0] == "d":
                duration = round(notes[2] - start_times[2], 3)
                start = start_times[2]
            elif notes[0] == "f":
                duration = round(notes[2] - start_times[3], 3)
                start = start_times[3]
            command = 'INSERT into ' + song_table + ' VALUES (?,?,?);'
            c.execute(command, (notes[0], round(start, 3), duration))
        
        if notes[0] == 'space':
            #c.execute('''DROP TABLE IF EXISTS songlist;''')
            c.execute('''CREATE TABLE IF NOT EXISTS songlist (title text, artist text, length real);''')
            c.execute('''CREATE UNIQUE INDEX IF NOT EXISTS idx_songlist_title ON songlist (title);''')
            c.execute('''INSERT OR REPLACE INTO songlist VALUES (?, ?, ?);''', (song, artist, notes[2]))

    command = 'SELECT * FROM ' + song_table + ' ORDER BY start ASC;'
    print(c.execute(command).fetchall())
    print(c.execute('''SELECT * FROM songlist''').fetchall())
    conn.commit() 
    conn.close() 

def add_new_song_num(song_name):
    conn = sqlite3.connect(local_song_db)
    c = conn.cursor()
    length = c.execute('''SELECT COUNT(*) FROM song_numbers''').fetchone()[0]
    new_index = length+1
    c.execute('''INSERT INTO song_numbers VALUES (?,?);''', (song_name, new_index))
    conn.commit()
    conn.close()

if __name__ == "__main__":
    doIt("Jessies Girl", "Rick Springfield")
    add_new_song_num("Jessies_Girl")

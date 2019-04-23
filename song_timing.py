import pygame#from pygame.locals import *
import time
import playsound
import sqlite3

beatmaps_db = "beatmaps.db"

def doIt(song, artist):
    song_file = "Songs/" + song + ".mp3"
    song_table = song.replace(" ", "_")
    song_table = song_table.replace("'", "")
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
                duration = notes[2] - start_times[0]
                start = start_times[0]
            elif notes[0] == "s":
                duration = notes[2] - start_times[1]
                start = start_times[1]
            elif notes[0] == "d":
                duration = notes[2] - start_times[1]
                start = start_times[2]
            elif notes[0] == "f":
                duration = notes[2] - start_times[1]
                start = start_times[3]
            command = 'INSERT into ' + song_table + ' VALUES (?,?,?);'
            c.execute(command, (notes[0], start, duration))
        
        if notes[0] == 'space':
            c.execute('''CREATE TABLE IF NOT EXISTS songlist (title text, artist text, length real);''')
            c.execute('''INSERT OR REPLACE into songlist VALUES (?, ?, ?);''', (song, artist, notes[2]))

    command = 'SELECT * FROM ' + song_table + ' ORDER BY start ASC;'
    print(c.execute(command).fetchall())
    print(c.execute('''SELECT * FROM songlist''').fetchall())

if __name__ == "__main__":
    doIt("Hotel California", "Eagles")
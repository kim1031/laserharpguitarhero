import pygame#from pygame.locals import *
import time
import playsound
import sqlite3

beatmaps_db = "beatmaps.db"

def doIt(song):
    song_file = song + ".mp3"
    pygame.init() 
    pygame.display.set_mode((100,100))

    conn = sqlite3.connect(beatmaps_db)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS twinkle (key text, start real, duration real);''')

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
                #print("a DOWN " + str(time.time() - starter))
                all_notes.append(("a", True, time.time() - start))
            if event.key == pygame.K_s:
                #print("s DOWN " + str(time.time() - starter))
                all_notes.append(("s", True, time.time() - start))
            if event.key == pygame.K_d:
                #print("d DOWN " + str(time.time() - starter))
                all_notes.append(("d", True, time.time() - start))
            if event.key == pygame.K_f:
                #print("f DOWN " + str(time.time() - starter))
                all_notes.append(("f", True, time.time() - start))
            if event.key == pygame.K_SPACE:
                all_notes.append(("space", True, time.time() - start))
        elif event.type == pygame.KEYUP:
            if event.key == pygame.K_a:
                #print("a UP " + str(time.time() - starter))
                all_notes.append(("a", False, time.time() - start))
            if event.key == pygame.K_s:
                #print("s UP " + str(time.time() - starter))
                all_notes.append(("s", False, time.time() - start))
            if event.key == pygame.K_d:
                #print("d UP " + str(time.time() - starter))
                all_notes.append(("d", False, time.time() - start))
            if event.key == pygame.K_f:
                #print("f UP " + str(time.time() - starter))
                all_notes.append(("f", False, time.time() - start))
            if event.key == pygame.K_SPACE:
                record = False

    #down = [False for i in range(4)] #0 = a, 1 = b, 2 = c, 3 = d
    start_times = [0 for i in range(4)]
    for notes in all_notes:
        if notes[1]:  #down press
            if notes[0] == "a":
                start_times[0] = notes[2]
                #down[0] = True
            elif notes[0] == "s":
                start_times[1] = notes[2]
            elif notes[0] == "d":
                start_times[2] = notes[2]
            elif notes[0] == "f":
                start_times[3] = notes[2]
        if not notes[1]:
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
            c.execute('''INSERT into twinkle VALUES (?,?,?);''', (notes[0], start, duration))

    print(c.execute('''SELECT * FROM twinkle ORDER BY start ASC;''').fetchall())

if __name__ == "__main__":
    doIt("twinkle")
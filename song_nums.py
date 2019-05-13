import sqlite3
song_db = '__HOME__/laserharpguitarhero/song_nums.db'

local_song_db = 'song_nums.db'

def request_handler(request):
    if request['method'] == 'GET':
        conn = sqlite3.connect(song_db)
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS song_numbers (name text, number int);''')
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Twinkle_Twinkle', 1))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Barracuda', 2))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Hotel_California', 3))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Jessies_Girl', 4))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Paint_It_Black', 5))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Seven_Nation_Army', 6))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('Take_Me_Out', 7))
        c.execute('''INSERT INTO song_numbers VALUES (?, ?);''',('This_Love', 8))

    conn.commit()
    conn.close()
    return 'we did the thing'

def update_db(song_name, song_num):
    conn = sqlite3.connect(local_song_db)
    c = conn.cursor()
    command = '''INSERT into song_numbers VALUES (?, ?);'''
    c.execute(command, (song_name, song_num))
    conn.commit()
    conn.close()

          
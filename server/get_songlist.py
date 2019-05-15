import sqlite3
song_db = '__HOME__/laserharpguitarhero/beatmaps.db'

def request_handler(request):
    '''
        Returns the song list.
    '''
    if request['method'] == "GET":
        try:
            conn = sqlite3.connect(song_db)
            command = 'SELECT * FROM songlist'
            c = conn.cursor()

            top = c.execute(command).fetchall()

            conn.commit()
            conn.close()

            return str(top)
        except:
            return ".1"
    else:
        return ".2"

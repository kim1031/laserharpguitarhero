import sqlite3
song_db = '__HOME__/laserharpguitarhero/beatmaps.db'


def request_handler(request):
    if request['method'] == "GET":
        try:
            conn = sqlite3.connect(song_db)

            song_table = request['values']['song']
            command = 'SELECT * FROM ' + song_table + ' ORDER BY start ASC;'

            c = conn.cursor()

            top = c.execute(command).fetchall()

            conn.commit()
            conn.close()

            return str(top)

        except:
            return "."
    else:
        return "."

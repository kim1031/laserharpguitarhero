import sqlite3
song_db = '__HOME__/beatmaps.db'

def request_handler(request):
    if request['method'] == "REQUEST":
        try:
            strtop = ""
            conn = sqlite3.connect(song_db)

            song_table = request['values']['song']
            command = 'SELECT * FROM ' + song_table + ' ORDER BY start ASC;'

            c = conn.cursor()

            top = c.execute(command).fetchall()

            conn.commit()
            conn.close()

            return str(strtop)
        
        except:
            return "."
    else:
    	return "."
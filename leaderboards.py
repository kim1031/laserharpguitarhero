import sqlite3

leaderboards_db = '__HOME__/laserharpguitarhero/leaderboards.db'

def request_handler(request):
    conn = sqlite3.connect(leaderboards_db)
    c = conn.cursor()
    if request['method'] == 'POST':
        values = request['form']
        c.execute('''CREATE TABLE IF NOT EXISTS scores_table (score int, song text, user text);''')
        c.execute('''INSERT into scores_table VALUES(?, ?, ?);''',(int(values['score']), values['song'], values['user']))
        conn.commit()
        conn.close()
        return 'success'
    if request['method'] == 'GET':
        values = request['values']
        song = values['song']
        scores = c.execute('''SELECT * FROM scores_table WHERE song == ? ORDER BY score DESC LIMIT 10;''',(song,)).fetchall()
        outs = ''
        for score in scores:
            outs += '[' + str(score[0]) + ',' + score[2] + ']' #[score,user]
        conn.commit()
        conn.close()
        return outs


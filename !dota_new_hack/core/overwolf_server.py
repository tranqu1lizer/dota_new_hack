import requests
from flask import Flask, request

app = Flask(__name__)

@app.route('/')
def get_players_info():
    result = ""

    if not request.args.get("count") or not request.args.get("players"):
        return "no players or count</br>"

    players_count = int(request.args.get("count"))
    players = request.args.get("players").split(",", players_count)

    for pl in players:
        acc_id = int(pl) & 0xFFFFFFFF
        parsed = requests.get(f"https://api.opendota.com/api/players/{str(acc_id)}/heroes").json()
        first = parsed[0]
        if first:
            hero_id = first["hero_id"]
            try:
                winrate = round(100*(first["win"]/first["games"]),1)
            except: continue

            result += f"acc: {pl}, wr: {str(winrate)}%</br>"

    return result


if __name__ == '__main__':
    app.run()

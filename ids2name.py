import os,json,clipboard ,requests

filename = "heroes.json"

def dw_heroes():
    url = f"https://raw.githubusercontent.com/odota/dotaconstants/master/build/{filename}"
    res = requests.get(url)
    open(filename, 'wb').write(res.content)

def convert_data(data):
    converted_data = {}
    for hero_id, hero_data in data.items():
        converted_data[int(hero_id)] = hero_data["localized_name"]

    return converted_data

dw_heroes()

with open(filename, "r") as file:
    data = json.load(file)

converted_map = convert_data(data)

result_str = "std::unordered_map<int, std::string> m_heroid = {\n"
for hero_id, hero_name in converted_map.items():
   result_str = result_str + f"    {{ {hero_id}, \"{hero_name}\" }},\n"

result_str = result_str[:-2] + "\n}; // generated ids2name.py"

clipboard.copy(result_str)
print(result_str)
os.remove(filename)
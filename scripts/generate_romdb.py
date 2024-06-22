#!/usr/bin/python3

#  This python script takes a romdb.xml input file that contains information about roms (generated with advancemame --listxml)
# It outputs a output.json contained an Enea-friendly rom database file

import xml.etree.ElementTree as ET
import json

# Function to convert XML tree to dictionary
def xml_to_dict(element):
    result = {}
    if len(element) == 0:
        return element.text
    for child in element:
        if len(child) == 0:
            result[child.tag] = child.text
        else:
            result[child.tag] = xml_to_dict(child)
    return result

# Parse the XML file
tree = ET.parse('romdb.xml')
root = tree.getroot()

# Extract only the 'game' elements and convert to dictionary
games = []

roms = {
    'roms': games
}

for game in root.findall('game'):
    game_data = {
        'name': game.get('name'),
        'info': {
        }
    }

    # Finding title
    title_element = game.find('description')
    if title_element is not None:
        game_data['info']['title'] = title_element.text

    # Finding year
    year_element = game.find('year')
    if year_element is not None:
        game_data['info']['year'] = year_element.text

    # Finding manufacturer
    manufacturer_element = game.find('manufacturer')
    if manufacturer_element is not None:
        game_data['info']['manufacturer'] = manufacturer_element.text

    # Check for the 'isbios' field and set it as a boolean value
    isbios_element = game.get('isbios')
    if isbios_element is not None:
        game_data['info']['isBios'] = isbios_element == 'yes'
    else:
        game_data['info']['isBios'] = False

    games.append(game_data)

# Write the JSON output to a file
with open('output.json', 'w') as json_file:
    json.dump(roms, json_file, indent=4)

print("Conversion complete. JSON output saved to output.json")

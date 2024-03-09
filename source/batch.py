import os

scene_list = ['GameScene', 'HostScene', 'ClientScene', 'MenuScene', 'SettingScene', 'LoadScene', 'LevelScene']

# split the word by capital letter


def split_by_uppercase(word: str) -> list[str]:
    result = []  # Initialize an empty list to store the parts
    current_word = ''  # Initialize an empty string to build up each word part

    for character in word:
        if character.isupper() and current_word:  # If the character is uppercase and it's not the first character
            result.append(current_word)  # Add the current word to the result list
            current_word = character  # Start a new word with the current character
        else:
            current_word += character  # Add the character to the current word

    result.append(current_word)  # Add the last word to the result list

    return result


for sc in scene_list:
    f1 = open(f'./scenes/{sc}.h', 'w')
    sp = split_by_uppercase(sc)
    sp = [x.upper() for x in sp]
    # connect by underscore
    sp = "_".join(sp)
    guard = f'__{sp}_H__'
    f1.write(f'#ifndef {guard}\n')
    f1.write(f'#define {guard}\n')
    f1.write(f'\n')
    f1.write(f'#endif\n')

    f2 = open(f'./scenes/{sc}.cpp', 'w')
    f2.write(f'#include "{sc}.h"\n')
    f2.write(f'\n')

    f1.close()
    f2.close()

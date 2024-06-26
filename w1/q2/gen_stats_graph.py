import matplotlib.pyplot as plt
import re
import os
if not os.path.exists('stat_plot'):
    os.makedirs('stat_plot')

def draw_pic(res_file, config_file, exp_name, max_val):
    sample = 0
    data = {}
    with open(res_file, 'r') as file:
        current_algorithm = None
        current_values = []
        for line in file:
            line = line.strip()
            if line.endswith(':'):
                if current_algorithm is not None:
                    data[current_algorithm] = current_values
                    current_values = []
                current_algorithm = line[:-1]
            else:
                if(isinstance(current_values, int)):
                    current_values.extend(map(int, line.split()))
                else:
                    current_values.extend(map(float, line.split()))

        if current_algorithm is not None:
            data[current_algorithm] = current_values

    with open(config_file, 'r') as file:
        # content = file.read()
        for line in file:
            line = line.strip()
            tokens = line.split(' ')
            sample = 0
            if 'SAMPLES' in tokens:
                sample = tokens[2]
    with open(config_file, 'r') as file:
        content = file.read()
        matches = re.findall(r'#define\s+SAMPLES\s+(\d+)', content)
        if matches:
            sample = int(matches[0])
    # print(sample)

    max_length = max(len(values) for values in data.values())
    x = list(range(1, max_length + 1))

    fig_width = max_length * 0.02
    if max_length > 200:
        plt.figure(figsize=(fig_width, fig_width * 0.3))
    # print(fig_width, plt.gcf().get_figwidth())

    markers = ['+', 'x', '^', 'o']
    colors = ['r', 'y', 'black', 'blue']
    for i, (algorithm, values) in enumerate(data.items()):
        if(markers[i] == '^' or markers[i] == 'o'): # Make icon hollowed, set facecolors to 'none'
            plt.scatter(x[:len(values)], values, label=algorithm, marker=markers[i], s=60, facecolors='none', edgecolors=colors[i])
        else:
            plt.scatter(x[:len(values)], values, label=algorithm, marker=markers[i], s=60, facecolors=colors[i])


    plt.xlabel('Experiment', fontsize=20)
    plt.ylabel(exp_name, fontsize=20)
    plt.title(f'Samples = {sample}', fontsize=32)
    plt.legend(fontsize=16)
    plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1)
    if max_val > 0:
        plt.ylim(0, max_val * 1.1)
    plt.savefig('stat_plot/' + exp_name + '_' + str(sample) + '.png')
    plt.close()


implmented_sample_gen = [        
        "rnd",
        "des_strict",
        "des",
        "as_strict",
        "as",
        "rnd3",
        "as_10",
        "rnd_1Percent",
        "dup",
        "same",
        "worst"
    ]

config_file = "./def.h"
max_rec_file = "./max_res.txt"
max_rec_dict = {}

with open(max_rec_file, 'r') as file:
    for line in file:
        key, value = line.strip().split(': ')
        if key != 'time':
            max_rec_dict[key] = int(value)
        else:
            max_rec_dict[key] = float(value)

max_run_max = max_rec_dict.get('max_run')
time_max = max_rec_dict.get('time')
cmp_max = max_rec_dict.get("cmp")

max_run_max = 0
time_max = 0
cmp_max = 0

for i in implmented_sample_gen:
    cmp_res_file = "./cmp_res_" + i + ".txt"
    time_res_file = "./time_res_" + i + ".txt"
    max_run_res_file = "./max_run_res_" + i + ".txt"
    draw_pic(max_run_res_file, config_file, 'max_run_Len_' + i, max_run_max)
    draw_pic(time_res_file, config_file, 'Time_' + i, time_max)
    draw_pic(cmp_res_file, config_file, 'Comparisons_' + i, cmp_max)
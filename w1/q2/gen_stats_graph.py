import matplotlib.pyplot as plt
import re
import os
if not os.path.exists('stat_plot'):
    os.makedirs('stat_plot')

def draw_pic(res_file, config_file, exp_name):
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

    fig_width = max_length * 0.025

    # print(fig_width, plt.gcf().get_figwidth())
    plt.figure(figsize=(fig_width, fig_width * 0.3))

    markers = ['+', 'x', '^']
    colors = ['r', 'y', 'black']
    for i, (algorithm, values) in enumerate(data.items()):
        if(markers[i] == '^'):
            plt.scatter(x[:len(values)], values, label=algorithm, marker=markers[i], s=60, facecolors='none', edgecolors=colors[i])
        else:
            plt.scatter(x[:len(values)], values, label=algorithm, marker=markers[i], s=60, facecolors=colors[i])


    plt.xlabel('Experiment')
    plt.ylabel(exp_name)
    plt.title(f'Samples = {sample}')
    plt.legend()

    plt.savefig('stat_plot/' + exp_name + '_' + str(sample) + '.png')


cmp_res_file = "./cmp_res.txt"
time_res_file = "./time_res.txt"
max_run_res_file = "./max_run_res.txt"
config_file = "./def.h"
draw_pic(cmp_res_file, config_file, 'Comparisons')
draw_pic(time_res_file, config_file, 'Time')
draw_pic(max_run_res_file, config_file, 'max_run_Len')
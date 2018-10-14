import os, sys
import argparse
import logging
import shutil
import subprocess

import matplotlib.pyplot as plt
import pandas as pd
import csv

from patternmatching_data import *


class CsvDialect(csv.Dialect):
    delimiter = ";"
    quotechar = None
    escapechar = None
    doublequote = None
    lineterminator = "\r\n"
    quoting = csv.QUOTE_NONE
    skipinitialspace = True


def check_dataset_outputs_1(datasets):
    outputs = experiment_1_outputs

    if not os.path.exists(result_dir + outputs['global_output_dir']):
        os.mkdir(result_dir + outputs['global_output_dir'])

    result = {}
    for dataset in datasets:
        inputs = datasets_inputs[dataset]
        for pattern_id, pattern in enumerate(inputs['pattern_list']):
            for run in outputs['runs']:
                result_file = result_dir + dataset + '/' + outputs['directory_pattern'].format(pattern_id=pattern_id,
                                                                                               run=run[
                                                                                                   'name']) + '/results.totem'
                if os.path.exists(result_file):
                    result[(dataset, pattern_id, run['name'])] = result_file
    return result


def show_experiment_1(datasets):
    result_files = check_dataset_outputs_1(datasets)
    outputs = experiment_1_outputs

    parsed_results = []

    for dataset, pattern_id, run_name in result_files:
        result_file = result_files[(dataset, pattern_id, run_name)]
        with open(result_file) as f:
            reader = csv.DictReader(f, dialect=CsvDialect())
            for row in reader:
                if row['Step'] == 'END':
                    parsed_results.append((run_name, '%s-%d' % (dataset, pattern_id), float(row['Cumulative time'])))

    df = pd.DataFrame(parsed_results, columns=['group', 'column', 'val'])

    df.pivot("column", "group", "val").plot(kind='bar')

    plt.ylabel('Time to solution (ms)')
    plt.savefig(result_dir + outputs['global_output_dir'] + 'result.png')
    plt.show()


def check_dataset_outputs_2(datasets):
    outputs = experiment_2_outputs

    if not os.path.exists(result_dir + outputs['global_output_dir']):
        os.mkdir(result_dir + outputs['global_output_dir'])

    result = {}
    for dataset in datasets:
        inputs = datasets_inputs[dataset]
        for pattern_id, pattern in enumerate(inputs['pattern_list']):
            for run in outputs['runs']:
                result_file = result_dir + dataset + '/' + outputs['directory_pattern'].format(pattern_id=pattern_id,
                                                                                               run=run[
                                                                                                   'name']) + '/results.totem'
                if os.path.exists(result_file):
                    result[(dataset, pattern_id, run['name'])] = result_file
    return result


def show_experiment_2(datasets):
    result_files = check_dataset_outputs_2(datasets)
    outputs = experiment_2_outputs

    parsed_results = []

    for dataset, pattern_id, run_name in result_files:
        result_file = result_files[(dataset, pattern_id, run_name)]
        with open(result_file) as f:
            reader = csv.DictReader(f, dialect=CsvDialect())
            for row in reader:
                if row['Step'] == 'END':
                    parsed_results.append((run_name, '%s-%d' % (dataset, pattern_id), float(row['Cumulative time'])))

    df = pd.DataFrame(parsed_results, columns=['group', 'column', 'val'])

    df.pivot("column", "group", "val").plot(kind='bar')

    plt.ylabel('Time to solution (ms)')
    plt.savefig(result_dir + outputs['global_output_dir'] + 'result.png')
    plt.show()


def check_dataset_outputs_3(datasets):
    outputs = experiment_3_outputs

    if not os.path.exists(result_dir + outputs['global_output_dir']):
        os.mkdir(result_dir + outputs['global_output_dir'])

    result = {}
    for dataset in datasets:
        inputs = datasets_inputs[dataset]
        for pattern_id, pattern in enumerate(inputs['pattern_list']):
            for run in outputs['runs']:
                result_file = result_dir + dataset + '/' + outputs['directory_pattern'].format(pattern_id=pattern_id,
                                                                                               run=run[
                                                                                                   'name']) + '/results.totem'
                if os.path.exists(result_file):
                    result[(dataset, pattern_id, run['name'])] = result_file
    return result


def show_experiment_3(datasets):
    result_files = check_dataset_outputs_3(datasets)
    outputs = experiment_3_outputs

    parsed_results = []

    for dataset, pattern_id, run_name in result_files:
        result_file = result_files[(dataset, pattern_id, run_name)]
        with open(result_file) as f:
            reader = csv.DictReader(f, dialect=CsvDialect())
            for row in reader:
                if row['Step'] == 'END':
                    parsed_results.append((run_name, '%s-%d' % (dataset, pattern_id), float(row['Cumulative time'])))

    df = pd.DataFrame(parsed_results, columns=['group', 'column', 'val'])

    df.pivot("column", "group", "val").plot(kind='bar')

    plt.ylabel('Time to solution (ms)')
    plt.savefig(result_dir + outputs['global_output_dir'] + 'result.png')
    plt.show()

experiments_handler = {
    1: show_experiment_1,
    2: show_experiment_2,
    3: show_experiment_3
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run patternmatching tests.')
    parser.add_argument('--experiment', choices=['1', '2', '3', 'all'],
                        default='all', help='experiment to run (default="all")')
    parser.add_argument('--dataset', choices=['test_1', 'test_2', 'test_3', 'test_4', 'test_all',
                                              'youtube', 'patent', 'IMDB', 'reddit', 'all'], \
                        default='all', help='dataset to run the experiment on')

    args = parser.parse_args()

    if args.experiment == 'all':
        experiments = [1 + i for i in range(3)]
    else:
        experiments = [int(args.experiment)]

    if args.dataset == 'all':
        datasets = ['youtube', 'patent', 'IMDB', 'Reddit']
    elif args.dataset == 'test_all':
        datasets = ['test_1', 'test_2', 'test_3', 'test_4']
    else:
        datasets = [args.dataset]

    if not os.path.exists(result_dir):
        os.mkdir(result_dir)

    for experiment in experiments:
        experiments_handler[experiment](datasets)

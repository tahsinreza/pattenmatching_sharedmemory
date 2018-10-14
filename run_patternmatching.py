import os, sys
import argparse
import logging
import shutil
import subprocess

from patternmatching_data import *


def get_exe_directory(build_type):
    if build_type == 32:
        return 'build/bin/'
    if build_type == 64:
        return 'build/EID64/bin/'
    return 'build/bin/'


def check_dataset_inputs(dataset):
    inputs = datasets_inputs[dataset]
    ok = True
    if not os.path.exists(inputs['root_directory']):
        logging.critical('Missing root directory : ' + inputs['root_directory'])
        ok = False
    if not os.path.exists(inputs['root_directory'] + inputs['input_background_graph']):
        logging.critical('Missing background graph : ' + inputs['root_directory'] + inputs['input_background_graph'])
        ok = False

    for pattern in inputs['pattern_list']:
        if not os.path.exists(inputs['root_directory'] + pattern['vertex_data']):
            logging.critical('Missing input file : ' + inputs['root_directory'] + pattern['vertex_data'])
            ok = False
        if not os.path.exists(inputs['root_directory'] + pattern['directory']):
            logging.critical('Missing input directory : ' + inputs['root_directory'] + pattern['directory'])
            ok = False
        if not os.path.exists(inputs['root_directory'] + pattern['directory'] + 'pattern_edge'):
            logging.critical('Missing input file : ' + inputs['root_directory'] + pattern['directory'] + 'pattern_edge')
            ok = False
        if not os.path.exists(inputs['root_directory'] + pattern['directory'] + 'pattern_vertex_data'):
            logging.critical(
                'Missing input file : ' + inputs['root_directory'] + pattern['directory'] + 'pattern_vertex_data')
            ok = False
    return ok


def clear_dataset_outputs_1(dataset):
    inputs = datasets_inputs[dataset]
    outputs = experiment_1_outputs

    if not os.path.exists(result_dir + dataset):
        os.mkdir(result_dir + dataset)

    for pattern_id, pattern in enumerate(inputs['pattern_list']):
        for run in outputs['runs']:
            directory = result_dir + dataset + '/' + outputs['directory_pattern'].format(pattern_id=pattern_id,
                                                                                         run=run['name'])
            if os.path.exists(directory):
                shutil.rmtree(directory)
            os.mkdir(directory)


def run_experiment_1(dataset):
    logging.getLogger('Experiment 1').info('Starting experiment 1 on ' + dataset)
    if not check_dataset_inputs(dataset):
        logging.getLogger('Experiment 1').critical('Missing inputs ==> Abort')
    clear_dataset_outputs_1(dataset)

    inputs = datasets_inputs[dataset]
    outputs = experiment_1_outputs

    exe_dir = get_exe_directory(inputs['build_type'])
    input_background_graph = inputs['root_directory'] + inputs['input_background_graph']

    for pattern_id, pattern in enumerate(inputs['pattern_list']):
        for run in outputs['runs']:
            command_line_exe = exe_dir + run['exe']
            command_line = [command_line_exe, '-i', input_background_graph]
            if 'compiled_output' in pattern:
                output_background_graph_binary = result_dir + dataset + '/' + pattern['compiled_output']
                command_line = command_line + ['-b', output_background_graph_binary]

            input_vertex_data = inputs['root_directory'] + pattern['vertex_data']
            command_line = command_line + ['-v', input_vertex_data]

            input_pattern_direcory = inputs['root_directory'] + pattern['directory']
            command_line = command_line + ['-p', input_pattern_direcory]

            output_results_directory = result_dir + dataset + '/' + outputs['directory_pattern'].format(
                pattern_id=pattern_id, run=run['name'])
            command_line = command_line + ['-o', output_results_directory]

            subprocess.run(command_line, check=True)


def clear_dataset_outputs_2(dataset):
    inputs = datasets_inputs[dataset]
    outputs = experiment_2_outputs

    if not os.path.exists(result_dir + dataset):
        os.mkdir(result_dir + dataset)

    for pattern_id, pattern in enumerate(inputs['pattern_list']):
        for run in outputs['runs']:
            directory = result_dir + dataset + '/' + outputs['directory_pattern'].format(pattern_id=pattern_id,
                                                                                         run=run['name'])
            if os.path.exists(directory):
                shutil.rmtree(directory)
            os.mkdir(directory)


def run_experiment_2(dataset):
    logging.getLogger('Experiment 2').info('Starting experiment 2 on ' + dataset)
    if not check_dataset_inputs(dataset):
        logging.getLogger('Experiment 2').critical('Missing inputs ==> Abort')
    clear_dataset_outputs_2(dataset)

    inputs = datasets_inputs[dataset]
    outputs = experiment_2_outputs

    exe_dir = get_exe_directory(inputs['build_type'])
    input_background_graph = inputs['root_directory'] + inputs['input_background_graph']

    for pattern_id, pattern in enumerate(inputs['pattern_list']):
        for run in outputs['runs']:
            command_line_exe = exe_dir + run['exe']
            command_line = [command_line_exe, '-i', input_background_graph]
            if 'compiled_output' in pattern:
                output_background_graph_binary = result_dir + dataset + '/' + pattern['compiled_output']
                command_line = command_line + ['-b', output_background_graph_binary]

            input_vertex_data = inputs['root_directory'] + pattern['vertex_data']
            command_line = command_line + ['-v', input_vertex_data]

            input_pattern_direcory = inputs['root_directory'] + pattern['directory']
            command_line = command_line + ['-p', input_pattern_direcory]

            output_results_directory = result_dir + dataset + '/' + outputs['directory_pattern'].format(
                pattern_id=pattern_id, run=run['name'])
            command_line = command_line + ['-o', output_results_directory]

            subprocess.run(command_line, check=True)


def clear_dataset_outputs_3(dataset):
    inputs = datasets_inputs[dataset]
    outputs = experiment_3_outputs

    if not os.path.exists(result_dir + dataset):
        os.mkdir(result_dir + dataset)

    for pattern_id, pattern in enumerate(inputs['pattern_list']):
        for run in outputs['runs']:
            directory = result_dir + dataset + '/' + outputs['directory_pattern'].format(pattern_id=pattern_id,
                                                                                         run=run['name'])
            if os.path.exists(directory):
                shutil.rmtree(directory)
            os.mkdir(directory)


def run_experiment_3(dataset):
    logging.getLogger('Experiment 2').info('Starting experiment 2 on ' + dataset)
    if not check_dataset_inputs(dataset):
        logging.getLogger('Experiment 2').critical('Missing inputs ==> Abort')
    clear_dataset_outputs_3(dataset)

    inputs = datasets_inputs[dataset]
    outputs = experiment_3_outputs

    exe_dir = get_exe_directory(inputs['build_type'])
    input_background_graph = inputs['root_directory'] + inputs['input_background_graph']

    for pattern_id, pattern in enumerate(inputs['pattern_list']):
        for run in outputs['runs']:
            command_line_exe = exe_dir + run['exe']
            command_line = [command_line_exe, '-i', input_background_graph]
            if 'compiled_output' in pattern:
                output_background_graph_binary = result_dir + dataset + '/' + pattern['compiled_output']
                command_line = command_line + ['-b', output_background_graph_binary]

            input_vertex_data = inputs['root_directory'] + pattern['vertex_data']
            command_line = command_line + ['-v', input_vertex_data]

            input_pattern_direcory = inputs['root_directory'] + pattern['directory']
            command_line = command_line + ['-p', input_pattern_direcory]

            output_results_directory = result_dir + dataset + '/' + outputs['directory_pattern'].format(
                pattern_id=pattern_id, run=run['name'])
            command_line = command_line + ['-o', output_results_directory]

            subprocess.run(command_line, check=True)


experiments_handler = {
    1: run_experiment_1,
    2: run_experiment_2,
    3: run_experiment_3
}


def build_all():
    subprocess.run(['make', 'all'], check=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run patternmatching tests.')
    parser.add_argument('--experiment', choices=['1', '2', '3', 'all'],
                        default='all', help='experiment to run (default="all")')
    parser.add_argument('--dataset', choices=['test_1','test_2','test_3','test_4', 'test_all',
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

    build_all()

    if not os.path.exists(result_dir):
        os.mkdir(result_dir)

    for experiment in experiments:
        for dataset in datasets:
            experiments_handler[experiment](dataset)

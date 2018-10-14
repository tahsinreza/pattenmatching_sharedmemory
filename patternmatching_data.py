
result_dir = 'run_patternmatching_result/'

datasets_inputs = {
    'test_1': {
        'root_directory': 'data/patternmatching/test_1/',
        'input_background_graph': 'graph.totem',
        'build_type': 32,
        'pattern_list': [
            {
                'directory': 'pattern_1/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    },
    'test_2': {
        'root_directory': 'data/patternmatching/test_2/',
        'input_background_graph': 'graph.totem',
        'build_type': 32,
        'pattern_list': [
            {
                'directory': 'pattern_1/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    },
    'test_3': {
        'root_directory': 'data/patternmatching/test_3/',
        'input_background_graph': 'graph.totem',
        'build_type': 32,
        'pattern_list': [
            {
                'directory': 'pattern_1/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    },
    'test_4': {
        'root_directory': 'data/patternmatching/test_4/',
        'input_background_graph': 'graph.totem',
        'build_type': 32,
        'pattern_list': [
            {
                'directory': 'pattern_1/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    },
    'IMDB': {
        'root_directory': 'data/patternmatching/IMDB/',
        'input_background_graph': 'graph.totem',
        'build_type': 64,
        'pattern_list': [
            {
                'directory': 'pattern_1/',
                'vertex_data': 'vertex_data/graph_vertex_data_1.totem',
                'compiled_output': 'graph_1.totem.bin'
            },
            {
                'directory': 'pattern_2/',
                'vertex_data': 'vertex_data/graph_vertex_data_2.totem',
                'compiled_output': 'graph_2.totem.bin'
            },
            {
                'directory': 'pattern_3/',
                'vertex_data': 'vertex_data/graph_vertex_data_2.totem',
                'compiled_output': 'graph_2.totem.bin'
            }
        ]
    },
    'patent': {
        'root_directory': 'data/patternmatching/patent/',
        'input_background_graph': 'graph.totem',
        'build_type': 32,
        'pattern_list': [
            {
                'directory': 'pattern_4/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_6/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_7/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_8/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    },
    'youtube': {
        'root_directory': 'data/patternmatching/youtube/',
        'input_background_graph': 'graph.totem',
        'build_type': 32,
        'pattern_list': [
            {
                'directory': 'pattern_4/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_6/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_7/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_8/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    },
    'reddit': {
        'root_directory': 'data/patternmatching/reddit/',
        'input_background_graph': 'graph.totem',
        'build_type': 64,
        'pattern_list': [
            {
                'directory': 'pattern_1/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            },
            {
                'directory': 'pattern_2/',
                'vertex_data': 'vertex_data/graph_vertex_data.totem',
                'compiled_output': 'graph.totem.bin'
            }
        ]
    }
}

experiment_1_outputs = {
    'directory_pattern': 'result_exp_1_{pattern_id}_{run}',
    'global_output_dir':'result_exp_1/',
    'runs': [
        {
            'name': 'none',
            'exe': 'patternmatching_test_optimization_none'
        },
        {
            'name': 'early_termination',
            'exe': 'patternmatching_test_optimization_early_termination'
        },
        {
            'name': 'work_aggregation',
            'exe': 'patternmatching_test_optimization_work_aggregation'
        },
        {
            'name': 'multiple_validation',
            'exe': 'patternmatching_test_optimization_multiple_validation'
        },
        {
            'name': 'all',
            'exe': 'patternmatching_test_optimization_all'
        }
    ]
}

experiment_2_outputs = {
    'directory_pattern': 'result_exp_2_{pattern_id}_{run}',
    'global_output_dir':'result_exp_2/',
    'runs': [
        {
            'name': 'intuitive',
            'exe': 'patternmatching_test_order_intuitive'
        },
        {
            'name': 'effectiveness',
            'exe': 'patternmatching_test_order_effectiveness'
        }
    ]
}

experiment_3_outputs = {
    'directory_pattern': 'result_exp_3_{pattern_id}_{run}',
    'global_output_dir':'result_exp_3/',
    'runs': [
        {
            'name': 'intuitive',
            'exe': 'patternmatching_test_order_enumerate_intuitive'
        },
        {
            'name': 'effectiveness',
            'exe': 'patternmatching_test_order_enumerate_effectiveness'
        },
        {
            'name': 'effectiveness_early',
            'exe': 'patternmatching_test_order_enumerate_effectiveness_early'
        }
    ]
}
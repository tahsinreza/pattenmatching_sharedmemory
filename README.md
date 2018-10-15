
# PruneJuice on shared memory

This is a single node implementation of the patternmatching pipeline shown in _Reza, T., Ripeanu, N., Tripoul, N., Sanders, G., and Pearce, R. PruneJuice: Pruning Trillion-edge Graphs to a Precise Pattern-Matching Solution. In Proceedings of The IEEE/ACM International Conference for High Performance Computing, Networking, Storage, and Analysis (SC'18), Dallas, Texas, 11 - 16 November, 2018._

The main contribution to the distributed memory system is the ability to dynamically aggregate topological information from the graph to inform the pruning heuristic. It is described in  _Tripoul, N., Halawa, H., Reza, T., Ripeanu, M., Sanders, G., and Pearce, R. There are Trillions of Little Forks in the Road. Choose Wisely! - Estimating the Cost and Likelihood of Success of Constrained Walks to Optimize a Graph Pruning Pipeline. In Proceedings of The 8th Workshop on Irregular Applications: Architectures and Algorithms (IA^3'18), co-located with SC'18, Dallas, Texas, 11 - 16 November, 2018._


## Installation
This software requires cmake >=3.10

```asm
sudo apt install cmake
cmake --version
```

The TOTEM Framwork upon which is built the pattern matching algorithm requires both CUDA >=9.1 and the Intel Threading Building Block library.
```asm
sudo apt install nvidia-cuda-toolkit gcc-6
nvcc --version

sudo mkdir /usr/local/cuda
sudo mkdir /usr/local/cuda/bin
sudo ln -s /usr/bin/nvcc /usr/local/cuda/bin/nvcc
```

```asm
sudo apt-get install libtbb-dev
```

Once the prerequisite are installed, you can clone the github and start running an example.

```asm
git clone https://github.com/tahsinreza/pattenmatching_sharedmemory.git
```
Then we can use make to build the cmake configuration and the various executables into the build/ folder.
```asm
make all
```

## Testing the patternmatching algorithm

For convenience, two python script can run and show the results of the experiments in the following paper : [http://www.ece.ubc.ca/~matei/papers/ia3-nicolas.pdf](http://www.ece.ubc.ca/~matei/papers/ia3-nicolas.pdf)

```asm
python run_patternmatching.py --help 

usage: run_patternmatching.py [-h] [--experiment {1,2,3,all}]
                              [--dataset {test_1,test_2,test_3,test_4,test_all,youtube,patent,IMDB,reddit,all}]

Run patternmatching tests.

optional arguments:
  -h, --help            show this help message and exit
  --experiment {1,2,3,all}
                        experiment to run (default="all")
  --dataset {test_1,test_2,test_3,test_4,test_all,youtube,patent,IMDB,reddit,all}

```

The test dataset are already included inside the repository at 'data/patternmatching/test_*'.
To launch the experiments on those datasets, the following command can be run. This should take a few seconds to finish.
```asm
python run_patternmatching.py --dataset test_all --experiment all
```

When this is done, the result can be displayed through 
```asm
python show_patternmatching_results.py --dataset test_all --experiment all
```

It accepts the same arguments as `run_patternmatching.py`.

## Additional datasets

The additional datasets are huge (1GB - 64GB). They can be downloaded here :
- Youtube (1GB) http://www.ece.ubc.ca/~treza/Youtube
- Patent (1GB) http://www.ece.ubc.ca/~treza/Patent
- IMDB (1GB) http://www.ece.ubc.ca/~treza/IMDB
- Reddit (64GB) http://www.ece.ubc.ca/~treza/Reddit

They should be put inside the folder `data/patternmatching/`.

The youtube and patent datasets use simple patterns, on a large-memory (512GB) machine: 4 socket Intel(R) Xeon(R) CPU E5-2670.v2 @2.50GHz (40 cores) less than a minute to complete.

The IMDB dataset has harder patterns and can take several hours to complete.

The Reddit dataset is huge with very complex pattern. A large memory machine is required (More than 128 GB of RAM). It can take ten's of hours to terminates.



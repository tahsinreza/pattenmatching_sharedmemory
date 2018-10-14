#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os, sys, subprocess
sys.path.append(os.path.dirname(os.path.realpath(__file__)))
import argparse 

    
def main(inputFileName, outputFileName, directed=False):
    lastline = str(subprocess.check_output(['tail', '-1', inputFileName]).decode('UTF-8'))
    nodeNumberStr=str(int(lastline.split(' ')[0])+1)
    edgesNumberStr = str(subprocess.check_output(['wc', '-l', inputFileName]).decode('UTF-8')).split(' ')[0]
    fileSize = int(subprocess.check_output(['stat', '--printf="%s"', inputFileName]).decode('UTF-8').replace('"',''))
    chunkSize=4096
    with open(inputFileName) as inputFile, open(outputFileName,'w') as outputFile:
        outputFile.write('#Nodes: '+nodeNumberStr+'\n')
        outputFile.write('#Edges: '+edgesNumberStr+'\n')
        if directed:
            outputFile.write('#Directed'+'\n')
        else:
            outputFile.write('#Undirected'+'\n')
                             
        i=0
        for chunk in iter(lambda: inputFile.read(chunkSize), ''):
            if(i % 1000):
                print('Current position : %d / %d'%(i*chunkSize,fileSize))
            outputFile.write(chunk)
            i+=1
    inputFile.close()
    outputFile.close()
    print('Converted file ' + inputFileName + ' into '+outputFileName)
    
main('data/patternmatching/IMDB/pattern_1/test_1/pattern_edge', 'data/patternmatching/IMDB/pattern_1/test_1/pattern_edge.totem')
#main('data/patternmatching/IMDB/all.sorted.edge', 'data/patternmatching/IMDB/graph.totem')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='convert_edges2totem.py', description='Generate constraint for pattern matching.')
    parser.add_argument('-i', '--input', help='Input edges', required=True)
    parser.add_argument('-o', '--output', help='Output file', required=True)
    parser.add_argument('-d', '--directed', help='Directed graph',action='store_true')
    try :
        args=parser.parse_args()
        
        inputEdges=args.input
        outputFile=args.output
        directed=args.directed
            
        main(inputEdges, outputFile, directed)
    except SystemExit:
        pass

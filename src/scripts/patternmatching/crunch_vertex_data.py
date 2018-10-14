#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os, sys, subprocess
sys.path.append(os.path.dirname(os.path.realpath(__file__)))
import argparse 

    
def main(inputFileList, outputFileName):
    firstIdList=[]
    for inputFileName in inputFileList:
        firstId = int(subprocess.check_output(['head', '-n', '1', inputFileName]).decode('UTF-8').split(' ')[0])
        firstIdList.append(firstId)
        
    print(firstIdList)
            
    chunkSize=4096
    with open(outputFileName,'w') as outputFile:
        for inputFileName in inputFileList:
            with open(inputFileName) as inputFile:                             
                print('Processing file : ' + inputFileName)
                for chunk in iter(lambda: inputFile.read(chunkSize), ''):
                    outputFile.write(chunk)
            inputFile.close()
    outputFile.close()
    print('Converted files into '+outputFileName)

inputDirectory='data/patternmatching/IMDB_PARSED/vertex_data/test2_3/'
main([inputDirectory+'vertex_data.actresses.list.vertex_data',\
      inputDirectory+'vertex_data.actors.list.vertex_data',\
      inputDirectory+'vertex_data.directors.list.vertex_data',\
      inputDirectory+'vertex_data.movie.list.vertex_data',\
      inputDirectory+'vertex_data.genres.list.vertex_data'],\
    inputDirectory+'graph_vertex_data.totem')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='crunch_vertex_data.py', description='Generate constraint for pattern matching.')
    parser.add_argument('-i', '--input_list', help='Input edges', nargs='+', required=True)
    parser.add_argument('-o', '--output', help='Output file', required=True)
    try :
        args=parser.parse_args()
        
        inputEdges=args.input_list
        outputFile=args.output
            
        main(inputEdges, outputFile, directed)
    except SystemExit:
        pass


#!/usr/bin/env python
# a stacked bar plot with errorbars
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import pylab
import sys,csv
import matplotlib.font_manager as fm

def plotFile(filename, outfile):



    entries = []
    with open(filename, 'r') as f:
        for line in f:
            if line[0] != '#':
                lineArray = line.split(' ')
                name = lineArray[0]
                build = float(lineArray[2]) / 100.0
                query = float(lineArray[4]) / 100.0
                update = float(lineArray[6]) / 100.0
                entries.append((name, build, query, update))
                

    entries.reverse()


    #names = [entry[0] for entry in entries]

    names = ["R-Tree ", "Synch. Traversal ", "TPR-Tree ", "STRIPES "]
    names.reverse()

    build = [entry[1] for entry in entries]
    query = [entry[2] for entry in entries]
    update = [entry[3] for entry in entries]


    pos = np.arange(len(entries)) + 0.5

    
    font = {'family' : 'Bitstream Vera Sans',
            'size'  : 20}

    matplotlib.rc('font', **font)

    fig = plt.figure(figsize=(10,4))
    fig.subplots_adjust(left=0.25, right=0.90)
    ax = fig.add_subplot(111)


    p1 = plt.barh(pos, build, align='center', color='b')
    p2 = plt.barh(pos, query, left = build, align='center', color='#F2EF16')
    p3 = plt.barh(pos, update, left = [build[i] + query[i] for i in range(len(entries))], align='center', color='r')
    ax.tick_params(pad=10)

    plt.yticks(pos, names)

    for tick in pylab.gca().xaxis.iter_ticks():
        tick[0].label2On = True
        tick[0].label1On = False
        tick[0].tick1On = False
        #tick[0].label2.set_rotation('vertical')
        

    #plt.xlim(xmin=0, xmax=4.5)

    plt.legend( (p1[0], p2[0], p3[0]), ('Build', 'Query', 'Update') )
    #plt.xlabel('Avg. Time per Tick (s)')
    #plt.grid(True)

    plt.savefig(outfile, format='pdf')
#    plt.show()


if __name__ == '__main__':
    plotFile('out/outfiles/Network/OLNetworkBreakdown.txt', 'out/pdf/Network/OLNetwork.pdf')
    plotFile('out/outfiles/Network/SGNetworkBreakdown.txt', 'out/pdf/Network/SGNetwork.pdf')
    plotFile('out/outfiles/Network/SANetworkBreakdown.txt', 'out/pdf/Network/SANetwork.pdf')


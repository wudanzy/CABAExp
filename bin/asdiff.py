#!/usr/bin/env python
# author:   wudanzy
# time:     2015-10-23
#
# the file figure out the difference between two AS-relationship files
# and assign as numbers for the new ASes
#
import sys
import networkx as nx
import heapq


class TopoGraph(nx.DiGraph):
    graph = None;
    fname = None;

    def __init__(self, filename):
        self.fname = filename
        self.graph = nx.DiGraph();
        self.readConfig(self.fname)
        for index in self.graph:
            self.graph.node[index]['new'] = 1
            self.graph.node[index]['range'] = []
            self.graph.node[index]['as'] = 0
        for index in self.graph:
            if 'ref' not in self.graph.node[index]:
                self.graph.node[index]['ref'] = 0
    
    def refReset(self):
        for index in self.graph:
            self.graph.node[index]['ref'] = 0
        for index in self.graph:
                for neigh in self.graph.neighbors_iter(index):
                    if self.graph.edge[index][neigh]['relation']==1:
                        self.refInc(index);

    def refInc(self, index):
        if 'ref' in self.graph.node[index]:
            self.graph.node[index]['ref'] += 1
        else:
            self.graph.node[index]['ref'] = 1

    def refDec(self, index):
        if 'ref' in self.graph.node[index]:
            self.graph.node[index]['ref'] -= 1
        else:
            raise

    def allocateASN(self, lst):
        self.refReset()
        for index in self.graph:
            if self.graph.node[index]['new'] == 0:
                if index <= 65535:
                    self.graph.node[index]['range'].append((0,65535))
                    self.graph.node[index]['as'] = index
                else:
                    asnHigh = index/65536
                    self.graph.node[index]['range'].append((asnHigh,asnHigh))
                    self.graph.node[index]['as'] = index
            else:
                if index <= 65535:
                    self.graph.node[index]['range'].append((0,65535))
                    self.graph.node[index]['as'] = index
                elif self.graph.node[index]['ref'] == 0:
                    asnHigh = index/65536
                    self.graph.node[index]['range'].append((asnHigh,asnHigh))
                    self.graph.node[index]['as'] = index
                    

        for index in lst:
            if len(self.graph.node[index]['range']) == 0:
                success = True 
                for neigh in self.graph.neighbors_iter(index):
                    if self.graph.edge[index][neigh]['relation']==1 and len(self.graph.node[neigh]['range']) > 0:
                        (lowBound, highBound) = self.graph.node[neigh]['range'][0];
                        if highBound > lowBound + 255:
                            middle = highBound - 255;
                        else:
                            middle1 = (lowBound + 1 + highBound)/2;
                            middle = (middle1 + 1 + highBound)/2;
                        if (middle > lowBound):
                            self.graph.node[neigh]['range'][0] = (lowBound,middle-1)
                            self.graph.node[index]['range'].append((middle, highBound))
                            self.graph.node[index]['as'] = middle * 65536 + self.graph.node[neigh]['as']%65536
                            break

        for index in self.graph:
            if self.graph.node[index]['as'] == 0:
                self.graph.node[index]['as'] = index

        asSet = set([])
        for index in self.graph:
            if self.graph.node[index]['as'] in asSet:
                print "in sets ", index, self.graph.node[index],
                for neigh in self.graph.neighbors_iter(index):
                    if self.graph.edge[index][neigh]['relation']==1:
                        print neigh, self.graph.node[neigh],
                print ""
            else:
                asSet.add(self.graph.node[index]['as'])


    def topoSequence(self):
        h = []
        seq = []
        inseq = set([])
        for index in self.graph:
            heapq.heappush(h, (self.graph.node[index]['ref'], index));
        while len(h) > 0:
            (ref, index) = heapq.heappop(h);
            if index not in inseq:
                seq.append(index);
                inseq.add(index)
                for neigh in self.graph.neighbors_iter(index):
                    if self.graph.edge[index][neigh]['relation']==-1:
                        self.refDec(neigh);
                        heapq.heappush(h, (self.graph.node[neigh]['ref'], neigh));
        return seq

    def makeOld(self, index):
        if index in self.graph:
            self.graph.node[index]['new'] = 0
        # else:
            # print "don't have ", index

    def splitstr(self, line, pat):
        ele = [];
        i = 0;
        tmpstr = "";
        while i <= len(line):
            if i < len(line) and line[i] != pat:
                tmpstr = tmpstr + line[i];
            else:
                if tmpstr != "":
                    ele.append(int(tmpstr));
                    tmpstr = "";
            i = i + 1;
        return ele;

    def readnextcmd(self, fh):
        try:
            line = fh.readline();
            while len(line) > 0 and (line[0]=='#' or len(self.splitstr(line[:-1],'|')) == 0):
                line = fh.readline();
            return self.splitstr(line[:-1], '|');
        except:
            print "Exception: ",sys.exc_info()[0];
            raise;
            
    def readConfig(self, filename):
        try:
            f = open(filename, "r");
            cmd = self.readnextcmd(f);
            while len(cmd) > 0:
                self.graph.add_edge(cmd[0], cmd[1], relation=cmd[2]);
                self.graph.add_edge(cmd[1], cmd[0], relation=-cmd[2]);
                if cmd[2] == -1:
                    self.refInc(cmd[1]);        
                cmd = self.readnextcmd(f);
            f.close();
        except:
            print "Exception: ", sys.exc_info()[0];
            raise;




if __name__ == '__main__':

    if len(sys.argv) < 3:
        print "Usage: assdiff.py asold asnew\n";
        sys.exit(-1);        

    old = TopoGraph(sys.argv[1]);
    new = TopoGraph(sys.argv[2]);
    for index in old.graph:
        new.makeOld(index);

    seq = new.topoSequence()
    allocSequence = [n for n in seq if new.graph.node[n]['new'] == 1]
    new.allocateASN(allocSequence)
    for index in new.graph:
        if new.graph.node[index]['as'] != index:
            print index, new.graph.node[index]['as']

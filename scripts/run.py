#!/bin/env python

# Copyright 2011 Cornell Database Group
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


#
#
#
# Currently assumes that any target corresponding to a single experiment has the
# same name as the filename containing the experiment (without the path or the
# '.h')
#
#
#


import sys, os, subprocess

def parse_targets(config_file):

    target_dict = {}
    curr_target = ''
    
    with open(config_file, "r") as f:
        for line in f:
            line = line.strip()
            if  len(line) > 0 and line[0] != '#':
                if line[-1] == ':':
                    curr_target = line[0:-1]
                    target_dict[curr_target] = []
                else:
                    target_dict[curr_target].append(line)

    return target_dict


# Note: This method currently does not currently check whether an experiment
# exists before attempting to run it. In the future we could check for the
# existance of the appropriate experiment using the 'os.walk' function.
#
# subprocess.Popen blocks on output 
#
#
def run(target_dict, target):

    if target in target_dict:
        for new_target in target_dict[target]:
            run(target_dict, new_target)

    else:

        command = ["./bin/index.exec", "experiment", "--name=" + target]

        print "Starting Experiment", target, " ".join(command)
        retcode = subprocess.call(command)

        if(retcode < 0):
            print "ERROR: Exited with signal ", (-1 * retcode)

        print "Finished Experiment", target


if __name__ == '__main__':
    # target = "SimpleGrid"
    # print "arg=", sys.argv[1]

    target_dict = parse_targets("scripts/experiments.config")
    run(target_dict, sys.argv[1])
    

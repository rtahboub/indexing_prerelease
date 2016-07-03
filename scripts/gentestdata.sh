#!/bin/bash

function docmd() {
    echo "$1"
    if( eval "(" $1 ")" )
    then 
        true
    else
	echo "Command failed"
	exit 1
    fi
}


# ----------------------------------------------------------------------------
# Scale num points small. 
# ----------------------------------------------------------------------------

# for p in 1000 3000 5000 7000 9000
# do
#     max=$(echo "sqrt($p. * 1000.0)" | bc)
#     docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints$p.dat' --binary=true --int=false --numPoints=$p --numTicks=10 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=40 --qMax=40 --maxSpeed=45 --numSpeeds=10"
# done



# for p in 10000 30000 50000 70000 90000
# do
#     max=$(echo "sqrt($p. * 10000.0)" | bc)
#     docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}short.dat' --binary=true --int=false --numPoints=$p --numTicks=10 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
# done



# for t in 1 2 3
# do
#     for p in 1000 3000 5000 7000 9000
#     do
#         max=$(echo "sqrt($p. * 1000.0)" | bc)
#         docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}conttrial${t}.dat' --binary=true --int=false --contQueries=true --numPoints=$p --numTicks=10 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=40 --qMax=40 --maxSpeed=45 --numSpeeds=10"
#     done
# done


# ----------------------------------------------------------------------------
# Scale num points large. 
# ----------------------------------------------------------------------------


#for p in 10000 30000 50000 70000 90000
#do
#    max=$(echo "sqrt($p. * 10000.0)" | bc)
#    docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}.dat' --binary=true --int=false --numPoints=$p --numTicks=100 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
#done



# for t in 1 2 3
# do
#     for p in 10000 30000 50000 70000 90000
#     do
#  	max=$(echo "sqrt($p. * 10000.0)" | bc)
#  	docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}trial${t}.dat' --binary=true --int=false --numPoints=$p --numTicks=100 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
#     done
# done



for t in 1 2 3
do
    for p in 100000 300000 500000 700000 900000
    do
        max=$(echo "sqrt($p. * 100000.0)" | bc)
        docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}trial${t}.dat' --binary=true --int=false --numPoints=$p --numTicks=100 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=4000 --qMax=4000 --maxSpeed=2000 --numSpeeds=200"
    done
done



# Continuous Queries

# for t in 1
# do
#     for p in 10000 30000 50000 70000 90000
#     do
#  	max=$(echo "sqrt($p. * 10000.0)" | bc)
#  	docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}conttrial${t}.dat' --binary=true --contQueries=true --int=false --numPoints=$p --numTicks=100 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
#  	#docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}small_updateconttrial${t}.dat' --binary=true --contQueries=true --int=false --numPoints=$p --numTicks=100 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.01 --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
#     done
# done

# ----------------------------------------------------------------------------
# Scale num points huge
# ----------------------------------------------------------------------------
# for t in 1 2 3
# do
#     for p in 100000 300000 500000 700000 900000
#     do
# 	max=$(echo "sqrt($p. * 100000.0)" | bc)
# 	docmd "./bin/datagen.exec uniform --outfile='traces/points/scalepoints${p}trial${t}.dat' --binary=true --int=false --numPoints=$p --numTicks=100 --maxX=$max --maxY=$max --probQuery=0.5 --probUpdate=0.5 --qMin=4000 --qMax=4000 --maxSpeed=2000 --numSpeeds=200"
#     done
# done

# ----------------------------------------------------------------------------
# Scale num queries and updates. 
# ----------------------------------------------------------------------------
# for q in 0.1 0.3 0.5 0.7 0.9
# do
#     for u in 0.1 0.3 0.5 0.7 0.9
#     do
#         ./bin/datagen.exec uniform --outfile="traces/query$q.update$u.dat" --binary=true --int=false --numPoints=10000 --numTicks=100 --maxX=1000 --maxY=1000 --probQuery=$q --probUpdate=$u --qMin=25 --qMax=75 --maxSpeed=45 --numSpeeds=10
#     done
# done

# ----------------------------------------------------------------------------
# Scale num queries and updates large. 
# ----------------------------------------------------------------------------

for t in 1 2 3 
do
    for q in 0.1 0.3 0.5 0.7 0.9
    do
	for u in 0.1 0.3 0.5 0.7 0.9
	do
            #docmd "./bin/datagen.exec uniform --outfile=traces/query_update/query$q.update${u}trial${t}.dat --binary=true --int=false --numPoints=50000 --numTicks=100 --maxX=22361 --maxY=22361 --probQuery=$q --probUpdate=$u --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
            docmd "./bin/datagen.exec uniform --outfile=traces/query_update/query$q.update${u}conttrial${t}.dat --binary=true --contQueries=true --int=false --numPoints=50000 --numTicks=100 --maxX=22361 --maxY=22361 --probQuery=$q --probUpdate=$u --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
	done
    done
done






# ----------------------------------------------------------------------------
# Scale num queries and updates small. 
# ----------------------------------------------------------------------------
# for q in 0.1 0.3 0.5 0.7 0.9
# do
#     for u in 0.1 0.3 0.5 0.7 0.9
#     do
#         ./bin/datagen.exec uniform --outfile="traces/query$q.update${u}small.dat" --binary=true --int=false --numPoints=5000 --numTicks=100 --maxX=2236 --maxY=2236 --probQuery=$q --probUpdate=$u --qMin=40 --qMax=40 --maxSpeed=20 --numSpeeds=4
#     done
# done




# ----------------------------------------------------------------------------
# Scale queries and updates small for moving crossover test. 
# ----------------------------------------------------------------------------
# for q in 0.01 0.05 0.09
# do
#     for u in 0.01 0.05 0.09
#     do
#         ./bin/datagen.exec uniform --outfile="traces/query$q.update$u.dat" --binary=true --int=false --numPoints=50000 --numTicks=100 --maxX=22361 --maxY=22361 --probQuery=$q --probUpdate=$u --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20
#     done
# done


# ----------------------------------------------------------------------------
# Scale queries and updates small for grant crossover test. 
# ----------------------------------------------------------------------------
# for q in 0.001 0.011 0.021 0.031 0.041 0.051 0.061 0.071 0.081 0.091 
# do
#     ./bin/datagen.exec uniform --outfile="traces/query$q.update0.1.dat" --binary=true --int=false --numPoints=10000 --numTicks=100 --maxX=1000 --maxY=1000 --probQuery=$q --probUpdate=0.1 --qMin=25 --qMax=75 --maxSpeed=45 --numSpeeds=10

# done




# ----------------------------------------------------------------------------
# Scale num hotspots for gaussian
# ----------------------------------------------------------------------------


# for t in 1 2 3
# do 
#     for h in 1 10 100 1000
#     do
# 	docmd "./bin/datagen.exec gaussian --outfile=\"traces/gauss/gauss${h}trial${t}.dat\" --binary=true --numPoints=50000 --numTicks=120 --space=22361 --probQuery=0.5 --insertTicks=20 --maxTup=20 --numHotspots=$h --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
# #	docmd "./bin/datagen.exec gaussian --outfile=\"traces/gauss/gauss${h}conttrial${t}.dat\" --binary=true --numPoints=50000 --contQueries=true --numTicks=120 --space=22361 --probQuery=0.5 --insertTicks=20 --maxTup=20 --numHotspots=$h --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"
#     done
# done 


# ----------------------------------------------------------------------------
# Huge Test
# ----------------------------------------------------------------------------

# docmd "./bin/datagen.exec uniform --outfile='traces/hugetest.dat' --binary=true --int=false --numPoints=3000000 --numTicks=5 --maxX=173205 --maxY=173205 --probQuery=0.00033 --probUpdate=0.001 --qMin=400 --qMax=400 --maxSpeed=200 --numSpeeds=20"

#docmd "./bin/datagen.exec uniform --outfile='traces/hugetest.dat' --binary=true --int=false --numPoints=10000000 --numTicks=3 --maxX=577350 --maxY=577350 --probQuery=0.0001 --probUpdate=0.001 --qMin=1333 --qMax=1333 --maxSpeed=666  --numSpeeds=66"



#docmd "./bin/datagen.exec uniform --outfile='traces/aework.dat' --binary=true --contQueries=true --int=false --numPoints=1000 --numTicks=3 --maxX=1000 --maxY=1000 --probQuery=0.1 --probUpdate=0.1 --qMin=50 --qMax=50 --maxSpeed=50  --numSpeeds=5"

# docmd "./bin/datagen.exec uniform --outfile='traces/hugetest.dat' --binary=true --int=false --numPoints=10000000 --numTicks=3 --maxX=577350 --maxY=577350 --probQuery=0.1 --probUpdate=0.001 --qMin=1333 --qMax=1333 --maxSpeed=666  --numSpeeds=66"



# for q in 0.001 0.005 0.009 
# do
#     for u in 0.001 0.005 0.009 
#     do
#         docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery$q.update$u.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=57735 --maxY=57735 --probQuery=$q --probUpdate=$u --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"
        
#     done
# done


# for i in 1 2 3
# do
#     docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.0001.update0.001trial${i}.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=577350 --maxY=577350 --probQuery=0.0001 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"

#     docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.0001.update0.001trial${i}cont.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --contQueries=true --maxX=577350 --maxY=577350 --probQuery=0.0001 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"

#     docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.0001.update0.01trial${i}.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=577350 --maxY=577350 --probQuery=0.0001 --probUpdate=0.01 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"

#     docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.0001.update0.01trial${i}cont.dat' --binary=true --contQueries=true --int=false --numPoints=1000000 --numTicks=100 --maxX=577350 --maxY=577350 --probQuery=0.0001 --probUpdate=0.01 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"

#     docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.001.update0.001trial${i}.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=577350 --maxY=577350 --probQuery=0.001 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"

#     docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.001.update0.001trial${i}cont.dat' --binary=true --contQueries=true --int=false --numPoints=1000000 --numTicks=100 --maxX=577350 --maxY=577350 --probQuery=0.001 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"
# done

# docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.01.update0.001.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=57735 --maxY=57735 --probQuery=0.01 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"
# docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.01.update0.001cont.dat' --binary=true --int=false --contQueries=true --numPoints=1000000 --numTicks=100 --maxX=57735 --maxY=57735 --probQuery=0.01 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"


# docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.001.update0.001.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=57735 --maxY=57735 --probQuery=0.01 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"
# docmd "./bin/datagen.exec uniform --outfile='traces/query_update/largequery0.001.update0.001cont.dat' --binary=true --int=false --contQueries=true --numPoints=1000000 --numTicks=100 --maxX=57735 --maxY=57735 --probQuery=0.01 --probUpdate=0.001 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"








# docmd "./bin/datagen.exec uniform --outfile='traces/largequery0.001.update0.50.dat' --binary=true --int=false --numPoints=1000000 --numTicks=100 --maxX=57735 --maxY=57735 --probQuery=0.001 --probUpdate=0.50 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"


#docmd "./bin/datagen.exec uniform --outfile='traces/largequery0.1.update0.1.dat' --binary=true --int=false --numPoints=1000000 --numTicks=10 --maxX=57735 --maxY=57735 --probQuery=0.1 --probUpdate=0.10 --qMin=600 --qMax=600 --maxSpeed=600  --numSpeeds=60"






# ----------------------------------------------------------------------------
# Fish Experiments
# ----------------------------------------------------------------------------

# for p in 10000 30000 50000 70000 90000 
# do 
#     ./bin/datagen.exec fish --outfile="traces/scalefish$p.dat" --numPoints=$p --minX=1500 --maxX=2500 --minY=1500 --maxY=2500 --alpha=25 --rho=160 --probInformed=.10 --numGoals=10 --numTicks=100 --binary=true --minSpeed=8 --maxSpeed=12
# done

# for p in 0.1 0.3 0.5 0.7 0.9 
# do 
#     ./bin/datagen.exec fish --outfile="traces/fishinformed$p.dat" --numPoints=50000 --minX=1500 --maxX=2500 --minY=1500 --maxY=2500 --alpha=25 --rho=160 --probInformed=$p --numGoals=10 --numTicks=100 --binary=true --minSpeed=8 --maxSpeed=12
# done

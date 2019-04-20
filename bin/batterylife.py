#!/usr/bin/python3

## This is a quick little python script that takes the output of
## energytrace-util, averages the current consumption for all samples
## after ten seconds, and then predicts battery life from a 100mAH
## CR2016 battery.

## Usage: et 60 | python3 batterylife.py


import sys;

capacity=100 #mAH

ampsum=0.0;
ampcount=0;

#Ugly shotgun parser to ignore comments and early records.
for line in sys.stdin:
    if line[0]=='#':
        pass;
    else:
        words=line.split();
        time=float(words[0]);
        amps=float(words[1]);
        milliamps=amps*1000.0;

        #We only count after the first 20 seconds, as booting takes 5 to 10 seconds.
        if time>20.0:
            ampcount=ampcount+1;
            ampsum=ampsum+amps;

if ampcount>0:
    ampavg=ampsum/(ampcount*1.0);
    milliamp=ampavg*1000.0;
    microamp=ampavg*1000000.0;
    print("%f µA average consumption"%microamp);
    hours=100/milliamp;
    days=hours/24.0;
    years=days/365.2425;
    months=years*12.0;
    print("%f months of CR2016 battery life."%months);
    if months<1:
        print("%f hours of CR2016 battery life."%hours);

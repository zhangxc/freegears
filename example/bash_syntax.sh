#!/bin/bash
T1="foo"
T2="bar"
if [ "$T1" = "$T2" ]; then
    echo expression evaluated as true
else
    echo expression evaluated as false
fi

#!/bin/bash
for i in `seq 1 10`;
do
    echo $i
done    

#!/bin/bash 
COUNTER=0
while [  $COUNTER -lt 10 ]; do
    echo The counter is $COUNTER
    let COUNTER=COUNTER+1 
done

#!/bin/bash 
COUNTER=20
until [  $COUNTER -lt 10 ]; do
    echo COUNTER $COUNTER
    let COUNTER-=1
done

#!/bin/bash
OPTIONS="Hello Quit"
select opt in $OPTIONS; do
if [ "$opt" = "Quit" ]; then
    echo done
    break
elif [ "$opt" = "Hello" ]; then
    echo Hello World
else
    clear
    echo bad option
fi
done

#!/bin/bash
echo Please, enter your name
read NAME
echo "Hi $NAME!"

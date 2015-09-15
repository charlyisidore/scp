#!/bin/bash

# Download all the SCP instances of the OR-library
# See http://people.brunel.ac.uk/~mastjjb/jeb/orlib/scpinfo.html

command -v lynx > /dev/null || { echo "Error: lynx is required to run the script"; exit 1; }
lynx -dump http://people.brunel.ac.uk/~mastjjb/jeb/orlib/files/ | grep -oe 'http://people.brunel.ac.uk/~mastjjb/jeb/orlib/files/\(scp.*\.txt\|rail.*\.gz\)' | wget -i - -nv

from_stn_instance()
{
	read line
	data=($(echo "$line" | tr -d "\r\n"))
	n=${data[0]}
	m=${data[1]}
	c=($(for j in $(seq 1 $n)
	     do echo 1
	     done))
	echo "$m $n"
	echo "${c[@]}"
	for i in $(seq 1 $m)
	do
		read line
		echo "3 $line"
	done
}

stntar=steiner-triple-covering.tar.gz
wget -nv http://mauricio.resende.info/data/steiner-triple-covering.tar.gz -O $stntar
for f in $(tar -ztf $stntar --wildcards --no-anchored 'data.*')
do
	tar -xzf $stntar "$f"
	bn=$(basename "$f")
	i=${bn:5}

	# Keep the instance as is (in "stn" format)
	echo "Extract instance stn$i..."
	mv "$f" "stn$i.txt"

	# Or convert to "scp" format
	#echo "Converting stn$i..."
	#from_stn_instance < "$f" > "stn$i.txt"
done
rm -rf $(tar -ztf $stntar)
rm -f $stntar


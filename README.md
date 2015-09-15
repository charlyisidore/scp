# SCP solver

Author: Charly Lersteau

A Set Cover Problem solver using GRASP [1] and CPLEX [2].



## Manual

    Usage: ./scp [OPTIONS] FILE
    
     Options:
      -a, --alpha=FLOAT          RCL threshold parameter (in [0,1])
      -e, --epsilon=FLOAT        Tolerance
      -f, --format=FORMAT        Instance file format (scp, rail, stn)
      -n INT                     Number of tries
      -q, --quiet                Don't produce any verbose output
      -r, --random=INT           Random seed
      -?, --help                 Give this help list



## Instances


### SCP and RAIL instances

The reader is referred to [3].

There are currently 87 data files.

50 of these data files are the test problem sets 4 to 6 and A to E from [4].

Problem sets 4, 5 and 6 are originally from the Balas and Ho set covering paper.

The following table gives the relationship between these test problem sets and the appropriate files:

    Problem set        Files
    4                  scp41, ..., scp410
    5                  scp51, ..., scp510
    6                  scp61, ..., scp65
    A                  scpa1, ..., scpa5
    B                  scpb1, ..., scpb5
    C                  scpc1, ..., scpc5
    D                  scpd1, ..., scpd5
    E                  scpe1, ..., scpe5

20 of these data files are the test problem sets E to H from [5].

The following table gives the relationship between these test problem sets and the appropriate files:

    Problem set        Files
    E                  scpnre1, ..., scpnre5
    F                  scpnrf1, ..., scpnrf5
    G                  scpnrg1, ..., scpnrg5
    H                  scpnrh1, ..., scpnrh5

10 of these data files are unicost problems from the paper [6].

The files scpcyc06, ..., scpcyc11 correspond to the CYC set of problems (number of edges required to hit every 4-cycle in a hypercube).
The files scpclr10, ..., scpclr13 correspond to the CLR set of problems (number of 4-tuples forming the smallest non-bi-chromatic hypergraph).
These files have been contributed by A. Wool

The format of all of these 80 data files is:

- number of rows (m), number of columns (n)
- the cost of each column c(j), j=1,...,n
- for each row i (i=1,...,m): the number of columns which cover row i followed by a list of the columns which cover row i

For the files associated with the European Journal of Operational Research paper by Beasley [5] :

1. the value of the optimal solution for each of these data files is given in the paper
2. the largest file is scpd5 of size 420Kb (approximately)
3. the entire set of files is of size 5800Kb (approximately)

For the files associated with the Naval Research Logistics paper [6] :

1. heuristic solution values for each of these data files are given in the paper
2. the largest file is scpnrh5 of size 2600Kb (approximately)

There are also 7 data files associated with real-world set covering problems. These data files are:

    rail507 with 507 rows and 63,009 columns
    rail516 with 516 rows and 47,311 columns
    rail582 with 582 rows and 55,515 columns
    rail2536 with 2536 rows and 1,081,841 columns
    rail2586 with 2586 rows and 920,683 columns
    rail4284 with 4284 rows and 1,092,610 columns
    rail4872 with 4872 rows and 968,672 columns

These data files arise from an application in Italian railways and have been contributed by Paolo Nobili.

As might be expected these problems have a number of special characteristics, specifically:

1. all column costs are either one or two
2. a column covers at most 12 rows
3. substantial reductions can be made by applying known row/column reduction tests

The format of these test problems is:

- number of rows (m), number of columns (n)
- for each column j (j=1,...,n): the cost of the column, the number of rows that it covers followed by a list of the rows  that it covers 

The largest file is rail4284 of size 61Mb (approximately).
The entire set of files is of size 260Mb (approximately).


### STN instances

Set covering problems derived from Steiner triple systems [7].

STEINER TRIPLE COVERING PROBLEM test instances

    Let n = number of variables
        m = number of triples
        T(i) = set of 3 indices of i-th triple

The problem is;

    MIN \sum_{j=1]^n x_j
    
    SUBJ. TO:
    
    \sum_{j \in T(i)} x_j \geq 1, for i = 1,...,m,
    
    x_j \in {0,1} for all j = 1,...,n

The following files are in the distribution:

    FILE       : INSTANCE  :    n   :     m
    ............................................
    data.9     : stn9      :    9   :    12
    data.15    : stn15     :   15   :    35   
    data.27    : stn27     :   27   :   117   
    data.45    : stn45     :   45   :   330   
    data.81    : stn81     :   81   :  1080   
    data.135   : stn135    :  135   :  3015   
    data.243   : stn243    :  243   :  9801   
    data.405   : stn405    :  405   : 27270   
    data.729   : stn729    :  729   : 88452   

The file format is:

    Line 1: n m
    Lines 2 to m+1:  3 variable indices for triple

Best known solutions 

    stn9      :   5  optimal (Fulkerson et al., 1974)
    stn15     :   9  optimal (Fulkerson et al., 1974)
    stn27     :  18  optimal (Fulkerson et al., 1974)
    stn45     :  30  optimal (Ratliff, 1979)
    stn81     :  61  optimal (Mannino and Sassano, 1995)
    stn135    : 103  optimal (Ostrowski et al., 2009, 2010)
    stn243    : 198  optimal (Ostrowski et al., 2009, 2010)
    stn405    : 335  BKS     (Resende & Toso, 2010)
    stn729    : 617  BKS     (Resende & Toso, 2010)



## References

[1]: Feo, T. A., & Resende, M. G. (1995). Greedy randomized adaptive search procedures. Journal of global optimization, 6(2), 109-133.

[2]: IBM ILOG CPLEX Optimization Studio. http://www-03.ibm.com/software/products/en/ibmilogcpleoptistud

[3]: Beasley's OR-Library. http://people.brunel.ac.uk/~mastjjb/jeb/orlib/scpinfo.html

[4]: J.E.Beasley "An algorithm for set covering problems" European Journal of Operational Research 31 (1987) 85-93

[5]: J.E.Beasley "A lagrangian heuristic for set-covering problems" Naval Research Logistics 37 (1990) 151-164

[6]: "Computational Experience with Approximation Algorithms for the Set Covering Problem",  by T. Grossman and A. Wool, European Journal of Operational Research 101(1) pages 81-92 (1997)

[7]: Set covering problems derived from Steiner triple systems. http://mauricio.resende.info/data/steiner-triple-covering.tar.gz


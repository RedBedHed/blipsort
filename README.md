# *Multi Pivot Pattern-Defeating Quicksort* (NOTE: THIS IS OUT OF DATE ALREADY. WHOOPS.)
 
A highly-optimized Introsort that draws from many research papers and open-source libraries.

## Techniques

### Multi-Pivot Partitioning
Multi-way partitioning has proven to yeild fewer cache misses than traditional one and two-way partitioning. Furthermore, three-pivot partitioning has proven to be slightly faster than two-pivot partitioning when sorting random arrays on modern computers. As such, the sort employs a multi-pivot partitioning strategy.

### Pivot Selectivity
The sort carefully selects pivots from a set of five sorted candidates. 

* If no candidate pairs are equal, the sort chooses three pivots from the middle elements.
* If a central pair is equal, the sort selects the tercile elements as pivots.
* If any other pair is equal, the sort uses the middle element as a pivot

This process, adapted from java's primitive sort, helps to ensure that we select pivots that divide the data as evenly as possible without sacrificing too much speed. 
The 2 and 3 pivot schemes work better for data without many pivot duplicates. When an inner partition is fairly large, any pivot duplicates must be swapped to the edges with an extra step.
On the contrary, the single pivot scheme works well for data with many pivot duplicates as it automatically collects these in the middle of the interval and ignores them in subsequent partitioning.

### Introsort
The sort is introspective, switching to a guaranteed nlog(n) sort if time complexity trends towards quadratic time. Like PDQsort, the sort switches to Heapsort for an interval after log(n) "bad" partitions&mdash; partitions that are significantly unbalanced.

### Insertion Sort
The sort uses Insertion sort on small intervals where asymptotic complexity matters less and instruction overhead matters more. The sort employs Java's Pair Insertion sort on every interval except the leftmost. Pair insertion sort inserts two elements at a time 
and doesn't need to perform a lower bound check, making it slightly faster than normal insertion sort in the context of quicksort.

### Pivot Retention
Like PDQSort, if a central candidate pivot is equal to the rightmost element of the previous partition, the sort swaps equal elements to the left and continues to the right, yeilding linear time on data comprised of equal elements.

### Optimism
Like PDQSort, if the partition is "good" (not highly unbalanced), and if no swaps were made during partitioning, the sort switches to insertion sort. If the insertion sort makes more than a constant number of moves, the sort bails and resumes quicksort. This allows the sort to achieve linear time on partially or fully sorted data.

### Breaking Patterns
Like PDQSort, if the partition is bad, the sort scrambles some elements to break up patterns.

Sources:

[Here](https://codeblab.com/wp-content/uploads/2009/09/DualPivotQuicksort.pdf)
is the original Dual-Pivot Quicksort by Vladimir
Yaroslavskiy.

[Here](https://www.researchgate.net/publication/289974363_Multi-Pivot_Quicksort_Theory_and_Experiments)
is the three pivot partitioning algorithm by Kushagra, Lopez-Ortiz, Munro, and Qiao.

[Here](https://github.com/orlp/pdqsort)
is the PDQsort algorithm by Orson Peters

[Here](https://](https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/DualPivotQuicksort.java)https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/DualPivotQuicksort.java)
is Java's Dual Pivot Quicksort

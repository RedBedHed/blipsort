# *Branchless-Lomuto in Pattern-Defeating Quicksort (Blipsort)*
 
A highly-optimized (and tiny) Introsort variant that draws from PDQsort, Java, and Orson Peter's branchless Lomuto partitioning.

## Speed

![Speed](https://github.com/RedBedHed/BLPDQsort/blob/main/blipsort_speed.png)

##### *clang 16, -O3*

## Complexity

| Best | Average | Worst | Memory |
|------|---------|-------|--------|
| n    | n log n | n log n | log n |

## Techniques

### Branchless Lomuto
The decades-old partitioning algorithm recently made a resurgence when researchers discovered ways to remove the inner branch. Orson Peter's method&mdash; which he published on his blog a little under two months ago&mdash; is the fastest yet. It employs a gap in the data to move elements twice per iteration rather than swapping them (three moves).

### Pivot Selectivity
Blipsort carefully selects the pivot from the middle of five sorted candidates. These candidates allow the sort to determine whether the data in the current interval is approximately descending and inform its "partition left" strategy.

### Introspection
Blipsort is introspective, switching to a guaranteed nlog(n) sort if it becomes quadratic. Like PDQsort, Blipsort switches to Heapsort after log(n) "bad" partitions&mdash; partitions that are significantly unbalanced.

### Insertion Sort
Blipsort uses Insertion sort on small intervals where asymptotic complexity matters less and instruction overhead matters more. Blipsort employs Java's Pair Insertion sort on every interval except the leftmost. Pair insertion sort inserts two elements at a time 
and doesn't need to perform a lower bound check, making it slightly faster than normal insertion sort in the context of quicksort.

### Pivot Retention
Similar to PDQsort, if any of the three middlemost candidate pivots is equal to the rightmost element of the partition at left, Blipsort moves equal elements to the left with branchless Lomuto and continues to the right, solving the dutch-flag problem and yeilding linear time on data comprised of equal elements.

### Optimism
Similar to PDQsort, if the partition is "good" (not highly unbalanced), Blipsort switches to insertion sort. If the Insertion sort makes more than a constant number of moves, Blipsort bails and resumes quicksort. This allows Blipsort to achieve linear time on already-sorted data.

### Breaking Patterns
Like PDQsort, if the partition is bad, Blipsort scrambles some elements to break up patterns.

### Rotation
When all of the candidate pivots are strictly descending, it is very likely that the interval is descending as well. Lomuto partitioning slows significantly on descending data. Therefore, Blipsort neglects to sort descending candidates and instead swap-rotates the entire interval before partitioning.

## Sources

[Here](https://github.com/orlp/pdqsort)
is the PDQsort algorithm by Orson Peters


[Here](https://orlp.net/blog/branchless-lomuto-partitioning/)
is the branchless Lomuto blog post by Orson Peters

[Here](https://](https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/DualPivotQuicksort.java)https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/DualPivotQuicksort.java)
is Java's Dual Pivot Quicksort

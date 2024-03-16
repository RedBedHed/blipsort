# *Branchless Lomuto in Pattern-Defeating Quicksort (Blipsort)*
 
A highly-optimized, memory-conscious, (and tiny) Introsort variant that draws from PDQsort, Java, and Orson Peters' and Lukas Bergdoll's branchless Lomuto partitioning. Iterative version [Here](https://github.com/RedBedHed/blipsort_iterative).

## Speed

![Speed](https://github.com/RedBedHed/BLPDQsort/blob/main/blipsort_speed.png)

##### *clang 16, -O3*

## Complexity

| Best | Average | Worst | Memory |
|------|---------|-------|--------|
| n    | n log n | n log n | log n |

## Visualization

https://github.com/RedBedHed/blipsort/assets/58797872/00986779-05a3-430a-bc67-11eb45a54756

###### *Blipsort, ported into the Sound of Sorting program by Timo Bingmann*

## Techniques

### Branchless Lomuto
The decades-old partitioning algorithm recently made a resurgence when researchers discovered ways to remove the inner branch. Orson Peters' and Lukas Bergdoll's [method](https://orlp.net/blog/branchless-lomuto-partitioning/)&mdash; published under two months ago&mdash; is the fastest yet. It employs a gap in the data to move elements twice per iteration rather than swapping them (three moves).

For arithmetic and pointer types, Blipsort employs branchless Lomuto partitioning. For other, larger types, Blipsort uses branchful or block Hoare partitioning. Branchful Hoare partitioning is slower than fulcrum or block partitioning. However, it uses no extra offset memory (better for medium embedded systems). Hoare partitioning is also slower than 2-3 pivot partitioning on random data, although marginally so. Block Hoare partitioning is significantly faster than its branchy counterpart. However, it does use extra offset memory (better for large embedded systems and PC).

While it works wonders for random arrays, branchless Lomuto does struggle with descending data. Blipsort attempts to remedy this by sampling five elements from the array and rotating the interval when all are strictly descending. However, this approach does not break all descending patterns. When an array contains strictly-descending elements at intervals, lomuto partitioning can slow down quite significantly in comparison to Hoare (In all fairness, Hoare is particularly well-suited for descending patterns).

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

### Custom Comparators
Blipsort allows its user to implement a custom boolean comparator. A comparator is best implemented with a lambda and no branches. A comparator implemented with a lambda can be inlined by an optimizing compiler, while a comparator implemented with a constexpr/inline function typically cannot.

## Usage

To sort with branchless Lomuto on small types and block Hoare on large types, call blipsort like so:

```c++
Arrays::blipsort(array, size);
```

To sort with branchless Lomuto on small types and branchy Hoare on large types (to conserve memory) call blipsort like so:
```c++
Arrays::blipsort_embed(array, size);
```

## Sources

[Here](https://github.com/orlp/pdqsort)
is the PDQsort algorithm by Orson Peters

[Here](https://orlp.net/blog/branchless-lomuto-partitioning/)
is the branchless Lomuto blog post by Orson Peters and Lukas Bergdoll

[Here](https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/DualPivotQuicksort.java)
is Java's Dual Pivot Quicksort

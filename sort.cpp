#include "sort.h"

namespace 
{ enum : uint32_t
{
    InsertionThreshold = 88,
    AscendingThreshold = 8,
    LargeDataThreshold = 128
};

/**
 * The DeBruijn constant.
 */
constexpr uint64_t DeBruijn64 =
    0x03F79D71B4CB0A89L;

/*
    * The DeBruijn map from key to integer
    * square index.
    */
constexpr uint8_t DeBruijnTableF[] = 
{
    0,  47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

/**
 * Fill trailing bits using prefix fill.
 *
 * @code
 * Example:
 *       10000000 >> 1
 *     = 01000000 | 10000000
 *     = 11000000 >> 2
 *     = 00110000 | 11000000
 *     = 11110000 >> 4
 *     = 00001111 | 11110000
 *     = 11111111
 * @endcode
 * @tparam E The type
 * @param x The integer
 */
constexpr void parallelPrefixFill
    (
    uint32_t & x
    ) 
{
    x |= x >> 1U;
    x |= x >> 2U;
    x |= x >> 4U;
    x |= x >> 8U;
    x |= x >> 16U;
}

/**
 * bitScanReverse
 * @authors Kim Walisch
 * @authors Mark Dickinson
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of most significant one bit
 */
constexpr int bitScanRev
    (
    uint32_t l
    ) 
{
    assert(l != 0);
    parallelPrefixFill(l);
    return DeBruijnTableF[(int)
        ((l * DeBruijn64) >> 58U)
    ];
}

/**
 * A simple swap method.
 *
 * @tparam E the element type
 * @param i the first element pointer
 * @param j the second element pointer
 */
template<typename E>
constexpr void swap
    (
    E *const i,
    E *const j
    ) 
{
    E const
    el = *i;
    *i = *j;
    *j = el;
}

/**
 *
 * A generic "sift down" method (AKA max-heapify)
 *
 * @tparam E the element type
 * @param a the pointer to the base of the current
 * sub-array
 * @param i the starting index
 * @param size the size of the current sub-array
 */
template<typename E>
inline void siftDown
    (
    E* const a,
    const int i,
    const int size
    ) 
{
    // Store size in
    // a local variable.
    const uint32_t n = size;

    // Establish non-leaf
    // boundary.
    const uint32_t o = n >> 1U;

    // Extract the element
    // to sift.
    E z = a[i];

    // initialize temporary
    // variables.
    uint32_t x = i, l, r;

    // consider only non-leaf
    // nodes.
    while(x < o) 
    {
        // y is currently
        // left child element.
        // Note: "l" here stands
        // for "left"
        r = (l = (x << 1U) + 1) + 1;
        E y = a[l];

        // if right child is
        // within the heap...
        // AND
        // if right child element
        // is greater than left
        // child element,
        // THEN
        // assign right child to
        // y and right index to l.
        // Note: "l" now stands
        // for "larger"
        if(r < n && y < a[r])
            y = a[l = r];

        // if y is less than or
        // equal to the element
        // we are sifting, then
        // we are done.
        if(y <= z) break;

        // move y up to the
        // parent index.
        a[x] = y;

        // Set parent index to
        // be the index of
        // the largest child.
        x = l;
    }

    // Place the sifted element.
    a[x] = z;
}

/**
 * <b>
 *  <i>Heap Sort</i>
 * </b>
 *
 * <p>
 * Classical heap sort that sorts the given range
 * in ascending order, building a max heap and
 * continuously sifting/swapping the max element
 * to the previous rightmost index.
 * </p>
 * @author Ellie Moore
 * @tparam E the element type
 * @param low a pointer to the leftmost index
 * @param high a pointer to the rightmost index
 */
template<typename E>
inline void hSort
    (
    E* const low,
    E* const high
    ) 
{
    E* r = high + 1;
    E* const l = low;

    // Build the heap.
    int x = r - l;
    for(int i =
        (x >> 1U); i >= 0; --i)
        siftDown(l, i, x);
    
    // Sort.
    while(l < --r) 
    {
        const E z = *l; *l = *r;
        siftDown(l, 0, --x);
        *r = z;
    }
}

/**
 * <b>
 *  <i>
 * Insertion Sort
 *  </i>
 * </b>
 *
 * <p>
 * Classical ascending insertion sort packaged with a
 * "pairing" optimization to be used in the context of
 * Quick Sort.
 * This optimization is used whenever the portion of
 * the array to be sorted is padded on the left by
 * a portion with lesser elements. The fact that all of
 * the elements on the left are automatically less than
 * the elements in the current portion allows us to skip
 * the costly lower boundary check in the nested loops
 * and insert two elements in one go.
 * </p>
 *
 * @authors Josh Bloch
 * @authors Jon Bently
 * @authors Ellie Moore
 * @tparam E the element type
 * @param low a pointer to the leftmost index
 * @param high a pointer to the rightmost index
 * @param isLeftmost whether or not this subarray is a
 * left-most partition.
 */
template<typename E, bool Bail = true>
inline bool iSort
    (
    bool leftmost,
    E *const low, 
    E *const high
    ) 
{
    E* l = low;
    E* r = high;
    int moves = 0;
    if (leftmost) 
    {
        // Traditional
        // insertion
        // sort.
        for (E *i = l + 1; i <= r; ++i) 
        {
            E t = *i, *j = i - 1;
            for (; j >= l && t < *j; --j)
                j[1] = *j;
            j[1] = t;

            if constexpr (Bail) 
            {
                // If we have moved too
                // many elements, abort.
                moves += (i - 1) - j;
                if(moves > AscendingThreshold)
                    return false;
            }
        }
    } 
    else 
    {
        // std::cout << "hi";
        // Pair insertion sort.
        // Skip elements that are
        // in ascending order.
        do if (l++ >= r) return true;
        while (*l >= *(l - 1));

        // This sort uses the sub
        // array at left to avoid
        // the lower bound check.
        // Assumes that this is not
        // the leftmost partition.
        for (E *i = l; ++l <= r; i = ++l) 
        {
            E ex = *i, ey = *l;

            // Make sure that
            // we insert the
            // larger element
            // first.
            if (ey < ex) 
            {
                ex = ey;
                ey = *i;
                ++moves;
            }

            // Insert the two
            // in one downward
            // motion.
            while (ey < *--i)
                i[2] = *i;
            (++i)[1] = ey;
            while (ex < *--i)
                i[1] = *i;
            i[1] = ex;

            if constexpr (Bail) 
            {
                // If we have moved too
                // many elements, abort.
                moves += (l - 2) - i;
                if(moves > AscendingThreshold) 
                    return false;
            }
        }

        // For odd length arrays,
        // insert the last element.
        E ez = *r;
        while (ez < *--r)
            r[1] = *r;
        r[1] = ez;
    }
    return true;
}


/**
 * Scramble a few elements to help
 * break patterns.
 *
 * @tparam E the element type
 * @param i the first element pointer
 * @param j the second element pointer
 */
template<typename E>
inline void scramble
    (
    E* const low, 
    E* const high, 
    const uint32_t len
    ) 
{
    if(len >= InsertionThreshold) 
    {
        const int _4th = len >> 2U;
        swap(low,  low  + _4th);
        swap(high, high - _4th);
        if(len > LargeDataThreshold)
        {
            swap(low  + 1, low  + (_4th + 1));
            swap(low  + 2, low  + (_4th + 2));
            swap(high - 2, high - (_4th + 2));
            swap(high - 1, high - (_4th + 1));
        }
    }
}

/**
 * <b>
 *  <i>Introspective Multi-Pivot Quick Sort</i>
 * </b>
 *
 * <p>
 * See readme (too lazy to type this right now)
 * </p>
 *
 * @authors Vladimir Yaroslavskiy
 * @authors Josh Bloch
 * @authors Jon Bently
 * @authors Shrinu Kushagra
 * @authors Alejandro Lopez-Ortiz
 * @authors J. Ian Munro
 * @authors Aurick Qiao
 * @authors Orson Peters
 * @authors Ellie Moore
 * @tparam E the element type
 * @tparam Leftmost whether we are considering
 * a leftmost partition.
 * @param low a pointer to the leftmost index
 * @param high a pointer to the rightmost index
 * @param height the distance of the current sort
 * tree from the initial height of 2log<sub>2</sub>n
 */
template
<typename E, bool Root = true>
void qSort
    (
    bool leftmost,
    E * low,
    E * high,
    int height
    ) 
{
    E* const olow = low;
    // Tail call loop.
    for(uint32_t x = high - low;;) 
    {
        // If this is not the
        // root node, sort the 
        // interval by insertion
        // sort if small enough.
        if constexpr (!Root)
        if (x < InsertionThreshold) 
        {
            // If we are in the Root,
            // we won't be insertion 
            // sorting until we 
            // iterate on the rightmost
            // part. However, we are
            // not in the root here, so
            // we need to be careful
            // to use guarded insertion
            // sort if this is the
            // leftmost partition.
            iSort<E, false>
            (leftmost, low, high);
            return;
        }

        // Heap sort when the runtime
        // trends towards quadratic.
        if(height < 0)
            return hSort(low, high);

        // Find an inexpensive
        // approximation of a third of
        // the interval.
        const uint32_t y = x >> 2U,
            _3rd = y + (y >> 1U),
            _6th = _3rd >> 1U;

        // Find an approximate
        // midpoint of the interval.
        E *const mid = low + (x >> 1U);

        // Assign tercile indices
        // to candidate pivots.
        E *const sl = low  + _3rd;
        E *const sr = high - _3rd;

        // Assign outer indices
        // to candidate pivots.
        E * cl = low  + _6th;
        E * cr = high - _6th;
        if(*low  < *cl) cl = low;
        if(*high > *cr) cr = high;

        // Insertion sort all five
        // candidate pivots in-place.
        if (*sl < *cl) 
        {
            E e = *sl;
            *sl = *cl;
            *cl =   e;
        }

        if (*mid < *sl) 
        {
            E e  = *mid;
            *mid =  *sl;
            *sl  =    e;
            if (e < *cl) 
            {
                *sl = *cl;
                *cl =   e;
            }
        }

        if (*sr < *mid) 
        {
            E e  =  *sr;
            *sr  = *mid;
            *mid =    e;
            if (e < *sl) 
            {
                *mid = *sl;
                *sl  =   e;
                if (e < *cl) 
                {
                    *sl = *cl;
                    *cl =   e;
                }
            }
        }

        if (*cr < *sr) 
        {
            E e = *cr;
            *cr = *sr;
            *sr =   e;
            if (e < *mid) 
            {
                *sr  = *mid;
                *mid =    e;
                if (e < *sl) 
                {
                    *mid = *sl;
                    *sl  =   e;
                    if (e < *cl) 
                    {
                        *sl = *cl;
                        *cl =   e;
                    }
                }
            }
        }
        
        // If any middle candidate 
        // pivot is equal to the 
        // rightmost element of the 
        // partition to the left, 
        // we have "retained" the pivot.
        // swap pivot duplicates to 
        // the side and sort the 
        // remainder. This is an
        // alternative to dutch-flag
        // partitioning.
        if(!leftmost)
        {
            // Check if we retained
            // the pivot to the left.
            E h = *(low - 1);
            if(h == *sl  || 
               h == *mid || 
               h == *sr) 
            {
                E* l = low,
                 * k = low;
                for(; k <= high; ++k)
                    if(*k == h)
                        swap(l++, k);
                low = l;
                if(low >= high)
                    return;
                x = high - low;
                continue;
            }
        }

        // Assign midpoint to pivot
        // variable.
        const E p = *mid;

        // Initialize l and g.
        // "less" and "great"
        // respectively.
        E *l = low - 1, 
          *k = high + 1;

        // skip over data
        // in place.
        while(*++l < p);
        if(leftmost)
            while(*--k >= p && l < k);
        else 
        while(*--k >= p);

        // Bring left end inside.
        // Left end will be
        // replaced and pivot will
        // be swapped back later.
        *mid = *l;

        E* g = l;

        while(g < k)
        {
            *g++ = *l;
            *l = *g;
            l += (*l < p);
        }
        *g = *l; *l = p; g = l;

        // Skip the middle part.
        l -= (l > low );
        g += (g < high);

        // Cheaply calculate an
        // eigth of the interval.
        const uint32_t 
            _8th = x >> 3U;

        // Calculate interval widths.
        const uint32_t
            ls = l - low,
            gs = high - g;

        // If the partition is fairly 
        // balanced, try insertion sort.
        // If insertion sort runtime
        // trends higher than O(n), fall 
        // back to quicksort. Idea from 
        // Orson Peters for linear
        // time on nearly sorted arrays.
        if(ls >= _8th &&
           gs >= _8th) 
        {
            if(!iSort(leftmost, low, l)) 
                goto l1;
            if(!iSort(false, g, high))
                goto l2;
            return;
        }

        // The partition is not balanced.
        // scramble some elements and
        // try to break the pattern.
        scramble(low, l, ls);
        scramble(g, high, gs);

        // This was a bad partition,
        // so decrement the height.
        // When the height is zero,
        // we will use heapsort.
        --height;

        // Sort left portion.
        l1: qSort<E, false>
        (leftmost, low, l, height);

        // Sort right portion 
        // iteratively.
        l2: low = g;

        // Find the wwidth of the
        // interval
        x = high - low;

        // If this is the root,
        // sort the interval
        // by insertion sort
        // if small enough.
        if constexpr (Root)
        if (x < InsertionThreshold) 
        {
            // If we are in the Root,
            // insertion sort will
            // be unguarded.
            iSort
            <E, false>(leftmost, low, high);
            return;
        }

        leftmost = false;
    }
}}

namespace Arrays 
{
    template <typename E>
    inline void uSort
        (
        E* const a,
        const uint32_t cnt
        ) 
    {
        if(cnt < InsertionThreshold)
        {
            iSort<E, false>(true, a, a + (cnt - 1));
            return;
        }
        // floor of log base 2 of cnt.
        const int log2Cnt = bitScanRev(cnt);
        return qSort(true, a, a + (cnt - 1), log2Cnt);
    }

    template void
    uSort<int64_t>(int64_t*, uint32_t);
    template void
    uSort<int32_t>(int32_t*, uint32_t);
    template void
    uSort<int16_t>(int16_t*, uint32_t);
    template void
    uSort<int8_t>(int8_t*, uint32_t);
}

#include<iostream>
#include<ctime>
#include<stdio.h>
#include<chrono>
#include<algorithm>
#include <bitset>
#include <climits>
#include <cstring>
#include <iostream>

// int main() {
//     int a[1000000];
//     srand(time(NULL));
//     for(int i = 0; i < 1000000; ++i)
//         a[i] = rand() % 1000000;
//     Arrays::uSort<int32_t>(a, 1000000);
//     for(int i = 1; i < 1000000; ++i)
//     {
//         if( a[i - 1] > a[i] )
//         {
//             std::cout << "\nNOT SORTED\n";
//             return 0;
//         }
//     }

//     std::cout << "\nSORTED\n";
//     return 0;       
// }
int b[10000000];
int main()
{
    srand(time(NULL));
    for(int o = 0; o < 100; ++o)
    {
    const int num = 100;
    typedef std::chrono::high_resolution_clock Clock;
    uint64_t t = 0;
    int a[num];
    for(int c = 0; c < 500; ++c)
    {
        std::memset(b, 0, sizeof(b));
        for(int i = 0; i < num; ++i) {
            b[a[i] = rand()% num]++;
        }

        // for(int i = 0; i < num; ++i) {
        //     if(i % 1000)
        //         a[i] = rand() % num;
        //     else a[i] = i;
        // }
        auto t1 = Clock::now();
        Arrays::uSort<int32_t>(a, num);
        // std::sort(a, a + num);
        auto t2 = Clock::now();
        t += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
        int i;
        for(i = 1; i < num; ++i)
        {
            // std::cout << a[i - 1] << ' ';
            if(a[i - 1] > a[i]) 
            {
                std::cout << "NOT SORTED!!!";
                goto out;
            }
        }
        // std::cout << a[i - 1] << '\n';

        for(int i = 0; i < num; ++i)
        {
            b[a[i]]--;

        }

        for(int j = 0; j < num; ++j)
        {
            if(b[a[j]] != 0)
            {
                std::cout << "MISSING ELEMENTS!!!\n";
                goto out;
            }
        }
    }
    std::cout << t << '\n';
    out: continue;
    }
    return 0;
}

// int main()
// {
//     int a[10000];
//     for(int i = 0; i < 10000; ++i)
//         a[i] = i;

//     if(iSort<int, false>(true, a + 0, a + 9999))
//         std::cout << "true\n";
//     else std::cout << "false\n";
//     for(int i = 0; i < 10; ++i)
//         std::cout << a[i] << ' ';
// }

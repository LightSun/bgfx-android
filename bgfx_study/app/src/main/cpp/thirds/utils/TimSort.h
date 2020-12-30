//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_TIMSORT_H
#define BGFX_STUDY_TIMSORT_H

#include <log.h>
#include "Array.h"
#include "../lua/SkRefCnt.h"
#include "Comparator.h"
#include "global.h"

namespace h7 {
/**
 * A stable, adaptive, iterative mergesort that requires far fewer than
 * n lg(n) comparisons when running on partially sorted arrays, while
 * offering performance comparable to a traditional mergesort when run
 * on random arrays.  Like all proper mergesorts, this sort is stable and
 * runs O(n log n) time (worst case).  In the worst case, this sort requires
 * temporary storage space for n/2 object references; in the best case,
 * it requires only a small constant amount of space.
 *
 * This implementation was adapted from Tim Peters's list sort for
 * Python, which is described in detail here:
 *
 *   http://svn.python.org/projects/python/trunk/Objects/listsort.txt
 *
 * Tim's C code may be found here:
 *
 *   http://svn.python.org/projects/python/trunk/Objects/listobject.c
 *
 * The underlying techniques are described in this paper (and may have
 * even earlier origins):
 *
 *  "Optimistic Sorting and Information Theoretic Complexity"
 *  Peter McIlroy
 *  SODA (Fourth Annual ACM-SIAM Symposium on Discrete Algorithms),
 *  pp 467-474, Austin, Texas, 25-27 January 1993.
 *
 * While the API to this class consists solely of static methods, it is
 * (privately) instantiable; a TimSort instance holds the state of an ongoing
 * sort, assuming the input array is large enough to warrant the full-blown
 * TimSort. Small arrays are sorted in place, using a binary insertion sort.
 *
 * @author Josh Bloch
 * @to_cpp heaven7
 */
    template<typename T>
    class TimSort {

    public:
        /**
            * Sorts the given range, using the given workspace array slice
            * for temp storage when possible. This method is designed to be
            * invoked from public methods (in class Arrays) after performing
            * any necessary array bounds checks and expanding parameters into
            * the required forms.
            *
            * @param a the array to be sorted
            * @param lo the index of the first element, inclusive, to be sorted
            * @param hi the index of the last element, exclusive, to be sorted
            * @param c the comparator to use
            * @param work a workspace array (slice)
            * @param workBase origin of usable space in work array
            * @param workLen usable size of work array
            * @since 1.8
            */
        static void sort(Array<T> &a, int lo, int hi, Comparator<T> *c,
                         Array<T> *work, int workBase, int workLen) {
            SkASSERT(c != nullptr && lo >= 0 && lo <= hi && hi <= a.size());

            int nRemaining = hi - lo;
            if (nRemaining < 2)
                return;  // Arrays of size 0 and 1 are always sorted

            // If array is small, do a "mini-TimSort" with no merges
            if (nRemaining < MIN_MERGE) {
                int initRunLen = countRunAndMakeAscending(a, lo, hi, c);
                binarySort(a, lo, hi, lo + initRunLen, c);
                return;
            }

            /**
             * March over the array once, left to right, finding natural runs,
             * extending short natural runs to minRun elements, and merging runs
             * to maintain stack invariant.
             */
            TimSort<T> ts = TimSort<T>(a, c, work, workBase, workLen);
            int minRun = minRunLength(nRemaining);
            do {
                // Identify next run
                int runLen = countRunAndMakeAscending(a, lo, hi, c);

                // If run is short, extend to min(minRun, nRemaining)
                if (runLen < minRun) {
                    int force = nRemaining <= minRun ? nRemaining : minRun;
                    binarySort(a, lo, lo + force, lo + runLen, c);
                    runLen = force;
                }

                // Push run onto pending-run stack, and maybe merge
                ts.pushRun(lo, runLen);
                ts.mergeCollapse();

                // Advance to find next run
                lo += runLen;
                nRemaining -= runLen;
            } while (nRemaining != 0);

            // Merge all remaining runs to complete sort
            SkASSERT(lo == hi);
            ts.mergeForceCollapse();
            SkASSERT(ts.stackSize == 1);
        }

    private:
        /**
        * The array being sorted.
        */
        Array<T> a;

        /**
    * This is the minimum sized sequence that will be merged.  Shorter
    * sequences will be lengthened by calling binarySort.  If the entire
    * array is less than this length, no merges will be performed.
    *
    * This constant should be a power of two.  It was 64 in Tim Peter's C
    * implementation, but 32 was empirically determined to work better in
    * this implementation.  In the unlikely event that you set this constant
    * to be a number that's not a power of two, you'll need to change the
    * {@link #minRunLength} computation.
    *
    * If you decrease this constant, you must change the stackLen
    * computation in the TimSort constructor, or you risk an
    * ArrayOutOfBounds exception.  See listsort.txt for a discussion
    * of the minimum stack length required as a function of the length
    * of the array being sorted and the minimum merge sequence length.
    */
        static constexpr int MIN_MERGE = 32;

        /**
         * The comparator for this sort.
         */
        Comparator<T> *c;

        /**
         * When we get into galloping mode, we stay there until both runs win less
         * often than MIN_GALLOP consecutive times.
         */
        static constexpr int MIN_GALLOP = 7;

        /**
         * This controls when we get *into* galloping mode.  It is initialized
         * to MIN_GALLOP.  The mergeLo and mergeHi methods nudge it higher for
         * random data, and lower for highly structured data.
         */
        int minGallop = MIN_GALLOP;

        /**
         * Maximum initial size of tmp array, which is used for merging.  The array
         * can grow to accommodate demand.
         *
         * Unlike Tim's original C version, we do not allocate this much storage
         * when sorting smaller arrays.  This change was required for performance.
         */
        static constexpr int INITIAL_TMP_STORAGE_LENGTH = 256;

        /**
         * Temp storage for merges. A workspace array may optionally be
         * provided in constructor, and if so will be used as long as it
         * is big enough.
         */
        Array<T> tmp;
        int tmpBase; // base of tmp array slice
        int tmpLen;  // length of tmp array slice

        /**
         * A stack of pending runs yet to be merged.  Run i starts at
         * address base[i] and extends for len[i] elements.  It's always
         * true (so long as the indices are in bounds) that:
         *
         *     runBase[i] + runLen[i] == runBase[i + 1]
         *
         * so we could cut the storage for this, but it's a minor amount,
         * and keeping all the info explicit simplifies the code.
         */
        int stackSize = 0;  // Number of pending runs on stack
        IntArray runBase;
        IntArray runLen;

        /**
   * Creates a TimSort instance to maintain the state of an ongoing sort.
   *
   * @param a the array to be sorted
   * @param c the comparator to determine the order of the sort
   * @param work a workspace array (slice)
   * @param workBase origin of usable space in work array
   * @param workLen usable size of work array
   */
     TimSort(Array<T> &a, Comparator<T>* c, Array<T>* work, int workBase, int workLen) {
            this->a = a;
            this->c = c;

            // Allocate temp storage (which may be increased later if necessary)
            int len = a.size();
            int tlen = (len < 2 * INITIAL_TMP_STORAGE_LENGTH) ?
                       unsign_shift_right(len, 1) : INITIAL_TMP_STORAGE_LENGTH;
            if (work == nullptr || workLen < tlen || workBase + tlen > work->size()) {
                tmp.clear();
                tmp.setSize(tlen);
               // tmp = newArray;
                tmpBase = 0;
                tmpLen = tlen;
            }
            else {
                tmp = *work;
                tmpBase = workBase;
                tmpLen = workLen;
            }

            /*
             * Allocate runs-to-be-merged stack (which cannot be expanded).  The
             * stack length requirements are described in listsort.txt.  The C
             * version always uses the same stack length (85), but this was
             * measured to be too expensive when sorting "mid-sized" arrays (e.g.,
             * 100 elements) in Java.  Therefore, we use smaller (but sufficiently
             * large) stack lengths for smaller arrays.  The "magic numbers" in the
             * computation below must be changed if MIN_MERGE is decreased.  See
             * the MIN_MERGE declaration above for more information.
             * The maximum value of 49 allows for an array up to length
             * Integer.MAX_VALUE-4, if array is filled by the worst case stack size
             * increasing scenario. More explanations are given in section 4 of:
             * http://envisage-project.eu/wp-content/uploads/2015/02/sorting.pdf
             */
            int stackLen = (len <    120  ?  5 :
                            len <   1542  ? 10 :
                            len < 119151  ? 24 : 49);
            runBase.setSize(stackLen);
            runLen.setSize(stackLen);
        }
        /**
* Returns the length of the run beginning at the specified position in
* the specified array and reverses the run if it is descending (ensuring
* that the run will always be ascending when the method returns).
*
* A run is the longest ascending sequence with:
*
*    a[lo] <= a[lo + 1] <= a[lo + 2] <= ...
*
* or the longest descending sequence with:
*
*    a[lo] >  a[lo + 1] >  a[lo + 2] >  ...
*
* For its intended use in a stable mergesort, the strictness of the
* definition of "descending" is needed so that the call can safely
* reverse a descending sequence without violating stability.
*
* @param a the array in which a run is to be counted and possibly reversed
* @param lo index of the first element in the run
* @param hi index after the last element that may be contained in the run.
         It is required that {@code lo < hi}.
* @param c the comparator to used for the sort
* @return  the length of the run beginning at the specified position in
*          the specified array
*/
        static inline int countRunAndMakeAscending(Array<T> &a, int lo, int hi,
                                                   Comparator<T> *c) {
            SkASSERT(lo < hi);
            int runHi = lo + 1;
            if (runHi == hi)
                return 1;

            // Find end of run, and reverse range if descending
            if (c->compare(&a[runHi++], &a[lo]) < 0) { // Descending
                while (runHi < hi && c->compare(&a[runHi], &a[runHi - 1]) < 0)
                    runHi++;
                reverseRange(a, lo, runHi);
            } else {                              // Ascending
                while (runHi < hi && c->compare(&a[runHi], &a[runHi - 1]) >= 0)
                    runHi++;
            }

            return runHi - lo;
        }

        /**
    * Reverse the specified range of the specified array.
    *
    * @param a the array in which a range is to be reversed
    * @param lo the index of the first element in the range to be reversed
    * @param hi the index after the last element in the range to be reversed
    */
        static inline void reverseRange(Array<T> &a, int lo, int hi) {
            hi--;
            while (lo < hi) {
                T t = a[lo];
                a[lo++] = a[hi];
                a[hi--] = t;
            }
        }

        /**
     * Sorts the specified portion of the specified array using a binary
     * insertion sort.  This is the best method for sorting small numbers
     * of elements.  It requires O(n log n) compares, but O(n^2) data
     * movement (worst case).
     *
     * If the initial part of the specified range is already sorted,
     * this method can take advantage of it: the method assumes that the
     * elements from index {@code lo}, inclusive, to {@code start},
     * exclusive are already sorted.
     *
     * @param a the array in which a range is to be sorted
     * @param lo the index of the first element in the range to be sorted
     * @param hi the index after the last element in the range to be sorted
     * @param start the index of the first element in the range that is
     *        not already known to be sorted ({@code lo <= start <= hi})
     * @param c comparator to used for the sort
     */
        static void binarySort(Array<T> &a, int lo, int hi, int start,
                               Comparator<T> *c) {
            SkASSERT(lo <= start && start <= hi);
            if (start == lo)
                start++;
            for (; start < hi; start++) {
                T pivot = a[start];

                // Set left (and right) to the index where a[start] (pivot) belongs
                int left = lo;
                int right = start;
                SkASSERT(left <= right);
                /*
                 * Invariants:
                 *   pivot >= all in [lo, left).
                 *   pivot <  all in [right, start).
                 */
                while (left < right) {
                    int mid = ((unsigned int) (left + right)) >> 1;
                    if (c->compare(&pivot, &a[mid]) < 0)
                        right = mid;
                    else
                        left = mid + 1;
                }
                SkASSERT(left == right);

                /*
                 * The invariants still hold: pivot >= all in [lo, left) and
                 * pivot < all in [left, start), so pivot belongs at left.  Note
                 * that if there are elements equal to pivot, left points to the
                 * first slot after them -- that's why this sort is stable.
                 * Slide elements over to make room for pivot.
                 */
                int n = start - left;  // The number of elements to move
                // Switch is just an optimization for arraycopy in default case
                switch (n) {
                    case 2:
                        a[left + 2] = a[left + 1];
                    case 1:
                        a[left + 1] = a[left];
                        break;
                    default:
                        //all move back from left -> left + 1
                        for (int i = n - 1; i >= 0; i--) {
                            a.swap(left + i, left + 1 + i);
                        }
                        //System.arraycopy(a, left, a, left + 1, n);
                }
                a[left] = pivot;
            }
        }

        /**
    * Returns the minimum acceptable run length for an array of the specified
    * length. Natural runs shorter than this will be extended with
    * {@link #binarySort}.
    *
    * Roughly speaking, the computation is:
    *
    *  If n < MIN_MERGE, return n (it's too small to bother with fancy stuff).
    *  Else if n is an exact power of 2, return MIN_MERGE/2.
    *  Else return an int k, MIN_MERGE/2 <= k <= MIN_MERGE, such that n/k
    *   is close to, but strictly less than, an exact power of 2.
    *
    * For the rationale, see listsort.txt.
    *
    * @param n the length of the array to be sorted
    * @return the length of the minimum run to be merged
    */
        static int minRunLength(int n) {
            SkASSERT(n >= 0);
            int r = 0;      // Becomes 1 if any 1 bits are shifted off
            while (n >= MIN_MERGE) {
                r |= (n & 1);
                n >>= 1;
            }
            return n + r;
        }

        /**
    * Pushes the specified run onto the pending-run stack.
    *
    * @param runBase index of the first element in the run
    * @param runLen  the number of elements in the run
    */
        void pushRun(int runBase, int runLen) {
            this->runBase[stackSize] = runBase;
            this->runLen[stackSize] = runLen;
            stackSize++;
        }

        /**
    * Examines the stack of runs waiting to be merged and merges adjacent runs
    * until the stack invariants are reestablished:
    *
    *     1. runLen[i - 3] > runLen[i - 2] + runLen[i - 1]
    *     2. runLen[i - 2] > runLen[i - 1]
    *
    * This method is called each time a new run is pushed onto the stack,
    * so the invariants are guaranteed to hold for i < stackSize upon
    * entry to the method.
    */
        void mergeCollapse() {
            while (stackSize > 1) {
                int n = stackSize - 2;
                if (n > 0 && runLen[n - 1] <= runLen[n] + runLen[n + 1]) {
                    if (runLen[n - 1] < runLen[n + 1])
                        n--;
                    mergeAt(n);
                } else if (runLen[n] <= runLen[n + 1]) {
                    mergeAt(n);
                } else {
                    break; // Invariant is established
                }
            }
        }

        /**
    * Merges all runs on the stack until only one remains.  This method is
    * called once, to complete the sort.
    */
        void mergeForceCollapse() {
            while (stackSize > 1) {
                int n = stackSize - 2;
                if (n > 0 && runLen[n - 1] < runLen[n + 1])
                    n--;
                mergeAt(n);
            }
        }

        /**
    * Merges the two runs at stack indices i and i+1.  Run i must be
    * the penultimate or antepenultimate run on the stack.  In other words,
    * i must be equal to stackSize-2 or stackSize-3.
    *
    * @param i stack index of the first of the two runs to merge
    */
        void mergeAt(int i) {
            SkASSERT(stackSize >= 2);
            SkASSERT(i >= 0);
            SkASSERT(i == stackSize - 2 || i == stackSize - 3);

            int base1 = runBase[i];
            int len1 = runLen[i];
            int base2 = runBase[i + 1];
            int len2 = runLen[i + 1];
            SkASSERT(len1 > 0 && len2 > 0);
            SkASSERT(base1 + len1 == base2);

            /*
             * Record the length of the combined runs; if i is the 3rd-last
             * run now, also slide over the last run (which isn't involved
             * in this merge).  The current run (i+1) goes away in any case.
             */
            runLen[i] = len1 + len2;
            if (i == stackSize - 3) {
                runBase[i + 1] = runBase[i + 2];
                runLen[i + 1] = runLen[i + 2];
            }
            stackSize--;

            /*
             * Find where the first element of run2 goes in run1. Prior elements
             * in run1 can be ignored (because they're already in place).
             */
            int k = gallopRight(&a[base2], a, base1, len1, 0, c);
            SkASSERT(k >= 0);
            base1 += k;
            len1 -= k;
            if (len1 == 0)
                return;

            /*
             * Find where the last element of run1 goes in run2. Subsequent elements
             * in run2 can be ignored (because they're already in place).
             */
            len2 = gallopLeft(&a[base1 + len1 - 1], a, base2, len2, len2 - 1, c);
            SkASSERT(len2 >= 0);
            if (len2 == 0)
                return;

            // Merge remaining runs, using tmp array with min(len1, len2) elements
            if (len1 <= len2)
                mergeLo(base1, len1, base2, len2);
            else
                mergeHi(base1, len1, base2, len2);
        }

        /**
    * Locates the position at which to insert the specified key into the
    * specified sorted range; if the range contains an element equal to key,
    * returns the index of the leftmost equal element.
    *
    * @param key the key whose insertion point to search for
    * @param a the array in which to search
    * @param base the index of the first element in the range
    * @param len the length of the range; must be > 0
    * @param hint the index at which to begin the search, 0 <= hint < n.
    *     The closer hint is to the result, the faster this method will run.
    * @param c the comparator used to order the range, and to search
    * @return the int k,  0 <= k <= n such that a[b + k - 1] < key <= a[b + k],
    *    pretending that a[b - 1] is minus infinity and a[b + n] is infinity.
    *    In other words, key belongs at index b + k; or in other words,
    *    the first k elements of a should precede key, and the last n - k
    *    should follow it.
    */
        static int gallopLeft(T *key, Array<T> &a, int base, int len, int hint,
                              Comparator<T> *c) {
            SkASSERT(len > 0 && hint >= 0 && hint < len);
            int lastOfs = 0;
            int ofs = 1;
            if (c->compare(key, &a[base + hint]) > 0) {
                // Gallop right until a[base+hint+lastOfs] < key <= a[base+hint+ofs]
                int maxOfs = len - hint;
                while (ofs < maxOfs && c->compare(key, &a[base + hint + ofs]) > 0) {
                    lastOfs = ofs;
                    ofs = (ofs << 1) + 1;
                    if (ofs <= 0)   // int overflow
                        ofs = maxOfs;
                }
                if (ofs > maxOfs)
                    ofs = maxOfs;

                // Make offsets relative to base
                lastOfs += hint;
                ofs += hint;
            } else { // key <= a[base + hint]
                // Gallop left until a[base+hint-ofs] < key <= a[base+hint-lastOfs]
                const int maxOfs = hint + 1;
                while (ofs < maxOfs && c->compare(key, &a[base + hint - ofs]) <= 0) {
                    lastOfs = ofs;
                    ofs = (ofs << 1) + 1;
                    if (ofs <= 0)   // int overflow
                        ofs = maxOfs;
                }
                if (ofs > maxOfs)
                    ofs = maxOfs;

                // Make offsets relative to base
                int tmp = lastOfs;
                lastOfs = hint - ofs;
                ofs = hint - tmp;
            }
            SkASSERT(-1 <= lastOfs && lastOfs < ofs && ofs <= len);

            /*
             * Now a[base+lastOfs] < key <= a[base+ofs], so key belongs somewhere
             * to the right of lastOfs but no farther right than ofs.  Do a binary
             * search, with invariant a[base + lastOfs - 1] < key <= a[base + ofs].
             */
            lastOfs++;
            while (lastOfs < ofs) {
                int m = lastOfs + ((unsigned int) (ofs - lastOfs) >> 1);

                if (c->compare(key, &a[base + m]) > 0)
                    lastOfs = m + 1;  // a[base + m] < key
                else
                    ofs = m;          // key <= a[base + m]
            }
            SkASSERT(lastOfs == ofs);    // so a[base + ofs - 1] < key <= a[base + ofs]
            return ofs;
        }

        /**
    * Like gallopLeft, except that if the range contains an element equal to
    * key, gallopRight returns the index after the rightmost equal element.
    *
    * @param key the key whose insertion point to search for
    * @param a the array in which to search
    * @param base the index of the first element in the range
    * @param len the length of the range; must be > 0
    * @param hint the index at which to begin the search, 0 <= hint < n.
    *     The closer hint is to the result, the faster this method will run.
    * @param c the comparator used to order the range, and to search
    * @return the int k,  0 <= k <= n such that a[b + k - 1] <= key < a[b + k]
    */
        static int gallopRight(T *key, Array<T> &a, int base, int len,
                               int hint, Comparator<T> *c) {
            SkASSERT(len > 0 && hint >= 0 && hint < len);

            int ofs = 1;
            int lastOfs = 0;
            if (c->compare(key, &a[base + hint]) < 0) {
                // Gallop left until a[b+hint - ofs] <= key < a[b+hint - lastOfs]
                int maxOfs = hint + 1;
                while (ofs < maxOfs && c->compare(key, &a[base + hint - ofs]) < 0) {
                    lastOfs = ofs;
                    ofs = (ofs << 1) + 1;
                    if (ofs <= 0)   // int overflow
                        ofs = maxOfs;
                }
                if (ofs > maxOfs)
                    ofs = maxOfs;

                // Make offsets relative to b
                int tmp = lastOfs;
                lastOfs = hint - ofs;
                ofs = hint - tmp;
            } else { // a[b + hint] <= key
                // Gallop right until a[b+hint + lastOfs] <= key < a[b+hint + ofs]
                int maxOfs = len - hint;
                while (ofs < maxOfs && c->compare(key, &a[base + hint + ofs]) >= 0) {
                    lastOfs = ofs;
                    ofs = (ofs << 1) + 1;
                    if (ofs <= 0)   // int overflow
                        ofs = maxOfs;
                }
                if (ofs > maxOfs)
                    ofs = maxOfs;

                // Make offsets relative to b
                lastOfs += hint;
                ofs += hint;
            }
            SkASSERT(- 1 <= lastOfs && lastOfs < ofs && ofs <= len);

            /*
             * Now a[b + lastOfs] <= key < a[b + ofs], so key belongs somewhere to
             * the right of lastOfs but no farther right than ofs.  Do a binary
             * search, with invariant a[b + lastOfs - 1] <= key < a[b + ofs].
             */
            lastOfs++;
            while (lastOfs < ofs) {
                int m = lastOfs + unsign_shift_right((ofs - lastOfs), 1);

                if (c->compare(key, &a[base + m]) < 0)
                    ofs = m;          // key < a[b + m]
                else
                    lastOfs = m + 1;  // a[b + m] <= key
            }
            SkASSERT(lastOfs == ofs);    // so a[b + ofs - 1] <= key < a[b + ofs]
            return ofs;
        }

        /**
    * Merges two adjacent runs in place, in a stable fashion.  The first
    * element of the first run must be greater than the first element of the
    * second run (a[base1] > a[base2]), and the last element of the first run
    * (a[base1 + len1-1]) must be greater than all elements of the second run.
    *
    * For performance, this method should be called only when len1 <= len2;
    * its twin, mergeHi should be called if len1 >= len2.  (Either method
    * may be called if len1 == len2.)
    *
    * @param base1 index of first element in first run to be merged
    * @param len1  length of first run to be merged (must be > 0)
    * @param base2 index of first element in second run to be merged
    *        (must be aBase + aLen)
    * @param len2  length of second run to be merged (must be > 0)
    * @return true if success
    */
        bool mergeLo(int base1, int len1, int base2, int len2) {
            SkASSERT(len1 > 0 && len2 > 0 && base1 + len1 == base2);

            // Copy first run into temp array
            Array<T>& a = this->a;
            Array<T> tmp = ensureCapacity(len1);
            int cursor1 = tmpBase; // Indexes into tmp array
            int cursor2 = base2;   // Indexes int a
            int dest = base1;      // Indexes int a
            tmp.setRegion(cursor1, a, base1, len1);
            //System.arraycopy(a, base1, tmp, cursor1, len1);

            // Move first element of second run and deal with degenerate cases
            a[dest++] = a[cursor2++];
            if (--len2 == 0) {
                a.setRegion(dest, tmp, cursor1, len1);
                //System.arraycopy(tmp, cursor1, a, dest, len1);
                return true;
            }
            if (len1 == 1) {
                a.setRegion(dest, a, cursor2, len2);
                //System.arraycopy(a, cursor2, a, dest, len2);
                a[dest + len2] = tmp[cursor1]; // Last elt of run 1 to end of merge
                return true;
            }

            Comparator<T> *c = this->c;  // Use local variable for performance
            int minGallop = this->minGallop;    //  "    "       "     "      "
            outer:
            while (true) {
                int count1 = 0; // Number of times in a row that first run won
                int count2 = 0; // Number of times in a row that second run won

                /*
                 * Do the straightforward thing until (if ever) one run starts
                 * winning consistently.
                 */
                do {
                    SkASSERT(len1 > 1 && len2 > 0);
                    if (c->compare(&a[cursor2], &tmp[cursor1]) < 0) {
                        a[dest++] = a[cursor2++];
                        count2++;
                        count1 = 0;
                        if (--len2 == 0)
                            goto next;
                    } else {
                        a[dest++] = tmp[cursor1++];
                        count1++;
                        count2 = 0;
                        if (--len1 == 1)
                            goto next;
                    }
                } while ((count1 | count2) < minGallop);

                /*
                 * One run is winning so consistently that galloping may be a
                 * huge win. So try that, and continue galloping until (if ever)
                 * neither run appears to be winning consistently anymore.
                 */
                do {
                    SkASSERT(len1 > 1 && len2 > 0);
                    count1 = gallopRight(&a[cursor2], tmp, cursor1, len1, 0, c);
                    if (count1 != 0) {
                        a.setRegion(dest, tmp, cursor1, count1);
                        // System.arraycopy(tmp, cursor1, a, dest, count1);
                        dest += count1;
                        cursor1 += count1;
                        len1 -= count1;
                        if (len1 <= 1) // len1 == 1 || len1 == 0
                            goto next;
                    }
                    a[dest++] = a[cursor2++];
                    if (--len2 == 0)
                        goto next;

                    count2 = gallopLeft(&tmp[cursor1], a, cursor2, len2, 0, c);
                    if (count2 != 0) {
                        a.setRegion(dest, a, cursor2, count2);
                        // System.arraycopy(a, cursor2, a, dest, count2);
                        dest += count2;
                        cursor2 += count2;
                        len2 -= count2;
                        if (len2 == 0)
                            goto next;
                    }
                    a[dest++] = tmp[cursor1++];
                    if (--len1 == 1)
                        goto next;
                    minGallop--;
                } while (count1 >= MIN_GALLOP | count2 >= MIN_GALLOP);
                if (minGallop < 0)
                    minGallop = 0;
                minGallop += 2;  // Penalize for leaving gallop mode
            }  // End of "outer" loop
            next:
            this->minGallop = minGallop < 1 ? 1 : minGallop;  // Write back to field

            if (len1 == 1) {
                SkASSERT(len2 > 0);
                a.setRegion(dest, a, cursor2, len2);
                //System.arraycopy(a, cursor2, a, dest, len2);
                a[dest + len2] = tmp[cursor1]; //  Last elt of run 1 to end of merge
            } else if (len1 == 0) {
                LOGE("Comparison method violates its general contract!");
                return false;
            } else {
                SkASSERT(len2 == 0);
                SkASSERT(len1 > 1);
                a.setRegion(dest, tmp, cursor1, len1);
                //System.arraycopy(tmp, cursor1, a, dest, len1);
            }
            return true;
        }

        /**
    * Like mergeLo, except that this method should be called only if
    * len1 >= len2; mergeLo should be called if len1 <= len2.  (Either method
    * may be called if len1 == len2.)
    *
    * @param base1 index of first element in first run to be merged
    * @param len1  length of first run to be merged (must be > 0)
    * @param base2 index of first element in second run to be merged
    *        (must be aBase + aLen)
    * @param len2  length of second run to be merged (must be > 0)
    */
        bool mergeHi(int base1, int len1, int base2, int len2) {
            SkASSERT(len1 > 0 && len2 > 0 && base1 + len1 == base2);

            // Copy first run into temp array
            Array<T> a = this->a;
            Array<T> tmp = ensureCapacity(len1);
            int tmpBase = this->tmpBase;
            tmp.setRegion(tmpBase, a, base2, len2);
            //System.arraycopy(a, base2, tmp, tmpBase, len2);

            int cursor1 = base1 + len1 - 1;  // Indexes into a
            int cursor2 = tmpBase + len2 - 1; // Indexes into tmp array
            int dest = base2 + len2 - 1;     // Indexes into a

            // Move last element of first run and deal with degenerate cases
            a[dest--] = a[cursor1--];
            if (--len1 == 0) {
                a.setRegion(dest - (len2 - 1), tmp, tmpBase, len2);
                // System.arraycopy(tmp, tmpBase, a, dest - (len2 - 1), len2);
                return true;
            }
            if (len2 == 1) {
                dest -= len1;
                cursor1 -= len1;
                a.setRegion(dest + 1, a, cursor1 + 1, len1);
                // System.arraycopy(a, cursor1 + 1, a, dest + 1, len1);
                a[dest] = tmp[cursor2];
                return true;
            }

            Comparator<T> *c = this->c;  // Use local variable for performance
            int minGallop = this->minGallop;    //  "    "       "     "      "
            outer:
            while (true) {
                int count1 = 0; // Number of times in a row that first run won
                int count2 = 0; // Number of times in a row that second run won

                /*
                 * Do the straightforward thing until (if ever) one run
                 * appears to win consistently.
                 */
                do {
                    SkASSERT(len1 > 0 && len2 > 1);
                    if (c->compare(&tmp[cursor2], &a[cursor1]) < 0) {
                        a[dest--] = a[cursor1--];
                        count1++;
                        count2 = 0;
                        if (--len1 == 0)
                            goto next;
                    } else {
                        a[dest--] = tmp[cursor2--];
                        count2++;
                        count1 = 0;
                        if (--len2 == 1)
                            goto next;
                    }
                } while ((count1 | count2) < minGallop);

                /*
                 * One run is winning so consistently that galloping may be a
                 * huge win. So try that, and continue galloping until (if ever)
                 * neither run appears to be winning consistently anymore.
                 */
                do {
                    SkASSERT(len1 > 0 && len2 > 1);
                    count1 = len1 - gallopRight(&tmp[cursor2], a, base1, len1, len1 - 1, c);
                    if (count1 != 0) {
                        dest -= count1;
                        cursor1 -= count1;
                        len1 -= count1;
                        a.setRegion(dest + 1, a, cursor1 + 1, count1);
                        //System.arraycopy(a, cursor1 + 1, a, dest + 1, count1);
                        if (len1 == 0)
                            goto next;
                    }
                    a[dest--] = tmp[cursor2--];
                    if (--len2 == 1)
                        goto next;

                    count2 = len2 - gallopLeft(&a[cursor1], tmp, tmpBase, len2, len2 - 1, c);
                    if (count2 != 0) {
                        dest -= count2;
                        cursor2 -= count2;
                        len2 -= count2;
                        a.setRegion(dest + 1, tmp, cursor2 + 1, count2);
                        //System.arraycopy(tmp, cursor2 + 1, a, dest + 1, count2);
                        if (len2 <= 1)  // len2 == 1 || len2 == 0
                            goto next;
                    }
                    a[dest--] = a[cursor1--];
                    if (--len1 == 0)
                        goto next;
                    minGallop--;
                } while (count1 >= MIN_GALLOP | count2 >= MIN_GALLOP);
                if (minGallop < 0)
                    minGallop = 0;
                minGallop += 2;  // Penalize for leaving gallop mode
            }  // End of "outer" loop
            next:
            this->minGallop = minGallop < 1 ? 1 : minGallop;  // Write back to field

            if (len2 == 1) {
                SkASSERT(len1 > 0);
                dest -= len1;
                cursor1 -= len1;
                a.setRegion(dest + 1, a, cursor1 + 1, len1);
                //System.arraycopy(a, cursor1 + 1, a, dest + 1, len1);
                a[dest] = tmp[cursor2];  // Move first elt of run2 to front of merge
            } else if (len2 == 0) {
                LOGE("Comparison method violates its general contract!");
                return false;
            } else {
                SkASSERT(len1 == 0);
                SkASSERT(len2 > 0);
                a.setRegion(dest - (len2 - 1), tmp, tmpBase, len2);
                // System.arraycopy(tmp, tmpBase, a, dest - (len2 - 1), len2);
            }
            return true;
        }

        /**
    * Ensures that the external array tmp has at least the specified
    * number of elements, increasing its size if necessary.  The size
    * increases exponentially to ensure amortized linear time complexity.
    *
    * @param minCapacity the minimum required capacity of the tmp array
    * @return tmp, whether or not it grew
    */
        Array<T> ensureCapacity(int minCapacity) {
            if (tmpLen < minCapacity) {
                // Compute smallest power of 2 > minCapacity
                int newSize = minCapacity;
                newSize |= newSize >> 1;
                newSize |= newSize >> 2;
                newSize |= newSize >> 4;
                newSize |= newSize >> 8;
                newSize |= newSize >> 16;
                newSize++;

                if (newSize < 0) // Not bloody likely!
                    newSize = minCapacity;
                else
                    newSize = bx::min(newSize, unsign_shift_right(a.size(), 1));

                tmp.setSize(newSize);
                tmpLen = newSize;
                tmpBase = 0;
            }
            return tmp;
        }
    };
}

#endif //BGFX_STUDY_TIMSORT_H

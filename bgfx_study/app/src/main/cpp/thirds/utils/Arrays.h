//
// Created by Administrator on 2020/12/28 0028.
//

#ifndef BGFX_STUDY_ARRAYS_H
#define BGFX_STUDY_ARRAYS_H

#include "Array.h"
#include "Comparator.h"
#include "TimSort.h"

namespace h7{
    class Arrays{
    public:

        /**
    * Sorts the specified array of objects according to the order induced by
    * the specified comparator.  All elements in the array must be
    * <i>mutually comparable</i> by the specified comparator (that is,
    * {@code c.compare(e1, e2)} must not throw a {@code ClassCastException}
    * for any elements {@code e1} and {@code e2} in the array).
    *
    * <p>This sort is guaranteed to be <i>stable</i>:  equal elements will
    * not be reordered as a result of the sort.
    *
    * <p>Implementation note: This implementation is a stable, adaptive,
    * iterative mergesort that requires far fewer than n lg(n) comparisons
    * when the input array is partially sorted, while offering the
    * performance of a traditional mergesort when the input array is
    * randomly ordered.  If the input array is nearly sorted, the
    * implementation requires approximately n comparisons.  Temporary
    * storage requirements vary from a small constant for nearly sorted
    * input arrays to n/2 object references for randomly ordered input
    * arrays.
    *
    * <p>The implementation takes equal advantage of ascending and
    * descending order in its input array, and can take advantage of
    * ascending and descending order in different parts of the the same
    * input array.  It is well-suited to merging two or more sorted arrays:
    * simply concatenate the arrays and sort the resulting array.
    *
    * <p>The implementation was adapted from Tim Peters's list sort for Python
    * (<a href="http://svn.python.org/projects/python/trunk/Objects/listsort.txt">
    * TimSort</a>).  It uses techniques from Peter McIlroy's "Optimistic
    * Sorting and Information Theoretic Complexity", in Proceedings of the
    * Fourth Annual ACM-SIAM Symposium on Discrete Algorithms, pp 467-474,
    * January 1993.
    *
    * @param <T> the class of the objects to be sorted
    * @param a the array to be sorted
    * @param c the comparator to determine the order of the array.  A
    *        {@code null} value indicates that the elements'
    *        {@linkplain Comparable natural ordering} should be used.
    * @throws ClassCastException if the array contains elements that are
    *         not <i>mutually comparable</i> using the specified comparator
    * @throws IllegalArgumentException (optional) if the comparator is
    *         found to violate the {@link Comparator} contract
    */
    template <typename T>
    static bool sort(Array<T>& a, Comparator<T>* c) {
            if (c == nullptr) {
                return false;
            } else {
                TimSort<T>::sort(a, 0, a.size(), c, nullptr, 0, 0);
                return true;
            }
        }
    };
}

#endif //BGFX_STUDY_ARRAYS_H

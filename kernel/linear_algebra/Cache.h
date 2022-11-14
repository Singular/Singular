#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <list>

// #include <assert.h>
// using namespace std;

/*! \class Cache
    \brief Class Cache is a template-implementation of a cache with
    arbitrary classes for representing keys and values, respectively.

    Each entry of the cache is of the form <em>key --> value</em>,
    where \e key is an instance of some \c KeyClass, and \e value
    an instance of some \c ValueClass.<br><br>
    This implementation comes with the possibility to define bounds
    on both the number of cached pairs <em>key --> value</em> and on
    the total \e weight of the cache.<br>
    Here, the \e weight of a cache is defined as the sum
    of \e weights of all cached values, where the \e weight of each value
    needs to be implemented in the actual class \c ValueClass.
    An example for the weight of a cached value
    may simply be its size in bytes, or may capture some other useful notion
    of how \e heavy the value is. E.g., when caching polynomials, the \e weight
    may be defined to be equal to the number of its monomials.<br><br>
    The <em>key --> value</em> pairs of a cache are being stored in two
    standard lists \c _key and \c _value of equal length <em>L</em>.<br>
    In order to enable a fast
    value lookup, the vector \c _key maintains an ordering such that<br>
    <em> i < j  ==>  _key[i] < _key[j],</em><br>
    where the right-hand side comparator
    \e < needs to be implemented by class \c KeyClass. Note that this
    ordering allows for value lookup in time <em>O(log(L))</em>, when given
    a specific key.<br><br>
    In addition to \c _key and \c _value, there is a third book-keeping
    structure in Cache: The vector \c _rank of integers captures ranking
    information among all cached values. More concretely,<br>
    <em>_rank : {0, 1, 2, ..., L - 1} --> {0, 1, 2, ..., L - 1}</em><br>
    is a bijection with the semantic<br>
    <em>_rank[s] < _rank[t] :<==> _value[_rank[s]] < _value[_rank[t]],</em><br>
    where the right-hand side comparator \e < needs to be
    implemented by class \c ValueClass. The intention here is that any relation
    <em>_rank[s] < _rank[t]</em>
    is to imply that the key-value pair
    <em>_key[_rank[t]] --> _value[_rank[t]]</em> will be kept at least as
    long in cache as <em> _key[_rank[s]] --> _value[_rank[s]]</em>.
    (I.e., loosely speaking, the higher the rank, the longer a pair is going
    to be cached.)<br><br>
    Whenever the cache becomes either too large (in terms of number of
    entries), or too \e heavy (in terms of accumulated weight), it will
    automatically shrink until both bounds will have been re-established. To
    this end, the <em>key --> value</em> pair with least value rank will be
    erased from the cache. This process is repeated until the cache is small
    enough again, in terms of both the number of entries and the \e weight.
    (Note that it may be necessary to erase numerous pairs after inserting
    just one <em>key --> value</em> when this value's \e weight is
    unproportionally high.)<br><br>
    In order to make the above procedures work, the two template classes
    \c KeyClass and \c ValueClass need to implement the following methods:<br>
    <c>bool KeyClass::operator< (const KeyClass& key),</c><br>
    <c>bool KeyClass::operator== (const KeyClass& key),</c><br>
    <c>bool ValueClass::operator< (const ValueClass& key),</c><br>
    <c>bool ValueClass::operator== (const ValueClass& key),</c><br>
    <c>int ValueClass::getWeight ().</c>
    \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
*/
template<class KeyClass, class ValueClass> class Cache
{
  private:
     /**
     * A bijection on the set {0, ..., _key.size() - 1}.<br>
     * Semantically, <c>_rank(j) > _rank(i)</c> means that the pair
     * <c>key(_rank(j)) --> value(_rank(j))</c> will be cached at least
     * as long as the pair <c>key(_rank(i)) -->  value(_rank(i))</c>.
     */
   std::list<int> _rank;

     /**
     * _key is sorted in ascending order, i.e.,
     * <em>j < i  ==>  _key(j) < _key(i),</em>
     * where the right-hand side comparator "<" needs to be implemented
     * in KeyClass.
     */
     std::list<KeyClass> _key;

     /**
     *  _value captures the actual objects of interest;<br>
     * \c _value[i] corresponds to \c _key[i] and may be retrieved
     * by calling Cache::getValue (const KeyClass&) const with the
     * argument \c _key[i]).
     */
     std::list<ValueClass> _value;

     /**
     * container for the weights of all cached values
     */
     std::list<int> _weights;

     /**
     * a pointer to some element of _key;
     * We make this mutable so that methods which leave the cache
     * unmodified but which alter _itKey can still be declared as
     * const, as the user would expect for these methods.
     */
     mutable typename std::list<KeyClass>::const_iterator _itKey;

     /**
     * a pointer to some element of _value;
     * We make this mutable so that methods which leave the cache
     * unmodified but which alter _itValue can still be declared as
     * const, as the user would expect for these methods.
     */
     mutable typename std::list<ValueClass>::const_iterator _itValue;

     /**
     * for storing the momentary weight of the given cache;<br>
     * This is the sum of \c _value[i].getWeight() over all \e i,
     * i.e., over all cached values. Equivalently, this is equal
     * to all weights stored in _weights.
     */
     int _weight;

     /**
     * the bound for the number of cached <c>key --> value</c> pairs;<br>
     * The cache will automatically ensure that this bound will never be
     * exceeded;
     * see Cache::shrink (const KeyClass&) and Cache::deleteLast ().
     */
     int _maxEntries;

     /**
     * the bound on total cache weight;<br>
     * The cache will automatically ensure that this bound will never be
     * exceeded; see
     * see Cache::shrink (const KeyClass&) and Cache::deleteLast ().
     */
     int _maxWeight;

     /**
     * A method for providing the index of a given key in the vector _key.
     * Either _key contains the given key, then its index will be returned.
     * Otherwise the position in _key, at which the given key should be placed
     * (with respect to the ordering in _key) is returned.
     * @param key an instance of KeyClass
     * @return the actual or would-be index of key in _key
     * @see Cache::hasKey (const KeyClass&) const
     */
     int getIndexInKey (const KeyClass& key) const;

     /**
     * A method for providing the index of a given value in the vector _rank.
     * Based on the rank of the given value, the position in _rank at which
     * the given value should be inserted, is returned.
     * (The method also works, when the given value is already contained in
     * the cache.)
     * @param value an instance of ValueClass
     * @return the actual or would-be index of value in _rank
     */
     int getIndexInRank (const ValueClass& value) const;

     /**
     * A method for shrinking the given cache so that it meet the bounds on
     * the maximum number of entries and total weight again.
     * The method returns true iff the process of shrinking deleted a pair
     * (k --> v) from the cache such that k equals the given key.
     * @param key an instance of KeyClass
     * @return true iff shrinking deleted a pair (key --> *)
     */
     bool shrink (const KeyClass& key);

     /**
     * A method for deleting the least-ranked cache entry.
     * The method returns true iff the deleted pair (k --> v)
     * satisfies k == key.
     * @param key an instance of KeyClass
     * @return true iff a pair (key --> *) was deleted
     */
     bool deleteLast (const KeyClass& key);
  public:
     /**
     * A constructor for class Cache.
     * The method makes sure that all member vectors be empty.
     */
     Cache();

     /**
     * A destructor for class Cache.
     * The method clears all member vectors. (This includes that
     * destructors are invoked, accordingly.)
     */
     ~Cache();

     /**
     * Copy implementation for class Cache.
     * Apart from copying all flat members, all vectors are being
     * deep-copied.
     */
     Cache (const Cache& c);

     /**
     * A user-suited constructor for class Cache.
     * The method makes sure that all member vectors be empty.
     * Moreover, the user can provide bounds for the maximum
     * number of entries in the cache, and for the total weight
     * of the cache.
     * @param maxEntries the (positive) maximum number of pairs
              (key --> value) in the cache
     * @param maxWeight the (positive) maximum weight of the cache
     */
     Cache (const int maxEntries, const int maxWeight);

     /**
     * A method for retrieving the momentary weight of the cache.
     * The return value will always be less than or equal to the result
     * of Cache::getMaxWeight () const.<br>
     * Semantically, the total weight of a cache is the sum of weights
     * of all cached values.
     * @return the momentary weight of the cache
     * @see Cache::getMaxWeight () const
     * @see MinorValue::getWeight () const
     */
     int getWeight () const;

     /**
     * A method for retrieving the momentary number of (key --> value) pairs
     * in the cache.
     * The return value will always be less than or equal to the result of
     * Cache::getMaxNumberOfEntries () const.
     * @return the momentary number of cached values of the cache
     * @see Cache::getMaxNumberOfEntries () const
     */
     int getNumberOfEntries () const;

     /**
     * A method for retrieving the maximum number of (key --> value) pairs
     * in the cache.
     * @return the maximum number of cached values of the cache
     * @see Cache::getNumberOfEntries () const
     * @see Cache::Cache (const int, const int)
     */
     int getMaxNumberOfEntries () const;

     /**
     * A method for retrieving the maximum weight of the cache.
     * @return the maximum weight of the cache
     * @see Cache::getWeight () const
     * @see Cache::Cache (const int, const int)
     */
     int getMaxWeight () const;

     /**
     * Checks whether the cache contains a pair (k --> v) such that
     * k equals the given key.
     * If so, the method returns true; false otherwise.
     * In order to make Cache::getValue (const KeyClass&) const
     * work properly, the user is strongly advised to always check key
     * containment by means of Cache::hasKey (const KeyClass&) const.
     * (The implementation at hand ensures that invoking hasKey and
     * getValue does not result in extra computational efforts.)
     * @param key the key for which containment is to be checked
     * @return true iff the cache contains the given key
     * @see Cache::getValue (const KeyClass&) const
     */
     bool hasKey (const KeyClass& key) const;

     /**
     * Returns the value for a given key.
     * The method assumes that there is actually an entry of the form
     * (key --> *) in the cache. This can be checked before using
     * Cache::hasKey (const KeyClass&) const. (Note that calling
     * both methods in direct succession does not result in extra
     * computational efforts.)
     * \par Assertions
     * If the given key is not contained in the cache, program execution
     * will be stopped.
     * @param key the key, for which the corresponding value is to be returned
     * @return the value corresponding to the given key
     * @see Cache::hasKey (const KeyClass&) const
     */
     ValueClass getValue (const KeyClass& key) const;

     /**
     * Inserts the pair (key --> value) in the cache.
     * If there is already some entry (key --> value'), then value will be
     * replaced by value'.
     * As putting some new pair in the cache may result in a violation
     * of the maximal number of entries or the weight of the cache, or both,
     * Cache::put (const KeyClass&, const ValueClass&) will always finalize by
     * calling the private method Cache::shrink(const KeyClass&), in order to
     * re-establish both bounds. Note that this may even result in deleting the
     * newly inserted pair (key --> value).<br>
     * Because of that undesirable but possible effect, the method returns
     * whether the pair is actually contained in the cache after invocation of
     * Cache::put (const KeyClass&, const ValueClass&).
     * @param key an instance of KeyClass
     * @param value an instance of ValueClass
     * @return whether the pair (key --> value) is contained in the modified
     *         cache
     * @see Cache::getValue (const KeyClass&) const
     */
     bool put (const KeyClass& key, const ValueClass& value);

     /**
     * Clears the cache so that it has no entry. This method will also
     * enforce destruction of all former entries of the cache.
     */
     void clear ();

     /**
     * A method for providing a printable version of the represented
     * cache, including all contained (key --> value) pairs.
     * @return a printable version of the given instance as instance of class
     *         string
     */
   std::string toString () const;

     /**
     * A method for printing a string representation of the given cache to
     * std::cout. This includes string representations of all contained
     * (key --> value) pairs.
     */
     void print () const;
};

#include "kernel/linear_algebra/CacheImplementation.h"

#endif
/* CACHE_H */

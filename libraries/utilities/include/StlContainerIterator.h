////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlContainerIterator.h (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> An adapter that transforms a begin/end pair of STL iterators into a read-only forward iterator with IsValid, Next, and Get functions</summary>
    template <typename IteratorType>
    class StlContainerIteratorBase
    {
    public:
        StlContainerIteratorBase() = default;

        /// <summary> Constructor </summary>
        ///
        /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
        /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
        StlContainerIteratorBase(IteratorType begin, IteratorType end);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _current != _end; }

        /// <summary> Returns true if the iterator knows its size. </summary>
        ///
        /// <returns> true if NumItemsLeft returns a valid number, false if not. </returns>
        bool HasSize() const { return true; }

        /// <summary>
        /// Returns the number of iterates left in this iterator, including the current one.
        /// </summary>
        ///
        /// <returns> The total number of iterates left. </returns>
        size_t NumItemsLeft() const { return _end - _current; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

    protected:
        IteratorType _current;
        IteratorType _end;
    };

    /// <summary> An adapter that transforms a begin/end pair of STL iterators into a read-only forward iterator with IsValid, Next, and Get functions</summary>
    template <typename IteratorType, typename ValueType = typename std::iterator_traits<std::decay_t<IteratorType>>::value_type>
    class StlContainerIterator : public StlContainerIteratorBase<IteratorType>
    {
        using BaseType = StlContainerIteratorBase<IteratorType>;

    public:
        using BaseType::BaseType;

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The value of the current iterate. </returns>
        ValueType Get() const { return *_current; }

    protected:
        using BaseType::_current;
    };

    /// <summary> Handy type alias for a StlContainerIterator over a std::vector </summary>
    template <typename ValueType>
    using VectorIterator = StlContainerIterator<typename std::vector<ValueType>::const_iterator>;

    /// <summary> Convenience function for creating StlContainerIterators </summary>
    ///
    /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
    /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
    ///
    /// <returns> A StlContainerIterator over the range specified by the begin and end iterators. </returns>
    template <typename IteratorType>
    auto MakeStlContainerIterator(IteratorType begin, IteratorType end) -> StlContainerIterator<IteratorType>;

    /// <summary> Convenience function for creating StlContainerIterators </summary>
    ///
    /// <param name="container"> The C++-conforming iterable container to iterate over. </param>
    ///
    /// <returns> A StlContainerIterator over the container</returns>
    template <typename ContainerType>
    auto MakeStlContainerIterator(ContainerType& container) -> StlContainerIterator<typename ContainerType::iterator, typename ContainerType::value_type>;

    /// <summary> An adapter that transforms a begin/end pair of STL iterators into a read-only forward iterator with IsValid, Next, and Get functions</summary>
    template <typename IteratorType, typename ValueType = typename std::iterator_traits<std::decay_t<IteratorType>>::value_type>
    class StlContainerReferenceIterator : public StlContainerIteratorBase<IteratorType>
    {
        using BaseType = StlContainerIteratorBase<IteratorType>;

    public:
        using BaseType::BaseType;

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The value of the current iterate. </returns>
        const ValueType& Get() const { return *_current; }

    protected:
        using BaseType::_current;
    };

    /// <summary> Handy type alias for a StlContainerReferenceIterator over a std::vector </summary>
    template <typename ValueType>
    using VectorReferenceIterator = StlContainerReferenceIterator<typename std::vector<ValueType>::const_iterator>;

    /// <summary> Convenience function for creating StlContainerReferenceIterators </summary>
    ///
    /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
    /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
    ///
    /// <returns> A StlContainerReferenceIterator over the range specified by the begin and end iterators. </returns>
    template <typename IteratorType>
    auto MakeStlContainerReferenceIterator(IteratorType begin, IteratorType end) -> StlContainerReferenceIterator<IteratorType>;

    /// <summary> Convenience function for creating StlContainerReferenceIterators </summary>
    ///
    /// <param name="container"> The C++-conforming iterable container to iterate over. </param>
    ///
    /// <returns> A StlContainerReferenceIterator over the container</returns>
    template <typename ContainerType>
    auto MakeStlContainerReferenceIterator(ContainerType& container) -> StlContainerReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type>;
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    template <typename IteratorType>
    StlContainerIteratorBase<IteratorType>::StlContainerIteratorBase(IteratorType begin, IteratorType end) :
        _current(begin),
        _end(end)
    {
    }

    template <typename IteratorType>
    void StlContainerIteratorBase<IteratorType>::Next()
    {
        if (IsValid())
        {
            ++_current;
        }
    }

    template <typename IteratorType>
    StlContainerIterator<IteratorType> MakeStlContainerIterator(IteratorType begin, IteratorType end)
    {
        return StlContainerIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    StlContainerIterator<typename ContainerType::iterator, typename ContainerType::value_type> MakeStlContainerIterator(ContainerType& container)
    {
        return StlContainerIterator<typename ContainerType::iterator, typename ContainerType::value_type>(container.begin(), container.end());
    }

    template <typename IteratorType>
    StlContainerReferenceIterator<IteratorType> MakeStlContainerReferenceIterator(IteratorType begin, IteratorType end)
    {
        return StlContainerReferenceIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    StlContainerReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type> MakeStlContainerReferenceIterator(ContainerType& container)
    {
        return StlContainerReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type>(container.begin(), container.end());
    }
} // namespace utilities
} // namespace ell

#pragma endregion implementation

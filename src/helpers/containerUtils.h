// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////

#ifndef containerUtils_h__
#define containerUtils_h__

#include "traits.h"
#include <algorithm>

namespace helpers{

    namespace detail{
        template<class T, EEraseIterValidy::Type T_EraseIterValidy = EraseIterValidy<T>::value>
        struct EraseImpl;

        template<class T>
        struct EraseImpl<T, EEraseIterValidy::IterReturned>
        {
            typedef typename T::iterator iterator;
            typedef typename T::const_iterator const_iterator;
            static iterator erase(T& container, iterator it) {
                return container.erase(it);
            }
            static const_iterator erase(T& container, const_iterator it) {
                return container.erase(it);
            }
        };
        template<class T>
        struct EraseImpl<T, EEraseIterValidy::NextValid>
        {
            // This one gets used for e.g. std::set whos erase does not return
            // an iterator until C++11
            typedef typename T::iterator iterator;
            typedef typename T::const_iterator const_iterator;
            static iterator erase(T& container, iterator it) {
                container.erase(it++);
                return it;
            }
            static const_iterator erase(T& container, const_iterator it) {
                container.erase(it++);
                return it;
            }
        };

        template<class T>
        struct EraseImpl<T, EEraseIterValidy::PrevValid>
        {
            typedef typename T::iterator iterator;
            typedef typename T::const_iterator const_iterator;
            static iterator erase(T& container, iterator it) {
                // If only previous iterators remain valid, store the predecessor
                // and return this after incrementing it after the erase
                // Corner case: If we erase the first element there is no previous one and we return begin()
                bool isBegin = it == container.begin();
                iterator tmp = it;
                if(!isBegin)
                    --tmp;
                container.erase(it);
                if(isBegin)
                    tmp = container.begin();
                else
                    ++tmp;
                return tmp;
            }
            static const_iterator erase(T& container, const_iterator it) {
                bool isBegin = it == container.begin();
                iterator tmp = it;
                if(!isBegin)
                    --tmp;
                container.erase(it);
                if(isBegin)
                    tmp = container.begin();
                else
                    ++tmp;
                return tmp;
            }
        };

        template<class T, bool T_hasPopFront = helpers::has_member_function_pop_front<T, void>::value>
        struct PopFrontImpl
        {
            static void pop(T& container)
            {
                container.pop_front();
            }
        };
        template<class T>
        struct PopFrontImpl<T, false>
        {
            static void pop(T& container)
            {
                container.erase(container.begin());
            }
        };
    } // namespace detail


    /// Removes an element from a container by its iterator and returns an iterator to the next element
    /// Works only for list and set as they don't invalidate other iterators, so erase is save to call inside a loop
    /// Works also for reverse iterators
    template<typename T>
    inline typename T::iterator erase(T& container, typename T::iterator it)
    {
        return detail::EraseImpl<T>::erase(container, it);
    }

    /*template<typename T>
    inline typename T::const_iterator erase(T& container, typename T::const_iterator it)
    {
        return EraseFromContainer<T>::erase(it);
    }*/

    template<typename T>
    inline typename T::reverse_iterator erase(T& container, typename T::reverse_iterator it)
    {
        typename T::reverse_iterator tmp = it;
        return typename T::reverse_iterator(erase(container, (++tmp).base()));
    }

    /*template<typename T>
    inline typename T::const_reverse_iterator erase(T& container, typename T::const_reverse_iterator it)
    {
        typename std::set<T>::const_reverse_iterator tmp = it;
        return typename T::const_reverse_iterator(erase(container, (++tmp).base()));
    }*/

    /// Removes the first element in a container
    template<typename T>
    inline void pop_front(T& container)
    {
        assert(!container.empty());
        detail::PopFrontImpl<T>::pop(container);
    }

    /// Returns true if the container contains the given value
    template<typename T, typename U>
    bool contains(const T& container, const U& value)
    {
        return std::find(container.begin(), container.end(), value) != container.end();
    }

} // namespace helpers

#endif // containerUtils_h__
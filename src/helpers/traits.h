// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your oposion) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifndef traits_h__
#define traits_h__

#include <boost/tti/has_member_function.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits.hpp>
#include <assert.h>

namespace helpers{

    /// Removes a pointer declaration from a type
    /// int* -> int; int ->int; int** -> int*
    template<typename T>
    struct remove_pointer
    {
        typedef T type;
    };
    template<typename T>
    struct remove_pointer<T*>
    {
        typedef T type;
    };


    /// Checks if the given type as a member function called reserve
    BOOST_TTI_HAS_MEMBER_FUNCTION(reserve)
    /// Checks if the given type as a member function called push_back
    BOOST_TTI_HAS_MEMBER_FUNCTION(push_back)
    BOOST_TTI_HAS_MEMBER_FUNCTION(pop_front)

    struct EEraseIterValidy
    {
        enum Type
        {
            IterReturned,  // Erase returns an iterator
            NextValid,     // Erase preserves validy of iterators after the erased one (or all others)
            PrevValid,     // Erase preserves validy of iterators preceding the erased one
            AllInvalidated // Erase invalidates all -> Erase from loop is not possible
        };
        Type t_;
        EEraseIterValidy(Type t) : t_(t) { assert(t_ >= IterReturned && t_ <= PrevValid); }
        operator Type() const { return t_; }
    private:
        //prevent automatic conversion for any other built-in types such as bool, int, etc
        template<typename T>
        operator T() const;
    };

    namespace detail{

        /// "Stores a type"
        template<typename T> struct Type2Type{
            typedef T type;
        };

        /// Evil hackery to put an expression into a type
        template<typename T>
        Type2Type<T> operator,(T, Type2Type<void>);

        template<typename T>
        T declval();

        template<class T>
        struct EraseReturnsIterator
        {
            typedef typename T::iterator iterator;
            typedef BOOST_TYPEOF_TPL((declval<T>().erase(declval<iterator>()), Type2Type<void>())) result;

            static CONSTEXPR bool value = boost::is_same<iterator, typename result::type>::value;
        };

    } // namespace detail

    /// Only specialize this one!
    template<class T>
    struct EraseIterValidyImpl{
        // Default case for unspecialized containers (whose erase dos not return an iterator) is AllInvalidate to be safe
        static CONSTEXPR EEraseIterValidy::Type value = EEraseIterValidy::AllInvalidated; 
    };

    /// Trait that returns the iterator validy (Enum member) after an erase call
    /// Not 
    template<class T, bool T_IterReturned = detail::EraseReturnsIterator<T>::value>
    struct EraseIterValidy
    {
        // If erase returns an iterator, just take it -> Done
        static CONSTEXPR EEraseIterValidy::Type value = EEraseIterValidy::IterReturned; 
    };
    /// If erase does not return an iterator fall back to
    template<class T>
    struct EraseIterValidy<T, false>: EraseIterValidyImpl<T>{};
}

#endif // traits_h__
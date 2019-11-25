#ifndef _melder_templates_h_
#define _melder_templates_h_
/* melder_templates.h
 *
 * Copyright (C) 1992-2019 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

//#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if<std::is_base_of<B,A>::value>::type
#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if_t<std::is_base_of<B,A>::value>
//#define Melder_ENABLE_IF_ISA(A,B)  , class = typename std::enable_if_v<std::is_base_of<B,A>>

template <typename Ret, typename T, typename... Args>
class MelderCallback {
	public:
		using FunctionType = Ret* (*) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  /*Melder_ENABLE_IF_ISA(T2,T)*/, typename Ret2  /*Melder_ENABLE_IF_ISA(Ret2,Ret)*/>
		MelderCallback (Ret2* (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) {
			static_assert (std::is_base_of <T, T2> :: value,
				"First argument of MelderCallback should have covariant type.");
			static_assert (std::is_base_of <Ret, Ret2> :: value,
				"Return type of MelderCallback should be covariant.");
		};
		Ret* operator () (T* data, Args ... args) { return _f (data, std::forward<Args>(args)...); }
		explicit operator bool () const noexcept { return !! _f; }
	private:
		FunctionType _f;
};
template <typename T, typename... Args>
class MelderCallback <void, T, Args...> {   // specialization
	public:
		using FunctionType = void (*) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  /*Melder_ENABLE_IF_ISA(T2,T)*/>
		MelderCallback (void (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) {
			static_assert (std::is_base_of <T, T2> :: value,
				"First argument of MelderCallback should have covariant type.");
		};
		void operator () (T* data, Args ... args) { _f (data, std::forward<Args>(args)...); }
		explicit operator bool () const noexcept { return !! _f; }
	private:
		FunctionType _f;
};
template <typename T, typename... Args>
class MelderCallback <int, T, Args...> {   // specialization
	public:
		using FunctionType = int (*) (T*, Args...);
		MelderCallback (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  /*Melder_ENABLE_IF_ISA(T2,T)*/>
		MelderCallback (int (*f) (T2*, Args...)) : _f (reinterpret_cast<FunctionType> (f)) {
			static_assert (std::is_base_of <T, T2> :: value,
				"First argument of MelderCallback should have covariant type.");
		};
		int operator () (T* data, Args ... args) { return _f (data, std::forward<Args>(args)...); }
		explicit operator bool () const noexcept { return !! _f; }
	private:
		FunctionType _f;
};

template <typename T>
class MelderCompareHook {
	public:
		typedef int (*FunctionType) (T*, T*);
		MelderCompareHook (FunctionType f = nullptr) : _f (f) { }
		template <typename T2  Melder_ENABLE_IF_ISA(T2,T)>
			MelderCompareHook (int (*f) (T2*, T2*)) : _f (reinterpret_cast<FunctionType> (f)) { };
		int operator () (T* data1, T* data2) noexcept { return _f (data1, data2); }
		explicit operator bool () const noexcept { return !! _f; }
		FunctionType get () { return _f; }
	private:
		FunctionType _f;
};

/* End of file melder_templates.h */
#endif

#pragma once
#include <unordered_map>
#include <tuple>
#include <type_traits>
//#include "flat_hash_map.hpp"

template<typename val,typename... Args>
class multiKeyMap {
public:
	using type = multiKeyMap<val, Args...>;// = type;
	using tupleType = std::tuple<Args...>;
	multiKeyMap() = default;
	val& get(Args... args) {
		return m_data[std::make_tuple(std::forward<Args>(args)...)];
	};
	auto set(val first, Args... args) {
		return m_data.insert(std::make_pair(std::make_tuple(std::forward<Args>(args)...),std::move(first)));
	};
	auto& begin() {
		return m_data.begin();
	}
	auto& end() {
		return m_data.end();
	}
	auto& cbegin() {
		return m_data.cbegin();
	}
	auto& cend() {
		return m_data.cend();
	}
	auto& find(Args... args) {
		return m_data.find(std::forward<Args>(args)...);
	}
	val& operator()(Args... args) {
		return m_data[std::make_tuple(std::forward<Args>(args)...)];
	}
	template<typename T>
	auto operator[](T t) {
		return hmm<sizeof...(Args)-1, T>(*this,t);
	}

	template<int i,typename ... Targs>
	class hmm {
	public:
		hmm(type& m, Targs&&... args) :mapy(m), m_stuff(std::make_tuple(std::forward<Targs>(args)...)) {};
		hmm(type& m, std::tuple<Targs...>&& args) :mapy(m),m_stuff(std::move(args)) {};

		template<typename T>
		auto operator[](T t) const{
			return hmm<i - 1, Targs..., T>(mapy,std::tuple_cat(std::move(m_stuff),std::make_tuple(std::move(t))));
		}
	private:
		type& mapy;
		std::tuple<Targs...> m_stuff;
	};
	template<typename ... Targs>
	class hmm<1,Targs...> {
	public:
		hmm(type& m, Targs&&... args) :mapy(m), m_stuff(std::make_tuple(std::forward<Targs>(args)...)) {};
		hmm(type& m, std::tuple<Targs...>&& args) :mapy(m), m_stuff(std::move(args)) {};

		template<typename T>
		val& operator[](T t)const {
			return mapy.m_data[std::tuple_cat(std::move(m_stuff), std::make_tuple(std::move(t)))];
		}
	private:
		type& mapy;
		std::tuple<Targs...> m_stuff;
	};
private:
	class tupleHash {
		public:
			std::size_t operator()(const std::tuple<Args...>& thing) const{
				//std::cout << std::hash<std::size_t>()(worky<0>()(thing)) << "\n";
				//std::cout << std::hash<std::size_t>()(tupleHashImpl(thing, std::index_sequence_for<Args...>())) << '\n';
				return std::hash<std::size_t>()(tupleHashImpl(thing, std::index_sequence_for<Args...>()));
			};
		private:
			template<std::size_t ...i>
			std::size_t tupleHashImpl(const std::tuple<Args...>& thing,std::index_sequence<i...>)const{
				std::size_t reVal = 0;
				//( ( reVal^std::hash<std::decay_t<decltype(std::get<i>(thing))>>()(std::get<i>(thing)) ), ...);
				(void)std::initializer_list<int>{(reVal^=7*i+(i+1)*std::hash<std::decay_t<decltype(std::get<i>(thing))>>()(std::get<i>(thing)), 0)...};
				return reVal;
			}
		/*
		template<int i>
		struct worky{
			std::size_t operator()(const std::tuple<Args...>& things)const {
				return (7*i+(i+1)*std::hash<std::decay<decltype(std::get<i>(things))>::type>()(std::get<i>(things))) ^ worky<i + 1>()(things);
			}
		};
		template<>
		struct worky<sizeof...(Args)> {
			std::size_t operator()(const std::tuple<Args...>& things)const {
				return 0;
			}
		};
		//*/
	};
	friend class hmm<1, Args...>;
	std::unordered_map<std::tuple<Args...>, val,tupleHash> m_data;
};


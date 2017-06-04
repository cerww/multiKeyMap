#pragma once
#include <unordered_map>
#include <tuple>
#include <type_traits>

template<typename val,typename... Args>
class multiKeyMap {
public:
	using type = multiKeyMap<val, Args...>;// = type;
	using tupleType = std::tuple<Args...>;
	constexpr multiKeyMap() = default;
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
	const auto& cbegin() {
		return m_data.cbegin();
	}
	const auto& cend() {
		return m_data.cend();
	}
	auto& find(Args... args) {
		return m_data.find(std::make_tuple(std::forward<Args>(args)...));
	}
	val& operator()(Args... args) {
		return m_data[std::make_tuple(std::forward<Args>(args)...)];
	}
	

	const val& operator[](const std::tuple<Args...>& abc) {
		return m_data[abc];
	}
	const val& operator[](const std::tuple<Args...>&& abc) {
		return m_data[std::forward<std::tuple<Args...>>(abc)];
	}
	
	template<typename T>
		const auto operator[](T t) {
		using currentType = std::decay_t<decltype(std::get<0>(tupleType{})) >;
		static_assert(std::is_convertible_v < std::decay_t<T>, std::decay_t<decltype(std::get<0>(tupleType{}))> >);

		return multiKeyMapIndexer<sizeof...(Args)-1, T>(*this, std::make_tuple(std::move(t)));
	}

	template<int i,typename ... Targs>
	class multiKeyMapIndexer {
	public:
		multiKeyMapIndexer(type& m, std::tuple<Targs...>&& args) :mapy(m),m_stuff(std::move(args)) {};
		template<typename T>
		const auto operator[](T t) const{
			using currentType = std::decay_t<decltype(std::get<sizeof...(Args)-i>(tupleType{}))>;
			static_assert(std::is_convertible_v < std::decay_t<T>, currentType >);

			return multiKeyMapIndexer<i - 1, Targs..., T>(mapy,std::tuple_cat(std::move(m_stuff),std::make_tuple(std::move(t))));
		}
	private:
		type& mapy;
		std::tuple<Targs...> m_stuff;
	};
	template<typename ... Targs>
	class multiKeyMapIndexer<1,Targs...> {
	public:
		multiKeyMapIndexer(type& m, std::tuple<Targs...>&& args) :mapy(m), m_stuff(std::move(args)) {};
		template<typename T>
		val& operator[](T t)const {
			using currentType = std::decay_t<decltype(std::get<sizeof...(Args)-1>(tupleType{})) > ;
			static_assert(std::is_convertible_v < std::decay_t<T>, currentType >);

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
				return std::hash<std::size_t>()(tupleHashImpl(thing, std::index_sequence_for<Args...>()));
			};
		private:
			template<std::size_t ...i>
			std::size_t tupleHashImpl(const std::tuple<Args...>& thing,std::index_sequence<i...>)const{
				std::size_t reVal = 0;
				//( ( reVal^std::hash<std::decay_t<decltype(std::get<i>(thing))>>()(std::get<i>(thing)) ), ...);
				(void)std::initializer_list<int>{(reVal^=(i+1)*(i+1)+(i+1)*std::hash<std::decay_t<decltype(std::get<i>(thing))>>()(std::get<i>(thing)), 0)...};
				return reVal;
			}
	};
	friend class multiKeyMapIndexer<1, Args...>;
	std::unordered_map<std::tuple<Args...>, val,tupleHash> m_data;
	//static_assert(tupleType, );
};


/*
template<int i>
struct worky{
std::size_t operator()(const std::tuple<Args...>& things)const {
return ((i+1)*(i+1)+(i+1)*std::hash<std::decay<decltype(std::get<i>(things))>::type>()(std::get<i>(things))) ^ worky<i + 1>()(things);
}
};
template<>
struct worky<sizeof...(Args)> {
std::size_t operator()(const std::tuple<Args...>& things)const {
return 0;
}
};
//*/
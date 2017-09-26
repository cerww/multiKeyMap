#pragma once
#include <unordered_map>
#include <tuple>
#include <type_traits>
constexpr uint64_t listOfPrimes[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,57,61,67,71,73,79,83,87,91,97,101,103,107,113,127 };
namespace std {
	template<typename ...Args>
	class hash<std::tuple<Args...>> {
	public:
		std::size_t operator()(const std::tuple<Args...>& thing) const {
			return std::hash<std::size_t>{}(tupleHashImpl(thing, std::index_sequence_for<Args...>())*listOfPrimes[sizeof...(Args)]);
		};
	private:
		template<std::size_t ...i>
		std::size_t tupleHashImpl(const std::tuple<Args...>& thing, std::index_sequence<i...>)const {
			std::size_t reVal = 0x2345;
			//( ( reVal^=std::hash<std::decay_t<decltype(std::get<i>(thing))>>()(std::get<i>(thing)) ), ...);
			(void)std::initializer_list<int>{(reVal ^= i + listOfPrimes[i*i] * std::hash<std::decay_t<decltype(std::get<i>(thing))>>()(std::get<i>(thing)), 0)...};
			return reVal;
		}
	};
}
template<typename...Args>
using tupleHash = std::hash<std::tuple<Args...>>;


template<typename val,typename... Args>
class multiKeyMap {
public:
	using type = multiKeyMap<val, Args...>;// = type;
	using tupleType = std::tuple<Args...>;
	using tupleHash_ = std::hash<tupleType>;//why did i put a _
	using mapType = std::unordered_map<tupleType, val>;
	
	auto& begin() {
		return m_data.begin();
	}
	auto& end() {
		return m_data.end();
	}
	const auto& cbegin() const{
		return m_data.cbegin();
	}
	const auto& cend() const{
		return m_data.cend();
	}
	auto& find(Args... args) {
		return m_data.find(std::make_tuple(std::forward<Args>(args)...));
	}
	const auto& find(Args... args)const {
		return m_data.find(std::make_tuple(std::forward<Args>(args)...));
	}
	std::size_t bucket_count() const{
		return m_data.bucket_count();
	}
	std::size_t size() const{
		return m_data.size();
	}
	val& operator[](const std::tuple<Args...>& abc) {
		return m_data[abc];
	}
	val& operator[](const std::tuple<Args...>&& abc) {
	 	return m_data[std::forward<std::tuple<Args...>>(abc)];
	}
	void reserve(const std::size_t items) {
		m_data.reserve(items);
	}
	
	template<typename T>
	auto operator[](T t) {
		static_assert(std::is_convertible_v < std::decay_t<T>, std::decay_t<decltype(std::get<0>(tupleType{}))> >);
		return multiKeyMapIndexer<sizeof...(Args)-1, T>(this->m_data, std::forward_as_tuple(std::move(t)));
	}

	template<int i, typename ... Targs>
	class multiKeyMapIndexer {//i prolly need a better name for this ;-;
	public:
		multiKeyMapIndexer(mapType& m, std::tuple<Targs...>&& args) :mapy(m),m_stuff(std::move(args)) {};
		template<typename T>
		decltype(auto) operator[](T t) const{
			static_assert(std::is_convertible_v < std::decay_t<T>, std::decay_t<decltype(std::get<sizeof...(Args)-i>(tupleType{}))> >);
			if constexpr(i == 1)
				return mapy[std::tuple_cat(std::move(m_stuff), std::forward_as_tuple(std::move(t)))];
			else
				return multiKeyMapIndexer<i - 1, Targs..., T>(mapy, std::tuple_cat(std::move(m_stuff), std::forward_as_tuple(std::move(t))));
		}
	private:
		mapType& mapy;
		std::tuple<Targs...>&& m_stuff;
	};
private:
	friend class multiKeyMapIndexer<1, Args...>;
	std::unordered_map<tupleType, val> m_data;
};

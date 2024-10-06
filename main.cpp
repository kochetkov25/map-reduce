#include <iostream>
#include <future>
#include <list>

template<class It, typename Fun1, typename Fun2>
auto map_reduce(It p, It q, Fun1 f1, Fun2 f2, size_t threads) -> decltype(f2(f1(*p), f1(*p)))
{
	/*for f1 func results*/
	using ReturnRes = decltype(f1(*p));
	std::vector<ReturnRes> VecValues;
	auto lbd = [&VecValues, &f1](It p, It q) {
		while (p != q)
		{
			VecValues.push_back(f1(*p));
			p++;
		}
		return 0;
	};
	/*for threads*/
	using Futures = decltype(std::async(std::launch::async, lbd, p, q));
	std::vector<Futures> VecThrds;

	const auto dist = std::distance(p, q);
	auto del = dist / threads;

	for (size_t i = 0; i < threads; i++)
	{
		auto cpP = p;
		std::advance(cpP, i * del);
		auto cpQ = cpP;
		/*if last iteration -> add to thread all remainin elements*/
		if ((i + 1) == threads)
			del += dist - threads * del;
		std::advance(cpQ, del);
		/*starting threads*/
		VecThrds.push_back(std::async(std::launch::async, lbd, cpP, cpQ));
	}

	for (auto& it : VecThrds)
		it.get();
	auto ItBgn = VecValues.begin();
	auto ItEnd = VecValues.end();
	auto result = *ItBgn;
	while (++ItBgn != ItEnd)
	{
		result = f2(result, *ItBgn);
	}
	return result;
}

/*main func*/
int main()
{
	std::list<int> l = { 1,2,3,4,5,6,7,8,9,10 };
	/*sum in 3 threads*/
	auto sum = map_reduce(
								l.begin(), l.end(),
								[](int i) {return i; },
								std::plus<int>(), 3
						 );

	std::cout << sum << std::endl;


	return 0;
}
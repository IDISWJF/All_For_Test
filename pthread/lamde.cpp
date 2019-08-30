#include<iostream>
#include<vector>
#include<string>
int main()
{
	int b = 1;

	std::string str;
	str = "abc";
	std::cout<<b<<std::endl;
	auto j =  [&](int b ){
		std::cout<<b<<std::endl;
		b++;
	};
	j(b);
	std::cout<<b<<std::endl;
	return 0;
}

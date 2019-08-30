#include<iostream>
#include<atomic>
#include<vector>
#include<thread>

std::atomic<int> i;
const int n = 10;
const int count = 1000;
void add()
{
    for(int j = 0;j < count;j++)
      i++;
}
int main()
{
    i.store(0);
    std::vector<std::thread> v;
    for(int j = 0;j < 10;j++)
    {
        v.push_back(std::move(std::thread(add)));
    }

    for(auto & w: v)
    {w.join();}

    if(i == n * count)
    std::cout<<"ok : "<<i<<std::endl; 
    else
    std::cout<<"failed : "<<i<<std::endl; 
    return 0;
}

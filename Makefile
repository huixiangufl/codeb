client: codeB.cpp helper.cpp client.m.cpp
	g++-4.8 -std=c++11 codeB.cpp helper.cpp client.m.cpp -o client -g

clean:
	rm client

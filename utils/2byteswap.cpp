#include <iostream>
#include <fstream>

using namespace std;

int main(int argc,char **argv)
{
	unsigned long curpos = 0;
	unsigned long length;

	if (argc != 3) {
		cout<<"Parameter counts not right\n";
		return -1;
	}

	ifstream infile(argv[1],ios::in | ios::binary); 
	ofstream outfile(argv[2],ios::out | ios::binary);

	if (!infile)
		cout<<"error open file\n";

	infile.seekg(0,ios::end);
	length = infile.tellg();
	infile.seekg(0,ios::beg);


	while (curpos < length) {	
		char inbuf[3];
		char outbuf[3];

		infile.seekg(curpos);
		infile.read (inbuf, 2);

		outbuf[0] = inbuf[1];
		outbuf[1] = inbuf[0];

		outfile.seekp(curpos);
		outfile.write(outbuf, 2);

		curpos += 2;
	}

	infile.close();
	outfile.close();
	return 0;
}

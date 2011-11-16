#include <iostream>
#include <fstream>

using namespace std;

int main(int argc,char **argv)
{
	unsigned long length;
	unsigned long curpos = 0;

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
		char inbuf[5];
		char outbuf[5];

		infile.seekg(curpos);
		infile.read (inbuf, 4);

		outbuf[0] = inbuf[3];
		outbuf[1] = inbuf[2];
		outbuf[2] = inbuf[1];
		outbuf[3] = inbuf[0];

		outfile.seekp(curpos);
		outfile.write(outbuf, 4);
		curpos += 4;
	}

	infile.close();
	outfile.close();
	return 0;
}

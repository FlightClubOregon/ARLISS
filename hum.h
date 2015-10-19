#ifndef HUM_H
#define HUM_H

using namespace std;


class Hum
{
	private:
		char humName[101];
	public:
		Hum();
		Hum(char humTempName[101]);
		~Hum();
		void humRead();

};
#endif

#include <avr/io.h>
#include "seg7.h"

int main()
{
	unsigned short data = 205;
	Seg7Init();

	while(1){
		Seg7DispNum(data,10);
	}
}
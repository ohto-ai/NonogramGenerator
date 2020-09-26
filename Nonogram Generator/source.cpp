#include "NonogramGenerator.h"

int main()
{
	NonogramGenerator generator;
	IMAGE image;
	loadimage(&image, "sans.png");
	generator.import(image, 50, 50);
	generator.generateTabel();
	generator.generateImage();
	saveimage("table.bmp", const_cast<IMAGE*>(&generator.table()));
	saveimage("ans.bmp", const_cast<IMAGE*>(&generator.ans()));
	generator.generateImage(32);
	saveimage("table2.bmp", const_cast<IMAGE*>(&generator.table()));
	saveimage("ans2.bmp", const_cast<IMAGE*>(&generator.ans()));
	return 0;
}
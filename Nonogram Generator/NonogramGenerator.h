#pragma once
#include <easyx.h>
#include <vector>
#include <string>
#include <algorithm>

enum NonogramType
{
	Original,	// 传统黑白版
	/*Colorful	// 彩色变种*/
};
class NonogramGenerator
{
public:
	void import(const IMAGE&, int, int);	// 导入图像
	void generateTabel(int threshold = 128);
	void generateImage(int zoomScale = 16);

	const IMAGE& ans() const;
	const IMAGE& table() const;

protected:
	NonogramType type()const;
	void setType(NonogramType);
protected:
	static void nearestZoom(const IMAGE&, IMAGE&, int, int);
	static void binarize(const IMAGE&, IMAGE&, int);
	static void inverse(const IMAGE&, IMAGE&);

	int width;
	int height;
	IMAGE _image;
	IMAGE _ans;
	IMAGE _table;
	NonogramType _type{ NonogramType::Original };
	std::vector<std::vector<int>> horizontalCount;
	std::vector<std::vector<int>> verticalCount;
};


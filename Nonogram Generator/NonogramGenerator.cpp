#include "NonogramGenerator.h"

void NonogramGenerator::import(const IMAGE&srcImage, int width, int height)
{
	nearestZoom(srcImage, _image, width, height);
}

void NonogramGenerator::generateTabel(int threshold)
{
	binarize(_image, _image, threshold);

	const auto dstImageBuff = GetImageBuffer(&_image);
	width = _image.getwidth();
	height = _image.getheight();
	horizontalCount.resize(width);
	verticalCount.resize(height);

	for (auto& count : horizontalCount)
		count.clear();
	for (auto& count : verticalCount)
		count.clear();

	for (int j = 0; j < height; j++)
	{
		auto dstLinePtr = dstImageBuff + j * width;
		int count = 0;
		for (int i = 0; i < width; i++)
		{
			if (GetRValue(dstLinePtr[i]) < threshold)
			{
				if (count > 0)
				{
					verticalCount[j].push_back(count);
					count = 0;
				}
			}
			else
			{
				++count;
			}
		}
	}
	for (int i = 0; i < width; i++)
	{
		int count = 0;
		for (int j = 0; j < height; j++)
		{
			if (GetRValue(dstImageBuff[j * width + i]) < threshold)
			{
				if (count > 0)
				{
					horizontalCount[i].push_back(count);
					count = 0;
				}
			}
			else
			{
				++count;
			}
		}
	}
}

void NonogramGenerator::generateImage(int zoomScale)
{
	auto getMaxCount = [](const auto& vecs)
	{
		int count = 0;
		for (auto& vec : vecs)
			if (count < vec.size())
				count = vec.size();
		return count;
	};
	auto dataWidth = getMaxCount(verticalCount);
	auto dataHeight = getMaxCount(horizontalCount);
	IMAGE tableImage((dataWidth + width) * zoomScale, (dataHeight + height) * zoomScale);
	IMAGE zoomedImage;
	nearestZoom(_image, zoomedImage, width * zoomScale, width * zoomScale);
	inverse(zoomedImage, zoomedImage);
	auto currentWorkdevice = GetWorkingImage();

	SetWorkingImage(&tableImage);
	setlinecolor(RGB(33,33,33));
	setlinestyle(PS_SOLID, 1);
	setbkcolor(WHITE);
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	settextstyle(zoomScale, 0, "Arial");
	cleardevice();

	for (int index = 0; index < verticalCount.size(); index++)
	{
		int y = (index + dataHeight) * zoomScale;
		for (int subIndex = verticalCount[index].size() - 1; subIndex >= 0; subIndex--)
		{
			int x = (dataWidth - verticalCount[index].size() + subIndex) * zoomScale;
			line(x, y, x, y + zoomScale);
			RECT rect{ x, y, x + zoomScale, y + zoomScale };
			drawtext(std::to_string(verticalCount[index][subIndex]).c_str(), &rect, DT_CENTER| DT_VCENTER| DT_SINGLELINE);
		}
	}
	for (int index = 0; index < horizontalCount.size(); index++)
	{
		int x = (index + dataWidth) * zoomScale;
		for (int subIndex = horizontalCount[index].size() - 1; subIndex >= 0; subIndex--)
		{
			int y = (dataHeight - horizontalCount[index].size() + subIndex) * zoomScale;
			line(x, y, x + zoomScale, y);
			RECT rect{ x, y, x + zoomScale, y + zoomScale };
			drawtext(std::to_string(horizontalCount[index][subIndex]).c_str(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}


	for (int i = 0; i < width; ++i)
		line((i + dataWidth) * zoomScale, 0, (i + dataWidth) * zoomScale, getheight() - 1);
	for (int i = 0; i < height; ++i)
		line(0, (i + dataHeight) * zoomScale, getwidth(), (i + dataHeight) * zoomScale);
	setlinestyle(PS_SOLID, 3);
	for (int i = 0; i < width; i += 5)
		line((i + dataWidth) * zoomScale, 0, (i + dataWidth) * zoomScale, getheight() - 1);
	for (int i = 0; i < height; i += 5)
		line(0, (i + dataHeight) * zoomScale, getwidth(), (i + dataHeight) * zoomScale);
	line(0, 0, 0, getheight() - 1);
	line(0, 0, getwidth() - 1, 0);
	line(getwidth() - 1, 0, getwidth() - 1, getheight() - 1);
	line(0, getheight() - 1, getwidth() - 1, getheight() - 1);
	_table = tableImage;

	
	putimage(dataWidth * zoomScale, dataHeight * zoomScale, &zoomedImage);
	setlinestyle(PS_SOLID, 1);
	for (int i = 0; i < width; ++i)
		line((i + dataWidth) * zoomScale, 0, (i + dataWidth) * zoomScale, getheight() - 1);
	for (int i = 0; i < height; ++i)
		line(0, (i + dataHeight) * zoomScale, getwidth(), (i + dataHeight) * zoomScale);
	setlinestyle(PS_SOLID, 3);
	for (int i = 0; i < width; i += 5)
		line((i + dataWidth) * zoomScale, 0, (i + dataWidth) * zoomScale, getheight() - 1);
	for (int i = 0; i < height; i += 5)
		line(0, (i + dataHeight) * zoomScale, getwidth(), (i + dataHeight) * zoomScale);
	line(0, 0, 0, getheight() - 1);
	line(0, 0, getwidth() - 1, 0);
	line(getwidth() - 1, 0, getwidth() - 1, getheight() - 1);
	line(0, getheight() - 1, getwidth() - 1, getheight() - 1);
	_ans = tableImage;
	SetWorkingImage(currentWorkdevice);
}

const IMAGE& NonogramGenerator::ans() const
{
	return _ans;
}

const IMAGE& NonogramGenerator::table() const
{
	return _table;
}

NonogramType NonogramGenerator::type() const
{
	return _type;
}

void NonogramGenerator::setType(NonogramType type)
{
	_type = type;
}

void NonogramGenerator::binarize(const IMAGE& srcImage, IMAGE& dstImage, int threshold)
{
	dstImage = srcImage;

	auto dstImageBuff = GetImageBuffer(&dstImage);
	auto dataLength = dstImage.getwidth() * dstImage.getheight();
	for (auto index = 0; index < dataLength; ++index)
	{
		int gray = GetRValue(RGBtoGRAY(dstImageBuff[index]));
		dstImageBuff[index] = gray < threshold ? BLACK : WHITE;
	}
}

void NonogramGenerator::inverse(const IMAGE& srcImage, IMAGE& dstImage)
{
	dstImage = srcImage;

	auto dstImageBuff = GetImageBuffer(&dstImage);
	auto dataLength = dstImage.getwidth() * dstImage.getheight();
	for (auto index = 0; index < dataLength; ++index)
		dstImageBuff[index] = ~dstImageBuff[index];
}

void NonogramGenerator::nearestZoom(const IMAGE& srcImage, IMAGE& dstImage, int width, int height)
{
	IMAGE tmpImage(width, height);

	auto dstImageBuff = GetImageBuffer(&tmpImage);
	const auto srcImageBuff = GetImageBuffer(const_cast<IMAGE*>(&srcImage));

	for (int j = 0; j < height; j++)
	{
		auto dstLinePtr = dstImageBuff + j * width;
		auto srcLinePtr = srcImageBuff + (j * srcImage.getheight() / height) * srcImage.getwidth();
		for (int i = 0; i < width; i++)
			dstLinePtr[i] = srcLinePtr[i * srcImage.getwidth() / width];
	}

	dstImage = tmpImage;
}

#include "Texture.h"

Texture::Texture(std::string name)
{
	name = "data/textures/" + name;

	// Load bmp
	byte header[54];
	uint dataPos;
	uint imageSize;
	byte* data;

	FILE* file = fopen(name.c_str(), "rb");
	assert(file);
	
	if (fread(header, 1, 54, file) != 54)
		assert(false);

	if (header[0] != 'B' || header[1] != 'M')
		assert(false);

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width_ = *(int*)&(header[0x12]);
	height_ = *(int*)&(header[0x16]);
	
	if (imageSize == 0)
		imageSize = width_ * height_ * 3;
	if (dataPos == 0)
		dataPos = 54;

	data = newp byte[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	data_ = newp uint[width_ * height_];
	for (int i = 0; i < width_ * height_; i++)
	{
		data_[i] = (255 << 24) | (data[i * 3] << 16) | (data[i * 3 + 1] << 8) | (data[i * 3 + 2]);
	}
}

Texture::Texture(int width, int height, int color /* = 0xffffffff */)
{
}

uint Texture::getTexelAt(glm::vec2 pos)
{
	// TODO: Wrapping
	pos.x *= width_;
	pos.y *= height_;

	int x = glm::clamp((int)pos.x, 0, width_ - 1);
	int y = glm::clamp((int)pos.y, 0, height_ -1);

	return data_[x + y * width_];
}
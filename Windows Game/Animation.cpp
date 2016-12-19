#include "Animation.h"

Animation::Animation(std::string file, float frameTime, sf::Vector2i spriteCount, bool loop)
{
	this->loop = loop;
	this->frameTime = frameTime;
	sf::Image image;
	image.loadFromFile(file);
	const sf::Uint8* pixels = image.getPixelsPtr();
	texture.loadFromFile(file);
	sprite.setTexture(texture);
	frameWidth = texture.getSize().x / spriteCount.x;
	frameHeight = texture.getSize().y / spriteCount.y;

	//für jeden einzelnen sprite wird hier die region erstellt
	for (int i = 0; i < spriteCount.x; i++)
	{
		for (int j = 0; j < spriteCount.y; j++)
		{
			Frame frame;
			int texWidth = texture.getSize().x, texHeight = texture.getSize().y;
			frame.textureRect = sf::IntRect{ i*static_cast<int>(frameWidth),j*static_cast<int>(frameHeight),static_cast<int>(frameWidth),static_cast<int>(frameHeight) };
			frame.region = CreateRectRgn(0, 0, frameWidth, frameHeight);
			//jeder durchsichtige pixel des sprites wird von der region entfernt
			for (unsigned int y = j*frameHeight; y < image.getSize().y - (spriteCount.y - (j + 1))*static_cast<int>(frameHeight); y++)
			{
				for (unsigned int x = i*frameWidth; x < image.getSize().x - (spriteCount.x - (i + 1))*static_cast<int>(frameWidth); x++)
				{
					if (pixels[y * image.getSize().x * 4 + x * 4 + 3] == 0)
					{
						HRGN hRegionPixel = CreateRectRgn(x - i*frameWidth, y - j*frameHeight, x + 1 - i*frameWidth, y + 1 - j*frameHeight);
						CombineRgn(frame.region, frame.region, hRegionPixel, RGN_XOR);
						DeleteObject(hRegionPixel);
					}
				}
			}
			frames.push_back(frame);
		}
	}
}

Animation::~Animation()
{
	for (auto &i : frames)
	{
		DeleteObject(i.region);
	}
}

void Animation::draw(sf::RenderWindow& window)
{
		window.setSize(sf::Vector2u{ frameWidth,frameHeight });
		sf::View view{ sf::FloatRect{0,0,static_cast<float>(frameWidth),static_cast<float>(frameHeight)} };
		window.setView(view);
		HRGN region = CreateRectRgn(0, 0, 1, 1);
		CombineRgn(region, frames[currentFrame].region, NULL, RGN_COPY);
		SetWindowRgn(window.getSystemHandle(), region, true);
		DeleteObject(region);
		sprite.setTextureRect(frames[currentFrame].textureRect);
		window.draw(sprite);
}

void Animation::update(float deltaTime)
{
	if (!done)
	{
		//animate the sprite
		timeSinceLastUpdate += deltaTime;
		if (timeSinceLastUpdate > frameTime)
		{
			if (currentFrame < frames.size() - 1)
				currentFrame++;
			else if (loop)
			{
				currentFrame = 0;
			}
			else
				done = true;
			timeSinceLastUpdate = 0.f;
		}
	}
}

void Animation::reset()
{
	currentFrame = 0;
	timeSinceLastUpdate = 0;
	done = false;
}

sf::Vector2i Animation::getSize()
{
	return sf::Vector2i{ static_cast<int>(frameWidth),static_cast<int>(frameHeight) };
}
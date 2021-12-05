#include "TextureManager.h"

unordered_map<string, sf::Texture> TextureManager::textures;

void TextureManager::LoadTexture(string fileName)
{
	string path = "src/images/";
	path += fileName + ".png";
	textures[fileName].loadFromFile(path);
}

sf::Texture& TextureManager::getTexture(string textureName)
{
	if (textures.find(textureName) == textures.end())
	{
		LoadTexture(textureName);
	}
	return textures[textureName];
}

void TextureManager::Clear()
{
	textures.clear();
}
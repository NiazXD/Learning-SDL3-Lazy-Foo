#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<string>

class LTexture {
public:

	static constexpr float kOriginalSize = -1.f;

	LTexture();

	~LTexture();

	bool loadFromFile(std::string path);

	void destroy();

	void setColor(Uint8 r, Uint8 g, Uint8 b);

	void setAlpha(Uint8 alpha);

	void setBlending(SDL_BlendMode blendMode);

	void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize,
		double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();
	bool isLoaded();

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};


constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };

SDL_Window* gWindow{ nullptr };
SDL_Renderer* gRenderer{ nullptr };

LTexture::LTexture() :
	mTexture{ nullptr },
	mWidth{ 0 },
	mHeight{ 0 }
{

}

LTexture::~LTexture() {
	destroy();
}

bool LTexture::loadFromFile(std::string path) {
	destroy();

	if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr) {
		SDL_Log("Unable to load Image! SDL Error: %s\n", SDL_GetError());
	}
	else {
			if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
				SDL_Log("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
			}
			else {
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

		SDL_DestroySurface(loadedSurface);
	}

	return mTexture != nullptr;
}

void LTexture::destroy() {
	SDL_DestroyTexture(mTexture);
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

void LTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
	SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::setBlending(SDL_BlendMode blendMode) {
	SDL_SetTextureBlendMode(mTexture, blendMode);
}

void LTexture::render(float x, float y, SDL_FRect* clip, float width, float height, double degrees, SDL_FPoint* center, SDL_FlipMode flipMode) {
	SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

	if (clip != nullptr) {
		dstRect.w = clip->w;
		dstRect.h = clip->h;
	}

	if (width > 0) {
		dstRect.w = width;
	}
	if (height > 0) {
		dstRect.h = height;
	}

	SDL_RenderTextureRotated(gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

bool LTexture::isLoaded() {
	return mTexture != nullptr;
}

LTexture gColorsTexture;

constexpr int kColorMagnitudeCount = 3;
constexpr Uint8 kColorMagnitudes[kColorMagnitudeCount] = { 0x00, 0x7f, 0xFF }; //0, half or full
enum class eColorChannel {
	TextureRed = 0,
	TextureGreen = 1,
	TextureBlue = 2,
	TextureAlpha = 3,

	BackgroundRed = 4,
	BackgroundGreen = 5,
	BackgroundBlue = 6,

	Total = 7,
	Unknown = 8
};


bool init();
bool loadMedia();
void clear();


bool init() {
	bool success{ true };

	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		SDL_Log("SDL could not be iniitalized! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		if (SDL_CreateWindowAndRenderer("SDL4 Tutorial: Key Presses and Key States", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer) == false) {
			SDL_Log("Window could not be created!SDL Error: %s\n", SDL_GetError());
			success = false;
		}
	}

	return success;
}

bool loadMedia() {
	bool success{ true };

	if (gColorsTexture.loadFromFile("07-color-modulation-and-alpha-blending/colors.png") == false) {
		SDL_Log("Unable to load image!\n");
		success = false;
	}

	return success;
}

void close() {
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	SDL_Quit();
}



int main(int argc, char* args[]) {
	int exitCode{ 0 };

	if (init() == false) {
		SDL_Log("Initialization failed\n");
		exitCode = 1;
	}
	else {
		if (loadMedia() == false) {
			SDL_Log("Unable to load media\n");
			exitCode = 2;
		}
		else {
			bool quit{ false };
			SDL_Event e;
			SDL_zero(e);

			Uint8 colorChannelsIndices[static_cast<int>(eColorChannel::Total)];
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureRed)] = 2;
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureGreen)] = 2;
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureBlue)] = 2;
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureAlpha)] = 2;

			colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundRed)] = 2;
			colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundGreen)] = 2;
			colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundBlue)] = 2;

			gColorsTexture.setBlending(SDL_BLENDMODE_BLEND);

			double degrees = 0.0;

			SDL_FlipMode flipMode = SDL_FLIP_NONE;

			while (quit == false) {
				while (SDL_PollEvent(&e) == true) {
					if (e.type == SDL_EVENT_QUIT) {
						quit = true;
					}
					else if (e.type == SDL_EVENT_KEY_DOWN) {
						eColorChannel channelToUpdate = eColorChannel::Unknown;
						switch (e.key.key) 
						{
						case SDLK_A:
							channelToUpdate = eColorChannel::TextureRed;
							break;
						case SDLK_S:
							channelToUpdate = eColorChannel::TextureGreen;
							break;
						case SDLK_D:
							channelToUpdate = eColorChannel::TextureBlue;
							break;
						case SDLK_F:
							channelToUpdate = eColorChannel::TextureAlpha;
							break;

						case SDLK_Q:
							channelToUpdate = eColorChannel::BackgroundRed;
							break;
						case SDLK_W:
							channelToUpdate = eColorChannel::BackgroundGreen;
							break;
						case SDLK_E:
							channelToUpdate = eColorChannel::BackgroundBlue;
							break;
						  
						}

						if (channelToUpdate != eColorChannel::Unknown) {
							colorChannelsIndices[static_cast<int>(channelToUpdate)]++;
							if (colorChannelsIndices[static_cast<int>(channelToUpdate)] >= kColorMagnitudeCount) {
								colorChannelsIndices[static_cast<int>(channelToUpdate)] = 0;
							}

							SDL_Log("Texture - R:%d G:%d B:%d A:%d | Background - R:%d G:%d B:%d",
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureRed)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureGreen)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureBlue)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureAlpha)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundRed)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundGreen)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundBlue)]]
							);

						}

					}

					SDL_SetRenderDrawColor(gRenderer,
						kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundRed)]],
						kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundGreen)]],
						kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundBlue)]],
						0xFF);
					SDL_RenderClear(gRenderer);

					gColorsTexture.setColor(
						kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureRed)]],
						kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureGreen)]],
						kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureBlue)]]
					);
					gColorsTexture.setAlpha(kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureAlpha)]]);
					gColorsTexture.render((kScreenWidth - gColorsTexture.getWidth()) / 2.f, (kScreenHeight - gColorsTexture.getHeight()) / 2.f);


					SDL_RenderPresent(gRenderer);

				}
			}
		}
	}


	close();

	return exitCode;

}
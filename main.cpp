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

	void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize);

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
		if (SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0x00, 0xFF, 0xFF)) == false) {
			SDL_Log("Unable to color key! SDL error: %s\n", SDL_GetError());
		}
		else {
			if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
				SDL_Log("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
			}
			else {
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

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

void LTexture::render(float x, float y, SDL_FRect* clip, float width, float height) {
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

	SDL_RenderTexture(gRenderer, mTexture, clip, &dstRect);
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

LTexture gSpriteSheetTexture;

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

	if (gSpriteSheetTexture.loadFromFile("05-sprite-clipping-and-stretching/dots.png") == false) {
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
			SDL_zero(e);;

			while (quit == false) {
				while (SDL_PollEvent(&e) == true) {
					if (e.type == SDL_EVENT_QUIT) {
						quit = true;
					}


					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					constexpr float kSpriteSize = 100.f;
					SDL_FRect spriteClip{ 0.f, 0.f, kSpriteSize, kSpriteSize };
					SDL_FRect spriteSize{ 0.f, 0.f, kSpriteSize, kSpriteSize };

					spriteClip.x = 0.f;
					spriteClip.y = 0.f;

					spriteSize.w = kSpriteSize;
					spriteSize.h = kSpriteSize;

					gSpriteSheetTexture.render(0.f, 0.f, &spriteClip, spriteSize.w, spriteSize.h);

					spriteClip.x = kSpriteSize;
					spriteClip.y = 0.f;
					
					spriteSize.w = kSpriteSize * 0.5f;
					spriteSize.h = kSpriteSize * 0.5f;

					gSpriteSheetTexture.render(kScreenWidth - spriteSize.w, 0.f, &spriteClip, spriteSize.w, spriteSize.h);

					spriteClip.x = 0.f;
					spriteClip.y = kSpriteSize;

					spriteSize.w = kSpriteSize * 2.f;
					spriteSize.h = kSpriteSize * 2.f;

					gSpriteSheetTexture.render(0.f, kScreenHeight - spriteSize.h, &spriteClip, spriteSize.w, spriteSize.h);

					spriteClip.x = kSpriteSize;
					spriteClip.y = kSpriteSize;

					spriteSize.w = kSpriteSize * 2.f;
					spriteSize.h = kSpriteSize * 2.f;

					gSpriteSheetTexture.render(kScreenWidth - spriteSize.w, kScreenHeight - spriteSize.h, &spriteClip, spriteSize.w, spriteSize.h);

					SDL_RenderPresent(gRenderer);

				}
			}
		}
	}


	close();

	return exitCode;

}
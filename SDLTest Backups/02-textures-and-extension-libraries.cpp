#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<string>


constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };

SDL_Window* gWindow{ nullptr };
SDL_Renderer* gRenderer{ nullptr };


class LTexture
{
public: 

	LTexture() :
		mTexture{ nullptr },
		mWidth{ 0 },
		mHeight{ 0 }
	{

	}

	~LTexture() {
		destroy();
	}

	bool loadFromFile(std::string path) {
		destroy();

		if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr) {
			SDL_Log("Unable to create texture from loaded path! SDL Error: %s", SDL_GetError());
		}
		else {
			if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
				SDL_Log("Unable to create Texture from Loaded Pixel! SDL Error: %s", SDL_GetError());
			}
			else {
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

			SDL_DestroySurface(loadedSurface);
		}


		return mTexture != nullptr;
	}

	void destroy() {
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}

	void render(float x, float y) {
		SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };
		SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
	}

	int getWidth() {
		return mWidth;
	}

	int getHeight() {
		return mHeight;
	}

	bool isLoaded() {
		return mTexture != nullptr;
	}

private:

	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;
};

LTexture gPngTexture;


bool init() {
	bool success{ true };

	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		SDL_Log("SDL could not be initialized! SDL Error: %s", SDL_GetError());
		success = false;
	}
	else {
		if (SDL_CreateWindowAndRenderer("SDL3 Tutorial: Textures and Extension Libraries", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer) == false) {
			SDL_Log("Window could not be created! SDL Error: %s", SDL_GetError());
			success = false;
		}
	}

	return success;
}

bool loadMedia() {
	bool success{ true };

	if (gPngTexture.loadFromFile("02-textures-and-extension-libraries/loaded.png") == false) {
		SDL_Log("Unable to load png image!\n");
		success = false;
	}

	return success;
}

void close() {
	gPngTexture.destroy();

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	SDL_Quit();
}


int main(int argc, char* args[]) {
	int exitCode{ 0 };

	if (init() == false) {
		SDL_Log("Unable to Initialize Program!\n");
		exitCode = 1;
	}
	else {

		if (loadMedia() == false) {
			SDL_Log("Unable to load media!\n");
			exitCode = 2;
		}
		else {
			
			bool quit{ false };

			SDL_Event e;
			SDL_zero(e);

			while (quit == false) {
				while (SDL_PollEvent(&e) == true) {
					if (e.type == SDL_EVENT_QUIT) {
						quit = true;
					}
				}

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFf, 0xFF);
				SDL_RenderClear(gRenderer);

				gPngTexture.render(0.f, 0.f);

				SDL_RenderPresent(gRenderer);

			}
		}
	}

	close();

	return exitCode;
}